#ifndef _SDFILE_H_
#define _SDFILE_H_

#include <inttypes.h>
#include <Stream.h>

class SdFatFile : public Stream {

public:
    SdFatFile();
    ~SdFatFile();

    virtual size_t write(uint8_t c);
    virtual size_t write(const uint8_t *buf, size_t size);
    virtual int read();
    virtual int peek();
    virtual int available();
    virtual void flush();

    operator bool();

    int read(void *buf, uint16_t nbyte);
    int seek(uint32_t pos);
    int close();

    friend class SdFatFs;

private:
    void *m_file;
};

#if 0
class SdFatFile : public Stream {

private:
    char _name[13]; // our name
    SdFile *_file;  // underlying file pointer

public:
    SdFatFile();
    ~SdFatFile();

    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buf, size_t size);
    virtual int read();
    virtual int peek();
    virtual int available();
    virtual void flush();

    int read(void *buf, uint16_t nbyte);
    boolean seek(uint32_t pos);
    uint32_t position();
    uint32_t size();
    void close();
    operator bool();
    char * name();

    boolean isDirectory(void);
    File openNextFile(uint8_t mode = O_RDONLY);
    void rewindDirectory(void);
  
    using Print::write;

    friend class SdFatFs;
};
#endif

#endif
