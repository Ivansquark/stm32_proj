#include "leds.h"

Leds* Leds::pThis = nullptr;

Leds::Leds() {
    pThis = this;
    init();
}

void Leds::init() {
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
    GPIOA->MODER &= ~GPIO_MODER_MODE15;
    GPIOA->MODER |= GPIO_MODER_MODE15_0;
    ledCommutationBoardOn();    
}