#ifndef DMA_H
#define DMA_H

#include "stm32f407xx.h"

class Dma {
public: 
    Dma();
    static Dma* pThis;
    void init_adc();
    inline void adc1_enable(){DMA2_Stream0->CR |= DMA_SxCR_EN;}
    void deinit();
    uint16_t adc1_data[2] = {0};
private:
    
};


#endif //DMA_H