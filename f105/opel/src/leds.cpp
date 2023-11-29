#include "leds.h"

Leds::Leds() {
    init();
}

void Leds::init() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRL |= (GPIO_CRL_MODE5);
    GPIOA->CRL &= ~GPIO_CRL_CNF5;
}