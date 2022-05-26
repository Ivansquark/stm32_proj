#include "main.h"


int main(void) {
    NormalQueue8 queue;
		__disable_irq();
    //! Enable FPU and sleep
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
    Sleep sleep;
    //! RCC
    Rcc rcc(48);    
    //Flash flash; //if no ar24c uses
    //float arrFlash_write[] = {1.1,2.2,3.3,4.4,5.5,6.6};
    //flash.write_buf(arrFlash_write);
    //float arrFlash_read[6] = {0};
    //flash.read_buf(arrFlash_read,24);   
    At24c at24;
    Adc adc;
    Timer3 timer3;
    Timer_antiRattle tAntiRattle;
    Timer_Bat tim_bat;
		__enable_irq();
    USB_DEVICE usb;
    
    Display display;    
    Uart3 uart3;    
    int x=111;    
    display.setPos(1,0);	
    display.sendStr((int8_t*)"Calibrator");
    display.setPos(0,1);	
    display.sendStr((int8_t*)"initialization...");
	Button butLED;
    timer3.delay_ms(2000);
    timer3.delay_ms(2000);    
    //float at24_write = 7.6;    
    //uint32_t te= *(uint32_t*)(&at24_write);    
    //at24.writeBytes(0,(uint8_t*)&te,4);
    //timer3.delay_ms(1);
	uint8_t at24_read[12] = {0};
    at24.readBytes(0,at24_read,4);
    
	while(1) { 			
/////////////////////////  UART or USB COEFFICIENTS MANIPULATION ????????????????????????????????????????
        if(adc.DataIsRead) {
            adc.DataIsRead = false;           
            
            //uart3_sendByte(0x55); uart3_sendByte(0xCC);
            //for(int i=0;i<4;i++) {
            //    uart.uart3_sendByte(*((uint8_t*)&adc->K_1 + i));
            //}
            //for(int i=0;i<4;i++) {
            //    uart.uart3_sendByte(*((uint8_t*)&adc->k_dx + i));
            //}
            //for(int i=0;i<4;i++) {
            //    uart.uart3_sendByte(*((uint8_t*)&adc->k_ln + i));
            //}            
			
            usb.tx_arr[0] = 0x55;      
						usb.tx_arr[1] = 0xCC;
            usb.tx_arr[2] = *((uint8_t*)&adc.K_1); usb.tx_arr[3] = *((uint8_t*)(&adc.K_1) +1);
            usb.tx_arr[4] = *((uint8_t*)(&adc.K_1) +2); usb.tx_arr[5] = *((uint8_t*)(&adc.K_1) + 3);
            usb.tx_arr[6] = *((uint8_t*)&adc.k_dx); usb.tx_arr[7] = *((uint8_t*)(&adc.k_dx) +1);
            usb.tx_arr[8] = *((uint8_t*)(&adc.k_dx) +2); usb.tx_arr[9] = *((uint8_t*)(&adc.k_dx) + 3);
            usb.tx_arr[10] = *((uint8_t*)&adc.k_ln); usb.tx_arr[11] = *((uint8_t*)(&adc.k_ln) +1);
            usb.tx_arr[12] = *((uint8_t*)(&adc.k_ln) +2); usb.tx_arr[13] = *((uint8_t*)(&adc.k_ln) + 3);
            usb.WriteINEP(0x01,(uint8_t*)(usb.tx_arr),14);   
            adc.receved_Flag=false;
        }
        if(adc.NewCoefReadyToWrite) {
            adc.NewCoefReadyToWrite = false;
            uint32_t K_1 = (adc.received_Data[0] | (adc.received_Data[1] << 8) |
                                (adc.received_Data[2] << 16) | (adc.received_Data[3] << 24));
            uint32_t k_dx = (adc.received_Data[4] | (adc.received_Data[5] << 8) |
                                (adc.received_Data[6] << 16) | (adc.received_Data[7] << 24));
            uint32_t k_ln = (adc.received_Data[8] | (adc.received_Data[9] << 8) |
                                (adc.received_Data[10] << 16) | (adc.received_Data[11] << 24));
            uint8_t arrK[24] = {0};
            for(int i=0; i<12; i++) {
                arrK[i] = adc.received_Data[i];
            }
            adc.K_1 = *(float*)(&K_1);
            adc.k_dx = *(float*)(&k_dx);
            adc.k_ln = *(float*)(&k_ln);            
            //flash.write_buf(adc.received_Data); // write received data with float coeef array into flash
            //adc.set_Coefficients();
            adc.NewCoefReadyToWrite = 0;            
            adc.receved_Flag=false;
            at24.writeBytes(0,arrK,12);
        }        
//////////////////////////////////////////////////////////////////////////////////////////////////////// 

/////////////////  LED BUTTON  /////////////////////////////////////////////////////////////////////////////        
        if(display.LED_flag && !display.LED_state()){ // LCD LED on-off
            display.LED_on();
        } else if(!display.LED_flag && display.LED_state()){
            display.LED_off();
        }				
////////////////////////////////////////////////////////////////////////////////////////////////////////////////        

        
///////////////////////  ADC  ///////////////////////////////////////////////////////////////////////////////////////        
        if(adc.Start_diods_Flag) {
            adc.read_diods_ADC();
            adc.Start_diods_Flag = false;            
        }        
        if(adc.Start_battery_Flag) {
			adc.read_battery_ADC();
            adc.Start_battery_Flag = false;	            
            display.diodeOn(); // Blink diode
            timer3.delay_ms(1);
            display.diodeOff();//timer3.delay_ms(100);            				  
        }
////////////////////////////////////////////////////////////////////////////////////////////////////////////
				
/////////////////////////// Read USB Rx buffer //////////////////////////////////////////////////
	if(usb.Receive_FLAG) {
		if(usb.rx_arr[0] == (char)0xAA && usb.rx_arr[1] == (char)0xAA) { //send ADCs
       usb.USB_send_ADC_FLAG = 1;
    }
	if(usb.rx_arr[0] == (char)0xDE && usb.rx_arr[1] == (char)0xAD) { // read coefs
           adc.DataIsRead = 1;       
       }
    if(usb.rx_arr[0] == (char)0xAB && usb.rx_arr[1] == (char)0xCD) { // write coefs
        adc.DataIsWrite = 1;     
        adc.NewCoefReadyToWrite = 1;
        adc.received_Data[0] = usb.rx_arr[2];
        adc.received_Data[1] = usb.rx_arr[3];
        adc.received_Data[2] = usb.rx_arr[4];
        adc.received_Data[3] = usb.rx_arr[5];
        adc.received_Data[4] = usb.rx_arr[6];
        adc.received_Data[5] = usb.rx_arr[7];
        adc.received_Data[6] = usb.rx_arr[8];
        adc.received_Data[7] = usb.rx_arr[9];    
        adc.received_Data[8] = usb.rx_arr[10];
        adc.received_Data[9] = usb.rx_arr[11];
        adc.received_Data[10] = usb.rx_arr[12];
        adc.received_Data[11] = usb.rx_arr[13];
    }
    usb.Receive_FLAG = false;
}			
/////////////////////////////////////////////////////////////////////////////////////////////////				

        
//////////////////////  Display and UART (USB) asynchronious send data ///////////////////////////////////////    				
      if(Timer3::pThis->Display_Show_Flag) {          
         timer3.delay_ms(1); //without this display crushes          
          if(usb.USB_send_ADC_FLAG) {
              usb.tx_arr[0] = 0xAA;
              usb.tx_arr[1] = 0xDD;
              usb.tx_arr[2] = (adc.ADC_Diode1);
              usb.tx_arr[3] = (adc.ADC_Diode1>>8);
              usb.tx_arr[4] = (adc.ADC_Diode2);
              usb.tx_arr[5] = (adc.ADC_Diode2>>8);
              usb.WriteINEP(0x01,(uint8_t*)(usb.tx_arr),6);
              usb.USB_send_ADC_FLAG = 0;
          }     
                      
            //if(!(adc.receved_Flag || adc.DataIsRead || adc.DataIsWrite )) {
            //     uart.uart3_sendByte(0xAA);uart.uart3_sendByte(0xDD);            
            //    for(int i=0;i<2;i++) {                
            //         uart.uart3_sendByte(*((uint8_t*)&adc.ADC_Diode1 + i));
            //    }
            //    for(int i=0;i<2;i++) {
            //         uart.uart3_sendByte(*((uint8_t*)&adc.ADC_Diode2 + i));
            //    }                                
            //} 					
            //! show diods parameters
                display.clearDisplay();
            display.setPos(0,0);        
            display.intToStr(adc.ADC_Diode1);			
            display.sendStr(display.arrStr);
            display.setPos(0,1);        
            display.intToStr(adc.ADC_Diode2);			
            display.sendStr(display.arrStr);
            //! show battery parameters
            display.setPos(7,1); 
			display.floatToStr(adc.Battery_V);		
            display.sendStr(display.arrFloat);            
            display.setPos(12,1); 
            display.sendStr((int8_t*)"BatV");
            //! show kV values
            display.setPos(8,0);        
            display.floatToStr(adc.kV);			
            display.sendStr(display.arrFloat);
			display.setPos(14,0);        
            display.sendStr((int8_t*)"kV");						         
            timer3.delay_ms(200);						
			timer3.Display_Show_Flag = false;
        } // Display        
/////////////////////////////////////////////////////////////////////////////////////////////////   
        sleep.sleep(); // sleep until any interrupt
/////////////////////////////////////////////////////////////////////////////////////////////////        
	}
    return 0;
}

void delay(uint32_t x) {
    while(x--);
}
