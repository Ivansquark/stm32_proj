#ifndef HAL_INIT
#define HAL_INIT

#include "main.h"


void HAL_initialization();

#ifdef __cplusplus
extern "C" {
#endif    


//!_________________________    functions declarations  ___________________________________________
void SystemClock_Config();
void MX_GPIO_Init(void);
void MX_FDCAN1_Init(void);
//static void MX_USB_OTG_FS_PCD_Init(void);

//!__________________________ HAL descriptors declaration _________________________________________
extern ETH_HandleTypeDef heth;											       
extern ETH_TxPacketConfig TxConfig;                                            
extern FDCAN_HandleTypeDef hfdcan1;
//PCD_HandleTypeDef hpcd_USB_OTG_FS;

#ifdef __cplusplus
}
#endif

#endif //HAL_INIT