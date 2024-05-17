#include "led.h"

uint16_t BUF_DMA [ARRAY_LEN] = {ZERO_DUTY};

void dma_start() {
    TIM16->CR1 |= TIM_CR1_CEN;
    DMA1_Channel1->CCR |= DMA_CCR_EN; // enable DMA
    //TIM16->DMAR = 13*4;
}

void led_init() {
    for(int i = 0; i<DELAY_LEN; i++) {
        BUF_DMA[i] = 0; 
    }
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER &=~ GPIO_MODER_MODE8;
    GPIOB->MODER |= GPIO_MODER_MODE8_1; //1:0 => alternative
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED8; //1:1 very high speed
    GPIOB->AFR[1] = (2<<0); // AF2 on PB8
    
    //! TIM16 channel 1 PWM initialization:
    RCC->APBENR2 |= RCC_APBENR2_TIM16EN;
    TIM16->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1);
    TIM16->CCMR1 &=~ (TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_3); // 0:1:1:0 - PWM mode 1 (active as CNT<CCR1)
    TIM16->CCER |= TIM_CCER_CC1E; // channel 1 enabled
    TIM16->CR1 |= TIM_CR1_ARPE; // Preload enable:
    TIM16->PSC = 0; // clk = 64MHz/(PSC+1)  ==  64 MHz
    TIM16->ARR =  MAX_PWM_COUNT; // for 800 kHz
    TIM16->CCR1 = 0;//TIM16->ARR/2; // duty cycle    
    TIM16->BDTR |= TIM_BDTR_MOE;    
    TIM16->DIER |= TIM_DIER_UDE; // update DMA request enable
    //TIM16->DCR = 0; //clear bits 0 in DBL - 1 transfer
    //! set register that will be changed on update ____ TIM DMA BURST MODE______________
    TIM16->DCR = TIM_DCR_DBA_0 | TIM_DCR_DBA_2 | TIM_DCR_DBA_3; //DBA[4:0]=01101 = 13 = start from CCR1
    //TIM16->DMAR = 13*4;
    TIM16->EGR |= TIM_EGR_UG; //Force update generation (UG = 1)
    TIM16->CR1 |= TIM_CR1_CEN; // counter enabled
    //! DMA init DMA_MUX = 46 for TIM16_up
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel1->CCR &=~ DMA_CCR_MSIZE;
    DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0; // 0:1  16 bits (for uint16_t)
    DMA1_Channel1->CCR &=~ DMA_CCR_PSIZE;
    DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0; // 0:1  16 bits (for PWM ARR)
    DMA1_Channel1->CCR |= DMA_CCR_MINC; // memory increment enable
    DMA1_Channel1->CCR |= DMA_CCR_DIR; // 1 - read from memory
    DMA1_Channel1->CCR |= DMA_CCR_TCIE; // transfer complete interrupt enable
    DMA1_Channel1->CCR |= DMA_CCR_CIRC;
    DMA1_Channel1->CNDTR = ARRAY_LEN; //8 leds for 24 bits + delay
    DMA1_Channel1->CPAR = (uint32_t)&TIM16->DMAR;
    DMA1_Channel1->CMAR = (uint32_t) BUF_DMA;
    DMAMUX1_Channel0->CCR |= 46;
    // DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

void set_diode_color(uint8_t r, uint8_t g, uint8_t b, uint8_t diode_num) {
    //for(int i = DELAY_LEN;i<sizeof(BUF_DMA);i++) {
    //    BUF_DMA[i] = ZERO_DUTY;
    //}
    for(volatile int i=0; i<8; i++) {
        if(BitIsSet(r,(7-i))) {
            BUF_DMA[DELAY_LEN + diode_num*24 + i+8] = ONE_DUTY;
        } else {
            BUF_DMA[DELAY_LEN + diode_num*24 + i+8] = ZERO_DUTY;
        }
        if(BitIsSet(g,(7-i))) {
            BUF_DMA[DELAY_LEN + diode_num*24 + i+0] = ONE_DUTY;
        } else {
            BUF_DMA[DELAY_LEN + diode_num*24 + i+0] = ZERO_DUTY;
        }
        if(BitIsSet(b,(7-i))) {
            BUF_DMA[DELAY_LEN + diode_num*24 + i+16] = ONE_DUTY;
        } else {
            BUF_DMA[DELAY_LEN + diode_num*24 + i+16] = ZERO_DUTY;
        }
    }
}
void set_all_diods_color(uint8_t r, uint8_t g, uint8_t b) {
    for(int i =0; i<8; i++) {
        set_diode_color(r,g,b,i);
    }    
    dma_start();
}

void diode_blink1() {
    for(int i=0;i<8;i++) {
        set_diode_color(5,0,0,i);
        dma_start();
        delay_us(10);
        set_diode_color(0,0,1,i);
        dma_start();
        delay_us(2);
    }      
}

void DMA1_Channel1_IRQHandler(void) {
  // clear interrupt flag
  if(DMA1->ISR & DMA_ISR_TCIF1) {
    DMA1->IFCR |= DMA_IFCR_CTCIF1;    
  }
    DMA1_Channel1->CCR &=~ DMA_CCR_EN; // disable DMA
    TIM16->CR1 &=~ TIM_CR1_CEN; //stop timer after each update
}