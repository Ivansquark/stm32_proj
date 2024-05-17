/*!
 * \file main file with main function from which program is started after initialization procedure in startup.cpp
 * 
 */
//#define __cplusplus 2017

#include "main.h"
#include "stdint.h"
#include "stdlib.h"
//*****************************
#include "buzzer.h"
#include "timer.h"
#include "watchdog.h"
#include "cache.h"
//*****************************
//#include "usb_device.h"
//*****************************

//*********** ETHERNET ********
#include "mrt_proto_slots.h"
//*****************************
//#include "ff.h"
//void *__dso_handle = nullptr; // dummy "guard" that is used to identify dynamic shared objects during global destruction. (in fini in startup.cpp)
//extern "C" void _exit(int i) {while (1);}
void delay(volatile uint32_t val);

//!__________________________ NET _________________________________________________________________
ETH_HandleTypeDef heth;											// HAL descriptor definition
ETH_TxPacketConfig TxConfig;									// HAL struct definition
extern struct netif gnetif;										// LWiP struct declaration
extern volatile uint8_t ethernetDataBuffer[0x1000a];    		// buffer for data 
MRTProto::protoMSG msg;											// object in data segment
MRTProto::FrameParser frameParser(ethernetDataBuffer, &msg);  	// object in data segment
Ethernet ethernet(&msg);          		                        // object in data segment
//_________________________________________________________________________________________________

//_______________  GLOBAL VARIABLES OBJECTS and FUNCTIONS _______________________________

FDCAN_HandleTypeDef hfdcan1;
PCD_HandleTypeDef hpcd_USB_OTG_FS;
void Error_Handler(void){}
static void SystemClock_Config();
static void MX_GPIO_Init(void);
static void MX_FDCAN1_Init(void);
//static void MX_USB_OTG_FS_PCD_Init(void);

//______________________________________________________________________________________
int main() {
//_______________ MALLOC SECTION______________________________________		
	//RCC->AHB2ENR |= RCC_AHB2ENR_SRAM1EN; // enable clocks at ram_D2
	//uint8_t* heapArr1 = (uint8_t*)malloc(5*sizeof(uint8_t));
	//for(int i = 0; i < 5; i++) {
	//  heapArr1[i] = 0x11;
	//}
//_______________ CACHE PREPARATIONS ___________________________________________________
	cachePrepare();
//_______________ ETHERNET BUFFERS SECTION______________________________________	
	RCC->AHB2ENR |= RCC_AHB2ENR_SRAM2EN; // enable clocks SRAM2 (LWiP heap)	
	RCC->AHB2ENR |= RCC_AHB2ENR_SRAM3EN; // enable clocks SRAM3 (Ethernet DMA BUFFERS)    
// ----------  FPU initialization -----------------------------------
	SCB->CPACR |= ((3UL << 20)|(3UL << 22));  //FPU enable
//--------------------------------------------------------------------
	Buzzer buzz;
	//Independed_WatchDog watchDog(0x4FF);
	GP_Timers tim2(2,GP_Timers::Period::ms);
	//GP_Timers tim3_watchDog(3,GP_Timers::Period::ms);
//------------------------------------------------------------------------
//--------------- HAL and objects with irq initializations-------------------
	__enable_irq();
	HAL_Init();
	SystemClock_Config();
	HAL_Delay(100);
	//uint32_t clock = HAL_RCC_GetSysClockFreq();
	//---------/* Initialize all configured peripherals */------------------
	MX_GPIO_Init();
	MX_FDCAN1_Init();

	//-------------------- NET ---------------------------------------------
	MX_LWIP_Init();
	ethernet.createServer();
	frameParser.Connect(sndHeartBeat); 
	frameParser.Connect(reqSet);
	//MX_USB_OTG_FS_PCD_Init();
	
//-------------------------------------------------------------------------		
  //__asm volatile ("cpsid i"); //turn off interrupts
  volatile uint32_t counter=0;
	while(1) {	
		//------------------------- ETHERNET -------------------------------        		
		ethernet.checkForSendingData();
		ethernetif_input(&gnetif);
		sys_check_timeouts();
		//------------------------------------------------------------------
		//HAL_ETH_Transmit(&heth, &TxConfig,1);
		if(counter==100000) {
		  //GPIOB->ODR ^= GPIO_ODR_OD9;
		  counter=0;
		}
		counter++;
	}
	return 0;
}

void delay(volatile uint32_t val) {
	while(val) {
		val--;
	}
}



void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
	Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
							  |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
	Error_Handler();
  }
  HAL_PWREx_EnableUSBVoltageDetector();
}

static void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = DISABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 1;
  hfdcan1.Init.NominalSyncJumpWidth = 1;
  hfdcan1.Init.NominalTimeSeg1 = 2;
  hfdcan1.Init.NominalTimeSeg2 = 2;
  hfdcan1.Init.DataPrescaler = 1;
  hfdcan1.Init.DataSyncJumpWidth = 1;
  hfdcan1.Init.DataTimeSeg1 = 1;
  hfdcan1.Init.DataTimeSeg2 = 1;
  hfdcan1.Init.MessageRAMOffset = 0;
  hfdcan1.Init.StdFiltersNbr = 0;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.RxFifo0ElmtsNbr = 0;
  hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxFifo1ElmtsNbr = 0;
  hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxBuffersNbr = 0;
  hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.TxEventsNbr = 0;
  hfdcan1.Init.TxBuffersNbr = 0;
  hfdcan1.Init.TxFifoQueueElmtsNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
	Error_Handler();
  }
}
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

}

uint32_t get_fattime (void) {	
  return	  (0)
	  | (0)
	  | (0)
	  | (0)
	  | (0)
	  | (0);
}

