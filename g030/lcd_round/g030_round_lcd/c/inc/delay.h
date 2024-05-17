#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void delay(volatile uint32_t);
    
#ifdef __cplusplus
}
#endif
#endif // DELAY_H