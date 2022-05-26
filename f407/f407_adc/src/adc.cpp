#include "adc.h"

Adc* Adc::pThis = nullptr;

Adc::Adc() {
    //ADC_init();
    init_injected_channels();
    pThis = this;
}

void Adc::read_diods_ADC() {
    //ADC_Diode1 = moving_average_diode1(read_diode1());    
    //ADC_Diode2 = moving_average_diode2(read_diode2());
    ADC_Diode1 = moving_average_diode1(Dma::pThis->adc1_data[0]);    
    ADC_Diode2 = moving_average_diode2(Dma::pThis->adc1_data[1]);
    //float k_dx = 333333.3; 
    float x = log((ADC_Diode2 * k_ln) / ADC_Diode1);
    //float K1 = 2.5;  
		//kV=0.123;
    kV = 1.0F/(pow(x/k_dx,1/K_1)); 
}

void Adc::read_battery_ADC() {
    for(int i = 0; i<1; i++) { // read straight 16 times
        ADC_Battery = moving_average_battery(read_battery());
    }
    Battery_V = ADC_Battery * K_battery;
}

uint16_t Adc::read_diode1() {
    ADC1->SQR3 = 0; // 1 conversion in channel 0
    ADC1->CR2 |= ADC_CR2_SWSTART;
    uint32_t timeout = 0xFFFFF;
    while(!(ADC1->SR & ADC_SR_EOC) && timeout--);        
    return ADC1->DR;
}
uint16_t Adc::read_diode2() {
    ADC1->SQR3 = 1; // 1 conversion in channel 1
    ADC1->CR2 |= ADC_CR2_SWSTART;
    uint32_t timeout = 0xFFFFF;
    while(!(ADC1->SR & ADC_SR_EOC) && timeout--);
    return ADC1->DR;
}
uint16_t Adc::read_battery() {
    //ADC2->SQR3 = 9; // 1 conversion in channel 9
    ADC2->CR2 |= ADC_CR2_SWSTART;
    uint16_t timeout = 0xFFFF;
    while(!(ADC2->SR & ADC_SR_EOC) && timeout ) {
        timeout--;
    }        
    return ADC2->DR;
}

uint16_t Adc::moving_average_diode1(uint16_t val) {
    MovAverSum_diode1 -= arr_diode1[Index_mov_aver_diode1];
    MovAverSum_diode1 += val;
    arr_diode1[Index_mov_aver_diode1] = val;
    Index_mov_aver_diode1++;
    if(Index_mov_aver_diode1 == MovAverLength){Index_mov_aver_diode1=0;}    
    return MovAverSum_diode1>>5; // (if MovAverLength=32) common case:log2(MovAverLength)
}

uint16_t Adc::moving_average_diode2(uint16_t val) {
    MovAverSum_diode2 -= arr_diode2[Index_mov_aver_diode2];
    MovAverSum_diode2 += val;
    arr_diode2[Index_mov_aver_diode2] = val;
    Index_mov_aver_diode2++;
    if(Index_mov_aver_diode2 == MovAverLength){Index_mov_aver_diode2=0;}    
    return MovAverSum_diode2>>5; // (if MovAverLength=32) common case:log2(MovAverLength)
}

uint16_t Adc::moving_average_battery(uint16_t val) {
    MovAverSum_battery -= arr_battery[Index_mov_aver_battery];
    MovAverSum_battery += val;
    arr_battery[Index_mov_aver_battery] = val;
    Index_mov_aver_battery++;
    if(Index_mov_aver_battery == MovAverLength){Index_mov_aver_battery=0;}    
    return MovAverSum_battery>>5; // (if MovAverLength=32) common case:log2(MovAverLength)
}

void Adc::set_Coefficients() {
    //Flash::pThis->read_buf(&kef, 12);
    At24c::pThis->readBytes(0, &kef, 12);
    K_1 = kef.K_1;
    k_dx = kef.k_dx;
    k_ln = kef.k_ln; 
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
void Adc::ADC_init() {
    //! PA0-ADC1_IN0, PA1-ADC1_IN1 
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; 
    GPIOA->MODER |= (GPIO_MODER_MODE0 | GPIO_MODER_MODE1);
    
    ADC->CCR &=~ ADC_CCR_ADCPRE; // 0:0 PCLK2/2 = 24MHz
    RCC->APB2ENR|=RCC_APB2ENR_ADC1EN; //clock adc1	    
	
	ADC1->CR2 &=~ ADC_CR2_CONT; //0-single conversion,                  1 - continious conversion	
	ADC1->SMPR2|=ADC_SMPR2_SMP0; // 111:480 cycles;
    ADC1->SMPR2|=ADC_SMPR2_SMP1; // 111:480 cycles;
    //ADC1->SQR3 =0; // 1 conversion
    //ADC1->SQR3 =0; // 1 conversion
    ADC1->SQR3 = 0; // 1 conversion in channel 0
    ADC1->SQR3 = 1; // 1 conversion in channel 1
	ADC1->CR2 &=~ ADC_CR2_EXTEN; // on software start 
    ADC1->CR2 |= ADC_CR2_EOCS; // The EOC bit is set at the end of each regular conversion
    ADC1->CR2|=ADC_CR2_ADON; //enable ADC
    
	//ADC1->SQR1&=~ADC_SQR1_L; // 0000 - 1 conversion on a channels
	//ADC1->SQR3|=ADC_SQR3_SQ1_4; //10000 - 16 channel on 1 conversion 
	//ADC1->SQR3&=~ADC_SQR3_SQ1_3;ADC1->SQR3&=~ADC_SQR3_SQ1_2;ADC1->SQR3&=~ADC_SQR3_SQ1_1;ADC1->SQR3&=~ADC_SQR3_SQ1_0;
    
    // ADC2 PB1
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; 
    GPIOB->MODER |= (GPIO_MODER_MODE1);
    RCC->APB2ENR|=RCC_APB2ENR_ADC2EN; //clock adc1	 
    
	ADC2->CR2&=~ADC_CR2_CONT; //0-single conversion,                  1 - continious conversion	
	//ADC2->CR2|=ADC_CR2_SWSTART; //event start conversion from software SWSTART
    ADC2->SMPR2|=ADC_SMPR2_SMP9; // 111:480 cycles;
    ADC2->SQR3 = 9; // 1 conversion in channel 9   
	ADC2->CR2 &=~ ADC_CR2_EXTEN; // on software start 
    ADC2->CR2 |= ADC_CR2_EOCS; // The EOC bit is set at the end of each regular conversion
    ADC2->CR2|=ADC_CR2_ADON; //enable ADC
    
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // BatVoltKey 
    GPIOE->MODER |= (GPIO_MODER_MODE8_0 );
		GPIOE->MODER &=~ GPIO_MODER_MODE8_1;
		
    set_Coefficients();		
}

//________________________________________________________________
//_____________________ ADC_ INJECTED CHANNELS ___________________
//_________________________________________________________________
void Adc::init_injected_channels() {
    //! PA0-ADC1_IN0, PA1-ADC1_IN1 
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; 
    GPIOA->MODER |= (GPIO_MODER_MODE0 | GPIO_MODER_MODE1); // analog input    
    ADC->CCR &=~ ADC_CCR_ADCPRE; // 0:0 PCLK2/2 = 24MHz
    RCC->APB2ENR|=RCC_APB2ENR_ADC1EN; //clock adc1	    
	
    ADC1->CR1 |= ADC_CR1_SCAN; // scan mode for sequence of channels (multichannel mode)
	ADC1->CR2 &=~ ADC_CR2_CONT; //1-continuous conversion (for regular channels),   0 -single conversion	
	ADC1->SMPR2|=ADC_SMPR2_SMP0; // 111:480 cycles;
    ADC1->SMPR2|=ADC_SMPR2_SMP1; // 111:480 cycles;
    //ADC1->SQR3 =0; // 1 conversion
    //ADC1->SQR3 =0; // 1 conversion
    //ADC1->SQR3 = 0; // 1 conversion in channel 0
    //ADC1->SQR3 = 1; // 1 conversion in channel 1
    
    ADC1->JSQR |= ADC_JSQR_JL_0; //0:1  - 2 conversion
    ADC1->JSQR |= ((0<<10)|(1<<15));  //channel_0 and channel_1 sequence conversion
    
	//ADC1->CR2 &=~ ADC_CR2_EXTEN; // on software start 
    ADC1->CR2 |= ADC_CR2_JEXTEN_0;// 0:1 external trigger on rising edge
    ADC1->CR2 &=~ ADC_CR2_JEXTSEL;
    ADC1->CR2 |= ADC_CR2_JEXTSEL_2; // start conversion on timer 3 update
    //ADC1->CR2 |= ADC_CR2_EOCS; // The EOC bit is set at the end of each regular conversion
    ADC1->CR2 |= ADC_CR2_DMA; // DMA enabled
    ADC1->CR2 |= ADC_CR2_DDS; // New DMA requests are issued
    ADC1->CR2|=ADC_CR2_ADON; //enable ADC
    
	//ADC1->SQR1&=~ADC_SQR1_L; // 0000 - 1 conversion on a channels
	//ADC1->SQR3|=ADC_SQR3_SQ1_4; //10000 - 16 channel on 1 conversion 
	//ADC1->SQR3&=~ADC_SQR3_SQ1_3;ADC1->SQR3&=~ADC_SQR3_SQ1_2;ADC1->SQR3&=~ADC_SQR3_SQ1_1;ADC1->SQR3&=~ADC_SQR3_SQ1_0;
    
    // ADC2 PB1
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; 
    GPIOB->MODER |= (GPIO_MODER_MODE1);
    RCC->APB2ENR|=RCC_APB2ENR_ADC2EN; //clock adc1	 
    
	ADC2->CR2&=~ADC_CR2_CONT; //0-single conversion,                  1 - continious conversion	
	//ADC2->CR2|=ADC_CR2_SWSTART; //event start conversion from software SWSTART
    ADC2->SMPR2|=ADC_SMPR2_SMP9; // 111:480 cycles;
    ADC2->SQR3 = 9; // 1 conversion in channel 9   
	ADC2->CR2 &=~ ADC_CR2_EXTEN; // on software start 
    ADC2->CR2 |= ADC_CR2_EOCS; // The EOC bit is set at the end of each regular conversion
    ADC2->CR2|=ADC_CR2_ADON; //enable ADC
    
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // BatVoltKey 
    GPIOE->MODER |= (GPIO_MODER_MODE8_0 );
	GPIOE->MODER &=~ GPIO_MODER_MODE8_1;
		
    set_Coefficients();			    
}