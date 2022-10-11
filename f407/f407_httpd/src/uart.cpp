#include "uart.h"
Uart *Uart::pThis[7] = {nullptr};

Uart::Uart(uint8_t num) : uartNum(num) {
    pThis[num] = this;
    init(num);
}
void Uart::sendByte(uint8_t byte) {
    switch (uartNum) {
    case 1: {
        uint32_t timeout = 0xFFFF;
        while ((!(USART1->SR & USART_SR_TXE)) && timeout--)
            ;
        USART1->DR = byte;
    } break;
    case 2: {
        uint32_t timeout = 0xFFFF;
        while ((!(USART2->SR & USART_SR_TXE)) && timeout--)
            ;
        USART2->DR = byte;
    } break;
    case 3: {
        uint32_t timeout = 0xFFFF;
        while ((!(USART3->SR & USART_SR_TXE)) && timeout--)
            ;
        USART3->DR = byte;
    } break;
    case 4: {
        uint32_t timeout = 0xFFFF;
        while ((!(UART4->SR & USART_SR_TXE)) && timeout--)
            ;
        UART4->DR = byte;
    } break;
    case 5: {
        uint32_t timeout = 0xFFFF;
        while ((!(UART5->SR & USART_SR_TXE)) && timeout--)
            ;
        UART5->DR = byte;
    } break;
    case 6: {
        uint32_t timeout = 0xFFFF;
        while ((!(USART6->SR & USART_SR_TXE)) && timeout--)
            ;
        USART6->DR = byte;
    } break;
    default:
        break;
    }
}
void Uart::sendStr(const char *str) {
    uint8_t i = 0;
    while (str[i] != '\0') {
        sendByte(str[i++]);
    }
}

void Uart::sendBuf(uint8_t *data, uint8_t len) {
    for(int i = 0; i < len; i++) {
        sendByte(data[i]);
    }
}

void Uart::init(uint8_t uartNum) {
    //! APB2 - USART1,6 - 84 MHz    **** APB1 - USART2-5 - 42 MHz
    switch (uartNum) {
    case 1: {
        //! _________________USART 1 init  pin B6-Tx B7-Rx AF7___________________________________
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
        GPIOB->MODER &= ~(GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
        GPIOB->MODER |= (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1); // 1:0 -alternate mode
        GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;                    // 1:1 - very high speed
        GPIOB->AFR[0] |= (7 << 28) | (7 << 24);                      // alt func 7-uart1 to 6 pin and to 7 pin
        //------------------- USART -------------------------------------
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        USART1->CR1 &= ~USART_CR1_M;     // 0 - 8 Data bits
        USART1->CR1 |= USART_CR1_TE;     // Transmission enabled
        USART1->CR1 |= USART_CR1_RE;     // Recieving enabled
        USART1->CR1 |= USART_CR1_RXNEIE; // enable interrupt on Rx from usart1
        // USART1->BRR=0x445C; //2400 (APB2CLK - baudrate/2)/baudrate
        // APB2 clk =84000000
        USART1->BRR = 0x222D; // 9600 //(84000000-4800)/9600
        // USART1->BRR=0x9C3; //9600 //(48000000-4800)/9600
        // USART1->BRR=0x16C; //115200
        USART1->CR1 |= USART_CR1_UE; // USART EN
        NVIC_EnableIRQ(USART1_IRQn);
    } break;
    case 2: {
        //! _________________USART 2 init  pin D5-Tx D6-Rx AF7___________________________________
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
        GPIOD->MODER &= ~(GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0);
        GPIOD->MODER |= (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1); // 1:0 -alternate mode
        GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5;                    // 1:1 - very high speed
        GPIOD->AFR[0] |= (7 << 24) | (7 << 20);                      // alt func 7-uart2 to 5 pin and to 6 pin
        //------------------- USART -------------------------------------
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
        USART2->CR1 &= ~USART_CR1_M;     // 0 - 8 Data bits
        USART2->CR1 |= USART_CR1_TE;     // Transmission enabled
        USART2->CR1 |= USART_CR1_RE;     // Recieving enabled
        USART2->CR1 |= USART_CR1_RXNEIE; // enable interrupt on Rx from usart1
        USART2->BRR = 0x1116;            // 9600 //(42000000-4800)/9600
        USART2->CR1 |= USART_CR1_UE;     // USART EN
        NVIC_EnableIRQ(USART2_IRQn);
    } break;
    case 3: {
        //! _________________USART 3 init  pin D8-Tx D9-Rx AF7___________________________________
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
        GPIOD->MODER &= ~(GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0);
        GPIOD->MODER |= (GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1); // 1:0 -alternate mode
        GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8;                    // 1:1 - very high speed
        GPIOD->AFR[1] |= (7 << 4) | (7 << 0);                        // alt func 7-uart2 to 8 pin and to 9 pin
        //------------------- USART -------------------------------------
        RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
        USART3->CR1 &= ~USART_CR1_M;     // 0 - 8 Data bits
        USART3->CR1 |= USART_CR1_TE;     // Transmission enabled
        USART3->CR1 |= USART_CR1_RE;     // Recieving enabled
        USART3->CR1 |= USART_CR1_RXNEIE; // enable interrupt on Rx from usart1
        USART3->BRR = 0x1116;            // 9600 //(42000000-4800)/9600
        USART3->CR1 |= USART_CR1_UE;     // USART EN
        NVIC_EnableIRQ(USART3_IRQn);
    } break;
    case 4: {
        //! _________________UART 4 init  pin A0-Tx C11-Rx AF8___________________________________
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
        GPIOA->MODER &= ~(GPIO_MODER_MODER0_0);
        GPIOA->MODER |= (GPIO_MODER_MODER0_1);    // 1:0 -alternate mode
        GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0; // 1:1 - very high speed
        GPIOC->MODER &= ~(GPIO_MODER_MODER11_0);
        GPIOC->MODER |= (GPIO_MODER_MODER11_1); // 1:0 -alternate mode

        GPIOA->AFR[0] |= (8 << 0);  // alt func 8 to 0 pin
        GPIOC->AFR[1] |= (8 << 12); // alt func 8 to 11 pin
        //------------------- USART -------------------------------------
        RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
        UART4->CR1 &= ~USART_CR1_M;     // 0 - 8 Data bits
        UART4->CR1 |= USART_CR1_TE;     // Transmission enabled
        UART4->CR1 |= USART_CR1_RE;     // Recieving enabled
        UART4->CR1 |= USART_CR1_RXNEIE; // enable interrupt on Rx from usart1
        UART4->BRR = 0x1116;            // 9600 //(42000000-4800)/9600
        UART4->CR1 |= USART_CR1_UE;     // USART EN
        NVIC_EnableIRQ(UART4_IRQn);
    } break;
    case 5: {
        //! _________________UART 5 init  pin C12-Tx D2-Rx AF8___________________________________
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
        GPIOC->MODER &= ~(GPIO_MODER_MODER12_0);
        GPIOC->MODER |= (GPIO_MODER_MODER12_1);   // 1:0 -alternate mode
        GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0; // 1:1 - very high speed
        GPIOD->MODER &= ~(GPIO_MODER_MODER2_0);
        GPIOD->MODER |= (GPIO_MODER_MODER2_1); // 1:0 -alternate mode

        GPIOC->AFR[1] |= (8 << 16); // alt func 8 to 12 pin
        GPIOD->AFR[0] |= (8 << 8);  // alt func 8 to 2 pin
        //------------------- USART -------------------------------------
        RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
        UART5->CR1 &= ~USART_CR1_M;     // 0 - 8 Data bits
        UART5->CR1 |= USART_CR1_TE;     // Transmission enabled
        UART5->CR1 |= USART_CR1_RE;     // Recieving enabled
        UART5->CR1 |= USART_CR1_RXNEIE; // enable interrupt on Rx from usart1
        UART5->BRR = 0x1116;            // 9600 //(42000000-4800)/9600
        UART5->CR1 |= USART_CR1_UE;     // USART EN
        NVIC_EnableIRQ(UART5_IRQn);
    } break;
    case 6: {
        //! _________________USART 6 init  pin C6-Tx C7-Rx AF8 ___________________________________
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
        GPIOC->MODER &= ~(GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
        GPIOC->MODER |= (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1); // 1:0 -alternate mode
        GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;                    // 1:1 - very high speed
        GPIOC->AFR[0] |= (8 << 28) | (8 << 24);                      // alt func 7-uart1 to 6 pin and to 7 pin
        //------------------- USART -------------------------------------
        RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
        USART6->CR1 &= ~USART_CR1_M;     // 0 - 8 Data bits
        USART6->CR1 |= USART_CR1_TE;     // Transmission enabled
        USART6->CR1 |= USART_CR1_RE;     // Recieving enabled
        USART6->CR1 |= USART_CR1_RXNEIE; // enable interrupt on Rx from usart1
        USART6->BRR = 0x222D;            // 9600 //(84000000-4800)/9600
        USART6->CR1 |= USART_CR1_UE;     // USART EN
        NVIC_EnableIRQ(USART6_IRQn);
    } break;
    }
}

extern "C" void USART6_IRQHandler(void) {
    if (USART6->SR & USART_SR_RXNE) {
        USART6->SR &= ~USART_SR_RXNE;
    }    
}

extern "C" void USART1_IRQHandler(void) {
    if (USART1->SR & USART_SR_RXNE) {
        USART1->SR &= ~USART_SR_RXNE;
    }
    Uart::pThis[1]->byteRead = USART6->DR;
    Uart::pThis[1]->interruptFlag = true;
}
extern "C" void USART2_IRQHandler(void) {
    if (USART2->SR & USART_SR_RXNE) {
        USART2->SR &= ~USART_SR_RXNE;
    }
    Uart::pThis[2]->byteRead = USART6->DR;
    Uart::pThis[2]->interruptFlag = true;
}
extern "C" void USART3_IRQHandler(void) {
    if (USART3->SR & USART_SR_RXNE) {
        USART3->SR &= ~USART_SR_RXNE;
    }
    Uart::pThis[3]->byteRead = USART6->DR;
    Uart::pThis[3]->interruptFlag = true;
}
extern "C" void UART4_IRQHandler(void) {
    if (UART4->SR & USART_SR_RXNE) {
        UART4->SR &= ~USART_SR_RXNE;
    }
    Uart::pThis[4]->byteRead = USART6->DR;
    Uart::pThis[4]->interruptFlag = true;
}
extern "C" void UART5_IRQHandler(void) {
    if (UART5->SR & USART_SR_RXNE) {
        UART5->SR &= ~USART_SR_RXNE;
    }
    Uart::pThis[5]->byteRead = USART6->DR;
    Uart::pThis[5]->interruptFlag = true;
}
