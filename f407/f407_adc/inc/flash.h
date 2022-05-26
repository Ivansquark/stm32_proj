#ifndef FLASH_H
#define FLASH_H

#include "stm32f4xx.h"

class Flash {
//! Last page of Flash (sector 8)  to keep koefficients
    static constexpr uint32_t FLASH_LAST = 0x08080000;  //(f407VE - 512 kBytes) 
    static constexpr uint32_t FLASH_START = 0x08000000;
    static constexpr uint32_t FLASH_PAGE_SIZE = 0x20000;	/*128K*/
    static constexpr uint16_t SIZE = 512; //! how many bytes are writing in flash
public:  
    Flash();
    static Flash* pThis;
    void read_buf(void* buf, uint16_t len);
    void write_buf(void* buf);
    void erase();
        

private:
    void flash_init();
    inline void unlock() {
        /*!<Flash unlock>*/
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
    }
    inline void lock() {
		FLASH->CR |= FLASH_CR_LOCK; /* Lock the flash back */
	}    
};


#endif //FLASH_H