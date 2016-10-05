#ifndef _OTA_H_
#define _OTA_H_

#define DEFAULT_OTA_ADDRESS 0x80000
#define DEFAULT_OTA_MDNS_BUF 128

class OTAClass {

public:
    OTAClass();

    int beginArduinoMdnsService(uint16_t port);
    int endArduinoMdnsService();

    int beginLocal(uint16_t port, bool reboot_when_success = true);

    int setOtaAddress(uint32_t address);
    int setRecoverPin(uint32_t pin1, uint32_t pin2 = 0xFFFFFFFF);

private:
    int get_image_info(uint32_t *img2_addr, uint32_t *img2_len, uint32_t *img3_addr, uint32_t *img3_len);
    int sync_ota_addr();
    int set_system_data(uint32_t address, uint32_t value);

    uint32_t ota_addr;

    unsigned char *mdns_buf;
    void *mdns_service_id;
    void *txtRecord;
};

extern OTAClass OTA;

#endif
