#include "buttons.h"
uint8_t enter_pressed_flag = 0;
uint8_t up_pressed_flag = 0;
uint8_t down_pressed_flag = 0;
uint8_t enter_short_pressed_flag = 0;
uint8_t up_short_pressed_flag = 0;
uint8_t down_short_pressed_flag = 0;
uint8_t enter_long_pressed_flag = 0;
uint8_t up_long_pressed_flag = 0;
uint8_t down_long_pressed_flag = 0;
void buttons_init() {
    enter_short_pressed_flag=0; up_short_pressed_flag=0; down_short_pressed_flag=0;
    enter_long_pressed_flag=0; up_long_pressed_flag=0; down_long_pressed_flag=0;
    enter_pressed_flag=0; up_pressed_flag=0; down_pressed_flag=0;
    //! __________GPIO init: B9-enter, C14-up, C15-down;______________
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
    RCC->IOPENR |= RCC_IOPENR_GPIOCEN;
    GPIOB->MODER &=~ GPIO_MODER_MODE9; //input state
    GPIOC->MODER &=~ (GPIO_MODER_MODE14 | GPIO_MODER_MODE15); //input state
    //! _____________EXTI init_____________
    RCC->APBENR2 |= RCC_APBENR2_SYSCFGEN;
    //SYSCFG->IT_LINE_SR[7] |= (SYSCFG_ITLINE7_SR_EXTI9|SYSCFG_ITLINE7_SR_EXTI14| SYSCFG_ITLINE7_SR_EXTI15);
    EXTI->IMR1 |= (EXTI_IMR1_IM9|EXTI_IMR1_IM14|EXTI_IMR1_IM15);
    EXTI->EXTICR[2] |= (EXTI_EXTICR3_EXTI9_0 ); //9-B9 
    EXTI->EXTICR[3] |= (EXTI_EXTICR4_EXTI14_1 | EXTI_EXTICR4_EXTI15_1); //14-C14 15-C15
    EXTI->FTSR1 |= (EXTI_FTSR1_FT9 | EXTI_FTSR1_FT14 | EXTI_FTSR1_FT15); //falling trigger (first fall) 
    NVIC_SetPriority(EXTI4_15_IRQn, 1);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
    //!__________ TIM6 init short press button timer______________________
    RCC->APBENR1 |= RCC_APBENR1_TIM6EN;
    TIM6->PSC = 64000; // TIM3 clk = 64 MHz  TIM6_clk - 1 kHz
    TIM6->ARR = 450; // period = 500ms
    TIM6->DIER |= TIM_DIER_UIE; // update interrupt enabled
    TIM6->CR1 |= TIM_CR1_CEN; //counter enabled    
    NVIC_EnableIRQ(TIM6_IRQn);
    //!__________ TIM7 init long press button timer______________________
    RCC->APBENR1 |= RCC_APBENR1_TIM7EN;
    TIM7->PSC = 64000; // TIM3 clk = 64 MHz  TIM6_clk - 1 kHz
    TIM7->ARR = 2000; // period = 2000ms
    TIM7->DIER |= TIM_DIER_UIE; // update interrupt enabled
    TIM7->CR1 |= TIM_CR1_CEN; //counter enabled    
    NVIC_EnableIRQ(TIM7_IRQn);
}

void EXTI4_15_IRQHandler(void) {
    NVIC_DisableIRQ(EXTI4_15_IRQn);//! disable interrupt while timer6 updates    
    NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
    TIM6->CR1 |= TIM_CR1_CEN;  
}

//! short press button timer timeout
void TIM6_IRQHandler(void) {
    //*((uint32_t*)NVIC->ICPR) |= 0x80;
    //SCB->ICSR |= SCB_ICSR_PENDSVCLR_Msk;
    TIM6->CNT = 0;
    TIM6->CR1 &=~ TIM_CR1_CEN; // disable timer
    if(EXTI->FPR1 & EXTI_FPR1_FPIF9) {
        //!Reset interrupt flag 
        NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
        EXTI->FPR1 |= EXTI_FPR1_FPIF9; // clear flag
        enter_pressed_flag = 1;        
    } else if(EXTI->FPR1 & EXTI_FPR1_FPIF14) {
        //!Reset interrupt flag
        NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
        EXTI->FPR1 |= EXTI_FPR1_FPIF14; // clear flag 
        down_pressed_flag = 1;
    } else if(EXTI->FPR1 & EXTI_FPR1_FPIF15) {
        //!Reset interrupt flag 
        NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
        EXTI->FPR1 |= EXTI_FPR1_FPIF15; // clear flag
        TIM6->CNT = 0;
        up_pressed_flag = 1;
    }  
    while(NVIC->ISPR[0] & 0x80) {
        NVIC->ICPR[0] |= 0x80;
    }
    if(enter_pressed_flag) {
        enter_pressed_flag = 0;
        //! if enter button still pressed 
        if(!(GPIOB->IDR & GPIO_IDR_ID9)) {
            enter_long_pressed_flag = 1; enter_short_pressed_flag = 0;
            TIM7->CR1 |= TIM_CR1_CEN; // start long pressed button timer
        } else {
            enter_short_pressed_flag = 1; enter_long_pressed_flag = 0;
            NVIC_EnableIRQ(EXTI4_15_IRQn);
        }
    } else if(up_pressed_flag) {
        up_pressed_flag = 0;
        //! if up button still pressed 
        if(!(GPIOC->IDR & GPIO_IDR_ID15)) {
            up_long_pressed_flag = 1; up_short_pressed_flag = 0;
            TIM7->CR1 |= TIM_CR1_CEN; // start long pressed button timer
        } else {
            up_short_pressed_flag = 1; up_long_pressed_flag = 0;
            NVIC_EnableIRQ(EXTI4_15_IRQn);
        }
    } else if(down_pressed_flag) {
        down_pressed_flag = 0;
        //! if down button still pressed 
        if(!(GPIOC->IDR & GPIO_IDR_ID14)) {
            down_long_pressed_flag = 1; down_short_pressed_flag = 0;
            TIM7->CR1 |= TIM_CR1_CEN; // start long pressed button timer
        } else {
            down_short_pressed_flag = 1; down_long_pressed_flag = 0; 
            NVIC_EnableIRQ(EXTI4_15_IRQn);
        }
    } else {
        TIM7->CR1 |= TIM_CR1_CEN;
        //NVIC_EnableIRQ(EXTI4_15_IRQn);
    }  
    TIM6->SR &=~ TIM_SR_UIF; //clear interrupt flag      
}
//! long pressed button additional timer
void TIM7_IRQHandler(void) {    
    TIM7->CNT = 0;
    TIM7->CR1 &=~ TIM_CR1_CEN; // disable timer
    TIM7->SR &=~ TIM_SR_UIF; //clear interrupt flag
    //!_________________________________________________________________________________________________
    //! @brief also need to clear pending register cause interrupt is appear with turned off NVIC!!!!!!
    if(EXTI->FPR1 & EXTI_FPR1_FPIF9) {
        //!Reset interrupt flag 
        NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
        EXTI->FPR1 |= EXTI_FPR1_FPIF9; // clear flag      
    } else if(EXTI->FPR1 & EXTI_FPR1_FPIF14) {
        //!Reset interrupt flag
        NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
        EXTI->FPR1 |= EXTI_FPR1_FPIF14; // clear flag 
    } else if(EXTI->FPR1 & EXTI_FPR1_FPIF15) {
        //!Reset interrupt flag 
        NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
        EXTI->FPR1 |= EXTI_FPR1_FPIF15; // clear flag
    }  
    while(NVIC->ISPR[0] & 0x80) {
        NVIC->ICPR[0] |= 0x80;
    }
    //______________________________________________________________________________
    NVIC_EnableIRQ(EXTI4_15_IRQn);         
}