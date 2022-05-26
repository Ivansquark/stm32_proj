#ifndef UART_H
#define UART_H

#include "stm32f4xx.h"
#include "adc.h"
#include "normalqueue.h"

class Uart3 {
public:
    Uart3();
    void uart3_sendByte(uint8_t data);
    void uart3_sendStr(const char* str);
    static Uart3* pThis;
private:
    void uart3_init(void);
};
#endif //UART_H