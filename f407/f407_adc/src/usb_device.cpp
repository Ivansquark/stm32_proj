#include "usb_device.h"
#include "usb_descriptors.h"

USB_DEVICE* USB_DEVICE::pThis=nullptr;

USB_DEVICE::USB_DEVICE()
{pThis=this; usb_init();} //fifo_init();

void USB_DEVICE::usb_init()
    {
        //! -------- инициализация переферии PA11-DM PA12-DP PA9-VBusSens --------------------
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
        GPIOA->MODER &=~ (GPIO_MODER_MODE11_0 | GPIO_MODER_MODE12_0);
        GPIOA->MODER |= (GPIO_MODER_MODE11_1 | GPIO_MODER_MODE12_1); // 1:0 - alternative function push-pull 1:1 full speed
        //GPIOA->MODER &=~ (GPIO_MODER_MODE9);  // 0:0 digital input
        //GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR11 | GPIO_OSPEEDER_OSPEEDR12);
        GPIOA->AFR[1] |= ((10<<16) | (10<<12)); // PA11 PA 12 alt func 10
        //! --------- тактирование USB --------------------------------------------------------------------
        //RCC->CFGR &=~ RCC_CFGR_ ; //! 0 - psk=3; (72*2/3 = 48 MHz)
        RCC->AHB2ENR|=RCC_AHB2ENR_OTGFSEN; //USB OTG clock enable
        // core        
        USB_OTG_FS->GAHBCFG|=USB_OTG_GAHBCFG_GINT; // globalk interrupt mask 1: отмена маскирования прерываний для приложения.
        //USB_OTG_FS->GAHBCFG|=USB_OTG_GAHBCFG_TXFELVL; //1: прерывание бита TXFE (находится в регистре OTG_FS_DIEPINTx) показывает, что IN Endpoint TxFIFO полностью пуст.
        //USB_OTG_FS->GAHBCFG|=USB_OTG_GAHBCFG_PTXFELVL; //1: прерывание когда, непериодический TxFIFO полностью пуст.
        //USB_OTG_FS->GUSBCFG|=USB_OTG_GUSBCFG_SRPCAP; // SRP Бит разрешения управления питанием порта USB (SRP capable bit).
        // FS timeout calibration Приложение должно запрограммировать это поле на основе скорости энумерации.
        USB_OTG_FS->GUSBCFG &=~ USB_OTG_GUSBCFG_TOCAL_2;
        USB_OTG_FS->GUSBCFG &=~ USB_OTG_GUSBCFG_TOCAL_1|USB_OTG_GUSBCFG_TOCAL_0; //1:0:0 - 4 /0.25 = 16 интервалов бит
        // USB turnaround time Диапазон частот AHB	TRDT 32	-	0x6
        USB_OTG_FS->GUSBCFG |= (USB_OTG_GUSBCFG_TRDT_2|USB_OTG_GUSBCFG_TRDT_1);
        USB_OTG_FS->GUSBCFG &=~ (USB_OTG_GUSBCFG_TRDT_3|USB_OTG_GUSBCFG_TRDT_0); //0:1:1:0 = 6
        //USB_OTG_FS->GINTMSK|=USB_OTG_GINTMSK_OTGINT; // unmask OTG interrupt OTG INTerrupt mask.         
        //USB_OTG_FS->GINTMSK|=USB_OTG_GINTMSK_MMISM; //прерывания ошибочного доступа 
        // device
        USB_OTG_DEVICE->DCFG |= USB_OTG_DCFG_DSPD; //1:1 device speed Скорость устройства 48 MHz
        USB_OTG_DEVICE->DCFG &=~ USB_OTG_DCFG_NZLSOHSK; //0: отправляется приложению принятый пакет OUT (нулевой или ненулевой длины) и отправляется handshake на основе бит NAK и STALL 
        // ~ non-zero-length status OUT handshake
        USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_USBRST | USB_OTG_GINTMSK_ENUMDNEM; // unmusk USB reset interrupt Сброс по шине // unmask enumeration done interrupt
        //USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_ESUSPM; // unmask early suspend interrupt //USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_USBSUSPM; // unmask USB suspend interrupt         //USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_SOFM; // unmask SOF interrupt
        //---------------------------------------------------------------------------
		USB_OTG_DEVICE->DCTL = USB_OTG_DCTL_SDIS;  //Отключиться
        USB_OTG_FS->GUSBCFG|=USB_OTG_GUSBCFG_FDMOD;//force device mode
        for(uint32_t i=0; i<1800000;i++){}//wait 25 ms
		//выключаем сенсор VbusB по которому включится резистор при наличии 5 В на Vbus (not nescessary for  f407)
        USB_OTG_FS->GCCFG &=~ USB_OTG_GCCFG_VBUSBSEN;
        USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS | USB_OTG_GCCFG_PWRDWN; 
        USB_OTG_FS->GINTSTS=0xFFFFFFFF; //rc_w1 read_and_clear_write_1 очистить регистр статуса		
        NVIC_SetPriority(OTG_FS_IRQn,1); //приоритет 1
        NVIC_EnableIRQ(OTG_FS_IRQn);
        USB_OTG_DEVICE->DCTL &= ~USB_OTG_DCTL_SDIS;   //Подключить USB      Soft disconnect    
}

void USB_DEVICE::fifo_init()
{
    /*! < Rx_size 32-битных слов. Минимальное значение этого поля 16, максимальное 256 >*/
    USB_OTG_FS->GRXFSIZ = 0; USB_OTG_FS->GRXFSIZ = RX_FIFO_SIZE; 
    /*! < размер и адрес Tx_FIFO (конец Rx_FIFO) для EP0 >*/
	  USB_OTG_FS->DIEPTXF0_HNPTXFSIZ = (TX_EP0_FIFO_SIZE<<16) | RX_FIFO_SIZE; 
	  //! IN endpoint transmit fifo size register
	  USB_OTG_FS->DIEPTXF[0] = (TX_EP1_FIFO_SIZE<<16) | (RX_FIFO_SIZE+TX_EP0_FIFO_SIZE);  //!размер и адрес Tx_FIFO  для EP1
	  USB_OTG_FS->DIEPTXF[1] = (TX_EP2_FIFO_SIZE<<16) | (RX_FIFO_SIZE+TX_EP0_FIFO_SIZE+TX_EP1_FIFO_SIZE); //!размер и адрес Tx_FIFO  для EP2
	  //USB_OTG_FS->DIEPTXF[2] = (TX_EP3_FIFO_SIZE<<16) | (RX_FIFO_SIZE+TX_EP0_FIFO_SIZE+TX_EP1_FIFO_SIZE+TX_EP2_FIFO_SIZE); //! размер и адрес Tx_FIFO  для EP3
	  USB_OTG_FS->DIEPTXF[2] = 0; // EP1_off
	  USB_OTG_FS->DIEPTXF[1] = 0; // EP2_off
	  // 1 пакета SETUP, CNT=1, endpoint 0 OUT transfer size register
	  //USB_OTG_OUT(0)->DOEPTSIZ = (USB_OTG_DOEPTSIZ_STUPCNT_0 | USB_OTG_DOEPTSIZ_PKTCNT) ; //STUPCNT 0:1 = 1
	  // XFRSIZE = 64 - размер транзакции в байтах
	USB_OTG_OUT(0)->DOEPTSIZ = 0;
    USB_OTG_OUT(0)->DOEPTSIZ = (USB_OTG_DOEPTSIZ_STUPCNT_0| USB_OTG_DOEPTSIZ_PKTCNT);  
	USB_OTG_OUT(0)->DOEPTSIZ |= 64;//0x40
}

void USB_DEVICE::Enumerate_Setup(void)               
{   
  //USB_DEVICE::pThis->resetFlag++;  
  uint16_t len=0;
  uint8_t *pbuf; 
  switch(swap(setupPack.wRequest))
  {    
    case GET_DESCRIPTOR_DEVICE:        
      switch(setupPack.setup.wValue)
      {        
        case USB_DESC_TYPE_DEVICE:   //Запрос дескриптора устройства
        //Uart3::pThis->uart3_sendStr("D_D\t\n");
        //counter++;
          len = sizeof(Device_Descriptor);
          pbuf = (uint8_t *)Device_Descriptor; // выставляем в буфер адрес массива с дескриптором устройства.							
          break;
        case USB_DESC_TYPE_CONFIGURATION:   //Запрос дескриптора конфигурации
        //Uart3::pThis->uart3_sendStr("C_D\t\n");  
				len = sizeof(confDescr);
          pbuf = (uint8_t *)&confDescr;				
          break;   		           
        case USBD_IDX_LANGID_STR: //Запрос строкового дескриптора
        //Uart3::pThis->uart3_sendStr("L_iD\t\n");
				len = sizeof(LANG_ID_Descriptor);
          pbuf = (uint8_t *)LANG_ID_Descriptor;  					
          break;
        case USBD_strManufacturer: //Запрос строкового дескриптора
        //Uart3::pThis->uart3_sendStr("s_M\t\n");
				len = sizeof(Man_String);
          pbuf = (uint8_t *)Man_String;                             
          break;
        case USBD_strProduct: //Запрос строкового дескриптора
         //Uart3::pThis->uart3_sendStr("s_P\t\n");
				len = sizeof(Prod_String);
          pbuf = (uint8_t *)Prod_String;         
          break;                     
        case USBD_IDX_SERIAL_STR: //Запрос строкового дескриптора
        //Uart3::pThis->uart3_sendStr("s_S\t\n");
				len = sizeof(SN_String);
          pbuf = (uint8_t *)SN_String;   
					//FLAG=true;
          break;   
				case USB_DESC_TYPE_DEVICE_QUALIFIER:
				//Uart3::pThis->uart3_sendStr("D_Q\t\n");
					len = sizeof(USBD_CDC_DeviceQualifierDesc);
					pbuf = (uint8_t *)USBD_CDC_DeviceQualifierDesc;
				break;				
      }
    break;	
  case SET_ADDRESS:  // Установка адреса устройства       
    /*!< записываем подтверждающий пакет статуса на адрес 0 нулевой длины и затем меняем адрес>*/
		addressFlag=true;
    //WriteINEP(0x00,pbuf,MIN(len , uSetReq.wLength));
    break;
	case GET_CONFIGURATION:
		/*Устройство передает один байт, содержащий код конфигурации устройства*/
		pbuf=(uint8_t*)&confDescr+5; //номер конфигурации (единственной)
		len=1;//WriteINEP(0x00,pbuf,MIN(len , uSetReq.wLength));
    //Uart3::pThis->uart3_sendStr("g_C\t\n");
	break;
    case SET_CONFIGURATION: // Установка конфигурации устройства
	  ep_1_2_3_init(); //инициализируем конечные точки 1-прием, передача и 2-настройка    
      //Uart3::pThis->uart3_sendStr("S_C\n");		
      break;       // len-0 -> ZLP
	case SET_INTERFACE: // Установка конфигурации устройства
	/*<здесь выбирается интерфейс (в данном случае не должен выбираться, т.к. разные конечные точки)>*/
      //Uart3::pThis->uart3_sendStr("SET_INTERFACE\n");    
    break;	  	  
	/* CDC Specific requests */
    case SET_LINE_CODING: //устанавливает параметры линии передач
    //Uart3::pThis->uart3_sendStr(" SLC \n");
	  setLineCodingFlag=true;	        
      break;
    case GET_LINE_CODING:
    //Uart3::pThis->uart3_sendStr(" GLC \n");
      len = sizeof(line_code);
      pbuf = (uint8_t *)line_code;
      //cdc_get_line_coding();           
      break;
    case SET_CONTROL_LINE_STATE:
    //Uart3::pThis->uart3_sendStr(" SCLS \n");
      cdc_set_control_line_state();    
      break;
    case SEND_BREAK:
    //Uart3::pThis->uart3_sendStr(" S_B \n");
      cdc_send_break();                
      break;
    case SEND_ENCAPSULATED_COMMAND:
    //Uart3::pThis->uart3_sendStr("SEND_ENCAPSULATED_COMMAND\n");
      cdc_send_encapsulated_command(); 
      break;
    case GET_ENCAPSULATED_RESPONSE:
    //Uart3::pThis->uart3_sendStr("GET_ENCAPSULATED_RESPONSE\n");
      cdc_get_encapsulated_command();  
      break;
	case CLEAR_FEATURE_ENDP:
			//Uart3::pThis->uart3_sendStr(" CLEAR_FEATURE_ENDP \n");
	  break;
	
	default:
			//Uart3::pThis->uart3_sendStr("def\t\n");
  //stall();
	
  //Uart3::pThis->uart3_sendStr(" STALL \n");
    break;
  }   
  WriteINEP(0x00,pbuf,MIN(len, setupPack.setup.wLength));   // записываем в конечную точку адрес дескриптора и его размер (а также запрошенный размер)
}

void USB_DEVICE::SetAdr(uint16_t value)
{  
    ADDRESS=value;
	  addressFlag = false;	  
    uint32_t add = value<<4;
    USB_OTG_DEVICE->DCFG |= add; //запись адреса.    
    //USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_IEPINT;
    USB_OTG_OUT(0)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);    
    //USART_debug::usart2_sendSTR("ADDRESS\n");
	  // необходимо выставить подтверждение принятия пакета выставления адреса 
	  
}
//-----------------------------------------------------------------------------------------
void USB_DEVICE::WriteINEP(uint8_t EPnum,uint8_t* buf,uint16_t minLen)
{
  USB_OTG_IN(EPnum)->DIEPTSIZ =0;
  /*!<записать количество пакетов и размер посылки>*/
  uint8_t Pcnt = (minLen-1)/64 + 1;  
  USB_OTG_IN(EPnum)->DIEPTSIZ |= (Pcnt<<19)|(minLen);
   /*!<количество передаваемых пакетов (по прерыванию USB_OTG_DIEPINT_XFRC передается один пакет)>*/
  USB_OTG_IN(EPnum)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA); //выставляем перед записью
  if(minLen) WriteFIFO(EPnum, buf, minLen ); //если нет байтов передаем пустой пакет    
}
//---------------------------------------------------------------------------------------------------
uint16_t USB_DEVICE::MIN(uint16_t len, uint16_t wLength)
{
    uint16_t x=0;
    (len<wLength) ? x=len : x=wLength;
    return x;
}
void USB_DEVICE::WriteFIFO(uint8_t fifo_num, uint8_t *src, uint16_t len)
{
    uint32_t words2write = (len+3)>>2; // делим на четыре    
    for (uint32_t index = 0; index < words2write; index++, src += 4)
    {
        /*!<закидываем в fifo 32-битные слова>*/
        USB_OTG_DFIFO(fifo_num) = *((uint32_t *)src);                
    }
    //USART_debug::usart2_sendSTR("WRITE in EP0\n");    
}

void USB_DEVICE::ReadSetupFIFO(void)
{  
  //Прочитать SETUP пакет из FIFO, он всегда 8 байт
  *(uint32_t *)&setupPack = USB_OTG_DFIFO(0);  //! берем адрес структуры, приводим его к указателю на адресное поле STM32, разыменовываем и кладем туда адрес FIFO_0
  // тем самым считывается первые 4 байта из Rx_FIFO
  *(((uint32_t *)&setupPack)+1) = USB_OTG_DFIFO(0); // заполняем вторую часть структуры (очень мудрено сделано)
}
void USB_DEVICE::ep_1_2_3_init()
{  
  USB_OTG_IN(1)->DIEPCTL=0;  
  USB_OTG_IN(2)->DIEPCTL=0;  
  USB_OTG_IN(3)->DIEPCTL=0;
  USB_OTG_OUT(1)->DOEPCTL=0;
  USB_OTG_OUT(2)->DOEPCTL=0;    
  USB_OTG_OUT(3)->DOEPCTL=0;
  USB_OTG_DEVICE->DAINTMSK &=~ ((7<<17)|(7<<1)); //turn off ep1 out and ep2 all

  /*!<EP3_IN, EP3_OUT - BULK, EP1_IN - INTERRUPT>*/
  USB_OTG_IN(1)->DIEPCTL|=64;// 64 байта в пакете
  USB_OTG_IN(1)->DIEPCTL|=USB_OTG_DIEPCTL_EPTYP_1;
  USB_OTG_IN(1)->DIEPCTL&=~USB_OTG_DIEPCTL_EPTYP_0; //1:0 - BULK
  //USB_OTG_IN(1)->DIEPCTL|=USB_OTG_DIEPCTL_TXFNUM_1;//Tx_FIFO_3 0:0:1:1
  USB_OTG_IN(1)->DIEPCTL|=USB_OTG_DIEPCTL_TXFNUM_0;//Tx_FIFO_3 0:0:1:1
  USB_OTG_IN(1)->DIEPCTL|=USB_OTG_DIEPCTL_SD0PID_SEVNFRM; //data0
  USB_OTG_IN(1)->DIEPCTL|=USB_OTG_DIEPCTL_USBAEP; //включаем конечную точку (выключается по ресету) 
  
  USB_OTG_OUT(1)->DOEPCTL|=64;// 64 байта в пакете
  USB_OTG_OUT(1)->DOEPCTL|=USB_OTG_DOEPCTL_EPTYP_1;
  USB_OTG_OUT(1)->DOEPCTL&=~USB_OTG_DOEPCTL_EPTYP_0; //1:0 - BULK 
  USB_OTG_OUT(1)->DOEPCTL|=USB_OTG_DIEPCTL_SD0PID_SEVNFRM; //data0
  USB_OTG_OUT(1)->DOEPCTL|=USB_OTG_DOEPCTL_USBAEP; //включаем конечную точку (выключается по ресету) 
  //------------------------------------------------------------------
  USB_OTG_IN(2)->DIEPCTL|=64;// 64 байта в пакете
  USB_OTG_IN(2)->DIEPCTL|=USB_OTG_DIEPCTL_EPTYP_1;
  USB_OTG_IN(2)->DIEPCTL|=USB_OTG_DIEPCTL_EPTYP_0; //1:1 - INTERRUPT
  USB_OTG_IN(2)->DIEPCTL |= USB_OTG_DIEPCTL_TXFNUM_1;
  //USB_OTG_IN(2)->DIEPCTL|=USB_OTG_DIEPCTL_TXFNUM_0;
  //USB_OTG_IN(1)->DIEPCTL&=~USB_OTG_DIEPCTL_TXFNUM_0;//Tx_FIFO_1 0:0:1:0 = 2 Ep-1
  USB_OTG_IN(2)->DIEPCTL|=USB_OTG_DIEPCTL_SD0PID_SEVNFRM; //data0
  USB_OTG_IN(2)->DIEPCTL|=USB_OTG_DIEPCTL_USBAEP; //включаем конечную точку (выключается по ресету) 
	  
  //!Демаскировать прерывание для каждой активной конечной точки, и замаскировать прерывания для всех не активных конечных точек в регистре OTG_FS_DAINTMSK.
  USB_OTG_DEVICE->DAINTMSK|=(1<<17)|(3<<1);//включаем прерывания на конечных точках 1-OUT  1-IN 2-IN 
  
  /*!<задаем максимальный размер пакета и количество пакетов конечной точки BULK_OUT (непонятно как может быть больше одного пакета), 
	  которое может принять Rx_FIFO>*/
  USB_OTG_OUT(1)->DOEPTSIZ = 0;
  USB_OTG_OUT(1)->DOEPTSIZ |= (1<<19)|(64<<0) ; //PKNT = 1 (DATA), макс размер пакета 64 байта	
  //USB_OTG_OUT(3)->DOEPTSIZ |= (1<<19)|(1<<0) ; //PKNT = 1 (DATA), 1 ,байт - прерывание по приему одного байта	
  // разрешаем прием пакета OUT на BULK точку 
  USB_OTG_OUT(1)->DOEPCTL|=USB_OTG_DOEPCTL_CNAK|USB_OTG_DOEPCTL_EPENA; //разрешаем конечную точку OUT
  //USB_OTG_OUT(0)->DOEPCTL|=USB_OTG_DOEPCTL_CNAK; //разрешаем конечную точку OUT
//-------------------------------------------------------
/*< Заполняем массив line_code>*/	
	for(uint8_t i=0;i<7;i++){line_code[i] = line_coding[i];}	
}

void USB_DEVICE::stall()
{
	/*TODO: send STALL signal*/
	USB_OTG_IN(0)->DIEPCTL|=USB_OTG_DIEPCTL_STALL;
}

void USB_DEVICE::cdc_set_line_coding(uint8_t size)
{
	//необходимо вычитывать из OUT пакета
	uint8_t lineC[8];	
	*(uint32_t*)(lineC) = USB_OTG_DFIFO(0);
	*((uint32_t*)(lineC)+1) = USB_OTG_DFIFO(0); //заполнили структуру
	for(uint8_t i=0;i<7;i++)
	{line_code[i] = *((uint8_t*)(&lineC)+i);} //Fill this array from FIFO		
	//Uart3::pThis->uart3_sendStr("l_c \n");       
}
void USB_DEVICE::cdc_get_line_coding()
{	
    //Uart3::pThis->uart3_sendStr("g_l_c\t\n");    
}

void USB_DEVICE::cdc_set_control_line_state() //rts, dtr (не используем пока)
{
    //Uart3::pThis->uart3_sendStr("s_c_l_s\t\n"); 
    INITIALIZED = true;
}
void USB_DEVICE::cdc_send_break()
{}
void USB_DEVICE::cdc_send_encapsulated_command()
{}
void USB_DEVICE::cdc_get_encapsulated_command()
{}

void USB_DEVICE::read_BULK_FIFO(uint8_t size)
{
	uint8_t size_on_for = (size+3)>>2;//делим на 4
	uint32_t buf[16]; //выделяем промежуточный буфер на 64 байта
	//uint8_t ostatok = size%4;
/*!<Засовываем в очередь>*/		
	for (uint8_t i=0;i<size_on_for;i++)
	{
		buf[i]=USB_OTG_DFIFO(0); //вычитываем из Rx_FIFO			
	}	
	for(uint8_t j=0;j<size;j++)
	{
		//BULK_OUT_buf[j]=*((uint8_t*)(buf)+j); //записываем в буфер
		rx_arr[j] = *((uint8_t*)(buf)+j); // write to rx buf
		//qBulk_OUT.push(BULK_OUT_buf[j]);
        //NormalQueue8::pThis->push(BULK_OUT_buf[j]);
	}
}


//!< здесь осуществляется все взаимодействие с USB
extern "C" void OTG_FS_IRQHandler(void)
{		
	//Инициализация конечной точки 0 при USB reset:
    if(USB_OTG_FS->GINTSTS &  USB_OTG_GINTSTS_USBRST)
	{
		//Uart3::pThis->uart3_sendStr("reset\n");			
		//1. Установка бита NAK для всех конечных точек OUT: SNAK = 1 в регистре OTG_FS_DOEPCTLx (для всех конечных точек OUT, x это номер конечной точки).
		//Используя этот бит, приложение может управлять передачей отрицательных подтверждений NAK конечной точки.
		USB_OTG_OUT(0)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
		USB_OTG_OUT(1)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
		USB_OTG_OUT(2)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
		USB_OTG_OUT(3)->DOEPCTL |= USB_OTG_DOEPCTL_SNAK;
		//2. Демаскирование следующих бит прерывания включаем прерывания конечной точки 0
		USB_OTG_DEVICE->DAINTMSK |= (1<<0); //control 0 IN endpoint  биты маски прерываний конечной точки IN разрешаем прерывания 
		USB_OTG_DEVICE->DAINTMSK |= (1<<16); //control 0 OUT endpoint  биты маски прерываний конечной точки OUT разрешаем прерывания
		// разрешаем прерывания завершения фазы настройки и завершения транзакции OUT
		USB_OTG_DEVICE->DOEPMSK |= USB_OTG_DOEPMSK_STUPM; //1 разрешаем прерывание SETUP Phase done .
		USB_OTG_DEVICE->DOEPMSK |= USB_OTG_DOEPMSK_XFRCM; //1 TransfeR Completed interrupt Mask. разрешаем на чтение
		// разрешаем прерывания таймаута и завершения транзакции IN
		USB_OTG_DEVICE->DIEPMSK |= USB_OTG_DIEPMSK_XFRCM; // TransfeR Completed interrupt Mask. (прерывание завершения транзакции) разрешаем на запись
		USB_OTG_IN(0)->DIEPINT |= USB_OTG_DIEPINT_ITTXFE;		
		/*!<обнуляем структуру>*/
		USB_DEVICE::pThis->setupPack.setup.bmRequestType=0;USB_DEVICE::pThis->setupPack.setup.bRequest=0;
		USB_DEVICE::pThis->setupPack.setup.wValue=0;USB_DEVICE::pThis->setupPack.setup.wIndex=0;
		USB_DEVICE::pThis->setupPack.setup.wLength=0;	
		
		USB_DEVICE::pThis->fifo_init(); //заново инициализируем FIFO
				//Сбросить все TXFIFO
		USB_OTG_FS->GRSTCTL = USB_OTG_GRSTCTL_TXFFLSH | USB_OTG_GRSTCTL_TXFNUM;
		while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH);
		//Сбросить RXFIFO
		USB_OTG_FS->GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;
		while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH); // сбрасываем Tx и Rx FIFO
		USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_ENUMDNEM; // unmask enumeration done interrupt
		USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_USBRST;	
		/*!<Обнууляем адрес>*/
		uint8_t value = 0x7F; //7 bits of address
		USB_OTG_DEVICE->DCFG &=~ (value<<4); //запись адреса 0.
		USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_USBRST; // сбрасываем бит		
	}
     /*! <start of Enumeration done> */
	if(USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_ENUMDNE)
	{
		//Uart3::pThis->uart3_sendStr("enum\t\n");
		//Инициализация конечной точки по завершению энумерации:
		USB_OTG_FS->GINTSTS |= USB_OTG_GINTSTS_ENUMDNE; // бит выставляется при окончании энумерации, необходимо его очистить
		USB_OTG_IN(0)->DIEPCTL &=~ USB_OTG_DIEPCTL_MPSIZ; //0:0 - 64 байта, Приложение должно запрограммировать это поле максимальным размером пакета для текущей логической конечной точки. 
		//!< разрешаем генерацию прерывания при приеме в FIFO, конечных точек IN, OUT и непустого буфера Rx
		USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM | USB_OTG_GINTMSK_IEPINT | USB_OTG_GINTMSK_OEPINT;		
		//На этом этапе устройство готово принять пакеты SOF и оно сконфигурировано для выполнения управляющих транзакций на control endpoint 0.
	}
//---------------------------------------------------------------------------------------------------------------------------------------
    /*! < прерывание конечной точки IN  (на передачу данных)> */
//-----------------------------------------------------------------------------------------------------------------------------------------
    if(USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_IEPINT) 
	{		
		uint32_t epnums  = USB_OTG_DEVICE->DAINT; // номер конечной точки вызвавшей прерывание 
        uint32_t epint;
        epnums &= USB_OTG_DEVICE->DAINTMSK;   	  // определяем этот номер	с учетом разрешенных точек
        if( epnums & 0x0001) // если конечная точка 0
		{ //EP0 IEPINT     
			epint = USB_OTG_IN(0)->DIEPINT;  //Этот регистр показывает статус конечной точки по отношению прерываниям.
			epint &= USB_OTG_DEVICE->DIEPMSK;  // считываем биты разрешенных прерываний 
			if(epint & USB_OTG_DIEPINT_XFRC) // если Transfer Completed interrupt. Показывает, что транзакция завершена как на AHB, так и на USB.
			{				
				//USART_debug::usart2_sendSTR("In XFRC\n");	
					      /*!< (TODO: реализовать очередь в которую сначала закидываем побайтово буффер с дескриптором) >*/                
				if(USB_OTG_IN(0)->DIEPTSIZ & USB_OTG_DIEPTSIZ_PKTCNT)//QueWord::pThis->is_not_empty()/*usb.Get_TX_Q_cnt(0)*/)	 			
				{/*!<Если есть еще пакеты в Tx_FIFO, необходимо их записать, а только потом разрешать OUT>*/					
				}
				else
				{
					//EndPoint ENAble. Приложение устанавливает этот бит, чтобы запустить передачу на конечной точке 0.
					//USB_OTG_IN(0)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA); // Clear NAK. Запись в этот бит очистит бит NAK для конечной точки.
					/*!<Разрешаем входную точку по приему пакета OUT>*/
					USB_OTG_OUT(0)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA); // Clear NAK. Запись в этот бит очистит бит NAK для конечной точки.
				}
			}
			if(epint & USB_OTG_DIEPINT_TOC) //TimeOut Condition. Показывает, что ядро определило событие таймаута на USB для последнего токена IN на этой конечной точке.
			{
                //Uart3::pThis->uart3_sendStr("USB_OTG_DIEPINT_TOC\n");
            }
			//Показывает, что токен IN был принят, когда связанный TxFIFO (периодический / не периодический) был пуст.
			if(epint & USB_OTG_DIEPINT_ITTXFE) // IN Token received when TxFIFO is Empty.
			{
                //Uart3::pThis->uart3_sendStr("USB_OTG_DIEPINT_ITTXFE\n");
            }
			//Показывает, что данные на вершине непериодического TxFIFO принадлежат конечной точке, отличающейся от той, для которой был получен токен IN.
			if(epint & USB_OTG_DIEPINT_INEPNE) //IN token received with EP Mismatch.
			{
                //Uart3::pThis->uart3_sendStr("USB_OTG_DIEPINT_INEPNE\n");
            }
			//  EndPoint DISableD interrupt. Этот бит показывает, что конечная точка запрещена по запросу приложения.
			if(( epint & USB_OTG_DIEPINT_EPDISD) == USB_OTG_DIEPINT_EPDISD)
			{}
			//когда TxFIFO для этой конечной точки пуст либо наполовину, либо полностью.
			if(epint & USB_OTG_DIEPINT_TXFE) //Transmit FIFO Empty.
			{
                //Uart3::pThis->uart3_sendStr("USB_OTG_DIEPINT_TXFE\n");
            }
			USB_OTG_IN(0)->DIEPINT = epint; //сбрасываем регистр статуса прерываний записью единицы rc_w1 (read/clear_write_1)
		}
		if( epnums & 0x0002) // если конечная точка 1 BULK IN
		{ //EP1 IEPINT
			//USART_debug::usart2_sendSTR("Bulk IN\n");
			epint = USB_OTG_IN(1)->DIEPINT;  //Этот регистр показывает статус конечной точки по отношению к событиям USB и AHB.
			epint &= USB_OTG_DEVICE->DIEPMSK;  // считываем разрешенные биты 
			/*!<передаем данные в BULK точку (если данные есть в данном FIFO то они передадутся и сработает это прерывание)>*/
			if(epint & USB_OTG_DIEPINT_XFRC) // передача пакета окончена
			{				
				//Uart3::pThis->uart3_sendStr("B_IN \n");                
      }
			if(epint & USB_OTG_DIEPINT_TXFE) //Transmit FIFO Empty.
			{
                //Uart3::pThis->uart3_sendStr("INt IN USB_OTG_DIEPINT_TXFE\n");
            }
            //Uart3::pThis->uart3_sendStr("bulk in\t\n");
			USB_OTG_IN(1)->DIEPINT = epint;//сбрасываем регистр статуса прерываний записью единицы rc_w1 (read/clear_write_1)
		}
		if( epnums & 0x0004) // если конечная точка 2 INTERRUPT IN
		{ //EP2 IEPINT
			epint = USB_OTG_IN(2)->DIEPINT;  //Этот регистр показывает статус конечной точки по отношению к событиям USB и AHB.
			epint &= USB_OTG_DEVICE->DIEPMSK;  // считываем разрешенные биты 
			/*!<передаем запрошенные данные в INTERRUPT точку communication interface>*/
		    USB_OTG_IN(2)->DIEPINT = epint;//сбрасываем регистр статуса прерываний записью единицы rc_w1 (read/clear_write_1)
		}
		//if( epnums & 0x0008) // если конечная точка 3 Bulk IN
		//{ //EP2 IEPINT
		//	epint = USB_OTG_IN(3)->DIEPINT;  //Этот регистр показывает статус конечной точки по отношению к событиям USB и AHB.
		//	epint &= USB_OTG_DEVICE->DIEPMSK;  // считываем разрешенные биты 
		//	/*!<передаем запрошенные данные в INTERRUPT точку communication interface>*/
    //        GPIOD->ODR ^= GPIO_ODR_OD15;
		//    USB_OTG_IN(3)->DIEPINT = epint;//сбрасываем регистр статуса прерываний записью единицы rc_w1 (read/clear_write_1)
		//}    
		USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_IEPINT; //IN EndPoints INTerrupt mask. Разрешаем прерывание конечных точек IN				
		return;
    }    
	//---------------------------------------------------------------------------------------------------------------------------------------
    /*! < прерывание конечной точки OUT  (на прием данных)> */
	//-----------------------------------------------------------------------------------------------------------------------------------------
	//На любом прерывании конечной точки OUT приложение должно прочитать регистр размера транзакции конечной точки
    if(USB_OTG_FS->GINTSTS & USB_OTG_GINTMSK_OEPINT) // прерывание конечной точки OUT (на прием данных) (срабатывает в первый раз при приеме Setup пакета)
    {		
		uint32_t epnums  = USB_OTG_DEVICE->DAINT;
		uint32_t epint;				
		epnums &= USB_OTG_DEVICE->DAINTMSK;			// определяем конечную точку	
		if( epnums & 0x00010000)		// конечная точка 0
		{ //EP0 OEPINT     
		    epint = USB_OTG_OUT(0)->DOEPINT; //Этот регистр показывает статус конечной точки по отношению к событиям USB и AHB.
		    epint &= USB_OTG_DEVICE->DOEPMSK;	 // считываем разрешенные биты 
		    // Transfer Completed interrupt. Это поле показывает, что для этой конечной точки завершена запрограммированная транзакция
		    if(epint & USB_OTG_DOEPINT_XFRC)
		    {// в момент SETUP приходят данные 
				//USB_DEVICE::pThis->WriteINEP(0,nullptr,0); //ZLP в пакете статуса ответа !!!ХЗ
		    }
		    //SETUP phase done. На этом прерывании приложение может декодировать принятый пакет данных SETUP.
		    if(epint & USB_OTG_DOEPINT_STUP) // Показывает, что фаза SETUP завершена (пришел пакет)
		    {		
                                
		    	USB_DEVICE::pThis->Enumerate_Setup(); // декодировать принятый пакет данных SETUP. (прочесть из Rx_FIFO 8 байт в первый раз должен быть запрос дескриптора устройства)
				if(USB_DEVICE::pThis->addressFlag) 
				{				
					USB_DEVICE::pThis->SetAdr((USB_DEVICE::pThis->setupPack.setup.wValue));//установить адрес устройства
					USB_DEVICE::pThis->addressFlag=false; 
				}				
		    }
		    if(epint & USB_OTG_DOEPINT_OTEPDIS)//OUT Token received when EndPoint DISabled. Показывает, что был принят токен OUT, когда конечная точка еще не была разрешена.
		    {}
				////////////////////////////////////////////////////////////////
				USB_OTG_OUT(0)->DOEPTSIZ = (USB_OTG_DOEPTSIZ_STUPCNT_0 | USB_OTG_DOEPTSIZ_PKTCNT);  
				USB_OTG_OUT(0)->DOEPTSIZ |= 24;//0x40
					USB_OTG_OUT(0)->DOEPCTL|=USB_OTG_DOEPCTL_CNAK;
				///////////////////////////////////////////////////////////////	
		    USB_OTG_OUT(0)->DOEPINT = epint; //сбрасываем регистр статуса прерываний записью единицы rc_w1 (read/clear_write_1)
		}
		if( epnums & 0x00020000)		// конечная точка 1 BULK_OUT
		{ //EP1 OEPINT
			//USART_debug::usart2_sendSTR("BULK_OUT\n");	
			epint = USB_OTG_OUT(1)->DOEPINT;  //Этот регистр показывает статус конечной точки по отношению к событиям USB и AHB.
		    epint &= USB_OTG_DEVICE->DOEPMSK; // считываем разрешенные биты 
			/*!<Когда приложение прочитало все данные, ядро генерирует прерывание XFRC>*/ 
			if(epint & USB_OTG_DOEPINT_XFRC)
			{
				/*!<разгребаем принятые данные в BULK точку>*/
				//USB_OTG_OUT(1)->DOEPCTL|=USB_OTG_DOEPCTL_CNAK|USB_OTG_DOEPCTL_EPENA;
			}
						
			//----------------------------------------------------------------------
		    //Uart3::pThis->uart3_sendStr("bulk_out\t\n");
            USB_OTG_OUT(1)->DOEPINT = epint; //сбрасываем регистр статуса прерываний записью единицы rc_w1 (read/clear_write_1)
		}		
		USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_OEPINT; //IN EndPoints INTerrupt mask. Разрешаем прерывание конечных точек IN
    return;
    }
//-----------------------------------------------------------------------------------------------	
	//!OTG_FS_GINTSTS_RXFLVL /* Receive FIFO non-empty */   буффера RX не пуст (принят пакет например: Out и Data)
	if(USB_OTG_FS->GINTSTS & USB_OTG_GINTSTS_RXFLVL)
	{			
		USB_OTG_FS-> GINTMSK &=~ USB_OTG_GINTMSK_RXFLVLM;// (запрещаем прерывания пока не прочитаем пакет, в Rx_FIFO уже лежат данные)
		/*!< !!!Чтение регистра Receive status read and pop дополнительно извлекает данные из вершины RxFIFO!!! >*/
		uint32_t reg_Rx_status = USB_OTG_FS->GRXSTSP; //считываем регистр статуса Rx_FIFO в который считывается информация из пакета
		uint8_t status = ((reg_Rx_status & USB_OTG_GRXSTSP_PKTSTS)>>17)&(0xF); //считываем статус пакета SETUP
		uint8_t epNum = (reg_Rx_status & USB_OTG_GRXSTSP_EPNUM); //узнаем конечную точку
		uint8_t bytesSize=((reg_Rx_status & USB_OTG_GRXSTSP_BCNT)>>4)&(0xFF);//узнаем количество пришедших байт
		
		if(bytesSize) 
		{//! Если количество байт принимаемого пакета не равно 0           
			switch (status) 
			{
				/*! <принят пакет данных OUT. DATA stage>*/
				case 2: 
				//USART_debug::usart2_sendSTR("OUT packet\n");
				{
					//USART_debug::usart2_sendSTR("OUT packet\n");					
					if(USB_DEVICE::pThis->setLineCodingFlag)
					{/*!<Пришел control пакет для записи в конечную точку OUT>*/
						//Uart3::pThis->uart3_sendStr("OUT completed\n");
						USB_DEVICE::pThis->cdc_set_line_coding(bytesSize);
						USB_DEVICE::pThis->setLineCodingFlag=false;						  	
					} //читаем пакет setLineCoding (из 8 байт)
					else 
					{//!другие пакеты (BULK) принимается всегда один DATA пакет после OUT пакета					  
						if (epNum == 1)
						{/*!< считываем Rx_FIFO в очередь>*/
							/*!<необходимо записать принятые байты в память (в буфер очереди)>*/
							uint8_t size = 64 - (USB_OTG_OUT(1)->DOEPTSIZ & 0xFF);
							USB_DEVICE::pThis->resetFlag=size;
							//Uart3::pThis->uart3_sendByte(size);
							USB_DEVICE::pThis->resetFlag=size;
							if(size)
							{								
								USB_DEVICE::pThis->read_BULK_FIFO(size); //read size bytes, fifo must become empty					
								USB_DEVICE::pThis->Receive_FLAG = true;
							}							
						}
					}											
				}
				break;
				/*принят пакет данных SETUP.*/
				case 6: // SETUP packet recieve Эти данные показывают, что в RxFIFO сейчас доступен для чтения пакет SETUP указанной конечной точки.
				{					
					{	
						//Uart3::pThis->uart3_sendStr("readFIFO\n");						
						USB_DEVICE::pThis->ReadSetupFIFO();	
						uint32_t setupStatus = USB_OTG_DFIFO(0); // считываем Setup stage done и отбрасываем его. 					
					}				
				} break;				
			}
		}
		else
		{
			switch (status) 
			{
				case 0x03: 
				/*<После того, как эта запись была извлечена из RxFIFO, ядро выставляет прерывание XFRC на указанной конечной точке OUT>*/
						//USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_OEPINT;
						if(epNum==1)
						{
							
							//USART_debug::usart2_sendSTR("BULK_OUT_COMPL \n");
							USB_OTG_OUT(1)->DOEPTSIZ = 0;
							USB_OTG_OUT(1)->DOEPTSIZ |= (1<<19)|(64<<0) ; //PKNT = 1 (DATA), макс размер пакета 64 байта
							USB_OTG_OUT(1)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
						}						
				break;
				case 0x04:  /* SETUP completed завершена транзакция SETUP (срабатывает прерывание). выставляется ACK*/
					{
						//Uart3::pThis->uart3_sendStr("SETUP Completed\n");
						 
						USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_OEPINT;
						USB_OTG_OUT(0)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);//запускаем конечную точку и разрешаем ее				
						//USB_OTG_IN(0)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
						// после этого необходимо заполнить Tx дескриптором устройства.
					}
				break;
			}			
		}		
		USB_OTG_FS-> GINTMSK |= USB_OTG_GINTMSK_RXFLVLM; //разрешаем генерацию прерывания наличия принятых данных в FIFO приема.		
	}
}