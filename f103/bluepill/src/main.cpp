#include "main.h"

volatile uint32_t ticks_delay;
extern void delay(uint32_t ms);
static __IO uint32_t s_timer;

extern "C" void SysTick_Handler(void)
{
    ticks_delay++;
}

void delay_ms(__IO uint32_t milisec) {
    // ------- SysTick CONFIG --------------
    if (SysTick_Config(24000)) { while(1);}
    uint32_t start = ticks_delay;
	while((ticks_delay - start) < milisec);
}

int main()
{
	rcc_ini();
	gpioc_ini();
	//usart1_ini();
	__enable_irq();		
	Spi2_master master;
	I2C1_master I2Cmast;
	ModbusMaster modbus;	

	//uint8_t byteRead=0;
	//GPIOC->BSRR|=GPIO_BSRR_BR13;
	//GPIOC->BSRR|=GPIO_BSRR_BS13;
	//GPIOC->BSRR=GPIO_BSRR_BR13;
	master.cs_idle();
	master.cs_set();
	//I2Cmast.writeBytes(0x0000,I2Cmast.arrDefault,20);
	//I2Cmast.rtc_write();
	//delay_ms(10);
	//I2Cmast.writeByte(0x0088,99);
	//I2Cmast.writeByte(0x0000,12);
	//delay_ms(10);
	
	//I2Cmast.readBytes(0x0000,20);
	//I2Cmast.arr[0]=I2Cmast.readByte(0x004A);
	ModbusMaster::counter=0;
	ModbusMaster::endReciveFlag=0;	
	ModbusMaster::delayCounter=0;
	GPIOC->ODR^=GPIO_ODR_ODR13;
	while(1)
	{		
		if(I2C1_master::timeFlag==1)  // по прерыванию от таймера считываем показания RTC1307
		{
			//GPIOC->ODR^=GPIO_ODR_ODR13;

			//I2Cmast.rtc_read();
			//master.sendBytes(&I2Cmast.rtcRead.sec); // отсылаем по SPI
			//master.sendBytes(ModbusMaster::Rx); // отсылаем по SPI
			master.sendByte(ModbusMaster::Rx[3]);
			master.sendByte(ModbusMaster::Rx[4]);
			master.sendByte(ModbusMaster::Rx[5]);
			master.sendByte(ModbusMaster::Rx[6]);
			master.sendByte(ModbusMaster::Rx[7]);
			master.sendByte(ModbusMaster::Rx[8]);
			master.sendByte(ModbusMaster::Rx[9]);
			master.sendByte(ModbusMaster::Rx[10]);
			I2C1_master::timeFlag=0;
			// посылаем запрос по ModBus...
			ModbusMaster::readRequest();			
		}		
		
		if (ModbusMaster::endReciveFlag==1) //функция обработки посылки.
		{
			
			if(ModbusMaster::counter>8)
			{
											
				ModbusMaster::readData();
						
			} 
			//ModbusMaster::counter=0;
			
			ModbusMaster::endReciveFlag=0;
		}
	}
    return 0;
}
