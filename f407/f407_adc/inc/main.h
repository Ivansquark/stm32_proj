#ifndef MAIN_H
#define MAIN_H

#include "stm32f4xx.h"
#include "rcc.h"
#include "disp1602.h"
#include "adc.h"
#include "timer.h"
#include "usb_device.h"
#include "button.h"
#include "flash.h"
#include "uart.h"
#include "i2c.h"
#include "sleep.h"

void gpio_init(void);
void delay(uint32_t x);

#endif //MAIN_H
