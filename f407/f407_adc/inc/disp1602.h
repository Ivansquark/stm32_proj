#ifndef DISP1602_H
#define DISP1602_H

#include "stm32f4xx.h"
#include "timer.h"
#include <stdlib.h>
#include <stdio.h>

class Display {
public:
    Display();
static Display* pThis;
    void sendStr(int8_t* str);
    void setPos(uint8_t x, uint8_t y);		
	void intToStr(uint32_t val);
    void floatToStr(float val);
    void arrStr_clear();

    int8_t arrStr[16];
    int8_t arrFloat[16];
    bool LED_flag = false;    

    inline void diodeToggle(){
        GPIOE->ODR ^= GPIO_ODR_OD15;        
    }
		inline void diodeOn(){
        GPIOE->BSRR |= GPIO_BSRR_BS15;        
    }
		inline void diodeOff(){
        GPIOE->BSRR |= GPIO_BSRR_BR15;        
    }		
    inline void writeData(uint8_t dt) { // low half byte
        if(((dt >> 3)&0x01)==1) {d7_1();} else {d7_0();}
        if(((dt >> 2)&0x01)==1) {d6_1();} else {d6_0();}
        if(((dt >> 1)&0x01)==1) {d5_1();} else {d5_0();}
        if((dt&0x01)==1) {d4_1();} else {d4_0();}
    }
    inline void writeCommand(uint8_t dt) {
        RS_0(); // command
        writeData(dt>>4); E_1(); delay(); E_0();
        writeData(dt); E_1(); delay(); E_0(); 
    }
    inline void data(uint8_t data) {
        RS_1(); writeData(data>>4); E_1(); delay(); E_0();
        writeData(data); E_1(); delay(); E_0();
    }
    inline void clearDisplay() {writeCommand(1); Timer3::pThis->delay_ms(1);}
    inline void sendByte(uint8_t byte) {data(byte);delay();}
    
    inline void LED_on() {GPIOC->BSRR |= GPIO_BSRR_BS10;}
    inline void LED_off() {GPIOC->BSRR |= GPIO_BSRR_BR10;}
    inline bool LED_state(){return (GPIOC->IDR & GPIO_IDR_ID10);}
    
    
private:    

    void gpios_init(void); 
    void display_init();
    void LED_init();
		void diode_init();
    inline void RS_1(){GPIOD->BSRR |= GPIO_BSRR_BS_0;} // data
    inline void RS_0(){GPIOD->BSRR |= GPIO_BSRR_BR_0;} // commands
    inline void RW_1(){GPIOD->BSRR |= GPIO_BSRR_BS_1;} // read
    inline void RW_0(){GPIOD->BSRR |= GPIO_BSRR_BR_1;} // write
    inline void E_1(){GPIOD->BSRR |= GPIO_BSRR_BS_2;}; // enable strobe
    inline void E_0(){GPIOD->BSRR |= GPIO_BSRR_BR_2;}; // disable
    
    inline void d7_1(){GPIOB->BSRR |= GPIO_BSRR_BS_7;}
    inline void d7_0(){GPIOB->BSRR |= GPIO_BSRR_BR_7;}
    inline void d6_1(){GPIOB->BSRR |= GPIO_BSRR_BS_6;}
    inline void d6_0(){GPIOB->BSRR |= GPIO_BSRR_BR_6;}
    inline void d5_1(){GPIOB->BSRR |= GPIO_BSRR_BS_5;}
    inline void d5_0(){GPIOB->BSRR |= GPIO_BSRR_BR_5;}
    inline void d4_1(){GPIOB->BSRR |= GPIO_BSRR_BS_4;}
    inline void d4_0(){GPIOB->BSRR |= GPIO_BSRR_BR_4;}    
    void delay();		
};

#endif //DISP1602_H