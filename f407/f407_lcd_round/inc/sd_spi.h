#ifndef SD_SPI_H
#define SD_SPI_H

#include "stm32f4xx.h"

class SD
{
public:
    SD();
    static SD* pThis;

    void init();
    void sd_spi_init();     
    void send_byte(uint8_t byte);
    uint8_t read_byte(uint8_t byte);

};



#endif //SD_SPI_H