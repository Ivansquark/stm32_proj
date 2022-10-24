#ifndef TIMER_H
#define TIMER_H

#include "encoder.h"
#include "stm32h743xx.h"

class GP_Timers {
  public:
    enum Period
    {
        ms = 0,
        us = 1
    };
    static uint8_t pThis_Number;
    GP_Timers(uint8_t timNum, Period period);
    bool TimSets = 0;
    uint32_t us_counter = 0;
    uint32_t us_counter2 = 0;
    uint32_t ms_counter = 0;
    void delay_us(uint32_t us);
    static GP_Timers* pThis[3];

  private:
    void timer_ini(uint8_t tim, Period period);
};

#endif // TIMER_H