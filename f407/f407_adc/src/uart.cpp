#include "uart.h"

Uart3* Uart3::pThis = nullptr;

Uart3::Uart3() {
    uart3_init();
}

void Uart3::uart3_sendByte(uint8_t data) {
    uint32_t Timeout = 10000;
    while(!(USART3->SR & USART_SR_TXE)){if(!Timeout--)break;};
    USART3->DR=data;
}

void Uart3::uart3_sendStr(const char* str) {
    uint8_t i=0;
    while(str[i]!='\0') {
        uart3_sendByte(str[i++]);
    }
}

void Uart3::uart3_init() {    
    /*!< PB10-Tx PB11-Rx >*/
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER |= (GPIO_MODER_MODER10_1);
    GPIOB->MODER &=~ (GPIO_MODER_MODER10_0);//alt func
    GPIOB->MODER |= (GPIO_MODER_MODER11_1 );
    GPIOB->MODER &=~ GPIO_MODER_MODER11_0; //1:0 alt func  
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10;  // max speed
    GPIOB->AFR[1] |= (7<<8)|(7<<12); // AF7 (usart) PA9 PA10
    /*!< UART1 init >*/
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    USART3->CR1 &=~ USART_CR1_M; //8 bits
    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; // Transmition receiving enabled
    USART3->CR1 |= USART_CR1_RXNEIE; //receive not empty interrupt enabled
    USART3->BRR=0x9C3; //9600 (APB2CLK=24МHz - baudrate/2)/baudrate
    //USART3->BRR=0xCF; //115200 (APB2CLK=24МHz - baudrate/2)/baudrate
    
    USART3->CR1 |= USART_CR1_UE; //USART1 enable    
    NVIC_EnableIRQ(USART3_IRQn);
}

extern "C" void USART3_IRQHandler(void) {
    //Adc::pThis->DataIsRead = true;
    Adc::pThis->receved_Flag = true;
    if(USART3->SR&USART_SR_RXNE)
   	{
       	USART3->SR &=~ USART_SR_RXNE; // очистка признака прерывания по приему
    
        uint8_t data = USART3->DR;
        Adc::pThis->UART_BYTE = data;
        //NormalQueue8::pThis->push(data);
        if( ! Adc::pThis->DataIsChecked) {
            if(Adc::pThis->countCheckDataRead == 0 && data == 0xde) {
                Adc::pThis->countCheckDataRead++;
            } else if(Adc::pThis->countCheckDataRead == 1 && data == 0xad) {                     
                Adc::pThis->countCheckDataRead = 0;
                Adc::pThis->DataIsRead = true;
                Adc::pThis->DataIsWrite = false;
                //Adc::pThis->receved_Flag = false;
            }
            else if (Adc::pThis->countCheckDataWrite == 0 && data == 0xab) {
                Adc::pThis->countCheckDataWrite++;
            } else if (Adc::pThis->countCheckDataWrite == 1 && data == 0xcd) {
                Adc::pThis->countCheckDataWrite = 0;
                //Adc::pThis->DataIsRead = false;
                Adc::pThis->DataIsWrite = true;
                Adc::pThis->DataIsChecked = true; // data is checked for writing coefs in flash
            } else {
                Adc::pThis->countCheckDataRead = 0;
                Adc::pThis->countCheckDataWrite = 0;
                Adc::pThis->receved_Flag = false;
            }
        } else {  // writing operation is need to implement
            if(Adc::pThis->countData == 11) {
                Adc::pThis->DataIsChecked = false;
                Adc::pThis->DataIsRead = false;
                Adc::pThis->DataIsWrite = false;
                Adc::pThis->received_Data[Adc::pThis->countData] = data;
                Adc::pThis->countData = 0;
                Adc::pThis->NewCoefReadyToWrite = true;
                Adc::pThis->receved_Flag = false;
            } else {
                Adc::pThis->received_Data[Adc::pThis->countData] = data;
                Adc::pThis->countData++;
            }
        }
    }
    USART3->DR = 0;;
}
