#ifndef MAIN_H_
#define MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"
void Error_Handler(void);
uint32_t get_fattime (void);

#ifdef __cplusplus
}
#endif

#endif //MAIN_H
