#include "fmc.h"

void Nor_LCD_interface::writeData(uint16_t data) {
    LCD->DATA = data>>8;    
	LCD->DATA = data; 
	//for(int i=0;i<10;i++) {
	//	__ASM("nop");
	//}
}
void Nor_LCD_interface::writeCmd(uint16_t cmd) {
    LCD->CMD = cmd>>8;
    LCD->CMD = cmd;
	//for(int i=0;i<10;i++) {
	//	__ASM("nop");
	//}   
}
void Nor_LCD_interface::writeReg(const uint16_t LCD_REG, const uint16_t reg_val) {
    LCD->CMD = LCD_REG>>8;
    LCD->CMD = LCD_REG;
    LCD->DATA = reg_val>>8;
    //LCD->DATA = 0x00;
    LCD->DATA = reg_val;
}

void Nor_LCD_interface::setColumn(uint16_t x) {
    writeReg(0x2A00, ((x >> 8) & 0x00FF)); //---start col |_| end col
    writeReg(0x2A01, (x & 0x00FF)); //Column set start 0x0000 two by 8 bits
}
void Nor_LCD_interface::setRow(uint16_t y) {
    writeReg(0x2B00, ((y >> 8) & 0x00FF)); //Row set start 0x0000 two by 8 bits
    writeReg(0x2B01, ((y) & 0x00FF)); //---
}
void Nor_LCD_interface::setRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    writeReg(0x2A00, ((x1 >> 8))); //---start col |_| end col
    writeReg(0x2A01, (x1 & 0x00FF)); //Column set start 0x0000 two by 8 bits
    writeReg(0x2A02, ((x2 >> 8))); //---
    writeReg(0x2A03, (x2 & 0x00FF)); //Column set end 0x0320 (800) two by 8 bits    
    //GP_Timers::pThis[0]->delay_ms(1);
    writeReg(0x2B00, ((y1 >> 8))); //---
    writeReg(0x2B01, (y1 & 0x00FF)); //Row set start 0x0000 two by 8 bits
    writeReg(0x2B02, ((y2 >> 8) )); //---
    writeReg(0x2B03, (y2 & 0x00FF)); //Row set end 0x01E0 (480) two by 8 bits
}

char* Nor_LCD_interface::intToChar(uint32_t x) {
    uint32_t y=x;        
    uint8_t count=0;
    while (y>0) {
        y=y/10;  count++; 
    } // counting numbers of figures
    y=x;    
    arrSize=count;
    if(x==0) {arrSize=1;arr[arrSize-1]='0';arr[arrSize]='\0'; return arr;} 
    for(uint8_t i=0;i<arrSize;i++) {            
        int x=y%10;
        if(x==0) {arr[arrSize-1-i]='0';} if(x==1) {arr[arrSize-1-i]='1';}
        if(x==2) {arr[arrSize-1-i]='2';} if(x==3) {arr[arrSize-1-i]='3';}
        if(x==4) {arr[arrSize-1-i]='4';} if(x==5) {arr[arrSize-1-i]='5';}
        if(x==6) {arr[arrSize-1-i]='6';} if(x==7) {arr[arrSize-1-i]='7';}
        if(x==8) {arr[arrSize-1-i]='8';} if(x==9) {arr[arrSize-1-i]='9';}
        y=y/10;        
    }
    if(arrSize+1<10) {
        //strcat(arr+arrSize,'\0'); 
        arr[arrSize]='\0';
    }
    return arr;
}

char* Nor_LCD_interface::floatTochar(float x) {        
    sprintf(arrFloat, "%.3f", x);    
    return arrFloat;       
}


//------------------------------------  LCD -----------------------------------------------
Nor_LCD* Nor_LCD::pThis = nullptr;

Nor_LCD::Nor_LCD() {    
    fmc_lcd_init();
    res_off();
    display_init();
    pThis = this;
}
void Nor_LCD::fmc_lcd_init() {
    // GPIOS Enable: 
    //! PD11 - FMC-A16(RS - D/C)  D14-FMC-D0  D15-FMC-D1
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN; //0x60000000 - D  0x60000001 - C
    GPIOD->MODER |= (GPIO_MODER_MODE11_1 | GPIO_MODER_MODE14_1 | GPIO_MODER_MODE15_1); 
    GPIOD->MODER &=~ (GPIO_MODER_MODE11_0 | GPIO_MODER_MODE14_0 | GPIO_MODER_MODE15_0); 
    GPIOD->OSPEEDR |= (GPIO_OSPEEDR_OSPEED11 | GPIO_OSPEEDR_OSPEED14 | GPIO_OSPEEDR_OSPEED15);
    GPIOD->AFR[1] |= ((12<<12)|(12<<24)|(12<<28));
    //! D0-FMC_D2, D1-FMC_D3, D4-FMC_NOE READ_EN, D5-FMC_NWE Write_EN, D7-FMC_NE1 (CS), D2-TS, D3-PEN
    GPIOD->MODER |= (GPIO_MODER_MODE0_1 | GPIO_MODER_MODE1_1 | GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1 | GPIO_MODER_MODE7_1);
    GPIOD->MODER &=~ (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE7_0); 
    GPIOD->OSPEEDR |= (GPIO_OSPEEDR_OSPEED0 | GPIO_OSPEEDR_OSPEED1 | GPIO_OSPEEDR_OSPEED4 | GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED7);
    GPIOD->AFR[0] |= ((12<<0) | (12<<4) | (12<<16) | (12<<20) | (12<<28));
    GPIOD->MODER |= (GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0);
    GPIOD->MODER &=~ (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1); 
    GPIOD->OSPEEDR |= (GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3);
    //! E7-FMC_D4, E8-FMC_D5, E9-FMC_D6, E10-FMC_D7,
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;
    GPIOE->MODER |= (GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1);
    GPIOE->MODER &=~ (GPIO_MODER_MODE7_0 | GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0 | GPIO_MODER_MODE10_0); 
    GPIOE->OSPEEDR |= (GPIO_OSPEEDR_OSPEED7 | GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9 | GPIO_OSPEEDR_OSPEED10);
    GPIOE->AFR[0] |= (12<<28);
    GPIOE->AFR[1] |= ((12<<0)|(12<<4)|(12<<8));
    //! A-15 RES LCD
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
    GPIOA->MODER |= (GPIO_MODER_MODE15_0);
    GPIOA->MODER &=~ (GPIO_MODER_MODE15_1); 
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED15);
	
	//! MPU CONFIG
	__DMB();  	
  	SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk; // Disable fault exceptions   	
  	MPU->CTRL = 0; // MPU disable Disable the MPU and clear the control register
	MPU->RNR = 0; //Set the Region number
	MPU->RBAR = 0X60000000; // Base Address LCD
	MPU->RASR = 0;
	MPU->RASR |= 1; //enable
	MPU->RASR |= (0x1B<<1);//size 0x1B - 256 MBytes
	MPU->RASR |= (0<<8); //subregion disable = 0;
	MPU->RASR |= (1<<16); //Bufferable enable = 1;
	MPU->RASR |= (0<<17); //Casheable disable = 0;
	MPU->RASR |= (0<<18); //Shearable disable = 0;
	MPU->RASR |= (0<<19); // TEX: 0:0:0 = 0 TypeExtField
	MPU->RASR |= (3<<24); // Full access
	MPU->RASR |= (0<<28); // Instruction access disable bit 1: Instruction fetches disabled. 0: Instruction fetches enabled.
	//! Enable MPU
	#define MPU_Control 0x00000004  //MPU_PRIVILEGED_DEFAULT
	MPU->CTRL = (MPU_Control | MPU_CTRL_ENABLE_Msk);	  	
  	SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk; // Enable fault exceptions  	
  	__DSB();
  	__ISB(); // Ensure MPU setting take effects 

    //! HCLK3 clock source
    RCC->D1CCIPR &=~ RCC_D1CCIPR_FMCSEL;
	//RCC->D1CCIPR |= RCC_D1CCIPR_FMCSEL_0;
    //! FMC clock enable
    RCC->AHB3ENR |= RCC_AHB3ENR_FMCEN;    
    FMC_Bank1_R->BTCR[0] &=~ FMC_BCRx_ASYNCWAIT; // Wait signal during asynchronous transfers
    FMC_Bank1_R->BTCR[0] &=~ FMC_BCRx_BURSTEN;   // Burst enable bit
    FMC_Bank1_R->BTCR[0] &=~ FMC_BCRx_CBURSTRW;  // write 0 - async 1 - sycnc
    FMC_Bank1_R->BTCR[0] &=~ FMC_BCRx_CPSIZE;    // CRAM Page Size
    FMC_Bank1_R->BTCR[0] &=~ FMC_BCRx_EXTMOD;    // 1: values inside FMC_BWTR register are taken into account 0: NO BWTR
    FMC_Bank1_R->BTCR[0] |= FMC_BCRx_FACCEN;     // Flash access enable  (for LCD)
    FMC_Bank1_R->BTCR[0] |= FMC_BCRx_MBKEN;      // Memory bank enable bit
    FMC_Bank1_R->BTCR[0] &=~ FMC_BCRx_MTYP;
	FMC_Bank1_R->BTCR[0] |= FMC_BCRx_MTYP_1;     // 0 = SRAM 1 = CRAM 2 = NOR
    FMC_Bank1_R->BTCR[0] &=~ FMC_BCRx_MTYP_0;    // 0 = SRAM 1 = CRAM 2 = NOR
    FMC_Bank1_R->BTCR[0] &=~ FMC_BCRx_MUXEN;     // Multiplexing Address/Data
    FMC_Bank1_R->BTCR[0] &=~ FMC_BCRx_MWID;      // 00 = 8b 01 = 16b 10 = 32bits
    FMC_Bank1_R->BTCR[0] &=~ FMC_BCRx_WAITCFG;   //Wait timing configuration. 0: NWAIT before wait state 1: active during wait state
    FMC_Bank1_R->BTCR[0] &=~ FMC_BCRx_WAITEN;    // wait enable
    FMC_Bank1_R->BTCR[0] &=~ FMC_BCRx_WAITPOL;   // Wait signal polarity bit. 0: NWAIT active low. 1: NWAIT active high
    FMC_Bank1_R->BTCR[0] |= FMC_BCRx_WREN;       // Write enable bit
    FMC_Bank1_R->BTCR[0] &=~ FMC_BCR1_CCLKEN;    // Continious clock enable
    FMC_Bank1_R->BTCR[0] |= FMC_BCR1_FMCEN;      // FMC enable

    FMC_Bank1_R->BTCR[1] &=~ FMC_BTRx_ACCMOD;
	FMC_Bank1_R->BTCR[1] |= FMC_BTRx_ACCMOD_0;    // Access mode 0 = A, !!! 1=B !!!  , 2 = C, 3 = D Use w/EXTMOD bit
    FMC_Bank1_R->BTCR[1] &=~ FMC_BTRx_ADDHLD;
	FMC_Bank1_R->BTCR[1] |= FMC_BTRx_ADDHLD;   // Address-hold phase duration 1..F * 2 * HCLK
	FMC_Bank1_R->BTCR[1] &=~ FMC_BTRx_ADDSET;   // Address setup phase duration 0..F * HCLK
    FMC_Bank1_R->BTCR[1] |= FMC_BTRx_ADDSET_3;   // Address setup phase duration 0..F * HCLK
    FMC_Bank1_R->BTCR[1] &=~ FMC_BTRx_BUSTURN;  // Bus turnaround phase duration 0...F
	//FMC_Bank1_R->BTCR[1] |= FMC_BTRx_BUSTURN;
    FMC_Bank1_R->BTCR[1] &=~ FMC_BTRx_CLKDIV;
	//FMC_Bank1_R->BTCR[1] |= FMC_BTRx_CLKDIV;   // 0000: Reserved  0001:FMC_CLK period = 2 × fmc_ker_ck periods
    FMC_Bank1_R->BTCR[1] &=~ FMC_BTRx_DATAST;
	FMC_Bank1_R->BTCR[1] |= (FMC_BWTRx_DATAST_3); // Data-phase duration 0: Reserved  01: DATAST phase duration = 1 × fmc_ker_ck clock cycles
    FMC_Bank1_R->BTCR[1] &=~ FMC_BTRx_DATLAT;
	FMC_Bank1_R->BTCR[1] |= FMC_BTRx_DATLAT;    // Data latency for synchronous NOR Flash memory 0(2CK)...F(17CK) 
	//FMC_Bank1_R->BTCR[2] &=~ FMC_BWTRx_ACCMOD; //access mode a
	//FMC_Bank1_R->BTCR[2] |= FMC_BWTRx_ACCMOD_0;
	//FMC_Bank1_R->BTCR[2] &=~ FMC_BWTRx_BUSTURN; //access mode a
	//FMC_Bank1_R->BTCR[2] &=~ FMC_BWTRx_DATAST;
	//FMC_Bank1_R->BTCR[2] |= FMC_BWTRx_DATAST;
	//FMC_Bank1_R->BTCR[2] &=~ FMC_BWTRx_ADDHLD;
	//FMC_Bank1_R->BTCR[2] |= FMC_BWTRx_ADDHLD;
	//FMC_Bank1_R->BTCR[2] &=~ FMC_BWTRx_ADDSET;
	//FMC_Bank1_R->BTCR[2] |= FMC_BWTRx_ADDSET;
}

void Nor_LCD::display_init() {
	GP_Timers::pThis[0]->delay_us(50000);
    res_off();
    GP_Timers::pThis[0]->delay_us(100000);
    res_on();
    GP_Timers::pThis[0]->delay_us(50000);
    //writeReg(0x0100, 0x0000); //software reset
	
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
    
   	// SDT
    writeReg(0xB600, 0x0008);
    writeReg(0xB500, 0x0050); //0x50  480x800
	GP_Timers::pThis[0]->delay_us(1000);
    writeReg(0x3500, 0x0000); 
	//! MADCTL - MY-ROW_order, MX-COLUMN_order, MV-COLUMN/ROW_exchange ML-vertical_refresh order
    //          RGB-order MH-Horizontal refresh order RSMX-Flip_hor RSMY-Flip_ver        
    writeReg(0x3600, 0x00A0); //MADCTL (orientation) row/column direction  (C/R exchange)
    writeReg(0x3A00, 0x0055); //COLMOD   interface pixel format    
	//writeCmd(0x0100); //soft reset
    GP_Timers::pThis[0]->delay_us(1000);
    writeReg(0x1100, 0x0000); //sleep out
    writeReg(0x1300, 0x0000); //Partial out
    GP_Timers::pThis[0]->delay_us(120000);
    //writeReg(0x2000, 0x0000); //inversion off
    //GP_Timers::pThis[0]->delay_ms(1);
    //writeReg(0x2300, 0x0000); //all pixel on
    GP_Timers::pThis[0]->delay_us(100000); 
    //writeReg(0x3800, 0x0000); //idle off
    //GP_Timers::pThis[0]->delay_ms(1); 

    
    GP_Timers::pThis[0]->delay_us(10000);
    writeReg(0x2900, 0x0000);//DISP on
    GP_Timers::pThis[0]->delay_us(100000);
	/*
    writeReg(0x2A00, 0x0000); //---start col |_| end col
    writeReg(0x2A01, 0x0000); //Column set start 0x0000 two by 8 bits
    writeReg(0x2A02, 0x0003); //---
    writeReg(0x2A03, 0x0020); //Column set end 0x0320 (800) two by 8 bits    
    //GP_Timers::pThis[0]->delay_ms(1);
    writeReg(0x2B00, 0x0000); //---
    writeReg(0x2B01, 0x0000); //Row set start 0x0000 two by 8 bits
    writeReg(0x2B02, 0x0001); //---
    writeReg(0x2B03, 0x00E0); //Row set end 0x01E0 (480) two by 8 bits
	*/
	setRect(0,0,0x0320,0x01E0);
    GP_Timers::pThis[0]->delay_us(1000);
	writeCmd(0x2C00);
    for(int i=0; i<=(480*800);i++){
        writeData(0x00FF);//RAMWR -- write 1 BLACK pixel
    }
    
    
}

void Pixel::setPixel(uint16_t x, uint16_t y, uint16_t color){
    setRow(y);
    setColumn(x);
	writeCmd(0x2C00);
	writeData(color);
}
void Rect::drawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    setRect(x1,y1,x2,y2);
	GP_Timers::pThis[0]->delay_us(1000);
	writeCmd(0x2C00);
    for(int i=0; i<((x2-x1+1)*(y2-y1+1)); i++) {
        writeData(color);//RAMWR -- write 1 pixel
    }
}

//------------------------ FIGUREs -----------------------------------------
Figure::Figure(){
	SCB->CPACR |= ((3UL << 20)|(3UL << 22));  /* set CP10 and CP11 Full Access */ //FPU enable
}
void Figure::drawHorizontalLine(uint16_t x1, uint16_t y1, uint16_t length, uint16_t width, uint16_t color) {
	drawRect(x1, y1, x1 + length, y1 + width, color);
}

void Figure::drawVerticalLine(uint16_t x1, uint16_t y1, uint16_t length, uint16_t width, uint16_t color) {
	drawRect(x1, y1, x1 + width, y1 + length, color);
}

void Figure::drawFrame(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t width, uint16_t color) {
	drawHorizontalLine(x1, y1, x2 - x1, width/2, color);
	drawHorizontalLine(x1, y2 - width/2, x2 - x1, width/2, color);
	drawVerticalLine(x1, y1, y2 - y1, width/2, color);
	drawVerticalLine(x2 - width/2, y1, y2-y1, width/2, color);
}

void Figure::drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	float length1 = sqrtf((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
    for(uint16_t i=0;i<uint32_t(length1);i++) { //uint16_t(length)     
        //setPixel(x1+i,y1+sqrtf(i),col);  		  
        setPixel(x1+(float)(i*(x2-x1)/length1), y1 + (float)(i*(y2-y1)/length1), color);            
    }
}
void Figure::drawFatLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t width, uint16_t color) {
	float len = sqrtf((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
	int16_t dx = (x2-x1); int16_t dy = (y2-y1);
	int16_t ddx=0; int16_t ddy=0;
	for(int i=0; i<width; i++){
		if(dx>=0) {
			ddx = -((int16_t)(i*(float)(dy/len)));
			ddy = (int16_t)(i*(float)(dx/len));	
		} else {
			ddy = abs((int16_t)(i*(float)(dx/len)));
			ddx = (int16_t)(i*(float)(dy/len));
		}
		drawLine(x1+ddx, y1+ddy, 
				x2+ddx, y2+ddy, color);
		drawLine(x1+1+ddx, y1+ddy, 
				x2+1+ddx, y2+ddy, color);
		drawLine(x1+ddx, y1+1+ddy, 
				x2+ddx, y2+1+ddy, color);					
	}
}

void Figure::drawCircle(uint16_t x,uint16_t y,float R, uint16_t color) { //Bresenham	
	int f = 1 - R;	int ddF_x = 1;
	int ddF_y = -2 * R; int x0 = 0;
  	int y0 = R;
  	setPixel(x , y + R, color);
  	setPixel(x , y - R, color);
  	setPixel(x + R, y , color);
  	setPixel(x - R, y , color);
  	while (x0<y0) {
    	if (f >= 0) {
    		y0--;
     		ddF_y += 2;
     		f += ddF_y;
    	}
    	x0++; ddF_x += 2; f += ddF_x;
    	setPixel(x + x0, y + y0, color);
    	setPixel(x - x0, y + y0, color);
    	setPixel(x + x0, y - y0, color);
    	setPixel(x - x0, y - y0, color);
    	setPixel(x + y0, y + x0, color);
    	setPixel(x - y0, y + x0, color);
    	setPixel(x + y0, y - x0, color);
    	setPixel(x - y0, y - x0, color);
	}			
}

void Figure::drawFatCircle(uint16_t x,uint16_t y,float R,uint16_t width, uint16_t color) {
	for(int i=0; i<width; i++) {
		drawCircle(x,y,R-i,color);
		drawCircle(x-1,y-1,R-i,color);
		drawCircle(x,y-1,R-i,color);
		drawCircle(x-1,y,R-i,color);
	}
}

void Figure::drawFloatCircle(uint16_t x,uint16_t y,float R,uint16_t col) {
	for (uint16_t i=0;i<=2*R;i++) {
        setPixel(x+i,y+sqrtf(R*R - (i-R)*(i-R))+R,col);
        setPixel(x+i,y-sqrtf(R*R - (i-R)*(i-R))+R,col);
        setPixel(x+sqrtf(R*R - (i-R)*(i-R))+R,y+i,col);
        setPixel(x-sqrtf(R*R - (i-R)*(i-R))+R,y+i,col);
    }
}

void Figure::drawFloatFatCircle(uint16_t x,uint16_t y,float R,uint16_t width, uint16_t color) {
	for(int j=0; j<width; j++) {
		drawFloatCircle(x+j, y+j, R-j, color);
		drawFloatCircle(x+j+1, y+j, R-j, color);
		drawFloatCircle(x+j, y+j+1, R-j, color);
	}
}

void Figure::drawFatLineOnCircle(uint16_t x, uint16_t y, uint16_t R, uint16_t angle, uint16_t width, uint16_t len, uint16_t color) {
	float dx = cos((float)angle*Pi/180); float dy = sin((float)angle*Pi/180);
	drawFatLine(x+(R-len)*dx, y+(R-len)*dy,x+R*dx,y+R*dy,width,color);
}

void Figure::drawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color) {
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x3, y3, color);
	drawLine(x3, y3, x1, y1, color);	
}

void Figure::drawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color) {
	float length1 = sqrtf((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
	float length2 = sqrtf((x3-x2)*(x3-x2)+(y3-y2)*(y3-y2));
	float length3 = sqrtf((x3-x1)*(x3-x1)+(y3-y1)*(y3-y1));
	float dx2 = (x3-x2)/length2; float dy2 = (y3-y2)/length2;
	float dx3 = (x1-x3)/length3; float dy3 = (y1-y3)/length3;
	float dx1 = (x2-x1)/length1; float dy1 = (y2-y1)/length1;
	for(int i=0; i <= length2; i++) {
		drawLine(x1, y1, x2+i*dx2, y2+i*dy2, color);
	}
	for(int i=0; i <= length3; i++) {
		drawLine(x2, y2, x3+i*dx3, y3+i*dy3, color);
	}
	for(int i=0; i <= length1; i++) {
		drawLine(x3, y3, x1+i*dx1, y1+i*dy1, color);
	}
}

void Figure::drawSych() {
	drawFrame(10,10,50,50,3,0xFF00);
	drawFrame(9,9,51,51,1,0xFFA0);		
	drawFatCircle(200,240,100,100,BLUE);
	drawFloatFatCircle(500,140,100,100,BLUE);
	drawFatCircle(200,280,50,50, BLACK);
	drawFatCircle(600,280,50,50, BLACK);
	drawFatLine(500,100,700,0,50, GRAY2);
	drawFatLine(100,0,300,100,50, GRAY2);
	drawFilledTriangle(400,450,400,400,350,450, BRIGHT_RED);
}