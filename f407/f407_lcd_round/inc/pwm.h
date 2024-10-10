#ifndef PWM_H_
#define PWM_H_

#include "main.h"

class Pwm
{
public:
    Pwm();
    static Pwm* pThis;
    void pwm1_start();
    void pwm2_start();
    void pwm_start();
    void pwm_stop();
    bool Pwm1Started = false;
    bool Pwm2Started = false;
    bool pwmIRQ = false;
private:
    void init();
};

#endif //PWM_H_
