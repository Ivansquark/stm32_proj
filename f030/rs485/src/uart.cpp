#include "uart.h"

Uart *Uart::pThis = nullptr;

Uart::Uart() {
    pThis = this;
    init();
}

void Uart::sendByte(uint8_t byte) {
    while (!(USART1->ISR & USART_ISR_TXE))
        ;
    USART1->TDR = byte;
}

void Uart::sendStr(const char *str) {
    while (*str) {
        sendByte(*((uint8_t *)str++));
    }
}

void Uart::init() {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= (GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1);
    GPIOA->MODER &= ~(GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0);      // 1:0 alt func
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR9 | GPIO_OSPEEDR_OSPEEDR10); // max speed
    GPIOA->OTYPER |= GPIO_OTYPER_OT_9;                                 // Tx as open drain
    // GPIOA->PUPDR |= GPIO_PUPDR_PUPD2_0; // 0:1 - pull-up
    GPIOA->AFR[0] |= (1 << 4) | (1 << 0); // AF1 (uart) A9 A10
    /*!< UART1 init >*/
    // USART2->CR3 |= USART_CR3_HDSEL; // for true one wire (one for ground and one for data+power)
    USART1->CR1 &= ~USART_CR1_UE;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    USART1->CR1 &= ~USART_CR1_M; // 8 bits
    // Transmition receiving enabled USART2->BRR=0x22B; baudrate=115200 (APB2CLK=64МHz - baudrate/2/baudrate
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART1->BRR = 0x341;             // baudrate=9600 (APB1CLK=8МHz - baudrate/2)/baudrate
    USART1->CR1 |= USART_CR1_RXNEIE; ////receive not empty interrupt enabled
    USART1->CR1 |= USART_CR1_UE;     // USART1 enable
    NVIC_EnableIRQ(USART1_IRQn);     // received DMA wont work without this interrupt
}

extern "C" void USART2_IRQHandler(void) {
    if (USART1->ISR & USART_ISR_RXNE) {
        Uart::pThis->receivedByte = USART1->RDR;
    }
}
