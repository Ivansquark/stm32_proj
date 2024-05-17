#ifndef HRTIM_H
#define HRTIM_H

#include "stm32h743xx.h"

class Hrtim {
public:
    Hrtim();
    static Hrtim* pThis;
    void setDuty(uint16_t duty);

private:
    void init();


    static constexpr uint32_t Period = 4000; //100 kHz PWM frequency
};

#endif //HRTIM_H