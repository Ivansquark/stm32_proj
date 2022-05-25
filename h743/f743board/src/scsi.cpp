#include "scsi.h"

bool SCSI::recieveCommandFlag=false;
bool SCSI::transiveFifoFlag=false;
bool SCSI::bulkFifoFlag=false;

SCSI* SCSI::pThis = nullptr;
SCSI::SCSI(){pThis = this;}

void SCSI::getCapacity() {
	volatile uint32_t BlockCount =  10000;//SD::pThis->SDCard.BlockCount - 1;
	uint32_t BlockSize = SD::pThis->SDCard.BlockSize;
	for(int i=0;i<4;i++) {
		capacity[i] = *((uint8_t*)(&BlockCount) + 3 - i);	
	}
	for(int i=0;i<4;i++) {
		capacity[i+4] = *((uint8_t*)(&BlockSize) + 3 - i);	
	}
	BlockLength = SD::pThis->SDCard.BlockSize;
	//capacity[3]--;
}

void SCSI::delay(uint32_t Delay) {
	while(Delay--);
}

/*! Ф-я обработки SCSI команд, должно быть прочитано FIFO и заполнена структура*/
void SCSI::SCSI_Execute(uint8_t ep_number) {
	getCapacity();
	uint32_t i, n;
    uint8_t j;	
	uint32_t temp;
	uint32_t timeout = 0;
	uint8_t buf[12] ={0};
	uint32_t nblocks = SD::pThis->SDCard.BlockCount - 1;//62521344 - 1;//488448;//
	uint32_t secsize = 512; //65535;
	/* приняли данные в EP1, Натягиваем scsi_cbw_t на приемный буфер*/
    scsi_cbw_t *cbw = (scsi_cbw_t *)USB_DEVICE::pThis->BULK_OUT_buf;
	//Если пакет успешно принят
	USB_DEVICE::pThis->resetFlag=cbw -> CBWCB[0];
	recieveCommandFlag=false;
	if (cbw->dCBWSignature==0x43425355) {
	//Сразу копируем значение dCBWTag в CSW.dCSWTag
        CSW.dCSWTag = (cbw -> dCBWTag);		
	//Определяем пришедшую команду	
	//if((cbw->CBWCB[0] != 40) && (cbw->CBWCB[0] != 37)) {
		
		uint32_t length = cbw->dCBWDataTransferLength; 		
	//}	
    	switch (cbw -> CBWCB[0]) {
		/*!Если INQUIRY*/		
    	case INQUIRY:
		//Проверка битов EVPD и CMDDT
        	if (cbw -> CBWCB[1] == 0) {				
				//Передаем стандартный ответ на INQUIRY
				timeout=0xFFFF;				
				while(transiveFifoFlag && timeout--);
				USB_DEVICE::pThis->WriteINEP(ep_number, inquiry, cbw -> CBWCB[4]);				
				//Заполняем поля CSW
        	    CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - cbw -> CBWCB[4];
				//Команда выполнилась успешно
        	    CSW.bCSWStatus = 0x00; 
				//Посылаем контейнер состояния
				timeout=0xFFFFF;
				while(transiveFifoFlag);// && timeout--);
				//for(uint32_t i=0;i<100000;i++);//pause to recieve ZLP
        	    USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13); //статус всегда ин пакет				
				delay(Delay);
			} else if (cbw -> CBWCB[1] == 1) {
				//Передаем стандартный ответ на INQUIRY	
				timeout=0xFFFFF;
				while(transiveFifoFlag && timeout--);			
				USB_DEVICE::pThis->WriteINEP(ep_number, MSC_Page80_Inquiry_Data, 8);				
				CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - 8;
				CSW.bCSWStatus = 0x00; 
				timeout=0xFFFFF;
				while(transiveFifoFlag);// && timeout--);
				for(uint32_t i=0;i<10000;i++);//pause to recieve ZLP
				USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13); //статус всегда ин пакет				
				delay(Delay);
			}
			else {
				timeout=0xFFFFF;
				while(transiveFifoFlag && timeout--);
        	    CSW.dCSWDataResidue = 0;(cbw -> dCBWDataTransferLength);
				//Сообщаем об ошибке выполнения команды
        	    CSW.bCSWStatus = 1;//0x01;
				//Посылаем контейнер состояния
				timeout=0xFFFFF;
				while(transiveFifoFlag);// && timeout--);
        	    USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
				//Подтверждаем
				delay(10000);
        	    CSW.bCSWStatus = 0x00;
				//Посылаем контейнер состояния
				timeout=0xFFFFF;
				while(transiveFifoFlag && timeout--);
        	    USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);				
				delay(Delay);
        	}
    	break;		
		//case REQUEST_SENSE:
		////USART_debug::usart2_sendSTR("\n REQUEST_SENSE \n");
		////Отправляем пояснительные данные
		//timeout=0xFFFFFF;
		//while(transiveFifoFlag && timeout--);
    	//USB_DEVICE::pThis->WriteINEP(ep_number, sense_data, 18);
		////Заполняем поля CSW
    	//CSW.dCSWDataResidue = 0;//(cbw -> dCBWDataTransferLength) - cbw -> CBWCB[4];
		////Команда выполнилась успешно
    	//CSW.bCSWStatus = 0x00;
		////Посылаем контейнер состояния
		//timeout=0xFFFFF;
		//while(transiveFifoFlag && timeout--);
    	//USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
		//delay(Delay);
    	//break;
//---------------------------------------------------------------------------------		
		case READ_CAPACITY_10:	
		//Сбросить все TXFIFO
		//USB_OTG_FS->GRSTCTL = USB_OTG_GRSTCTL_TXFFLSH | USB_OTG_GRSTCTL_TXFNUM;
		//while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH); //очищаем Tx_FIFO, которое почему то переполняется				
		//Передаем структуру		
		timeout=0xFFFFFFF;
		while(transiveFifoFlag && timeout--);
		USB_DEVICE::pThis->WriteINEP(ep_number, capacity, 8);
		//Заполняем и передаем CSW
    	CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - 8;// - cbw -> CBWCB[4];
    	CSW.bCSWStatus = 0x00;
		timeout=0xFFFFF;
		while(transiveFifoFlag);// && timeout--);
		//for(uint32_t i=0;i<1000;i++);//pause to recieve ZLP
    	USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);		
		DebugFlag++;
		//DebugCounter = i;
		//USB_DEVICE::pThis->counter = n;
		delay(Delay);
    	break;
//--------------------------------------------------------------
		case MODE_SENSE_6:		
		//USART_debug::usart2_sendSTR("\n MODE_SENSE_6 \n");
		timeout=0xFFFFFFFF;
		while(transiveFifoFlag && timeout--);
		USB_DEVICE::pThis->WriteINEP(ep_number, mode_sense_6, 4);
		//timeout=0xFFFF;
		//while(transiveFifoFlag && timeout--);
		//USB_DEVICE::pThis->WriteINEP(ep_number, MSC_Mode_Sense6_data, 4);
		CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - 4;
		CSW.bCSWStatus = 0x00;
		timeout=0xFFFFFFFF;
		while(transiveFifoFlag && timeout--);
		delay(10000);
		USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);		
		//DebugCounter1++;
		delay(Delay);
		break;
//-----------------------------------------------------------------------------------		
		case READ_10:		
			//записываем в I начальный адрес читаемого блока
			i = ((cbw -> CBWCB[2] << 24) | (cbw -> CBWCB[3] << 16) | (cbw -> CBWCB[4] << 8) | (cbw -> CBWCB[5]));
			temp = i;
			//записываем в n адрес последнего читаемого блока
			n = i + ((cbw -> CBWCB[7] << 8) | cbw -> CBWCB[8]);
			//выполняем чтение и передачу блоков		
			for ( ; i < n; i++) {
				//Читаем блок из SD, помещаем в массив uint8_t buf_512[2048]
				//Flash::pThis->read_buf(Flash::FLASH_PROGRAMM_ADDRESS+i*2048, buf_512, 2048);
				SD::pThis->ReadBlock(i,(uint32_t*)buf_512,BlockLength);
				//for(int i=0;i<(BlockLength<<2);i++) {
				//	*((uint32_t*)buf_512 + i) = __builtin_bswap32(*((uint32_t*)buf_512 + i));
				//}
				//Так как размер конечной точки 64 байта, передаем 512 байт за 8 раз
				uint32_t count=0;			
				for (j = 0; j < 8; j++) {	
					//Передаем часть буфера
					count++;
					timeout=0xFFFFFFFF;
					while(transiveFifoFlag && timeout--);
					USB_DEVICE::pThis->WriteINEP(ep_number, (buf_512+j*64), 64);
					//for(uint32_t i=0;i<1000;i++);//pause to recieve ZLP
					
				}			
			}
			CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - 512*(n-temp);
			//if(CSW.dCSWDataResidue != 0) {
				DebugCounter4 = CSW.dCSWDataResidue;
			//}
			CSW.bCSWStatus = 0x00;
			//USB_DEVICE::pThis->counter=USB_OTG_IN(2)->DTXFSTS;
			timeout=0xFFFFF;
			while(transiveFifoFlag);// && timeout--);		
			for(uint32_t i=0;i<100;i++);//pause to recieve ZLP in end of transaction
			USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);	
			//if(n!=1){
				
			//}	
			if(n == 68){
				DebugCounter3 = n;
				DebugCounter2 = temp;
				
			}	
			DebugCounter1 = length;									
			//for(uint32_t i=0;i<1000;i++);//pause to recieve ZLP
			USB_DEVICE::pThis->counter++;
			delay(Delay);
		break;
//---------------------------------------------------------------------------------		
		case WRITE_10:		
			//recieveDataFlag=true; // флаг о том что принимаем данные а не команду.
			//записываем в i начальный адрес записываемого блока
			i = ((cbw -> CBWCB[2] << 24) | (cbw -> CBWCB[3] << 16) | (cbw -> CBWCB[4] << 8) | (cbw -> CBWCB[5]));
			//записываем в n адрес последнего записываемого блока
			n = i + ((cbw -> CBWCB[7] << 8) | cbw -> CBWCB[8]);
			//выполняем чтение и запись блоков
			for(uint32_t z=0;z<20000;z++);//;ждем пока данные заполнят FIFO несколько раз				
			//делим по BlockLength байт
			for(uint8_t j=0; j<n; j++) {		
				// 512 bytes	
				for(uint32_t i1=0; i1<8; i1++) { 
					if(SCSI::bulkFifoFlag) {
						//USB_DEVICE::pThis->read_BULK_FIFO(64);
						//SCSI::bulkFifoFlag=false;
					}
					//for(uint32_t z=0;z<20000;z++);//;ждем пока данные заполнят FIFO несколько раз
					for(uint8_t k=0;k<64;k++) {
						if (!NormalQueue8::pThis->isEmpty()) {
							buf_512[64*i1+k] = NormalQueue8::pThis->pop();//заполнение массива 8 раз подряд
						}					
					}
					//USART_debug::usart2_sendSTR("\n WR_64 \n");							
				}	
				SD::pThis->WriteBlock(i+j,(uint32_t*)(buf_512),BlockLength); //write by 512 bytes					
				//Flash::pThis->write_any_buf(Flash::FLASH_PROGRAMM_ADDRESS+j*2048, buf_512, 2048); //записываем во флэш 2048 байт
			}		
			//Заполняем и посылаем CSW
			CSW.dCSWDataResidue = 0;//(cbw -> dCBWDataTransferLength ) - cbw -> CBWCB[4];
			CSW.bCSWStatus = 0x00;
			timeout=0xFFFFF;
			while(transiveFifoFlag && timeout--);
			//for(uint32_t i=0;i<1000;i++);//pause to recieve ZLP
			USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
			//USART_debug::usart2_sendSTR("\n WR_10 \n");
			//DebugCounter1++;
			delay(Delay);
		break;
//----------------------------------------------------------
		case TEST_UNIT_READY:
			//USART_debug::usart2_sendSTR("\n TEST_UNIT_READY \n");		
			CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength);
			CSW.bCSWStatus = 0x00;
			timeout=0xFFFFF;
			while(transiveFifoFlag && timeout--);
			for(uint32_t i=0;i<100;i++);//pause to recieve ZLP
			USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
			for(uint32_t i=0;i<100;i++);//pause to recieve ZLP			
			delay(Delay);
		break;
//-------------------------------------------------------------		
		//case PREVENT_ALLOW_MEDIUM_REMOVAL:
		//	//USART_debug::usart2_sendSTR("\n PREVENT_ALLOW_MEDIUM_REMOVAL \n");		
		//	CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength);
		//	CSW.bCSWStatus = 0x00;
		//	timeout=0xFFFFF;
		//	while(transiveFifoFlag && timeout--);
		//	USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);			
		//	delay(Delay);
		//break;
//------------------------------------------------------------------------		
		//case READ_FORMAT_CAPACITIES:			
		//	buf[0]  = buf[1] = buf[2] = 0;
    	//	buf[3]  = 8;
    	//	buf[4]  = (uint8_t)(nblocks >> 24);
    	//	buf[5]  = (uint8_t)(nblocks >> 16);
    	//	buf[6]  = (uint8_t)(nblocks >> 8);
    	//	buf[7]  = (uint8_t)(nblocks >> 0);
		//	buf[8]  = (uint8_t)(0x02);//scsi_read_format_capacities(&nblocks, &secsize);
    	//	buf[9]  = (uint8_t)(secsize >> 16);
    	//	buf[10] = (uint8_t)(secsize >> 8);
    	//	buf[11] = (uint8_t)(secsize >> 0);
    	//	USB_DEVICE::pThis->WriteINEP(ep_number, buf, 12);
		//	CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength) - 12;
		//	CSW.bCSWStatus = 0x00;
		//	timeout=0xFFFFF;
		//	while(transiveFifoFlag && timeout--);
		//	USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);			
		//	delay(Delay);
		//break;
//--------------------------------------------------------------		
		//Неизвестная команда  отвечаем ошибкой
    	default:
		//USART_debug::usart2_send(cbw -> CBWCB[0]);
		//USART_debug::usart2_sendSTR("\n SCSI_ERR \n");		
		//Заполняем поля CSW
    	    CSW.dCSWDataResidue = (cbw -> dCBWDataTransferLength);
		//Сообщаем об ошибке выполнения команды
    	    CSW.bCSWStatus = 0x01;
		//Посылаем контейнер состояния
			timeout=0xFFFFF;
			while(transiveFifoFlag && timeout--);
    	    USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
		//Подтверждаем
    	    CSW.bCSWStatus = 0x00;
		//Посылаем контейнер состояния
			timeout=0xFFFFF;
			while(transiveFifoFlag && timeout--);
    	    USB_DEVICE::pThis->WriteINEP(ep_number, (uint8_t *)&CSW, 13);
			delay(Delay);						
    	    break;	
    	}   
	}       
}