#ifndef LCDPAR_H
#define LCDPAR_H

#include "stm32f10x.h"
#include "math.h"
#include "stdio.h"
#include "string.h"
#include "temp.h"

class LcdParInterface
{
public:
    void fillScreen(uint16_t color);
    //----------- reset B15 -----------------------
    const void reset_on(){GPIOB->BSRR|=GPIO_BSRR_BR15;}    // low
    const void reset_off(){GPIOB->BSRR|=GPIO_BSRR_BS15;}   // high
    //----------  DC B14 --------------------------------
    const void dc_data(){GPIOB->BSRR|=GPIO_BSRR_BS14;}     // High
    const void dc_command(){GPIOB->BSRR|=GPIO_BSRR_BR14;}  // Low
    //----------  WR B13 -----------------------------------
    inline const void wr_on(){GPIOB->BSRR|=GPIO_BSRR_BR13;}    //low
    inline const void wr_idle(){GPIOB->BSRR|=GPIO_BSRR_BS13;}  //high
    //----------  RD B12 -----------------------------------
    const void rd_on(){GPIOB->BSRR|=GPIO_BSRR_BR12;}    //low
    const void rd_idle(){GPIOB->BSRR|=GPIO_BSRR_BS12;}  //high
    //----------  CS B11 -----------------------------------
    const void cs_on(){GPIOB->BSRR|=GPIO_BSRR_BR11;}   //low
    const void cs_idle(){GPIOB->BSRR|=GPIO_BSRR_BS11;} //high
    inline const void wr_strobe(){wr_on();wr_idle();}   
    
    void reset();
    void write(uint8_t byte);
    void send_command(uint8_t com);
    void send_data(uint8_t data);
    void sendByte(uint8_t byte);
    void send_word(uint16_t data);
    void setColumn(uint16_t StartCol, uint16_t EndCol);
    void setRow(uint16_t StartRow, uint16_t EndRow);
    void setXY(int poX, int poY);
    void setPixel(int poX, int poY, int color);
       
    inline void swap(uint16_t x1, uint16_t x2) __attribute__((__always_inline__)){
        if(x2>x1){uint16_t z=x2;x2=x1;x1=z;}
    }
    inline void checkXYswap(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
    __attribute__((__always_inline__)) {
        swap(x1,x2); swap(y1,y2);
    }

    inline void delay(uint32_t x)
    {   while(x>0){x--;}    }
};

class LcdParIni:LcdParInterface
{
public:
    LcdParIni();
private:
    void lcd_ini();
    void tft_ini();
};

//*********** Класс обработки цифр и букв ********************
class Font_interface:public LcdParInterface
{
public:
    uint32_t char_to_int(char* str,uint8_t size);
    char arr[20];
    char arrFloat[20]{0};
    volatile uint8_t arrSize=0;

    void intToChar(uint32_t x); 
    void floatToChar(const float& val);
};


class Font_30x40: public Font_interface {
public:
    void drawSymbol(uint16_t x, uint16_t y, const uint16_t* symbol);
    void drawString(uint16_t x, uint16_t y, bool red, const char* str);
    void drawTemperature();
    void drawHumidity();
    void setTemperature(const float& temp);
    const float& getTemperature();
    void setHumidity(const float& hum);
    const float& getHumidity();
private:
    float temperature=0;
    float humidity=0;
};

#endif //LCDPAR_H