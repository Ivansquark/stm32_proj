#include "main.h"

/************************************************************************
 * инициализация статических переменных в данной версии не работает по причине недоделаного startup  */
volatile ModBus* mod_ptr=nullptr;
volatile uint8_t TIM3_flag=0;	// флаг прерывания по таймеру 3
volatile uint8_t button_flag=0; // флаг прерывания по кнопке
volatile float g_xFloat=10.F;


//timer4 0.0001sec one symbol on 9600 ~1ms

//-----------------------------------------------------------------------------------------------------
extern "C" void EXTI0_IRQHandler(void) //прерывание от кнопки по заднему фронту
{
  for(int i=0;i<7200000;i++) {}  //ждем ~100 мс  от дребезга
  button_flag=1;
  EXTI->PR = EXTI_PR_PR0; //Сбрасываем флаг прерывания
}
//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
extern "C" void TIM3_IRQHandler(void) //обработчик прерывания раз в 500 мс
{
	TIM3->SR &=~ TIM_SR_UIF; //скидываем флаг прерывания

	//если наш таймер больше уставки задержки и есть символы то есть gap -перерыв в посылке 
    //и можно ее обрабатывать
	mod_ptr->res_table.regs[0]++;  //через указатель т.к. объект еще не создан
	TIM3_flag=1;
         	
	NVIC_ClearPendingIRQ(TIM3_IRQn);
}
//--------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------
extern "C" void TIM4_IRQHandler(void) //обработчик прерывания раз в 100 мкс
{
	TIM4->SR &=~ TIM_SR_UIF;

	//если наш таймер больше уставки задержки и есть символы то есть gap -перерыв в посылке 
    //и можно ее обрабатывать
    if((ModBus::rxtimer++>ModBus::delay)&(ModBus::rxcnt>1))  // если rxtimer>delay:
    {
		ModBus::rxgap=1;   // то выставляем флаг об окончании пакета
		//LED13::toggle();
		//TIM4->SR |= TIM_SR_UIF;
	}
    else
    ModBus::rxgap=0;     	
	NVIC_ClearPendingIRQ(TIM4_IRQn);
}
//--------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------
extern "C" void USART1_IRQHandler(void) // обработка прерывания от USART1
{	    
//***************************************************************************
// *USART2 interrupt
// обработчик прерываний usart нужен для двух вещей для принятия и отправки символов
// с включением и выключением соотвествующих прерываний
// **************************************************************************
    //Receive Data register not empty interrupt
		
	//LED13::ledOn();
    if(USART1->SR&USART_SR_RXNE)
   	{
       	USART1->SR&=~USART_SR_RXNE; // очистка признака прерывания по приему
       	ModBus::rxtimer=0;		    // обнуляем счетчик таймера
       	if(ModBus::rxcnt>(255-2))  // если переполнен буфер обнуляем счетчик байтов на прием (проверка мусора)
       	{ModBus::rxcnt=0;}
       	ModBus::buffer[ModBus::rxcnt++]=USART1->DR;  // принимаем байт в буффер из DR
		   //if(GPIOD->IDR&GPIO_IDR_IDR_12)
		   //{led12_off;}
		   //else {led12_on;}
   	}
	if(USART1->SR&USART_SR_TC)
   	{		   
	    //USART1->SR&=~USART_SR_TC;//очистка признака прерывания передачи
        if(ModBus::txcnt<ModBus::txlen)
        {								
            USART1_ini::usart1_send(ModBus::buffer[ModBus::txcnt++]);//Передаем, увеличивая счетчик переданных байтов										
        }
        else
        {				
            //посылка закончилась и мы снимаем высокий уровень сRS485 TXE
            ModBus::txlen=0; //обнуляем длину буфера на передачу
            USART1->CR1|=USART_CR1_RXNEIE; //включаем прерывание по приему
			USART1->CR1 &=~USART_CR1_TCIE; // выключаем прерывание по передаче                
            TIM4->DIER|=TIM_DIER_UIE;   // включаем таймер
		}
    }	 
}
//--------------------------------------------------------------------------------------------------------

int main()
{
	RCCini rcc;
	
	LED13 led;
	TIMER4 tim4;
	USART1_ini usart1; //создаем и инициализируем объект управления USART1

	TIMER3 tim3;//запускаем таймер3

	ModBus modbus;     //создаем и инициализируем объект управления по vodbus, адрес устройства=1;
	mod_ptr=&modbus;
	
	//SYStick systick;
	
	ModBus::rxtimer=0;//счетчик таймера - переменная в которой мы считаем таймоут (несколько раз пока меньше delay)
    ModBus::rxcnt=0; //количество принятых символов
    ModBus::delay=420; //таймаут приема (равняется 3,5 символа для скорости 9600 бод)//задержка по которой вычисляется конец посылки
    ModBus::rxgap=0;//окончание приема флаг 1-окончание приема (rxtimer>delay)
    //ModBus::buffer[255];//буфер в который считываются данные с uart и потом записываются в uart
    ModBus::txcnt=0;//количество переданных символов
    ModBus::txlen=0;//длина посылки на отправку   

	//modbus.res_table.regs[0]=15;	
	ADC adc;
	Button but;
	
	__enable_irq();	// принудительно включаем прерывания	
	button_flag=0;
	g_xFloat=10.F;
	float MO=g_xFloat;
	float sko=g_xFloat/10.0; //10%
	volatile uint8_t but_num =0;
	while(1)
	{
		//USART1_ini::usart1_send(0x2);
		
        if(ModBus::rxgap==1) //ждем gap - т.е. промежуток свидетельствующий об окончании пакета
		{		
			//USART1_ini::usart1_send(0x7);
			//LED13::toggle();
        	modbus.MODBUS_SLAVE();//проверяем и если пакет нам по адресу и не битый то формируем ответ
			
        	modbus.net_tx3();//если есть признак готовности посылки инициализируем передачу
        }   
		if(TIM3_flag==1) //500 ms   заполняем регистры значениями
		{
			float randNum = adc.gausRand(MO,sko);			
			volatile void* y = &randNum;			
			volatile uint32_t x = *((uint32_t*)y); //в int набор бит			
			volatile uint16_t test=x&0x00FF; //делим на две части
			volatile uint16_t test1=x>>16;
			//			
			uint16_t h = ((x>>16)&0x8000)|((((x&0x7f800000)-0x38000000)>>13)&0x7c00)|((x>>13)&0x03ff); //half float в виде набора бит			
			modbus.res_table.regs[1]=test1;
			modbus.res_table.regs[2]=test; // два регистра по 16 бит во float 
			modbus.res_table.regs[3]=h;		// 16 битный half float
						
			TIM3_flag=0;
		}
		if (button_flag==1) // прерывание от кнопки
		{
			LED13::toggle();
			switch(but_num)
			{
				case 0: 
					but_num++;
					MO=g_xFloat*5.0;
					sko=g_xFloat*5/20.0; //5% СКО				
				break;

				case 1: 
					but_num++;
					MO=g_xFloat*5.0;
					sko=g_xFloat*5/10.0; //10%				
				break;

				case 2: 
					but_num++;
					MO=g_xFloat*10;
					sko=g_xFloat*10/20.0; //5%				
				break;

				case 3: 
					but_num++;
					MO=g_xFloat*10;
					sko=g_xFloat*10/10.0; //10%				
				break;

				case 4: 
					but_num++;
					MO=g_xFloat*50;
					sko=g_xFloat*50/20.0; //5%				
				break;

				case 5: 
					but_num++;
					MO=g_xFloat*50;
					sko=g_xFloat*50/10.0; //10%				
				break;

				case 6:
					but_num++;
					MO=g_xFloat;
					sko=g_xFloat/40.0; //2.5%
				break;

				case 7:				
					but_num=0;
					MO=g_xFloat;
					sko=g_xFloat/10.0; //10%				
				break;

				default:break;
			}			
			button_flag=0;
		}

		
		/*
		led.ledOn();
		systick.delay_ms(1000);
		led.ledOff();
		systick.delay_ms(1000);		
		//usart1_send(c);
		USART1_ini::usart1_sendSTR(str);
		//delay(0xfffff);
		*/
	//LED13::ledOn();
	
	}
    return 0;
}
