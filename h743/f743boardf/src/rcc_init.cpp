#include "rcc_init.h"

RCC_INIT::RCC_INIT(uint16_t clock) {
    rcc_init(clock);
};

void RCC_INIT::rcc_init(uint16_t clock) {
    //FLASH->ACR = 0;
	//FLASH->ACR |= FLASH_ACR_LATENCY_4WS;
	PWR->CR3 &= ~ (PWR_CR3_LDOEN);
	PWR->CR3 &=~ ( PWR_CR3_SCUEN | PWR_CR3_BYPASS);
	PWR->D3CR |= PWR_D3CR_VOS;
	while((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY);

    RCC->CR |= RCC_CR_HSEON; // Enable HSE		
		while((RCC->CR & RCC_CR_HSERDY) == 0); // Wait till HSE is ready
        RCC->CFGR = 0;
        RCC -> PLLCKSELR = 0; //  PLL1 — внешний кварц.
		RCC -> PLLCKSELR = RCC_PLLCKSELR_PLLSRC_HSE; //  PLL1 — внешний кварц.
		switch (clock)
		{			
			case 64: 
			{
				//PLLM = 4
				RCC -> PLLCKSELR |= (2<<4); // = 2
				//PLL1DIVR     bits
				//DIVN1[8:0]  0  - 8   PLLN = 64     
				//DIVP1[6:0]  9  - 15  PLLP = 2 => 1 in register
				//DIVQ1[6:0]  16 - 22  PLLQ = 2	=> 1 in register
				//DIVR1[6:0]  24 - 30  PLLR = 2	=> 1 in register
				RCC -> PLL1DIVR = 0;
				RCC -> PLL2DIVR = 0;
				RCC -> PLL3DIVR = 0;
				RCC -> PLL1DIVR  |= (1<<24)|(1<<16)|(1<<9)|(64<<0);	
                RCC->PLLCFGR = 0;
				RCC->PLLCFGR &=~ RCC_PLLCFGR_PLL1RGE; //Указание диапазона входной частоты PLL1 /* Select PLL1 input reference frequency range: VCI */
				RCC->PLLCFGR |= RCC_PLLCFGR_PLL1RGE_1;
				RCC->PLLCFGR |= RCC_PLLCFGR_PLL1VCOSEL; // Указание диапазона выходной частоты PLL1 /* Select PLL1 output frequency range : VCO */	
				RCC -> D1CFGR |= (RCC_D1CFGR_D1CPRE_3); //Divide on 2
				RCC -> D1CFGR &=~ RCC_D1CFGR_HPRE; //Divide on 1				
				RCC -> D1CFGR &=~  RCC_D1CFGR_D1PPRE_2;
				RCC -> D1CFGR &=~ (RCC_D1CFGR_D1PPRE_1 | RCC_D1CFGR_D1PPRE_0); // 1:0:0 Делитель на 2 D1PPRE1 : rcc_pclk1 = rcc_hclk1 / 2
				RCC -> D2CFGR &=~  RCC_D2CFGR_D2PPRE1_2;
				RCC -> D2CFGR &=~ (RCC_D2CFGR_D2PPRE1_1 | RCC_D2CFGR_D2PPRE1_0); // 1:0:0 Делитель на 2 D2PPRE1 : rcc_pclk1 = rcc_hclk1 / 2
				RCC -> D2CFGR &=~  RCC_D2CFGR_D2PPRE2_2;
				RCC -> D2CFGR &=~ (RCC_D2CFGR_D2PPRE2_1 | RCC_D2CFGR_D2PPRE2_0); // 1:0:0 Делитель на 2 D2PPRE2 : rcc_pclk1 = rcc_hclk1 / 2
				RCC -> D3CFGR &=~  RCC_D3CFGR_D3PPRE_2;
				RCC -> D3CFGR &=~ (RCC_D3CFGR_D3PPRE_1 | RCC_D3CFGR_D3PPRE_0); // 1:0:0 Делитель на 2 D2PPRE3 : rcc_pclk1 = rcc_hclk1 / 2	
				//FLASH->ACR &=~ FLASH_ACR_LATENCY;
				//FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
			}	break;				
			case 400:
			{
				//PLLM = 4
				RCC -> PLLCKSELR |= (4<<4); // = 4 
				//PLL1DIVR     bits
				//DIVN1[8:0]  0  - 8   PLLN = 400     
				//DIVP1[6:0]  9  - 15  PLLP = 2  => 1 in register
				//DIVQ1[6:0]  16 - 22  PLLQ = 2  => 1 in register
				//DIVR1[6:0]  24 - 30  PLLR = 2  => 1 in register
				RCC -> PLL1DIVR = 0;
				RCC -> PLL2DIVR = 0;
				RCC -> PLL3DIVR = 0;
				RCC -> PLL1DIVR  |= (1<<24)|(1<<16)|(1<<9)|(359<<0);	
                RCC->PLLCFGR = 0;
				RCC->PLLCFGR |= RCC_PLLCFGR_PLL1RGE_2; //Указание диапазона входной частоты PLL1 /* Select PLL1 input reference frequency range: VCI */
				RCC->PLLCFGR &=~ RCC_PLLCFGR_PLL1VCOSEL; // Указание диапазона выходной частоты PLL1 /* Select PLL1 output frequency range : VCO */	
				RCC -> D1CFGR = 0;
				RCC -> D1CFGR &=~ (RCC_D1CFGR_D1CPRE); //Divide on 1
				RCC -> D1CFGR |= RCC_D1CFGR_HPRE_3; //Divide on 2				
				RCC -> D1CFGR |=  RCC_D1CFGR_D1PPRE_2;
				RCC -> D1CFGR &=~ (RCC_D1CFGR_D1PPRE_1 | RCC_D1CFGR_D1PPRE_0); // 1:0:0 Делитель на 2 D1PPRE1 : rcc_pclk1 = rcc_hclk1 / 2
				RCC -> D2CFGR |=  RCC_D2CFGR_D2PPRE1_2;
				RCC -> D2CFGR &=~ (RCC_D2CFGR_D2PPRE1_1 | RCC_D2CFGR_D2PPRE1_0); // 1:0:0 Делитель на 2 D2PPRE1 : rcc_pclk1 = rcc_hclk1 / 2
				RCC -> D2CFGR |=  RCC_D2CFGR_D2PPRE2_2;
				RCC -> D2CFGR &=~ (RCC_D2CFGR_D2PPRE2_1 | RCC_D2CFGR_D2PPRE2_0); // 1:0:0 Делитель на 2 D2PPRE2 : rcc_pclk1 = rcc_hclk1 / 2
				RCC -> D3CFGR |=  RCC_D3CFGR_D3PPRE_2;
				RCC -> D3CFGR &=~ (RCC_D3CFGR_D3PPRE_1 | RCC_D3CFGR_D3PPRE_0); // 1:0:0 Делитель на 2 D2PPRE3 : rcc_pclk1 = rcc_hclk1 / 2	
				FLASH->ACR &=~ FLASH_ACR_LATENCY;
				FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
			}	break;
			case 480:
			{
				//PLLM = 4
				RCC -> PLLCKSELR |= (8<<4); // = 8 
				//PLL1DIVR     bits
				//DIVN1[8:0]  0  - 8   PLLN = 500     
				//DIVP1[6:0]  9  - 15  PLLP = 2  => 1 in register
				//DIVQ1[6:0]  16 - 22  PLLQ = 2  => 1 in register
				//DIVR1[6:0]  24 - 30  PLLR = 2  => 1 in register
				RCC -> PLL1DIVR = 0;
				RCC -> PLL2DIVR = 0;
				RCC -> PLL3DIVR = 0;
				RCC -> PLL1DIVR  |= (1<<24)|(1<<16)|(1<<9)|(400<<0);	
                RCC->PLLCFGR = 0;
				RCC->PLLCFGR |= RCC_PLLCFGR_PLL1RGE_3; //Указание диапазона входной частоты PLL1 /* Select PLL1 input reference frequency range: VCI */
				RCC->PLLCFGR &=~ RCC_PLLCFGR_PLL1VCOSEL; // Указание диапазона выходной частоты PLL1 /* Select PLL1 output frequency range : VCO */	
				RCC -> D1CFGR = 0;
				RCC -> D1CFGR &=~ (RCC_D1CFGR_D1CPRE); //Divide on 1
				RCC -> D1CFGR |= RCC_D1CFGR_HPRE_3; //Divide on 2				
				RCC -> D1CFGR |=  RCC_D1CFGR_D1PPRE_2;
				RCC -> D1CFGR &=~ (RCC_D1CFGR_D1PPRE_1 | RCC_D1CFGR_D1PPRE_0); // 1:0:0 Делитель на 2 D1PPRE1 : rcc_pclk1 = rcc_hclk1 / 2
				RCC -> D2CFGR |=  RCC_D2CFGR_D2PPRE1_2;
				RCC -> D2CFGR &=~ (RCC_D2CFGR_D2PPRE1_1 | RCC_D2CFGR_D2PPRE1_0); // 1:0:0 Делитель на 2 D2PPRE1 : rcc_pclk1 = rcc_hclk1 / 2
				RCC -> D2CFGR |=  RCC_D2CFGR_D2PPRE2_2;
				RCC -> D2CFGR &=~ (RCC_D2CFGR_D2PPRE2_1 | RCC_D2CFGR_D2PPRE2_0); // 1:0:0 Делитель на 2 D2PPRE2 : rcc_pclk1 = rcc_hclk1 / 2
				RCC -> D3CFGR |=  RCC_D3CFGR_D3PPRE_2;
				RCC -> D3CFGR &=~ (RCC_D3CFGR_D3PPRE_1 | RCC_D3CFGR_D3PPRE_0); // 1:0:0 Делитель на 2 D2PPRE3 : rcc_pclk1 = rcc_hclk1 / 2	
				FLASH->ACR &=~ FLASH_ACR_LATENCY;
				FLASH->ACR |= FLASH_ACR_LATENCY_3WS;
				RCC -> PLLCKSELR |= RCC_PLLCKSELR_PLLSRC_HSE; //  PLL1 — внешний кварц.
				//return;
			}	break;            
			default: /* Enable HSI*/ RCC->CR &=~ RCC_CR_HSEON; RCC->CR |= RCC_CR_HSION; return; break;
		}
		RCC -> PLL1FRACR = 0x1A00;//0x1A00; //Дробный делитель частоты PLL (если нужен)
		
		RCC->PLLCFGR |= RCC_PLLCFGR_DIVP1EN | RCC_PLLCFGR_DIVQ1EN | RCC_PLLCFGR_DIVR1EN;
		//RCC->PLLCFGR |= RCC_PLLCFGR_PLL1FRACEN; // Включение дробного делителя.					
		RCC->CR |= RCC_CR_PLLON; //Пуск PLL1 и ожидание готовности
		while(!(RCC->CR & RCC_CR_PLL1RDY));
		//PA8 - MCO
		//RCC->AHB4ENR |= RCC_AHB4ENR_GPIOAEN;
		//GPIOA->MODER|=GPIO_MODER_MODE8_1; GPIOA->MODER&=~GPIO_MODER_MODE8_0; //alternative function
		//GPIOA->OSPEEDR|=GPIO_OSPEEDR_OSPEED8;// 1:1 max speed
		//GPIOA->AFR[1]|=GPIO_AFRH_AFRH0; // alternative function 0 is MCO1;
		//RCC->CFGR |= (RCC_CFGR_MCO1_1 | RCC_CFGR_MCO1_0);  ; //0:1:1 clock on MCO1 PA8
		//RCC->CFGR|=RCC_CFGR_MCO1PRE_2; //Divide on 1; 
		RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
		SYSCFG->CCCSR |= SYSCFG_CCCSR_EN; //compensation cell
		//RCC->CR |= RCC_CR_CSION;
		RCC->CFGR &= ~RCC_CFGR_SW; //0
		RCC->CFGR |=  (RCC_CFGR_SW_1 | RCC_CFGR_SW_0); //0:0 Задаем PLL1 как источник SYSCLK и ожидаем готовности
		while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL1);
		RCC->CR &=~ RCC_CR_HSION; // Enable HSE
		// dont forget set FLASH LATENCY!!!!!!!!!!!!!!!!!!!!!!!!!
        //FLASH->ACR = 0;
		//FLASH->ACR |= FLASH_ACR_LATENCY_4WS;		
}