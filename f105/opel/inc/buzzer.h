#ifndef BUZZER_H
#define BUZZER_H

#include "main.h"

class Buzzer {
public:
    Buzzer();
    inline void on() {
        GPIOA->BSRR |= GPIO_BSRR_BS1;
    }
    inline void off() {
        GPIOA->BSRR |= GPIO_BSRR_BR1;
    }
    void beep(volatile uint32_t ms);
private:
    void init();
};

#endif //BUZZER_H
