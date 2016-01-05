

#include "FlashMemory.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "flash_api.h"
#include "cmsis_os.h"

#ifdef __cplusplus
}
#endif

flash_t flash_obj;

FlashMemoryClass::FlashMemoryClass(unsigned int base_address, unsigned int _buf_size) {
    _base_address = base_address;
    buf_size = (_buf_size > FLASH_SECTOR_SIZE) ? FLASH_SECTOR_SIZE : _buf_size;

    buf = (unsigned char *) malloc ( buf_size );

    pFlash = (void *)(&flash_obj);
}

FlashMemoryClass::~FlashMemoryClass() {
    if (buf != NULL) {
        free(buf);
        buf = NULL;
    }
}

void FlashMemoryClass::read() {
    flash_stream_read((flash_t *)pFlash, _base_address, buf_size, buf);
}

void FlashMemoryClass::update() {
    flash_erase_sector((flash_t *)pFlash, _base_address);
    flash_stream_write((flash_t *)pFlash, _base_address, buf_size, buf);
}

FlashMemoryClass FlashMemory = FlashMemoryClass(FLASH_MEMORY_APP_BASE, FLASH_SECTOR_SIZE);

