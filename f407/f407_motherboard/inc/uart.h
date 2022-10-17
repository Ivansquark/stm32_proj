#ifndef UART_H
#define UART_H

#include "stm32f4xx.h"

class Uart
{
public:
    Uart(uint8_t num = 1);
    void sendByte(uint8_t byte);
    void sendStr(const char* str);
    const uint8_t uartNum;
    bool interruptFlag=false;
    uint8_t byteRead=0;
    static Uart* pThis[7];
private:
    void init(uint8_t num);
};

extern "C" void USART1_IRQHandler(void);
extern "C" void USART2_IRQHandler(void);
extern "C" void USART3_IRQHandler(void);
extern "C" void UART4_IRQHandler(void);
extern "C" void UART5_IRQHandler(void);
extern "C" void USART6_IRQHandler(void);

#endif //UART_H