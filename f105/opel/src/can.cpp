#include "can.h"

CAN_HandleTypeDef hcan1{0};
CAN_HandleTypeDef hcan2{0};

Can* Can::pThis = nullptr;

Can::Can() {
    pThis = this;
    init();
}


HAL_StatusTypeDef Can::send(const uint8_t* data, uint8_t size) {
    return HAL_CAN_AddTxMessage(&hcan1, &TxHeader, (uint8_t*)data, &TxMailbox);
}

char Can::intToCharFirst(uint8_t val) {    
    return (val/10)+0x30;
}
char Can::intToCharSecond(uint8_t val){
    return (val%10)+0x30;
}

void Can::setVoltage(uint8_t voltageInteger, uint8_t voltageFraction) {
    BAT3ADC[5] = intToCharFirst(voltageInteger);
    BAT3ADC[7] = intToCharSecond(voltageInteger);

    BAT4ADC[5] = intToCharFirst(voltageFraction);
    //BAT4ADC[7] = intToCharSecond(voltageInteger);    
}

void Can::init() {
    MX_GPIO_Init();
    MX_CAN_Init();
    /* USER CODE BEGIN 2 */
    TxHeader.StdId = 0x0290;
    TxHeader.ExtId = 0;
    TxHeader.RTR = CAN_RTR_DATA; // CAN_RTR_REMOTE
    TxHeader.IDE = CAN_ID_STD;   // CAN_ID_EXT
    TxHeader.DLC = 8; //data size
    TxHeader.TransmitGlobalTime = (FunctionalState)0;
    HAL_CAN_Start(&hcan1);
    //HAL_CAN_ActivateNotification(&hcan1,
    //                             CAN_IT_RX_FIFO1_MSG_PENDING | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_ERROR | CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE);
    HAL_CAN_ActivateNotification(&hcan1,
                                 CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_ERROR | CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE);
    HAL_CAN_Start(&hcan2);
    HAL_CAN_ActivateNotification(&hcan2,
                                 CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_ERROR | CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE);
}

void Can::MX_CAN_Init(void) {

    

    //------------ CAN 1 for listening ------------------------------
    hcan2.Instance = CAN1;
    hcan2.Init.Prescaler = 16;    
    hcan2.Init.Mode = CAN_MODE_SILENT;//CAN_MODE_LOOPBACK;//
    hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ; // becomes significant on high speeds
    hcan2.Init.TimeSeg1 = CAN_BS1_13TQ;
    hcan2.Init.TimeSeg2 = CAN_BS2_1TQ;
    hcan2.Init.TimeTriggeredMode = DISABLE; //nodes synchronize mechanism (additional, not allowed by all devices)
    hcan2.Init.AutoBusOff = ENABLE; //automatic resets bus after error overflow
    hcan2.Init.AutoWakeUp = DISABLE; //
    hcan2.Init.AutoRetransmission = ENABLE; // if no ack send another time
    hcan2.Init.ReceiveFifoLocked = DISABLE; // circle buffer in receive FIFO (if ENABLED - usual buffer)
    hcan2.Init.TransmitFifoPriority = ENABLE; // usual fifo mechanism (if disabled priority mechanism tranmittion enabled)
    if (HAL_CAN_Init(&hcan2) != HAL_OK) {
        //Error_Handler();
    }
    CAN_FilterTypeDef sFilterConfig1;
    sFilterConfig1.FilterBank = 0;
    sFilterConfig1.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig1.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig1.FilterIdHigh = 0x0000;
    sFilterConfig1.FilterIdLow = 0x0000;
    sFilterConfig1.FilterMaskIdHigh = 0x0000;
    sFilterConfig1.FilterMaskIdLow = 0x0000;
    sFilterConfig1.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig1.FilterActivation = ENABLE;
    //sFilterConfig1.SlaveStartFilterBank = 14; //only for second can
    if (HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig1) != HAL_OK) {
        //Error_Handler();
    }

//------------ CAN 2 for transmit ------------------------------

    //hcan.State = HAL_CAN_STATE_RESET;
    hcan1.Instance = CAN2;
    hcan1.Init.Prescaler = 4;
    //CAN_MODE_NORMAL  CAN_MODE_LOOPBACK   CAN_MODE_SILENT   CAN_MODE_SILENT_LOOPBACK
    hcan1.Init.Mode = CAN_MODE_NORMAL;//CAN_MODE_LOOPBACK;//
    //hcan.Init.Mode = CAN_MODE_LOOPBACK;//CAN_MODE_LOOPBACK;//
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ; // becomes significant on high speeds
    hcan1.Init.TimeSeg1 = CAN_BS1_16TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
    hcan1.Init.TimeTriggeredMode = DISABLE; //nodes synchronize mechanism (additional, not allowed by all devices)
    hcan1.Init.AutoBusOff = ENABLE; //automatic resets bus after error overflow
    hcan1.Init.AutoWakeUp = DISABLE; //
    hcan1.Init.AutoRetransmission = ENABLE; // if no ack send another time
    hcan1.Init.ReceiveFifoLocked = DISABLE; // circle buffer in receive FIFO (if ENABLED - usual buffer)
    hcan1.Init.TransmitFifoPriority = ENABLE; // usual fifo mechanism (if disabled priority mechanism tranmittion enabled)
    if (HAL_CAN_Init(&hcan1) != HAL_OK) {
        //Error_Handler();
    }
    CAN_FilterTypeDef sFilterConfig;
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    //sFilterConfig.FilterIdHigh = 0x06A6 << 5;
    //sFilterConfig.FilterIdLow = 0x0000; //for 18 bit id
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    //sFilterConfig.FilterMaskIdHigh = 0x07FE << 5; //last bit check
    //sFilterConfig.FilterMaskIdLow = 0x0000; // for 18 bit id
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;
    //sFilterConfig.FilterActivation = DISABLE;
    sFilterConfig.SlaveStartFilterBank = 0x2d;
    //sFilterConfig.SlaveStartFilterBank = 14;
    //hcan1.Instance = CAN1;
    if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK) {
        Error_Handler();
    }
    hcan1.Instance = CAN2;
}

void Can::MX_GPIO_Init(void) {
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    //__HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
}



void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
    if (HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &Can::pThis->RxHeader, Can::pThis->RxData) == HAL_OK) {
        if (Can::pThis->RxHeader.StdId == 0x175) {
            if(Can::pThis->RxData[2] == 0x40) {
                if(Can::pThis->flagMenu) {
                    Button::pThis->butState = ButtonState::UP_DOWN;
                    Can::pThis->flagMenu = false;
                } else {
                    Button::pThis->butState = ButtonState::DOWN;
                }                
            }
            if(Can::pThis->RxData[2] == 0x80) {
                if(Can::pThis->flagMenu) {
                    Button::pThis->butState = ButtonState::UP_DOWN;
                    Can::pThis->flagMenu = false;
                } else {
                    Button::pThis->butState = ButtonState::UP;
                }
            }
            if(Can::pThis->RxData[2] == 0xC0) {
                if(Can::pThis->flagMenu) {
                    Button::pThis->butState = ButtonState::UP_DOWN;
                    Can::pThis->flagMenu = false;
                    Can::pThis->flagTimerHalfSecStart = false;
                    Can::pThis->timerHalfSec = 0;
                } else {
                    //TODO:start timer
                    if(!Can::pThis->flagTimerHalfSecStart) {
                        Can::pThis->flagTimerHalfSecStart = true;
                        Can::pThis->timerHalfSec = 0;
                    }                     
                }                
            }
            if(Can::pThis->RxData[2] == 0x00) {
                if(Can::pThis->flagTimerHalfSecStart) {
                    if (Can::pThis->timerHalfSec < 3) {                        
                        Can::pThis->flagTimerHalfSecStart = false;
                        Can::pThis->timerHalfSec = 0;
                        Button::pThis->butState = ButtonState::DOWN_LONG;                        
                    } else {
                        Can::pThis->timerHalfSec = 0;
                        Can::pThis->flagTimerHalfSecStart = false;
                        Button::pThis->butState = ButtonState::UP_LONG;
                    }
                }
            }
            if (!(GPIOB->IDR & GPIO_IDR_IDR10)) {
                if (Can::pThis->RxData[3] == 0x06) {
                //     if (Can::pThis->flagTimerHalfSecStart) {
                //         if (Can::pThis->timerHalfSec < 3) {
                //             Can::pThis->flagTimerHalfSecStart = false;
                //             Can::pThis->timerHalfSec = 0;
                //             Button::pThis->butState = ButtonState::DOWN_LONG;
                //         }
                //         else {
                //             Can::pThis->timerHalfSec = 0;
                //             Can::pThis->flagTimerHalfSecStart = false;
                //             Button::pThis->butState = ButtonState::UP_LONG;
                //         }
                //     }
                    if (Can::pThis->flagMenu) {
                        // Button::pThis->butState = ButtonState::UP_DOWN;
                        // Can::pThis->flagMenu = false;
                    }
                    else {
                        Button::pThis->butState = ButtonState::DOWN_LONG;
                    }
                }
                if (Can::pThis->RxData[3] == 0x4) {
                    if (Can::pThis->flagMenu) {
                        // Button::pThis->butState = ButtonState::UP_DOWN;
                        // Can::pThis->flagMenu = false;
                    }
                    else {
                        Button::pThis->butState = ButtonState::DOWN;
                    }
                }
                if (Can::pThis->RxData[3] == 0x5) {
                    if (Can::pThis->flagMenu) {
                        // Button::pThis->butState = ButtonState::UP_DOWN;
                        // Can::pThis->flagMenu = false;
                    }
                    else {
                        Button::pThis->butState = ButtonState::UP;
                    }
                }
                if (Can::pThis->RxData[3] == 0x03) {
                    if (Can::pThis->flagMenu) {
                        Button::pThis->butState = ButtonState::UP_DOWN;
                        Can::pThis->flagMenu = false;
                    }
                    else {
                        Button::pThis->butState = ButtonState::UP_LONG;
                    }
                    // if (Can::pThis->flagMenu) {
                    //     Button::pThis->butState = ButtonState::UP_DOWN;
                    //     Can::pThis->flagMenu = false;
                    //     Can::pThis->flagTimerHalfSecStart = false;
                    //     Can::pThis->timerHalfSec = 0;
                    // }
                    // else {
                    //     //TODO:start timer
                    //     if (!Can::pThis->flagTimerHalfSecStart) {
                    //         Can::pThis->flagTimerHalfSecStart = true;
                    //         Can::pThis->timerHalfSec = 0;
                    //     }
                    // }
                }
            }
        }        
    }
    if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &Can::pThis->RxHeader, Can::pThis->RxData) == HAL_OK) {
        if(Can::pThis->RxHeader.StdId != 0x6A6) {
            if (Can::pThis->RxHeader.StdId == 0x308) {
                if(Can::pThis->RxData[0] == 0x07) {
                    Can::pThis->flagMenu = true;
                } else if(Can::pThis->RxData[0] == 0x03) {
                    Can::pThis->flagMenu = false;
                }                
            }
        }
    }
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
    //uint32_t er = HAL_CAN_GetError(&Can::pThis->hcan);
    HAL_CAN_GetError(&hcan2);
    //Uart::pThis->sendStr("ErrCallback");
    //Uart::pThis->sendByte(er);
}


void CEC_CAN_IRQHandler(void) { 
    HAL_CAN_IRQHandler(&hcan2);
}