#include "button.h"

Button* Button::pThis = nullptr;

Button::Button() {
    pThis = this;
    init();
}

void Button::init() {
    // GPIO inits A5_UP A6_DOWN - buttons
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER6); // 0:0 input state
    GPIOA->PUPDR |= (GPIO_PUPDR_PUPDR5_0 | GPIO_PUPDR_PUPDR6_0);
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR5_1 | GPIO_PUPDR_PUPDR6_1); // 0:1 pull-up
    //! EXTI inits
    /*!< SYSCFG and comparator clock enable */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
    SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR2_EXTI5_PA | SYSCFG_EXTICR2_EXTI6_PA);
    EXTI->IMR |= (EXTI_IMR_IM5 | EXTI_IMR_IM6);
    EXTI->FTSR |= (EXTI_FTSR_FT5 | EXTI_FTSR_FT6); //falling trigger
    NVIC_EnableIRQ(EXTI4_15_IRQn);
    //!__________ TIM2 init short press button timer______________________
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 32000; // TIM2 clk = 32 MHz  TIM3_clk - 1 kHz
    TIM2->ARR = 500; // period = 500ms
    TIM2->DIER |= TIM_DIER_UIE; // update interrupt enabled
    TIM2->CR1 |= TIM_CR1_CEN; //counter enabled    
    NVIC_EnableIRQ(TIM2_IRQn);
    //!__________ TIM3 init long press button timer______________________
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 32000; // TIM3 clk = 32 MHz  TIM3_clk - 1 kHz
    TIM3->ARR = 1500; // period = 1500ms
    TIM3->DIER |= TIM_DIER_UIE; // update interrupt enabled
    TIM3->CR1 |= TIM_CR1_CEN; //counter enabled    
    NVIC_EnableIRQ(TIM3_IRQn);
}

void EXTI4_15_IRQHandler(void) {
    NVIC_ClearPendingIRQ(EXTI4_15_IRQn); 
    NVIC_DisableIRQ(EXTI4_15_IRQn);//! disable interrupt while timer6 updates     
    TIM2->CR1 |= TIM_CR1_CEN; //start timer
}

void TIM2_IRQHandler(void) {
    NVIC_ClearPendingIRQ(TIM2_IRQn);
    NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
    TIM2->SR &= ~TIM_SR_UIF; //clear interrupt flag
    TIM2->CR1 &= ~TIM_CR1_CEN; //stop timer
    
    
    //pressed both buttons
    
    if((EXTI->PR & EXTI_PR_PIF5) and (EXTI->PR & EXTI_PR_PIF6)) {
        //Was pressed both buttons
        EXTI->PR |= EXTI_PR_PIF5; // clear flag     
        EXTI->PR |= EXTI_PR_PIF6; // clear flag
        NVIC_ClearPendingIRQ(EXTI4_15_IRQn); // without this sets another interrupt!!!!!!!!!!
        Button::pThis->butState = ButtonState::UP_DOWN;   
    } else if(EXTI->PR & EXTI_PR_PIF5) {
        EXTI->PR |= EXTI_PR_PIF5; // clear flag 
        NVIC_ClearPendingIRQ(EXTI4_15_IRQn);    
        Button::pThis->butState = ButtonState::UP;
    } else if(EXTI->PR & EXTI_PR_PIF6) {
        EXTI->PR |= EXTI_PR_PIF6; // clear flag
        NVIC_ClearPendingIRQ(EXTI4_15_IRQn);        
        Button::pThis->butState = ButtonState::DOWN;
    }  
    if(!(GPIOA->IDR & GPIO_IDR_5)) {
        EXTI->PR |= EXTI_PR_PIF5; // clear flag
        NVIC_ClearPendingIRQ(EXTI4_15_IRQn);   
        Button::pThis->butState = ButtonState::UP_LONG;
        TIM3->CR1 |= TIM_CR1_CEN; // start long pressed button timer
    } else if(!(GPIOA->IDR & GPIO_IDR_6)) {
        EXTI->PR |= EXTI_PR_PIF6; // clear flag
        NVIC_ClearPendingIRQ(EXTI4_15_IRQn);   
        Button::pThis->butState = ButtonState::DOWN_LONG;
        TIM3->CR1 |= TIM_CR1_CEN; // start long pressed button timer
    } else {
        NVIC_EnableIRQ(EXTI4_15_IRQn); // short pressed + pause for rattle is finished
    }
    TIM2->CNT = 0;
}

void TIM3_IRQHandler(void) {
    NVIC_EnableIRQ(EXTI4_15_IRQn); // short pressed + pause for rattle is finished
    EXTI->PR |= EXTI_PR_PIF5; // clear flag
    EXTI->PR |= EXTI_PR_PIF6; // clear flag
    NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
    TIM3->SR &=~ TIM_SR_UIF; //clear interrupt flag
    TIM3->CR1 &=~ TIM_CR1_CEN; // disable timer
    TIM3->CNT = 0;
}