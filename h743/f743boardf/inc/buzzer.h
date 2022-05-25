#ifndef BUZZER_H
#define BUZZER_H

#include "stm32h743xx.h"

class Buzzer {
public:
    Buzzer(){buzzer_init();}
    void beep() {
        int x=10000;
        GPIOB->ODR ^= GPIO_ODR_OD9;
        while(x--);
    }
    inline void off() {GPIOB->ODR &=~ GPIO_ODR_OD9;}
private:
    void buzzer_init(){
        RCC->AHB4ENR|=RCC_AHB4ENR_GPIOBEN;
	    GPIOB->MODER|=GPIO_MODER_MODE9_0; GPIOB->MODER&=~GPIO_MODER_MODE9_1; //0:1 output
	    GPIOB->OSPEEDR|=GPIO_OSPEEDR_OSPEED9;// 1:1 max speed        
    }    
};

#endif //BUZZER_H