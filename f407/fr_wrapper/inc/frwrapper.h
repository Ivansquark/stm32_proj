#ifndef FRWRAPPER_H_
#define FRWRAPPER_H_

#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#include "portmacro.h"
#include "main.h"

class iObjectFR
{
public: 
    virtual ~iObjectFR(){}
    virtual void run()=0;
    TaskHandle_t handle=nullptr;   //pointer to task control structure of freertos
};
////////////////////////////////////////////////////////////////////////////////////////////////////////
class cRtos
{
public:
/*************************- task -****************************************************************/
    static void taskCreate(iObjectFR* obj, const char *pName,const uint16_t stackDepth,uint8_t priority)
    { obj->handle = (xTaskCreate(Run,pName,stackDepth,obj,priority,&obj->handle)==pdTRUE) ? obj->handle : nullptr;}                 
    //{ xTaskCreate(reinterpret_cast<TaskFunction_t>(Run),pName,stackDepth,&obj,priority,NULL);}
    
/*************************- queue -**************************************************************/    
    static void queueCreate(uint8_t length, uint8_t size)
    {xQueueCreate(length,size);}    
    static bool queueSend(QueueHandle_t &queue, const void *pItemToQueue,uint16_t timeout)
    {return (xQueueSend(queue, pItemToQueue, timeout)==pdTRUE);}
    static bool queueSendISR(QueueHandle_t &queue, const void *pItemToQueue)
    {return (xQueueSendFromISR(queue, pItemToQueue, nullptr)==pdTRUE);}
    static bool queueRecieve(QueueHandle_t &queue,void* pItem, uint32_t timeout)
    {return (xQueueReceive(queue,pItem,timeout)==pdTRUE);}
    static void queueDelete(QueueHandle_t &queue){vQueueDelete(queue);}
/********************************- semaphore -*************************************************/
    static SemaphoreHandle_t semaphoreCreate()
    {return xSemaphoreCreateBinary();}
    static void semFromIsr(SemaphoreHandle_t handle)
    {xSemaphoreGiveFromISR(handle,nullptr);}
    static bool semRecieve(SemaphoreHandle_t handle, uint16_t timeout)
    {return xSemaphoreTake(handle, timeout);}
    static void deleteSemaphore(SemaphoreHandle_t handle)
    {vSemaphoreDelete(handle);}
/********************************- critical section -********************************************/

/********************************- scheduller -**************************************************/
    static void startScheduler()
    {vTaskStartScheduler();}
    static void sleep(uint16_t x){vTaskDelay(x);}
private:
    //static TaskFunction_t Run(void* x)  // void(*Run)(void*)    
    static void Run(void* x)  // set object here
    {static_cast<iObjectFR*>(x)->run();}    
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T,uint16_t length>
class QueueOS
{
public:
    QueueOS(){queueHandle = cRtos::queueCreate(length,sizeof(T));} //create queue with creation class object
    ~QueueOS(){cRtos::queueDelete(queueHandle);}
    bool queueFromIsr(const T &item){return cRtos::queueSendISR(queueHandle,item);}
    bool queueRecieve(T &item, uint32_t timeout){return cRtos::queueRecieve(queueHandle,item,timeout);}
private:
    QueueHandle_t queueHandle; //указатель на структуру очереди
};
class SemaphoreOS
{
public:
    SemaphoreOS(){semHandle=cRtos::semaphoreCreate();}
    ~SemaphoreOS(){cRtos::deleteSemaphore(semHandle);}
    void giveFromIsr(){cRtos::semFromIsr(semHandle);}
    bool takeSem(uint16_t timeout){return cRtos::semRecieve(semHandle,timeout);}
private:
    SemaphoreHandle_t semHandle;
    //StaticSemaphore_t mutex;
};
/*******************************************************************************************/

class BlinkFR:public iObjectFR
{
public:
    BlinkFR(){leds_ini();}
    ~BlinkFR()override{}
    void run() override
    {
        blink14();
    }
private:
    void blink14()
    {
        while(1)
        {
            GPIOD->ODR^=GPIO_ODR_ODR_14;cRtos::sleep(100);
        }        
    }
    void leds_ini()
    {
        RCC->AHB1ENR|=RCC_AHB1ENR_GPIODEN;
	    GPIOD->MODER|=GPIO_MODER_MODER15_0|GPIO_MODER_MODER14_0|GPIO_MODER_MODER13_0|GPIO_MODER_MODER12_0;
	    GPIOD->MODER&=~(GPIO_MODER_MODER15_1|GPIO_MODER_MODER14_1|GPIO_MODER_MODER13_1|GPIO_MODER_MODER12_1);
	    GPIOD->OTYPER=0; GPIOD->OSPEEDR|=0; GPIOD->ODR=0;
    }
};


#endif //FRWRAPPER_H_