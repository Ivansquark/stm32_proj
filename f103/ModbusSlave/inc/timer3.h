#ifndef TIMER3_H_
#define TIMER3_H_

#include "main.h"

class TIMER3
{
public:
    TIMER3()
    {
        timer3_ini();
    }
private:
    void timer3_ini()
    {
        RCC->APB1ENR|=RCC_APB1ENR_TIM3EN; //тактирование на таймер  ("НА ТАЙМЕР ЧАСТОТА ПРИХОДИТ БОЛЬШАЯ В ДВА РАЗА")
        TIM3->PSC=36000; //0  //делить частоту шины apb1(36MHz*2 при SysClk -72MHz) на 36 => частота 2kHz 
        TIM3->ARR=1000-1; //считаем до 1000 => прерывание раз в 500 мс  period 
	    
        TIM3->CR1|=TIM_CR1_ARPE;  // задействуем регистр auto reload
	    TIM3->DIER|=TIM_DIER_UIE; //включаем прерывание по таймеру -   1: Update interrupt enabled.
        TIM3->CR1|=TIM_CR1_CEN; //включаем таймер
	    NVIC_EnableIRQ(TIM3_IRQn); //включаем обработку прерывания по таймеру 3
    }
};


#endif //TIMER3_H_