#ifndef FLASH_H
#define FLASH_H

#include "stm32f0xx_hal.h"

class Flash {
public:
    static bool setRedoutProtectionToOne();
private:    
};

#endif //FLASH_H