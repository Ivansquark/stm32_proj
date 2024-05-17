#ifndef UART_H
#define UART_H

#include "main.h"

class Uart
{
public:
    Uart(uint8_t num = 1);
    void sendByte(uint8_t byte);
    void sendStr(const char* str);
    void sendBuf(uint8_t* data, uint8_t len);
    const uint8_t uartNum;
    bool interruptFlag=false;
    uint8_t byteRead=0;
    static Uart* pThis[7];

    static constexpr uint8_t TEMP_SIZE = 10;
    uint8_t tempBuff[TEMP_SIZE] = {0};
    uint8_t tempBuffCounter = 0;
private:
    void init(uint8_t num);
    Uart(const Uart&) = delete;
    Uart(Uart&&) = delete;
};

extern "C" void USART1_IRQHandler(void);
extern "C" void USART2_IRQHandler(void);
extern "C" void USART3_IRQHandler(void);
extern "C" void UART4_IRQHandler(void);
extern "C" void UART5_IRQHandler(void);
extern "C" void USART6_IRQHandler(void);

#endif //UART_H
