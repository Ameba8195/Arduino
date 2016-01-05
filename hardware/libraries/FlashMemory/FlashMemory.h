#ifndef _FLASH_MEMORY_H_
#define _FLASH_MEMORY_H_

// Decide starting flash address for storing application data
// User should pick address carefully to avoid corrupting image section
#define FLASH_MEMORY_APP_BASE 0xFF000

class FlashMemoryClass
{
private:
    unsigned int _base_address;
    void* pFlash;

public:
    FlashMemoryClass(unsigned int base_address, unsigned int _buf_size);
    ~FlashMemoryClass();

    void read();
    void update();

    unsigned int buf_size;
    unsigned char *buf;
};

extern FlashMemoryClass FlashMemory;

#endif
