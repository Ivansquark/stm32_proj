#ifndef LEDS_H
#define LEDS_H

#include "main.h"

class Leds {
public:
    Leds();
    inline void ledCanOn() {
        GPIOA->BSRR |= GPIO_BSRR_BS5;
    }
    inline void ledCanOff() {
        GPIOA->BSRR |= GPIO_BSRR_BR5;
    }
    inline void ledCanToggle() {
        GPIOA->ODR ^= GPIO_ODR_ODR5;
    }    
private:
    void init();
};

#endif // LEDS_H