#ifndef NET_H
#define NET_H

#include "main.h"
#include <stdint.h>
#include "lwip.h"
#include "lwip/tcp.h"
#include "mrt_proto.h"


class Ethernet {
public:
    enum server_states {
        ES_NONE = 0,
        ES_ACCEPTED,
        ES_RECEIVE,
        ES_CLOSE
    };
    
    Ethernet(MRTProto::protoMSG* t_msg=nullptr);
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

    bool isFrameReadyForSend=false;
    volatile uint32_t lenSendTCP = 0;
    volatile uint8_t sendPacketCounter=0;
    volatile uint16_t packetCounter=0;

    
    void checkForSendingData();

    void sendAnsHeartBeat();
    void sendAnsSet();

private:
    static constexpr uint8_t ip[4] = {193,168,0,2}; 
    static constexpr uint16_t port = 55555; 
    MRTProto::protoMSG* mMsg = nullptr;
};

#endif //NET_H