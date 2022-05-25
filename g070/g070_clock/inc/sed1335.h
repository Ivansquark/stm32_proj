#ifndef SED1335_H_INCLUDED
#define SED1335_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include "sed1335-stm32.h"
#include "fonts.h"
//#include "figures.h"

extern char buffer[];

extern void GLCD_Initialize(void);

extern void GLCD_WriteText(unsigned char x, unsigned char y, char *text);

// ���� ABSOLUTE_COORDINATE, �� x < 320, y < 240; ���� RELATIVE_COORDINATE, �� x < 13, y < 7.
typedef enum {ABSOLUTE_COORDINATE, RELATIVE_COORDINATE} CoordinateType;
extern void GLCD_HideCursor(void);

extern void GLCD_DrawRectangle(unsigned int x, unsigned int y, unsigned int b, unsigned int a, int color);
extern void GLCD_DrawCircle(unsigned int cx, unsigned int cy ,unsigned int radius, int color);
extern void GLCD_DrawLine(int X1, int Y1,int X2,int Y2,int color);
extern void GLCD_DrawHorLine(int x1, int y1,int x2, int color);
extern void GLCD_InvertRow(unsigned int x, unsigned y, unsigned int x2, int color);

extern void GLCD_DrawPicture2(const unsigned char pic[206][40]);
extern void GLCD_DrawPicture(const unsigned char* picture);

	void GLCD_DrawFigure(const unsigned char* figure, int pos,uint8_t reverse);
	unsigned char reverse_bits(unsigned char b);	

extern void GLCD_ClearText(void);
extern void GLCD_ClearGraphic(void);
extern void GLCD_ClearTextArea(int x, int y, int x2, int y2);
extern void GLCD_ClearGraphicArea(int x, int y, int x2, int y2);

extern void GLCD_Program_CheckFont(void);

void GLCD_LoadFont8x8_(void);
void GLCD_WriteText(unsigned char x, unsigned char y, char *text);
extern const unsigned char FONT_8x8[64][8];
extern const unsigned short Classic_Console_Eng8x16[];
extern const unsigned short Classic_Console_Rus8x16[];

#endif /* SED1335_INCLUDED */
