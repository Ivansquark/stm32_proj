#include "flash.h"

Flash_Data flash_data = {0};

inline void flash_unlock() {
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;
}

inline void flash_lock() {
    while(FLASH->SR & FLASH_SR_BSY1); // wait until not busy
    FLASH->CR |= FLASH_CR_LOCK;
}

void flash_erase_page() {
    flash_unlock();
    while(FLASH->SR & FLASH_SR_BSY1); // wait until not busy
    FLASH->CR |= FLASH_CR_PER; // page erase enable
    FLASH->CR |= (FLASH_PAGE_NUMBER<<3);  //page number
    FLASH->CR |= FLASH_CR_STRT;
    while(FLASH->SR & FLASH_SR_BSY1); // wait untill not busy
    FLASH->CR &=~ FLASH_CR_PER; // page erase enable
}

void flash_read_bytes(uint8_t* arr, uint8_t bytes_num) {
    for(int i=0; i<bytes_num; i++) {
        arr[i] = *((uint8_t*)FLASH_PAGE + i);
    }
}
void flash_write_bytes(uint8_t* arr, uint8_t bytes_num) {
    flash_erase_page();
    //! programming in double word => bytes_num must be divided on 8 without residual
    while(FLASH->SR & FLASH_SR_BSY1); // wait until not busy
    FLASH->CR |= FLASH_CR_PG; // programming enable
    FLASH->CR |= (FLASH_PAGE_NUMBER<<3);  //page number
    FLASH->CR &=~ FLASH_CR_PER;
    for(uint8_t i=0 ; i<bytes_num; i+=8) {
        *((uint32_t*)(FLASH_PAGE+i)) = (uint32_t)(*((uint8_t*)(arr+3+i))<<24) + (*((uint8_t*)(arr+2+i))<<16) +
                                       (*((uint8_t*)(arr+1+i))<<8) + (*((uint8_t*)(arr+i)));           
        *((uint32_t*)(FLASH_PAGE+4U+i)) = (uint32_t)(*((uint8_t*)(arr+7+i))<<24) + (*((uint8_t*)(arr+6+i))<<16) +
                                          (*((uint8_t*)(arr+5+i))<<8) + (*((uint8_t*)(arr+4+i))); 
    } 
    while(FLASH->SR & FLASH_SR_BSY1); // wait until not busy
    //while(!(FLASH->SR & FLASH_SR_EOP)); // wait for operation succeeded (only if interrupt flag EOPIE=1)
    FLASH->SR |= FLASH_SR_EOP; // clear flag
    FLASH->CR &=~ FLASH_CR_PG; // clear flag
    flash_lock();
}

void flash_read_data() {
    flash_read_bytes((uint8_t*)&flash_data,16);
}

void flash_write_data(uint8_t* arr) {
    flash_write_bytes(arr, 16);
}