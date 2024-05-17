#ifndef GPTIMERS_H_
#define GPTIMERS_H_

#include "main.h"
#include "irq.h"

class GpTimer
{
public:
    GpTimer(uint8_t timNum = 2);
    static bool timFlag;
private:
    void init(uint8_t tim);
};

#endif //GPTIMERS_H_
