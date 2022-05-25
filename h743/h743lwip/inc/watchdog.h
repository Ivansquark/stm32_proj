#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "stm32h743xx.h"

class Independed_WatchDog {
public:
    Independed_WatchDog(uint32_t time = 0xFFF) {
        WD_init(time);
    }
    inline void refreshCounter() {
        IWDG1->KR = 0x0000AAAA; //Refreshes counter value
    }
private:
    inline void WD_init(uint32_t time) {
        IWDG1->KR = 0x0000cccc; // starts counts from FFF when 0 => IWDG reset
        IWDG1->KR = 0x00005555; //Enable register access
        IWDG1->PR = 7;
        IWDG1->RLR = time; //set max value 
        while(IWDG1->SR != 0);
        IWDG1->KR = 0x0000AAAA; //Refreshes counter value
        // need to call method refreshCounter() in counter or main cycle to refresh WatchDog counter        
    }
};

#endif //WATCHDOG_H