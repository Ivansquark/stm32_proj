#ifndef SLEEP_H
#define SLEEP_H

#include "stm32f407xx.h"

class Sleep {
public:    
    Sleep();
    static Sleep* pThis;
    void sleep();
private:
    void init();
    
};


#endif SLEEP_H