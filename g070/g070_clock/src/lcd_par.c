#include "lcd_par.h"

void drawNum_left(uint8_t num, uint8_t reverse){
    CS_1;
    drawNum(num,reverse);
}
void drawNum_right(uint8_t num, uint8_t reverse){
    CS_2;
    drawNum(num,reverse);
}

void lcd_init() {
    lcd_gpio_init();
    CS_1;
    GLCD_Initialize();
    delay_ms(10);
    GLCD_ClearText();
    delay_ms(10);
	GLCD_ClearGraphic();
    delay_ms(10);
    CS_2;
    GLCD_Initialize();
    delay_ms(10);
    GLCD_ClearText();
    delay_ms(10);
	GLCD_ClearGraphic();
    delay_ms(10);
}

void drawNum(uint8_t num, uint8_t reverse) {
    if(num<100) {
		int first = 0;
		int second = 0;
		if(reverse) {
			first = num%10;
			second = num/10;
		} else {
			second = num%10;
			first = num/10;
		}
		
		switch(first) {
			case 0:
				//GLCD_ClearGraphic();
				GLCD_DrawFigure(zero,0,reverse);
			break;
			case 1:
				//GLCD_ClearGraphic();
				GLCD_DrawFigure(one,0,reverse);
			break;
			case 2:
				//GLCD_ClearGraphic();
				GLCD_DrawFigure(two,0,reverse);
			break;
			case 3:
				//GLCD_ClearGraphic();
				GLCD_DrawFigure(three,0,reverse);
			break;
			case 4:
				//GLCD_ClearGraphic();
				GLCD_DrawFigure(four,0,reverse);
			break;
			case 5:
				//GLCD_ClearGraphic();
				GLCD_DrawFigure(five,0,reverse);
			break;
			case 6:
				//GLCD_ClearGraphic();
				GLCD_DrawFigure(six,0,reverse);
			break;
			case 7:
				//GLCD_ClearGraphic();
				GLCD_DrawFigure(seven,0,reverse);
			break;
			case 8:
				//GLCD_ClearGraphic();
				GLCD_DrawFigure(eight,0,reverse);
			break;
			case 9:
				//GLCD_ClearGraphic();
				GLCD_DrawFigure(nine,0,reverse);
			break;
		}
		switch(second) {
			case 0:				
				GLCD_DrawFigure(zero,1,reverse);
			break;
			case 1:
				GLCD_DrawFigure(one,1,reverse);
			break;
			case 2:
				GLCD_DrawFigure(two,1,reverse);
			break;
			case 3:
				GLCD_DrawFigure(three,1,reverse);
			break;
			case 4:
				GLCD_DrawFigure(four,1,reverse);
			break;
			case 5:
				GLCD_DrawFigure(five,1,reverse);
			break;
			case 6:
				GLCD_DrawFigure(six,1,reverse);
			break;
			case 7:
				GLCD_DrawFigure(seven,1,reverse);
			break;
			case 8:
				GLCD_DrawFigure(eight,1,reverse);
			break;
			case 9:
				GLCD_DrawFigure(nine,1,reverse);
			break;
		}		
	}
}