#include "main.h"
#include "uart.h"
#include "can.h"
#include "button.h"

void SystemClock_Config(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    __enable_irq();
    Uart uart;
    Can can;
    Button buttons;
    
    while (1) {
        HAL_Delay(150);        
        can.TxHeader.StdId = 0x06A1;
        while (HAL_CAN_GetTxMailboxesFreeLevel(&can.hcan) == 0) {
        };
        if (can.send(can.TEMP, 8) != HAL_OK) {
            uart.sendStr("CAN_error");
        }
        switch(buttons.butState) {
            case ButtonState::NOT_PRESSED: {
                
            } break;
            case ButtonState::UP: {
                can.TxHeader.StdId = 0x0290;
                while (HAL_CAN_GetTxMailboxesFreeLevel(&can.hcan) == 0){};
                if (can.send(can.UP, 8) != HAL_OK) {
                    uart.sendStr("CAN_error");
                }
                uart.sendStr("UP");
                buttons.butState = ButtonState::NOT_PRESSED;
            } break;
            case ButtonState::DOWN: {
                can.TxHeader.StdId = 0x0290;
                while (HAL_CAN_GetTxMailboxesFreeLevel(&can.hcan) == 0){};
                if (can.send(can.DOWN, 8) != HAL_OK) {
                    uart.sendStr("CAN_error");
                }
                uart.sendStr("DOWN");
                buttons.butState = ButtonState::NOT_PRESSED;
            } break;
            case ButtonState::UP_DOWN: {
                can.TxHeader.StdId = 0x0290;
                while (HAL_CAN_GetTxMailboxesFreeLevel(&can.hcan) == 0){};
                if (can.send(can.ENTER, 8) != HAL_OK) {
                    uart.sendStr("CAN_error");
                }
                uart.sendStr("UP_DOWN");
                buttons.butState = ButtonState::NOT_PRESSED;
            } break;
            case ButtonState::UP_LONG: {
                can.TxHeader.StdId = 0x0290;
                while (HAL_CAN_GetTxMailboxesFreeLevel(&can.hcan) == 0){};
                if (can.send(can.MAIN, 8) != HAL_OK) {
                    uart.sendStr("CAN_error");
                }
                uart.sendStr("MAIN");
                buttons.butState = ButtonState::NOT_PRESSED;
            } break;
            case ButtonState::DOWN_LONG: {
                can.TxHeader.StdId = 0x0290;
                while (HAL_CAN_GetTxMailboxesFreeLevel(&can.hcan) == 0){};
                if (can.send(can.BC, 8) != HAL_OK) {
                    uart.sendStr("CAN_error");
                }
                uart.sendStr("BC");
                buttons.butState = ButtonState::NOT_PRESSED;
            } break;
        }        
    }
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        Error_Handler();
    }
}

void Error_Handler(void) {
    __disable_irq();
    while (1) {
    }
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line) {}
#endif /* USE_FULL_ASSERT */
