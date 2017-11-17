#include "Arduino.h"
#include "Alexa.h"


#ifdef __cplusplus
extern "C" {
#endif


#include "wifi_conf.h"
#include "wifi_constants.h"
#include "wifi_structures.h"
#include "lwip_netconf.h"
#include "lwip/err.h"
#include "lwip/api.h"
#include <dhcp/dhcps.h>

#include "alexa/alexa.h"
#include "alexa/alexa_audio.h"
#include "alexa/alexa_timeline.h"

/** The AVS alarm ring tone resource */
#include "alexa/res/alexa_mp3_alarm.c"



#include "device.h"
#include "gpio_api.h"
#include "gpio_irq_api.h"
#include "gpio_irq_ex_api.h"
#include "timer_api.h"


#define YOUR_SSID "free_or_die"
#define YOUR_PW   "0928513843"
// Select your codec
#define ALEXA_CODEC_ALC5680_I2C (1)
#define ALEXA_CODEC_SGTL5000    (0)

#include "i2s_api.h"
#include "mp3/mp3_codec.h"
#if ALEXA_CODEC_ALC5680_I2C
#include "alc5680.h"
#endif

#if ALEXA_CODEC_SGTL5000
#include "sgtl5000.h"
#endif


#ifdef __cplusplus
}
#endif


#ifndef SDRAM_BSS_SECTION
#define SDRAM_BSS_SECTION                        \
        SECTION(".sdram.bss")
#endif


/**
 * The Alexa configuration data struct to init Alexa service
 */
static alexa_context_t alexa_context;

static i2s_t i2s_obj;

/**
 * The wakeup pin.
 *
 * When ALC5680 reconize the wakeup word it will toggle this pin
 */
#define GPIO_IRQ_VOICE_PIN        PC_5
static gpio_irq_t gpio_audio;

/**
 * The LED pin on ALC5680 to provide information.
 */
#define GPIO_LED_PIN              PE_5
#define CODEC_LED_ON  (0)
#define CODEC_LED_OFF (1)
int codec_led_state = CODEC_LED_OFF;
static gpio_t gpio_led;
static gtimer_t timer_led;

/**
 * The Alexa configuration data struct to init Alexa service
 */

#define I2S_SCLK_PIN            PC_1
#define I2S_WS_PIN              PC_0
#define I2S_SD_PIN              PC_2

/**
 * If we hit a i2x tx buf not available. It means all 4 buffer are used.
 * 1 frame takes 1s / 24KHz * 16 bit = 24 ms to consume.
 * So we wait 48ms to lower down the miss rate.
 */
#define ALEXA_AUDIO_I2S_TX_BUF_DELAY (48)

// The mp3 download from AVS has 1152 bytes decoded in each frame
#define ALEXA_I2S_DMA_PAGE_SIZE 1152
#define ALEXA_I2S_DMA_PAGE_NUM     4   // Vaild number is 2~4

// DMA buffer for I2S
SDRAM_BSS_SECTION static uint8_t i2s_tx_buf[ALEXA_I2S_DMA_PAGE_SIZE*ALEXA_I2S_DMA_PAGE_NUM];
SDRAM_BSS_SECTION static uint8_t i2s_rx_buf[ALEXA_I2S_DMA_PAGE_SIZE*ALEXA_I2S_DMA_PAGE_NUM];

/**
 * @defgroup alexa_avs_heap 
 *     AVS use a array as its thread stack instead of using malloc from OS heap.
 * @{
 */
TaskHandle_t alexa_audio_tx_thread_handle = NULL;
#define ALEXA_AUDIO_TX_THREAD_PRIORITY_NORMAL   ( tskIDLE_PRIORITY + 1 )
#define ALEXA_AUDIO_TX_THREAD_PRIORITY_CRITICAL ( tskIDLE_PRIORITY + 2 )

#define ALEXA_AUDIO_TX_HEAP_SIZE (1024)
SDRAM_BSS_SECTION static uint8_t audio_tx_heap[ALEXA_AUDIO_TX_HEAP_SIZE * sizeof( StackType_t )];

/** semaphore to check status */
static xSemaphoreHandle audio_tx_sema = NULL;


#define ALEXA_CONN_HEAP_SIZE (512)
SDRAM_BSS_SECTION static uint8_t audio_conn_heap[ALEXA_CONN_HEAP_SIZE * sizeof( StackType_t )];

/** Current on use channel. See group alexa_res */
static uint8_t priority_queue = ALEXA_AUDIO_CHANNEL_NONE;

// The audio tx data source: dialog
#define DIALOG_QUEUE_LENGTH (30)
static xQueueHandle dialog_queue;

// The audio tx data source: content
#define CONTENT_QUEUE_LENGTH (50)
static xQueueHandle content_queue;

/**
 * Cached mp3 data.
 *
 * When play mp3 data, the source data may not complete and need further data to decode.
 * We use this varaible to cache the un-decoded data
 */
#define DIALOG_DATA_CACHE_SIZE (16384+4096)
uint32_t dialog_data_len = 0;

SDRAM_BSS_SECTION uint8_t dialog_data_cache[DIALOG_DATA_CACHE_SIZE];

#define CONTENT_DATA_CACHE_SIZE (16384+4096)
uint32_t content_data_len = 0;

SDRAM_BSS_SECTION uint8_t content_data_cache[CONTENT_DATA_CACHE_SIZE];

uint32_t content_configured = 0;
uint16_t content_sample_rate = 0;
uint8_t content_channel_number = 0;
uint32_t content_delay_time = 24;

#define BUTTON_DEBOUNCE_TIMEOUT 1000 //1s
static int irqtickcount = 0;

/**
 * <!-- ++++++++++++++++ group alexa_res ++++++++++++++++ -->
 * @defgroup alexa_res alexa resource control
 * @{
 */

/** the current resource in use */
static uint32_t alexa_res_id = ALEXA_AUDIO_RES_NONE;

/** resource data pointer */
static unsigned char *res_data = NULL;

/** resource data length */
static uint32_t res_len = 0;

/** resource data index that we currently played position */
static uint32_t res_idx = 0;

/** 
 * @}
 * <!-- ---------------- group alexa_res ---------------- -->
 */

// the mp3 decoder obj
mp3_decoder_t alexa_mp3;

SDRAM_BSS_SECTION static uint8_t decode_buf[4608];

void gpio_audio_irq_handler (uint32_t id, gpio_irq_event event)
{	
	  int tickdiff = (xTaskGetTickCount() - irqtickcount);
	  
	  if(tickdiff < BUTTON_DEBOUNCE_TIMEOUT) 
	  	return;
	  else
	  	irqtickcount = xTaskGetTickCount();
	  	
    {
        printf("irq\r\n");
        if (alexa_avs_state() && alexa_avs_upload_state() == 0) {
            if (!alexa_audio_is_recording()) {
                alexa_audio_record_start(8000); // timeout 8s
            } else {
                alexa_audio_record_stop();
            }
        }
    }
}

void timer_led_handler(uint32_t id)
{
    if (alexa_avs_state()) {
        if (alexa_audio_is_recording()) {
            gpio_write(&gpio_led, CODEC_LED_ON);
        } else {
            gpio_write(&gpio_led, CODEC_LED_OFF);
        }
    } else {
        codec_led_state = (codec_led_state == CODEC_LED_OFF) ? CODEC_LED_ON : CODEC_LED_OFF;
        gpio_write(&gpio_led, codec_led_state);
    }

    if (wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS)
    {
        gtimer_reload(&timer_led, 300 * 1000);
    }
}

static void i2s_tx_complete(void *data, char *pbuf) { return ; }

static void i2s_rx_complete(void *data, char *prxbuf) {
#if (ALEXA_CODEC_ALC5680_I2C) || (ALEXA_CODEC_SGTL5000)
    if (alexa_audio_handle_rx((uint8_t*)prxbuf, ALEXA_I2S_DMA_PAGE_SIZE) != 0) {
        // something wrong?
    }
    i2s_recv_page(&i2s_obj);
#endif
}

static void init_alexa_i2s()
{
	i2s_obj.channel_num = CH_MONO;
	i2s_obj.sampling_rate = SR_16KHZ;
	i2s_obj.word_length = WL_16b;
	i2s_obj.direction = I2S_DIR_TXRX;
	i2s_init(&i2s_obj, I2S_SCLK_PIN, I2S_WS_PIN, I2S_SD_PIN);
	i2s_set_dma_buffer(&i2s_obj, (char*)i2s_tx_buf, (char*)i2s_rx_buf, ALEXA_I2S_DMA_PAGE_NUM, ALEXA_I2S_DMA_PAGE_SIZE);
	i2s_tx_irq_handler(&i2s_obj, (i2s_irq_handler)i2s_tx_complete, NULL);
	i2s_rx_irq_handler(&i2s_obj, (i2s_irq_handler)i2s_rx_complete, NULL);

    i2s_send_page(&i2s_obj, (uint32_t*)i2s_get_tx_page(&i2s_obj));
	i2s_recv_page(&i2s_obj);
}

/**
 * The Audio callback fucntion as AVS audio part is ready for initiialize.
 */
static void init_audio_codec()
{
#if ALEXA_CODEC_ALC5680_I2C
    init_alexa_i2s();
    alc5680_i2c_init();
    alc5680_get_version();
    // alc5680 volume range: 0~255
    set_alc5680_volume(125, 125);
    alexa_avs_set_volume(50);
#endif

#if ALEXA_CODEC_SGTL5000
	sgtl5000_enable();
    sgtl5000_setVolume(0.3);
    sgtl5000_inputSelect(SGTL5000_AUDIO_INPUT_MIC);
    sgtl5000_micGain(40);
    sgtl5000_unmuteHeadphone();
#endif
}

uint32_t audio_do_tx(alexa_audio_t *data, uint8_t channel, uint8_t push_head) {
    if (channel == ALEXA_AUDIO_CHANNEL_DIALOG) {
        if ( uxQueueSpacesAvailable( dialog_queue ) > 0 ) {
            if (push_head) {
                xQueueSendToFront( dialog_queue, ( void * ) data, ( TickType_t ) 0 );
            } else {
                xQueueSend( dialog_queue, ( void * ) data, ( TickType_t ) 0 );
            }
            return 0;
        } else {
            return 1;
        }
    } else if (channel == ALEXA_AUDIO_CHANNEL_CONTENT) {
        if ( uxQueueSpacesAvailable( content_queue ) > 0 ) {
            if (push_head) {
                xQueueSendToFront( content_queue, ( void * ) data, ( TickType_t ) 0 );
            } else {
                xQueueSend( content_queue, ( void * ) data, ( TickType_t ) 0 );
            }
            return 0;
        } else {
            return 1;
        }
    }
}

static void set_audio_codec_volume(uint32_t volume)
{
#if ALEXA_CODEC_ALC5680_I2C
    // alexa volume range: 0~100, alc5680 volume range: 0~255
    // we only use alc5680 range 0~250
    set_alc5680_volume( (volume / 2) * 5, (volume / 2) * 5 );
#endif
}

static void audio_change_channel(uint8_t channel, uint8_t enable)
{
    uint8_t priority_queue_old;

    priority_queue_old = priority_queue;
    priority_queue = (enable) ? (priority_queue | channel) : (priority_queue & ~channel);
    if (priority_queue_old != priority_queue) {
        xSemaphoreGive(audio_tx_sema);
    }
}

static void play_resource(uint32_t res_id)
{
    if ( res_id == ALEXA_AUDIO_RES_NONE ) {
        alexa_res_id = ALEXA_AUDIO_RES_NONE;
        audio_change_channel(ALEXA_AUDIO_CHANNEL_ALERTS, 0);
    } else if ( res_id == ALEXA_AUDIO_RES_ALARM ) {
        res_data = alexa_res_alarm_mp3;
        res_idx = 0;
        res_len = alexa_res_alarm_mp3_len;
        alexa_res_id = ALEXA_AUDIO_RES_ALARM;
        audio_change_channel(ALEXA_AUDIO_CHANNEL_ALERTS, 1);
    }
}

static uint32_t wifi_last_retry_timestamp = 0;

static void alexa_conn_thread(void *pvParameters)
{
    int ret;

    gpio_init(&gpio_led, GPIO_LED_PIN);
    gpio_dir(&gpio_led, PIN_OUTPUT);    // Direction: Output
    gpio_mode(&gpio_led, PullNone);     // No pull
    gpio_write(&gpio_led, codec_led_state);

    // Initial a periodical timer
    gtimer_init(&timer_led, TIMER0);
    gtimer_start_periodical(&timer_led, 300 * 1000, (void*)timer_led_handler, (uint32_t)&timer_led);

    while (1) {
        if (wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS) {
            if (wifi_last_retry_timestamp == 0 || xTaskGetTickCount() > wifi_last_retry_timestamp + 3000) {
                ret = wifi_connect( YOUR_SSID, RTW_SECURITY_WPA2_AES_PSK, YOUR_PW, strlen(YOUR_SSID), strlen(YOUR_PW), 0, NULL );
//                ret = wifi_connect( YOUR_SSID, RTW_SECURITY_OPEN, NULL, strlen(YOUR_SSID), 0, 0, NULL );
                if (ret == 0) {
                    LwIP_DHCP(0, DHCP_START);
                }
            }
        }
        vTaskDelay(500);
    }
}

static void do_dialog() {
    uint32_t idx;
    alexa_audio_t audio_data;
    int *ptx_buf;
    int frame_size;
    mp3_info_t info;

    do {
        if (xQueueReceive( dialog_queue, &audio_data, ALEXA_AUDIO_I2S_TX_BUF_DELAY ) != pdTRUE) {
            break;
        }

        if (audio_data.id <= 0) {
            break;
        }

        if (audio_data.flag & ALEXA_AUDIO_FLAG_START) {
            dialog_data_len = 0;
            if (audio_data.destructor != NULL) audio_data.destructor( &audio_data );
            vTaskDelay(1000); // Wait 1 seconds for buffering. (Do we really need this or we can fix it in another way?)
        }

        if (audio_data.flag & ALEXA_AUDIO_FLAG_PLAY) {
            // the mp3 sample rate is 24KHz, so we change it to 24KHz temporally
            i2s_obj.InitDat.I2SRate = SR_24KHZ;
            HalI2SSetRateRtl8195a(&(i2s_obj.InitDat));

            vTaskPrioritySet(alexa_audio_tx_thread_handle, ALEXA_AUDIO_TX_THREAD_PRIORITY_CRITICAL);

            if (dialog_data_len > 0) {
                memcpy( dialog_data_cache + dialog_data_len, audio_data.data, audio_data.data_len);
                dialog_data_len = dialog_data_len + audio_data.data_len;
                if (audio_data.destructor != NULL) audio_data.destructor( &audio_data );
                audio_data.data = dialog_data_cache;
                audio_data.data_len = dialog_data_len;
            }

            idx = 0;
            while (1) {
                while ((ptx_buf = i2s_get_tx_page(&i2s_obj)) == NULL) {
                    vTaskPrioritySet(alexa_audio_tx_thread_handle, ALEXA_AUDIO_TX_THREAD_PRIORITY_NORMAL);
                    vTaskDelay(ALEXA_AUDIO_I2S_TX_BUF_DELAY);
                }
                vTaskPrioritySet(alexa_audio_tx_thread_handle, ALEXA_AUDIO_TX_THREAD_PRIORITY_CRITICAL);

                frame_size = mp3_decode((mp3_decoder_t *)alexa_mp3, audio_data.data + idx, audio_data.data_len - idx, (signed short *)ptx_buf, &info);

                if (frame_size == 0) break;

                idx += frame_size;

                if (info.audio_bytes > 0) {
                    i2s_send_page(&i2s_obj, (uint32_t*)ptx_buf);
                } else {
                    // decode error
                    HalSerialPutcRtl8195a('@');
                }

                if (idx >= audio_data.data_len) break;
            }

            if (idx < audio_data.data_len) {
                dialog_data_len = audio_data.data_len - idx;
                if ( dialog_data_cache == audio_data.data) {
                    memmove( dialog_data_cache, dialog_data_cache + idx, dialog_data_len);
                } else {
                    _memcpy( dialog_data_cache, audio_data.data + idx, dialog_data_len);
                }
            } else {
                dialog_data_len = 0;
            }

            if (audio_data.destructor != NULL) audio_data.destructor( &audio_data );

            vTaskPrioritySet(alexa_audio_tx_thread_handle, ALEXA_AUDIO_TX_THREAD_PRIORITY_NORMAL);

            // AVS audio recording needs 16KHz, so we change it back to 16KHz
            i2s_obj.InitDat.I2SRate = SR_16KHZ;
            HalI2SSetRateRtl8195a(&(i2s_obj.InitDat));
        }

        if (audio_data.flag & ALEXA_AUDIO_FLAG_EOF) {
            alexa_audio_change_channel( ALEXA_AUDIO_CHANNEL_DIALOG, 0 );
            if (audio_data.destructor != NULL) audio_data.destructor( &audio_data );
            content_data_len = 0;
        }
    } while (0);
}

static void do_alert()
{
    int *ptx_buf;
    int frame_size;
    mp3_info_t info;

    do {
        if (alexa_timeline_get_dialog_timeout() > 0) {
            vTaskDelay(ALEXA_AUDIO_I2S_TX_BUF_DELAY);
            break;
        }

        if ((ptx_buf = i2s_get_tx_page(&i2s_obj)) == NULL) {
            // alerts has less priorty than dialog. We should break here to check if we have dialog came in
            vTaskDelay(ALEXA_AUDIO_I2S_TX_BUF_DELAY);
            break;
        }

        portENTER_CRITICAL();
        frame_size = mp3_decode((mp3_decoder_t *)&alexa_mp3, res_data + res_idx, ALEXA_I2S_DMA_PAGE_SIZE, (signed short *)ptx_buf, &info);

        if (frame_size == 0) {
            portEXIT_CRITICAL();
            break;
        }

        res_idx += frame_size;
        if (res_idx + frame_size > res_len) {
            res_idx = 0;
        }
        portEXIT_CRITICAL();

        if (info.audio_bytes > 0) {
            i2s_send_page(&i2s_obj, (uint32_t*)ptx_buf);
        }
    } while (0);
}

void set_sample_rate(uint16_t sample_rate) {
    if (sample_rate != 0) {
        uint16_t desired_sample_rate;
        if (sample_rate == 8000) {
            desired_sample_rate = SR_8KHZ;
        } else if (sample_rate == 16000) {
            desired_sample_rate = SR_16KHZ;
        } else if (sample_rate == 22050) {
            desired_sample_rate = SR_22p05KHZ;
        } else if (sample_rate == 24000) {
            desired_sample_rate = SR_24KHZ;
        } else if (sample_rate == 32000) {
            desired_sample_rate = SR_32KHZ;
        } else if (sample_rate == 44100) {
            desired_sample_rate = SR_44p1KHZ;
        } else if (sample_rate == 48000) {
            desired_sample_rate = SR_48KHZ;
        }
        if (desired_sample_rate != i2s_obj.InitDat.I2SRate) {
            i2s_obj.InitDat.I2SRate = desired_sample_rate;
            HalI2SSetRateRtl8195a(&(i2s_obj.InitDat));
        }
    }
}

uint16_t sample_rate_table[4][3] = {
    {11025, 12000, 8000}, // mpeg 2.5
    {0, 0, 0}, // reserved
    {22050, 24000, 16000}, // mpeg 2
    {44100, 48000, 32000}, // mpeg 1
};

void configure_content(uint8_t *data, uint32_t *plen, uint8_t data_type) {
    if (data_type == 0) {
        // mp3
        uint8_t has_header = 0;
        uint32_t idx = 0;
        for (idx = 0; idx < *plen-4; idx++) {
            // mp3 frame begin with 0xFFE
            if (data[idx] != 0xFF) continue;
            if ((data[idx+1] & 0xE0) != 0xE0) continue;

            // reserved MEPG Audio version is not supported
            if ((data[idx+1] & 0x18) == 0x08) continue;

            // only support layer 3
            if ((data[idx+1] & 0x06) != 0x02) continue;

            // bad bitrate description
            if ((data[idx+2] & 0xF0) == 0xF0) continue;

            // bad sample rate description
            if ((data[idx+2] & 0x0C) == 0x0C) continue;

            has_header = 1;
            break;
        }
        if (has_header) {
            printf("skip %d data\r\n", idx);
            memmove(data, data+idx, *plen - idx);
            *plen = *plen - idx;

            content_sample_rate = sample_rate_table[ (data[1] & 0x18) >> 3 ][ (data[2] & 0x0C) >> 2 ];
            content_channel_number = ((data[3] & 0xC0) >> 6) == 3 ? 1 : 2;
            content_delay_time = 2*576000/content_sample_rate;
            printf("SR:%d CH:%d\r\n", content_sample_rate, content_channel_number);

            set_sample_rate(content_sample_rate);

            content_configured = 1;
        }
    }
}

static void do_content() {
    int i;
    uint32_t idx;
    alexa_audio_t audio_data;
    int *ptx_buf;
    int frame_size;
    mp3_info_t info;

    do {
        if (xQueueReceive( content_queue, &audio_data, ALEXA_AUDIO_I2S_TX_BUF_DELAY ) != pdTRUE) {
            break;
        }

        //printf("Q:%d\r\n", CONTENT_QUEUE_LENGTH - uxQueueSpacesAvailable(content_queue));

        if (audio_data.flag & ALEXA_AUDIO_FLAG_START) {
            content_data_len = 0;
            if (audio_data.destructor != NULL) audio_data.destructor( &audio_data );
            audio_data.data = NULL;
            audio_data.data_len = 0;
            content_configured = 0;
            content_delay_time = 24;

            vTaskDelay(5000); // wait 5 seconds for buffering
        }

        if (audio_data.flag & ALEXA_AUDIO_FLAG_PLAY) {
            if (audio_data.flag & ALEXA_AUDIO_FLAG_TYPE_TS) {
                if (audio_data.destructor != NULL) audio_data.destructor( &audio_data );
                break;
            }

            if (content_configured == 0) {
                configure_content(audio_data.data, &(audio_data.data_len), 0);
            } else {
                set_sample_rate(content_sample_rate);
            }

            vTaskPrioritySet(alexa_audio_tx_thread_handle, ALEXA_AUDIO_TX_THREAD_PRIORITY_CRITICAL);

            if (content_data_len > 0) {
                memcpy( content_data_cache + content_data_len, audio_data.data, audio_data.data_len);
                content_data_len = content_data_len + audio_data.data_len;
                if (audio_data.destructor != NULL) audio_data.destructor( &audio_data );
                audio_data.data = content_data_cache;
                audio_data.data_len = content_data_len;
            }

            idx = 0;
            while (1) {
                while ((ptx_buf = i2s_get_tx_page(&i2s_obj)) == NULL) {
                    vTaskPrioritySet(alexa_audio_tx_thread_handle, ALEXA_AUDIO_TX_THREAD_PRIORITY_NORMAL);
                    vTaskDelay(content_delay_time);
                }
                vTaskPrioritySet(alexa_audio_tx_thread_handle, ALEXA_AUDIO_TX_THREAD_PRIORITY_CRITICAL);

                frame_size = mp3_decode((mp3_decoder_t *)&alexa_mp3, audio_data.data + idx, audio_data.data_len - idx, (signed short *)decode_buf, &info);

                if (frame_size == 0) break;

                idx += frame_size;

                if (info.audio_bytes > 0) {
                    if (info.channels != 1) {
                        for (i=0; i<info.audio_bytes/4; i++) {
                            ((int16_t *)decode_buf)[i] = ((int16_t *)decode_buf)[i*2];
                        }
                        info.channels = 1;
                        info.audio_bytes /= 2;
                    }
                    memcpy(ptx_buf, decode_buf, 1152);
                    i2s_send_page(&i2s_obj, (uint32_t*)ptx_buf);

                    if (info.audio_bytes == 1152 * 2) {
                        while ((ptx_buf = i2s_get_tx_page(&i2s_obj)) == NULL) {
                            vTaskDelay(content_delay_time);
                        }
                        memcpy(ptx_buf, decode_buf+1152, 1152);
                        i2s_send_page(&i2s_obj, (uint32_t*)ptx_buf);
                    }
                }

                if (idx >= audio_data.data_len) break;
            };

            if (idx < audio_data.data_len) {
                content_data_len = audio_data.data_len - idx;
                if ( content_data_cache == audio_data.data) {
                    memmove( content_data_cache, content_data_cache + idx, content_data_len);
                } else {
                    _memcpy( content_data_cache, audio_data.data + idx, content_data_len);
                }
            } else {
                content_data_len = 0;
            }

            if (audio_data.destructor != NULL) audio_data.destructor( &audio_data );

            vTaskPrioritySet(alexa_audio_tx_thread_handle, ALEXA_AUDIO_TX_THREAD_PRIORITY_NORMAL);

            // AVS audio recording needs 16KHz, so we change it back to 16KHz
        }

        if (audio_data.flag & ALEXA_AUDIO_FLAG_EOF) {
            i2s_obj.InitDat.I2SRate = SR_16KHZ;
            HalI2SSetRateRtl8195a(&(i2s_obj.InitDat));

            if ( CONTENT_QUEUE_LENGTH - uxQueueSpacesAvailable(content_queue) == 0 ) {
                alexa_audio_change_channel( ALEXA_AUDIO_CHANNEL_CONTENT, 0 );
            }

            if (audio_data.destructor != NULL) audio_data.destructor( &audio_data );
            audio_data.data = NULL;
            audio_data.data_len = 0;

            content_data_len = 0;
        }
    } while (0);
}


static void alexa_audio_tx_thread(void *pvParameters)
{
    alexa_mp3 = mp3_create();

    dialog_queue = xQueueCreate(DIALOG_QUEUE_LENGTH, sizeof(alexa_audio_t));
    content_queue = xQueueCreate(CONTENT_QUEUE_LENGTH, sizeof(alexa_audio_t));

    audio_tx_sema = xSemaphoreCreateCounting(0xffffffff, 0);

    while (1) {
        if(xSemaphoreTake(audio_tx_sema, 0xFFFFFFFF) != pdTRUE) {
            continue;
        }

        while (1) {
            if (priority_queue == 0) break;

            if (priority_queue & ALEXA_AUDIO_CHANNEL_DIALOG) {
                do_dialog();
            } else if (priority_queue & ALEXA_AUDIO_CHANNEL_ALERTS) {
                do_alert();
            } else if (priority_queue & ALEXA_AUDIO_CHANNEL_CONTENT) {
                do_content();
            }
        }
    }
}

AlexaClass::AlexaClass()
{
    memset(&alexa_context, 0, sizeof(alexa_context_t));






}

AlexaClass::~AlexaClass()
{
}

void AlexaClass::begin()
{

		// little delay for wait wifi driver ready
    vTaskDelay(1000);
		
		//os_thread_create(alexa_audio_tx_thread, NULL, OS_PRIORITY_REALTIME, ALEXA_AUDIO_TX_HEAP_SIZE);
	if ( xTaskGenericCreate( alexa_audio_tx_thread, "alexaATX", ALEXA_AUDIO_TX_HEAP_SIZE, ( void * ) NULL, tskIDLE_PRIORITY + 1, &alexa_audio_tx_thread_handle, (StackType_t *)audio_tx_heap, NULL) != pdPASS )
        printf("\n\r%s xTaskCreate(alexa_audio_tx_thread) failed", __FUNCTION__);
		//os_thread_create(alexa_conn_thread, NULL, OS_PRIORITY_REALTIME, ALEXA_CONN_HEAP_SIZE);
	if ( xTaskGenericCreate( alexa_conn_thread, "alexaConn", ALEXA_CONN_HEAP_SIZE, ( void * ) NULL, tskIDLE_PRIORITY + 3, NULL, (StackType_t *)audio_conn_heap, NULL) != pdPASS )
        printf("\n\r%s xTaskCreate(alexa_conn_thread) failed", __FUNCTION__);

    // 1. wait for wifi connected
    while (wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS) {
        vTaskDelay(1000);
    }

    wifi_disable_powersave();

    // 2. initialize audio service
    init_audio_codec();
    init_alexa_audio_service(
        audio_do_tx,
        audio_change_channel,
        set_audio_codec_volume,
        play_resource
    );
    
    init_alexa_avs_service(&alexa_context);
    
    
    // 6. setup and decide when to recording audio data
    gpio_irq_init(&gpio_audio, GPIO_IRQ_VOICE_PIN, gpio_audio_irq_handler, NULL);
    gpio_irq_set(&gpio_audio, IRQ_RISE, 1);
    gpio_irq_enable(&gpio_audio);

		vTaskDelete(NULL);
}
void AlexaClass::setAvsRefreshToken(char *avs_refresh_token, int len)
{
		alexa_context.refresh_token = (uint8_t *)avs_refresh_token;
		alexa_context.refresh_token_len = len;
}

void AlexaClass::setAvsClientId(char *avs_client_id, int len)
{
    alexa_context.avs_client_id = avs_client_id;
    alexa_context.avs_client_id_len = len;
}

void AlexaClass::setAvsClientSecret(char *avs_client_secret, int len)
{
    alexa_context.avs_client_secret = avs_client_secret;
    alexa_context.avs_client_secret_len = len;
}
void AlexaClass::setAvsHttp2Host(char *avs_http2_host, int len)
{
    alexa_context.avs_hp2_host = avs_http2_host;
    alexa_context.avs_hp2_host_len = len;
}




AlexaClass Alexa;
