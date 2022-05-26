/*!
 * \file main file with main function from which program is started after
 * initialization procedure in startup.cpp *
 */

#include "main.h"
#include "global_variables.h"
#include "hal_init.h"
//*****************************
#include "cache.h"
#include "flash.h"
#include "leds.h"
#include "sysinit.h"
#include "watchdog.h"
//*****************************

SysInit sysInit; // FREQUENCY init
Flash flash;
Leds led;
void All_DeInit();

int main() {
    // flash.writeBootFlagAndSectorNum(3);
    if (flash.isNeedGoToFlashNewFirmware) {
        // start flash new firmware
        HAL_StatusTypeDef retVal = flash.flashNewFirmware();
        if (retVal != HAL_StatusTypeDef::HAL_OK) {
        } else {
            flash.clearBootFlag();
            //______________ Software RESET _______________________
            //__ To write to this register, you must write 0x5FA
            //__ to the VECTKEY field, otherwise the
            //__ processor ignores the write._____________________________
            SCB->AIRCR = 0x05FA0004;
        }
    } else {
        // TODO: goto firmware
        //! _____________	here proceed transition to new programm _____________________
        //clear stack
        extern void* _stack_begin;
        
        
        
        for(volatile uint32_t i = 0x10000; i < 0x20000; i++) {
            *((uint8_t*)(&_stack_begin)  + i) = 0;
        }
        //memset((void*)&stackEndPtr, 2, 0x10000);
        volatile uint32_t mainAppAddr = flash.FLASH_BANK1_FIRMWARE;
        volatile uint32_t mainAppStack =
            (volatile uint32_t) * ((volatile uint32_t *)mainAppAddr); // check first 4 bytes (where must be stack ptr)
        All_DeInit();
        void (*mainApplication)() = (void (*)()) * (uint32_t *)(mainAppAddr + 4); // Corrected!!!
        SCB->VTOR = (uint32_t)flash.FLASH_BANK1_FIRMWARE; // set new interrupt VECTOR table address
        __set_MSP(mainAppStack);                          // set new stack address
        mainApplication();                                // go to start main programm
        /*!< in linker main programm need to set PROGRAMM START ADDRESS to MAIN_PROGRAMM_ADDRESS (0x08020000 - sector
         * 1)>*/
    }
    while (1) {
        // if error togle led
        led.ledCommutationBoardOn();
        for(volatile int i = 0; i < 8000000; i++);
        led.ledCommutationBoardOff();
        for(volatile int i = 0; i < 8000000; i++);
    }
    return 0;
}

void All_DeInit() {
    HAL_RCC_DeInit();
    HAL_DeInit();
    //__disable_irq();
}

void Error_Handler(void) {
    while (1) {
    }
}
