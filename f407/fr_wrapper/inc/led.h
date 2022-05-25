#ifndef LED_H_
#define LED_H_

//#include "main.h"
//#include "irq.h"

class PD_led:public Interruptable
{
public:
    PD_led()
    {
		pThis=this;
        RCC_init();
        portD_led_ini();
    }
    void led12_on(){GPIOD->BSRRL=GPIO_BSRR_BS_12;}
    void led13_on(){GPIOD->BSRRL=GPIO_BSRR_BS_13;}
    void led14_on(){GPIOD->BSRRL=GPIO_BSRR_BS_14;}
    void led15_on(){GPIOD->BSRRL=GPIO_BSRR_BS_15;}
    void led12_off(){GPIOD->BSRRH=GPIO_BSRR_BS_12;}
    void led13_off(){GPIOD->BSRRH=GPIO_BSRR_BS_13;}
    void led14_off(){GPIOD->BSRRH=GPIO_BSRR_BS_14;}
    void led15_off(){GPIOD->BSRRH=GPIO_BSRR_BS_15;}
	void led12_toggle(){GPIOD->ODR^=GPIO_ODR_ODR_12;}
	void led13_toggle(){GPIOD->ODR^=GPIO_ODR_ODR_13;}
	void led14_toggle(){GPIOD->ODR^=GPIO_ODR_ODR_14;}
	void led15_toggle(){GPIOD->ODR^=GPIO_ODR_ODR_15;}
	void IrqHandler(){pThis->led12_toggle();}
private:
	static PD_led* pThis;
    void portD_led_ini()
    {
        RCC->AHB1ENR|=RCC_AHB1ENR_GPIODEN;
	    GPIOD->MODER|=GPIO_MODER_MODER15_0|GPIO_MODER_MODER14_0|GPIO_MODER_MODER13_0|GPIO_MODER_MODER12_0;
	    GPIOD->MODER&=~(GPIO_MODER_MODER15_1|GPIO_MODER_MODER14_1|GPIO_MODER_MODER13_1|GPIO_MODER_MODER12_1);
	    GPIOD->OTYPER=0;
	    GPIOD->OSPEEDR|=0;
	    GPIOD->ODR=0;
    }
    void RCC_init()
    {
    	#define PLL_M 4
    	#define PLL_N 168
    	#define PLL_Q 7
    	#define PLL_P 2
    	RCC->CFGR&=~RCC_CFGR_SW;//SW=0 внутренний источник тактирования;
    	RCC->CR&=~RCC_CR_PLLON;// выключаем PLL чтобы можно было бы записывать в PLLCFGR
    	RCC->CFGR=0;
    	RCC->PLLCFGR=0;//обнуляем регистр настройки частоты
    	RCC->CR|=RCC_CR_HSION;
    	RCC->CR&=~RCC_CR_HSION;
    	RCC->CR|=RCC_CR_HSEON; // включение внешнего кварца
    	while(!(RCC->CR&RCC_CR_HSERDY)){}
    	RCC->CFGR|=RCC_CFGR_HPRE_DIV1|RCC_CFGR_PPRE1_DIV4|RCC_CFGR_PPRE2_DIV2; 
    	/////*!< SYSCLK not divided */  
    	/*!< HCLK divided by 4 (APB1)*/ 
    	/*!< HCLK divided by 2 (APB2)*/ //(42 84MHz)	
    	RCC->PLLCFGR|=PLL_M|(PLL_N<<6)|(((PLL_P>>1)-1)<<16)|RCC_PLLCFGR_PLLSRC_HSE|(PLL_Q<<24);
    	RCC->CR|=RCC_CR_PLLON;
    	while(!(RCC->CR&RCC_CR_PLLRDY)){}
    	//Настроить Flash prefetch, instruction cache, data cache и wait state
    	FLASH->ACR|=FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS | FLASH_ACR_PRFTEN;
      	//выбрать PLL источником тактирования и дождаться его готовности
    	//RCC->CFGR&=~RCC_CFGR_SW;//SW=00;
    	//RCC->CFGR|=RCC_CFGR_SW_HSE;//SW=01;
    	RCC->CFGR|=RCC_CFGR_SW_PLL;
    	while(!(RCC->CFGR&RCC_CFGR_SWS_PLL)){} //дождемся выставления бита готовности PLL
    }
};
PD_led* PD_led::pThis=nullptr;

#endif //LED_H_