#ifndef GPTIMERS_H_
#define GPTIMERS_H_

#include "stm32f4xx.h"

class GpTimer
{
public:
    GpTimer(uint8_t timNum = 2);
    static bool timFlag;
private:
    void init(uint8_t tim);
};

extern "C" void TIM2_IRQHandler(void); //500 ms
extern "C" void TIM3_IRQHandler(void);
extern "C" void TIM4_IRQHandler(void);

#endif //GPTIMERS_H_