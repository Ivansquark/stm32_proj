#include "adc.h"

Adc* Adc::pThis = nullptr;

Adc::Adc() {
    init();
    pThis = this;
}

uint16_t Adc::read() {
    ADC1->CR |= ADC_CR_ADSTART;
    uint32_t timeout = 0xFFFF;
    while(ADC1->ISR & ADC_ISR_EOC && timeout--);
    data = ADC1->DR;
    return data;
}

void Adc::init() {
    //! ADC GPIO init
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODE12; //1:1 - analog mode

    // ADC on software start init
    RCC->APBENR2 |= RCC_APBENR2_ADCEN;
    ADC1->CR |= ADC_CR_ADVREGEN;
    ADC1->CFGR1 |= ADC_CFGR1_DISCEN; //discontinuous mode 
    ADC1->SMPR |= ADC_SMPR_SMP1; //1:1:1 max sample time 160.5 cycles
    ADC1->SMPR &=~ ADC_SMPR_SMPSEL16; //channel 16 use SMP1 sampling time
    ADC1->CHSELR |= ADC_CHSELR_CHSEL16; // select 16 channel
        
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL) {}
    ADC1->CR |= ADC_CR_ADEN;
    ADC1->CALFACT = ADC1->DR;
    // timer when ADC is working
    RCC->APBENR1 |= RCC_APBENR1_TIM3EN;
    TIM3->PSC = 16; // 1MHz
    TIM3->ARR |= 1000-1; // 1 kHz  (1 ms period)
    TIM3->DIER |= TIM_DIER_UIE; //update interrupt enable
    TIM3->CR1 |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM3_IRQn);
}

extern "C" void TIM3_IRQHandler(void) {
    TIM3->SR &=~ TIM_SR_UIF; //clear interrupt flag
    Adc::pThis->read();
}