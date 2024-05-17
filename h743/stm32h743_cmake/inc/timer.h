#ifndef TIMER_H
#define TIMER_H

#include "stm32h743xx.h"

class GP_Timers {
public:    
    GP_Timers(uint8_t timNum = 2);      
    static uint8_t pThis_Number;    
    uint8_t currentNumber = 0;
    bool TimSets=false;
    volatile uint32_t counter_us=0;   
    void delay_us(volatile uint32_t us); 
    
   uint32_t modbusPacketCounter_ms = 0;
    uint32_t modbusPacketCounterStopValue = 0;
    bool modbusPacketTimerStarted = false;
    void modbusPacketTimer_start(uint32_t ms);
    void modbusPacketTimer_stop();
    static void (*callbackModbusPacketTimer)();
    void setCallbackModbusPacketTimer(void (*fPtr)());   //connection to timeout function

    
static GP_Timers* pThis[4];
private:
    void init(uint8_t timNum);
};


class Basic_Timers {
public:
    Basic_Timers(uint8_t timNum = 6);
    static Basic_Timers* pThis[2];
    static uint8_t pThis_Number;    
    
    //! --------------------------- Timer for checking is luggage in a tunnel ---------------------


private:
    void init(uint8_t timNum);    
    uint8_t currentNumber = 0;
};



#endif //TIMER_H
