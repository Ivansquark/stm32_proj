#ifndef RS485_H
#define RS485_H

#include "main.h"
#include "modbus.h"

namespace RS485 {
    void init();
    void sendByte(uint8_t byte);
    void sendStr(const char *str);
    bool isReceived();

    void setByte(uint8_t byte);
    uint8_t getByte();
    
    void setRead();
    void setWrite();
}

#endif // RS485_H
