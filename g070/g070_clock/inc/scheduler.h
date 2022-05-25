#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "stm32g070xx.h"

extern volatile uint32_t ms_counter;
extern volatile uint32_t ms_delay_counter;
extern volatile uint32_t us_counter;
extern volatile uint32_t s_counter;

void scheduler_init();

void delay_us(uint32_t us);
void delay_ms_sys(uint32_t ms);


#endif //SCHEDULER_H