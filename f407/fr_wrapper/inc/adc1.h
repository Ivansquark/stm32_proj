#ifndef ADC1_H
#define ADC1_H_

#include "main.h"

class Adc1
{
public:
    Adc1(uint8_t x=0):adc_num(x)
    {adc1_ini(x);}
    uint16_t readADC()
    {
        ADC1->CR2|=ADC_CR2_SWSTART; // by software
        while(!(ADC1->SR&ADC_SR_EOC)){}
        return ADC1->DR;
    }
    float getTemp()
    {
        if(adc_num==0){return 0.0F;}        
        else if(adc_num==16)
        {
            float x = 25.0F + ((float)(readADC())*3.0F/4095.F - 0.76F)/0.0025F;
            return x;
        } 
        return 1.0F;
    }    
private:
    uint8_t adc_num;
    void adc1_ini(uint8_t numChannel)
    {
        switch(numChannel)
        {
            // PA0
            case 0:
            // //////////////// PIN definition //////////////////
            RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;
            GPIOA->MODER|=GPIO_MODER_MODER0; // 1:1 - analog mode
            //---------------------------------------------------
            RCC->APB2ENR|=RCC_APB2ENR_ADC1EN;
            ADC1->CR2|=ADC_CR2_SWSTART; // by software
            ADC1->CR2&=~ADC_CR2_CONT; // by single conversion mode
            ADC1->SMPR2&=~ADC_SMPR2_SMP0;  // 000 - 3 cycles
            ADC1->SQR3 &=~ADC_SQR3_SQ1; // 00000 - 0 chanel 1-conversion
            ADC1->CR2|=ADC_CR2_ADON; //ADC on
            break;
            // Temp Sensor
            case 16: 
            RCC->APB2ENR|=RCC_APB2ENR_ADC1EN;
            ADC->CCR|=ADC_CCR_TSVREFE; //for temp sens
            ADC1->CR2|=ADC_CR2_SWSTART; // by software
            ADC1->CR2&=~ADC_CR2_CONT; // by single conversion mode
            ADC1->SMPR1|=ADC_SMPR1_SMP16;  // 000 - 3 cycles
            ADC1->SQR3 |=ADC_SQR3_SQ1_4; // 10000 - 16 chanel 1-conversion
            ADC1->CR2|=ADC_CR2_ADON; //ADC on
            break;
            default:break;
        }        
    }
};
#endif //ADC1_H_