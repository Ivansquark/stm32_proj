#include "main.h"
#include "math.h"
#include "GC9A01.h"

#define LED_ON ((GPIOD->ODR) &= ~GPIO_ODR_ODR_7)
#define LED_OFF ((GPIOD->ODR) |= GPIO_ODR_ODR_7)
#define LED_TOGGLE ((GPIOD->ODR) ^= GPIO_ODR_ODR_7)
void leds_init();
void delay(uint32_t ms);
int main()
{
    // SCB->VTOR = (FLASH_BASE+0x20000); /* Vector Table Relocation in Internal FLASH */ // (its done in bootloader)
    //  ----------  FPU initialization -----------------------------------
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 and CP11 Full Access FPU init */
    Rcc rcc(8);                                        // RCC config
    // Rcc rcc(24); //RCC config
    // Flash flash;
    SD sd;
    GC9A01_init();
    struct GC9A01_frame frame = {{0,0},{239,239}};
    GC9A01_set_frame(frame);
    while (1)
    {
        uint8_t color[3];
        // Swiss flag
        color[0] = 0xFF;
        for (int x = 0; x < 240; x++) {
            for (int y = 0; y < 240; y++) {
                if ((x >= 1*48 && x < 4*48 && y >= 2*48 && y < 3*48) ||
                    (x >= 2*48 && x < 3*48 && y >= 1*48 && y < 4*48)) {
                    color[1] = 0xFF;
                    color[2] = 0xFF;
                } else {
                    color[1] = 0x00;
                    color[2] = 0x00;
                }
                if (x == 0 && y == 0) {
                    GC9A01_write(color, sizeof(color));
                } else {
                    GC9A01_write_continue(color, sizeof(color));
                }
            }
        }
        delay(10000);

        //Triangle
        color[0] = 0xFF;
        color[1] = 0xFF;
        for (int x = 0; x < 240; x++) {
            for (int y = 0; y < 240; y++) {
                if (x < y) {
                    color[2] = 0xFF;
                } else {
                    color[2] = 0x00;
                }
                if (x == 0 && y == 0) {
                    GC9A01_write(color, sizeof(color));
                } else {
                    GC9A01_write_continue(color, sizeof(color));
                }
            }
        }
        delay(10000);
        // // Rainbow
        // float frequency = 0.026;
        // for (int x = 0; x < 240; x++)
        // {
         
        //     color[0] = sin(frequency * x + 0) * 127 + 128;
        //     color[1] = sin(frequency * x + 2) * 127 + 128;
        //     color[2] = sin(frequency * x + 4) * 127 + 128;
        //     for (int y = 0; y < 240; y++)
        //     {
        //         if (x == 0 && y == 0)
        //         {
        //             GC9A01_write(color, sizeof(color));
        //         }
        //         else
        //         {
        //             GC9A01_write_continue(color, sizeof(color));
        //         }
        //     }
        // }
        delay(10000);
    }
    return 0;
}

void delay(uint32_t ms) {
    uint32_t MS = ms*1440;
    while(MS--){}
}
