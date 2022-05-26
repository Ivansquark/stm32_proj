#include "flash.h"

Flash* Flash::pThis = nullptr;

Flash::Flash() {
    flash_init();
}


void Flash::flash_init() {
    pThis = this;
    unlock();
    FLASH->CR &=~ FLASH_CR_PSIZE;  // program x8
}

void Flash::erase() {
    lock();
    unlock();
	while(FLASH->SR & FLASH_SR_BSY); // no flash memory operation is ongoing (BSY -busy)
	if (FLASH->SR & FLASH_SR_EOP) 
	{
		FLASH->SR = FLASH_SR_EOP; //END of operation Set by hardware when a Flash operation (programming / erase) is completed  Reset by writing a 1
	}
	FLASH->CR |= FLASH_CR_SER;//sector erase chosen
    FLASH->CR |= FLASH_CR_SNB_3; // sector 8;
    FLASH->CR &=~ (FLASH_CR_SNB_4|FLASH_CR_SNB_2|FLASH_CR_SNB_1|FLASH_CR_SNB_0); //sector number 8    
	
	FLASH->CR |= FLASH_CR_STRT; // This bit triggers an ERASE operation when set.
	while(FLASH->SR & FLASH_SR_BSY); // no flash memory operation is ongoing (BSY -busy)
    //while (!(FLASH->SR & FLASH_SR_EOP)); //while not end of operation
	FLASH->SR = FLASH_SR_EOP;	//reset bit
	FLASH->CR &= ~FLASH_CR_SER; //unchose page erase
}

void Flash::read_buf(void* buf, uint16_t len) {
    lock();
    unlock();
    uint32_t addr = FLASH_LAST;
    for(int i=0; i<len;i++) {
        *((uint8_t*)buf+i) = *((uint8_t*)addr+i);
    }
	//for (int i=0;i<len;i+=2)  // fill array with values thats in flash
	//{
	//	*((uint8_t*)buf+i) =  *(uint8_t*)(addr + i);
	//	*((uint8_t*)buf+i+1) =  *(uint8_t*)(addr + i + 1);
	//}
	lock();
}

void Flash::write_buf(void* buf) {
    uint32_t addr = FLASH_LAST;
    erase(); //erasing last sector		
	while (FLASH->SR & FLASH_SR_BSY); // no flash memory operation is ongoing
	if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR = FLASH_SR_EOP;	
    } //reset bit
	FLASH->CR |= FLASH_CR_PG; //Flash programming chosen
	/*!< half word writing>*/
    /*
	for(int i=0; i<SIZE; i+=2)
	{
		if(i+1 != SIZE)
		{
			*(volatile uint16_t*)(addr+i) = (*(uint16_t*)((uint8_t*)buf + i + 1)<<8) + *((uint8_t*)buf+i);
		}
		else {*(volatile uint16_t*)((uint8_t*)addr+i) = 0x0000 + *((uint8_t*)buf+i);}
		while (!(FLASH->SR & FLASH_SR_EOP)); //while writing page
		FLASH->SR = FLASH_SR_EOP;	
	}
    */
    for(int i=0; i<SIZE;i++) {
        *((uint8_t*)addr+i) = *((uint8_t*)buf+i);
    }
	FLASH->CR &= ~(FLASH_CR_PG);	
	lock();
}