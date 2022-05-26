#ifndef I2C_H
#define I2C_H

#include "stm32f407xx.h"

class At24c {
	static constexpr uint32_t Timeout_i2c = 500000;
	const uint8_t slaveWrite = 0xA0; // address and write bit
	const uint8_t slaveRead = 0xA1;  // address and read bit
	//const uint8_t slaveWrite = 0x01; // address and write bit
	//const uint8_t slaveRead = 0x00;  // address and read bit
public:
	At24c();
static At24c* pThis;
	void writeBytes(uint16_t addr, uint8_t* buf, uint16_t size);  
	void readBytes(uint16_t addr, void* buf,uint16_t size);
	void writeByte(uint16_t address,uint8_t byte);
	uint8_t readByte(uint16_t address);
	volatile uint8_t At24c_arr[32];
	uint16_t arrSize;

private:
	void at24c_init();
};

#endif //I2C_H