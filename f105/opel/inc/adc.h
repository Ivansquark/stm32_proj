#ifndef ADC_H
#define ADC_H

#include "main.h"


class Adc {
public: 
    Adc();
    uint32_t voltage = 0;
    void getVoltage();
    uint8_t voltageInteger = 0;
    uint8_t voltageFractional = 0;
private:
    void init();
    uint16_t read();
};

#endif //ADC_H