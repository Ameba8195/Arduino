/*
SoftwareSerial.cpp (formerly NewSoftSerial.cpp) - 
Multi-instance software serial library for Arduino/Wiring
-- Interrupt-driven receive and other improvements by ladyada
   (http://ladyada.net)
-- Tuning, circular buffer, derivation from class Print/Stream,
   multi-instance support, porting to 8MHz processors,
   various optimizations, PROGMEM delay tables, inverse logic and 
   direct port writing by Mikal Hart (http://www.arduiniana.org)
-- Pin change interrupt macros by Paul Stoffregen (http://www.pjrc.com)
-- 20MHz processor support by Garrett Mace (http://www.macetech.com)
-- ATmega1280/2560 support by Brett Hagman (http://www.roguerobotics.com/)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

The latest version of this library can always be found at
http://arduiniana.org.
*/

// 
// Includes
// 
#include <Arduino.h>
#include <SoftwareSerial.h>

#ifdef __cplusplus
extern "C" {
#endif

serial_t sobj;
extern PinDescription g_APinDescription[];

#ifdef __cplusplus
}
#endif

//
// Statics
//
SoftwareSerial *SoftwareSerial::active_object = NULL;

//
// Private methods
//

void SoftwareSerial::handle_interrupt(uint32_t id, SerialIrq event) {
    if (active_object != NULL) {
        active_object->recv(id, event);
    }
}

// This function sets the current object as the "listening"
// one and returns true if it replaces another 
bool SoftwareSerial::listen()
{
    bool ret = false;

    if (active_object != this && active_object != NULL)
    {
        active_object->stopListening();
        ret = true;
    }

    serial_init(pUART, (PinName)g_APinDescription[transmitPin].pinname, (PinName)g_APinDescription[receivePin].pinname);
    serial_baud(pUART, speed);
    serial_format(pUART, 8, ParityNone, 1);

    serial_irq_handler(pUART, handle_interrupt, (uint32_t)pUART);
    serial_irq_set(pUART, RxIrq, 1);
    serial_irq_set(pUART, TxIrq, 1);
    active_object = this;

    return ret;
}

// Stop listening. Returns true if we were actually listening.
bool SoftwareSerial::stopListening()
{
    serial_free(pUART);
    pUART = NULL;
    return true;
}

//
// The receive routine called by the interrupt handler
//
void SoftwareSerial::recv(uint32_t id, SerialIrq event)
{
    volatile char d = 0;
    uint8_t next;

    if(event == RxIrq) {
        d = serial_getc(pUART);
        next = (_receive_buffer_tail + 1) % _SS_MAX_RX_BUFF;
        if (next != _receive_buffer_head) {
            _receive_buffer[_receive_buffer_tail] = d; // save new byte
            _receive_buffer_tail = next;
        } else {
            _buffer_overflow = true;
        }
    }
}

//
// Constructor
//
SoftwareSerial::SoftwareSerial(uint8_t receivePin, uint8_t transmitPin, bool inverse_logic /* = false */) : 
  _buffer_overflow(false)
{
    this->receivePin = receivePin;
    this->transmitPin = transmitPin;
    pUART = NULL;
}

//
// Destructor
//
SoftwareSerial::~SoftwareSerial()
{
    end();
}

//
// Public methods
//

void SoftwareSerial::begin(long speed)
{
    pUART = &sobj;
    this->speed = speed;
    listen();
}

void SoftwareSerial::end()
{
    stopListening();
}


// Read data from buffer
int SoftwareSerial::read()
{
    if (!isListening())
        return -1;

    // Empty buffer?
    if (_receive_buffer_head == _receive_buffer_tail)
        return -1;

    // Read from "head"
    uint8_t d = _receive_buffer[_receive_buffer_head]; // grab next byte
    _receive_buffer_head = (_receive_buffer_head + 1) % _SS_MAX_RX_BUFF;
    _buffer_overflow = false;
    return d;
}

int SoftwareSerial::available()
{
    if (!isListening())
        return 0;

    return (_receive_buffer_tail + _SS_MAX_RX_BUFF - _receive_buffer_head) % _SS_MAX_RX_BUFF;
}

size_t SoftwareSerial::write(uint8_t b)
{
    serial_putc(pUART, b);
  
    return 1;
}

void SoftwareSerial::flush()
{
    if (!isListening())
        return;

    _receive_buffer_head = _receive_buffer_tail = 0;
    _buffer_overflow = false;
}

int SoftwareSerial::peek()
{
    if (!isListening())
        return -1;

    // Empty buffer?
    if (_receive_buffer_head == _receive_buffer_tail)
        return -1;

    // Read from "head"
    return _receive_buffer[_receive_buffer_head];
}

