#ifndef ADC_H
#define ADC_H

#include "stm32f4xx.h"
#include "flash.h"
#include "i2c.h"
#include "math.h"
#include "dma.h"

class Adc
{
public:
    Adc();
    static Adc* pThis;
    static constexpr uint8_t MovAverLength=32; 
   
    struct Kefs {
        //!  a*x^2 + b*x + c    
        float K_1;
        float k_dx;
        float k_ln;
    };
    Kefs kef = {0};
    uint16_t ADC_Diode1 = 0;
    uint16_t ADC_Diode2 = 0;   
    float kV = 0.0;
    
    uint16_t ADC_Battery = 0;
    float Battery_V = 1.0;
    
    bool Start_diods_Flag = false;
    bool Start_battery_Flag = false;    
    bool receved_Flag = false;
    bool DataIsChecked = false;
    bool DataIsRead = false;
    bool DataIsWrite = false;
    bool NewCoefReadyToWrite = false;
    uint8_t countCheckDataRead = 0;
    uint8_t countCheckDataWrite = 0;
    uint8_t countData = 0;      
    uint8_t received_Data[24] = {0};    
    uint8_t UART_BYTE = 0;
    
    uint16_t read_diode1();
    uint16_t read_diode2();
    uint16_t read_battery();
    
    void read_diods_ADC();
    void read_battery_ADC();
    void set_Coefficients();
    
    float K_1;
    float k_dx;   
    float k_ln;
		inline void BatKeyOn() {GPIOE->BSRR |= GPIO_BSRR_BS8;}
		inline void BatKeyOff() {GPIOE->BSRR |= GPIO_BSRR_BR8;}
    
private: 
    void ADC_init();
    void init_injected_channels();
    uint8_t Index_mov_aver_diode1=0;
    uint8_t Index_mov_aver_diode2=0;    
    uint16_t MovAverSum_diode1=0;
    uint16_t MovAverSum_diode2=0;
    uint8_t Index_mov_aver_battery=0;
    uint16_t MovAverSum_battery=0;

    uint16_t arr_diode1[MovAverLength] = {0};
    uint16_t arr_diode2[MovAverLength] = {0};
    uint16_t arr_battery[MovAverLength] = {0};
    uint16_t moving_average_diode1(uint16_t val);
    uint16_t moving_average_diode2(uint16_t val);
    uint16_t moving_average_battery(uint16_t val);
    
    static constexpr float K_battery = 0.00477;  // 13V - 2.17V - 2963 => K = 13/2963=
};



#endif //ADC_H