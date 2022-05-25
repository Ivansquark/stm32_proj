#include "net.h"

Ethernet* Ethernet::pThis = nullptr;
tcp_pcb* Ethernet::tpcbPtr = nullptr;
tcp_pcb* Ethernet::tcp_server_pcb = nullptr;

Ethernet::Ethernet(MRTProto::protoMSG* t_msg) {
    pThis = this; mMsg = t_msg;
}

//struct tcp_pcb* tcp_server_pcb;

//extern volatile uint32_t message_count=0;
volatile uint8_t ethernetDataBuffer[0x1000a]; //buffer for data 

//---------------------------- CREATE SERVER -------------------------------------------------------------------------- 
/**
  * @brief  Initializes the tcp echo server
  * @param  None
  * @retval None
  */
void Ethernet::createServer() {
	ip_addr_t DestIPaddr;
	IP4_ADDR(&DestIPaddr,ip[0],ip[1],ip[2],ip[3]);
	//* create protocol control block for new connection
	tcp_server_pcb =  tcp_new();
	err_t err;
	if(tcp_server_pcb != NULL) {
		// bind tcp pcb to IP and PORT 
		err = tcp_bind(tcp_server_pcb,&DestIPaddr, port);
		if(err == ERR_OK) {
			/* start tcp listening for echo_pcb */
			tcp_server_pcb = tcp_listen(tcp_server_pcb);
			/* initialize LwIP tcp_accept callback function for new connection*/
			tcp_accept(tcp_server_pcb, tcp_server_accept);
		} else {
			/* deallocate the pcb */
			memp_free(MEMP_TCP_PCB, tcp_server_pcb);
		}		
	}
}
//---------------------------------------------------------------------------------------------------------------------

//--------------------- ACCEPT NEW CONNECTION TO SERVER ---------------------------------------------------------------
/**
  * @brief  This function is the implementation of tcp_accept LwIP callback
  * @param  arg: not used
  * @param  newpcb: pointer on tcp_pcb struct for the newly created tcp connection
  * @param  err: not used
  * @retval err_t: error status
  */
err_t Ethernet::tcp_server_accept(void* arg,struct tcp_pcb* newpcb, err_t err) {
	err_t ret_err;
	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);
	/* set priority for the newly accepted tcp connection newpcb */
	tcp_setprio(newpcb, TCP_PRIO_MIN);
	struct server_struct* es;
	/* allocate structure es to maintain tcp connection informations (different for each connection) */
	es = (struct server_struct *)mem_malloc(sizeof(struct server_struct));
	//ss = (struct server_struct *)mem_malloc(sizeof(struct server_struct)); // mem for glob structure
	if (es != NULL) {
		es->state = ES_ACCEPTED;
		es->pcb = newpcb;
		//ss->pcb = newpcb;
		es->retries=0;
		es->p = NULL;
		// pass newly allocated es structure as argument to newpcb (argument to all callback functions)*/
		tcp_arg(newpcb,es);
		//* initialize lwIP tcp_recv callback function for newpcb (receive packet)*/
		tcp_recv(newpcb, tcp_server_recv);
		//* initialize lwIP tcp_err callback function for newpcb */
		tcp_err(newpcb, tcp_server_error);
		//* initialize lwIP tcp_poll callback function for newpcb */
		tcp_poll(newpcb, tcp_server_poll, 1);
		//* set callback on ack event from remote host (when data was sended)
		tcp_sent(newpcb, tcp_server_sent);
		ret_err = ERR_OK;
		err = ERR_OK;
	} else {
		tcp_server_connection_close(newpcb, es);
		/* return memory error */
		ret_err = ERR_MEM;
		err = ERR_OK;
	}
	return ret_err;
}
//---------------------------------------------------------------------------------------------------------------------

//----------------- RECEIVE DATA FROM CLIENT --------------------------------------------------------------------------
/**
  * @brief  This function is the implementation for tcp_recv LwIP callback
  * @param  arg: pointer on a argument for the tcp_pcb connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  pbuf: pointer on the received pbuf
  * @param  err: error information regarding the reveived pbuf
  * @retval err_t: error code
  */
err_t Ethernet::tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
	struct server_struct *es;
	err_t ret_err;
	LWIP_ASSERT("arg != NULL",arg != NULL);
	es = (struct server_struct *)arg;	
	pThis->tpcbPtr = tpcb; // copy ptr of control block
	/* if we receive an empty tcp frame from client => close connection */
	if (p == NULL) {
		/* remote host closed connection */
		es->state = ES_CLOSE;
		if(es->p == NULL) {
			/* we're done sending, close connection */
			tcp_server_connection_close(tpcb, es);
		} 
		ret_err = ERR_OK;
	} else if(err != ERR_OK) {
		/* else : a non empty frame was received from client but for some reason err != ERR_OK */
		if (p != NULL) {
			pbuf_free(p); /* free received pbuf (nothing do with data)*/
			es->p = NULL;			
		}
		ret_err = err;
	} else if(es->state == ES_ACCEPTED) {
		//--------------------- FIRST RECEIVED DATA /* first data chunk in p->payload */ ------------------------
    	es->state = ES_RECEIVE;		
     	es->p = p;		///* store reference to incoming pbuf (chain) */
        pThis->lenSendTCP = es->p->len;		
		tcp_sent(es->pcb, tcp_server_sent); ///* initialize LwIP tcp_sent callback function */		
		memcpy((char*)ethernetDataBuffer,(const char*)p->payload,p->len); // copy received data in global buffer		
        if(MRTProto::FrameParser::pThis->isFullFrameInPacket(p->len)){
            MRTProto::FrameParser::pThis->parseFrame((MRTProto::BYTE_t*)ethernetDataBuffer, p->len);
			pbuf_free(p);
    		pbuf_free(es->p);
    		es->p = NULL;
    		tcp_recved(tpcb, TCP_MSS); //send ACK (with new WND size)           
        }
		ret_err = ERR_OK;
	} else if (es->state == ES_RECEIVE) {
		///* more data received from client and previous data has been already sent*/
		if(es->p == NULL) {
            // empty received buffer (first packet of frame received here)
			es->p = p;
            memcpy((char*)ethernetDataBuffer,(const char*)p->payload,p->len); // copy received data in global buffer		
			pThis->tpcbPtr = tpcb;
    		pThis->lenSendTCP = es->p->len;
			//! -------- CHECK FOR FULL FRAME IN PACKET (ALL FRAMES EXCEPT cmdSet) ------------------------------------
			if(MRTProto::FrameParser::pThis->isFullFrameInPacket(p->len)) {						
				MRTProto::FrameParser::pThis->parseFrame((MRTProto::BYTE_t*)ethernetDataBuffer, p->len);
				pbuf_free(p);
    			pbuf_free(es->p);
    			es->p = NULL;
    			tcp_recved(tpcb, TCP_MSS); //send ACK (with new WND size)                   
            }             
			ret_err = ERR_OK;			
		} else {
            //---------------- RECEIVE IN NOT EMPTY BUFFER (ONLY FOR BIG FRAME - cmdSet)-------------------------------
			memcpy((char*)ethernetDataBuffer + es->p->len, (const char*)p->payload, p->len);
            es->p->len += p->len;
            if(MRTProto::FrameParser::pThis->isFullFrameInPacket(es->p->len)) {
                pThis->tpcbPtr = tpcb;
                pThis->lenSendTCP = es->p->len;
                pThis->isFrameReadyForSend = true;  
                tcp_recved(tpcb, TCP_MSS); //send ACK (with new WND size)
                pbuf_free(p);
                pbuf_free(es->p);
                es->p=NULL;               
            } else {
                //TODO: parse packet (cause may receive several protocol command in packet)                
                if(MRTProto::FrameParser::pThis->isCmdSet()){
                    tcp_recved(tpcb, TCP_MSS); //send ACK (with new WND size)                
                    pbuf_free(p);                
                } else {
					// FREE ALL BUFFERS IF WE GET SOME WIERD DATA
                    tcp_recved(tpcb, TCP_MSS); //send ACK (with new WND size)
                    pbuf_free(p);
                    pbuf_free(es->p);
                    es->p=NULL;
                }
            }            
		}
		ret_err = ERR_OK;
	} else if(es->state == ES_CLOSE) {
		/* data received when connection already closed */
		/* Acknowledge data reception */
		tcp_recved(tpcb, p->tot_len);
		/* free pbuf and do nothing */
        pbuf_free(p);
		ret_err = ERR_OK;
	} else {
		tcp_recved(tpcb, p->tot_len);		
		pbuf_free(p);
		ret_err = ERR_OK;
	}
	return ret_err;
}
//---------------------------------------------------------------------------------------------------------------------

//----------------- CALLBACK FUNCTION THAT CALLED ON ACK SIGNAL FROM REMOTE HOST WHEN IT GETS SENDED DATA -------------
/**
  * @brief  This function implements the tcp_sent LwIP callback (called when ACK
  *         is received from remote host for sent data)
  * @param  None
  * @retval None
  */
err_t Ethernet::tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
	struct server_struct *es;
	LWIP_UNUSED_ARG(len);
	es = (struct server_struct *)arg;
	es->retries = 0;
    /* still got pbufs to send */
	if(es->p != NULL) {		
        pbuf_free(es->p);
        es->p = NULL;				
	} else {
        /* if no more data to send and client closed connection*/
		if(es->state == ES_CLOSE){            
		    tcp_server_connection_close(tpcb, es);
        }		
	}
	return ERR_OK;
}
//---------------------------------------------------------------------------------------------------------------------

void Ethernet::checkForSendingData() {
	if(isFrameReadyForSend) {
        uint32_t size = tcp_sndbuf(tpcbPtr);            
        uint16_t sendSize = 200/2;
        int times = lenSendTCP / sendSize;
        int remainder = lenSendTCP % sendSize;
                    
        // Send frame by different packets (if have space in send buffer)
        if(size >= sendSize) {
            if(packetCounter<times) {
                tcp_write(tpcbPtr, (const void*)(ethernetDataBuffer + sendSize*packetCounter), sendSize, 1); 
                tcp_recved(tpcbPtr, TCP_MSS);                                        
                packetCounter++;
            } else {
                if(remainder) {
                    // send last part of frame
                    tcp_write(tpcbPtr, (const void*)(ethernetDataBuffer + sendSize*packetCounter), remainder, 1);    
                    packetCounter = 0;
                    isFrameReadyForSend = false;
                    tcp_recved(tpcbPtr, TCP_MSS);
                } else {
                    // all frame has been sended
                    packetCounter = 0;
                    isFrameReadyForSend = false;
                    tcp_recved(tpcbPtr, TCP_MSS);
                }                    
            }                
        }                        
    }
}


//--------------------- POLL FUNCTION ---------------------------------------------------------------------------------
/**
  * @brief  This function implements the tcp_poll LwIP callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: pointer on the tcp_pcb for the current tcp connection
  * @retval err_t: error code
  */
err_t Ethernet::tcp_server_poll(void *arg, struct tcp_pcb *tpcb) {
	err_t ret_err;
	struct server_struct *es;
	es = (struct server_struct *)arg;
	if (es != NULL) {
		if (es->p != NULL) {
		} else {
			if(es->state == ES_CLOSE) {
				tcp_server_connection_close(tpcb, es);
			}
		}
		ret_err = ERR_OK;
	} else {
		tcp_abort(tpcb);
		ret_err = ERR_ABRT;
	}
	return ret_err;
}
void Ethernet::tcp_server_error(void* arg, err_t er) {

}

//---------------------------------------------------------------------------------------------------------------------

//---------------------------- FUNCTION CALLED ON CLOSE CONNECTION ----------------------------------------------------
/**
  * @brief  This functions closes the tcp connection
  * @param  tcp_pcb: pointer on the tcp connection
  * @param  es: pointer on echo_state structure
  * @retval None
  */
void Ethernet::tcp_server_connection_close(struct tcp_pcb *tpcb, struct server_struct *es) {
  // remove all callbacks
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);
  if (es != NULL)  {
    mem_free(es);
  }
  tcp_close(tpcb);
}

//*********************************** PRTOCOL COMMUNICATION METHODS ***************************************************

void Ethernet::sendAnsHeartBeat() {
    MRTProto::Head *head = (MRTProto::Head *)(ethernetDataBuffer);
    head->type = MRTProto::ansHeartBeat;
    uint32_t heartBeatCounter = *(uint32_t *)((uint8_t *)ethernetDataBuffer + 8);
    *(uint32_t *)((uint8_t *)ethernetDataBuffer + 8) = heartBeatCounter + 1;    
    // isFrameReadyForSend = true;
    tcp_write(pThis->tpcbPtr, (const void *)(ethernetDataBuffer), 12, 1);    
}

void Ethernet::sendAnsSet() {

}

//--------------- TCP FUNCTION THATS SEND DATA TO REMOTE HOST ---------------------------------------------------------
/**
  * @brief  This function is used to send data for tcp connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  es: pointer on echo_state structure
  * @retval None
  */
// static void tcp_server_send(struct tcp_pcb *tpcb, struct server_struct *es) {
// 	struct pbuf *ptr;
// 	err_t wr_err = ERR_OK;
// 	while ((wr_err == ERR_OK) && (es->p != NULL) && (es->p->len <= tcp_sndbuf(tpcb))) {
// 		 /* get pointer on pbuf from es structure */
// 		ptr = es->p;
// 		/* enqueue data for transmission */
// 		wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
// 		if (wr_err == ERR_OK) {
// 			uint16_t plen;
// 			u8_t freed;
// 			plen = ptr->len;
// 			/* continue with next pbuf in chain (if any) */
// 			es->p = ptr->next;
// 			if(es->p != NULL) {
// 				/* increment reference count for es->p */
// 				pbuf_ref(es->p);
// 			}
// 			/* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
// 			do { freed = pbuf_free(ptr);}
// 			while(freed == 0);
// 			/* Update tcp window size to be advertized : should be called when received
//       		data (with the amount plen) has been processed by the application layer */
// 			tcp_recved(tpcb, plen);
// 		} else if(wr_err == ERR_MEM) {
// 			/* we are low on memory, try later / harder, defer to poll */
// 			es->p = ptr;
// 		} else{
// 			//other problem
// 		}
// 	}
// }
//---------------------------------------------------------------------------------------------------------------------