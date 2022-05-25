#ifndef BUTTONS_H
#define BUTTONS_H


//! @brief start first timer on 500 ms at timeout, if button still pressed then long time press, else short time press

#include "stm32g070xx.h"

extern uint8_t enter_pressed_flag;
extern uint8_t up_pressed_flag;
extern uint8_t down_pressed_flag;
extern uint8_t enter_short_pressed_flag;
extern uint8_t up_short_pressed_flag;
extern uint8_t down_short_pressed_flag;
extern uint8_t enter_long_pressed_flag;
extern uint8_t up_long_pressed_flag;
extern uint8_t down_long_pressed_flag;

void buttons_init();

#endif //BUTTONS_H