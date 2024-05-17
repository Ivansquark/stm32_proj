#ifndef MODBUS_H
#define MODBUS_H

#include "main.h"
constexpr uint8_t BUF_SZ = 255;                     //размер буфера

namespace Modbus {

void init(); //инициализируем константы

void parsePacket(); //метод обработки данных модбас, принятых по uart и формирования ответа
//метод обрабытывающая команды 03 (read) и 04 
void TX_03_04();
//метод обрабатывающая команды 06 - запись холдинг регистра
void TX_06();
void TX_EXCEPTION(unsigned char error_type); // метод ошибки запрошенных данных
// void TIM3_init(void); // таймер 3 для подсчета задержки
void sendReply(); //Включение передачи по окончанию приема и обработки данных
// last two bytes in packet
uint16_t Crc16(uint8_t *data, uint8_t length);

void setIsPacketReceived(bool flag);
bool getIsPacketReceived();

void setIsWriteRequest(bool flag);
bool getIsWriteRequest();

void setIsReadRequest(bool flag);
bool getIsReadRequest();

void setControlReg(uint16_t value);
uint16_t getControlReg();

void setDirectionReg(uint16_t value);
uint16_t getDirectionReg();

//volatile unsigned char buffer[BUF_SZ]; //буфер в который считываются данные с uart и потом записываются в uart
//volatile unsigned int rxtimer;
//volatile unsigned char rxcnt; //количество принятых символов
//volatile unsigned char txcnt; //количество переданных символов
//volatile unsigned char txlen; //длина посылки на отправку
//volatile unsigned int delay; //задержка по которой вычисляется конец посылки

// PARAMETERRS ARRAY 0 PARAMETER = MODBUS ADDRESS
constexpr uint8_t ADDRESS = 1;                      // 0-элемент это адрес SLAVE
constexpr uint8_t MODBUS_WRD_SZ = (BUF_SZ - 5) / 2; //максимальное количество регистров в ответе 125
constexpr uint8_t Hold_Reg_Size = 2; //это максимальное количество объектов (холдинг регистров)

// receive
constexpr uint16_t MotorControlAddress_Reg = 0x2000;
constexpr uint16_t MotorForward_Data = 10;
constexpr uint16_t MotorBackward_Data = 6;
constexpr uint16_t MotorStop_Data = 1;

// reply
constexpr uint16_t MotorDirection_Reg = 28;
constexpr uint16_t MotorStopForward_Data = 0x0;
constexpr uint16_t MotorStopBackward_Data = 0x01;
constexpr uint16_t MotorRunForward_Data = 0x02;
constexpr uint16_t MotorRunBackward_Data = 0x03;
} // namespace Modbus

#endif // MODBUS_H
