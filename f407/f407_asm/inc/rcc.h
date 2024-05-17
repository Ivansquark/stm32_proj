#ifndef RCC_H
#define RCC_H

#include "stm32f4xx.h"

class Rcc {
public:
    Rcc(uint8_t quarz = 8);
private:
    void init(uint8_t quarz);
};

#endif //RCC_H