#include "timer.h"

Timer3* Timer3::pThis = nullptr;

Timer3::Timer3() {
    timer_init();
    pThis=this;
}

void Timer3::timer_init() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;    
    TIM3->PSC = 48; // TIM3 clk = 48 MHz
    TIM3->ARR = 1000; // period = 1ms
    TIM3->DIER |= TIM_DIER_UIE; // update interrupt enabled
    TIM3->CR1 |= TIM_CR1_CEN; //counter enabled    
    
    NVIC_EnableIRQ(TIM3_IRQn);
}

void Timer3::delay_ms(uint32_t ms) {
    Counter_ms=0;
    while (Counter_ms <= ms);
}
//! every 1 ms
extern "C" void TIM3_IRQHandler(void) { 
    TIM3->SR &=~ TIM_SR_UIF;
    Timer3::pThis->Counter_ms++;  
    if(Timer3::pThis->Display_counter == 100) {
        Timer3::pThis->Display_counter = 0;
        Timer3::pThis->Display_Show_Flag = true;
    } else {Timer3::pThis->Display_counter++;}
    Adc::pThis->Start_diods_Flag = true;    
	//NVIC_ClearPendingIRQ(TIM6_IRQn);
}

//-----------------------------------------------------------------------------
Timer_antiRattle* Timer_antiRattle::pThis = nullptr;

Timer_antiRattle::Timer_antiRattle() {
    timer_init();
    pThis=this;
}

void Timer_antiRattle::timer_init() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;    
    TIM4->PSC = 4800; // TIM3 clk = 48 MHz  TIM4_clk - 1 kHz
    TIM4->ARR = 200; // period = 20ms
    TIM4->DIER |= TIM_DIER_UIE; // update interrupt enabled
    TIM4->CR1 |= TIM_CR1_CEN; //counter enabled    
    NVIC_EnableIRQ(TIM4_IRQn);
}
extern "C" void TIM4_IRQHandler(void) {
    TIM4->SR &=~ TIM_SR_UIF;
    NVIC_EnableIRQ(EXTI15_10_IRQn);     
    TIM4->CR1 &=~ TIM_CR1_CEN; // disable single shot timer for button anti rattle
}
//----------------------------------------------------------------------------------------
Timer_Bat* Timer_Bat::pThis = nullptr;

Timer_Bat::Timer_Bat() {
    timer_init();
    pThis = this;
}

void Timer_Bat::timer_init() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;    
    TIM5->PSC = 4800; // TIM3 clk = 48 MHz*2  TIM4_clk - 1 kHz
    TIM5->ARR = 10000; // period = 1000ms = 1 sec
    TIM5->DIER |= TIM_DIER_UIE; // update interrupt enabled
    TIM5->CR1 |= TIM_CR1_CEN; //counter enabled    
    NVIC_EnableIRQ(TIM5_IRQn);
}

void Timer_Bat::delay_sec(uint32_t sec) {
    Counter_sec = 0;
    while(Counter_sec < sec) {
        __ASM("nop");
    }
}

extern "C" void TIM5_IRQHandler(void) {
    TIM5->SR &=~ TIM_SR_UIF;
    Timer_Bat::pThis->Counter_sec++;  
    Adc::pThis->Start_battery_Flag = true;    
}