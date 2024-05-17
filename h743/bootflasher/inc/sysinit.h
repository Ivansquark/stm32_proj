#ifndef SYSINIT_H
#define SYSINIT_H

#include "main.h"
#include "cache.h"

class SysInit {
public:
    SysInit();
    static SysInit* pThis;
private:    
    void SystemClock_Config();
    void MX_GPIO_Init(void);
};

#endif //SYSINIT
