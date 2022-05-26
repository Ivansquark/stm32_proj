#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f4xx.h"
#include "timer.h"
#include "disp1602.h"

class Button {
public:    
    Button();
static Button* pThis; 

private:
   void but_init();    
};

#endif //BUTTON_H