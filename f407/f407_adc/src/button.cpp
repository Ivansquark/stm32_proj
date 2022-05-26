#include "button.h"

Button* Button::pThis = nullptr;

Button::Button() {
    but_init();
		pThis=this;
}

void Button::but_init() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    GPIOD->MODER &=~ GPIO_MODER_MODE15; // input state
    
		RCC->APB2ENR |=  RCC_APB2ENR_SYSCFGEN;
    SYSCFG->EXTICR[3] |= (3<<12); // PD on 15 EXTI
    EXTI->IMR |= EXTI_IMR_IM15;
    EXTI->RTSR |= EXTI_RTSR_TR15;
		NVIC_EnableIRQ(EXTI15_10_IRQn);
}

extern "C" void EXTI15_10_IRQHandler(void) {
    if(EXTI->PR & EXTI_PR_PR15) {
        //!Reset interrupt flag 
        NVIC_DisableIRQ(EXTI15_10_IRQn);//! отключаем прерывание от кнопки (прерывания не будет пока не отсчитает таймер 1)
        TIM4->CR1 |= TIM_CR1_CEN; //! включаем таймер1 для отсчета задержки от дребезга по прерыванию которого включаем прерывание от кнопки
        if(!Display::pThis->LED_flag){
            Display::pThis->LED_flag = true;
        } else {Display::pThis->LED_flag = false;}
        EXTI->PR = EXTI_PR_PR15;  //reset interrupt flag
        //NVIC_EnableIRQ(EXTI15_10_IRQn);
    }
}

