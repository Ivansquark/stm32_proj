#ifndef ADC_H
#define ADC_H

#include "stm32g070xx.h"
#include "flash.h"
#include "led.h"

#define MAX_ADC 4000

uint16_t adc_read();
extern uint16_t adc_data;

void set_auto_brigtness();

void adc_init();

#endif //ADC_H