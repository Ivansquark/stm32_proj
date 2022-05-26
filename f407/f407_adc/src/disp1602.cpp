#include "disp1602.h"

Display* Display::pThis = nullptr;

Display::Display() {
		pThis=this;
    gpios_init();
    display_init();
    LED_init();
		diode_init();
    for(int i =0 ;i<sizeof(arrStr);i++){arrStr[i]=0;}
}

void Display::intToStr(uint32_t val) {
    for(int i = 0; i<sizeof(arrStr);  i++) {
        arrStr[i] = 0;
    }
    if(val == 0) {
        arrStr[0] = '0';
        arrStr[1] = '\0';
        return;
    }
		int count=0;
		while(val) {			
			arrStr[count++]=val%10;
			val/=10;			
		}
		for(int i=0; i<count/2;i++) {
			int8_t temp;
			temp = arrStr[i];
			arrStr[i] = arrStr[count-i-1];
			arrStr[count-i-1] = temp;
		}
		for(int i=0;i<count;i++) {
			arrStr[i]+=0x30;
		}
		arrStr[count] = '\0';
}

void Display::floatToStr(float val) {
    sprintf((char*)arrFloat, "%.2f",  val);
}

void Display::arrStr_clear() {
    for(int i=0; i<20;i++) {
        arrStr[i]=0;
    }
}

void Display::sendStr(int8_t* str) {
    int8_t i = 0;
    while(str[i] != '\0') {
        sendByte(str[i++]);
				//delay();
    }
}

void Display::setPos(uint8_t x, uint8_t y) {
    switch(y) {
        case 0:
            writeCommand(x|0x80);
            //Timer::pThis->delay_ms(1);
            break;
        case 1:
            writeCommand((0x40+x)|0x80);
            //Timer::pThis->delay_ms(1);
            break;
    }
}

void Display::gpios_init(void) {
    /*!< Display: D0 - RS, D1 - R/W, D2 - E   (D13 - control led) >*/
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    GPIOD->MODER &=~ (GPIO_MODER_MODER0_1 | GPIO_MODER_MODER1_1 | GPIO_MODER_MODER2_1 | GPIO_MODER_MODER13_1);
    GPIOD->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER13_0);
    GPIOD->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR0 | GPIO_OSPEEDER_OSPEEDR1 | GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR13); // push-pull max speed  
    
    /*!< Display - 4bits: B4 - DB4, B5 - DB5, B6 - DB6, B7 - DB7 >*/    
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER &=~ (GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);
    GPIOB->MODER |= (GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR4 | GPIO_OSPEEDER_OSPEEDR5 | GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7); //push pull high speed    
}

void Display::display_init() {
    
    Timer3::pThis->delay_ms(50);
    RW_0();
    RS_0();writeData(3); E_1(); delay(); E_0();
    Timer3::pThis->delay_ms(2);


    //*!< function set >
    //writeCommand(0x28);
    //Timer::pThis->delay_ms(2);
    writeCommand(0x28);
    Timer3::pThis->delay_ms(2);
    //!< 1DCB  D-Display, C-cursor, B-Blinking control >  
    writeCommand(0x0C);
    Timer3::pThis->delay_ms(3);
    //!< Display clear >
    writeCommand(0x01);
    Timer3::pThis->delay_ms(2);  
    //!< entry set mode >
    writeCommand(0x06);    
    Timer3::pThis->delay_ms(100);            
    RS_0();writeData(3); E_1(); delay(); E_0();
    Timer3::pThis->delay_ms(2);


    //*!< function set >
    //writeCommand(0x28);
    //Timer::pThis->delay_ms(2);
    writeCommand(0x28);
    Timer3::pThis->delay_ms(2);
    //!< 1DCB  D-Display, C-cursor, B-Blinking control >  
    writeCommand(0x0C);
    Timer3::pThis->delay_ms(3);
    //!< Display clear >
    writeCommand(0x01);
    Timer3::pThis->delay_ms(2);  
    //!< entry set mode >
    writeCommand(0x06);    
    Timer3::pThis->delay_ms(100);   
    /*
    Timer::pThis->delay_ms(50);
    RW_0();
    command8(0x30);
    Timer::pThis->delay_ms(1);
    command8(0x30);
    Timer::pThis->delay_ms(1);
    command8(0x30);
    Timer::pThis->delay_ms(1);    
    //!< function set >
    writeCommand(0x38);
    Timer::pThis->delay_ms(1);
    writeCommand(0x38);
    Timer::pThis->delay_ms(1);
    //!< 1DCB  D-Display, C-cursor, B-Blinking control >  
    writeCommand(0x0E);
    Timer::pThis->delay_ms(1);
    //!< Display clear >
    writeCommand(0x01);
    Timer::pThis->delay_ms(1);  
    //!< entry set mode >
    writeCommand(0x06);    
    Timer::pThis->delay_ms(1);
    //writeCommand(0x14);    
    //Timer::pThis->delay_ms(1);
    //writeCommand(0x0C);    
    //Timer::pThis->delay_ms(1);
		*/
}

void Display::delay() {
for(int i=0; i<48*10;i++){}
}
    
void Display::LED_init() {
    //! PC10 LED enable pin
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    GPIOC->MODER |= GPIO_MODER_MODE10_0;
    GPIOC->MODER &=~ GPIO_MODER_MODE10_1;  // 0:1 push-pull
}

void Display::diode_init(){
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
    GPIOE->MODER |= GPIO_MODER_MODE15_0;
    GPIOE->MODER &=~ GPIO_MODER_MODE15_1;  // 0:1 push-pull
}