#include "dma.h"

Dma* Dma::pThis = nullptr;

Dma::Dma() {
    pThis = this;
}

void Dma::init_adc() {
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    DMA2_Stream0->CR &=~ DMA_SxCR_CHSEL; // DMA2 channel 0 (ADC1)
    DMA2_Stream0->CR &=~ DMA_SxCR_MBURST; //memory single transfer
    DMA2_Stream0->CR &=~ DMA_SxCR_PBURST; //peripheral single transfer
    DMA2_Stream0->CR |= DMA_SxCR_MINC; // Memory increment enabled
    DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0; // Memory 0:1 half-word (16-bit)
    DMA2_Stream0->CR |= DMA_SxCR_PSIZE_0; // Peripheral 0:1 half-word (16-bit)
    DMA2_Stream0->CR &=~ DMA_SxCR_DIR; // Direction 0:0 from peripheral to memory
    DMA2_Stream0->CR &=~ DMA_SxCR_TCIE; //Transfer complete interrupt enable  
    DMA2_Stream0->NDTR = 2; // Number of data items to transfer (2 adc JDR registers)
    DMA2_Stream0->PAR = (uint32_t)&ADC1->JDR1;
    DMA2_Stream0->M0AR = (uint32_t)adc1_data; //address of the first element
    adc1_enable();
}


void Dma::deinit() {
    DMA2_Stream0->CR &=~ DMA_SxCR_CHSEL; // DMA2 channel 0 (ADC1)
    DMA2_Stream0->CR &=~ DMA_SxCR_MBURST; //memory single transfer
    DMA2_Stream0->CR &=~ DMA_SxCR_PBURST; //peripheral single transfer
    DMA2_Stream0->CR &=~ DMA_SxCR_MSIZE; // 0:1 half-word (16-bit)
    DMA2_Stream0->CR &=~ DMA_SxCR_PSIZE; // 0:1 half-word (16-bit)
    DMA2_Stream0->NDTR = 0; // Number of data items to transfer
    
}

extern "C" void DMA2_Stream0_IRQHandler(void) {
    DMA2->LIFCR |= DMA_LIFCR_CFEIF0; // clear interrupt flag
    ADC1->SR &=~ ADC_SR_JEOC; //clear flag
    Dma::pThis->adc1_enable();
}