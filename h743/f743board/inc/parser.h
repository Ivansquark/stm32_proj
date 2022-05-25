#ifndef PARSER_H
#define PARSER_H

#include "fmc.h"
#include "ff.h"

class Parser: public Nor_LCD_interface {
public:
    Parser();
    static Parser* pThis;

    uint16_t font[224][576]={0x0000};
    uint8_t fileBuf[512]={0};
    void drawSymbol(uint16_t x, uint16_t y, char byte);
    void drawString(uint16_t x, uint16_t y, char* byte);
    void drawIntValue(uint16_t x, uint16_t y, char* byte, uint8_t digitsNum);
    void setColors(uint16_t back, uint16_t fig);

    void fat_init();
private: 
    void init();
    uint16_t* byteToChar(char byte);
    FATFS fs;
    FRESULT res;
    FIL test; 
    UINT testBytes;
    uint32_t fontCount=0;

    static uint16_t color_background_old;
    static uint16_t color_figure_old; 
    uint16_t color_background_new = 0xFFFF;
    uint16_t color_figure_new = 0x0000;

};

#endif PARSER_H