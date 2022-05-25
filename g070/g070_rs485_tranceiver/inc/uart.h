#ifndef UART_H
#define UART_H

#include "stm32g070xx.h"
#include "queue.h"

//! @brief uart transiever from 2 to 4

#define PIN_OFF ((GPIOB->BSRR) |= (GPIO_BSRR_BR1))
#define PIN_ON ((GPIOB->BSRR) |= (GPIO_BSRR_BS1))

extern volatile uint8_t uartRS485_received_packet;
extern volatile uint8_t uartLinux_received_packet;

//! RS485 usart2
void uartRS485_init(Queue* q);
void pin_init();
void uartRS485_sendByte(uint8_t byte);

//! Linux usart4
void uartLinux_init(Queue* q);
void uartLinux_sendByte(uint8_t byte);

#endif //UART_H