extern "C" void Reset_Handler();
extern "C" void Default_Handler();
extern "C" void NMI_Handler(); 
//если будет объявлена функция с таким же именем то именно она и будет обработчиком этого прерывания.
extern "C" void HardFault_Handler();
extern "C" void SVC_Handler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void PendSV_Handler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void SysTick_Handler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void WWDG_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void PVD_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void RTC_TAMP_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void FLASH_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void RCC_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void EXTI0_1_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void EXTI2_3_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void EXTI4_15_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void DMA1_Channel1_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void DMA1_Channel2_3_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void DMA1_Ch4_5_DMAMUX1_OVR_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void ADC1_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void TIM1_BRK_UP_TRG_COM_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void TIM1_CC_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void TIM2_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void TIM3_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void LPTIM1_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void LPTIM2_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void TIM14_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void TIM16_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void TIM17_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void I2C1_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void I2C2_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void SPI1_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void SPI2_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void USART1_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void USART2_IRQHandler()   __attribute__((weak, alias("Default_Handler")));
extern "C" void LPUART1_IRQHandler()   __attribute__((weak, alias("Default_Handler")));

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
  PVD_IRQHandler,                    /* PVD through EXTI Line detection */                        
  RTC_TAMP_IRQHandler,             	 /* Tamper and TimeStamps through the EXTI line */            
  FLASH_IRQHandler,               	 /* FLASH_IRQHandler				 */                      
  RCC_IRQHandler,                  	 /* RCC             		         */                                          
  EXTI0_1_IRQHandler,                /* EXTI Line 0 and 1            */                                           
  EXTI2_3_IRQHandler,                /* EXTI Line 2 and 3            */                       
  EXTI4_15_IRQHandler,               /* EXTI Line 4 to 15            */                          
  0,           					       /* reserved                   */                          
  DMA1_Channel1_IRQHandler,          /* DMA1 Channel 1               */                          
  DMA1_Channel2_3_IRQHandler,        /* DMA1 Channel 2 and Channel 3 */                          
  DMA1_Ch4_5_DMAMUX1_OVR_IRQHandler, /* DMA1 Channel 4 to Channel 5, DMAMUX1 overrun */                  
  ADC1_IRQHandler,                   /* ADC1                        */                   
  TIM1_BRK_UP_TRG_COM_IRQHandler,    /* TIM1 Break, Update, Trigger and Commutation */                   
  TIM1_CC_IRQHandler,                /* TIM1 Capture Compare         */                  
  TIM2_IRQHandler,                   /* TIM2                         */                   
  TIM3_IRQHandler,                   /* TIM3                         */                   
  LPTIM1_IRQHandler,                 /* LPTIM1                       */                   
  LPTIM2_IRQHandler,                 /* LPTIM2                       */                   
  TIM14_IRQHandler,                  /* TIM14                        */                         
  0,              					 /* reserved					 */                          
  TIM16_IRQHandler,                  /* TIM16                        */                          
  TIM17_IRQHandler,                  /* TIM17                        */                          
  I2C1_IRQHandler,                   /* I2C1                         */                          
  I2C2_IRQHandler,                   /* I2C2                         */         
  SPI1_IRQHandler,                   /* SPI1                         */         
  SPI2_IRQHandler,                   /* SPI2                         */
  USART1_IRQHandler,                 /* USART1                       */                          
  USART2_IRQHandler,                 /* USART2                       */                  
  LPUART1_IRQHandler,                /* LPUART1                      */                   
  0					                 /* reserved                     */          
};

extern void (*__preinit_array_start []) (void) __attribute__((weak)); //from linker
extern void (*__preinit_array_end []) (void) __attribute__((weak));	//from linker
extern void (*__init_array_start []) (void) __attribute__((weak)); //from linker constructors
extern void (*__init_array_end []) (void) __attribute__((weak)); //from linker	constructors
extern void (*__fini_array_start []) (void) __attribute__((weak)); //from linker destructors
extern void (*__fini_array_end []) (void) __attribute__((weak)); //from linker	destructors
void __attribute__ ((weak)) _init(void)  {} // dummy This section holds executable instructions that contribute to the process initialization code. When a program starts to run, the system arranges to execute the code in this section before calling the main program entry point (called main for C programs). 
void __attribute__ ((weak)) _fini(void)  {} // dummy заглушка This section holds executable instructions that contribute to the process termination code. That is, when a program exits normally, the system arranges to execute the code in this section

/* Iterate over all the init routines.  */
void
__libc_init_array (void) // static initialization constructors function
{
  int count;  int i;
  count = __preinit_array_end - __preinit_array_start; //counts of preinit functions DK what it is
  for (i = 0; i < count; i++)
    __preinit_array_start[i] ();
  _init ();
  count = __init_array_end - __init_array_start; // counts of init constructors
  for (i = 0; i < count; i++)
    __init_array_start[i] ();
}
/* Run all the cleanup routines.  */
void
__libc_fini_array (void) //!< destructors not usefull in microcontrollers
{
  int count;
  int i;  
  count = __fini_array_end - __fini_array_start;
  for (i = count; i > 0; i--)
    __fini_array_start[i-1] ();
  _fini ();
}

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

extern "C" void NMI_Handler(void) {
  while(1);
} 
extern "C" void HardFault_Handler() {  
  while(1){
    for(int i=0; i<4600000;i++){}
    //GPIOB->ODR ^= GPIO_ODR_OD9;
  }
}