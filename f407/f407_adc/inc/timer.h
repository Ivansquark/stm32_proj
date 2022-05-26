#ifndef TIMER_H
#define TIMER_H

#include "stm32f4xx.h"
#include "adc.h"
//#include "disp1602.h"
//! ms timer
class Timer3 {
public:
    Timer3();
    static Timer3* pThis;
    void delay_ms(uint32_t ms);
    uint32_t Counter_ms{0};
    uint32_t Display_counter = 0;
    bool Display_Show_Flag = false;

private:
    void timer_init();   
};
// antirattle timer for button PD-15
class Timer_antiRattle {
public:
    Timer_antiRattle();
    void delay_ms(uint32_t ms);    
static Timer_antiRattle* pThis;
private:
    void timer_init();
};

// timer-5 ADC_bat timer
class Timer_Bat {
public:
    Timer_Bat();
    static Timer_Bat* pThis;
    void delay_sec(uint32_t sec);
    uint32_t Counter_sec{0};
private:
    void timer_init();
};

#endif //TIMER_H