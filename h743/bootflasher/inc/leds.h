#ifndef LEDS_H
#define LEDS_H

#include "main.h"

class Leds {
public:
    Leds();
    static Leds* pThis;
    inline void ledCommutationBoardOn() {
        GPIOA->BSRR |= GPIO_BSRR_BR15;
    }
    inline void ledCommutationBoardOff() {
        GPIOA->BSRR |= GPIO_BSRR_BS15;
    }
private:
    void init();
};

#endif //LEDS_H