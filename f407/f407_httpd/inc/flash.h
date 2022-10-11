#ifndef FLASH_H
#define FLASH_H

#include "main.h"
#include <stdint.h>
#include <string.h>
//	A main memory block divided into 4 sectors of 16 Kbytes, next 1 sector of 64 Kbytes, and next 4-7 sectors of 128
// Kbytes:
// Sector 0 0x08000000 - 0x08003FFF 16 Kbytes   --  BOOTFLASHER  --
// Sector 1 0x08004000 - 0x08007FFF 16 Kbytes   --  BOOT flag   --
// Sector 2 0x08008000 - 0x0800BFFF 16 Kbyte    --      IPs     --
// Sector 3 0x0800C000 - 0x0800FFFF 16 Kbytes
// Sector 4 0x08010000 - 0x0801FFFF 64 Kbytes
// Sector 5 0x08020000 - 0x0803FFFF 128 Kbytes  -- Main program --
// Sector 6 0x08040000 - 0x0805FFFF 128 Kbytes  -- Main program --

class Flash {
  public:
    static constexpr uint8_t FLASH_BOOT_FLAG_SECTOR_NUM = 1;
    static constexpr uint8_t FLASH_IP_SECTOR_NUM = 2;
    static constexpr uint32_t FLASH_SECTOR1_NUM = 5;
    static constexpr uint32_t FLASH_SECTOR2_NUM = 6;
    static constexpr uint32_t FLASH_START = 0x08000000;
    static constexpr uint32_t FLASH_BOOT_FLAGS_ADDR = 0x08004000;
    static constexpr uint32_t FLASH_BANK1_FIRMWARE = 0x08020000;
    static constexpr uint32_t FLASH_BANK2_FIRMWARE = 0x08040000;
    static constexpr uint32_t SECTOR_SIZE = 0x20000;
    static constexpr uint32_t HALF_SECTOR_SIZE = 0x10000;
    static constexpr uint8_t BOOT_FLAG = 0x01;
    static constexpr uint32_t CCMRAM_addr = 0x10000000;
    static constexpr uint32_t FLASH_IP_ADDR = 0x08008000;

    Flash();
    static Flash *pThis;
    //-------- BOOT --------------
    void writeBootFlag();
    void clearBootFlag(void);
    void readBootFlag(void);
    //-------- IPs ---------------
    uint8_t IPs[4] = {0};
    void readIPs();
    void writeIPs(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3);

    void readFromBank2ToRAM();
    HAL_StatusTypeDef WriteSectorsFromBank2ToBank1();
    HAL_StatusTypeDef writeBufBank1(uint32_t len);
    HAL_StatusTypeDef writeBufBank2(uint32_t len);

    // first half of 128 kB RAM (second in CCMRAM)
    uint8_t RamBuff[HALF_SECTOR_SIZE];
    uint8_t sectors = 0;
    // if 0xFF => start main program
    // else start bootflasher
    uint8_t bootFlag = 0xFF;

    void clearRamBuffer(void);

  private:
    void lock(void);
    void unlock(void);
    void eraseSector(uint8_t sector);
};

#endif // FLASH_H
