#include "main.h"

volatile uint8_t uartRS485_received_packet;
volatile uint8_t uartLinux_received_packet;

void delay(volatile uint32_t x);

int main(void) {
    rcc_init(16);
    Queue queueRS485;
    Queue queueLinux;
    queue_init(&queueRS485);
    queue_init(&queueLinux);
    uartRS485_init(&queueRS485);
    uartLinux_init(&queueLinux);
    __enable_irq();    
    
    while(1) {   
        if(uartRS485_received_packet) {
            //uint8_t tempArr[13];
            uint8_t count = 0;
            //delay(1000000);
            for(int i = 0; i < 17; i++) {
                if(queueRS485.arr[i]!=0) {
                    count = i;
                    break;
                }
            }
            for(int i=count; i<13+count;i++) {
                uartLinux_sendByte(queueRS485.arr[i]);
            }
            queue_clear(&queueRS485);
            uartRS485_received_packet = 0;            
        }
        if(uartLinux_received_packet) {
            for(int i = 0; i < 11; i++) {
                uartRS485_sendByte(queueLinux.arr[i]);
            }
            queue_clear(&queueLinux);
            uartLinux_received_packet = 0;            
        }
        //delay(1000000);
        //uartRS485_sendByte(0x01);
    }    
    return 0;
}

void delay(volatile uint32_t x) {
    while(x--);
}