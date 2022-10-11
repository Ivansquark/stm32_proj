#ifndef NET_H
#define NET_H

#include "lwip.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "main.h"
#include "rcc.h"
#include "i2c.h"

class Ethernet {
  public:
    enum server_states
    {
        ES_NONE = 0,
        ES_ACCEPTED,
        ES_RECEIVE,
        ES_CLOSE
    };

    Ethernet();
    static Ethernet *pThis;

    struct server_struct {
        enum server_states state; /* connection status */
        uint8_t retries;
        tcp_pcb *pcb; /* pointer on the current tcp structure */
        pbuf *p;      /* pointer on pbuf to be transmitted */
    };
    
    void setNewIP(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3);
    
    
    //-----------------------   TCP ---------------------------------------------------------------
    void createServer();
    static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
    static void tcp_server_connection_close(struct tcp_pcb *tpcb, struct server_struct *es);
    static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
    static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
    static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
    static void tcp_server_error(void *arg, err_t er);
    // static struct server_struct *ss; //  global server structure ptr for transmittion
    static tcp_pcb *tcp_server_pcb;
    volatile struct client_struct *cs; // state structure
    static tcp_pcb *tpcbPtr;

    bool isPC_connected = false;
    bool isFrameReadyForSend = false;
    volatile uint32_t lenSendTCP = 0;
    volatile uint8_t sendPacketCounter = 0;
    volatile uint16_t packetCounter = 0;

    bool isDataForSend();
    bool FirmwareUpdateFlag = false;

    // uint8_t tempBuff[600] = {0};
    // uint16_t sizeTempBuff = 0;

    uint32_t totalLen = 0;
    
    //---------------------------   UDP -----------------------------------------------------------
    uint8_t udp_msg_send[255];
    uint8_t udp_msg_receive[255];
    static udp_pcb *udpLocalPcb;
    static udp_pcb *udpBroadcastPcb;
    // static udp_pcb* udpCommutationPcb;

    static void udp_connect(void);
    static void udp_disconnect();

    void udp_BROADCAST_send(const uint8_t *buff, uint16_t len);    
    void udp_sendToPC(const uint8_t *buff, uint16_t len);

    static void udp_receive_BROADCAST_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr,
                                               u16_t port);
    static void udp_receive_Local_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr,
                                           u16_t port);
    // static void udpCommutation_receive_callback(void *arg, struct udp_pcb *upcb,
    //                                 struct pbuf *p, const ip_addr_t *addr, u16_t port);

    //--------------------------    Adresses    ---------------------------------------------------
    //  temporary IP adresses, thats can changed on PC request
    ip_addr_t local_IP = IPADDR4_INIT_BYTES(ip[0], ip[1], ip[2], ip[3]);
    static uint8_t local_ip[4];
    ip_addr_t PC_IP = IPADDR4_INIT_BYTES(192, 168, 3, 100);
    
    uint16_t udp_commutation_port_in = 60001;
    uint16_t udp_commutation_port_out = 60002;
    uint16_t udp_control_port_in = 26185;
    //  constant adresses
    static constexpr uint8_t ip[4] = {192, 168, 3, 200};           // this IP    
    static constexpr uint16_t TCP_PORT = 55555;
    static constexpr uint16_t COMMUTATION_PORT_IN = 26184;
    static constexpr uint16_t COMMUTATION_PORT_OUT = 26185;
    static constexpr uint16_t PC_PORT_OUT = 60001;
    static constexpr uint16_t UDP_PC_PORT = 44444;
    static constexpr uint16_t BROADCAST_PORT = 0xFFFF;
};

#endif // NET_H
