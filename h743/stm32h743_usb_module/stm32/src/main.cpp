/*!
 * \file main file with main function from which program is started after
 * initialization procedure in startup.cpp
 *
 */

//*****************************************************
#include "main.h"
#include "stdint.h"
#include "stdlib.h"
//***************************************************
#include "buzzer.h"
#include "encoder.h"
#include "fmc.h"
#include "pardac.h"
#include "rcc_init.h"
#include "timer.h"
//#include "lcd_par.h"
#include "normalqueue.h"
#include "rtc.h"
#include "sd.h"
#include "watchdog.h"
//*******************************************************
#include "usb_device.h"
// void *__dso_handle = nullptr; // dummy "guard" that is used to identify
// dynamic shared objects during global destruction. (in fini in startup.cpp)
//_______________  GLOBAL VARIABLES OBJECTS and FUNCTIONS
//___________________________________________

bool StartPlay = false;
uint16_t Freq = 0;

extern "C" void _exit(int i) {
    while (1)
        ;
}
#define Pi 3.1415926535F
uint8_t arr1[512] = {0};
uint8_t arr2[512] = {0};
uint8_t arr3[512] = {0};
void Error_Handler(void) {}
void SystemClock_Config(void);
bool is_USB;
//______________________________________________________________________________________

int main() {
    StartPlay = false;
    Freq = 0;
    //_______________ MALLOC SECTION______________________________________
    RCC->AHB2ENR |= RCC_AHB2ENR_SRAM1EN; // enable clocks at ram_D2
    uint8_t* heapArr1 = (uint8_t*)malloc(5 * sizeof(uint8_t));
    for (int i = 0; i < 5; i++) { heapArr1[i] = 0x11; }
    //____________________________________________________________________

    // ----------  FPU initialization -----------------------------------
    SCB->CPACR |= ((3UL << 20) | (3UL << 22));
        /* set CP10 and CP11 Full Access */ // FPU enable
    //--------------------------------------------------------------------
    // RCC_INIT rcc(400);  // realy 360 MHz
    uint32_t x = 0;
    srand(x);
    is_USB = false;
    //--------------- objects initializations (without irq) ----------------
    //Rtc rtc;
    Buzzer buzz;
    // Independed_WatchDog watchDog(0x4FF);
    GP_Timers tim2(2, GP_Timers::Period::us);
    GP_Timers tim3_watchDog(3, GP_Timers::Period::ms);
    GP_Timers tim4_encoders(4, GP_Timers::Period::us); // encoders timer 100 us
    Encoder enc;
    //------------------------------------------------------------------------
    //--------------- HAL and objects with irq
    //initializations-------------------
    __enable_irq();
    HAL_Init();
    SystemClock_Config();
    // uint32_t clock = HAL_RCC_GetSysClockFreq();
    SD sd;
    MX_USB_DEVICE_Init(); // usb initialization function \TODO: set in method of
                          // Usb class
    Nor_LCD lcd;
    Figure fig;
    //-------------------------------------------------------------------------
    //__asm volatile ("cpsid i"); //turn off interrupts

    // uint8_t arr[512]={0};
    // for(int i=0; i<512; i++) {
    //	arr[i] = i+10;
    //}
    // addr = 512/4=128-address_in_uint32_t    128*100 - address of block number
    // 100
    // sd.WriteBlock(0,(uint32_t*)arr,512);
    // sd.EraseBlock(0,61056);
    NormalQueue8 que; // 128*8 SIZE queue

    char readFileBuffer[10] = {'0', '0', '0', '0', '0',
                               '0', '0', '0', '0', '0'};
    uint32_t z = 0;
    while (1) {
        if(StartPlay) {
            StartPlay = false;
            buzz.play(Freq);            
        }        
        //buzz.beep();
        x++;
        // fig.drawFilledTriangle((rand() % 800),(rand() % 480),
        //					   (rand() % 800),(rand() %
        //480), 					   (rand() % 800),(rand() % 480), (uint16_t)(rand() % 0xFFFF));
        // fig.drawRect((rand() % 800),(rand() % 480),(rand() % 800),(rand() %
        // 480), (rand() % 0xFFFF)); tim2.delay_ms(2500); lcd.writeData(x++);
        if (enc.But_PA3) {
            if (enc.enc_counter != 0) {
                enc.enc_counter--;
            } else {
                enc.enc_counter = 360;
            }
            fig.drawFatLineOnCircle(400, 240, 150, enc.enc_counter + 5, 5, 40,
                                    fig.YELLOW);
            fig.drawFatLineOnCircle(400, 240, 150, enc.enc_counter, 5, 40,
                                    fig.RED);
            // enc.SM_backward(10);
            enc.But_PA3 = 0;
        }
        if (enc.But_PB5) {
            if (enc.enc_counter != 360) {
                enc.enc_counter++;
            } else {
                enc.enc_counter = 0;
            }
            fig.drawFatLineOnCircle(400, 240, 150, enc.enc_counter, 5, 40,
                                    fig.YELLOW);
            fig.drawFatLineOnCircle(400, 240, 150, enc.enc_counter + 5, 5, 40,
                                    fig.RED);
            // enc.SM_forward(10);
            enc.But_PB5 = false;
        }
        if (enc.But_PC9) {
            uint16_t counter = 5;
            while (counter) {
                if (tim2.TimSets) { // 1 ms
                    buzz.beep();
                    counter--;
                    tim2.TimSets = false;
                }
            }
            buzz.off();
            enc.But_PC9 = false;
        }
        // watchDog.refreshCounter(); // resets in counter
        // show time:
        //rtc.getTime();
    }
    return 0;
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
    __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    // frequency
    RCC_OscInitStruct.PLL.PLLN = 360;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_1;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 |
                                  RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection =
        RCC_PERIPHCLK_SDMMC | RCC_PERIPHCLK_USB;
    PeriphClkInitStruct.PLL3.PLL3M = 1;
    PeriphClkInitStruct.PLL3.PLL3N = 48;
    PeriphClkInitStruct.PLL3.PLL3P = 2;
    PeriphClkInitStruct.PLL3.PLL3Q = 8;
    PeriphClkInitStruct.PLL3.PLL3R = 2;
    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_3;
    PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
    PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL;
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        Error_Handler();
    }
    HAL_PWREx_EnableUSBVoltageDetector();
}

