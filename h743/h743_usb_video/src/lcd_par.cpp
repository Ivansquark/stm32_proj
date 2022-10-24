#include "lcd_par.h"
LCD_par* LCD_par::pThis = nullptr;

LCD_par::LCD_par() {
    init();
    display_init();
    pThis = this;
}

void LCD_par::writeData(uint16_t data) {    
    bit_setting(data>>8);
    DATA();
    GPIOD->BSRR |= (bits.bitMASK_D_set | (bits.bitMASK_D_reset << 16));
    GPIOE->BSRR |= (bits.bitMASK_E_set | (bits.bitMASK_E_reset << 16));
    WRITE(); 
    bit_setting(data);
    GPIOD->BSRR |= (bits.bitMASK_D_set | (bits.bitMASK_D_reset << 16));
    GPIOE->BSRR |= (bits.bitMASK_E_set | (bits.bitMASK_E_reset << 16));
    WRITE();
}
void LCD_par::writeCmd(uint16_t cmd) {
    bit_setting(cmd>>8);
    CMD();
    GPIOD->BSRR |= (bits.bitMASK_D_set | (bits.bitMASK_D_reset << 16));
    GPIOE->BSRR |= (bits.bitMASK_E_set | (bits.bitMASK_E_reset << 16));
    WRITE(); 
    bit_setting(cmd);
    GPIOD->BSRR |= (bits.bitMASK_D_set | (bits.bitMASK_D_reset << 16));
    GPIOE->BSRR |= (bits.bitMASK_E_set | (bits.bitMASK_E_reset << 16));
    WRITE();
}
void LCD_par::writeReg(uint16_t cmd, uint16_t data) {
    writeCmd(cmd);
    writeData(data);
}

void LCD_par::reset() {
    GP_Timers::pThis[0]->delay_ms(50);
    RES_off();
    GP_Timers::pThis[0]->delay_ms(150);
    RES_on();
}

void LCD_par::bit_setting(uint8_t data) {
    bits.bitMASK_D_reset=0; bits.bitMASK_D_set=0;
    bits.bitMASK_E_reset=0; bits.bitMASK_E_set=0;
    if(data & 1) { bits.bitMASK_D_set |= (1<<14);  }
    else {bits.bitMASK_D_reset |= (1<<14);}
    if((data>>1) & 1) {bits. bitMASK_D_set |= (1<<15);  }
    else {bits.bitMASK_D_reset |= (1<<15);}
    if(data>>2 & 1) { bits.bitMASK_D_set |= (1<<0);  }
    else {bits.bitMASK_D_reset |= (1<<0);}
    if((data>>3) & 1) {bits. bitMASK_D_set |= (1<<1);  }
    else {bits.bitMASK_D_reset |= (1<<1);}

    if((data>>4) & 1) {bits.bitMASK_E_set |= (1<<7);}
    else {bits.bitMASK_E_reset |= (1<<7);}
    if((data>>5) & 1) {bits.bitMASK_E_set |= (1<<8);}
    else {bits.bitMASK_E_reset |= (1<<8);}
    if((data>>6) & 1) {bits.bitMASK_E_set |= (1<<9);}
    else {bits.bitMASK_E_reset |= (1<<9);}
    if((data>>7) & 1) {bits.bitMASK_E_set |= (1<<10);}
    else {bits.bitMASK_E_reset |= (1<<10);}
}

void LCD_par::init(){
    //GPIOS init:
    //PD11 - FMC-A16(RS - D/C)  D14-FMC-D0  D15-FMC-D1 
    //D0-FMC_D2, D1-FMC_D3, D4-FMC_NOE READ_EN, D5-FMC_NWE Write_EN, D7-FMC_NE1 (CS), D2-TS, D3-PEN
    //! PD11 - FMC-A16(RS - D/C)  D14-FMC-D0  D15-FMC-D1
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN; //0x60000000 - D  0x60000001 - C
    GPIOD->MODER |= (GPIO_MODER_MODE11_0 | GPIO_MODER_MODE14_0 | GPIO_MODER_MODE15_0) | 
                    (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE7_0 |
                    GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0); 
    GPIOD->MODER &=~ (GPIO_MODER_MODE11_1 | GPIO_MODER_MODE14_1 | GPIO_MODER_MODE15_1 |
                      GPIO_MODER_MODE0_1 | GPIO_MODER_MODE1_1 | GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1 | GPIO_MODER_MODE7_1 |
                      GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1); 
    GPIOD->OSPEEDR |= (GPIO_OSPEEDR_OSPEED11 | GPIO_OSPEEDR_OSPEED14 | GPIO_OSPEEDR_OSPEED15 |
                    GPIO_OSPEEDR_OSPEED0 | GPIO_OSPEEDR_OSPEED1 | GPIO_OSPEEDR_OSPEED4 | GPIO_OSPEEDR_OSPEED5 | 
                    GPIO_OSPEEDR_OSPEED7 | GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3);
    GPIOD->BSRR |= (GPIO_BSRR_BS4 | GPIO_BSRR_BS5 | GPIO_BSRR_BS7);
    //E7-FMC_D4, E8-FMC_D5, E9-FMC_D6, E10-FMC_D7,
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
    GPIOE->MODER |= (GPIO_MODER_MODE7_0 | GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0 | GPIO_MODER_MODE10_0);
    GPIOE->MODER &=~ (GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1); 
    GPIOE->OSPEEDR |= (GPIO_OSPEEDR_OSPEED7 | GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9 | GPIO_OSPEEDR_OSPEED10);   
    //! A-15 RES LCD
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
    GPIOA->MODER |= (GPIO_MODER_MODE15_0);
    GPIOA->MODER &=~ (GPIO_MODER_MODE15_1); 
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED15);           
}

void LCD_par::display_init() {
    CS_on();
    reset();

    writeReg(0xF000,0x0055);
    writeReg(0xF001,0x00AA);
    writeReg(0xF002,0x0052);
    writeReg(0xF003,0x0008);
    writeReg(0xF004,0x0001);
    //# AVDD: manual); LCD_WR_DATA(
	writeReg(0xB600,0x0034);
	writeReg(0xB601,0x0034);
	writeReg(0xB602,0x0034);
	writeReg(0xB000,0x000D);//09
	writeReg(0xB001,0x000D);
	writeReg(0xB002,0x000D);
    //# AVEE: manual); LCD_WR_DATA( -6V
	writeReg(0xB700,0x0024);
	writeReg(0xB701,0x0024);
	writeReg(0xB702,0x0024);
	writeReg(0xB100,0x000D);
	writeReg(0xB101,0x000D);
	writeReg(0xB102,0x000D);
    //#Power Control for
	//VCL
	writeReg(0xB800,0x0024);
	writeReg(0xB801,0x0024);
	writeReg(0xB802,0x0024);
	writeReg(0xB200,0x0000);
    //# VGH: Clamp Enable); LCD_WR_DATA(
	writeReg(0xB900,0x0024);
	writeReg(0xB901,0x0024);
	writeReg(0xB902,0x0024);
	writeReg(0xB300,0x0005);
	writeReg(0xB301,0x0005);
	writeReg(0xB302,0x0005);
    //# VGL(LVGL):
	writeReg(0xBA00,0x0034);
	writeReg(0xBA01,0x0034);
	writeReg(0xBA02,0x0034);
    //# VGL_REG(VGLO)
	writeReg(0xB500,0x000B);
	writeReg(0xB501,0x000B);
	writeReg(0xB502,0x000B);
    //# VGMP/VGSP:
	writeReg(0xBC00,0x0000);
	writeReg(0xBC01,0x00A3);
	writeReg(0xBC02,0x0000);
    //# VGMN/VGSN
	writeReg(0xBD00,0x0000);
	writeReg(0xBD01,0x00A3);
	writeReg(0xBD02,0x0000);
    //# VCOM=-0.1
	writeReg(0xBE00,0x0000);
	writeReg(0xBE01,0x0063);//4f

    //#R+
	writeReg(0xD100,0x0000);
	writeReg(0xD101,0x0037);
	writeReg(0xD102,0x0000);
	writeReg(0xD103,0x0052);
	writeReg(0xD104,0x0000);
	writeReg(0xD105,0x007B);
	writeReg(0xD106,0x0000);
	writeReg(0xD107,0x0099);
	writeReg(0xD108,0x0000);
	writeReg(0xD109,0x00B1);
	writeReg(0xD10A,0x0000);
	writeReg(0xD10B,0x00D2);
	writeReg(0xD10C,0x0000);
	writeReg(0xD10D,0x00F6);
	writeReg(0xD10E,0x0001);
	writeReg(0xD10F,0x0027);
	writeReg(0xD110,0x0001);
	writeReg(0xD111,0x004E);
	writeReg(0xD112,0x0001);
	writeReg(0xD113,0x008C);
	writeReg(0xD114,0x0001);
	writeReg(0xD115,0x00BE);
	writeReg(0xD116,0x0002);
	writeReg(0xD117,0x000B);
	writeReg(0xD118,0x0002);
	writeReg(0xD119,0x0048);
	writeReg(0xD11A,0x0002);
	writeReg(0xD11B,0x004A);
	writeReg(0xD11C,0x0002);
	writeReg(0xD11D,0x007E);
	writeReg(0xD11E,0x0002);
	writeReg(0xD11F,0x00BC);
	writeReg(0xD120,0x0002);
	writeReg(0xD121,0x00E1);
	writeReg(0xD122,0x0003);
	writeReg(0xD123,0x0010);
	writeReg(0xD124,0x0003);
	writeReg(0xD125,0x0031);
	writeReg(0xD126,0x0003);
	writeReg(0xD127,0x005A);
	writeReg(0xD128,0x0003);
	writeReg(0xD129,0x0073);
	writeReg(0xD12A,0x0003);
	writeReg(0xD12B,0x0094);
	writeReg(0xD12C,0x0003);
	writeReg(0xD12D,0x009F);
	writeReg(0xD12E,0x0003);
	writeReg(0xD12F,0x00B3);
	writeReg(0xD130,0x0003);
	writeReg(0xD131,0x00B9);
	writeReg(0xD132,0x0003);
	writeReg(0xD133,0x00C1);
    //#G+
	writeReg(0xD200,0x0000);
	writeReg(0xD201,0x0037);
	writeReg(0xD202,0x0000);
	writeReg(0xD203,0x0052);
	writeReg(0xD204,0x0000);
	writeReg(0xD205,0x007B);
	writeReg(0xD206,0x0000);
	writeReg(0xD207,0x0099);
	writeReg(0xD208,0x0000);
	writeReg(0xD209,0x00B1);
	writeReg(0xD20A,0x0000);
	writeReg(0xD20B,0x00D2);
	writeReg(0xD20C,0x0000);
	writeReg(0xD20D,0x00F6);
	writeReg(0xD20E,0x0001);
	writeReg(0xD20F,0x0027);
	writeReg(0xD210,0x0001);
	writeReg(0xD211,0x004E);
	writeReg(0xD212,0x0001);
	writeReg(0xD213,0x008C);
	writeReg(0xD214,0x0001);
	writeReg(0xD215,0x00BE);
	writeReg(0xD216,0x0002);
	writeReg(0xD217,0x000B);
	writeReg(0xD218,0x0002);
	writeReg(0xD219,0x0048);
	writeReg(0xD21A,0x0002);
	writeReg(0xD21B,0x004A);
	writeReg(0xD21C,0x0002);
	writeReg(0xD21D,0x007E);
	writeReg(0xD21E,0x0002);
	writeReg(0xD21F,0x00BC);
	writeReg(0xD220,0x0002);
	writeReg(0xD221,0x00E1);
	writeReg(0xD222,0x0003);
	writeReg(0xD223,0x0010);
	writeReg(0xD224,0x0003);
	writeReg(0xD225,0x0031);
	writeReg(0xD226,0x0003);
	writeReg(0xD227,0x005A);
	writeReg(0xD228,0x0003);
	writeReg(0xD229,0x0073);
	writeReg(0xD22A,0x0003);
	writeReg(0xD22B,0x0094);
	writeReg(0xD22C,0x0003);
	writeReg(0xD22D,0x009F);
	writeReg(0xD22E,0x0003);
	writeReg(0xD22F,0x00B3);
	writeReg(0xD230,0x0003);
	writeReg(0xD231,0x00B9);
	writeReg(0xD232,0x0003);
	writeReg(0xD233,0x00C1);
    //#B+
	writeReg(0xD300,0x0000);
	writeReg(0xD301,0x0037);
	writeReg(0xD302,0x0000);
	writeReg(0xD303,0x0052);
	writeReg(0xD304,0x0000);
	writeReg(0xD305,0x007B);
	writeReg(0xD306,0x0000);
	writeReg(0xD307,0x0099);
	writeReg(0xD308,0x0000);
	writeReg(0xD309,0x00B1);
	writeReg(0xD30A,0x0000);
	writeReg(0xD30B,0x00D2);
	writeReg(0xD30C,0x0000);
	writeReg(0xD30D,0x00F6);
	writeReg(0xD30E,0x0001);
	writeReg(0xD30F,0x0027);
	writeReg(0xD310,0x0001);
	writeReg(0xD311,0x004E);
	writeReg(0xD312,0x0001);
	writeReg(0xD313,0x008C);
	writeReg(0xD314,0x0001);
	writeReg(0xD315,0x00BE);
	writeReg(0xD316,0x0002);
	writeReg(0xD317,0x000B);
	writeReg(0xD318,0x0002);
	writeReg(0xD319,0x0048);
	writeReg(0xD31A,0x0002);
	writeReg(0xD31B,0x004A);
	writeReg(0xD31C,0x0002);
	writeReg(0xD31D,0x007E);
	writeReg(0xD31E,0x0002);
	writeReg(0xD31F,0x00BC);
	writeReg(0xD320,0x0002);
	writeReg(0xD321,0x00E1);
	writeReg(0xD322,0x0003);
	writeReg(0xD323,0x0010);
	writeReg(0xD324,0x0003);
	writeReg(0xD325,0x0031);
	writeReg(0xD326,0x0003);
	writeReg(0xD327,0x005A);
	writeReg(0xD328,0x0003);
	writeReg(0xD329,0x0073);
	writeReg(0xD32A,0x0003);
	writeReg(0xD32B,0x0094);
	writeReg(0xD32C,0x0003);
	writeReg(0xD32D,0x009F);
	writeReg(0xD32E,0x0003);
	writeReg(0xD32F,0x00B3);
	writeReg(0xD330,0x0003);
	writeReg(0xD331,0x00B9);
	writeReg(0xD332,0x0003);
	writeReg(0xD333,0x00C1);
    //#R-///////////////////////////////////////////
	writeReg(0xD400,0x0000);
	writeReg(0xD401,0x0037);
	writeReg(0xD402,0x0000);
	writeReg(0xD403,0x0052);
	writeReg(0xD404,0x0000);
	writeReg(0xD405,0x007B);
	writeReg(0xD406,0x0000);
	writeReg(0xD407,0x0099);
	writeReg(0xD408,0x0000);
	writeReg(0xD409,0x00B1);
	writeReg(0xD40A,0x0000);
	writeReg(0xD40B,0x00D2);
	writeReg(0xD40C,0x0000);
	writeReg(0xD40D,0x00F6);
	writeReg(0xD40E,0x0001);
	writeReg(0xD40F,0x0027);
	writeReg(0xD410,0x0001);
	writeReg(0xD411,0x004E);
	writeReg(0xD412,0x0001);
	writeReg(0xD413,0x008C);
	writeReg(0xD414,0x0001);
	writeReg(0xD415,0x00BE);
	writeReg(0xD416,0x0002);
	writeReg(0xD417,0x000B);
	writeReg(0xD418,0x0002);
	writeReg(0xD419,0x0048);
	writeReg(0xD41A,0x0002);
	writeReg(0xD41B,0x004A);
	writeReg(0xD41C,0x0002);
	writeReg(0xD41D,0x007E);
	writeReg(0xD41E,0x0002);
	writeReg(0xD41F,0x00BC);
	writeReg(0xD420,0x0002);
	writeReg(0xD421,0x00E1);
	writeReg(0xD422,0x0003);
	writeReg(0xD423,0x0010);
	writeReg(0xD424,0x0003);
	writeReg(0xD425,0x0031);
	writeReg(0xD426,0x0003);
	writeReg(0xD427,0x005A);
	writeReg(0xD428,0x0003);
	writeReg(0xD429,0x0073);
	writeReg(0xD42A,0x0003);
	writeReg(0xD42B,0x0094);
	writeReg(0xD42C,0x0003);
	writeReg(0xD42D,0x009F);
	writeReg(0xD42E,0x0003);
	writeReg(0xD42F,0x00B3);
	writeReg(0xD430,0x0003);
	writeReg(0xD431,0x00B9);
	writeReg(0xD432,0x0003);
	writeReg(0xD433,0x00C1);
    //#G-//////////////////////////////////////////////
	writeReg(0xD500,0x0000);
	writeReg(0xD501,0x0037);
	writeReg(0xD502,0x0000);
	writeReg(0xD503,0x0052);
	writeReg(0xD504,0x0000);
	writeReg(0xD505,0x007B);
	writeReg(0xD506,0x0000);
	writeReg(0xD507,0x0099);
	writeReg(0xD508,0x0000);
	writeReg(0xD509,0x00B1);
	writeReg(0xD50A,0x0000);
	writeReg(0xD50B,0x00D2);
	writeReg(0xD50C,0x0000);
	writeReg(0xD50D,0x00F6);
	writeReg(0xD50E,0x0001);
	writeReg(0xD50F,0x0027);
	writeReg(0xD510,0x0001);
	writeReg(0xD511,0x004E);
	writeReg(0xD512,0x0001);
	writeReg(0xD513,0x008C);
	writeReg(0xD514,0x0001);
	writeReg(0xD515,0x00BE);
	writeReg(0xD516,0x0002);
	writeReg(0xD517,0x000B);
	writeReg(0xD518,0x0002);
	writeReg(0xD519,0x0048);
	writeReg(0xD51A,0x0002);
	writeReg(0xD51B,0x004A);
	writeReg(0xD51C,0x0002);
	writeReg(0xD51D,0x007E);
	writeReg(0xD51E,0x0002);
	writeReg(0xD51F,0x00BC);
	writeReg(0xD520,0x0002);
	writeReg(0xD521,0x00E1);
	writeReg(0xD522,0x0003);
	writeReg(0xD523,0x0010);
	writeReg(0xD524,0x0003);
	writeReg(0xD525,0x0031);
	writeReg(0xD526,0x0003);
	writeReg(0xD527,0x005A);
	writeReg(0xD528,0x0003);
	writeReg(0xD529,0x0073);
	writeReg(0xD52A,0x0003);
	writeReg(0xD52B,0x0094);
	writeReg(0xD52C,0x0003);
	writeReg(0xD52D,0x009F);
	writeReg(0xD52E,0x0003);
	writeReg(0xD52F,0x00B3);
	writeReg(0xD530,0x0003);
	writeReg(0xD531,0x00B9);
	writeReg(0xD532,0x0003);
	writeReg(0xD533,0x00C1);
    //#B-///////////////////////////////
	writeReg(0xD600,0x0000);
	writeReg(0xD601,0x0037);
	writeReg(0xD602,0x0000);
	writeReg(0xD603,0x0052);
	writeReg(0xD604,0x0000);
	writeReg(0xD605,0x007B);
	writeReg(0xD606,0x0000);
	writeReg(0xD607,0x0099);
	writeReg(0xD608,0x0000);
	writeReg(0xD609,0x00B1);
	writeReg(0xD60A,0x0000);
	writeReg(0xD60B,0x00D2);
	writeReg(0xD60C,0x0000);
	writeReg(0xD60D,0x00F6);
	writeReg(0xD60E,0x0001);
	writeReg(0xD60F,0x0027);
	writeReg(0xD610,0x0001);
	writeReg(0xD611,0x004E);
	writeReg(0xD612,0x0001);
	writeReg(0xD613,0x008C);
	writeReg(0xD614,0x0001);
	writeReg(0xD615,0x00BE);
	writeReg(0xD616,0x0002);
	writeReg(0xD617,0x000B);
	writeReg(0xD618,0x0002);
	writeReg(0xD619,0x0048);
	writeReg(0xD61A,0x0002);
	writeReg(0xD61B,0x004A);
	writeReg(0xD61C,0x0002);
	writeReg(0xD61D,0x007E);
	writeReg(0xD61E,0x0002);
	writeReg(0xD61F,0x00BC);
	writeReg(0xD620,0x0002);
	writeReg(0xD621,0x00E1);
	writeReg(0xD622,0x0003);
	writeReg(0xD623,0x0010);
	writeReg(0xD624,0x0003);
	writeReg(0xD625,0x0031);
	writeReg(0xD626,0x0003);
	writeReg(0xD627,0x005A);
	writeReg(0xD628,0x0003);
	writeReg(0xD629,0x0073);
	writeReg(0xD62A,0x0003);
	writeReg(0xD62B,0x0094);
	writeReg(0xD62C,0x0003);
	writeReg(0xD62D,0x009F);
	writeReg(0xD62E,0x0003);
	writeReg(0xD62F,0x00B3);
	writeReg(0xD630,0x0003);
	writeReg(0xD631,0x00B9);
	writeReg(0xD632,0x0003);
	writeReg(0xD633,0x00C1);


    //#Enable Page0
	writeReg(0xF000,0x0055);
	writeReg(0xF001,0x00AA);
	writeReg(0xF002,0x0052);
	writeReg(0xF003,0x0008);
	writeReg(0xF004,0x0000);

    //# RGB I/F Setting
	writeReg(0xB000,0x0008);
	writeReg(0xB001,0x0005);
	writeReg(0xB002,0x0002);
	writeReg(0xB003,0x0005);
	writeReg(0xB004,0x0002);

    // SDT
    writeReg(0xB600, 0x0008);
    writeReg(0xB500, 0x0050); //0x50  480x800

    //## Gate EQ:
	writeReg(0xB700,0x0000);
	writeReg(0xB701,0x0000);
	//## Source EQ:
	writeReg(0xB800,0x0001);
	writeReg(0xB801,0x0005);
	writeReg(0xB802,0x0005);
	writeReg(0xB803,0x0005);
	//# Inversion: Column inversion (NVT)
	writeReg(0xBC00,0x0000);
	writeReg(0xBC01,0x0000);
	writeReg(0xBC02,0x0000);
	//# BOE's Setting(default)
	writeReg(0xCC00,0x0003);
	writeReg(0xCC01,0x0000);
	writeReg(0xCC02,0x0000);

    //# Display Timing:
	writeReg(0xBD00,0x0001);
	writeReg(0xBD01,0x0084);
	writeReg(0xBD02,0x0007);
	writeReg(0xBD03,0x0031);
	writeReg(0xBD04,0x0000);
	writeReg(0xBA00,0x0001);
	writeReg(0xFF00,0x00AA);
	writeReg(0xFF01,0x0055);
	writeReg(0xFF02,0x0025);
	writeReg(0xFF03,0x0001);
    
    writeReg(0x3500, 0x0000); 
    writeReg(0x3600, 0x0000); //MADCTL (orientation) row/column direction
    writeReg(0x3A00, 0x0055); //COLMOD   interface pixel format    
	writeCmd(0x0100); //soft reset
    GP_Timers::pThis[0]->delay_ms(1);
    writeReg(0x1100, 0x0000); //sleep out
    GP_Timers::pThis[0]->delay_ms(1200);
    writeReg(0x1300, 0x0000); //Partial out
    GP_Timers::pThis[0]->delay_ms(10);
    //writeReg(0x2000, 0x0000); //inversion off
    //GP_Timers::pThis[0]->delay_ms(1);
    writeReg(0x2300, 0x0000); //all pixel on
    //GP_Timers::pThis[0]->delay_ms(1); 
    //writeReg(0x3800, 0x0000); //idle off
    //GP_Timers::pThis[0]->delay_ms(1); 
    //! MADCTL - MY-ROW_order, MX-COLUMN_order, MV-COLUMN/ROW_exchange ML-vertical_refresh order
    //          RGB-order MH-Horizontal refresh order RSMX-Flip_hor RSMY-Flip_ver 
    
    //Vertical refresh order
    //Horizontal refresh order
    
    GP_Timers::pThis[0]->delay_ms(10);
    writeReg(0x2900, 0x0000);//DISP on
    GP_Timers::pThis[0]->delay_ms(1200);

    writeReg(0x2A00, 0x0000); //---start col |_| end col
    writeReg(0x2A01, 0x0000); //Column set start 0x0000 two by 8 bits
    writeReg(0x2A02, 0x0003); //---
    writeReg(0x2A03, 0x0020); //Column set end 0x0320 (800) two by 8 bits    
    //GP_Timers::pThis[0]->delay_ms(1);
    writeReg(0x2B00, 0x0000); //---
    writeReg(0x2B01, 0x0000); //Row set start 0x0000 two by 8 bits
    writeReg(0x2B02, 0x0003); //---
    writeReg(0x2B03, 0x0020); //Row set end 0x01E0 (480) two by 8 bits   
    GP_Timers::pThis[0]->delay_ms(1);
    for(int i=0; i<=1000;i++){
        writeReg(0x2C00, 0xFFFF);//RAMWR -- write 1 BLACK pixel
    }
}