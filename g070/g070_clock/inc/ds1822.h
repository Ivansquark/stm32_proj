#ifndef DS1822_H
#define DS1822_H

#include "stm32g070xx.h"

//! @brief ds1822 temperature sensor one wire implementation

#define OW_0    0x00
#define OW_1    0xff
#define OW_R    0xff

extern const uint8_t convert_T[];
extern const uint8_t read_scratch[];
extern uint8_t scratch[];

typedef struct TEMPERATURE_DS1822 {
    uint8_t sign;
    uint8_t integer;
    uint8_t fractional;
} Temperature_ds1822;

extern Temperature_ds1822 temp1822;

void uart_sendByte(uint8_t byte);
void uart_sendStr(const char* str);
extern volatile uint8_t uart_received_byte;
extern volatile uint8_t uart_received_flag;

void ds1822_init();
void uart_dma_send(const uint8_t *command, uint16_t len);
void uart_dma_read(const uint8_t *command, uint8_t* buf, uint16_t len);
void one_wire_reset();
void ds_1822_read_temperature();
#endif //DS_1822