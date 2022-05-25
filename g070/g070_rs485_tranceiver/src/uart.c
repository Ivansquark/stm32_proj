#include "uart.h"

Queue* queueRS485_ptr = NULL;
Queue* queueLinux_ptr = NULL;

void pin_init() {
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER &=~ GPIO_MODER_MODE1;
    GPIOB->MODER |= GPIO_MODER_MODE1_0; //0:1 => output
}

void uartRS485_init(Queue* q) {
    queueRS485_ptr = q;
    uartRS485_received_packet = 0;
    pin_init();
    /*!< A2-Tx A3-Rx >*/
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);
    GPIOA->MODER &=~ (GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0); //1:0 alt func
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3);  // max speed
    GPIOA->AFR[0] |= (1<<8)|(1<<12); // AF1 (uart) A2 A3
    /*!< UART2 init >*/
    RCC->APBENR1 |= RCC_APBENR1_USART2EN;
    USART2->CR1 &=~ USART_CR1_M; //8 bits
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE; // Transmition receiving enabled
    USART2->CR1 &=~ USART_CR1_FIFOEN;
    //USART2->BRR=0x22B; //baudrate=115200 (APB2CLK=64МHz - baudrate/2)/baudrate
    USART2->BRR=0x682; //baudrate=9600 (APB1CLK=64МHz - baudrate/2)/baudrate
    USART2->CR1 |= USART_CR1_RXNEIE_RXFNEIE; ////receive not empty interrupt enabled
    USART2->CR1 |= USART_CR1_UE; //USART1 enable
    NVIC_EnableIRQ(USART2_IRQn); //received DMA wont work without this interrupt    
}

void uartRS485_sendByte(uint8_t byte) {
    uint32_t timeout = 0xFFFF;
    while((!(USART2->ISR & USART_ISR_TXE_TXFNF)) && timeout--);
    PIN_ON;
    USART2->ICR |= USART_ICR_TCCF;
    USART2->TDR = byte;
    timeout = 0xFFFF;
    while((!(USART2->ISR & USART_ISR_TC)) && timeout--);
    PIN_OFF;
}
void uartLinux_init(Queue* q) {
    queueLinux_ptr = q;
    uartLinux_received_packet = 0;
    /*!< A0-Tx A1-Rx >*/
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER |= (GPIO_MODER_MODE0_1 | GPIO_MODER_MODE1_1);
    GPIOA->MODER &=~ (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0); //1:0 alt func
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED0 | GPIO_OSPEEDR_OSPEED1);  // max speed
    GPIOA->AFR[0] |= (4<<0)|(4<<4); // AF4 (uart) A0 A1
    /*!< UART4 init >*/
    RCC->APBENR1 |= RCC_APBENR1_USART4EN;
    USART4->CR1 &=~ USART_CR1_M; //8 bits
    USART4->CR1 |= USART_CR1_TE | USART_CR1_RE; // Transmition receiving enabled
    USART4->CR1 &=~ USART_CR1_FIFOEN;
    //USART4->BRR=0x22B; //baudrate=115200 (APB2CLK=64МHz - baudrate/2)/baudrate
    //USART4->BRR=0x1A09; //baudrate=9600 (APB1CLK=64МHz - baudrate/2)/baudrate
    USART4->BRR=0x682; //baudrate=9600 (APB1CLK=16МHz - baudrate/2)/baudrate
    USART4->CR1 |= USART_CR1_RXNEIE_RXFNEIE; ////receive not empty interrupt enabled
    USART4->CR1 |= USART_CR1_UE; //USART1 enable
    NVIC_EnableIRQ(USART3_4_IRQn); //received DMA wont work without this interrupt    
}

void uartLinux_sendByte(uint8_t byte) {
    uint32_t timeout = 0xFFFF;
    while((!(USART4->ISR & USART_ISR_TXE_TXFNF)) && timeout--);
    USART4->TDR = byte;
}

// receive in queue
void USART2_IRQHandler(void) {
    if(USART2->ISR & USART_ISR_FE) {
        USART2->ICR |= USART_ICR_FECF;
    }
    if(USART2->ISR & USART_ISR_ORE) {
        USART2->ICR |= USART_ICR_ORECF;
    }
    if(USART2->ISR & USART_ISR_PE) {
        USART2->ICR |= USART_ICR_PECF;
    }
    if(USART2->ISR & USART_ISR_IDLE) {
        USART2->ICR |= USART_ICR_IDLECF;
    }
    if(USART2->ISR & USART_ISR_RXNE_RXFNE) {
        //USART2->ISR &=~ USART_ISR_RXNE_RXFNE;
        uint8_t uart_received_byte = USART2->RDR;
        //uartLinux_sendByte(uart_received_byte);
        queue_push(queueRS485_ptr, uart_received_byte);
        if(queue_size(queueRS485_ptr) == 13) {
            uartRS485_received_packet = 1;
        }
    }    
}
void USART3_4_IRQHandler(void) {
    if(USART4->ISR & USART_ISR_FE) {
        USART4->ICR |= USART_ICR_FECF;
    }
    if(USART4->ISR & USART_ISR_ORE) {
        USART4->ICR |= USART_ICR_ORECF;
    }
    if(USART4->ISR & USART_ISR_PE) {
        USART4->ICR |= USART_ICR_PECF;
    }
    if(USART4->ISR & USART_ISR_IDLE) {
        USART4->ICR |= USART_ICR_IDLECF;
    }
    if(USART4->ISR & USART_ISR_RXNE_RXFNE) {
        //USART4->ISR &=~ USART_ISR_RXNE_RXFNE;
        uint8_t uart_received_byte = USART4->RDR;
        //uartRS485_sendByte(uart_received_byte);
        queue_push(queueLinux_ptr, uart_received_byte);
        if(queue_size(queueLinux_ptr) == 11) {
            uartLinux_received_packet = 1;
        }
    }    
}