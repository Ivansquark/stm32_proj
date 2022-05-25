#ifndef IRQ_H_
#define IRQ_H_

//#include "main.h"
//#include "portmacro.h"
//#include "semphr.h"

class InterruptManager
{
    public:
    static void (*IsrV[88])();
    static void AddHandler(void (*Fptr)(), IRQn_Type irq){IsrV[irq]=Fptr;}
    static void CallHandler(IRQn_Type irq){IsrV[irq]();}
};
void (*InterruptManager::IsrV[88])()={nullptr};
class Interruptable
{
public:
    Interruptable(){}
    virtual ~Interruptable(){}
    virtual void IrqHandler()=0;
};

template<IRQn_Type irq>
class InterruptableClasses
{
public:
    InterruptableClasses(){}
    constexpr static uint8_t maxClasses=10;
    static void AddClass(Interruptable* addClass)
    {
        for(uint8_t i=0;i<maxClasses;i++)
        {
            if(arrOffInterruptableClasses[i]==nullptr)
            {
                arrOffInterruptableClasses[i] = addClass;
                break;
            }
        }
    }
    static void RemoveClass(Interruptable* addClass)
    {
        for(uint8_t i=0;i<maxClasses;i++)
        {
            if(arrOffInterruptableClasses[i]==addClass)
            {
                arrOffInterruptableClasses[i] = nullptr;
                break;               
            }
        }
    }
    static void CallHandler()
    {
        for(uint8_t i=0;i<maxClasses;i++)
        {
            if(arrOffInterruptableClasses[i]!=nullptr)
            {
                arrOffInterruptableClasses[i]->IrqHandler();               
            }
        }
    }
    static void setVector(){InterruptManager::AddHandler(CallHandler,irq);}
   
private:
    static Interruptable* arrOffInterruptableClasses[10];
};
template <IRQn irq>
Interruptable *InterruptableClasses<irq>::arrOffInterruptableClasses[10]={nullptr};




extern "C" void TIM4_IRQHandler(void)
{
    TIM4->SR&=~TIM_SR_UIF; //drop Update interrupt flag
    //GPIOC->ODR ^= GPIO_ODR_ODR13;    
    InterruptManager::CallHandler(TIM4_IRQn);
}
extern xSemaphoreHandle xBinarySemaphore;
extern "C" void TIM3_IRQHandler(void)
{
    TIM3->SR&=~TIM_SR_UIF; //drop Update interrupt flag
    //portBASE_TYPE xHigherPriorityTaskWoken=pdFALSE;
    //xSemaphoreGiveFromISR(xBinarySemaphore,&xHigherPriorityTaskWoken);
    //if (xHigherPriorityTaskWoken==pdTRUE)
    //{
    //    portYIELD_FROM_ISR(3);
    //}
    //GPIOC->ODR ^= GPIO_ODR_ODR13;    
    InterruptManager::CallHandler(TIM3_IRQn);
    
}
extern "C" void TIM2_IRQHandler(void)
{
    TIM2->SR&=~TIM_SR_UIF; //drop Update interrupt flag    
    InterruptManager::CallHandler(TIM2_IRQn);
}

#endif //IRQ_H_