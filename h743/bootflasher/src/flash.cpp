#include "flash.h"

Flash *Flash::pThis = nullptr;

Flash::Flash() {
    init();
    pThis = this;
    readBootFlag();
    if(bootFlag == 0x01 && sectors > 0 && sectors <= 3) {
        isNeedGoToFlashNewFirmware = true;
    }
}

void Flash::writeBootFlags(void *buf, uint32_t len) { writeBufBank2(FLASH_BOOT_FLAG_SECTOR_NUM, buf, len); }

void Flash::writeBootFlagAndSectorNum(uint8_t sectors_num) {
    uint32_t fullData = (BOOT_FLAG) | (sectors_num << 8);
    writeBootFlags((void *)(&fullData), 4);
}

void Flash::clearBootFlag() {
    // uint32_t temp = 0xFFFFFFFF;
    // writeBootFlags((void*)(&temp),4);
    eraseBank2Sector(0);
}

void Flash::readBootFlag() {
    volatile uint32_t addr = FLASH_BOOT_FLAGS_ADDR;
    sectors = (*(uint32_t *)(addr)) >> 8;
    bootFlag = (*(uint32_t *)(addr));
}

HAL_StatusTypeDef Flash::flashNewFirmware() {
    HAL_StatusTypeDef RETURN_VALUE{HAL_StatusTypeDef::HAL_ERROR};
    if (sectors > 3 || !sectors) {
        return RETURN_VALUE;
    }
    readFromBank2ToRAM(sectors);
    RETURN_VALUE = writeFromRAMtoBank1(sectors);
    if (RETURN_VALUE != HAL_StatusTypeDef::HAL_OK) {
        // TODO: set error status
    }
    
    return RETURN_VALUE;
}

void Flash::readFromBank2ToRAM(uint8_t sectors) {
    for (int i = 0; i < sectors; i++) {
        memcpy((RamBuffer + i * SECTOR_SIZE), (void *)(FLASH_BANK2_FIRMWARE + i*SECTOR_SIZE), SECTOR_SIZE);
    }
}

HAL_StatusTypeDef Flash::writeFromRAMtoBank1(uint8_t sectors) {
    HAL_StatusTypeDef RETURN_VALUE{HAL_StatusTypeDef::HAL_ERROR};
    for (int i = 0; i < sectors; i++) {
        RETURN_VALUE = writeBufBank1(i + 1, (RamBuffer + i * SECTOR_SIZE), SECTOR_SIZE);
        if (RETURN_VALUE != HAL_StatusTypeDef::HAL_OK) {
            // TODO: set error status
            return RETURN_VALUE;
        }
    }
    return RETURN_VALUE;
}

HAL_StatusTypeDef Flash::writeFromRAMtoBank2(uint8_t sectors) {
    HAL_StatusTypeDef RETURN_VALUE{HAL_StatusTypeDef::HAL_ERROR};
    for (int i = 0; i < sectors; i++) {
        RETURN_VALUE = writeBufBank2(i + 1, (RamBuffer + i * SECTOR_SIZE), SECTOR_SIZE);
        if (RETURN_VALUE != HAL_StatusTypeDef::HAL_OK) {
            // TODO: set error status
            return RETURN_VALUE;
        }
    }
    return RETURN_VALUE;
}

HAL_StatusTypeDef Flash::WriteSectorsFromBank2ToBank1(uint8_t sectors) {
    HAL_StatusTypeDef RETURN_VALUE{HAL_StatusTypeDef::HAL_ERROR};
    if (sectors > 3) {
        return RETURN_VALUE;
    }
    readFromBank2ToRAM(sectors);
    writeFromRAMtoBank1(sectors);
}

HAL_StatusTypeDef Flash::writeBufBank1(uint8_t sector, void *buf, uint32_t len) {
    //! flash writes only in flash word=256bits=32bytes
    HAL_StatusTypeDef RETURN_VALUE{HAL_StatusTypeDef::HAL_ERROR};
    if (!len) {
        return RETURN_VALUE;
    }        
    eraseBank1Sector(sector);
    unlock_bank1();
    clearFlagsBank1();
    volatile uint32_t addr = 0x08000000 + 0x20000 * sector;
    uint8_t tempArr[32] = {0};
    uint16_t lenBy32 = len / 32;
    uint16_t remainder32 = len % 32;
    uint16_t wordCounter = 0;
    for (int i = 0; i < lenBy32; i++) {
        for (int j = 0; j < 32; j++) {
            tempArr[j] = *((uint8_t *)buf + i * 32 + j); // copy data to flash word
        }
        RETURN_VALUE = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, addr + i * 32, (uint32_t)tempArr);
        if (RETURN_VALUE != HAL_StatusTypeDef::HAL_OK) {
            // TODO: set error status
        }
        wordCounter++;
    }
    if (remainder32) {
        for (int j = 0; j < 32; j++) {
            if (j < remainder32) {
                tempArr[j] = *((uint8_t *)buf + wordCounter * 32 + j); // copy data to flash word
            } else {
                tempArr[j] = 0;
            }
        }
        RETURN_VALUE = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, addr + wordCounter * 32, (uint32_t)tempArr);
        if (RETURN_VALUE != HAL_StatusTypeDef::HAL_OK) {
            // TODO: set error status
        }
    }
    lock();
    return RETURN_VALUE;
}

HAL_StatusTypeDef Flash::writeBufBank2(uint8_t sector, void *buf, uint32_t len) {
    HAL_StatusTypeDef RETURN_VALUE{HAL_StatusTypeDef::HAL_ERROR};
    if (!len)
        return RETURN_VALUE;
    eraseBank2Sector(sector);
    unlock_bank2();
    clearFlagsBank2();
    volatile uint32_t addr = 0x08100000 + 0x20000 * sector;
    uint8_t tempArr[32] = {0};
    uint16_t lenBy32 = len / 32;
    uint16_t remainder32 = len % 32;
    uint16_t wordCounter = 0;
    for (int i = 0; i < lenBy32; i++) {
        for (int j = 0; j < 32; j++) {
            tempArr[j] = *((uint8_t *)buf + i * 32 + j); // copy data to flash word
        }
        RETURN_VALUE = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, addr + i * 32, (uint32_t)tempArr);
        if (RETURN_VALUE != HAL_StatusTypeDef::HAL_OK) {
            // TODO: set error status
        }
        wordCounter++;
    }
    if (remainder32) {
        for (int j = 0; j < 32; j++) {
            if (j < remainder32) {
                tempArr[j] = *((uint8_t *)buf + wordCounter * 32 + j); // copy data to flash word
            } else {
                tempArr[j] = 0;
            }
        }
        RETURN_VALUE = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, addr + wordCounter * 32, (uint32_t)tempArr);
        if (RETURN_VALUE != HAL_StatusTypeDef::HAL_OK) {
            // TODO: set error status
        }
    }

    lock();
    return RETURN_VALUE;
}

void Flash::eraseBank1Sector(uint8_t sector) {
    lock();
    unlock_bank1();
    clearFlagsBank1();
    while (FLASH->SR1 & FLASH_SR_BSY)
        ; // no flash memory operation is ongoing (BSY -busy)
    if (FLASH->SR1 & FLASH_SR_OPERR) {
        FLASH->CCR1 |= FLASH_CCR_CLR_OPERR; // clear previous write/erase errors
    }
    if (FLASH->SR1 & FLASH_SR_EOP) {
        FLASH->CCR1 = FLASH_CCR_CLR_EOP; // reset end of programming bit
    }
    FLASH->CR1 |= FLASH_CR_SER;  // set erase flag
    FLASH->CR1 &= ~FLASH_CR_SNB; // clear sector number;
    FLASH->CR1 |= (sector & 0x7) << FLASH_CR_SNB_Pos;
    FLASH->CR1 |= FLASH_CR_START;
    while (FLASH->SR1 & FLASH_SR_QW) {
    }
    while (FLASH->SR1 & FLASH_SR_BSY) {
    }
    FLASH->CR1 &= ~FLASH_CR_SER; // unchose page erase
    lock();
}
void Flash::eraseBank2Sector(uint8_t sector) {
    lock();
    unlock_bank2();
    clearFlagsBank2();
    while (FLASH->SR2 & FLASH_SR_BSY)
        ; // no flash memory operation is ongoing (BSY -busy)
    if (FLASH->SR2 & FLASH_SR_OPERR) {
        FLASH->CCR2 |= FLASH_CCR_CLR_OPERR; // clear previous write/erase errors
    }
    if (FLASH->SR2 & FLASH_SR_EOP) {
        FLASH->CCR2 = FLASH_CCR_CLR_EOP; // reset end of programming bit
    }
    FLASH->CR2 |= FLASH_CR_SER;  // set erase flag
    FLASH->CR2 &= ~FLASH_CR_SNB; // clear sector number;
    FLASH->CR2 |= (sector & 0xF) << FLASH_CR_SNB_Pos;
    FLASH->CR2 |= FLASH_CR_START;
    while (FLASH->SR2 & FLASH_SR_QW) {
    }
    while (FLASH->SR2 & FLASH_SR_BSY) {
    }
    lock();
}

void Flash::init() {
    lock();
    unlock_bank1();
    unlock_bank2();
    // clear status errors
    clearFlagsBank1();
    clearFlagsBank2();
    FLASH->CR1 &= ~FLASH_CR_PSIZE; // program by one byte
    FLASH->CR2 &= ~FLASH_CR_PSIZE; // program by one byte
    lock();
}