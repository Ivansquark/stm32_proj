#ifndef USB_DEVICE_HPP_
#define USB_DEVICE_HPP_

#include "stm32f407xx.h"
#include "uart.h"
#include "normalqueue.h"

#define USB_OTG_DEVICE      ((USB_OTG_DeviceTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_DEVICE_BASE))
#define USB_OTG_IN(ep_num)  ((USB_OTG_INEndpointTypeDef *)(USB_OTG_FS_PERIPH_BASE + USB_OTG_IN_ENDPOINT_BASE)+ep_num)
#define USB_OTG_OUT(ep_num) ((USB_OTG_OUTEndpointTypeDef *)(USB_OTG_FS_PERIPH_BASE +USB_OTG_OUT_ENDPOINT_BASE)+ep_num)

#define RX_FIFO_SIZE         64 //размер очередей в 32 битных словах
#define TX_EP0_FIFO_SIZE     128
#define TX_EP1_FIFO_SIZE     128
#define TX_EP2_FIFO_SIZE     0
#define TX_EP3_FIFO_SIZE     0
//адреса разных FIFO
#define USB_OTG_DFIFO(i) *(uint32_t *)((uint32_t)USB_OTG_FS_PERIPH_BASE + USB_OTG_FIFO_BASE + (i) * USB_OTG_FIFO_SIZE) 

class USB_DEVICE
{
public: 
    USB_DEVICE();    
    static USB_DEVICE* pThis;
    bool Receive_FLAG = false;
    bool USB_send_ADC_FLAG = false;
		uint8_t tx_arr[64] = {0};
		uint8_t rx_arr[64] = {0};
		//bool FLAG2 = false;
    bool INITIALIZED = false;
	//QueByte qBulk_OUT;
	uint8_t BULK_OUT_buf[64]{0};

	void ReadSetupFIFO(void);
    void Enumerate_Setup();
	void fifo_init();
	uint32_t counter{0};
	uint32_t resetFlag{0};
	uint16_t ADDRESS=0;
    uint16_t CurrentConfiguration=0;

	bool addressFlag{false};
	bool setLineCodingFlag{false};
	void SetAdr(uint16_t value);
	void read_BULK_FIFO(uint8_t size);
	void WriteINEP(uint8_t EPnum,uint8_t* buf,uint16_t minLen);
	void cdc_set_line_coding(uint8_t size);
	
	uint8_t BULK_BUF[64]{0};
    
	typedef struct setup_request
    {
    	uint8_t bmRequestType=0; // D7 направление передачи фазы данных 0 = хост передает в устройство 1 = устройство передает на хост...
    	uint8_t bRequest=0;	   // Запрос (2-OUT…6-SETUP)
    	uint16_t wValue=0;	   // (Коды Запросов дескрипторов)
    	uint16_t wIndex=0;	   //
    	uint16_t wLength=0;	   //
    }USB_SETUP_req;		
	
	/*! <накладываем на структуру объединение, чтобы обращаться к различным полям> */
	#pragma pack(push, 1)
	typedef union
	{
		USB_SETUP_req setup; //!< размер структуры
		uint8_t b[8];	 	 //!< массив байтов равный размеру структуры
		uint16_t wRequest;	 //!< Слово объединяющее первые два байта структуры	
	} setupP;    	
	#pragma pack(pop)
	setupP setupPack{0};
	//setupPack setPack;
        
private:
	//#pragma pack (push,1)
    uint8_t line_code[7]{0};
	/*!<меняем местами байты в полуслове, т.к. 16 битные величины USB передает наоборот>*/
	inline uint16_t swap(uint16_t x)
	{return ((x>>8)&(0x00FF))|((x<<8)&(0xFF00));}
    void usb_init();      
	void ep_1_2_3_init();  
	inline void stall();
	//void usbControlPacketProcessed();
	
	void cdc_get_line_coding();
	void cdc_set_control_line_state();
	void cdc_send_break();
	void cdc_send_encapsulated_command(); 
	void cdc_get_encapsulated_command();
	//void getConfiguration();
    //void Set_CurrentConfiguration(uint16_t value);
    
    uint16_t MIN(uint16_t len, uint16_t wLength);
    void WriteFIFO(uint8_t fifo_num, uint8_t *src, uint16_t len);    
};

//!< здесь осуществляется все взаимодействие с USB
extern "C" void OTG_FS_IRQHandler(void);

#endif //USB_DEVICE_HPP_