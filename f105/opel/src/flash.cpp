#include "flash.h"

bool Flash::setRedoutProtectionToOne() {
    while (FLASH->SR & FLASH_SR_BSY) {
    } // Check that no Flash memory operation is ongoing
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB; // access to flash
    FLASH->OPTKEYR = 0x45670123;
    FLASH->OPTKEYR = 0xCDEF89AB; // access to Option Bytes;
    while (FLASH->SR & FLASH_SR_BSY) {
    }                             // Check that no Flash memory operation is ongoing
    FLASH->CR |= FLASH_CR_OPTWRE; // Option byte write enable
    // Option byte programming perform a half-word write operation at the desired Flash address
    FLASH->CR |= FLASH_CR_OPTPG;
    *((uint8_t *)(0x1FFF0F800)) |= 0xFF; // 0xAA - LEVEL_0; 0xCC - LEVEL_2; else - LEVEL_1 _
    while (FLASH->SR & FLASH_SR_BSY) {
    } // Check that no Flash memory operation is ongoing
    if ((FLASH->SR & FLASH_SR_EOP) != 0) {
        FLASH->SR = FLASH_SR_EOP;
    }
    FLASH->CR &= ~FLASH_CR_OPTPG;
    // if (FLASH->OBR & FLASH_OBR_RDPRT1) {
    //     //*!< Read protection Level 1 */
    //     return true;
    // } else
    //     return false;
}