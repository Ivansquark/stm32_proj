#include "ds1822.h"

const uint8_t convert_T[] = {
                OW_0, OW_0, OW_1, OW_1, OW_0, OW_0, OW_1, OW_1, // 0xcc SKIP ROM
                OW_0, OW_0, OW_1, OW_0, OW_0, OW_0, OW_1, OW_0  // 0x44 CONVERT
};
const uint8_t read_scratch[] = {
                OW_0, OW_0, OW_1, OW_1, OW_0, OW_0, OW_1, OW_1, // 0xcc SKIP ROM
                OW_0, OW_1, OW_1, OW_1, OW_1, OW_1, OW_0, OW_1, // 0xbe READ SCRATCH
                OW_R, OW_R, OW_R, OW_R, OW_R, OW_R, OW_R, OW_R,
                OW_R, OW_R, OW_R, OW_R, OW_R, OW_R, OW_R, OW_R
};
uint8_t scratch[sizeof(read_scratch)];

volatile uint8_t uart_received_byte = 0;
volatile uint8_t uart_received_flag = 0;
Temperature_ds1822 temp1822;

void uart_sendByte(uint8_t byte) {
    uint32_t timeout = 0xFFFF;
    while((!(USART2->ISR & USART_ISR_TXE_TXFNF)) && timeout--);
    USART2->TDR = byte;
}
void uart_sendStr(const char* str) {
    uint8_t i=0;
    while(str[i]!='\0') {
        uart_sendByte(str[i++]);
    }
}

void ds1822_init() {
    //! uart as one wire interface  Tx as open drain
    uart_received_byte = 0;
    uart_received_flag = 0;
    /*!< A2-Tx A3-Rx >*/
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);
    GPIOA->MODER &=~ (GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0); //1:0 alt func
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3);  // max speed
    GPIOA->OTYPER |= GPIO_OTYPER_OT2; // Tx as open drain
    //GPIOA->PUPDR |= GPIO_PUPDR_PUPD2_0; // 0:1 - pull-up
    GPIOA->AFR[0] |= (1<<8)|(1<<12); // AF1 (uart) A2 A3
    /*!< UART1 init >*/
    //USART2->CR3 |= USART_CR3_HDSEL; // for true one wire (one for ground and one for data+power)
    USART2->CR1 &=~ USART_CR1_UE;
    RCC->APBENR1 |= RCC_APBENR1_USART2EN;
    USART2->CR1 &=~ USART_CR1_M; //8 bits
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE; // Transmition receiving enabled
    USART2->CR1 &=~ USART_CR1_FIFOEN;
    //USART2->BRR=0x22B; //baudrate=115200 (APB2CLK=64МHz - baudrate/2)/baudrate
    USART2->BRR=0x1A09; //baudrate=9600 (APB1CLK=64МHz - baudrate/2)/baudrate
    USART2->CR1 |= USART_CR1_RXNEIE_RXFNEIE; ////receive not empty interrupt enabled
    USART2->CR1 |= USART_CR1_UE; //USART1 enable
    NVIC_EnableIRQ(USART2_IRQn); //received DMA wont work without this interrupt
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}

void uart_dma_send(const uint8_t *command, uint16_t len) {
    //DMA1_Channel2->CCR &=~ DMA_CCR_EN;
    //! DMA init DMA_MUX = 53 for USART2_Tx
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel2->CCR &=~ DMA_CCR_MSIZE; // 0:0  8 bits (for uint8_t)
    DMA1_Channel2->CCR &=~ DMA_CCR_PSIZE; // 0:0  8 bits (for USART_DR)
    DMA1_Channel2->CCR |= DMA_CCR_MINC; // memory increment enable
    DMA1_Channel2->CCR |= DMA_CCR_DIR; // 1 - read from memory
    DMA1_Channel2->CCR |= DMA_CCR_TCIE; // transfer complete interrupt enable
    DMA1_Channel2->CCR &=~ DMA_CCR_CIRC;
    DMA1_Channel2->CNDTR = len; 
    DMA1_Channel2->CPAR = (uint32_t)&(USART2->TDR);
    DMA1_Channel2->CMAR = (uint32_t) command;
    
    DMAMUX1_Channel1->CCR |= 53;

    USART2->CR3 |= USART_CR3_DMAT; //DMA enable transmitter
    DMA1_Channel2->CCR |= DMA_CCR_EN; // enable DMA (send first byte, others auto setted by uart)
    //DMA1_Channel2->CCR &=~ DMA_CCR_CIRC;
}

void uart_dma_read(const uint8_t *command, uint8_t* buf, uint16_t len) {
    //! DMA init DMA_MUX = 52 for USART2_Rx    
    DMA1_Channel3->CCR &=~ DMA_CCR_MSIZE; // 0:0  8 bits (for uint8_t)
    DMA1_Channel3->CCR &=~ DMA_CCR_PSIZE; // 0:0  8 bits (for USART_DR)
    DMA1_Channel3->CCR |= DMA_CCR_MINC; // memory increment enable
    DMA1_Channel3->CCR &=~ DMA_CCR_DIR; // 0 - read from periphery
    DMA1_Channel3->CCR |= DMA_CCR_TCIE; // transfer complete interrupt enable
    DMA1_Channel3->CCR &=~ DMA_CCR_CIRC;
    DMA1_Channel3->CNDTR = len; 
    DMA1_Channel3->CPAR = (uint32_t)&(USART2->RDR);
    DMA1_Channel3->CMAR = (uint32_t) buf;    
    DMAMUX1_Channel2->CCR |= 52;

    //! DMA init DMA_MUX = 53 for USART2_Tx
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel2->CCR &=~ DMA_CCR_MSIZE; // 0:0  8 bits (for uint8_t)
    DMA1_Channel2->CCR &=~ DMA_CCR_PSIZE; // 0:0  8 bits (for USART_DR)
    DMA1_Channel2->CCR |= DMA_CCR_MINC; // memory increment enable
    DMA1_Channel2->CCR |= DMA_CCR_DIR; // 1 - read from memory
    DMA1_Channel2->CCR |= DMA_CCR_TCIE; // transfer complete interrupt enable
    DMA1_Channel2->CCR &=~ DMA_CCR_CIRC;
    DMA1_Channel2->CNDTR = len; 
    DMA1_Channel2->CPAR = (uint32_t)&(USART2->TDR);
    DMA1_Channel2->CMAR = (uint32_t) command;    
    DMAMUX1_Channel1->CCR |= 53;

    USART2->CR3 |= (USART_CR3_DMAT | USART_CR3_DMAR); //DMA enable transmitter and receiver
    DMA1_Channel2->CCR |= DMA_CCR_EN; // enable DMA (send first byte, others auto setted by uart)
    DMA1_Channel3->CCR |= DMA_CCR_EN; // enable DMA (send first byte, others auto setted by uart)
    //while(!(DMA1->ISR |= DMA_ISR_TCIF2)); //transfer complete channel 2 flag
}

void one_wire_reset() {
    USART2->BRR=0x1A0A; //9600
    uint8_t ow_presence;
    //USART2->ICR |= USART_ICR_TCCF;// transmittion complete clear flag
    USART2->TDR = 0xf0;
    uint32_t timeout=0xffff;
    while(!(USART2->ISR & USART_ISR_TC) && timeout--);
    ow_presence = USART2->RDR;
    USART2->BRR=0x22B; //115200
    if (ow_presence != 0xf0) {
                return;
    }
}

void ds_1822_read_temperature() {
    one_wire_reset();
    uart_dma_send(convert_T, sizeof(convert_T));
    for (volatile int i=0; i<10000000; i++); // 750 ms
    one_wire_reset();
    uart_dma_read(read_scratch, scratch,  32);
    uint16_t tt=0;
    for (volatile int i=31;i>15; i--) {
        if (scratch[i] == 0xff) {
            tt = (tt<<1) | 0x0001; // add right bit if 1
        } else {
            tt = tt<<1;
        }
    }
    //uint8_t arr[32]={0};
    //for(int i=0;i<32;i++) {
    //    arr[i] = scratch[i];
    //}
    tt = ((tt>>8)&0xFF) | ((tt<<8)&0xFF00); 
    temp1822.sign = tt>>14;
    temp1822.integer = ((tt>>10)&0x1)*64 + ((tt>>9)&0x1)*32 + ((tt>>8)&0x1)*16 +
                        ((tt>>7)&0x1)*8 + ((tt>>6)&0x1)*4 + ((tt>>5)&0x1)*2 + ((tt>>4)&0x1);
    temp1822.fractional = ((tt>>3)&0x1)*50 + ((tt>>2)&0x1)*25 + ((tt>>1)&0x1)*12;
}

void USART2_IRQHandler(void) {
    USART2->ISR &=~ USART_ISR_RXNE_RXFNE;
    uart_received_byte = USART2->RDR;
    //uint8_t x = uart_received_byte;
}

void DMA1_Channel2_3_IRQHandler(void) {
  // clear interrupt flag
  if(DMA1->ISR & DMA_ISR_TCIF2) {
    DMA1->IFCR |= DMA_IFCR_CTCIF2;  
    DMA1_Channel2->CCR &=~ DMA_CCR_EN; // disable DMA  
  }
  if(DMA1->ISR & DMA_ISR_TCIF3) {
    DMA1->IFCR |= DMA_IFCR_CTCIF3; 
    DMA1_Channel3->CCR &=~ DMA_CCR_EN; // disable DMA   
  }    
}
