#ifndef SED1335_STM32_H
#define SED1335_STM32_H

#include "stm32g070xx.h"

void GLCD_InitializePorts_(void);
void GLCD_HardReset_(void);
void GLCD_WriteData_(unsigned char dataToWrite);
void GLCD_WriteCommand_(unsigned char commandToWrite);
void MY_LCD_DATA_Write(uint8_t data);
void lcd_gpio_init();
void delay_ms(volatile uint32_t ms);

#endif //SED1335_STM32_H