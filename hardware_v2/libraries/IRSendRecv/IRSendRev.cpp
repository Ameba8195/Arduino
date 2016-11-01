#include "Arduino.h"
#include "IRSendRev.h"
#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "serial_api.h"
#include "serial_ex_api.h"
#include "gpio_irq_api.h"
#include "gpio_irq_ex_api.h"
#include "timer_api.h"

extern PinDescription g_APinDescription[];

#ifdef __cplusplus
}
#endif

#define GAP 5000

// receiver states
#define STATE_IDLE     2
#define STATE_MARK     3
#define STATE_SPACE    4
#define STATE_STOP     5

void gpio_ir_recv_handler(uint32_t id, uint32_t event) {

    uint32_t current_timestamp;
    IRSendRev *pIR;

    pIR = (IRSendRev *)id;

    gpio_irq_disable((gpio_irq_t *)(pIR->pGpioIrqRecv));

    current_timestamp = micros();

    switch(pIR->rcvstate) {
        case STATE_IDLE:
            if ((pIR->recv_wait_int) == IRQ_LOW) {
                pIR->rcvstate = STATE_MARK;
                pIR->prev_timestamp = current_timestamp;
            }
            break;
        case STATE_MARK:
            if ((pIR->recv_wait_int) == IRQ_HIGH) {
                // mark transition to space
                pIR->recv_buf[(pIR->recv_len)++] = current_timestamp - (pIR->prev_timestamp);
                pIR->prev_timestamp = current_timestamp;
                pIR->rcvstate = STATE_SPACE;
                gtimer_reload((gtimer_t *)(pIR->pTimerRecv), GAP);
                gtimer_start((gtimer_t *)(pIR->pTimerRecv));                
            }
            break;
        case STATE_SPACE:
            if ((pIR->recv_wait_int) == IRQ_LOW) {
                // space transition to mark
                gtimer_stop((gtimer_t *)(pIR->pTimerRecv));
                pIR->recv_buf[(pIR->recv_len)++] = current_timestamp - (pIR->prev_timestamp);
                pIR->prev_timestamp = current_timestamp;
                pIR->rcvstate = STATE_MARK;
            }
            break;
        case STATE_STOP:
            break;
    }

    if ((pIR->recv_wait_int) == IRQ_LOW )
    {
        // Change to listen to high level event
        (pIR->recv_wait_int) = IRQ_HIGH;
        gpio_irq_set((gpio_irq_t *)(pIR->pGpioIrqRecv), (gpio_irq_event)IRQ_HIGH, 1);
        gpio_irq_enable((gpio_irq_t *)(pIR->pGpioIrqRecv));
    }
    else if ((pIR->recv_wait_int) == IRQ_HIGH)
    {
        // Change to listen to low level event
        (pIR->recv_wait_int) = IRQ_LOW;
        gpio_irq_set((gpio_irq_t *)(pIR->pGpioIrqRecv), (gpio_irq_event)IRQ_LOW, 1);
        gpio_irq_enable((gpio_irq_t *)(pIR->pGpioIrqRecv));
    }

}

void ir_recv_timer_handler(uint32_t id) {

    IRSendRev *pIR;

    pIR = (IRSendRev *)id;

    if (pIR->rcvstate == STATE_SPACE && (pIR->recv_wait_int) == IRQ_LOW) {
        pIR->recv_buf[(pIR->recv_len)++] = micros() - (pIR->prev_timestamp);
    }

    pIR->rcvstate = STATE_STOP;
}

IRSendRev::IRSendRev(uint8_t receivePin, uint8_t transmitPin) {
    this->receivePin = receivePin;
    this->transmitPin = transmitPin;

    pUART = malloc( sizeof(serial_t) );

    serial_init((serial_t *)pUART, (PinName)(g_APinDescription[transmitPin].pinname), (PinName)(g_APinDescription[receivePin].pinname));
    serial_baud((serial_t *)pUART, 38*1000*2);
    serial_format((serial_t *)pUART, 8, ParityNone, 1);

    // initialize receiving
    pGpioIrqRecv = malloc( sizeof(gpio_irq_t) );
    pTimerRecv = malloc( sizeof(gtimer_t) );
}

IRSendRev::~IRSendRev() {
    if (pGpioIrqRecv != NULL) {
        free(pGpioIrqRecv);
        pGpioIrqRecv = NULL;
    }
    if (pTimerRecv != NULL) {
        free(pTimerRecv);
        pTimerRecv = NULL;
    }
    if (pUART != NULL) {
        serial_free((serial_t *)pUART);
        free(pUART);
        pUART = NULL;
    }
}

void IRSendRev::Init(int revPin) {

    gpio_irq_init((gpio_irq_t *)pGpioIrqRecv, (PinName)(g_APinDescription[revPin].pinname), (gpio_irq_handler)gpio_ir_recv_handler, (uint32_t)this);
    gpio_irq_set((gpio_irq_t *)pGpioIrqRecv, (gpio_irq_event)IRQ_LOW, 1);
    gpio_irq_enable((gpio_irq_t *)pGpioIrqRecv);

    gtimer_init((gtimer_t *)pTimerRecv, TIMER4);
    gtimer_start_one_shout((gtimer_t *)pTimerRecv, GAP, (void *)ir_recv_timer_handler, (uint32_t)this);
    gtimer_stop((gtimer_t *)pTimerRecv);

    Clear();
}

void IRSendRev::Init() {
    Clear();
}

void IRSendRev::Clear() {
    recv_len = 0;
    memset(recv_buf, 0, RAWBUF);
    prev_timestamp = 0;
    recv_wait_int = IRQ_LOW;
    rcvstate = STATE_IDLE;
}

unsigned char IRSendRev::Recv(unsigned char *revData) {

    int i, j;
    uint32_t nshort = 0;
    uint32_t doubleshort = 0;
    uint32_t nlong = 0;
    uint32_t count_data = 0;

    /* 4 = 2 for start pulse, 2 for end pulse */
    count_data = (recv_len - 4) / 16;

    // calculate nshort
    for (i=0; i<10; i++) {
        nshort += recv_buf[2+2*i];
    }
    nshort /= 10; // nshort = 10 times average
    doubleshort = nshort*2;

    for (i=3, j=0; j<10 && i<recv_len-2; i+=2) {
        if(recv_buf[i] > doubleshort) {
            nlong += recv_buf[i];
            j++;
        }
    }
    nlong /= j; // nlong = expected 10 times average

    // fill in data    
    for (i=0; i<count_data; i++) {
        revData[i+D_DATA] = 0x00;
        for (j=0; j<8; j++) {
            if( recv_buf[3+16*i+j*2] > doubleshort) {
                revData[i+D_DATA] |=  (0x01 << (7-j));
            } else {
                revData[i+D_DATA] &= ~(0x01 << (7-j));
            }
        }
    }

    revData[D_LEN]     = count_data + 5;
    revData[D_STARTH]  = recv_buf[0] / 50;
    revData[D_STARTL]  = recv_buf[1] / 50;
    revData[D_SHORT]   = nshort / 50;
    revData[D_LONG]    = nlong / 50;
    revData[D_DATALEN] = count_data;

    Clear();
    return revData[D_LEN] + 1;
}

//if get some data from IR
unsigned char IRSendRev::IsDta() {

    if (rcvstate != STATE_STOP) {
        return 0;
    }

    if (recv_len < 20 || (recv_len - 2) % 8 != 2) {
        Clear();
        return 0;
    }

    return ((recv_len-2) / 16) + 6;
}

void IRSendRev::Send(unsigned char *idata, unsigned char ifreq)
{
    int i, j;

    int len = idata[0];
    unsigned char c;
    unsigned char start_high    = idata[1];
    unsigned char start_low     = idata[2];
    unsigned char nshort        = idata[3];
    unsigned char nlong         = idata[4];
    unsigned char datalen       = idata[5];

    uint32_t bytecount;
    unsigned char *pClockData;

    bytecount = start_high * ifreq / 100;
    pClockData = (unsigned char *) malloc (bytecount);
    for (i=0; i<bytecount; i++) {
        pClockData[i] = 0x55;
    }
    serial_baud((serial_t *)pUART, ifreq*1000*2);

    serial_send_blocked((serial_t *)pUART, (char *)pClockData, bytecount, 10);

    HalDelayUs(start_low * 50);

    bytecount = nshort * ifreq / 100;
    for (i=0; i<datalen; i++) {
        c = idata[i+6];
        for (j=0; j<8; j++) {
            serial_send_blocked((serial_t *)pUART, (char *)pClockData, bytecount, 1);
            if (c & (0x01 << (7-j))) {
                HalDelayUs(nlong * 50);
            } else {
                HalDelayUs(nshort * 50);
            }
        }
    }
    serial_send_blocked((serial_t *)pUART, (char *)pClockData, bytecount, 1);

    free(pClockData);
}

#if defined(BOARD_RTL8195A)
IRSendRev IR(0, 1);
#elif defined(BOARD_RTL8710)
IRSendRev IR(17, 5);
#else
IRSendRev IR(0, 1);
#endif
