#ifndef LEDS_H
#define LEDS_H

#include "main.h"

namespace Leds {
    void init();
    void backwardOn();
    void backwardOff();
    void forwardOn();
    void forwardOff();
    void emergencyOn();
    void emergencyOff();
    void emergencyToggle();
}

#endif // LEDS_H
