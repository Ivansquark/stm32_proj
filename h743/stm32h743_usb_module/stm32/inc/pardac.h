#ifndef PARDAC_H
#define PARDAC_H

#include "stm32h743xx.h"

class ParDac {
public:
    ParDac();
    inline void set_dac127(uint8_t val){GPIOE->ODR = val;}
private:
    void pardac_init();
};

#endif //PARDAC_H