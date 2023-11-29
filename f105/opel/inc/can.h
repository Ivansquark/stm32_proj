#ifndef CAN_H
#define CAN_H

//#include "stm32f0xx.h"
#include "main.h"
#include "button.h"
//#include "uart.h"

extern "C" void CEC_CAN_IRQHandler(void);
extern "C" void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
extern "C" void CEC_CAN_IRQHandler(void);

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

class Can {
public:
    Can();
    static Can* pThis;
    //CAN_HandleTypeDef hcan={0}; // Can descriptor structure

    volatile bool flagMenu = false;
    volatile bool flagTimerHalfSecStart = false;
    volatile bool flagTimerHalfPressedOnce = false;
    volatile uint8_t timerHalfSec = 0;
    
    CAN_TxHeaderTypeDef TxHeader = {0}; // transmit identifier
    CAN_RxHeaderTypeDef RxHeader = {0}; // received identifier
    uint8_t TxData[8] = {0};
    uint8_t RxData[8] = {0};
    uint32_t TxMailbox = 0; // number Tx Fifo

    const uint8_t MAIN[8] = {0x80, 0xE0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    const uint8_t BC[8] = {0x80, 0xE4, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    const uint8_t ENTER[8] = {0x80, 0x7D, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

    uint8_t UP[8] =   {0x80, 0x7C, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t DOWN[8] = {0x80, 0x7C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t TEMP_START[8] = {0x61, 0x00, 0x42, 0x17, 0x20, 0x00, 0x00, 0x00};
    uint8_t TEMP_ALLWAYS[8] = {0x61, 0x00, 0x42, 0x16, 0x21, 0x00, 0x42, 0x00};

    HAL_StatusTypeDef send(const uint8_t* data, uint8_t size);

    uint8_t SPACE[8] =      {0x45, 0x96, 0x00, 0x22, 0x40, 0x10, 0x00, 0x20};
    uint8_t BAT0[8] =       {0x04, 0x96, 0x00, 0x20, 0x00, 0x20, 0x00, 0x42};
    uint8_t BAT1[8] =       {0x03, 0x96, 0x00, 0x61, 0x00, 0x74, 0x00, 0x74};
    uint8_t BAT2[8] =       {0x02, 0x96, 0x00, 0x65, 0x00, 0x72, 0x00, 0x79};
    uint8_t BAT3ADC[8] =    {0x01, 0x96, 0x00, 0x20, 0x00, 0x31, 0x00, 0x33};
    uint8_t BAT4ADC[8] =    {0x00, 0x96, 0x00, 0x2E, 0x00, 0x38, 0x00, 0x56};

    void setVoltage(uint8_t voltageInteger, uint8_t voltageFraction);
    char intToCharSecond(uint8_t val);
    char intToCharFirst(uint8_t val);

private:
    void init();
    void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
    void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan);
    void MX_CAN_Init(void);
    void MX_GPIO_Init(void);
};


#endif //CAN_H