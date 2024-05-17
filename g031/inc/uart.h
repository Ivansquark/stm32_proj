#ifndef UART_H
#define UART_H

#include "stm32g0xx.h"

class UART {
public:
    UART();
    static UART* pThis;
    void sendByte(uint8_t byte);
    void sendStr(const char* str);
    uint8_t received_byte=0;
    bool received_flag = false;
private:
    void init();
};

#endif //UART_H