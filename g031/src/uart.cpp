#include "uart.h"

UART* UART::pThis = nullptr;

UART::UART() {
    init();
    pThis = this;
}

void UART::sendByte(uint8_t byte) {
    //USART1->CR1 |= USART_CR1_TXEIE_TXFNFIE;
	uint32_t timeout = 0xFFFF;
    while((!(USART1->ISR & USART_ISR_TXE_TXFNF)) && timeout--);
	USART1->TDR = (uint8_t)(byte);
}

void UART::sendStr(const char* str) {
    uint8_t i=0;
    while(str[i]!='\0') {
        sendByte(str[i++]);
    }
}

void UART::init() {
    /*!< PB6-Tx PB7-Rx >*/
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER |= (GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
    GPIOB->MODER &=~ (GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0); //alt func
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7);  // max speed
    GPIOB->AFR[1] |= (0<<24)|(0<<28); // AF0 (uart) PB6 PB7
    /*!< UART1 init >*/
    USART1->CR1 &=~ USART_CR1_UE;
    RCC->APBENR2 |= RCC_APBENR2_USART1EN;
    USART1->CR1 &=~ USART_CR1_M; //8 bits
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE; // Transmition receiving enabled
    USART1->CR1 |= USART_CR1_RXNEIE_RXFNEIE; //receive not empty interrupt enabled
    USART1->CR1 &=~ USART_CR1_FIFOEN;
    USART1->BRR=0x682; //baudrate=9600 (APB2CLK=16МHz - baudrate/2)/baudrate
    USART1->CR1 |= USART_CR1_UE; //USART1 enable
    NVIC_EnableIRQ(USART1_IRQn);
}

extern "C" void USART1_IRQHandler(void) {
    if(USART1->ISR & USART_ISR_IDLE) {
		USART1->ICR |= USART_ICR_IDLECF;
	}
	if(USART1->ISR & USART_ISR_ORE) {
		USART1->ICR |= USART_ICR_ORECF;
	}
	if(USART1->ISR & USART_ISR_FE) {
		USART1->ICR |= USART_ICR_FECF;
	}
	if(USART1->ISR & USART_ISR_NE) {
		USART1->ICR |= USART_ICR_NECF;
	}
	if(USART1->ISR & USART_ISR_RXNE_RXFNE) {
		USART1->ISR &=~ USART_ISR_RXNE_RXFNE;
		UART::pThis->received_byte = USART1->RDR;
        UART::pThis->received_flag = true;
	}
}