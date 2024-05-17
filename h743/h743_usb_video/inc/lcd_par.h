#ifndef LCD_PAR_H
#define LCD_PAR_H

#include "stm32h743xx.h"
#include "timer.h"

class LCD_par {
public:
    LCD_par();
static LCD_par* pThis;

    void writeData(uint16_t data);
    void writeCmd(uint16_t cmd);
    void writeReg(uint16_t cmd, uint16_t data);
    
    

inline void DATA() { GPIOD->BSRR |= GPIO_BSRR_BS11; }
inline void CMD() { GPIOD->BSRR |= GPIO_BSRR_BR11; } 
inline void READ() {GPIOD->BSRR |= GPIO_BSRR_BR4; GPIOD->BSRR |= GPIO_BSRR_BS4;}
inline void WRITE() { GPIOD->BSRR |= GPIO_BSRR_BR5; GPIOD->BSRR |= GPIO_BSRR_BS5;}   
inline void RES_on() { GPIOA->BSRR |= GPIO_BSRR_BS15; }
inline void RES_off() { GPIOA->BSRR |= GPIO_BSRR_BR15; }
inline void CS_on() { GPIOD->BSRR |= GPIO_BSRR_BR7; }
inline void CS_off() { GPIOD->BSRR |= GPIO_BSRR_BS7; }

private:
    void init();
    void display_init();
    void bit_setting(uint8_t data); 
    void reset();
    struct bitset {
        uint32_t bitMASK_D_set=0x0;
        uint32_t bitMASK_D_reset=0x0;
        uint32_t bitMASK_E_set=0x0;
        uint32_t bitMASK_E_reset=0x0;
    };
    bitset bits;
};


#endif //LCD_PAR_H