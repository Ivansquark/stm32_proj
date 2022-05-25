#ifndef ENCODER_H
#define ENCODER_H

#include "stm32h743xx.h"
#include "timer.h"

class Encoder {
public: 
    Encoder();
    static Encoder* pThis;
    bool But_PC3=false;
    bool But_PA3=false;
    bool But_PB5=false;
    bool But_PB7=false;
    bool But_PA8=false;
    bool But_PC9=false;
    uint16_t enc_counter=0;
    uint16_t enc1_counter=0;
    uint16_t enc2_counter=0;
    void SM_forward(uint16_t steps);
    void SM_backward(uint16_t steps);
    uint8_t enc1_state_old = 0;
    uint8_t enc1_state_new = 0;
    uint8_t enc2_state_old = 0;
    uint8_t enc2_state_new = 0;
    void enc1_counter_change();
    void enc2_counter_change();    
    
private:
    void encoder_init();
    void shag_init();
    void SM_set_1();
    void SM_set_2();
    void SM_set_3();
    void SM_set_4();
    void SM_step_forward();
    void SM_step_backward();
};


#endif //ENCODER_H