#include "flash.h"

Flash* Flash::pThis = nullptr;

Flash::Flash() {
	pThis = this;
	init();
}

void Flash::writeBootFlags(void* buf, uint32_t len) {
	//!___________ Erase sector 1 0x08004000 (second sector)______________________
	lock();
	unlock();
	while(FLASH->SR & FLASH_SR_BSY); // no flash memory operation is ongoing (BSY -busy)
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP; //END of operation Set by hardware when a Flash operation (programming / erase) is completed  Reset by writing a 1
	}
	FLASH->CR |= FLASH_CR_SER;//sector erase chosen
	FLASH->CR &=~ FLASH_CR_SNB; // clear sector number;
    FLASH->CR |= (FLASH_BOOT_FLAG_SECTOR_NUM<<3); // choose sector number (second sector);
	
	FLASH->CR |= FLASH_CR_STRT; // This bit triggers an ERASE operation when set.
	while (FLASH->SR & FLASH_SR_BSY); //while not end of operation
	FLASH->SR = FLASH_SR_EOP;	//reset bit
	FLASH->CR &= ~FLASH_CR_SER; //unchose page erase
	//!_____________ Programm sector 1 ___________________________
	while (FLASH->SR & FLASH_SR_BSY); // no flash memory operation is ongoing
	if (FLASH->SR & FLASH_SR_EOP) 
	{		FLASH->SR = FLASH_SR_EOP;	} //reset bit
	FLASH->CR &=~ FLASH_CR_SNB; // clear sector number;
    FLASH->CR |= (FLASH_BOOT_FLAG_SECTOR_NUM<<3); // choose sector number (second sector);
	FLASH->CR |= FLASH_CR_PG; //Flash programming chosen
	/*!<1 byte writing>*/
	for(int i=0; i<len;i++) {
        *((uint8_t*)FLASH_BOOT_FLAGS_ADDR+i) = *((uint8_t*)buf+i);
    }
	FLASH->CR &= ~(FLASH_CR_PG); // finish programming	
	lock();	
}

void Flash::readBootFlags(void* buf, uint32_t len) {
	uint32_t addr = FLASH_BOOT_FLAGS_ADDR;
    for(int i=0; i<len;i++) {
        *((uint8_t*)buf+i) = *((uint8_t*)addr+i);
    }
}

void Flash::init() {
	lock();
	unlock();
	FLASH->CR &=~ FLASH_CR_PSIZE;  // program by one byte 
	lock();
} 

void Flash::lock() {
	FLASH->CR |= FLASH_CR_LOCK;
}
void Flash::unlock() {
	//! -- Flash unlock --
	FLASH->KEYR = 0x45670123;
	FLASH->KEYR = 0xCDEF89AB;
}