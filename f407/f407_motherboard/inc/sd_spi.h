#ifndef SD_SPI_H
#define SD_SPI_H

#include "stm32f4xx.h"
//#include "GenericTypeDefs.h"
#include "pff.h"
//#include "diskio.h"

class SD
{
public:
    SD();
    static SD* pThis;

    FATFS fs;         /* Рабочая область (file system object) для логических дисков */
	FRESULT res;

    uint8_t send_cmd(uint8_t cmd, uint32_t arg);
    uint8_t sd_init();
    uint8_t fat_init();

    typedef enum : uint8_t {
    	RES_OK = 0,		/* 0: Function succeeded */
    	RES_ERROR,		/* 1: Disk error */
    	RES_NOTRDY,		/* 2: Not ready */
    	RES_PARERR		/* 3: Invalid parameter */
    } L_DRESULT;
    
    
//private:
    L_DRESULT read_sector(
        BYTE *buff,   /* Pointer to the read buffer (NULL:Read bytes are forwarded to the stream) */
        DWORD sector, /* Sector number (LBA) */
        UINT offset,  /* Byte offset to read from (0..511) */
        UINT count /* Number of bytes to read (ofs + cnt mus be <= 512) */);
    L_DRESULT write_sector(
        const BYTE *buff, /* Pointer to the bytes to be written (NULL:Initiate/Finalize sector write) */
        DWORD sc /* Number of bytes to send, Sector number (LBA) or zero */);
    inline void __attribute((always_inline)) cs_idle() {
        GPIOE->ODR |= GPIO_ODR_ODR_6;
    }
    inline void __attribute((always_inline)) cs_set() {
        GPIOE->ODR &=~ GPIO_ODR_ODR_6;;
    }
    __attribute((always_inline)) inline void spiMaxSpeed() {
        SPI2->CR1 &=~ SPI_CR1_SPE; // disable
        SPI2->CR1 &=~ (7<<3);
        SPI2->CR1 |= (0<<3);// SPI_CR1_BR; // (000=>psc=2) 1:1:1  => 42000000/256 = 164,0625 kHz - SPI-2 clk
        SPI2->CR1 |= SPI_CR1_SPE;  // enable
    }
    __attribute((always_inline)) inline void spiMinSpeed() {
        SPI2->CR1 &=~ SPI_CR1_SPE; // disable
        SPI2->CR1 &=~ (7<<3);
        SPI2->CR1 |= (7<<3);// SPI_CR1_BR; // (000=>psc=2) 1:1:1  => 42000000/256 = 164,0625 kHz - SPI-2 clk
        SPI2->CR1 |= SPI_CR1_SPE;  // enable
    }
    void init();
    void sd_spi_init();     
    void send_byte(uint8_t byte);
    uint8_t read_byte(uint8_t byte);
    

    /* Definitions for MMC/SDC command */
    static constexpr uint8_t CMD0	= (0x40+0);     /* GO_IDLE_STATE */
    static constexpr uint8_t CMD1	= (0x40+1);     /* SEND_OP_COND (MMC) */
    static constexpr uint8_t ACMD41	= (0xC0+41);    /* SEND_OP_COND (SDC) */
    static constexpr uint8_t CMD8	= (0x40+8);     /* SEND_IF_COND */
    static constexpr uint8_t CMD16	= (0x40+16);	/* SET_BLOCKLEN */
    static constexpr uint8_t CMD17	= (0x40+17);	/* READ_SINGLE_BLOCK */
    static constexpr uint8_t CMD24	= (0x40+24);	/* WRITE_BLOCK */
    static constexpr uint8_t CMD55	= (0x40+55);	/* APP_CMD */
    static constexpr uint8_t CMD58	= (0x40+58);	/* READ_OCR */
    /* Card type flags (CardType) */
    static constexpr uint8_t SD_MMC		=		0x01;	/* MMC ver 3 */
    static constexpr uint8_t SD_SD1		=		0x02;	/* SD ver 1 */
    static constexpr uint8_t SD_SD2		=		0x04;	/* SD ver 2 */
    static constexpr uint8_t SD_BLOCK   =		0x08;	/* Block addressing */   
    BYTE CardType;
};



#endif //SD_SPI_H