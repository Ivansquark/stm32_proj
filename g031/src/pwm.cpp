#include "pwm.h"

PWM* PWM::pThis = nullptr;

PWM::PWM() {
    init();
    pThis = this;
}

void PWM::setDuty(uint16_t duty) {
    TIM1->CCR1 = duty;
    this->duty = ((duty*100)/(TIM1->ARR));         
}

void PWM::init() {
    //! GPIO init
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODE8_1;
    GPIOA->MODER &=~ GPIO_MODER_MODE8_0; // 1:0 alt func
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED8;
    GPIOA->AFR[1] = (2<<0); // AF2 on PA8

    //! PWM on TIM1
    RCC->APBENR2 |= RCC_APBENR2_TIM1EN;
    TIM1->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1);
    TIM1->CCMR1 &=~ (TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_3); // 0:1:1:0 - PWM mode 1 (active as CNT<CCR1)
    TIM1->CCER |= TIM_CCER_CC1E; // channel 1 enabled
    TIM1->PSC = 1;
    TIM1->ARR = 36361; // for 200 Hz
    TIM1->CCR1 = TIM1->ARR/2;    
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->CR1 |= TIM_CR1_CEN;
}