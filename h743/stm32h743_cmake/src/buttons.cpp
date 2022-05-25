#include "buttons.h"

Buttons* Buttons::pThis = nullptr;

Buttons::Buttons() {
    pThis = this;
    init();
}

void Buttons::init() {
    //! BUT_ON-PC8 BUT_OFF-PC9 input state
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;
    GPIOC->MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9); //0:0 Input mode

    RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
    SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PC | SYSCFG_EXTICR3_EXTI9_PC);
    EXTI->FTSR1 |= (EXTI_FTSR1_TR8 | EXTI_FTSR1_TR9);  // on fall edge
    EXTI->IMR1 |= (EXTI_IMR1_IM8 | EXTI_IMR1_IM9); // unmask input interrupt
    EXTI->PR1 |= (EXTI_PR1_PR8 | EXTI_PR1_PR9); // clear pending bits
    NVIC_EnableIRQ(EXTI9_5_IRQn);

    //! BUT_?-PE11 BUT_?-PE12 BUT_?-PE13 input state
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
    GPIOE->MODER &= ~(GPIO_MODER_MODE11 | GPIO_MODER_MODE12 | GPIO_MODER_MODE13); //0:0 Input mode

    RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
    SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PE);
    SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PE | SYSCFG_EXTICR4_EXTI13_PE);
    EXTI->FTSR1 |= (EXTI_FTSR1_TR11 | EXTI_FTSR1_TR12 | EXTI_FTSR1_TR13);  // on fall edge
    EXTI->IMR1 |= (EXTI_IMR1_IM11 | EXTI_IMR1_IM12 | EXTI_IMR1_IM13); // unmask input interrupt
    EXTI->PR1 |= (EXTI_PR1_PR11 | EXTI_PR1_PR12 | EXTI_PR1_PR13); // clear pending bits
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}


extern "C" void EXTI9_5_IRQHandler(void) {
    if(EXTI->PR1 & EXTI_PR1_PR8) {
        EXTI->PR1 |= EXTI_PR1_PR8; // clear pending flag
        Buttons::pThis->butOnPressed = true;
        NVIC_DisableIRQ(EXTI9_5_IRQn);
        TIM5->CR1 |= TIM_CR1_CEN; //start timer thats enabling EXTI interrupts 
    }
    if(EXTI->PR1 & EXTI_PR1_PR9) {
        EXTI->PR1 |= EXTI_PR1_PR9; // clear pending flag
        Buttons::pThis->butOffPressed = true;
        NVIC_DisableIRQ(EXTI9_5_IRQn);
        TIM5->CR1 |= TIM_CR1_CEN; //start timer thats enabling EXTI interrupts
    }
}

extern "C" void EXTI15_10_IRQHandler(void) {
    if(EXTI->PR1 & EXTI_PR1_PR11) {
        EXTI->PR1 |= EXTI_PR1_PR11; // clear pending flag
        Buttons::pThis->but11Pressed = true;
        NVIC_DisableIRQ(EXTI15_10_IRQn);
        TIM5->CR1 |= TIM_CR1_CEN; //start timer thats enabling EXTI interrupts
    }
    if(EXTI->PR1 & EXTI_PR1_PR12) {
        EXTI->PR1 |= EXTI_PR1_PR12; // clear pending flag
        Buttons::pThis->but12Pressed = true;
        NVIC_DisableIRQ(EXTI15_10_IRQn);
        TIM5->CR1 |= TIM_CR1_CEN; //start timer thats enabling EXTI interrupts
    }
    if(EXTI->PR1 & EXTI_PR1_PR13) {
        EXTI->PR1 |= EXTI_PR1_PR13; // clear pending flag
        Buttons::pThis->but13Pressed = true;
        NVIC_DisableIRQ(EXTI15_10_IRQn);
        TIM5->CR1 |= TIM_CR1_CEN; //start timer thats enabling EXTI interrupts
    }
}
