#include "adc.h"

uint16_t adc_data = 0;

uint16_t adc_read() {
    ADC1->CR |= ADC_CR_ADSTART;
    uint32_t timeout = 0xFFFF;
    while(ADC1->ISR & ADC_ISR_EOC && timeout--);
    adc_data = ADC1->DR;
    return adc_data;
}

void adc_init() {
    //! ADC GPIO init PA0 - FOTO resistor
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODE0; //1:1 - analog mode
    //!
       
    // ADC on software start init
    RCC->APBENR2 |= RCC_APBENR2_ADCEN;
    ADC->CCR |= ADC_CCR_PRESC_0;
    ADC1->CR &=~ ADC_CR_ADEN;
    ADC1->CR |= ADC_CR_ADVREGEN;
    //ADC1->CFGR2 &=~ ADC_CFGR2_CKMODE_1;
    ADC1->CFGR2 |= ADC_CFGR2_CKMODE; //1:1 Synchronious clock mode PCLK 
    ADC1->CFGR1 |= ADC_CFGR1_DISCEN; //discontinuous mode 
    ADC1->SMPR |= ADC_SMPR_SMP1; //1:1:1 max sample time 160.5 cycles
    ADC1->SMPR &=~ ADC_SMPR_SMPSEL0; //channel 16 use SMP1 sampling time
    ADC1->CHSELR |= ADC_CHSELR_CHSEL0; // select 0 channel
    //! ADC calibration    
    for(volatile int i = 0; i<1000;i++);
    ADC1->CR |= ADC_CR_ADCAL; 
    
    while (!(ADC1->ISR & ADC_ISR_EOCAL)) {}
    ADC1->CR |= ADC_CR_ADEN;
    ADC1->CALFACT = ADC1->DR;
}

void set_auto_brigtness() {
    if(flash_data.auto_brightness) {
        uint16_t adc_data = adc_read();
        uint16_t red = flash_data.red;
        uint16_t green = flash_data.green;
        uint16_t blue = flash_data.blue;
        uint16_t adc_percantage = (100 * adc_data) / MAX_ADC; // percentage of ADC value
        set_all_diods_color((red*adc_percantage)/100,(green*adc_percantage)/100,(blue*adc_percantage)/100);
    } else {
        set_all_diods_color(flash_data.red,flash_data.green,flash_data.blue); 
    }
}
