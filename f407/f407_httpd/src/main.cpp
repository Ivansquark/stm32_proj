#include "main.h"
#include "lwip.h"
#include "lwip/apps/httpd.h"
#include "net.h"
#include <string.h>
void *__dso_handle = 0;     // to ignore error about destruction objects
extern struct netif gnetif; // LWiP struct declaration

char const *TAGCHAR[] = {"p", "r"};
char const **TAGS = TAGCHAR;
uint32_t n = 0;
const char *LEDS_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);
const tCGI LEDS_CGI = {"/leds.cgi", LEDS_CGI_Handler};
tCGI CGI_TAB[1];
uint8_t ledstate = 0;
char htmstr1[] = "<input value=\"";
char htmstr2[] = "\" name=\"led\" type=\"checkbox\"";

Ethernet ethernet;

void led_init();
inline void led_on() { GPIOD->BSRR |= GPIO_BSRR_BR7; }
inline void led_off() { GPIOD->BSRR |= GPIO_BSRR_BS7; }
inline void led_toggle() { GPIOD->ODR ^= GPIO_ODR_OD7; }

uint16_t SSI_Handler(int iIndex, char *pcInsert, int iInsertLen) {
    strcpy(pcInsert, htmstr1);
    if (iIndex == 0) {
        strcat(pcInsert, "1");
        strcat(pcInsert, htmstr2);
        if (ledstate & 0x01) {
            strcat(pcInsert, " checked> ");
        } else {
            strcat(pcInsert, "> ");
        }
        return strlen(pcInsert);
    } else if (iIndex == 1) {
        strcat(pcInsert, "2");
        strcat(pcInsert, htmstr2);
        if (ledstate & 0x02) {
            strcat(pcInsert, " checked> ");
        } else {
            strcat(pcInsert, "> ");
        }
        return strlen(pcInsert);
    } 
    return 0;
}

const char *LEDS_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]) {
    uint32_t i = 0;
    if (iIndex == 0) {
        led_off();
        ledstate = 0;
        for (i = 0; i < iNumParams; i++) {
            if (strcmp(pcParam[i], "led") == 0) {
                if (strcmp(pcValue[i], "1") == 0) {
                    ledstate = 1;
                    led_on();
                } else {
                    led_off();
                    ledstate = 0;
                }
            }
        }
    }
    return "/led.shtml";
}

int main(void) {
    httpd_init();
    http_set_ssi_handler(SSI_Handler, (char const **)TAGS, 2);
    CGI_TAB[0] = LEDS_CGI;
    http_set_cgi_handlers(CGI_TAB, 1);
    // ethernet.createServer();
    led_init();
    while (true) {
        ethernetif_input(&gnetif);
        sys_check_timeouts();
    }
}

void led_init() {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    GPIOD->MODER |= GPIO_MODER_MODE7_0;
}

void Error_Handler(void) {
    __disable_irq();
    while (1) {
    }
}
