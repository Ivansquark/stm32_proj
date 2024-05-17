#ifndef FONT_H
#define FONT_H

//#include "F_28x32.h"
#include "fmc.h"
#include "F_28x30_Digit.h"
#include "stdio.h"

class Font_28x30_D: public Nor_LCD_interface {
public:   
    Font_28x30_D(uint16_t color_background_C=0xFFFF, uint16_t color_figure_C=0x0000);
    void drawSymbol(uint16_t x, uint16_t y, char byte);
    void drawString(uint16_t x, uint16_t y, char* byte);
    void drawIntValue(uint16_t x, uint16_t y, char* byte, uint8_t digitsNum);

    void setColors(uint16_t back, uint16_t fig);
    
private:    
    const uint16_t* byteToChar(char byte); 
    uint32_t char_to_int(char* str,uint8_t size);    
    
    static uint16_t color_background_old;
    static uint16_t color_figure_old; 
    uint16_t color_background_new = 0xFFFF;
    uint16_t color_figure_new = 0x0000;

    char arr[20]{0};
    char arrFloat[20]{0};
    uint8_t arrSize=0;
};

#endif //FONT_H