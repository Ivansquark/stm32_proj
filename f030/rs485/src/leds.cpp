#include "leds.h"

void Leds::init() {
    // B5 - backward, B6 - forward, B7 - EMERGENCY
    // push pull high speed
    GPIOB->MODER |= (GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
    GPIOB->MODER &= ~(GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR5 | GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7);
}

void Leds::backwardOn() { GPIOB->BSRR |= GPIO_BSRR_BS_5; }
void Leds::backwardOff() { GPIOB->BSRR |= GPIO_BSRR_BR_5; }
void Leds::forwardOn() { GPIOB->BSRR |= GPIO_BSRR_BS_6; }
void Leds::forwardOff() { GPIOB->BSRR |= GPIO_BSRR_BR_6; }
void Leds::emergencyOn() { GPIOB->BSRR |= GPIO_BSRR_BS_7; }
void Leds::emergencyOff() { GPIOB->BSRR |= GPIO_BSRR_BR_7; }
void Leds::emergencyToggle() { GPIOB->ODR ^= GPIO_ODR_7; }
