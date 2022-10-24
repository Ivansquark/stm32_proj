#ifndef RCC_INIT_H_
#define RCC_INIT_H_

//#include "main.h"
#include "stm32h743xx.h"

class RCC_INIT
{
public:
	RCC_INIT(uint16_t clock=8);	
private:
	void rcc_init(uint16_t clock);	
};

#endif //RCC_INIT_H_
