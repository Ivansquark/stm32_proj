#include "main.h"
#include "uart.h"
#include "adc.h"
#include "pwm.h"
#include "math.h"
//extern "C" void _exit(void) { while(1);}

constexpr uint8_t R = 10; //radius of magnet rotation in mm
constexpr uint8_t C = 10; // from sensor to pole in mm
constexpr uint32_t a = R*R + (C+R)*(C+R); //L^2 = x^2 + (C+R+y)^2  sin(a)=x/R
constexpr uint32_t b = 2*R*(C+R);         //l^2 = x^2 - (C+R+y)^2  cos(a)=x/R
constexpr float PI = 3.14;                // ADC~F*(1/l^2 - 1/L^2)

void delay(uint32_t x);

int main(void) {
    PWM pwm;
    Adc adc;
    UART uart;
    __enable_irq();
    uint8_t ADC_max = 2000;
    //uint32_t ADC_K = ADC_max*10;
    float temp_K = (float)a*a/(float)(b*b) - 1;
    float K = (float)(1/(ADC_max*temp_K));
    bool start_PC_communicate = false;
    while(1) {
        float numenator = (float)((a*a)*(K*adc.data));
        float denumenator = (float)((b*b)*(K*adc.data+1));
        float temp = (float)numenator/(float)denumenator;
        float COS = sqrt(temp);
        float ARCCOS = acos(COS);
        uint8_t angle_data = 180*ARCCOS/PI;
        pwm.setDuty((5455 + angle_data*323)); //from 15 % to 95 %
        uart.sendByte(adc.data);
        //if(start_PC_communicate) {

            //uart.sendByte(pwm.duty);
            //uart.sendByte(angle_data);
        //}
        //TIM1->CCR1 = 5820 + adc.data*7;
        delay(1600);
        //if(uart.received_flag) {
        //    if(uart.received_byte == 0xFF) {
        //        start_PC_communicate = true;
        //    } else if(uart.received_byte == 0x00) {
        //        start_PC_communicate = false;
        //    } else {
        //        if(start_PC_communicate) {
                      // set 90 degrees angle
                      //if(uart.received_byte == 90) {
                      //    ADC_max = adc.data;
                      //}
        //            K = uart.received_byte;
        //        }                
        //    }
        //    uart.received_flag = false;            
        //}
    }
    return 0;
}

void delay(uint32_t x) {
    while(x--);
}