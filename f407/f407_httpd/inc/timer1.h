#ifndef TIMER1_H
#define TIMER1_H

#include "main.h"

class Timer1 {
    public:
        Timer1();
        static Timer1* pThis;
    private:
        void init();
};

#endif //TIMER1_H
