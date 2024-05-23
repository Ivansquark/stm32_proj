#include "GC9A01.h"
#include "sd_spi.h"

#define ORIENTATION 1   // Set the display orientation 0,1,2,3

// Command codes:
#define COL_ADDR_SET        0x2A
#define ROW_ADDR_SET        0x2B
#define MEM_WR              0x2C
#define COLOR_MODE          0x3A
#define COLOR_MODE__12_BIT  0x03
#define COLOR_MODE__16_BIT  0x05
#define COLOR_MODE__18_BIT  0x06
#define MEM_WR_CONT         0x3C
#define ON                  1
#define OFF                 0
#define RESET_ON            ((GPIOB->ODR |= GPIO_ODR_ODR_12))
#define RESET_OFF           ((GPIOB->ODR &=~ GPIO_ODR_ODR_12))
#define DC_ON               ((GPIOB->ODR |= GPIO_ODR_ODR_11))
#define DC_OFF              ((GPIOB->ODR &=~ GPIO_ODR_ODR_11))
#define CS_ON               ((GPIOB->ODR |= GPIO_ODR_ODR_13))
#define CS_OFF              ((GPIOB->ODR &=~ GPIO_ODR_ODR_13))
#define MS1                 1440

void GC9A01_spi_tx(uint8_t *data, size_t len) {
    for(int i = 0; i < len; i++) {
        SD::pThis->send_byte(data[i]);
    }    
}

void GC9A01_set_reset(uint8_t val) {
    if (val==0) {
        RESET_OFF;
    } else {
        RESET_ON;
    }
}

void GC9A01_set_data_command(uint8_t val) {
    if (val==0) {
        DC_OFF;
    } else {
        DC_ON;
    }
}

void GC9A01_set_chip_select(uint8_t val) {
    if (val==0) {
        CS_OFF;
    } else {
        CS_ON;
    }
}

void GC9A01_write_command(uint8_t cmd) {
    GC9A01_set_data_command(OFF);
    GC9A01_set_chip_select(OFF);
    GC9A01_spi_tx(&cmd, sizeof(cmd));
    GC9A01_set_chip_select(ON);
}

void GC9A01_write_data(uint8_t *data, size_t len) {
    GC9A01_set_data_command(ON);
    GC9A01_set_chip_select(OFF);
    GC9A01_spi_tx(data, len);
    GC9A01_set_chip_select(ON);
}

static inline void GC9A01_write_byte(uint8_t val) {
    GC9A01_write_data(&val, sizeof(val));
}

void GC9A01_write(uint8_t *data, size_t len) {
    GC9A01_write_command(MEM_WR);
    GC9A01_write_data(data, len);
}

void GC9A01_write_continue(uint8_t *data, size_t len) {
    GC9A01_write_command(MEM_WR_CONT);
    GC9A01_write_data(data, len);
}

void GC9A01_init(void) {
    GPIOB->MODER |= (GPIO_MODER_MODER11_0);
    GPIOB->MODER &= ~(GPIO_MODER_MODER11_1);
    GPIOB->MODER |= (GPIO_MODER_MODER12_0);
    GPIOB->MODER &= ~(GPIO_MODER_MODER12_1);
    GPIOB->MODER |= (GPIO_MODER_MODER13_0);
    GPIOB->MODER &= ~(GPIO_MODER_MODER13_1);
    GC9A01_set_chip_select(ON);
    //LL_mDelay(5);
    for(volatile int i = 0; i < 5*MS1; i++){}
    GC9A01_set_reset(OFF);
    //LL_mDelay(10);
    for(volatile int i = 0; i < 10*MS1; i++){}
    GC9A01_set_reset(ON);
    
    //LL_mDelay(120);
    for(volatile int i = 0; i < 120*MS1; i++){}
    
    /* Initial Sequence */ 
    
    GC9A01_write_command(0xEF);
    
    GC9A01_write_command(0xEB);
    GC9A01_write_byte(0x14);
    
    GC9A01_write_command(0xFE);
    GC9A01_write_command(0xEF);
    
    GC9A01_write_command(0xEB);
    GC9A01_write_byte(0x14);
    
    GC9A01_write_command(0x84);
    GC9A01_write_byte(0x40);
    
    GC9A01_write_command(0x85);
    GC9A01_write_byte(0xFF);
    
    GC9A01_write_command(0x86);
    GC9A01_write_byte(0xFF);
    
    GC9A01_write_command(0x87);
    GC9A01_write_byte(0xFF);
    
    GC9A01_write_command(0x88);
    GC9A01_write_byte(0x0A);
    
    GC9A01_write_command(0x89);
    GC9A01_write_byte(0x21);
    
    GC9A01_write_command(0x8A);
    GC9A01_write_byte(0x00);
    
    GC9A01_write_command(0x8B);
    GC9A01_write_byte(0x80);
    
    GC9A01_write_command(0x8C);
    GC9A01_write_byte(0x01);
    
    GC9A01_write_command(0x8D);
    GC9A01_write_byte(0x01);
    
    GC9A01_write_command(0x8E);
    GC9A01_write_byte(0xFF);
    
    GC9A01_write_command(0x8F);
    GC9A01_write_byte(0xFF);
    
    
    GC9A01_write_command(0xB6);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x00);
    
    GC9A01_write_command(0x36);
    
#if ORIENTATION == 0
    GC9A01_write_byte(0x18);
#elif ORIENTATION == 1
    GC9A01_write_byte(0x28);
#elif ORIENTATION == 2
    GC9A01_write_byte(0x48);
#else
    GC9A01_write_byte(0x88);
#endif
    
    GC9A01_write_command(COLOR_MODE);
    GC9A01_write_byte(COLOR_MODE__18_BIT);
    
    GC9A01_write_command(0x90);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x08);
    
    GC9A01_write_command(0xBD);
    GC9A01_write_byte(0x06);
    
    GC9A01_write_command(0xBC);
    GC9A01_write_byte(0x00);
    
    GC9A01_write_command(0xFF);
    GC9A01_write_byte(0x60);
    GC9A01_write_byte(0x01);
    GC9A01_write_byte(0x04);
    
    GC9A01_write_command(0xC3);
    GC9A01_write_byte(0x13);
    GC9A01_write_command(0xC4);
    GC9A01_write_byte(0x13);
    
    GC9A01_write_command(0xC9);
    GC9A01_write_byte(0x22);
    
    GC9A01_write_command(0xBE);
    GC9A01_write_byte(0x11);
    
    GC9A01_write_command(0xE1);
    GC9A01_write_byte(0x10);
    GC9A01_write_byte(0x0E);
    
    GC9A01_write_command(0xDF);
    GC9A01_write_byte(0x21);
    GC9A01_write_byte(0x0c);
    GC9A01_write_byte(0x02);
    
    GC9A01_write_command(0xF0);
    GC9A01_write_byte(0x45);
    GC9A01_write_byte(0x09);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x26);
    GC9A01_write_byte(0x2A);
    
    GC9A01_write_command(0xF1);
    GC9A01_write_byte(0x43);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x72);
    GC9A01_write_byte(0x36);
    GC9A01_write_byte(0x37);
    GC9A01_write_byte(0x6F);
    
    GC9A01_write_command(0xF2);
    GC9A01_write_byte(0x45);
    GC9A01_write_byte(0x09);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x26);
    GC9A01_write_byte(0x2A);
    
    GC9A01_write_command(0xF3);
    GC9A01_write_byte(0x43);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x72);
    GC9A01_write_byte(0x36);
    GC9A01_write_byte(0x37);
    GC9A01_write_byte(0x6F);
    
    GC9A01_write_command(0xED);
    GC9A01_write_byte(0x1B);
    GC9A01_write_byte(0x0B);
    
    GC9A01_write_command(0xAE);
    GC9A01_write_byte(0x77);
    
    GC9A01_write_command(0xCD);
    GC9A01_write_byte(0x63);
    
    GC9A01_write_command(0x70);
    GC9A01_write_byte(0x07);
    GC9A01_write_byte(0x07);
    GC9A01_write_byte(0x04);
    GC9A01_write_byte(0x0E);
    GC9A01_write_byte(0x0F);
    GC9A01_write_byte(0x09);
    GC9A01_write_byte(0x07);
    GC9A01_write_byte(0x08);
    GC9A01_write_byte(0x03);
    
    GC9A01_write_command(0xE8);
    GC9A01_write_byte(0x34);
    
    GC9A01_write_command(0x62);
    GC9A01_write_byte(0x18);
    GC9A01_write_byte(0x0D);
    GC9A01_write_byte(0x71);
    GC9A01_write_byte(0xED);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x18);
    GC9A01_write_byte(0x0F);
    GC9A01_write_byte(0x71);
    GC9A01_write_byte(0xEF);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x70);
    
    GC9A01_write_command(0x63);
    GC9A01_write_byte(0x18);
    GC9A01_write_byte(0x11);
    GC9A01_write_byte(0x71);
    GC9A01_write_byte(0xF1);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x18);
    GC9A01_write_byte(0x13);
    GC9A01_write_byte(0x71);
    GC9A01_write_byte(0xF3);
    GC9A01_write_byte(0x70);
    GC9A01_write_byte(0x70);
    
    GC9A01_write_command(0x64);
    GC9A01_write_byte(0x28);
    GC9A01_write_byte(0x29);
    GC9A01_write_byte(0xF1);
    GC9A01_write_byte(0x01);
    GC9A01_write_byte(0xF1);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x07);
    
    GC9A01_write_command(0x66);
    GC9A01_write_byte(0x3C);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0xCD);
    GC9A01_write_byte(0x67);
    GC9A01_write_byte(0x45);
    GC9A01_write_byte(0x45);
    GC9A01_write_byte(0x10);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x00);
    
    GC9A01_write_command(0x67);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x3C);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x01);
    GC9A01_write_byte(0x54);
    GC9A01_write_byte(0x10);
    GC9A01_write_byte(0x32);
    GC9A01_write_byte(0x98);
    
    GC9A01_write_command(0x74);
    GC9A01_write_byte(0x10);
    GC9A01_write_byte(0x85);
    GC9A01_write_byte(0x80);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x00);
    GC9A01_write_byte(0x4E);
    GC9A01_write_byte(0x00);
    
    GC9A01_write_command(0x98);
    GC9A01_write_byte(0x3e);
    GC9A01_write_byte(0x07);
    
    GC9A01_write_command(0x35);
    GC9A01_write_command(0x21);
    
    GC9A01_write_command(0x11);
    //LL_mDelay(120);
    for(volatile int i = 0; i < 120*MS1; i++){}
    GC9A01_write_command(0x29);
    //LL_mDelay(20);
    for(volatile int i = 0; i < 20*MS1; i++){}
    
}

void GC9A01_set_frame(struct GC9A01_frame frame) {

    uint8_t data[4];
    
    GC9A01_write_command(COL_ADDR_SET);
    data[0] = (frame.start.X >> 8) & 0xFF;
    data[1] = frame.start.X & 0xFF;
    data[2] = (frame.end.X >> 8) & 0xFF;
    data[3] = frame.end.X & 0xFF;
    GC9A01_write_data(data, sizeof(data));

    GC9A01_write_command(ROW_ADDR_SET);
    data[0] = (frame.start.Y >> 8) & 0xFF;
    data[1] = frame.start.Y & 0xFF;
    data[2] = (frame.end.Y >> 8) & 0xFF;
    data[3] = frame.end.Y & 0xFF;
    GC9A01_write_data(data, sizeof(data));
    
}