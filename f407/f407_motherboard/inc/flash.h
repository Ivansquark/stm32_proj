#ifndef FLASH_H
#define FLASH_H

#include "stm32f4xx.h"

	//	A main memory block divided into 4 sectors of 16 Kbytes, next 1 sector of 64 Kbytes, and next 4-7 sectors of 128 Kbytes:
	// Sector 0 0x08000000 - 0x08003FFF 16 Kbytes
	// Sector 1 0x08004000 - 0x08007FFF 16 Kbytes
	// Sector 2 0x08008000 - 0x0800BFFF 16 Kbyte
	// Sector 3 0x0800C000 - 0x0800FFFF 16 Kbytes
	// Sector 4 0x08010000 - 0x0801FFFF 64 Kbytes
	// Sector 5 0x08020000 - 0x0803FFFF 128 Kbytes
	// Sector 6 0x08040000 - 0x0805FFFF 128 Kbytes ...	
class Flash {
public:	
	Flash();
	static Flash* pThis;
	void writeBootFlags(void* buf, uint32_t len);
	void readBootFlags(void* buf, uint32_t len);	
	
private:
	void init();
	void unlock();
	void lock();

	static constexpr uint32_t FLASH_START = 0x08000000; // f407VE - 512 kBytes
	static constexpr uint32_t FLASH_BOOT_FLAGS_ADDR = 0x08004000; // f407VE - 512 kBytes
	static constexpr uint8_t FLASH_BOOT_FLAG_SECTOR_NUM = 1;
	
};	

#endif //FLASH_H