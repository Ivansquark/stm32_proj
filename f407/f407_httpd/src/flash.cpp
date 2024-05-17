#include "flash.h"

Flash *Flash::pThis = nullptr;

Flash::Flash() {
    pThis = this;
    bootFlag = 0xFF;
    memset(RamBuff, 0, sizeof(RamBuff));
    lock();
    unlock();
    FLASH->CR &= ~FLASH_CR_PSIZE; // program by one byte
    lock();
    readBootFlag();
}

void Flash::clearRamBuffer(void) { memset(RamBuff, 0xFF, SECTOR_SIZE); }

void Flash::writeBootFlag() {
    //!___________ Erase sector 1 0x08004000 (second sector)______________________
    lock();
    unlock();
    while (FLASH->SR & FLASH_SR_BSY) {
    } // no flash memory operation is ongoing (BSY -busy)
    if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR = FLASH_SR_EOP; // END of operation Set by hardware when a Flash operation (programming / erase) is
                                  // completed  Reset by writing a 1
    }
    FLASH->CR |= FLASH_CR_SER;                      // sector erase chosen
    FLASH->CR &= ~FLASH_CR_SNB;                     // clear sector number;
    FLASH->CR |= (FLASH_BOOT_FLAG_SECTOR_NUM << 3); // choose sector number (second sector);
    FLASH->CR |= FLASH_CR_STRT;                     // This bit triggers an ERASE operation when set.
    while (FLASH->SR & FLASH_SR_BSY) {
    }                           // while not end of operation
    FLASH->SR = FLASH_SR_EOP;   // reset bit
    FLASH->CR &= ~FLASH_CR_SER; // unchose page erase
    //!_____________ Programm sector 1 ___________________________
    while (FLASH->SR & FLASH_SR_BSY) {
    } // no flash memory operation is ongoing
    if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR = FLASH_SR_EOP;
    }                                               // reset bit
    FLASH->CR &= ~FLASH_CR_SNB;                     // clear sector number;
    FLASH->CR |= (FLASH_BOOT_FLAG_SECTOR_NUM << 3); // choose sector number (second sector);
    FLASH->CR |= FLASH_CR_PG;                       // Flash programming chosen
    /*!<1 byte writing>*/
    *((uint8_t *)FLASH_BOOT_FLAGS_ADDR) = BOOT_FLAG;
    FLASH->CR &= ~(FLASH_CR_PG); // finish programming
    lock();
}

void Flash::clearBootFlag(void) { eraseSector(FLASH_BOOT_FLAG_SECTOR_NUM); }

void Flash::readBootFlag(void) {
    // sectors = (*(uint32_t *)(addr)) >> 8;
    bootFlag = (*(uint32_t *)(FLASH_BOOT_FLAGS_ADDR));
}

void Flash::readIPs() {
    memcpy((void*)IPs, (void*)FLASH_IP_ADDR, 4);
    //IPs[0] = (uint8_t)*((uint8_t*)FLASH_IP_ADDR);
    //IPs[1] = (uint8_t)*((uint8_t*)FLASH_IP_ADDR + 1);
    //IPs[2] = (uint8_t)*((uint8_t*)FLASH_IP_ADDR + 2);
    //IPs[3] = (uint8_t)*((uint8_t*)FLASH_IP_ADDR + 3);
}
void Flash::writeIPs(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3){
    //!___________ Erase sector 2 0x08008000 (second sector)______________________
    lock();
    unlock();
    while (FLASH->SR & FLASH_SR_BSY) {
    } // no flash memory operation is ongoing (BSY -busy)
    if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR = FLASH_SR_EOP; // END of operation Set by hardware when a Flash operation (programming / erase) is
                                  // completed  Reset by writing a 1
    }
    FLASH->CR |= FLASH_CR_SER;                      // sector erase chosen
    FLASH->CR &= ~FLASH_CR_SNB;                     // clear sector number;
    FLASH->CR |= (FLASH_IP_SECTOR_NUM << 3); // choose sector number (second sector);
    FLASH->CR |= FLASH_CR_STRT;                     // This bit triggers an ERASE operation when set.
    while (FLASH->SR & FLASH_SR_BSY) {
    }                           // while not end of operation
    FLASH->SR = FLASH_SR_EOP;   // reset bit
    FLASH->CR &= ~FLASH_CR_SER; // unchose page erase
    //!_____________ Programm sector 1 ___________________________
    while (FLASH->SR & FLASH_SR_BSY) {
    } // no flash memory operation is ongoing
    if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR = FLASH_SR_EOP;
    }                                               // reset bit
    FLASH->CR &= ~FLASH_CR_SNB;                     // clear sector number;
    FLASH->CR |= (FLASH_BOOT_FLAG_SECTOR_NUM << 3); // choose sector number (second sector);
    FLASH->CR |= FLASH_CR_PG;                       // Flash programming chosen
    /*!<1 byte writing>*/
    *((uint8_t *)FLASH_IP_ADDR) = ip0;
    *((uint8_t *)FLASH_IP_ADDR + 1) = ip1;
    *((uint8_t *)FLASH_IP_ADDR + 2) = ip2;
    *((uint8_t *)FLASH_IP_ADDR + 3) = ip3;
    FLASH->CR &= ~(FLASH_CR_PG); // finish programming
    lock();
}
void Flash::readFromBank2ToRAM() {
    memcpy(RamBuff, (void *)FLASH_BANK2_FIRMWARE, HALF_SECTOR_SIZE);
    memcpy((uint8_t *)CCMRAM_addr, (void *)(FLASH_BANK2_FIRMWARE + HALF_SECTOR_SIZE), HALF_SECTOR_SIZE);
}

HAL_StatusTypeDef Flash::WriteSectorsFromBank2ToBank1() {
    HAL_StatusTypeDef RETURN_VALUE = HAL_ERROR;
    readFromBank2ToRAM();
    RETURN_VALUE = writeBufBank1(SECTOR_SIZE);
    return RETURN_VALUE;
}

HAL_StatusTypeDef Flash::writeBufBank1(uint32_t len) {
    HAL_StatusTypeDef RETURN_VALUE = HAL_ERROR;
    if (!len) {
        return RETURN_VALUE;
    }
    eraseSector(FLASH_SECTOR1_NUM);
    unlock();
    //!_____________ Programm sector 1 ___________________________
    while (FLASH->SR & FLASH_SR_BSY)
        ; // no flash memory operation is ongoing
    if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR = FLASH_SR_EOP;
    }                                      // reset bit
    FLASH->CR &= ~FLASH_CR_SNB;            // clear sector number;
    FLASH->CR |= (FLASH_SECTOR1_NUM << 3); // choose sector number (second sector);
    FLASH->CR |= FLASH_CR_PG;              // Flash programming chosen
    /*!<1 byte writing>*/
    if (len < HALF_SECTOR_SIZE) {
        for (uint32_t i = 0; i < len; i++) {
            *((uint8_t *)FLASH_BANK1_FIRMWARE + i) = *(RamBuff + i);
        }
    } else {
        for (uint32_t i = 0; i < HALF_SECTOR_SIZE; i++) {
            *((uint8_t *)FLASH_BANK1_FIRMWARE + i) = *(RamBuff + i);
        }
        for (uint32_t i = HALF_SECTOR_SIZE; i < len; i++) {
            *((uint8_t *)FLASH_BANK1_FIRMWARE + i) = *((uint8_t *)(void*)CCMRAM_addr + i);
        }
    }
    FLASH->CR &= ~(FLASH_CR_PG); // finish programming
    lock();
    return RETURN_VALUE;
}

HAL_StatusTypeDef Flash::writeBufBank2(uint32_t len) {
    HAL_StatusTypeDef RETURN_VALUE = HAL_ERROR;
    if (!len) {
        return RETURN_VALUE;
    }
    eraseSector(FLASH_SECTOR2_NUM);
    unlock();
    //!_____________ Programm sector 1 ___________________________
    while (FLASH->SR & FLASH_SR_BSY){}// no flash memory operation is ongoing
    if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR = FLASH_SR_EOP;
    }                                      // reset bit
    FLASH->CR &= ~FLASH_CR_SNB;            // clear sector number;
    FLASH->CR |= (FLASH_SECTOR2_NUM << 3); // choose sector number (second sector);
    FLASH->CR |= FLASH_CR_PG;              // Flash programming chosen
    /*!<1 byte writing>*/
    if (len < HALF_SECTOR_SIZE) {
        for (uint32_t i = 0; i < len; i++) {
            *((uint8_t *)FLASH_BANK2_FIRMWARE + i) = *(RamBuff + i);
        }
    } else {
        for (uint32_t i = 0; i < HALF_SECTOR_SIZE; i++) {
            *((uint8_t *)FLASH_BANK2_FIRMWARE + i) = *(RamBuff + i);
        }
        for (uint32_t i = HALF_SECTOR_SIZE; i < len; i++) {
            *((uint8_t *)FLASH_BANK2_FIRMWARE + i) = *((uint8_t *)(void*)CCMRAM_addr + i);
        }
    }
    FLASH->CR &= ~(FLASH_CR_PG); // finish programming
    lock();
    return RETURN_VALUE;
}

void Flash::eraseSector(uint8_t sector) {
    lock();
    unlock();
    while (FLASH->SR & FLASH_SR_BSY) {
    } // no flash memory operation is ongoing (BSY -busy)
    if (FLASH->SR & FLASH_SR_EOP) {
        // END of operation Set by hardware when a Flash operation (programming / erase) is
        // completed  Reset by writing a 1
        FLASH->SR = FLASH_SR_EOP;
    }
    FLASH->CR |= FLASH_CR_SER;  // sector erase chosen
    FLASH->CR &= ~FLASH_CR_SNB; // clear sector number;    while (FLASH->SR1 & FLASH_SR_BSY)
    FLASH->CR |= (sector << 3); // choose sector number (second sector);
    FLASH->CR |= FLASH_CR_STRT; // This bit triggers an ERASE operation when set.
    while (FLASH->SR & FLASH_SR_BSY) {
    }                           // while not end of operation
    FLASH->SR = FLASH_SR_EOP;   // reset bit
    FLASH->CR &= ~FLASH_CR_SER; // unchose page erase
    lock();
}

void Flash::lock(void) { FLASH->CR |= FLASH_CR_LOCK; }
void Flash::unlock(void) {
    //! -- Flash unlock --
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;
}
