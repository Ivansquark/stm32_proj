//#include "main.h"
//#include "gpio.h"
//#include "my_delay.h"
//#include "global_variables.h"
#include "sed1335-stm32.h"


#define CD_data		(GPIOA->BSRR |= GPIO_BSRR_BR7)
#define CD_cmd		(GPIOA->BSRR |= GPIO_BSRR_BS7)
#define WR_on		(GPIOA->BSRR |= GPIO_BSRR_BR6)
#define WR_off		(GPIOA->BSRR |= GPIO_BSRR_BS6)
#define RST_off		(GPIOA->BSRR |= GPIO_BSRR_BS1)
#define RST_on		(GPIOA->BSRR |= GPIO_BSRR_BR1)

void lcd_gpio_init() {
	//! PB0-PB7 data  PA1_RST PA4_CS1 PA5_CS2 PA6_WR PA7_D/C
	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	GPIOB->MODER &=~ (GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3 |
					  GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
	GPIOB->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 |
					GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0); //output
	GPIOB->PUPDR = (GPIO_PUPDR_PUPD0_0 | GPIO_PUPDR_PUPD1_0 | GPIO_PUPDR_PUPD2_0 | GPIO_PUPDR_PUPD3_0 |
					GPIO_PUPDR_PUPD4_0 | GPIO_PUPDR_PUPD5_0 | GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0); //pull_up
	GPIOA->MODER &=~ (GPIO_MODER_MODE1 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7 |
					 GPIO_MODER_MODE4 | GPIO_MODER_MODE5);
	GPIOA->MODER |= (GPIO_MODER_MODE1_0 | GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0 |
					 GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0); //output
	GPIOA->PUPDR = (GPIO_PUPDR_PUPD1_0 | GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0 | GPIO_PUPDR_PUPD4_0 |
					GPIO_PUPDR_PUPD5_0); //pull_up
	GPIOB->ODR |= 0xFF;
}

void MY_LCD_DATA_Write(uint8_t data) {
	GPIOB->ODR &= 0xFF00;
	GPIOB->ODR |= data;
}

void GLCD_InitializePorts_(void) {
	CD_cmd;
	WR_off;
	RST_off;
}

void GLCD_HardReset_(void) {
	RST_on;
	delay_ms(1);
	RST_off;
}

void GLCD_WriteData_(unsigned char dataToWrite) {	
	CD_data;
	MY_LCD_DATA_Write(dataToWrite);
	//delay_ms(1);	
	WR_on;
	delay_ms(1);
	WR_off;
	//CD_cmd;
	delay_ms(1);
}

void GLCD_WriteCommand_(unsigned char commandToWrite) {
	CD_cmd;
	MY_LCD_DATA_Write(commandToWrite);
	//delay_ms(1);
	WR_on;
	delay_ms(1);
	WR_off;
	delay_ms(1);
}	

void delay_ms(volatile uint32_t ms) {
	for(volatile int i = 0; i<ms*5; i++);
}

