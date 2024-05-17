#ifndef ADC_H
#define ADC_H

#include "stm32g0xx.h"

class Adc {
public:
    Adc();
    static Adc* pThis;
    uint16_t read();
    uint16_t data=0;
private:
    void init();
};

#endif //ADC_H