#include "scheduler.h"

volatile uint32_t ms_counter=0;
volatile uint32_t s_counter=0;
volatile uint32_t us_counter=0;
volatile uint32_t ms_delay_counter = 0;

void delay_us(uint32_t us) {
    us_counter = 0;
    while(us_counter< us) {
        __ASM("nop");
    }
}

void delay_ms_sys(uint32_t ms) {
    ms_delay_counter = 0;
    while(ms_delay_counter< ms) {
        __ASM("nop");
    }
}

void scheduler_init() {
    // _____ initialization of system timer ______
    ms_counter=0;
    if(SysTick_Config(64000)) {
        while(1); //ERROR
    }
    // _______ initialization TIM3 for us timer ___________
    RCC->APBENR1 |= RCC_APBENR1_TIM3EN;
    TIM3->PSC = 0; // 64MHz
    TIM3->ARR |= 64-1; // 1 MHz  (1 us period)
    TIM3->DIER |= TIM_DIER_UIE; //update interrupt enable
    TIM3->CR1 |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM3_IRQn);
}

void SysTick_Handler(void) {
    ms_counter++;
    ms_delay_counter++;
    if(ms_counter == 5000) {
        ms_counter = 0;
        s_counter++;
    }
}

void TIM3_IRQHandler(void) {
    TIM3->SR &=~ TIM_SR_UIF; //clear interrupt flag
    us_counter++;
}