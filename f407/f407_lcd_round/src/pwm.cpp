#include "pwm.h"
Pwm* Pwm::pThis = nullptr;

Pwm::Pwm() {
    pThis = this;
    init();
}

void Pwm::pwm1_start() {
    TIM2->CCER &= ~TIM_CCER_CC1E; // channel 1 disbled
    TIM2->CCER |= TIM_CCER_CC4E;  // channel 4 enabled
    TIM2->CNT = 0;
    TIM2->CR1 |= TIM_CR1_CEN;
    Pwm1Started = true;
}
void Pwm::pwm2_start() {
    TIM2->CCER &= ~TIM_CCER_CC4E; // channel 4 disbled
    TIM2->CCER |= TIM_CCER_CC1E;  // channel 1 enabled
    TIM2->CNT = 0;
    TIM2->CR1 |= TIM_CR1_CEN;
    Pwm2Started = true;
}

void Pwm::pwm_start() {
    TIM2->CCER |= TIM_CCER_CC4E; // channel 4 enabled
    TIM2->CCER |= TIM_CCER_CC1E;  // channel 1 enabled
    TIM2->CNT = 0;
    TIM2->CR1 |= TIM_CR1_CEN;
    Pwm1Started = true;
    Pwm2Started = true;

}

void Pwm::pwm_stop() {
    TIM2->CR1 &= ~TIM_CR1_CEN;
    GPIOA->ODR &= ~(GPIO_ODR_ODR_3 | GPIO_ODR_ODR_15);
    TIM2->CNT = 0;
    //TIM2->CCER &= ~TIM_CCER_CC1E; // channel 1 disabled
    //TIM2->CCER &= ~TIM_CCER_CC4E; // channel 4 disabled
    Pwm1Started = false;
    Pwm2Started = false;
}

void Pwm::init() {
    //-----------------------------------------------------
    // PWM A3-PCLOCK1 A15-PCLOCK2
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODER3_1 | GPIO_MODER_MODER15_1;
    GPIOA->MODER &= ~(GPIO_MODER_MODER3_0 | GPIO_MODER_MODER15_0);
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3 | GPIO_OSPEEDER_OSPEEDR15;
    GPIOA->AFR[0] |= (1 << 12);
    GPIOA->AFR[1] |= (1 << 28);
    GPIOA->ODR &= ~(GPIO_ODR_ODR_3 | GPIO_ODR_ODR_15);

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1);
    TIM2->CCMR2 &= ~(TIM_CCMR2_OC4M_0); // 1:1:0 - PWM (active as CNT<CCR1)
    TIM2->CCER |= TIM_CCER_CC4E;        // channel 4 enabled
    TIM2->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1);
    TIM2->CCMR1 &= ~(TIM_CCMR1_OC1M_0); // 1:1:0 - PWM (active as CNT<CCR1)
    TIM2->CCER |= TIM_CCER_CC1E;        // channel 1 enabled

    TIM2->PSC = 84 - 1;   // 1 MHz
    //TIM2->PSC = 8 - 1;   // 10 MHz
    TIM2->ARR = 1000 - 1; // 5 MHz

    TIM2->CCR1 = (TIM2->ARR + 1) / 4;
    TIM2->CCR4 = (TIM2->ARR  + 1) / 6;
    //TIM2->BDTR |= TIM_BDTR_MOE;
    //TIM2->DIER |= TIM_DIER_UIE; // interrupt at overload
    //NVIC_EnableIRQ(TIM2_IRQn);  // irq enable
}

extern "C" void TIM2_IRQHandler(void) {
    Pwm::pThis->pwmIRQ = true;
    TIM2->SR &= ~TIM_SR_UIF;
    NVIC_ClearPendingIRQ(TIM2_IRQn);
}
