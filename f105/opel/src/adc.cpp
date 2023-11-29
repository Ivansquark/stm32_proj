#include "adc.h"

Adc::Adc() {
    init();
}

void Adc::getVoltage() {
    voltage =  (2120 * read()) / (4095);
    voltageInteger = voltage/100;
    voltageFractional = voltage % 100;
}

uint16_t Adc::read() {
    ADC1->CR2 &= ~ADC_CR2_SWSTART;
    ADC1->CR2 |= ADC_CR2_SWSTART;
    uint32_t timeout = 0xFFFF;
    while((ADC1->SR & ADC_SR_EOC) && timeout--);
    return ADC1->DR;
}
void Adc::init() {
    //! GPIO init
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    
    GPIOC->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->CFGR2 |= RCC_CFGR_ADCPRE_DIV2;
    uint32_t timeout = 0xFFFFFF;
    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->CR2 |= ADC_CR2_CAL;
    while((ADC1->CR2 & ADC_CR2_CAL) && timeout--){}
    ADC1->CR2 |= ADC_CR2_EXTSEL; //111 swstart
    ADC1->SMPR1 |= ADC_SMPR1_SMP10;
    ADC1->SQR3 |= 10; // channel10 one conversion
    
    ADC1->CR2 |= ADC_CR2_ADON;
}