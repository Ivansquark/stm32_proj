#ifndef BUTTON_H
#define BUTTON_H

//#include "can.h"
#include "stm32f0xx_hal.h"

extern "C" void EXTI4_15_IRQHandler(void);
extern "C" void TIM2_IRQHandler(void);
extern "C" void TIM3_IRQHandler(void);

enum class ButtonState {
    NOT_PRESSED = 0,
    UP,
    DOWN,
    UP_LONG,
    DOWN_LONG,
    UP_DOWN
};

class Button {
public:
    Button();
    static Button* pThis;
    ButtonState butState = ButtonState::NOT_PRESSED;
private:
    void init();
};

#endif //BUTTON_H