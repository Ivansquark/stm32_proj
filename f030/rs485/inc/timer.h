#ifndef TIMER_H
#define TIMER_H

#include "main.h"
#include "modbus.h"

namespace Timer {

// main timer for 1 ms
void timer3_init();
void delay_ms(uint32_t ms);

// callback on timers
//void (*fPtr)();

// timer for 500 ms
void startPowerOnTimer(uint16_t ms);
void stopPowerOnTimer();
void setPowerOnTimeout(void (*fPtr)());

// timer for 100 ms
void startPowerOffTimer(uint16_t ms);
void stopPowerOffTimer();
void setPowerOffTimeout(void (*fPtr)());

bool getInBackward();
bool getInForward();

constexpr uint8_t ANTIBOUNCE_COUNTER = 10; // ms

// modbus timer for 100 us
void timer16_init();

};

#endif //TIMER_H
