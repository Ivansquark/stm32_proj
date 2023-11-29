#include "main.h"
//#include "uart.h"
#include "can.h"
#include "button.h"
#include "adc.h"
#include "leds.h"
#include "buzzer.h"

//CAN_HandleTypeDef hcan1;
//CAN_HandleTypeDef hcan2;

void SystemClock_Config(void);
void display_init(Can& can);
void drawAdc(Can& can, Adc& adc);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    __enable_irq();
    //Uart uart;
    Can can;
    Button buttons;
    Adc adc;
    Leds led;
    Buzzer buzzer;
    //buzzer.beep(500);
    //HAL_Delay(500);
    //uint32_t counter = 0xFFFF;
    // for (int i = 0; i < 2; i++) {
    //     HAL_Delay(50);
    //     can.TxHeader.StdId = 0x06A1;
    //     while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {};
    //     if (can.send(can.TEMP_ALLWAYS, 8) != HAL_OK) {
    //         //uart.sendStr("CAN_error");
    //     }
    // }
    
    bool flagMainMenu = false;
    

    // HAL_Delay(150);
    // can.TxHeader.StdId = 0x3D0;
    // can.TxData[0] = 0x00;    can.TxData[1] = 0x40;    can.TxData[2] = 0x87;    can.TxData[3] = 0x32;
    // can.TxData[4] = 0x00;    can.TxData[5] = 0x00;    can.TxData[6] = 0x50;    can.TxData[7] = 0x00;
    // while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    // };
    // if (can.send(can.TxData, 8) != HAL_OK) {
    //     //uart.sendStr("CAN_error");
    // }

    // HAL_Delay(200);
    // can.TxHeader.StdId = 0x6A1;
    // can.TxData[0] = 0x61;    can.TxData[1] = 0x00;    can.TxData[2] = 0x42;    can.TxData[3] = 0x16;
    // can.TxData[4] = 0x01;    can.TxData[5] = 0x00;    can.TxData[6] = 0x42;    can.TxData[7] = 0x00;
    // while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    // };
    // if (can.send(can.TxData, 8) != HAL_OK) {
    //     //uart.sendStr("CAN_error");
    // }
    // HAL_Delay(200);
    // can.TxHeader.StdId = 0x6A1;
    // can.TxData[0] = 0x61;    can.TxData[1] = 0x00;    can.TxData[2] = 0x42;    can.TxData[3] = 0x16;
    // can.TxData[4] = 0x01;    can.TxData[5] = 0x00;    can.TxData[6] = 0x42;    can.TxData[7] = 0x00;
    // while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    // };
    // if (can.send(can.TxData, 8) != HAL_OK) {
    //     //uart.sendStr("CAN_error");
    // }
    // HAL_Delay(200);
    // can.TxHeader.StdId = 0x6A1;
    // can.TxData[0] = 0x61;    can.TxData[1] = 0x00;    can.TxData[2] = 0x42;    can.TxData[3] = 0x16;
    // can.TxData[4] = 0x01;    can.TxData[5] = 0x00;    can.TxData[6] = 0x42;    can.TxData[7] = 0x00;
    // while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    // };
    // if (can.send(can.TxData, 8) != HAL_OK) {
    //     //uart.sendStr("CAN_error");
    // }
    
    display_init(can);
    drawAdc(can, adc);
    uint8_t adcLowVoltageBeebCounter = 0;
    bool adcLowVoltageBeebOneTime = false;
    bool adcHighVoltageBeebOneTime = false;
    uint32_t beepDelayCounter = 0;
    bool isBeepDelayCounterEnd = false;
    //flagMainMenu = true;

    uint8_t oneSecCounter = 0;
    bool oneTimeFlagDisp = false;
    while (1) {
        // if(flagMainMenu) {
        //     if(adcDrawCounter>=2){
        //         drawAdc(can, adc);
        //         adcDrawCounter=0;
        //     } else {
        //         adcDrawCounter++;
        //     }            
        // }
        drawAdc(can, adc);
        adc.getVoltage();
        //      Beep counter for minute delay, then turn on beep function
        if (isBeepDelayCounterEnd) {
            if (!adcLowVoltageBeebOneTime) {
                if (adc.voltageInteger < 13) {
                    //if(adcLowVoltageBeebCounter >= 10) {
                    buzzer.beep(500);
                    HAL_Delay(100);
                    buzzer.beep(1000);
                    //adcLowVoltageBeebCounter = 0;
                    adcLowVoltageBeebOneTime = true;
                    //} else {
                    //    adcLowVoltageBeebCounter++;
                    //}            
                }
            }
            if (!adcHighVoltageBeebOneTime) {
                if (adc.voltageInteger >= 15) {
                    //if(adcLowVoltageBeebCounter >= 10) {
                    buzzer.beep(500);
                    HAL_Delay(100);
                    buzzer.beep(1000);
                    //adcLowVoltageBeebCounter = 0;
                    adcHighVoltageBeebOneTime = true;
                    //} else {
                    //    adcLowVoltageBeebCounter++;
                    //}            
                }
            }
            if (adc.voltage >= 1350) {
                adcLowVoltageBeebOneTime = false;
            }
            if (adc.voltage < 1400) {
                adcHighVoltageBeebOneTime = false;
            }
        }
        else {
            beepDelayCounter++;
            if (beepDelayCounter == 500) isBeepDelayCounterEnd = true;
        }

        HAL_Delay(200);
        can.TxHeader.StdId = 0x06A1;
        while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {};
        if (can.send(can.TEMP_ALLWAYS, 8) != HAL_OK) {
            //uart.sendStr("CAN_error");
        }
        led.ledCanToggle();
        switch (buttons.butState) {
        case ButtonState::NOT_PRESSED: {

            } break;
        case ButtonState::UP: {
                //buzzer.beep(30);
                can.TxHeader.StdId = 0x0290;
                while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {};
                if (can.send(can.UP, 8) != HAL_OK) {
                    //uart.sendStr("CAN_error");
                }
                //uart.sendStr("UP");
                buttons.butState = ButtonState::NOT_PRESSED;
            } break;
        case ButtonState::DOWN: {
                //buzzer.beep(30);
                can.TxHeader.StdId = 0x0290;
                while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {};
                if (can.send(can.DOWN, 8) != HAL_OK) {
                    //uart.sendStr("CAN_error");
                }
                //uart.sendStr("DOWN");
                buttons.butState = ButtonState::NOT_PRESSED;
            } break;
        case ButtonState::UP_DOWN: {
                //buzzer.beep(60);
                can.TxHeader.StdId = 0x0290;
                while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {};
                if (can.send(can.BC, 8) != HAL_OK) {
                    //uart.sendStr("CAN_error");
                }
                //uart.sendStr("UP_DOWN");
                buttons.butState = ButtonState::NOT_PRESSED;
                flagMainMenu = false;
            } break;
        case ButtonState::UP_LONG: {
                //buzzer.beep(100);
                //display_init(can);
                can.TxHeader.StdId = 0x0290;
                while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {};
                if (can.send(can.MAIN, 8) != HAL_OK) {
                    //uart.sendStr("CAN_error");
                }
                flagMainMenu = true;
                //uart.sendStr("MAIN");
                buttons.butState = ButtonState::NOT_PRESSED;
            } break;
        case ButtonState::DOWN_LONG: {
                //buzzer.beep(100);
                can.TxHeader.StdId = 0x0290;
                while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {};
                if (can.send(can.ENTER, 8) != HAL_OK) {
                    //uart.sendStr("CAN_error");
                }
                //flagMainMenu = false;
                //uart.sendStr("BC");
                buttons.butState = ButtonState::NOT_PRESSED;
            } break;
        }

        //!TODO: counter
        if (oneSecCounter >= 4) {
            oneSecCounter = 0;
            can.TxHeader.StdId = 0x3D0;
            can.TxData[0] = 0x00;    can.TxData[1] = 0xC0;    can.TxData[2] = 0x87;    can.TxData[3] = 0x32;
            can.TxData[4] = 0x01;    can.TxData[5] = 0x14;    can.TxData[6] = 0x3E;    can.TxData[7] = 0x00;
            while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
            };
            if (can.send(can.TxData, 8) != HAL_OK) {
                //uart.sendStr("CAN_error");
            }
            if (!oneTimeFlagDisp) {
                oneTimeFlagDisp = true;
                display_init(can);
            }
        }
        else {
            oneSecCounter++;
            if(can.flagTimerHalfSecStart) {
                if(can.timerHalfSec >= 2) {
                    can.timerHalfSec = 0;
                    can.flagTimerHalfSecStart = false;
                    Button::pThis->butState = ButtonState::UP_LONG;
                } else {
                    can.timerHalfSec++;
                }                
            }
        }
    }
}


void display_init(Can& can) {
    //______ Display init ______________________________________________________________________________________________________
    //HAL_Delay(50);
    // can.TxHeader.StdId = 0x3D0;
    // can.TxData[0] = 0x00;    can.TxData[1] = 0xC0;    can.TxData[2] = 0x87;    can.TxData[3] = 0x32;
    // can.TxData[4] = 0x01;    can.TxData[5] = 0x14;    can.TxData[6] = 0x3E;    can.TxData[7] = 0x00;
    // while (HAL_CAN_GetTxMailboxesFreeLevel(&can.hcan) == 0) {
    // };
    // if (can.send(can.TxData, 8) != HAL_OK) {
    //     //uart.sendStr("CAN_error");
    // }


    // can.TxHeader.StdId = 0x3D0;
    // can.TxData[0] = 0x85;    can.TxData[1] = 0xC0;    can.TxData[2] = 0x87;    can.TxData[3] = 0x32;
    // can.TxData[4] = 0x01;    can.TxData[5] = 0x00;    can.TxData[6] = 0x50;    can.TxData[7] = 0x00;
    // while (HAL_CAN_GetTxMailboxesFreeLevel(&can.hcan) == 0) {
    // };
    // if (can.send(can.TxData, 8) != HAL_OK) {
    //     //uart.sendStr("CAN_error");
    // }

    // HAL_Delay(150);
    // can.TxHeader.StdId = 0x348;
    // can.TxData[0] = 0x40;    can.TxData[1] = 0x96;    can.TxData[2] = 0x91;    can.TxData[3] = 0x01;
    // can.TxData[4] = 0x40;    can.TxData[5] = 0x00;    can.TxData[6] = 0x57;    can.TxData[7] = 0x59;
    // while (HAL_CAN_GetTxMailboxesFreeLevel(&can.hcan) == 0) {
    // };
    // if (can.send(can.TxData, 8) != HAL_OK) {
    //     //uart.sendStr("CAN_error");
    // }
    // HAL_Delay(150);
    // can.TxHeader.StdId = 0x3D0;
    // can.TxData[0] = 0x85;    can.TxData[1] = 0xC0;    can.TxData[2] = 0x87;    can.TxData[3] = 0x32;
    // can.TxData[4] = 0x01;    can.TxData[5] = 0x00;    can.TxData[6] = 0x4B;    can.TxData[7] = 0x00;
    // while (HAL_CAN_GetTxMailboxesFreeLevel(&can.hcan) == 0) {
    // };
    // if (can.send(can.TxData, 8) != HAL_OK) {
    //     //uart.sendStr("CAN_error");
    // }
    HAL_Delay(50);
    can.TxHeader.StdId = 0x348;
    can.TxData[0] = 0x40;    can.TxData[1] = 0x96;    can.TxData[2] = 0x91;    can.TxData[3] = 0x09;
    can.TxData[4] = 0x41;    can.TxData[5] = 0x00;    can.TxData[6] = 0x57;    can.TxData[7] = 0x59;
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    };
    if (can.send(can.TxData, 8) != HAL_OK) {
        //uart.sendStr("CAN_error");
    }
    HAL_Delay(5);
    can.TxHeader.StdId = 0x759;
    can.TxData[0] = 0x41;    can.TxData[1] = 0x96;    can.TxData[2] = 0x00;    can.TxData[3] = 0x0A;
    can.TxData[4] = 0x41;    can.TxData[5] = 0x04;    can.TxData[6] = 0x00;    can.TxData[7] = 0x4F;
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    };
    if (can.send(can.TxData, 8) != HAL_OK) {
        //uart.sendStr("CAN_error");
    }
    can.TxHeader.StdId = 0x759;
    can.TxData[0] = 0x00;    can.TxData[1] = 0x96;    can.TxData[2] = 0x00;    can.TxData[3] = 0x50;
    can.TxData[4] = 0x00;    can.TxData[5] = 0x45;    can.TxData[6] = 0x00;    can.TxData[7] = 0x4C;
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    };
    if (can.send(can.TxData, 8) != HAL_OK) {
        //uart.sendStr("CAN_error");
    }
    // can.TxHeader.StdId = 0x759;
    // can.TxData[0] = 0x02;    can.TxData[1] = 0x96;    can.TxData[2] = 0x00;    can.TxData[3] = 0x20;
    // can.TxData[4] = 0x00;    can.TxData[5] = 0x4F;    can.TxData[6] = 0x00;    can.TxData[7] = 0x4D;
    // while (HAL_CAN_GetTxMailboxesFreeLevel(&can.hcan) == 0) {
    // };
    // if (can.send(can.TxData, 8) != HAL_OK) {
    //     //uart.sendStr("CAN_error");
    // }
    // can.TxHeader.StdId = 0x759;
    // can.TxData[0] = 0x01;    can.TxData[1] = 0x96;    can.TxData[2] = 0x00;    can.TxData[3] = 0x45;
    // can.TxData[4] = 0x00;    can.TxData[5] = 0x47;    can.TxData[6] = 0x00;    can.TxData[7] = 0x41;
    // while (HAL_CAN_GetTxMailboxesFreeLevel(&can.hcan) == 0) {
    // };
    // if (can.send(can.TxData, 8) != HAL_OK) {
    //     //uart.sendStr("CAN_error");
    // }
    // can.TxHeader.StdId = 0x759;
    // can.TxData[0] = 0x00;    can.TxData[1] = 0x96;    can.TxData[2] = 0x00;    can.TxData[3] = 0x00;
    // can.TxData[4] = 0xFF;    can.TxData[5] = 0xFF;    can.TxData[6] = 0xFF;    can.TxData[7] = 0xFF;
    // while (HAL_CAN_GetTxMailboxesFreeLevel(&can.hcan) == 0) {
    // };
    // if (can.send(can.TxData, 8) != HAL_OK) {
    //     //uart.sendStr("CAN_error");
    // }
    can.TxHeader.StdId = 0x380;
    can.TxData[0] = 0x23;    can.TxData[1] = 0x51;    can.TxData[2] = 0x00;    can.TxData[3] = 0x00;
    can.TxData[4] = 0x00;    can.TxData[5] = 0x00;    can.TxData[6] = 0xC1;    can.TxData[7] = 0x00;
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    };
    if (can.send(can.TxData, 8) != HAL_OK) {
        //uart.sendStr("CAN_error");
    }
    HAL_Delay(50);
    can.TxHeader.StdId = 0x380;
    can.TxData[0] = 0x23;    can.TxData[1] = 0x51;    can.TxData[2] = 0x00;    can.TxData[3] = 0x00;
    can.TxData[4] = 0x00;    can.TxData[5] = 0x00;    can.TxData[6] = 0xC1;    can.TxData[7] = 0x00;
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    };
    if (can.send(can.TxData, 8) != HAL_OK) {
        //uart.sendStr("CAN_error");
    }
}

void drawAdc(Can& can, Adc& adc) {
    adc.getVoltage();
    can.setVoltage(adc.voltageInteger, adc.voltageFractional);
    // can.TxHeader.StdId = 0x460;
    // can.TxData[0] = 0x01;    can.TxData[1] = 0x00;    can.TxData[2] = 0x00;    can.TxData[3] = 0x00;
    // can.TxData[4] = 0x00;    can.TxData[5] = 0x00;    can.TxData[6] = 0x00;    can.TxData[7] = 0x00;
    // while (HAL_CAN_GetTxMailboxesFreeLevel(&can.hcan) == 0) {
    // };
    // HAL_Delay(5);
    can.TxHeader.StdId = 0x348;
    can.TxData[0] = 0x40;    can.TxData[1] = 0x96;    can.TxData[2] = 0x91;    can.TxData[3] = 0x01;
    can.TxData[4] = 0x40;    can.TxData[5] = 0x00;    can.TxData[6] = 0x57;    can.TxData[7] = 0x59;
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    };
    if (can.send(can.TxData, 8) != HAL_OK) {
        //uart.sendStr("CAN_error");
    }
    HAL_Delay(5);
    // can.TxHeader.StdId = 0x368;
    // can.TxData[0] = 0x40;    can.TxData[1] = 0x91;    can.TxData[2] = 0x96;    can.TxData[3] = 0x01;
    // can.TxData[4] = 0x01;    can.TxData[5] = 0x40;    can.TxData[6] = 0x00;    can.TxData[7] = 0x00;
    // while (HAL_CAN_GetTxMailboxesFreeLevel(&can.hcan) == 0) {
    // };
    // if (can.send(can.TxData, 8) != HAL_OK) {
    //    //uart.sendStr("CAN_error");
    // }
    HAL_Delay(5);
    can.TxHeader.StdId = 0x759;
    if (can.send(can.SPACE, 8) != HAL_OK) {
        //uart.sendStr("CAN_error");
    }
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    };
    HAL_Delay(5);
    can.TxHeader.StdId = 0x759;
    if (can.send(can.BAT0, 8) != HAL_OK) {
        //uart.sendStr("CAN_error");
    }
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    };
    HAL_Delay(5);
    can.TxHeader.StdId = 0x759;
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    };
    if (can.send(can.BAT1, 8) != HAL_OK) {
        //uart.sendStr("CAN_error");
    }
    HAL_Delay(5);
    can.TxHeader.StdId = 0x759;
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    };
    if (can.send(can.BAT2, 8) != HAL_OK) {
        //uart.sendStr("CAN_error");
    }
    HAL_Delay(5);
    can.TxHeader.StdId = 0x759;
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    };
    if (can.send(can.BAT3ADC, 8) != HAL_OK) {
        //uart.sendStr("CAN_error");
    }
    HAL_Delay(5);
    can.TxHeader.StdId = 0x759;
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    };
    if (can.send(can.BAT4ADC, 8) != HAL_OK) {
        //uart.sendStr("CAN_error");
    }
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    //RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    //RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    //RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    //RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    //RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
        | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }

    /** Configure the Systick interrupt time
    */
    __HAL_RCC_PLLI2S_ENABLE();
}

void Error_Handler(void) {
    __disable_irq();
    while (1) {
    }
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line) {}
#endif /* USE_FULL_ASSERT */
