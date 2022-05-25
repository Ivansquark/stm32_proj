#include "main.h"

//xSemaphoreHandle xBinarySemaphore;


//extern "C" void vTask1 (void *arg)
//{       
//    while(1)
//    {
//        GPIOD->ODR^=GPIO_ODR_ODR_15;
//        vTaskDelay(100);
//    }
//}
//extern "C" void vTaskIsrHandle(void* pvParameter)
//{
//    PD_led led1;
//    GpTimer timer(3);
//    InterruptableClasses<TIM3_IRQn> intClass;    
//    intClass.AddClass(&led1);
//    intClass.setVector();   
//    while(1)
//    {
//        xSemaphoreTake(xBinarySemaphore,portMAX_DELAY); //ожидаем пока не отдастся семафор из прерывания 
//        // задача в заблокированном состоянии
//        InterruptManager::CallHandler(TIM3_IRQn);
//        //GPIOD->ODR^=GPIO_ODR_ODR_13;
//    }
//}


int main()
{
    PD_led* led1 = new PD_led;
    GpTimer timer(3);
    InterruptableClasses<TIM3_IRQn>* intClass=new InterruptableClasses<TIM3_IRQn>;    
    intClass->AddClass(led1);
    intClass->setVector();       
    //Adc1 adc(16);
    //sum1 s4(3);
    //SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));    /* set CP10 and CP11 Full Access */
    //SpiLcd lcd;
    //
    //lcd.fillScreen(0x0000);
    //Line line(20,20,200,200,Line::RED);
    //line.setDiscretLine(21,21,201,201,Line::YELLOW);
    //Elipse elipse(100,100,100,Elipse::CYAN);
    //Font_16x16 font;
    //font.intToChar(adc.readADC());
    //font.print(10,10,0xff00,font.arr,5);
    //font.floatTochar(adc.getTemp());
    //font.print(100,10,0xff00,font.arrFloat,5);   

    
             
    //vSemaphoreCreateBinary(xBinarySemaphore);    
    //if (xBinarySemaphore!=NULL)
    //{
    //    xTaskCreate(vTask1, "LED15", 100, NULL, 1, NULL);
    //    //xTaskCreate(vTaskIsrHandle,"LED13",100,NULL,3,NULL);
    //    __enable_irq();  
    //    vTaskStartScheduler();//set SP to _estack so main stack no need at all
    //    // if we have objects on main stack they will be corrupted
    //}
    //xTaskCreate(vTask1,"LED15",40,NULL,2,NULL);
    //vTaskStartScheduler();
    BlinkFR* bl=new BlinkFR;    
    cRtos::taskCreate(bl,"LED14",100,3);
    cRtos::startScheduler();

    while(1)
    {
        /*
        for(uint32_t i=0;i<4000000000;i++)
        {
            uint32_t x=i;
            if(i%100==0){x=-i;}
            line.setLine(50,50,100+x,100+sqrtf(i),Line::CYAN);
            font.intToChar(adc.readADC());
            font.print(10,10,0xff00,font.arr,5);
            font.floatTochar(adc.getTemp());
            font.print(100,10,0xff00,font.arrFloat,5);
            font.delay(800000);
        }                
        */
    }
    return 0;
}