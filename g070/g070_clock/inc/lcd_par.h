#ifndef LCD_PAR_H
#define LCD_PAR_H

#include "stm32g070xx.h"
#include "figures.h"
#include "sed1335.h"

#define CS_1_on		(GPIOA->BSRR |= GPIO_BSRR_BR4)
#define CS_1_off	(GPIOA->BSRR |= GPIO_BSRR_BS4)
#define CS_2_on		(GPIOA->BSRR |= GPIO_BSRR_BR5)
#define CS_2_off	(GPIOA->BSRR |= GPIO_BSRR_BS5)
#define CS_1	CS_1_on; CS_2_off;	
#define CS_2	CS_2_on; CS_1_off;

void lcd_init();
void drawNum(uint8_t num, uint8_t reverse);
void drawNum_left(uint8_t num, uint8_t reverse);
void drawNum_right(uint8_t num, uint8_t reverse);
#endif //LCD_PAR_H