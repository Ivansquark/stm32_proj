#ifndef RCC_H
#define RCC_H

#include <stdint.h>
#include "stm32f4xx.h"

class Rcc {
public:
    Rcc(int freq = 8);
private:
    void rcc_init(int freq);    
};



#endif //RCC_H
