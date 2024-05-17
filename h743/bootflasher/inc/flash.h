#ifndef FLASH_H
#define FLASH_H

#include "main.h"
#include <string.h>

//	A main memory for STM32H743VG:
	//  Bank 1
    // Sector0 0x08000000 - 0x0801FFFF 128 KiB  --- BOOTLOADER ---
	// Sector1 0x08020000 - 0x08037FFF 128 KiB  -- Main program --   
	// Sector2 0x08040000 - 0x08057FFF 128 KiB  -- Main program --
	// Sector3 0x08060000 - 0x0807FFFF 128 KiB  -- Main program --

    //  Bank 2
	// Sector0 0x08100000 - 0x0811FFFF 128 KiB  --   BOOT flag  --
	// Sector1 0x08120000 - 0x0813FFFF 128 KiB  -- Main program --
	// Sector2 0x08140000 - 0x0815FFFF 128 KiB  -- Main program --
    // Sector3 0x08160000 - 0x0817FFFF 128 KiB  

class Flash {
public:
    static constexpr uint32_t FLASH_START = 0x08000000; 
	static constexpr uint32_t FLASH_BOOT_FLAGS_ADDR = 0x08100000;
	static constexpr uint8_t  FLASH_BOOT_FLAG_SECTOR_NUM = 0;
    static constexpr uint32_t FLASH_BANK1_FIRMWARE = 0x08020000;
    static constexpr uint32_t FLASH_BANK2_FIRMWARE = 0x08120000;
    static constexpr uint32_t SECTOR_SIZE = 0x20000;
    const uint8_t BOOT_FLAG = 0x01;


    Flash();
    static Flash* pThis;
	void writeBootFlags(void* buf, uint32_t len);
	void writeBootFlagAndSectorNum(uint8_t sectors);
    void clearBootFlag();
    void readBootFlag();
    

    HAL_StatusTypeDef WriteSectorsFromBank2ToBank1(uint8_t sectors);

    HAL_StatusTypeDef writeBufBank1(uint8_t sector, void* buf, uint32_t len);
    HAL_StatusTypeDef writeBufBank2(uint8_t sector, void* buf, uint32_t len);

    void readFromBank2ToRAM(uint8_t sectors);
    HAL_StatusTypeDef writeFromRAMtoBank1(uint8_t sectors);
    HAL_StatusTypeDef writeFromRAMtoBank2(uint8_t sectors);

    HAL_StatusTypeDef flashNewFirmware();

    bool isNeedGoToFlashNewFirmware = false;
    uint8_t RamBuffer[0x5FFFF] = {0};
    uint8_t sectors = 0;
    uint8_t bootFlag = 0xFF;

private:
    void init();

    void eraseBank1Sector(uint8_t sector);
    void eraseBank2Sector(uint8_t sector);

    
    inline void clearFlagsBank1() {
        FLASH->CCR1 |= (FLASH_CCR_CLR_WRPERR | FLASH_CCR_CLR_OPERR      | FLASH_CCR_CLR_STRBERR     |
                        FLASH_CCR_CLR_RDPERR | FLASH_CCR_CLR_PGSERR     | FLASH_CCR_CLR_DBECCERR    |
                        FLASH_CCR_CLR_RDSERR | FLASH_CCR_CLR_SNECCERR   | FLASH_CCR_CLR_INCERR);
    }
    inline void clearFlagsBank2() {
        FLASH->CCR2 |= (FLASH_CCR_CLR_WRPERR | FLASH_CCR_CLR_OPERR      | FLASH_CCR_CLR_STRBERR     |
                        FLASH_CCR_CLR_RDPERR | FLASH_CCR_CLR_PGSERR     | FLASH_CCR_CLR_DBECCERR    |
                        FLASH_CCR_CLR_RDSERR | FLASH_CCR_CLR_SNECCERR   | FLASH_CCR_CLR_INCERR);
    }

    inline void unlock_bank1() {
        //! -- Flash unlock --
	    FLASH->KEYR1 = 0x45670123;
        FLASH->KEYR1 = 0xCDEF89AB;        
    }
    inline void unlock_bank2() {
        //! -- Flash unlock --
	    FLASH->KEYR2 = 0x45670123;
        FLASH->KEYR2 = 0xCDEF89AB;
    }
	void lock() {
        FLASH->CR1 |= FLASH_CR_LOCK;
        FLASH->CR2 |= FLASH_CR_LOCK;
    }
};




#endif //FLASH_H