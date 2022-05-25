#ifndef BUUTONS_H
#define BUTTONS_H

#include "main.h"
#include "timer.h"

//! Timer 5 is used to set delay for disabling rattle
class Buttons
{
public:
    Buttons();
    static Buttons* pThis;


    bool butOnPressed = false;
    bool butOffPressed = false;
    bool but11Pressed = false;
    bool but12Pressed = false;
    bool but13Pressed = false;
private:
    void init();    


    Buttons(const Buttons&) = delete;
    Buttons& operator=(const Buttons&) = delete;
    Buttons(const Buttons&&) = delete;
    Buttons& operator=(const Buttons&&) = delete;
};



#endif //BUTTOS_H