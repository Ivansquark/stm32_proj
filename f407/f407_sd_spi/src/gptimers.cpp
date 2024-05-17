#include "gptimers.h"

bool GpTimer::timFlag = 0;

GpTimer::GpTimer(uint8_t timNum){
    init(timNum);
}

void GpTimer::init(uint8_t tim) {
    switch(tim) {
        case 2: {
            RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;
            TIM2->PSC = 840;// TIM clk = 84000000  (clk APB1*2) => 100 kHz
            TIM2->ARR = 50000-1; //500 ms
            TIM2->DIER|=TIM_DIER_UIE; //interrupt at overload 
            TIM2->CR1|=TIM_CR1_CEN;
            NVIC_EnableIRQ(TIM2_IRQn); //irq enable
        } break;
        case 3: {
            RCC->APB1ENR|=RCC_APB1ENR_TIM3EN;
            TIM3->PSC = 42000;// TIM clk = 84000000  (clk APB1*2) => 1 MHz
            TIM3->ARR = 1000-1; //500 ms
            TIM3->DIER|=TIM_DIER_UIE; //interrupt at overload 
            TIM3->CR1|=TIM_CR1_CEN;
            NVIC_EnableIRQ(TIM3_IRQn); //irq enable
        } break;
        case 4: {
            RCC->APB1ENR|=RCC_APB1ENR_TIM4EN;
            TIM4->PSC = 42000;// TIM clk = 84000000  (clk APB1*2) => 1 MHz
            TIM4->ARR = 2000-1; //1 s
            TIM4->DIER|=TIM_DIER_UIE; //interrupt at overload 
            TIM4->CR1|=TIM_CR1_CEN;
            NVIC_EnableIRQ(TIM4_IRQn); //irq enable
        }
        break;
        default:break;
    }        
}

//500 ms
extern "C" void TIM2_IRQHandler(void) {
	TIM2->SR &=~ TIM_SR_UIF; 
	GpTimer::timFlag = true; 
	NVIC_ClearPendingIRQ(TIM4_IRQn); 
}
extern "C" void TIM3_IRQHandler(void) {
	TIM3->SR &=~ TIM_SR_UIF;	
	NVIC_ClearPendingIRQ(TIM4_IRQn); 
}
extern "C" void TIM4_IRQHandler(void) {
	TIM4->SR &=~ TIM_SR_UIF;	 
	NVIC_ClearPendingIRQ(TIM4_IRQn); 
}