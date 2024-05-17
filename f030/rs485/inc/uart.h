#ifndef UART_H
#define UART_H

#include "stm32f0xx.h"

class Uart {
  public:
    Uart();
    static Uart *pThis;
    void sendByte(uint8_t byte);
    void sendStr(const char *str);
    uint8_t receivedByte = 0;

  private:
    void init();
};

#endif // UART_H