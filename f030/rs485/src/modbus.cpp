#include "modbus.h"

volatile unsigned char buffer[BUF_SZ]; //буфер в который считываются данные с uart и потом записываются в uart
volatile unsigned int rxtimer = 0;
volatile unsigned char rxcnt = 0; //количество принятых символов
volatile unsigned char txcnt = 0; //количество переданных символов
volatile unsigned char txlen = 0; //длина посылки на отправку
volatile unsigned int delay = 50; //задержка по которой вычисляется конец посылки

namespace Modbus {

union {
    int16_t regs[Hold_Reg_Size]; // внутренние 16битные регистры инф с которых передается
    int8_t bytes[Hold_Reg_Size * 2]; // внутренние 8битные
} res_table;
//счетчик таймера - переменная в которой мы считаем таймаут (несколько раз пока меньше delay)
volatile bool IsPacketReceived = false; //окончание приема флаг 1-окончание приема (rxtimer>delay)
volatile bool IsWriteRequest = false;
volatile bool IsReadRequest = false;

} // namespace Modbus

void Modbus::init() {
    res_table.regs[0] = 1; // MotorControlAddress_Reg
    res_table.regs[1] = 0; // MotorDirectionReg
}

void Modbus::setIsPacketReceived(bool flag) {
    IsPacketReceived = flag;
}
bool Modbus::getIsPacketReceived() {
    return IsPacketReceived;
}
void Modbus::setIsWriteRequest(bool flag) {
    IsWriteRequest = flag;
}
bool Modbus::getIsWriteRequest() {
    return IsWriteRequest;
}
void Modbus::setIsReadRequest(bool flag) {
    IsReadRequest = flag;
}
bool Modbus::getIsReadRequest() {
    return IsReadRequest;
}
void Modbus::setControlReg(uint16_t value){
    res_table.regs[0] = value;
}
uint16_t Modbus::getControlReg() {
    return res_table.regs[0];
}

void Modbus::setDirectionReg(uint16_t value) {
   res_table.regs[1] = value; 
}
uint16_t Modbus::getDirectionReg() {
    return res_table.regs[1];
}


void Modbus::TX_03_04() {
    // READING input & holding registers
    volatile uint16_t regNumber = 0;
    //volatile uint16_t regCount = 0;
    int tmp_val;
    // MODBUS->buffer[0] =SET_PAR[0]; // adress - stays a same as in received
    // MODBUS->buffer[1] = 3; //query type - - stay a same as in recived
    // MODBUS->buffer[2] = data byte count
    // 2-3  - starting address
    regNumber = ((buffer[2] << 8) + buffer[3]); //стартовый адрес для чтения (два байта адреса младший справа)
    // 4-5 - number of registers
    //regCount = ((buffer[4] << 8) + buffer[5]); //количество регистров для чтения (также как и для адресов)

    if (regNumber == MotorDirection_Reg) {
        tmp_val = res_table.regs[1];
        buffer[2] = 2;
        buffer[3] = tmp_val >> 8; // начиная от n = 3 первые три зарезервированы
        buffer[4] = tmp_val; // регистры 16-битные => кладем их побайтно сначала младший потом старший
        // responce length 7 = адр, функц, колво байт=2, high, low, CRCh, CRCl;
        txlen = 7;
    } else {
        // exception illegal data adress 0x02
        TX_EXCEPTION(0x02);
    }
}
void Modbus::TX_06() {
    // Writing
    uint16_t tmp = 0;
    // MODBUS[0] = SET_PAR[0]; // adress - stays a same as in recived
    // MODBUS[1] = 6; //query type - - stay a same as in recived
    // 2-3  - adress   , 4-5 - value
    tmp = ((buffer[2] << 8) + buffer[3]); // adress  адрес регистра
    if (tmp == MotorControlAddress_Reg) {
        IsWriteRequest = true;
        txlen = rxcnt; // responce length длина отправляемой посылки равна длине принятой посылки
        res_table.regs[0] = (buffer[4] << 8) + buffer[5]; //записываем число
    } else {
        // illegal data
        TX_EXCEPTION(0x02);
    }
}
void Modbus::TX_EXCEPTION(unsigned char error_type) {
    // Exception if wrong query
    // modbus exception - illegal data=01 ,adress=02 etc
    // illegal operation
    buffer[2] = error_type; // exception
    txlen = 5;              // responce length
}

void Modbus::parsePacket() {
    unsigned int tmp;
    // recive and checking rx query проверяем адрес и счетчик rxcount > 5
    if ((buffer[0] != 0) & (rxcnt > 5) & ((buffer[0] == ADDRESS) | (buffer[0] == 255))) {
        tmp = Crc16((uint8_t *)buffer, rxcnt - 2); // вычисляем CRC с помощью таблицы и кладем в tmp
        volatile uint16_t CRChi = tmp & 0x00FF;
        volatile uint16_t CRClo = tmp >> 8;
        // если первый байт пришедшего CRC == второму байту вычесленного CRC и...
        // если второй байт пришедшего CRC == первому байту вычесленного CRC
        if ((buffer[rxcnt - 2] == CRChi) & (buffer[rxcnt - 1] == CRClo)) {
            //если мы сюда попали значит пакет наш и crc совпало - надо проверить поддерживаем ли мы такой запрос
            // choosing function
            // проверяем второй байт пакета (метод)
            switch (buffer[1]) {
            case 3: // в случае  0x03 переходим в метод read:
                TX_03_04();
                break;
            case 4: // в случае  0x04 переходим в метод read:
                TX_03_04();
                break;
            case 6: // в случае  0x06 переходим в метод write:
                TX_06();
                break;
            default:
                // illegal operation
                TX_EXCEPTION(0x01);
            }
            // После обработки данных и заполнении буфера отправки
            //добавляем CRC и готовим к отсылке
            // adding CRC16 to reply
            tmp = Crc16((uint8_t *)buffer, txlen - 2); //вычисляем CRC буфера отправки
            buffer[txlen - 2] = tmp;                   // добавляем в буфер младший байт CRC
            buffer[txlen - 1] = tmp >> 8;              // добавляем в буфер старший байт CRC
            txcnt = 0; //обнуляем счетчик байтов на передачу
        }
    }
    //сброс индикаторов окончания посылки
    IsPacketReceived = false;        // сбрасываем флаг о окончании пакета
    rxcnt = 0;        // обнуляем счетчик приема
    rxtimer = 0xFFFF; // обнуляем таймер
}
void Modbus::sendReply() {
    // если буфер на передачу заполнен и передача не производилась
    if ((txlen > 0) & (txcnt == 0)) {
        USART1->CR1 &= ~USART_CR1_RXNEIE; //выкл прерывание на прием чтобы не мешалось
        USART1->CR1 |= USART_CR1_TCIE;    //включаем перывание на окочание передачи
        // usart2_send(uart->buffer[uart->txcnt]); // посылаем первый байт по UART ошибка!!"!!"
    }
}

uint16_t Modbus::Crc16(uint8_t *data, uint8_t length) {
    unsigned short reg_crc = 0xFFFF; // Load a 16–bit register with FFFF hex (all 1’s).
    while (length--) {
        unsigned char data1 = *data;
        data++;
        reg_crc ^= data1;
        // Exclusive OR the first 8–bit byte of the message with the low–order byte of the 16–bit CRC register,
        // putting the result in the CRC register.
        for (int j = 0; j < 8; j++) {
            // Shift the CRC register one bit to the right (toward the LSB),
            // zero–filling the MSB. //"EXTRACT!!!- то есть сравнивать байт до XOR-а => в if-е" and examine the LSB.
            //(If the LSB was 1): Exclusive OR the CRC register with the polynomial value 0xA001
            //(1010 0000 0000 0001).
            if (reg_crc & 0x0001) {
                reg_crc = (reg_crc >> 1) ^ 0xA001;
            } // LSB(b0)=1
            // (If the LSB was 0): Repeat Step 3 (another shift).
            else
                reg_crc = reg_crc >> 1;
        }
    }
    return reg_crc;
}
