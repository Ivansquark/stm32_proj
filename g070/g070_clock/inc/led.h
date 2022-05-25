#ifndef LED_H
#define LED_H

#include "stm32g070xx.h"
#include "scheduler.h"

//! total time for one bit is 1.25 us => PWM frequency=800 kHz
//! if 0.35s high and 0.9s low => ZERO code
//! if 0.9s high and 0.35s low => ONE code
#define DELAY_LEN 48  // wait delay 48 times with zeroes (need 50 us between packets datasheet)
#define LED_COUNT 8   // numbers of leds
#define MAX_PWM_COUNT 79 //PWM - 100%
#define ZERO_DUTY  21  //for 1.25 ARR=80 => for 0.35 ARR=0.35*30/1.25=22.4 (from 0 to 21 = 22)
#define ONE_DUTY  59     //for 1.25 ARR=80 => for 0.9 ARR=0.9*30/1.25=60 (from 0 to 59 = 60)
#define TOTAL_BITS 24
#define ARRAY_LEN   (DELAY_LEN + LED_COUNT*TOTAL_BITS)
//--------------------------------------------------
#define BitIsSet(reg, bit) ((reg & (1<<bit)) != 0)
//--------------------------------------------------


void led_init();
void dma_start();
void set_diode_color(uint8_t r, uint8_t g, uint8_t b, uint8_t diode_num);
void set_all_diods_color(uint8_t r, uint8_t g, uint8_t b);
void diode_blink1();

#endif //LED_H