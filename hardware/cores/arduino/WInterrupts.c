#include "Winterrupts.h"

void attachInterrupt(uint32_t pin, void (*callback)(void), uint32_t mode) {
    if ( g_APinDescription[pin].ulPinType != PIO_GPIO_IRQ ) {
        pinRemoveMode(pin);
        
    }

    switch(mode) {
        case LOW:
            break;
        case CHANGE:
            break;
        case FALLING:
            pinMode(pin, INPUT_IRQ_FALL);
            digitalSetIrqHandler(pin, callback);
            break;
        case RISING:
            pinMode(pin, INPUT_IRQ_RISE);
            digitalSetIrqHandler(pin, callback);
            break;
    }
}

void detachInterrupt(uint32_t pin) {
    if ( g_APinDescription[pin].ulPinType == PIO_GPIO_IRQ ) {
        pinRemoveMode(pin);
    }
}
