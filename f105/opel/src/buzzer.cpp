#include "buzzer.h"

Buzzer::Buzzer() {
    init();
}

void Buzzer::beep(volatile uint32_t ms) {
    on();
    HAL_Delay(ms);
    off();
}

void Buzzer::init() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRL |= (GPIO_CRL_MODE1);
    GPIOA->CRL &= ~GPIO_CRL_CNF1;
}