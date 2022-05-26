#include "timer.h"

namespace Timer {
uint32_t Counter_ms{0};
// power on timer
uint16_t powerOnCounter = 0;
uint16_t powerOnTime = 0;
bool PowerOnTimerStarted = false;
void (*powerOnTimeout)();
void setPowerOnTimeout(void (*fPtr)()) { powerOnTimeout = fPtr; }
void startPowerOnTimer(uint16_t ms) {
    powerOnTime = ms;
    PowerOnTimerStarted = true;
}
void stopPowerOnTimer() {
    powerOnTime = 0;
    PowerOnTimerStarted = false;
}
// power off timer
uint16_t powerOffCounter = 0;
uint16_t powerOffTime = 0;
bool PowerOffTimerStarted = false;
void (*powerOffTimeout)();
void setPowerOffTimeout(void (*fPtr)()) { powerOffTimeout = fPtr; }
void startPowerOffTimer(uint16_t ms) {
    powerOffTime = ms;
    PowerOffTimerStarted = true;
}
void stopPowerOffTimer() {
    powerOffTime = 0;
    PowerOffTimerStarted = false;
}

// check direct input pins
bool InBackward = false;
bool InForward = false;
uint8_t antibounceBackwardCounter = 0;
uint8_t antibounceForwardCounter = 0;
bool getInBackward() { return InBackward; }
bool getInForward() { return InForward; }
} // namespace Timer

void Timer::timer3_init() {
    // FREQUENCY on APB1 = 8MHz
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 8;
    TIM3->ARR = 1000;           // period = 1ms
    TIM3->DIER |= TIM_DIER_UIE; // update interrupt enabled
    TIM3->CR1 |= TIM_CR1_CEN;   // counter enabled

    NVIC_EnableIRQ(TIM3_IRQn);
}

void Timer::delay_ms(uint32_t ms) {
    Counter_ms = 0;
    while (Counter_ms <= ms)
        ;
}

// interrupt for power on/off timer
extern "C" void TIM3_IRQHandler(void) {
    TIM3->SR &= ~TIM_SR_UIF;
    // NVIC_ClearPendingIRQ(TIM6_IRQn);
    if (Timer::PowerOnTimerStarted) {
        if (Timer::powerOnCounter >= Timer::powerOnTime) {
            Timer::powerOnCounter = 0;
            Timer::powerOnTime = 0;
            Timer::PowerOnTimerStarted = false;
            Timer::powerOnTimeout();
        } else {
            Timer::powerOnCounter++;
        }
    }
    if (Timer::PowerOffTimerStarted) {
        if (Timer::powerOffCounter >= Timer::powerOnTime) {
            Timer::powerOffCounter = 0;
            Timer::powerOffTime = 0;
            Timer::PowerOffTimerStarted = false;
            Timer::powerOffTimeout();
        } else {
            Timer::powerOffCounter++;
        }
    }
    // check for direct input pins
    if (!(GPIOB->IDR & GPIO_IDR_0)) {
        if (Timer::antibounceBackwardCounter >= Timer::ANTIBOUNCE_COUNTER) {
            Timer::InBackward = true;
            Timer::antibounceBackwardCounter = 0;
        } else {
            Timer::antibounceBackwardCounter++;
        }
    } else {
        Timer::InBackward = false;
        Timer::antibounceBackwardCounter = 0;
    }
    if (!(GPIOB->IDR & GPIO_IDR_1)) {
        if (Timer::antibounceForwardCounter >= Timer::ANTIBOUNCE_COUNTER) {
            Timer::InForward = true;
            Timer::antibounceForwardCounter = 0;
        } else {
            Timer::antibounceForwardCounter++;
        }
    } else {
        Timer::InForward = false;
        Timer::antibounceForwardCounter = 0;
    }
}

// --------------------     Modbus timer    ---------------------------------------
void Timer::timer16_init() {
    // FREQUENCY on APB1 = 8MHz
    RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
    TIM16->PSC = 8;
    TIM16->ARR = 100;            // period = 100us
    TIM16->DIER |= TIM_DIER_UIE; // update interrupt enabled
    TIM16->CR1 |= TIM_CR1_CEN;   // counter enabled

    NVIC_EnableIRQ(TIM16_IRQn);
}

extern volatile unsigned int rxtimer;
extern volatile unsigned char rxcnt; //количество принятых символов
extern volatile unsigned char txcnt; //количество переданных символов
extern volatile unsigned char txlen; //длина посылки на отправку
extern volatile unsigned int delay; //задержка по которой вычисляется конец посылки

extern "C" void TIM16_IRQHandler() {
    TIM16->SR &= ~TIM_SR_UIF;
    //если наш таймер больше уставки задержки и есть символы то есть gap -перерыв в посылке
    //и можно ее обрабатывать
    if ((rxtimer++ > delay) & (rxcnt > 1)) // если rxtimer>delay:
    {
        Modbus::setIsPacketReceived(true); // то выставляем флаг об окончании пакета
    } else {
        //Modbus::setIsPacketReceived(false);
    }
    NVIC_ClearPendingIRQ(TIM16_IRQn);
}
