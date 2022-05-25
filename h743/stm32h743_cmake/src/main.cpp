/*!
 * \file main file with main function from which program is started after initialization procedure in startup.cpp * 
 */

#include "main.h"
#include "global_variables.h"
#include "hal_init.h"
//*****************************
#include "buzzer.h"
#include "net.h"
#include "timer.h"
#include "watchdog.h"
#include "cache.h"
#include "sd_spi.h"
#include "buttons.h"
#include "flash.h"
//*****************************
//#include "usb_device.h"
//*****************************

//!_______________________   GLOBAL NET OBJECTS    ________________________________________________
extern struct netif gnetif;                                             // LWiP struct declaration
uint8_t ethernetDataBuffer[0x1000a] = {0};    		                    // buffer Rx and Tx data 
Ethernet ethernet;
//_________________________________________________________________________________________________

//!________________________  OTHER GLOBAL OBJECTS   _______________________________________________
//SD sd;

//Buzzer buzz;
Flash flash;
Buttons buttons;
    
//_________________________________________________________________________________________________

int main() {       
    corePreparation();	
    //flash.writeBufBank2(3, &temp, sizeof(temp));  // check for debug
    //sd.fat_init();
    //--------------------- HAL and objects (on stack) with irq initializations    ----------------
    HAL_initialization();                           // sys clock initialization and get sys FREQUENCY

    GP_Timers tim2(2);
    GP_Timers untirattleTimer(5);
    //Independed_WatchDog watchDog(0x4FF);
    //GP_Timers tim3_watchDog(3);
	//!-----------------------  NET initialization    ---------------------------------------------
    MX_LWIP_Init();
	ethernet.createServer();
    HAL_Delay(1);
    //!-----------------------------    UDP Connection   ------------------------------------------
    ethernet.udp_client_connect();
    //!-----------------------------    MAIN LOOP   -----------------------------------------------		  
	while(1) {	
		//!-------------------------    ETHERNET polling    ---------------------------------------
        //ethernetif_input(&gnetif);  // receiving data if we have no data for send
		//sys_check_timeouts();
        //!------------------------ Reinit section  -----------------------------------------------
        MX_LWIP_Process();
	}
	return 0;
}

void Error_Handler(void) {
    while(1){}
}


// uint32_t get_fattime (void) {	
//   return	  (0)
// 	  | (0)
// 	  | (0)
// 	  | (0)
// 	  | (0)
// 	  | (0);
// }

