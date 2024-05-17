#ifndef BUZZER_H
#define BUZZER_H

#include "stm32h743xx.h"
#include "timer.h"

class Buzzer {
  public:
    Buzzer() { buzzer_init(); }
    void beep() {
        int x = 10000;
        GPIOB->ODR ^= GPIO_ODR_OD9;
        while (x--)
            ;
    }
    void play(uint32_t freq) {
        if (!freq) return;
        uint32_t per = 500000 / freq;
        GP_Timers::pThis[0]->ms_counter = 0;
        while (GP_Timers::pThis[0]->ms_counter < 50) {
            on();
            GP_Timers::pThis[0]->delay_us(per);
            off();
            GP_Timers::pThis[0]->delay_us(per);
        }
    }
    inline void startPlay();
    inline void on() { GPIOB->BSRR |= GPIO_BSRR_BS9; }
    inline void off() { GPIOB->BSRR |= GPIO_BSRR_BR9; }

  private:
    void buzzer_init() {
        RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
        GPIOB->MODER |= GPIO_MODER_MODE9_0;
        GPIOB->MODER &= ~GPIO_MODER_MODE9_1;    // 0:1 output
        GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED9; // 1:1 max speed
    }
    bool StartPlay = false;
};

#endif // BUZZER_H