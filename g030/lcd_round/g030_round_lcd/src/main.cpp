#include "main.h"
#include "delay.h"

#include "math.h"
//extern "C" void _exit(void) { while(1);}

constexpr uint8_t R = 10; //radius of magnet rotation in mm
constexpr uint8_t C = 10; // from sensor to pole in mm
constexpr uint32_t a = R*R + (C+R)*(C+R); //L^2 = x^2 + (C+R+y)^2  sin(a)=x/R
constexpr uint32_t b = 2*R*(C+R);         //l^2 = x^2 - (C+R+y)^2  cos(a)=x/R
constexpr float PI = 3.141;                // ADC~F*(1/l^2 - 1/L^2)

int main(void) {
    delay(50);
    while(1) {

    }
    return 0;
}



void sun() {
    asm("nop");
}