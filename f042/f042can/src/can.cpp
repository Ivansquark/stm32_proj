#include "can.h"

Can* Can::pThis = nullptr;

Can::Can() {
    pThis = this;
    init();
}


HAL_StatusTypeDef Can::send(const uint8_t* data, uint8_t size) {
    return HAL_CAN_AddTxMessage(&hcan, &TxHeader, (uint8_t*)data, &TxMailbox);
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
    HAL_CAN_Start(&hcan);
    HAL_CAN_ActivateNotification(&hcan,
                                 CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_ERROR | CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE);
}

void Can::MX_CAN_Init(void) {

    //hcan.State = HAL_CAN_STATE_RESET;
    hcan.Instance = CAN;
    hcan.Init.Prescaler = 42;
    //CAN_MODE_NORMAL  CAN_MODE_LOOPBACK   CAN_MODE_SILENT   CAN_MODE_SILENT_LOOPBACK
    hcan.Init.Mode = CAN_MODE_LOOPBACK;
    hcan.Init.SyncJumpWidth = CAN_SJW_1TQ; // becomes significant on high speeds
    hcan.Init.TimeSeg1 = CAN_BS1_6TQ;
    hcan.Init.TimeSeg2 = CAN_BS2_1TQ;
    hcan.Init.TimeTriggeredMode = DISABLE; //nodes synchronize mechanism (additional, not allowed by all devices)
    hcan.Init.AutoBusOff = ENABLE; //automatic resets bus after error overflow
    hcan.Init.AutoWakeUp = DISABLE; //
    hcan.Init.AutoRetransmission = ENABLE; // if no ack send another time
    hcan.Init.ReceiveFifoLocked = DISABLE; // circle buffer in receive FIFO (if ENABLED - usual buffer)
    hcan.Init.TransmitFifoPriority = ENABLE; // usual fifo mechanism (if disabled priority mechanism tranmittion enabled)
    if (HAL_CAN_Init(&hcan) != HAL_OK) {
        //Error_Handler();
    }
    CAN_FilterTypeDef sFilterConfig;
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;
    // sFilterConfig.SlaveStartFilterBank = 14;
    if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK) {
        //Error_Handler();
    }
}

void Can::MX_GPIO_Init(void) {
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (HAL_CAN_GetRxMessage(&Can::pThis->hcan, CAN_RX_FIFO0, &Can::pThis->RxHeader, Can::pThis->RxData) == HAL_OK) {
        //uart.sendStr("RxFifo");
        //for(int i = 0; i<8; i++) {
        //    Uart::pThis->sendByte(Can::pThis->RxData[i]);
        //}
    }
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
    uint32_t er = HAL_CAN_GetError(&Can::pThis->hcan);
    Uart::pThis->sendStr("ErrCallback");
    Uart::pThis->sendByte(er);
}


void CEC_CAN_IRQHandler(void) { 
    HAL_CAN_IRQHandler(&Can::pThis->hcan);
}