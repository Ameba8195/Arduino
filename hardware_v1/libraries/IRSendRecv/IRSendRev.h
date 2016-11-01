#ifndef _IRSENDREV_H_
#define _IRSENDREV_H_

#include <inttypes.h>

// len, start_H, start_L, nshort, nlong, data_len, data[data_len]....
#define D_LEN       0
#define D_STARTH    1
#define D_STARTL    2
#define D_SHORT     3
#define D_LONG      4
#define D_DATALEN   5
#define D_DATA      6

#define USECPERTICK 50  // microseconds per clock interrupt tick
#define RAWBUF 300 // Length of raw duration buffer

// Marks tend to be 100us too long, and spaces 100us too short
// when received due to sensor lag.
#define MARK_EXCESS 100

// Results returned from the decoder
class decode_results {
    public:
    volatile unsigned int *rawbuf; // Raw intervals in .5 us ticks
    int rawlen;           // Number of records in rawbuf.
};

class IRSendRev
{
public:
    IRSendRev(uint8_t receivePin, uint8_t transmitPin);
    ~IRSendRev();

private:
    uint8_t receivePin;
    uint8_t transmitPin;

    //**************************send*********************************
public:
    void Send(unsigned char *idata, unsigned char ifreq);

private:
    void *pUART;

    //**************************recv*********************************
public:

    void Init(int revPin);                          // init
    void Init();
    unsigned char Recv(unsigned char *revData);     // 
    unsigned char IsDta();                          // if IR get data
    void Clear();                                   // clear IR data

private:
    uint8_t  rcvstate;
    uint32_t recv_len;
    uint32_t recv_buf[RAWBUF];
    uint32_t prev_timestamp;

    void *pGpioIrqRecv;
    void *pTimerRecv;
    uint8_t  recv_wait_int;

    friend void gpio_ir_recv_handler(uint32_t id, uint32_t event);
    friend void ir_recv_timer_handler(uint32_t id);
};

extern IRSendRev IR;

#endif
