#include "font.h"

uint16_t Font_28x30_D::color_background_old = 0xFFFF;
uint16_t Font_28x30_D::color_figure_old = 0x0000;

Font_28x30_D::Font_28x30_D(uint16_t color_background_C, uint16_t color_figure_C) {    
    color_background_new = color_background_C;
    color_figure_new = color_figure_C;
    const uint16_t* charAddress = image_data_Font_28x30_0x20;
    for(int i=0; i<11760; i++){
        if(*((uint16_t*)charAddress + i) == color_background_old) {
            *((uint16_t*)charAddress + i) = color_background_new;
        } else if(*((uint16_t*)charAddress + i) == color_figure_old) {
            *((uint16_t*)charAddress + i) = color_figure_new;
        }
    }
    color_background_old = color_background_new;
    color_figure_old = color_figure_new;    
}

void Font_28x30_D::setColors(uint16_t back, uint16_t fig) {
    const uint16_t* charAddress = image_data_Font_28x30_0x20;
    for(int i=0; i<11760; i++){
        if(*((uint16_t*)charAddress + i) == color_background_old) {
            *((uint16_t*)charAddress + i) = back;
        } else if(*((uint16_t*)charAddress + i) == color_figure_old) {
            *((uint16_t*)charAddress + i) = fig;
        }
    }
    color_background_old = back;
    color_figure_old = fig;
}

void Font_28x30_D::drawSymbol(uint16_t x, uint16_t y, char byte){
    const uint16_t* charAddress = byteToChar(byte);
    setRect(x,y,x+27,y+29);
    writeCmd(0x2C00);
    for(int i=0; i<840; i++) {
        writeData(*((uint16_t*)charAddress + i));
    }
}

void Font_28x30_D::drawString(uint16_t x, uint16_t y, char* byte) {
    uint16_t dx = 0;
    while(*byte) {
        drawSymbol(x+dx,y,*byte);
        dx+=28;
        byte++;
    }
}


const uint16_t* Font_28x30_D::byteToChar(char byte) {    
    
    if(byte == 0x30) {
        return image_data_Font_28x30_0x30;
    } else if(byte == 0x31) {
        return image_data_Font_28x30_0x31;
    } else if(byte == 0x32) {
        return image_data_Font_28x30_0x32;
    } else if(byte == 0x33) {
        return image_data_Font_28x30_0x33;
    } else if(byte == 0x34) {
        return image_data_Font_28x30_0x34;
    } else if(byte == 0x35) {
        return image_data_Font_28x30_0x35;
    } else if(byte == 0x36) {
        return image_data_Font_28x30_0x36;
    } else if(byte == 0x37) {
        return image_data_Font_28x30_0x37;
    } else if(byte == 0x38) {
        return image_data_Font_28x30_0x38;
    } else if(byte == 0x39) {
        return image_data_Font_28x30_0x39;
    } else if(byte == 0x2e) {
        return image_data_Font_28x30_0x2e;
    } else if(byte == 0x3a) {
        return image_data_Font_28x30_0x3a;
    } else if(byte == 0x20) {
        return image_data_Font_28x30_0x20;
    }    
    return nullptr;
}

uint32_t Font_28x30_D::char_to_int(char* str,uint8_t size) {
    uint32_t x;
    for(uint8_t i=0;i<size;i++) {
        uint8_t dec=0;
        if (str[i]==48){dec=0;} if (str[i]==49){dec=1;}
        if (str[i]==50){dec=2;} if (str[i]==51){dec=3;}
        if (str[i]==52){dec=4;} if (str[i]==53){dec=5;}
        if (str[i]==54){dec=6;} if (str[i]==55){dec=7;}
        if (str[i]==56){dec=8;} if (str[i]==57){dec=9;}
        x+=dec*pow(10,size-i);           
    }
    return x;
}

void Font_28x30_D::drawIntValue(uint16_t x, uint16_t y, char* byte, uint8_t digitsNum) {
    uint8_t count=0; uint16_t dx=0;
    while(*byte) {
        drawSymbol(x+dx,y,*byte);
        dx+=28;
        byte++; count++;
    }
    while(count<digitsNum) {
        drawSymbol(x+dx,y,0x20);
        dx+=28;  count++;
    }
}