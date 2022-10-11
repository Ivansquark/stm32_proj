#include "main.h"
#include "net.h"
#include "lwip/apps/httpd.h"
#include <string.h>
void * __dso_handle = 0; // to ignore error about destruction objects
extern struct netif gnetif; // LWiP struct declaration


Ethernet ethernet;

int main(void) {
    httpd_init();
    //ethernet.createServer();    
    while (true) {
        
        ethernetif_input(&gnetif);
        sys_check_timeouts();        
    }
}

void Error_Handler(void) {
    __disable_irq();
    while (1) {
    }
}
