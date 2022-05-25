#include "timer.h"

extern uint32_t FREQUENCY;
//*************************************************************************************************
//*****************************         General Purpose Timers            *************************
//*************************************************************************************************
GP_Timers *GP_Timers::pThis[4] = {nullptr};
uint8_t GP_Timers::pThis_Number = 0;
void (*GP_Timers::callbackModbusPacketTimer)() = nullptr;

GP_Timers::GP_Timers(uint8_t timNum) {
    currentNumber = timNum;
    init(timNum);
    pThis[currentNumber - 2] = this;
}

void GP_Timers::delay_us(volatile uint32_t us) {
    counter_us = 0;
    while (counter_us < us) {
        asm("NOP");
    }
}
//----------------------        Modbus end packet timer     ---------------------------------------
void GP_Timers::modbusPacketTimer_start(uint32_t ms) {
    modbusPacketCounter_ms = 0;
    modbusPacketTimerStarted = true;
    modbusPacketCounterStopValue = ms;
}
void GP_Timers::modbusPacketTimer_stop() {
    modbusPacketTimerStarted = false;
    modbusPacketCounter_ms = 0;
}
void GP_Timers::setCallbackModbusPacketTimer(void (*fPtr)()) {callbackModbusPacketTimer = fPtr;}

void GP_Timers::init(uint8_t tim) {
    //! TIM2, TIM3, TIM4, TIM5 - for APB1*2 => clk = FREQUENCY * (1/HPRE_DIV2) * (1/APB1_DIV2) * 2 =>
    // FREQUENCY = 400 MHz => TIM clk = 200 MHz  => PSC+1=TIM_clk/NeededFreq => NeededFreq=TIM_clk/(PSC+1)
    uint32_t TIM_clk = FREQUENCY / 2;
    switch (tim) {
    case 2:
        RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;
        TIM2->PSC = (TIM_clk / (10000000)) - 1; // PSC = 200/10 - 1; PSC = 19 => NeededFreq = 10 MHz
        TIM2->ARR = 10;                         // 1 us
        TIM2->DIER |= TIM_DIER_UIE;             // interrupt at overload
        TIM2->CR1 |= TIM_CR1_CEN;
        NVIC_EnableIRQ(TIM2_IRQn); // irq enable
        break;
    case 3:
        RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;
        TIM3->PSC = (TIM_clk / (1000000)) - 1; // 200/1 - 1; PSC = 199 => NeededFreq = 1 MHz
        TIM3->ARR = 1000;                      // 1 ms
        TIM3->DIER |= TIM_DIER_UIE;            // interrupt at overload
        TIM3->CR1 |= TIM_CR1_CEN;
        NVIC_EnableIRQ(TIM3_IRQn); // irq enable

        break;
    case 4:
        RCC->APB1LENR |= RCC_APB1LENR_TIM4EN;
        TIM4->PSC = (TIM_clk / (1000000)) - 1; // 200/1 - 1; PSC = 199 => NeededFreq = 1 MHz
        TIM4->ARR = 1000;                      // 1 ms
        TIM4->DIER |= TIM_DIER_UIE;            // interrupt at overload
        TIM4->CR1 |= TIM_CR1_CEN;              // timer start
        NVIC_EnableIRQ(TIM4_IRQn); // irq enable
        break;
    case 5:
        //! Antirattle timer 100 ms
        RCC->APB1LENR |= RCC_APB1LENR_TIM5EN;
        TIM5->PSC = 2 * (TIM_clk / (1000000)) - 1; // 400/1 - 1; PSC = 399 => NeededFreq = 0.5 MHz
        TIM5->ARR = 50000;                         // 100 ms
        TIM5->DIER |= TIM_DIER_UIE;                // interrupt at overload
        TIM5->CR1 |= TIM_CR1_CEN;
        NVIC_EnableIRQ(TIM5_IRQn); // irq enable
        break;
    default:
        break;
    }
}


//*************************************************************************************************
//*****************************         Basic Timers            ***********************************    
//*************************************************************************************************
Basic_Timers *Basic_Timers::pThis[2] = {nullptr};
uint8_t Basic_Timers::pThis_Number = 0;

Basic_Timers::Basic_Timers(uint8_t timNum) {
    currentNumber = timNum;
    init(timNum);
    pThis[currentNumber - 6] = this; // (currentNumber must be 6 or 7)
}

void Basic_Timers::init(uint8_t timNum) {
    uint32_t TIM_clk = FREQUENCY / 2;
    switch (timNum) {
    case 6:
        RCC->APB1LENR |= RCC_APB1LENR_TIM6EN;
        TIM6->PSC = (TIM_clk / (1000000)) - 1; // 200/1 - 1; PSC = 199 => NeededFreq = 1 MHz
        TIM6->ARR = 1000;                      // 1 ms
        TIM6->DIER |= TIM_DIER_UIE;            // interrupt at overload
        NVIC_EnableIRQ(TIM6_DAC_IRQn);         // irq enable
        TIM6->CR1 |= TIM_CR1_CEN;              // start timer
        break;
    case 7:
        RCC->APB1LENR |= RCC_APB1LENR_TIM7EN;
        TIM7->PSC = (TIM_clk / (1000000)) - 1; // 200/1 - 1; PSC = 199 => NeededFreq = 1 MHz
        TIM7->ARR = 1000;                      // 1 ms
        TIM7->DIER |= TIM_DIER_UIE;            // interrupt at overload
        NVIC_EnableIRQ(TIM7_IRQn);             // irq enable
        TIM7->CR1 |= TIM_CR1_CEN;              // start timer
        break;
    default:
        break;
    }
}

//! us timer
extern "C" void TIM2_IRQHandler(void) {
    TIM2->SR &= ~TIM_SR_UIF; // drop Update interrupt flag
    GP_Timers::pThis[0]->TimSets = true;
    GP_Timers::pThis[0]->counter_us++;
}

//! 1 ms timer - Independent Watchdog
extern "C" void TIM3_IRQHandler(void) {
    TIM3->SR &= ~TIM_SR_UIF; // drop Update interrupt flag
    GP_Timers::pThis[1]->TimSets = true;
    IWDG1->KR = 0x0000AAAA; // Refreshes counter value in one ms
}

//! 1 ms
extern "C" void TIM4_IRQHandler(void) {
    TIM4->SR &= ~TIM_SR_UIF; // drop Update interrupt flag
    if (GP_Timers::pThis[2]->modbusPacketTimerStarted) {
        if (GP_Timers::pThis[2]->modbusPacketCounter_ms >= GP_Timers::pThis[2]->modbusPacketCounterStopValue) {
            GP_Timers::callbackModbusPacketTimer();
            GP_Timers::pThis[2]->modbusPacketTimerStarted = false;
            GP_Timers::pThis[2]->modbusPacketCounter_ms = 0;
        } else {
            GP_Timers::pThis[2]->modbusPacketCounter_ms++;
        }
    }
}

//! 100 ms timer for delay to exclude rattle from buttons
extern "C" void TIM5_IRQHandler(void) {
    TIM5->SR &= ~TIM_SR_UIF; // drop Update interrupt flag
    // GP_Timers::pThis[3]->TimSets=true;
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

//! __________________________ Basic_Timers interrupt request functions ____________________________

//! _________________   1ms timer   for commutation board communication ____________________________
extern "C" void TIM6_DAC_IRQHandler(void) {
    TIM6->SR &= ~TIM_SR_UIF; // drop Update interrupt flag
}

//! ___ 1ms timer to set Timers for waiting reply (if no reply on timeout => send error status) ___
extern "C" void TIM7_IRQHandler(void) {
    TIM7->SR &= ~TIM_SR_UIF; // drop Update interrupt flag
    //! TODO: set another timers
}
