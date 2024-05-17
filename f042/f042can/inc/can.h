#ifndef CAN_H
#define CAN_H

//#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include "uart.h"

extern "C" void CEC_CAN_IRQHandler(void);
extern "C" void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
extern "C" void CEC_CAN_IRQHandler(void);

class Can {
public:
    Can();
    static Can* pThis;
    CAN_HandleTypeDef hcan={0}; // Can descriptor structure
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
    uint8_t TEMP[8] = {0x11, 0x00, 0x42, 0x16, 0x20, 0x00, 0x00, 0x00};

    HAL_StatusTypeDef send(const uint8_t* data, uint8_t size);

private:
    void init();
    void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
    void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan);
    void MX_CAN_Init(void);
    void MX_GPIO_Init(void);
};


#endif //CAN_H