#include "main.h"
#include "uart.h"

Uart uart;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);

int main(void) {
  __enable_irq();

  while (1) {
    uart.sendByte(0x31);
    for (volatile int i = 0; i < 1000000; i++)
      ;
  }
}
