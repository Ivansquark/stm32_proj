#include "rs485.h"

namespace RS485 {
bool received_flag = false;
uint8_t receivedByte = 0;
} // namespace RS485

void RS485::init() {
    //PA8 - read/write RS485
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER &= ~GPIO_MODER_MODER8_1;
    GPIOA->MODER |= GPIO_MODER_MODER8_0;     // push pull high speed
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR8; // max speed
    GPIOA->BSRR |= GPIO_BSRR_BR_8;

    /*!< PA9-Tx PA10-Rx >*/
    GPIOA->MODER |= (GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1);
    GPIOA->MODER &= ~(GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0);      // alt func
    
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR9 | GPIO_OSPEEDR_OSPEEDR10); // max speed
    //GPIOA->PUPDR |= GPIO_PUPDR_PUPDR10_0;
    //GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR10_1;
    //GPIOA->ODR |= GPIO_ODR_10;
    GPIOA->AFR[1] |= (1 << 4) | (1 << 8);                               // AF1 (RS485) PA9 PA10
    /*!< RS4851 init >*/
    USART1->CR1 &= ~USART_CR1_UE;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    USART1->CR1 &= ~USART_CR1_M;                // 8 bits
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE; // Transmition receiving enabled
    USART1->CR1 |= USART_CR1_RXNEIE;            // receive not empty interrupt enabled
    USART1->CR1 |= USART_CR1_TCIE;
    // TODO: count proper baud rate for 8MHz
    //USART1->BRR = 0x682;         // baudrate=9600 (APB2CLK=16МHz - baudrate/2)/baudrate
    //USART1->BRR = 0x341;         // baudrate=9600 (APB2CLK=8МHz - baudrate/2)/baudrate
    USART1->BRR = 0x341;         // baudrate=9600 (APB2CLK=8МHz - baudrate/2)/baudrate
    USART1->CR1 |= USART_CR1_UE; // USART1 enable
    NVIC_EnableIRQ(USART1_IRQn);
}

void RS485::setRead() { GPIOA->BSRR |= GPIO_BSRR_BR_8; }
void RS485::setWrite() { GPIOA->BSRR |= GPIO_BSRR_BS_8; }

bool RS485::isReceived() { return received_flag; }

void RS485::setByte(uint8_t byte) { receivedByte = byte; }
uint8_t RS485::getByte() { return receivedByte; }

void RS485::sendByte(uint8_t byte) {
    // USART1->CR1 |= USART_CR1_TXEIE_TXFNFIE;
    uint32_t timeout = 0xFFFF;
    while ((!(USART1->ISR & USART_ISR_TXE)) && timeout--)
        ;
    if (!timeout) {
        return;
    }
    setWrite();
    USART1->TDR = (uint8_t)(byte);
    //setRead();
}

void RS485::sendStr(const char *str) {
    uint8_t i = 0;
    while (str[i] != '\0') {
        sendByte(str[i++]);
    }
}

extern volatile unsigned int rxtimer;
extern volatile unsigned char rxcnt; //количество принятых символов
extern volatile unsigned char txcnt; //количество переданных символов
extern volatile unsigned char txlen; //длина посылки на отправку
extern volatile unsigned int delay; //задержка по которой вычисляется конец посылки
extern volatile unsigned char buffer[];

extern "C" void USART1_IRQHandler(void) {
    if (USART1->ISR & USART_ISR_IDLE) {
        USART1->ICR |= USART_ICR_IDLECF;
    }
    if (USART1->ISR & USART_ISR_ORE) {
        USART1->ICR |= USART_ICR_ORECF;
    }
    if (USART1->ISR & USART_ISR_FE) {
        USART1->ICR |= USART_ICR_FECF;
    }
    if (USART1->ISR & USART_ISR_NE) {
        USART1->ICR |= USART_ICR_NCF;
    }
    if (USART1->ISR & USART_ISR_RXNE) {
        USART1->ISR &= ~USART_ISR_RXNE;
        // RS485::setByte(USART1->RDR);
        RS485::received_flag = true;

        rxtimer = 0; // обнуляем счетчик таймера
        // если переполнен буфер обнуляем счетчик байтов на прием (проверка мусора)
        if (rxcnt > (255 - 2)) {
            rxcnt = 0;
        }
        uint8_t temp = USART1->RDR;
        buffer[rxcnt++] = temp; // принимаем байт в буффер из DR
    }
    if (USART1->ISR & USART_ISR_TC) {
        // USART1->SR&=~USART_SR_TC;//очистка признака прерывания передачи
        if (txcnt < txlen) {
            RS485::sendByte(buffer[txcnt++]); //Передаем, увеличивая счетчик переданных байтов
        } else {
            //посылка закончилась и мы снимаем высокий уровень сRS485 TXE
            txlen = 0;               //обнуляем длину буфера на передачу
            USART1->CR1 |= USART_CR1_RXNEIE; //включаем прерывание по приему
            USART1->CR1 &= ~USART_CR1_TCIE;  // выключаем прерывание по передаче
            TIM14->DIER |= TIM_DIER_UIE;      // включаем таймер modbus
            RS485::setRead();
        }
    }
}
