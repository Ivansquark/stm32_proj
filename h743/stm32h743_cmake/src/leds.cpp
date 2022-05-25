#include "leds.h"

Leds::Leds() {
    init();
}

void Leds::init() {
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
    GPIOA->MODER &= ~GPIO_MODER_MODE15;
    GPIOA->MODER |= GPIO_MODER_MODE15_0;
    GPIOA->BSRR |= GPIO_BSRR_BS15; //off
}