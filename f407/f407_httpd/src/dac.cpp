#include "dac.h"

Dac *Dac::pThis = nullptr;

Dac::Dac() {
    pThis = this;
    init();
}

void Dac::init() {
    /*!< U-PA4 I-PA5 dac-output >*/
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER |= (GPIO_MODER_MODER4_1 | GPIO_MODER_MODE5_1);
    GPIOA->MODER &= ~(GPIO_MODER_MODER4_0 | GPIO_MODER_MODE5_0);         // 1:0 - alt function
    GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR4 | GPIO_OSPEEDER_OSPEEDR5); // 1:1 highest speed
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    DAC->CR |= DAC_CR_EN1; // DAC channel 1 enabled
    DAC->CR |= DAC_CR_EN2;
    DAC->DHR12R1 = 0; // DAC channel1 12-bit right-aligned data holding register
}

void Dac::sin(uint32_t width) {
    for (uint8_t i = 0; i < 60; i++) {
        DAC->DHR12R1 = buf_sin[i];
        for (uint32_t i = 0; i < width; i++) {
        }
    }
}
