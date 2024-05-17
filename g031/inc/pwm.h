#ifndef PWM_H
#define PWM_H

#include "stm32g0xx.h"

class PWM {
public:
    PWM();
    static PWM* pThis;
    void setDuty(uint16_t duty);
    uint8_t duty=0;
private:
    void init();
};

#endif //PWM_H