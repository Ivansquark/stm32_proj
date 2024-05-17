void Reset_Handler();
void Default_Handler();
void NMI_Handler(); 
//если будет объявлена функция с таким же именем то именно она и будет обработчиком этого прерывания.
void HardFault_Handler();
void SVC_Handler()   __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler()   __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler()   __attribute__((weak, alias("Default_Handler")));
void WWDG_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void PVD_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void RTC_TAMP_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void EXTI0_1_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void EXTI2_3_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void EXTI4_15_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel2_3_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void ADC1_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_UP_TRG_COM_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void TIM6_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void TIM7_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void TIM14_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void TIM15_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void TIM16_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void TIM17_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void I2C1_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void I2C2_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
void USART3_4_IRQHandler()   __attribute__((weak, alias("Default_Handler")));

int main();
extern void* _estack; 
void (*vectors[])() __attribute__((section(".isr_vectors"))) = 
{  
  (void(*)())(&_estack), 	//_estack,  //адрес в адрес указателя на ф-ю (нужно взять адрес у адреса 8))
  Reset_Handler,
  NMI_Handler,
  HardFault_Handler,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  SVC_Handler,
  0,
  0,
  PendSV_Handler,
  SysTick_Handler,  
  /* External Interrupts */
  WWDG_IRQHandler,                   /* Window WatchDog              */                                        
  0,              
  RTC_TAMP_IRQHandler,             	 /* Tamper and TimeStamps through the EXTI line */            
  FLASH_IRQHandler,               	 /* FLASH_IRQHandler				 */                      
  RCC_IRQHandler,                  	 /* RCC             		         */                                          
  EXTI0_1_IRQHandler,                /* EXTI Line 0 and 1            */                                           
  EXTI2_3_IRQHandler,                /* EXTI Line 2 and 3            */                       
  EXTI4_15_IRQHandler,               /* EXTI Line 4 to 15            */                          
  0,           					             /* reserved                   */                          
  DMA1_Channel1_IRQHandler,          /* DMA1 Channel 1               */                          
  DMA1_Channel2_3_IRQHandler,        /* DMA1 Channel 2 and Channel 3 */                          
  DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler, /* DMA1 Channel 4 to Channel 5, DMAMUX1 overrun */                  
  ADC1_IRQHandler,                   /* ADC1                        */                   
  TIM1_BRK_UP_TRG_COM_IRQHandler,    /* TIM1 Break, Update, Trigger and Commutation */                   
  TIM1_CC_IRQHandler,                /* TIM1 Capture Compare         */                  
  0,                  
  TIM3_IRQHandler,                   /* TIM3                         */                   
  TIM6_IRQHandler,                 /* LPTIM1                       */                   
  TIM7_IRQHandler,                 /* LPTIM2                       */                   
  TIM14_IRQHandler,                  /* TIM14                        */                         
  TIM15_IRQHandler,              		 /* TIM15_IRQHandler					 */                          
  TIM16_IRQHandler,                  /* TIM16                        */                          
  TIM17_IRQHandler,                  /* TIM17                        */                          
  I2C1_IRQHandler,                   /* I2C1                         */                          
  I2C2_IRQHandler,                   /* I2C2                         */         
  SPI1_IRQHandler,                   /* SPI1                         */         
  SPI2_IRQHandler,                   /* SPI2                         */
  USART1_IRQHandler,                 /* USART1                       */                          
  USART2_IRQHandler,                 /* USART2                       */                  
  USART3_4_IRQHandler                /* LPUART1                      */                   
};

extern void *_sidata, *_sdata, *_edata, *_sbss, *_ebss; //из линкер скрипта
void __attribute__((naked,noreturn))Reset_Handler()  //В такие функции обычно пихают ассемблер - naked
{
	void **pSource, **pDest;
	for (pSource = &_sidata, pDest = &_sdata; pDest != &_edata; pSource++, pDest++) //заполняем data из ROM в RAM
	{*pDest = *pSource;}

	for (pDest = &_sbss; pDest != &_ebss; pDest++)   //заполняем нулями
	{*pDest = 0;}
  __asm volatile ("cpsid i" : : : "memory"); //откл прерывания
	main();
	while(1);
}
void __attribute__(()) Default_Handler() {
    while(1);
}

void NMI_Handler(void) {
  while(1);
} 
void HardFault_Handler() {  
  while(1){
    for(int i=0; i<4600000;i++){}
    //GPIOB->ODR ^= GPIO_ODR_OD9;
  }
}