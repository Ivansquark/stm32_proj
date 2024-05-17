#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f10x.h"
#include "usart1.h"
#include "rcc.h"
#include "core_cm3.h"
#include "spi2.h"
#include "spi1.h"
#include "i2c1.h"


#define LED_on GPIOC->ODR|=GPIO_ODR_ODR13
#define LED_off GPIOC->ODR&=~GPIO_ODR_ODR13

void gpioc_ini();
void delay(uint32_t x);


//***************************************************

void gpioc_ini()
{
	RCC->APB2ENR|=RCC_APB2ENR_IOPCEN;
	GPIOC->CRH&=~GPIO_CRH_CNF13;
	GPIOC->CRH|=GPIO_CRH_MODE13;//50MHz
	//GPIOC->ODR=0;//GPIO_ODR_ODR13;	
}

void delay(volatile uint32_t x)
{
	//while(x>0)
	//{x--;}
	for(uint32_t i=0;i<x;i++){}
}



#endif //MAIN_H
