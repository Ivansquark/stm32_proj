#ifndef NET_H
#define NET_H

#include "main.h"
#include "hal_init.h"
#include <stdint.h>
#include "lwip.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"


class Ethernet {
public:
    enum server_states {
        ES_NONE = 0,
        ES_ACCEPTED,
        ES_RECEIVE,
        ES_CLOSE
    };
    
    Ethernet();
    static Ethernet* pThis;

    
    struct server_struct{
        enum server_states state; /* connection status */
        uint8_t retries;
        tcp_pcb *pcb; /* pointer on the current tcp structure */
        pbuf* p; /* pointer on pbuf to be transmitted */
    };

    void createServer();
    static err_t tcp_server_accept(void* arg,struct tcp_pcb* newpcb, err_t err);
    static void tcp_server_connection_close(struct tcp_pcb *tpcb, struct server_struct *es);
    static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
    static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
    static void tcp_server_error(void* arg, err_t er);
    //static struct server_struct *ss; //  global server structure ptr for transmittion
    static tcp_pcb* tcp_server_pcb;
    volatile struct client_struct *cs; // state structure
    static tcp_pcb* tpcbPtr;

    volatile uint32_t lenSendTCP = 0;
    volatile uint8_t sendPacketCounter=0;
    volatile uint16_t packetCounter=0;
    
    bool isDataForSend();
    bool isFrameReadyForSend = false;
    
    static constexpr uint8_t ip[4] = {193,168,0,2}; 


    static constexpr uint16_t port = 55555; 
    uint8_t tempBuff[0xFFFF] = {0};
    uint16_t sizeTempBuff = 0;
    uint32_t totalLen = 0;

    //////////////  UDP //////////////////////////////////////////
    static udp_pcb* upcb;
    uint8_t udp_msg[100];
    static void udp_client_connect(void);
    static void udp_receive_callback(void *arg, struct udp_pcb *upcb,
                                     struct pbuf *p, const ip_addr_t *addr, u16_t port);    
private:
    
    
};

#endif //NET_H
