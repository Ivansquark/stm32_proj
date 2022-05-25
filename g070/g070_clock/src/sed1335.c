#include "sed1335.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>



#define 	SED1335_SYSTEM_SET   	0x40
#define 	SED1335_SLEEP_IN   		0x53
#define 	SED1335_DISP_OFF   		0x58
#define 	SED1335_DISP_ON   		0x59
#define 	SED1335_SCROLL   		0x44
#define 	SED1335_CSRFORM   		0x5d
#define 	SED1335_CGRAM_ADR   	0x5c
#define 	SED1335_CSRDIR_U   		0x4e
#define 	SED1335_CSRDIR_D   		0x4f
#define 	SED1335_CSRDIR_L   		0x4d
#define 	SED1335_CSRDIR_R   		0x4c
#define 	SED1335_HDOT_SCR   		0x5a
#define 	SED1335_OVLAY   		0x5b
#define 	SED1335_CSRW   			0x46
#define 	SED1335_CSRR   			0x47
#define 	SED1335_MWRITE   		0x42
#define 	SED1335_MREAD   		0x43

#define 	SED1335_SCR_WIDTH   				319

#define FONT_8x8_SELECTED

#ifdef FONT_8x8_SELECTED
	#define 	SED1335_SYSTEM_SET_M0   			0		//The internal CGROM is selected.
	#define 	SED1335_SYSTEM_SET_M2   			0		//The character height is 8 pixels.
	#define 	SED1335_SYSTEM_SET_WS   			0		//A single panel drive is selected.
	#define 	SED1335_SYSTEM_SET_IV   			1		//Screen origin compensation is not done.
	#define 	SED1335_SYSTEM_SET_FX   			7		//Horizontal Character Size in pixels.
	#define 	SED1335_SYSTEM_SET_FY   			7		//Vertical Character Size in pixels.
	#define 	SED1335_SYSTEM_SET_WF   			1		//Two-frame AC drive is selected.
	#define 	SED1335_SYSTEM_SET_CR   			39		//Character bytes per row.
	#define 	SED1335_SYSTEM_SET_TCR   			42//90		//Total Character bytes per row.
	#define 	SED1335_SYSTEM_SET_LF   			239		//Frame height in lines.
	#define 	SED1335_SYSTEM_SET_APL   			40		//The horizontal address range of the virtual screen.
	#define 	SED1335_SYSTEM_SET_APH   			0
	#define 	SED1335_LINES   							30
	#define 	SED1335_SCROLL_SAD1L   				0		//The memory start address of screen block 1.
	#define 	SED1335_SCROLL_SAD1H   				0		//The memory start address of screen block 1.
	#define 	SED1335_SCROLL_SL1   					239		//The size of screen block 1, in lines.
	#define 	SED1335_SCROLL_SAD2L   				0xB0 //0x00	// 176. The memory start address of screen block 2.
	#define 	SED1335_SCROLL_SAD2H   				0x04 //0x08	// 1024. The memory start address of screen block 2.
	#define 	SED1335_SCROLL_SL2   					239		//The size of screen block 2, in lines.
	#define 	SED1335_SCROLL_SAD3L   				0
	#define 	SED1335_SCROLL_SAD3H  				0
	#define 	SED1335_SCROLL_SAD4L   				0
	#define 	SED1335_SCROLL_SAD4H   				0
	#define 	SED1335_CSRFORM_CRX   				0x04	//Width of the cursor, in pixels.
	#define 	SED1335_CSRFORM_CRY   				0x07	//Height of the cursor, in pixels.
	#define 	SED1335_CSRFORM_CM   					0		//When 0 - underscore cursor, when 1 - block cursor.
	#define 	SED1335_OVLAY_MX0   					1
	#define 	SED1335_OVLAY_MX1   					0
	#define 	SED1335_OVLAY_DM1   					0
	#define 	SED1335_OVLAY_DM2   					0
	#define 	SED1335_OVLAY_OV   						0
	#define 	SED1335_CGRAM_ADR_SAGL   			0
	#define 	SED1335_CGRAM_ADR_SAGH   			0x70
	#define		SED1335_CGRAM1_ADR_SAGL				0 
	#define		SED1335_CGRAM1_ADR_SAGH				SED1335_CGRAM_ADR_SAGH + 0x04 //0x04 = 0x80 (ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½) * 8 (ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½). 
	#define		SED1335_CGRAM2_ADR_SAGL				0
	#define		SED1335_CGRAM2_ADR_SAGH				SED1335_CGRAM_ADR_SAGH + 0x07 //0x07 = 0xE0 * 8.
	#define 	SED1335_HDOT_SCR_SCRD   			0
	#define 	SED1335_DISP_ON_OFF_FLASH   	0x16
	//#define 	SED1335_TEXTSIZE   		((SED1335_SCROLL_SAD2H << 8) + SED1335_SCROLL_SAD2L)

	#define		SED1335_GRAPHICSTART	((SED1335_SCROLL_SAD2H << 8) + SED1335_SCROLL_SAD2L)

	#define 	SED1335_GRAPHICSIZE   	((SED1335_SCROLL_SL2+1) * (SED1335_SCR_WIDTH+1))>>3
	#define 	SED1335_MEM_END   		10800
	#define 	SED1335_SYSTEM_SET_P1	0x10 | (SED1335_SYSTEM_SET_IV << 5) | (SED1335_SYSTEM_SET_WS << 3) | (SED1335_SYSTEM_SET_M2 << 2) | SED1335_SYSTEM_SET_M0
	#define 	SED1335_SYSTEM_SET_P2	0x00 | (SED1335_SYSTEM_SET_WF << 7) | SED1335_SYSTEM_SET_FX
	#define 	SED1335_CSRFORM_P2   	0x00 | (SED1335_CSRFORM_CM << 7) | SED1335_CSRFORM_CRY
	#define 	SED1335_OVLAY_P1   		0x00 | (SED1335_OVLAY_OV << 4) | (SED1335_OVLAY_DM2 << 3) | (SED1335_OVLAY_DM1 << 2) | (SED1335_OVLAY_MX1 << 1) | SED1335_OVLAY_MX0

	#define SED1335_LENGTH_X_CHARS	40
	#define SED1335_LENGTH_Y_CHARS 	30
#endif

#ifdef	FONT_8x16_SELECTED
	#define 	SED1335_SYSTEM_SET_M0   			1		//The external CGROM is selected.
	#define 	SED1335_SYSTEM_SET_M2   			1		//The character height is 16 pixels.
	#define 	SED1335_SYSTEM_SET_WS   			0		//A single panel drive is selected.
	#define 	SED1335_SYSTEM_SET_IV   			1		//Screen origin compensation is not done.
	#define 	SED1335_SYSTEM_SET_FX   			7		//Horizontal Character Size in pixels.
	#define 	SED1335_SYSTEM_SET_FY   			15		//Vertical Character Size in pixels.
	#define 	SED1335_SYSTEM_SET_WF   			1		//Two-frame AC drive is selected.
	#define 	SED1335_SYSTEM_SET_CR   			39		//Character bytes per row.
	#define 	SED1335_SYSTEM_SET_TCR   			42//90		//Total Character bytes per row.
	#define 	SED1335_SYSTEM_SET_LF   			239		//Frame height in lines.
	#define 	SED1335_SYSTEM_SET_APL   			40		//The horizontal address range of the virtual screen.
	#define 	SED1335_SYSTEM_SET_APH   			0
	#define 	SED1335_LINES   							30
	#define 	SED1335_SCROLL_SAD1L   				0		//The memory start address of screen block 1.
	#define 	SED1335_SCROLL_SAD1H   				0		//The memory start address of screen block 1.
	#define 	SED1335_SCROLL_SL1   					239		//The size of screen block 1, in lines.
	#define 	SED1335_SCROLL_SAD2L   				0xB0 //0x00	// 176. The memory start address of screen block 2.
	#define 	SED1335_SCROLL_SAD2H   				0x04 //0x08	// 1024. The memory start address of screen block 2.
	#define 	SED1335_SCROLL_SL2   					239		//The size of screen block 2, in lines.
	#define 	SED1335_SCROLL_SAD3L   				0
	#define 	SED1335_SCROLL_SAD3H  				0
	#define 	SED1335_SCROLL_SAD4L   				0
	#define 	SED1335_SCROLL_SAD4H   				0
	#define 	SED1335_CSRFORM_CRX   				0x04	//Width of the cursor, in pixels.
	#define 	SED1335_CSRFORM_CRY   				0x07	//Height of the cursor, in pixels.
	#define 	SED1335_CSRFORM_CM   					0		//When 0 - underscore cursor, when 1 - block cursor.
	#define 	SED1335_OVLAY_MX0   					1
	#define 	SED1335_OVLAY_MX1   					0
	#define 	SED1335_OVLAY_DM1   					0
	#define 	SED1335_OVLAY_DM2   					0
	#define 	SED1335_OVLAY_OV   						0
	#define 	SED1335_CGRAM_ADR_SAGL   			0
	#define 	SED1335_CGRAM_ADR_SAGH   			0x70
	#define		SED1335_CGRAM1_ADR_SAGL				0 
	#define		SED1335_CGRAM1_ADR_SAGH				SED1335_CGRAM_ADR_SAGH + 0x04 //0x04 = 0x80 (ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½) * 8 (ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½). 
	#define		SED1335_CGRAM2_ADR_SAGL				0
	#define		SED1335_CGRAM2_ADR_SAGH				SED1335_CGRAM_ADR_SAGH + 0x07 //0x07 = 0xE0 * 8.
	#define 	SED1335_HDOT_SCR_SCRD   			0
	#define 	SED1335_DISP_ON_OFF_FLASH   	0x16
	//#define 	SED1335_TEXTSIZE   		((SED1335_SCROLL_SAD2H << 8) + SED1335_SCROLL_SAD2L)

	#define		SED1335_GRAPHICSTART	((SED1335_SCROLL_SAD2H << 8) + SED1335_SCROLL_SAD2L)

	#define 	SED1335_GRAPHICSIZE   	((SED1335_SCROLL_SL2+1) * (SED1335_SCR_WIDTH+1))>>3
	#define 	SED1335_MEM_END   		10800
	#define 	SED1335_SYSTEM_SET_P1	0x10 | (SED1335_SYSTEM_SET_IV << 5) | (SED1335_SYSTEM_SET_WS << 3) | (SED1335_SYSTEM_SET_M2 << 2) | SED1335_SYSTEM_SET_M0
	#define 	SED1335_SYSTEM_SET_P2	0x00 | (SED1335_SYSTEM_SET_WF << 7) | SED1335_SYSTEM_SET_FX
	#define 	SED1335_CSRFORM_P2   	0x00 | (SED1335_CSRFORM_CM << 7) | SED1335_CSRFORM_CRY
	#define 	SED1335_OVLAY_P1   		0x00 | (SED1335_OVLAY_OV << 4) | (SED1335_OVLAY_DM2 << 3) | (SED1335_OVLAY_DM1 << 2) | (SED1335_OVLAY_MX1 << 1) | SED1335_OVLAY_MX0
	
	#define SED1335_LENGTH_X_CHARS	40
	#define SED1335_LENGTH_Y_CHARS 	15
#endif

#define 	SED1335_LENGTH_X_PIXELS	320
#define 	SED1335_LENGTH_Y_PIXELS	240
#define 	SED1335_LENGTH_X_BYTES	40
#define		SED1335_LENGTH_Y_BYTES	30



// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½:
void GLCD_SetPixel_(unsigned int x, unsigned int y, int color);
void GLCD_SetPixels_(unsigned int x, unsigned int y, unsigned char pixels);
void GLCD_SetCursorAddress_(unsigned int address);
void GLCD_TextGoTo_(unsigned char x, unsigned char y);
void GLCD_GraphicGoTo_(unsigned int x, unsigned int y);
void GLCD_LoadFont8x8_(void);
void GLCD_LoadFont8x16_(void);

void GLCD_WriteText(unsigned char x, unsigned char y, char *text);

// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½-ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½:
//void GLCD_InitializePorts_(void);
//void GLCD_HardReset_(void);
//void GLCD_WriteCommand_(unsigned char);
//void GLCD_WriteData_(unsigned char);
unsigned char GLCD_ReadData_(void);

// ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½. ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½, ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½.
#define BUFFER_SIZE 100
char buffer[BUFFER_SIZE];

void GLCD_Initialize()
{
	GLCD_InitializePorts_();
	GLCD_HardReset_();	
	
	GLCD_WriteCommand_(SED1335_DISP_OFF);
	GLCD_WriteData_(SED1335_DISP_ON_OFF_FLASH);
	
	GLCD_WriteCommand_(SED1335_SYSTEM_SET);
	GLCD_WriteData_(SED1335_SYSTEM_SET_P1);	
	GLCD_WriteData_(SED1335_SYSTEM_SET_P2);		
	GLCD_WriteData_(SED1335_SYSTEM_SET_FY);		
	GLCD_WriteData_(SED1335_SYSTEM_SET_CR);		
	GLCD_WriteData_(SED1335_SYSTEM_SET_TCR);	
	GLCD_WriteData_(SED1335_SYSTEM_SET_LF);		
	GLCD_WriteData_(SED1335_SYSTEM_SET_APL);	
	GLCD_WriteData_(SED1335_SYSTEM_SET_APH);	
	
	GLCD_WriteCommand_(SED1335_SCROLL);   
	GLCD_WriteData_(SED1335_SCROLL_SAD1L);		
	GLCD_WriteData_(SED1335_SCROLL_SAD1H);		
	GLCD_WriteData_(SED1335_SCROLL_SL1);		
	GLCD_WriteData_(SED1335_SCROLL_SAD2L);		
	GLCD_WriteData_(SED1335_SCROLL_SAD2H);		
	GLCD_WriteData_(SED1335_SCROLL_SL2);		
	GLCD_WriteData_(SED1335_SCROLL_SAD3L);		
	GLCD_WriteData_(SED1335_SCROLL_SAD3H); 		
	GLCD_WriteData_(SED1335_SCROLL_SAD4L);		
	GLCD_WriteData_(SED1335_SCROLL_SAD4H);

	
	GLCD_WriteCommand_(SED1335_CSRFORM);
	GLCD_WriteData_(SED1335_CSRFORM_CRX);		
	GLCD_WriteData_(SED1335_CSRFORM_P2);		
	
	GLCD_WriteCommand_(SED1335_CGRAM_ADR);       
	GLCD_WriteData_(SED1335_CGRAM_ADR_SAGL);			
	GLCD_WriteData_(SED1335_CGRAM_ADR_SAGH);				
	
	GLCD_WriteCommand_(SED1335_CSRDIR_R);      

	GLCD_WriteCommand_(SED1335_HDOT_SCR);       
	GLCD_WriteData_(SED1335_HDOT_SCR_SCRD);			
	
	GLCD_WriteCommand_(SED1335_OVLAY);            
	GLCD_WriteData_(SED1335_OVLAY_P1);	
	
	//#ifdef	FONT_8x8_SELECTED	
	//	GLCD_LoadFont8x8_();		
	//#endif
		
	GLCD_WriteCommand_(SED1335_DISP_ON);
	GLCD_WriteData_(SED1335_DISP_ON_OFF_FLASH);
	
	//// ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	//GLCD_GraphicGoTo_(0, 240);
	//GLCD_WriteCommand_(SED1335_MWRITE);
	//GLCD_WriteData_('.');
}

bool checkDisplayTaskInvoked_ = false;
uint16_t checkDisplayTaskCurrentMs_ = 0;
const uint16_t CHECK_DISPLAY_TASK_MS = 500;

void GLCD_WriteText(unsigned char x, unsigned char y, char *text)
{
	GLCD_TextGoTo_(x, y);
	GLCD_WriteCommand_(SED1335_MWRITE);
	while(*text) {
		unsigned char ch = *text;		
		#ifdef FONT_8x16_SELECTED
		GLCD_WriteData_(ch);
		#endif
		
		#ifdef FONT_8x8_SELECTED
		if((ch >= 48) && (ch <= 57)) {	
			ch += 98;
		}
		else if(ch == (unsigned char)'¹') {
			ch = 156;
		}		
		GLCD_WriteData_(ch);
		#endif
		text++;
	}
}

void GLCD_CheckDisplay_InvokeTask_()
{
	if(!checkDisplayTaskInvoked_)
	{
		checkDisplayTaskCurrentMs_++;
		if(checkDisplayTaskCurrentMs_ == CHECK_DISPLAY_TASK_MS)
		{
			checkDisplayTaskCurrentMs_ = 0;
			
			checkDisplayTaskInvoked_ = true;
		}
	}
	else
	{
		// error
	}
}


void GLCD_HideCursor()
{
	GLCD_TextGoTo_(40, 30);
}

void GLCD_DrawRectangle(unsigned int x, unsigned int y, unsigned int b, unsigned int a, int color)
{
  	unsigned int j; // zmienna pomocnicza
  	// rysowanie linii pionowych (boki)
  	for (j = 0; j < a; j++) {
	  	GLCD_SetPixel_(x, y + j, color);
		GLCD_SetPixel_(x + b - 1, y + j, color);
	}
  	// rysowanie linii poziomych (podstawy)
  	for (j = 0; j < b; j++)	{
		GLCD_SetPixel_(x + j, y, color);
		GLCD_SetPixel_(x + j, y + a - 1, color);
	}
}

void GLCD_DrawCircle(unsigned int cx, unsigned int cy ,unsigned int radius, int color)
{
	int x, y, xchange, ychange, radiusError;
	x = radius;
	y = 0;
	xchange = 1 - 2 * radius;
	ychange = 1;
	radiusError = 0;
	while(x >= y)
  	{
  		GLCD_SetPixel_(cx+x, cy+y, color); 
  		GLCD_SetPixel_(cx-x, cy+y, color); 
  		GLCD_SetPixel_(cx-x, cy-y, color);
  		GLCD_SetPixel_(cx+x, cy-y, color); 
  		GLCD_SetPixel_(cx+y, cy+x, color); 
  		GLCD_SetPixel_(cx-y, cy+x, color); 
  		GLCD_SetPixel_(cx-y, cy-x, color); 
  		GLCD_SetPixel_(cx+y, cy-x, color); 
  		y++;
  		radiusError += ychange;
  		ychange += 2;
  		if ( 2*radiusError + xchange > 0 )
    	{
    		x--;
			radiusError += xchange;
			xchange += 2;
		}
  	}
}


void GLCD_DrawLine(int X1, int Y1,int X2,int Y2, int color)
{
	int CurrentX, CurrentY, Xinc, Yinc, 
    Dx, Dy, TwoDx, TwoDy, 
	TwoDxAccumulatedError, TwoDyAccumulatedError;

	Dx = (X2-X1); // obliczenie skï¿½adowej poziomej
	Dy = (Y2-Y1); // obliczenie skï¿½adowej pionowej

	TwoDx = Dx + Dx; // podwojona skï¿½adowa pozioma
	TwoDy = Dy + Dy; // podwojona skï¿½adowa pionowa

	CurrentX = X1; // zaczynamy od X1
	CurrentY = Y1; // oraz Y1

	Xinc = 1; // ustalamy krok zwiï¿½kszania pozycji w poziomie 
	Yinc = 1; // ustalamy krok zwiï¿½kszania pozycji w pionie

	if(Dx < 0) // jesli skï¿½adowa pozioma jest ujemna 
  	{
  		Xinc = -1; // to bï¿½dziemy siï¿½ "cofaï¿½" (krok ujemny)
  		Dx = -Dx;  // zmieniamy znak skï¿½adowej na dodatni
  		TwoDx = -TwoDx; // jak rï¿½wnieï¿½ podwojonej skï¿½adowej
  	}

	if (Dy < 0) // jeï¿½li skï¿½adowa pionowa jest ujemna
  	{
  		Yinc = -1; // to bï¿½dziemy siï¿½ "cofaï¿½" (krok ujemny)
  		Dy = -Dy; // zmieniamy znak skï¿½adowej na dodatki
  		TwoDy = -TwoDy; // jak rï¿½wniez podwojonej skï¿½adowej
  	}

	GLCD_SetPixel_(X1,Y1, color); // stawiamy pierwszy krok (zapalamy pierwszy piksel)

	if ((Dx != 0) || (Dy != 0)) // sprawdzamy czy linia skï¿½ada siï¿½ z wiï¿½cej niï¿½ jednego punktu ;)
  	{
  		// sprawdzamy czy skï¿½adowa pionowa jest mniejsza lub rï¿½wna skï¿½adowej poziomej
  		if (Dy <= Dx) // jeï¿½li tak, to idziemy "po iksach"
    	{ 
    		TwoDxAccumulatedError = 0; // zerujemy zmiennï¿½ 
    		do // ruszamy w drogï¿½
	  		{
      			CurrentX += Xinc; // do aktualnej pozycji dodajemy krok 
      			TwoDxAccumulatedError += TwoDy; // a tu dodajemy podwojonï¿½ skï¿½adowï¿½ pionowï¿½
      			if(TwoDxAccumulatedError > Dx)  // jeï¿½li TwoDxAccumulatedError jest wiï¿½kszy od Dx
        		{
        			CurrentY += Yinc; // zwiï¿½kszamy aktualnï¿½ pozycjï¿½ w pionie
        			TwoDxAccumulatedError -= TwoDx; // i odejmujemy TwoDx
        		}
       			GLCD_SetPixel_(CurrentX,CurrentY, color);// stawiamy nastï¿½pny krok (zapalamy piksel)
       		}while (CurrentX != X2); // idziemy tak dï¿½ugo, aï¿½ osiï¿½gniemy punkt docelowy
     	}
   		else // w przeciwnym razie idziemy "po igrekach" 
      	{
      		TwoDyAccumulatedError = 0; 
      		do 
	    	{
        		CurrentY += Yinc; 
        		TwoDyAccumulatedError += TwoDx;
        		if(TwoDyAccumulatedError>Dy) 
          		{
          			CurrentX += Xinc;
          			TwoDyAccumulatedError -= TwoDy;
          		}
         		GLCD_SetPixel_(CurrentX,CurrentY, color); 
         	}while (CurrentY != Y2);
    	}
  	}
}

void GLCD_DrawHorLine(int x1, int y1,int x2, int color)
{
	x1 = x1 < 0 ? 0 : x1;
	x1 = x1 > SED1335_LENGTH_X_PIXELS - 1 ? SED1335_LENGTH_X_PIXELS -  1: x1;
	
	y1 = y1 < 0 ? 0 : y1;
	y1 = y1 > SED1335_LENGTH_Y_PIXELS - 1 ? SED1335_LENGTH_Y_PIXELS - 1 : y1;
	
	x2 = x2 < 0 ? 0 : x2;
	x2 = x2 > SED1335_LENGTH_X_PIXELS - 1 ? SED1335_LENGTH_X_PIXELS -  1: x2;
	
	if(color)
	{
		uint8_t x_startByte = x1 / 8;
		uint8_t x_startBits = x1 % 8;
		
		uint8_t x_endByte = x2 / 8;
		uint8_t x_endBits = x2 % 8;
		
		if(x_startBits != 0)
		{
			uint8_t byte = 0;
		
			for(int i = 0; i < 8 - x_startBits; i++)
			{
				byte += (1 << i);
			}
			
			GLCD_SetPixels_(x_startByte, y1, byte);
			
			x_startByte++;
		}
		
		for(int i = x_startByte; i < x_endByte; i++)
		{
			GLCD_SetPixels_(i, y1, 0xff);
		}
		
		if(x_endBits != 0)
		{
			uint8_t byte = 0;
		
			for(int i = 0; i < 8 - x_endBits; i++)
			{
				byte += (1 << i);
			}
			
			byte = 255 - byte;
			
			GLCD_SetPixels_(x_endByte, y1, byte);
		}
	}
	else
	{
		uint8_t x_startByte = x1 / 8;
		uint8_t x_endByte = x2 / 8 + 1;
		
		for(int i = x_startByte; i < x_endByte; i++)
		{
			GLCD_SetPixels_(i, y1, 0x00);
		}
	}
}

void GLCD_InvertRow(unsigned int x, unsigned int y, unsigned int x2, int color)
{
	int start_y, end_y, height_y;
	char byte;
	height_y = 8;	
	if(y == 0)
		start_y = 0;
	else
		start_y = 1;
		
	if(y == SED1335_LENGTH_Y_BYTES - 1)
		end_y = 0;
	else
		end_y = 1;
		
	#ifdef FONT_8x8_SELECTED
		height_y = 8;
	#endif
	
	#ifdef FONT_8x16_SELECTED
		height_y = 16;
	#endif
		
	if(color == 0)
		byte = 0x00;
	else
		byte = 0xff;
	
	int i, j;
	for(i = 0; i < x2 - x; i++)
	{
		for(j = -start_y; j < height_y + end_y; j++)
		{
			GLCD_SetPixels_(x + i, y * height_y + j, byte);
		}	
	}	
}

unsigned char reverse_bits(unsigned char b) {
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

void GLCD_DrawFigure(const unsigned char* figure, int pos, uint8_t reverse) {
	int x, y;
	if(reverse) {
		switch(pos) {
			case 0:			
				for(y = 0; y < SED1335_LENGTH_Y_PIXELS; y++) {
					for(x = 0; x < SED1335_LENGTH_X_BYTES/2; x++) {
						GLCD_SetPixels_(x, y, reverse_bits(figure[(SED1335_LENGTH_Y_PIXELS * SED1335_LENGTH_X_BYTES/2-1) - 
																  (y * SED1335_LENGTH_X_BYTES/2 + x)]));
					}
				}
			break;
			case 1:
				for(y = 0; y < SED1335_LENGTH_Y_PIXELS; y++)
				{
					for(x = SED1335_LENGTH_X_BYTES/2; x < SED1335_LENGTH_X_BYTES; x++)
					{
						GLCD_SetPixels_(x, y, reverse_bits(figure[(SED1335_LENGTH_Y_PIXELS * SED1335_LENGTH_X_BYTES/2-1) -
																  (y * SED1335_LENGTH_X_BYTES/2 + x)]));
					}
				}
			break;
			default:
				break;		
		}
	} else { //usual
		switch(pos) {
			case 0:			
				for(y = 0; y < SED1335_LENGTH_Y_PIXELS; y++)
				{
					for(x = 0; x < SED1335_LENGTH_X_BYTES/2; x++)
					{
						GLCD_SetPixels_(x, y, figure[y * SED1335_LENGTH_X_BYTES/2 + x]);
					}
				}
			break;	
			case 1:
				for(y = 0; y < SED1335_LENGTH_Y_PIXELS; y++)
				{
					for(x = SED1335_LENGTH_X_BYTES/2; x < SED1335_LENGTH_X_BYTES; x++)
					{
						GLCD_SetPixels_(x, y, figure[y * SED1335_LENGTH_X_BYTES/2 + x]);
					}
				}
			break;
			default:
				break;		
		}
	}	
}

void GLCD_DrawPicture(const unsigned char* pic)
{
	int x, y;
	for(y = 0; y < SED1335_LENGTH_Y_PIXELS; y++)
	{
		for(x = 0; x < SED1335_LENGTH_X_BYTES; x++)
		{
			GLCD_SetPixels_(x, y, pic[y * SED1335_LENGTH_X_BYTES + x]);
		}
	}
}	

void GLCD_DrawPicture2(const unsigned char pic[206][40])
{
	int x, y;
	for(y = 0; y < 206; y++)
	{
		for(x = 0; x < SED1335_LENGTH_X_BYTES; x++)
		{
			GLCD_SetPixels_(x, y + 20, pic[y][x]);
		}
	}
}	

void GLCD_ClearText(void)
{
	int i;
	GLCD_TextGoTo_(0,0);
	GLCD_WriteCommand_(SED1335_MWRITE);
	for(i = 0; i < SED1335_LENGTH_X_BYTES * SED1335_LENGTH_Y_BYTES; i++)
	{
		GLCD_WriteData_(' ');
	}
}

void GLCD_ClearGraphic(void)
{
	unsigned int i;
	GLCD_SetCursorAddress_(SED1335_GRAPHICSTART);
	GLCD_WriteCommand_(SED1335_MWRITE);
	for(i = 0; i < (SED1335_LENGTH_X_BYTES * SED1335_LENGTH_Y_PIXELS); i++)
	{
		GLCD_WriteData_(0x00);
	}
}

void GLCD_ClearTextArea(int x, int y, int x2, int y2)
{
	int i, j;
	
	for(i = y; i <= y2; i++)
	{
		GLCD_TextGoTo_(x,i);
		GLCD_WriteCommand_(SED1335_MWRITE);
		for(j = 0; j < x2 - x; j++)
		{
			GLCD_WriteData_(' ');
		}	
	}	
}

void GLCD_ClearGraphicArea(int x, int y, int x2, int y2)
{
	unsigned int i, j;
	
	for(i = y; i < y2; i++)
	{
		GLCD_SetCursorAddress_(SED1335_GRAPHICSTART + (i * SED1335_LENGTH_X_BYTES) + x);
		GLCD_WriteCommand_(SED1335_MWRITE);
		for(j = 0; j < x2 - x; j++)
			GLCD_WriteData_(0x00);
	}	
}


	

void GLCD_SetPixel_(unsigned int x,unsigned int y, int color)
{
	unsigned char tmp = 0;
	unsigned int address = SED1335_GRAPHICSTART + (SED1335_LENGTH_X_BYTES * y) + (x/8); 
	GLCD_SetCursorAddress_(address);

	GLCD_WriteCommand_(SED1335_MREAD);
	tmp = color;

	GLCD_SetCursorAddress_(address);
	GLCD_WriteCommand_(SED1335_MWRITE);
	GLCD_WriteData_(tmp);
}

void GLCD_SetPixels_(unsigned int x, unsigned int y, unsigned char pixels)
{
	unsigned int address = SED1335_GRAPHICSTART + (SED1335_LENGTH_X_BYTES * y) + (x); 
	GLCD_SetCursorAddress_(address);

	GLCD_WriteCommand_(SED1335_MWRITE);
	unsigned char pix = ~pixels; //inversion
	GLCD_WriteData_(pix);
	//GLCD_WriteData_(pixels);
}

void GLCD_SetCursorAddress_(unsigned int address)
{
	GLCD_WriteCommand_(SED1335_CSRW);
	GLCD_WriteData_((unsigned char)(address & 0xFF));
	GLCD_WriteData_((unsigned char)(address >> 8));
}

void GLCD_TextGoTo_(unsigned char x, unsigned char y) {
	GLCD_SetCursorAddress_((y * 40) + x);
	//GLCD_SetCursorAddress_((y) + x);
}

void GLCD_GraphicGoTo_(unsigned int x, unsigned int y)
{
	GLCD_SetCursorAddress_(SED1335_GRAPHICSTART + (y * 40) + x/8);
}

void GLCD_LoadFont8x8_(void) {
	//ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ CP1251
	int i, j;	
	GLCD_WriteCommand_(SED1335_CSRW);
	GLCD_WriteData_(SED1335_CGRAM1_ADR_SAGL);			
	GLCD_WriteData_(SED1335_CGRAM1_ADR_SAGH);		
	GLCD_WriteCommand_(SED1335_MWRITE);	
	for(i = 0; i < 32; i++)	{
		for(j = 0; j < 8; j++)		{	
			GLCD_WriteData_(FONT_8x8[i][j]);
		}	
	}	
	GLCD_WriteCommand_(SED1335_CSRW);
	GLCD_WriteData_(SED1335_CGRAM2_ADR_SAGL);			
	GLCD_WriteData_(SED1335_CGRAM2_ADR_SAGH);				
	GLCD_WriteCommand_(SED1335_MWRITE);		
	for(i = 32; i < 64; i++)	{
		for(j = 0; j < 8; j++)		{	
			GLCD_WriteData_(FONT_8x8[i][j]);
		}
	}
}

void GLCD_LoadFont8x16_(void)
{
		// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ CP1251.
	// ï¿½ï¿½ï¿½ï¿½ï¿½ 256 ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½. 128 - ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½, 64 - ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½, 64 - ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½.
	GLCD_WriteCommand_(SED1335_CSRW);
	GLCD_WriteData_(SED1335_CGRAM_ADR_SAGL);			
	GLCD_WriteData_(SED1335_CGRAM_ADR_SAGH);	
	
	GLCD_WriteCommand_(SED1335_MWRITE);
	
	int i;
	for(i = 0; i < 128 * 16; i++)
		{}//GLCD_WriteData_(Classic_Console_Eng8x16[i]);
		
	for(i = 0; i < 64 * 16; i++)
		GLCD_WriteData_(0xFF);	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ 64 ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½!
		
	for(i = 0; i < 64 * 16; i++)
		{}//GLCD_WriteData_(Classic_Console_Rus8x16[i]);
}