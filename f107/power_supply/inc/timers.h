#ifndef TIMER3_H_
#define TIMER3_H_

#include "stm32f10x.h"

/*!
    \brief Timers class
    Tim1 - first timer - 100 ms for exclude button rattling
    Tim4 - 1 second timer interval
    --- Tim2+Tim3  - 32bits counter, counts on ETR input ---
    Tim3 - slave
*/

class Timers
{
public:
    Timers(uint8_t init);
    static volatile bool timer_mSecFlag;
    uint16_t encoderValue();
    void checkValue();
private:
    void init(uint8_t init);        
};

class Interruptable {
public:
    Interruptable();
    virtual ~Interruptable();
    virtual void run() = 0;
};

class InterruptManager {
  public:
    InterruptManager() = delete;
    static void (*IsrVectors[41])();
    static void addHandler(void (*f)(), int irq);
    static void call(int irq);

  private:
};

template <int IRQ> class InterruptClass {
  public:
  InterruptClass(){}
    static void addInterruptClass(Interruptable *c) {
        for (int i = 0; i < 10; i++) {
            if (arrIntClasses[i] == nullptr) {
                arrIntClasses[i] = c;
                return;
            }
        }
    }
    static void delInterruptClass(Interruptable *c) {
        for (int i = 0; i < 10; i++) {
            if (arrIntClasses[i] == c) {
                arrIntClasses[i] = nullptr;
                return;
            }
        }
    }
    static void eventHandler() {
        for (int i = 0; i < 10; i++) {
            if(arrIntClasses[i] != nullptr) {
                arrIntClasses[i]->run();
            }            
        }
    }
    static void setVector() { InterruptManager::addHandler(eventHandler, IRQ); }

    static Interruptable *arrIntClasses[10];
};
template <int IRQ>
Interruptable* InterruptClass<IRQ>::arrIntClasses[10] = {nullptr};
class Adder: public Interruptable {
public:
    Adder();
    int i = 0;
    void run() override;
};

#endif //TIMER3_H_
