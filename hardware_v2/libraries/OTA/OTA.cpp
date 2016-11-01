#include "Arduino.h"
#include "OTA.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <lwip/sockets.h>
#include "flash_api.h"
#include "sys_api.h"
#include "mDNS.h"

#ifdef __cplusplus
}
#endif

#define OTA_DEBUG
#ifdef OTA_DEBUG
  #define OTA_PRINTF(...) { printf(__VA_ARGS__); }
#else
  #define OTA_PRINTF(...) {}
#endif

#define BUFSIZE 512
#define IMAGE_2			0x0000B000

#define DEFAULT_IMAGE_DOWNLOAD_TIMEOUT 30000

OTAClass::OTAClass() {
    ota_addr = 0xFFFFFFFF;
    mdns_buf = NULL;
    mdns_service_id = NULL;
    txtRecord = NULL;
}

int OTAClass::beginArduinoMdnsService(char *device_name, uint16_t port) {

    int ret = -1;

    mdns_buf = (unsigned char *) malloc (DEFAULT_OTA_MDNS_BUF);
    txtRecord = malloc (sizeof(TXTRecordRef));

    do {
        if (mDNSResponderInit() != 0) {
            OTA_PRINTF("Fail to init mDNS service\r\n");
            break;
        }

        TXTRecordCreate((TXTRecordRef *)txtRecord, DEFAULT_OTA_MDNS_BUF, mdns_buf);

        TXTRecordSetValue((TXTRecordRef *)txtRecord, "board",       strlen("ameba_rtl8195a"), "ameba_rtl8195a");
        TXTRecordSetValue((TXTRecordRef *)txtRecord, "auth_upload", strlen("no"),             "no");
        TXTRecordSetValue((TXTRecordRef *)txtRecord, "tcp_check",   strlen("no"),             "no");
        TXTRecordSetValue((TXTRecordRef *)txtRecord, "ssh_upload",  strlen("no"),             "no");

        mdns_service_id = mDNSRegisterService(device_name, "_arduino._tcp", "local", port, (TXTRecordRef *)txtRecord);

        TXTRecordDeallocate((TXTRecordRef *)txtRecord);

        ret = 0;
    } while (0);

    if (ret < 0) {
        OTA_PRINTF("Fail to begin Arduino mDNS service\r\n");
    }

    return ret;
}

int OTAClass::endArduinoMdnsService() {
    if (mdns_service_id != NULL) {
        mDNSDeregisterService(mdns_service_id);
    }
    mDNSResponderDeinit();
    if (mdns_buf != NULL) {
        free(mdns_buf);
        mdns_buf = NULL;
    }
    if (txtRecord != NULL) {
        free(txtRecord);
        txtRecord = NULL;
    }
}

int OTAClass::beginLocal(uint16_t port, bool reboot_when_success) {

    int ret = -1;

    // variables for image processing
    flash_t flash;
    uint32_t img2_addr, img2_len, img3_addr, img3_len;
    uint32_t img_upper_bound;
    uint32_t checksum = 0;
    uint32_t signature1, signature2;

    // variables for network processing
    int server_socket = -1;
    int client_socket = -1;
    struct sockaddr_in localHost;
    struct sockaddr_in client_addr;
    int socket_error, socket_timeout;
    socklen_t optlen;

    // variables for OTA
    unsigned char *buf = NULL;
    int read_bytes = 0, processed_len;
    uint32_t file_info[3];
    uint32_t ota_len;
    uint32_t ota_blk_size = 0;

    int i, n;

    do {
        sync_ota_addr();

        get_image_info(&img2_addr, &img2_len, &img3_addr, &img3_len);
        img_upper_bound = img2_addr + 0x10 + img2_len; // image2 base + header + len
        if (img3_len > 0) {
            img_upper_bound += 0x10 + img3_len; // image 3 header + len
        }

        if ((ota_addr & 0xfff != 0) || (ota_addr == ~0x0) || (ota_addr < img_upper_bound)) {
            OTA_PRINTF("Invalid OTA address: %08X\r\n", ota_addr);
            break;
        }

        buf = (unsigned char *) malloc (BUFSIZE);
        if (buf == NULL) {
            OTA_PRINTF("Fail to allocate memory\r\n");
            break;
        }

        server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_socket < 0) {
            OTA_PRINTF("Fail to create socket\r\n");
            break;
        }

        memset(&localHost, 0, sizeof(localHost));
        localHost.sin_family = AF_INET;
        localHost.sin_port = htons(port);
        localHost.sin_addr.s_addr = INADDR_ANY;

        if (lwip_bind(server_socket, (struct sockaddr *)&localHost, sizeof(localHost)) < 0) {
            OTA_PRINTF("Bind fail\r\n");
            break;
        }

        if (lwip_listen(server_socket , 1) < 0) {
            OTA_PRINTF("Listen fail\r\n");
            break;
        }

        OTA_PRINTF("Wait for client\r\n");
        n = (int) sizeof( client_addr );
        memset(&client_addr, 0, sizeof(client_addr));
        client_socket = lwip_accept(server_socket, (struct sockaddr *) &client_addr, (socklen_t *)&n);
        OTA_PRINTF("Client connected. IP:%s port:%d\r\n\r\n", inet_ntoa(client_addr.sin_addr.s_addr), ntohs(client_addr.sin_port));

        socket_timeout = DEFAULT_IMAGE_DOWNLOAD_TIMEOUT;
        lwip_setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &socket_timeout, sizeof(socket_timeout));        

        OTA_PRINTF("Read OTA info...\r\n");
        read_bytes = read(client_socket, file_info, sizeof(file_info));
        if (read_bytes < 0) {
           OTA_PRINTF("Fail to read OTA info\r\n");
           break;
        }

        if (file_info[2] == 0) {
            OTA_PRINTF("OTA image len is 0\r\n");
            break;
        }

        ota_len = file_info[2];
        ota_blk_size = ((ota_len - 1) / 4096) + 1;
        for (i = 0; i < ota_blk_size; i++) {
            flash_erase_sector(&flash, ota_addr + i * 4096);
        }

        OTA_PRINTF("Start download\r\n");

        // Now download OTA image
        processed_len = 0;
        while( processed_len < ota_len ) {
            memset(buf, 0, BUFSIZE);
            read_bytes = read(client_socket, buf, BUFSIZE);

            if (read_bytes < 0) {
                optlen = sizeof(socket_error);
                getsockopt(client_socket, SOL_SOCKET, SO_ERROR, &socket_error, &optlen);
                if (socket_error == EAGAIN) {
                    // socket timeout
                }
                break;
            }

            if (flash_stream_write(&flash, ota_addr + processed_len, read_bytes, buf) < 0) {
                OTA_PRINTF("Write sector fail\r\n");
                break;
            }

            processed_len += read_bytes;
        }

        if (processed_len != ota_len) {
            OTA_PRINTF("Download fail\r\n");
            break;
        }

        // Read OTA image from flash and calculate checksum
        checksum = processed_len = 0;
        while ( processed_len < ota_len ) {
            n = (processed_len + BUFSIZE < ota_len) ? BUFSIZE : (ota_len - processed_len);
            flash_stream_read(&flash, ota_addr + processed_len, n, buf);
            for (i=0; i<n; i++) checksum += (buf[i] & 0xFF);
            processed_len += n;
        }

        if (checksum != file_info[0]) {
            OTA_PRINTF("Bad checksum:%d expected:%d\r\n", checksum, file_info[0]);
            break;
        }

        // Put signature for OTA image
        flash_write_word(&flash, ota_addr +  8, 0x35393138);
        flash_write_word(&flash, ota_addr + 12, 0x31313738);
        flash_read_word(&flash, ota_addr +  8, &signature1);
        flash_read_word(&flash, ota_addr + 12, &signature2);
        if (signature1 != 0x35393138 || signature2 != 0x31313738) {
            OTA_PRINTF("Put signature fail\r\n");
            break;
        }

        // Mark image 2 as old image
        flash_write_word(&flash, img2_addr + 8, 0x35393130);

        ret = 0;
        OTA_PRINTF("OTA success\r\n");

    } while (0);

    if (buf != NULL) {
        free(buf);
    }

    if (server_socket >= 0) {
        close(server_socket);
    }

    if (client_socket >= 0) {
        close(client_socket);
    }

    if (ret < 0) {
        OTA_PRINTF("OTA fail\r\n");
    } else {
        if (reboot_when_success) {
            sys_reset();
        }
    }

    return ret;
}

int OTAClass::setOtaAddress(uint32_t address) {

    set_system_data(0x0000, address);
    ota_addr = address;
}

int OTAClass::setRecoverPin(uint32_t pin1, uint32_t pin2) {
    uint8_t boot_pin1 = 0xFF;
    uint8_t boot_pin2 = 0xFF;
    uint32_t boot_pins = 0;
    
    if ( pin1 < TOTAL_GPIO_PIN_NUM ) {
        boot_pin1 = (g_APinDescription[pin1].pinname) & 0xFF;
        boot_pin1 |= 0x80;
    }

    if ( pin2 < TOTAL_GPIO_PIN_NUM ) {
        boot_pin2 = (g_APinDescription[pin2].pinname) & 0xFF;
        boot_pin2 |= 0x80;
    }

    if (boot_pin1 != 0xFF || boot_pin2 != 0xFF) {
        boot_pins = (boot_pin1 << 8) | boot_pin2;
        set_system_data(0x0008, boot_pins);
    }
}

int OTAClass::set_system_data(uint32_t address, uint32_t value) {

    flash_t flash;
    uint32_t i, data;

    flash_write_word(&flash, FLASH_SYSTEM_DATA_ADDR + address, value);
    flash_read_word(&flash, FLASH_SYSTEM_DATA_ADDR + address, &data);

    if (value != data) {

		//erase backup sector
		flash_erase_sector(&flash, FLASH_RESERVED_DATA_BASE);

		//backup system data to backup sector
		for(i = 0; i < 0x1000; i+= 4){
			flash_read_word(&flash, FLASH_SYSTEM_DATA_ADDR + i, &data);
			flash_write_word(&flash, FLASH_RESERVED_DATA_BASE + i,data);
		}

		//erase system data
		flash_erase_sector(&flash, FLASH_SYSTEM_DATA_ADDR);

		//write data back to system data
		for(i = 0; i < 0x1000; i+= 4){
			flash_read_word(&flash, FLASH_RESERVED_DATA_BASE + i, &data);
			if(i == address) data = value;
			flash_write_word(&flash, FLASH_SYSTEM_DATA_ADDR + i,data);
		}

		//erase backup sector
		flash_erase_sector(&flash, FLASH_RESERVED_DATA_BASE);
    }
}

int OTAClass::get_image_info(uint32_t *img2_addr, uint32_t *img2_len, uint32_t *img3_addr, uint32_t *img3_len) {

    flash_t flash;
    uint32_t img3_load_addr = 0;

    *img2_addr = IMAGE_2;
    *img3_addr = *img3_len = 0;

    flash_read_word(&flash, *img2_addr, img2_len);
    *img3_addr = IMAGE_2 + *img2_len + 0x10;
    flash_read_word(&flash, *img3_addr, img3_len);
    flash_read_word(&flash, (*img3_addr) + 4, &img3_load_addr);

    if (img3_load_addr != 0x30000000) {
        // There is no img3
        *img3_addr = *img3_len = 0;
    }
}

int OTAClass::sync_ota_addr() {
    flash_t flash;
    uint32_t ota_addr_in_flash;

    flash_read_word(&flash, FLASH_SYSTEM_DATA_ADDR, &ota_addr_in_flash);
    if (ota_addr_in_flash == ~0x0) {
        // No OTA address configuired in flash
        OTA_PRINTF("use default OTA address\r\n");
        ota_addr = DEFAULT_OTA_ADDRESS;
        flash_write_word(&flash, FLASH_SYSTEM_DATA_ADDR, ota_addr);
    } else {
        ota_addr = ota_addr_in_flash;
    }
}

OTAClass OTA;

