.PHONY: all,clean,load

#_________________________________________________________________________________________________
#_________________________ GCC compilator commands _______________________________________________	
#_________________________________________________________________________________________________	
CC = arm-none-eabi-gcc
OBJC = arm-none-eabi-objcopy
OBJD = arm-none-eabi-objdump
AS = arm-none-eabi-as
LD = arm-none-eabi-ld

INC = inc/
LIB = lib/
BLD = build/
SRC = src/
halSRC = hal/src/
halINC = hal/inc/
halLegacy = hal/inc/Legacy/ 
halBLD = build/hal/
usbINC = usb/inc/
usbSRC = usb/src/
usbBLD = build/usb/
ffsINC = ffs/inc/
ffsSRC = ffs/src/
ffsBLD = build/ffs/
#///////////////////////////////////
lwipINC = lwip/inc/
lwipSRC = lwip/src/
lwipSrcApi = lwip/src/api/
lwipSrcCore = lwip/src/core/
lwipSrcCoreIP4 = lwip/src/core/ipv4/
lwipSrcNetif = lwip/src/netif/
lwipSrcNetifPPP = lwip/src/netif/ppp/
lwipBLD = build/lwip/
lwipINCLUDE = lwip/src/include/
lwipSYSTEM = lwip/system/
#///////////////////////////////////
mrtSRC = MRT_proto/src/
mrtINC = MRT_proto/inc/
mrtBLD = build/MRT_proto/
#///////////////////////////////////
#FRS = freeRTOS/src/
#FRH = freeRTOS/inc/

CPPFLAGS = -mthumb -mcpu=cortex-m7 -c -g -O0 -Wall \
	-mfpu=fpv5-d16 -mfloat-abi=hard -fno-exceptions \
	-fno-math-errno -ffunction-sections -fdata-sections -u_printf_float \
	-fno-rtti --specs=nosys.specs --specs=nano.specs -fno-common -D"assert_param(x)="	

LCPPFLAGS = -mcpu=cortex-m7 -march=armv7e-m -mthumb -g -nostartfiles -lm -lc -lstdc++ \
		 -mfloat-abi=hard  -mfpu=fpv5-d16 -specs=nosys.specs -specs=nano.specs \
		 -fno-use-cxa-atexit -Xlinker -Map=$(BLD)main.map		 
#-z muldefs  -u_printf_float 

load: $(BLD)main.bin
	openocd -f lib/stlink.cfg -f lib/stm32h7x.cfg -c "program $(BLD)main.bin verify reset exit 0x08000000"
	
all: $(BLD)main.bin $(BLD)main.elf $(BLD)main.lst
$(BLD)main.bin: $(BLD)main.elf
	$(OBJC) $(BLD)main.elf $(BLD)main.bin -O binary

$(BLD)main.lst: $(BLD)main.elf
	$(OBJD) -D $(BLD)main.elf > $(BLD)main.lst

#------------------------------------  LINKER ------------------------------------------------------------------

$(BLD)main.elf: $(BLD)startup.o $(BLD)main.o $(BLD)timer.o 
$(BLD)main.elf: $(BLD)system_stm32h7xx.o $(BLD)malloc.o $(BLD)cache.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal.o $(halBLD)stm32h7xx_hal_cortex.o $(halBLD)stm32h7xx_hal_dma.o $(halBLD)stm32h7xx_hal_dma_ex.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_eth.o $(halBLD)stm32h7xx_hal_eth_ex.o $(halBLD)stm32h7xx_hal_fdcan.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_exti.o $(halBLD)stm32h7xx_hal_flash.o $(halBLD)stm32h7xx_hal_flash_ex.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_gpio.o $(halBLD)stm32h7xx_hal_hsem.o $(halBLD)stm32h7xx_hal_mdma.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_msp.o $(halBLD)stm32h7xx_hal_pcd.o $(halBLD)stm32h7xx_hal_pcd_ex.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_pwr.o $(halBLD)stm32h7xx_hal_pwr_ex.o $(halBLD)stm32h7xx_hal_rcc.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_rcc_ex.o $(halBLD)stm32h7xx_hal_tim.o $(halBLD)stm32h7xx_hal_tim_ex.o
$(BLD)main.elf: $(halBLD)stm32h7xx_it.o $(halBLD)stm32h7xx_ll_delayblock.o $(halBLD)stm32h7xx_ll_usb.o
$(BLD)main.elf: $(ffsBLD)diskio.o $(ffsBLD)pff.o
$(BLD)main.elf:	$(lwipBLD)ethernetif.o $(lwipBLD)lwip.o $(lwipBLD)memp.o $(lwipBLD)lan8742.o
$(BLD)main.elf:	$(lwipBLD)pbuf.o $(lwipBLD)etharp.o	$(lwipBLD)netif.o $(lwipBLD)init.o $(lwipBLD)mem.o	
$(BLD)main.elf: $(lwipBLD)def.o $(lwipBLD)tcp.o $(lwipBLD)tcp_out.o $(lwipBLD)ip4.o $(lwipBLD)ip4_frag.o
$(BLD)main.elf: $(lwipBLD)icmp.o $(lwipBLD)inet_chksum.o $(lwipBLD)ip4_addr.o $(lwipBLD)udp.o $(lwipBLD)ip.o
$(BLD)main.elf: $(lwipBLD)tcp_in.o $(lwipBLD)timeouts.o $(lwipBLD)ethernet.o $(lwipBLD)err.o
$(BLD)main.elf: $(BLD)net.o $(mrtBLD)mrt_proto.o $(mrtBLD)mrt_proto_slots.o
	$(CC) -o $(BLD)main.elf -T$(LIB)stm32h743.ld \
	$(BLD)startup.o $(BLD)timer.o $(BLD)main.o \
	$(BLD)malloc.o $(BLD)cache.o \
	$(BLD)system_stm32h7xx.o \
	$(halBLD)stm32h7xx_hal.o $(halBLD)stm32h7xx_hal_cortex.o $(halBLD)stm32h7xx_hal_dma.o $(halBLD)stm32h7xx_hal_dma_ex.o \
	$(halBLD)stm32h7xx_hal_eth.o   	$(halBLD)stm32h7xx_hal_eth_ex.o  	$(halBLD)stm32h7xx_hal_fdcan.o 		\
	$(halBLD)stm32h7xx_hal_exti.o   $(halBLD)stm32h7xx_hal_flash.o  	$(halBLD)stm32h7xx_hal_flash_ex.o 	\
	$(halBLD)stm32h7xx_hal_gpio.o   $(halBLD)stm32h7xx_hal_hsem.o   	$(halBLD)stm32h7xx_hal_mdma.o 		\
	$(halBLD)stm32h7xx_hal_msp.o    $(halBLD)stm32h7xx_hal_pcd.o    	$(halBLD)stm32h7xx_hal_pcd_ex.o 	\
	$(halBLD)stm32h7xx_hal_pwr.o    $(halBLD)stm32h7xx_hal_pwr_ex.o 	$(halBLD)stm32h7xx_hal_rcc.o 		\
	$(halBLD)stm32h7xx_hal_rcc_ex.o $(halBLD)stm32h7xx_hal_tim.o    	$(halBLD)stm32h7xx_hal_tim_ex.o 	\
	$(halBLD)stm32h7xx_it.o 		$(halBLD)stm32h7xx_ll_delayblock.o  $(halBLD)stm32h7xx_ll_usb.o 		\
	$(ffsBLD)diskio.o 			$(ffsBLD)pff.o 				 							\
	$(lwipBLD)ethernetif.o	$(lwipBLD)lwip.o $(lwipBLD)memp.o $(lwipBLD)lan8742.o $(lwipBLD)pbuf.o \
	$(lwipBLD)etharp.o $(lwipBLD)netif.o $(lwipBLD)init.o $(lwipBLD)mem.o $(lwipBLD)def.o $(lwipBLD)tcp.o \
	$(lwipBLD)tcp_out.o $(lwipBLD)ip4.o $(lwipBLD)ip4_frag.o $(lwipBLD)icmp.o $(lwipBLD)inet_chksum.o \
	$(lwipBLD)ip4_addr.o $(lwipBLD)udp.o $(lwipBLD)ip.o $(lwipBLD)tcp_in.o $(lwipBLD)timeouts.o \
	$(lwipBLD)ethernet.o $(lwipBLD)err.o $(BLD)net.o $(mrtBLD)mrt_proto.o $(mrtBLD)mrt_proto_slots.o \
	-I$(LIB) $(LCPPFLAGS)

#-------------------------------------- HEX -------------------------------------------------------	
	arm-none-eabi-objcopy -j .text -j .data -j .bss -O ihex $(BLD)main.elf $(BLD)main.hex  
	@echo "*******************************************************"
	@echo "*******************   SIZE   **************************"
	@echo "*******************************************************"
	arm-none-eabi-size $(BLD)main.elf
	@echo "*******************************************************"

#______________________________________________________________________________________________
#______________________ SYSTEM FUNCTIONS_______________________________________________________
#______________________________________________________________________________________________
$(BLD)startup.o: $(LIB)startup.cpp
	$(CC) $(LIB)startup.cpp -o $(BLD)startup.o $(CPPFLAGS)
$(BLD)system_stm32h7xx.o: $(LIB)system_stm32h7xx.cpp
	$(CC) $(LIB)system_stm32h7xx.cpp -o $(BLD)system_stm32h7xx.o $(CPPFLAGS)	
$(BLD)malloc.o: $(SRC)malloc.cpp
	$(CC) $(SRC)malloc.cpp -o $(BLD)malloc.o -I$(INC) -I$(FRH) $(CPPFLAGS)
$(BLD)cache.o: $(SRC)cache.cpp
	$(CC) $(SRC)cache.cpp -o $(BLD)cache.o -I$(INC) -I$(LIB) -I$(halINC) $(CPPFLAGS)	


#_________________________________________________________________________________________________
#_________________________ Basic functions _______________________________________________________	
#_________________________________________________________________________________________________
$(BLD)timer.o: $(SRC)timer.cpp #$(INC) #$(FRH)
	$(CC) $(SRC)timer.cpp -o $(BLD)timer.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)	
		
#_________________________________________________________________________________________________
#_________________________ FAT_FS Library ver 0.14a ______________________________________________	
#_________________________________________________________________________________________________
$(ffsBLD)diskio.o: $(ffsSRC)diskio.cpp 
	$(CC) $(ffsSRC)diskio.cpp -o $(ffsBLD)diskio.o -I$(INC) -I$(ffsINC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(ffsBLD)pff.o: $(ffsSRC)pff.cpp 
	$(CC) $(ffsSRC)pff.cpp -o $(ffsBLD)pff.o -I$(INC) -I$(ffsINC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
#_________________________________________________________________________________________________
#_________________________ USB Mass Storage Device LIBRARY _______________________________________	
#_________________________________________________________________________________________________
# $(usbBLD)usb_device.o: $(usbSRC)usb_device.cpp #$(INC) #$(FRH)
# 	$(CC) $(usbSRC)usb_device.cpp -o $(usbBLD)usb_device.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
# $(usbBLD)usbd_conf.o: $(usbSRC)usbd_conf.cpp #$(INC) #$(FRH)
# 	$(CC) $(usbSRC)usbd_conf.cpp -o $(usbBLD)usbd_conf.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
# $(usbBLD)usbd_core.o: $(usbSRC)usbd_core.cpp #$(INC) #$(FRH)
# 	$(CC) $(usbSRC)usbd_core.cpp -o $(usbBLD)usbd_core.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
# $(usbBLD)usbd_ctlreq.o: $(usbSRC)usbd_ctlreq.cpp #$(INC) #$(FRH)
# 	$(CC) $(usbSRC)usbd_ctlreq.cpp -o $(usbBLD)usbd_ctlreq.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
# $(usbBLD)usbd_desc.o: $(usbSRC)usbd_desc.cpp #$(INC) #$(FRH)
# 	$(CC) $(usbSRC)usbd_desc.cpp -o $(usbBLD)usbd_desc.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
# $(usbBLD)usbd_ioreq.o: $(usbSRC)usbd_ioreq.cpp #$(INC) #$(FRH)
# 	$(CC) $(usbSRC)usbd_ioreq.cpp -o $(usbBLD)usbd_ioreq.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
# $(usbBLD)usbd_msc.o: $(usbSRC)usbd_msc.cpp #$(INC) #$(FRH)
# 	$(CC) $(usbSRC)usbd_msc.cpp -o $(usbBLD)usbd_msc.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
# $(usbBLD)usbd_msc_bot.o: $(usbSRC)usbd_msc_bot.cpp #$(INC) #$(FRH)
# 	$(CC) $(usbSRC)usbd_msc_bot.cpp -o $(usbBLD)usbd_msc_bot.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
# $(usbBLD)usbd_msc_data.o: $(usbSRC)usbd_msc_data.cpp #$(INC) #$(FRH)
# 	$(CC) $(usbSRC)usbd_msc_data.cpp -o $(usbBLD)usbd_msc_data.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
# $(usbBLD)usbd_msc_scsi.o: $(usbSRC)usbd_msc_scsi.cpp #$(INC) #$(FRH)
# 	$(CC) $(usbSRC)usbd_msc_scsi.cpp -o $(usbBLD)usbd_msc_scsi.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
# $(usbBLD)usbd_storage_if.o: $(usbSRC)usbd_storage_if.cpp #$(INC) #$(FRH)
# 	$(CC) $(usbSRC)usbd_storage_if.cpp -o $(usbBLD)usbd_storage_if.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)

#_________________________________________________________________________________________________							
#__________________________ HAL LIBRARY __________________________________________________________
#_________________________________________________________________________________________________
$(halBLD)stm32h7xx_hal.o: $(halSRC)stm32h7xx_hal.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal.cpp -o $(halBLD)stm32h7xx_hal.o -I$(INC) -I$(LIB) -I$(halINC) -I$(halLegacy) $(CPPFLAGS)
$(halBLD)stm32h7xx_hal_cortex.o: $(halSRC)stm32h7xx_hal_cortex.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_cortex.cpp -o $(halBLD)stm32h7xx_hal_cortex.o -I$(INC) -I$(LIB) -I$(halINC) -I$(halLegacy) $(CPPFLAGS)
$(halBLD)stm32h7xx_hal_dma.o: $(halSRC)stm32h7xx_hal_dma.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_dma.cpp -o $(halBLD)stm32h7xx_hal_dma.o -I$(INC) -I$(LIB) -I$(halINC) -I$(halLegacy) $(CPPFLAGS)
$(halBLD)stm32h7xx_hal_dma_ex.o: $(halSRC)stm32h7xx_hal_dma_ex.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_dma_ex.cpp -o $(halBLD)stm32h7xx_hal_dma_ex.o -I$(INC) -I$(LIB) -I$(halINC) -I$(halLegacy) $(CPPFLAGS)	
$(halBLD)stm32h7xx_hal_eth.o: $(halSRC)stm32h7xx_hal_eth.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_eth.cpp -o $(halBLD)stm32h7xx_hal_eth.o -I$(INC) -I$(LIB) -I$(halINC) -I$(halLegacy) $(CPPFLAGS)	
$(halBLD)stm32h7xx_hal_eth_ex.o: $(halSRC)stm32h7xx_hal_eth_ex.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_eth_ex.cpp -o $(halBLD)stm32h7xx_hal_eth_ex.o -I$(INC) -I$(LIB) -I$(halINC) -I$(halLegacy) $(CPPFLAGS)	
$(halBLD)stm32h7xx_hal_exti.o: $(halSRC)stm32h7xx_hal_exti.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_exti.cpp -o $(halBLD)stm32h7xx_hal_exti.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(halBLD)stm32h7xx_hal_fdcan.o: $(halSRC)stm32h7xx_hal_fdcan.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_fdcan.cpp -o $(halBLD)stm32h7xx_hal_fdcan.o -I$(INC) -I$(LIB) -I$(halINC) -I$(halLegacy) $(CPPFLAGS)
$(halBLD)stm32h7xx_hal_flash.o: $(halSRC)stm32h7xx_hal_flash.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_flash.cpp -o $(halBLD)stm32h7xx_hal_flash.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(halBLD)stm32h7xx_hal_flash_ex.o: $(halSRC)stm32h7xx_hal_flash_ex.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_flash_ex.cpp -o $(halBLD)stm32h7xx_hal_flash_ex.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(halBLD)stm32h7xx_hal_gpio.o: $(halSRC)stm32h7xx_hal_gpio.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_gpio.cpp -o $(halBLD)stm32h7xx_hal_gpio.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(halBLD)stm32h7xx_hal_hsem.o: $(halSRC)stm32h7xx_hal_hsem.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_hsem.cpp -o $(halBLD)stm32h7xx_hal_hsem.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)		
$(halBLD)stm32h7xx_hal_mdma.o: $(halSRC)stm32h7xx_hal_mdma.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_mdma.cpp -o $(halBLD)stm32h7xx_hal_mdma.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(halBLD)stm32h7xx_hal_msp.o: $(halSRC)stm32h7xx_hal_msp.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_msp.cpp -o $(halBLD)stm32h7xx_hal_msp.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(halBLD)stm32h7xx_hal_pcd.o: $(halSRC)stm32h7xx_hal_pcd.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_pcd.cpp -o $(halBLD)stm32h7xx_hal_pcd.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(halBLD)stm32h7xx_hal_pcd_ex.o: $(halSRC)stm32h7xx_hal_pcd_ex.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_pcd_ex.cpp -o $(halBLD)stm32h7xx_hal_pcd_ex.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(halBLD)stm32h7xx_hal_pwr.o: $(halSRC)stm32h7xx_hal_pwr.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_pwr.cpp -o $(halBLD)stm32h7xx_hal_pwr.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)	
$(halBLD)stm32h7xx_hal_pwr_ex.o: $(halSRC)stm32h7xx_hal_pwr_ex.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_pwr_ex.cpp -o $(halBLD)stm32h7xx_hal_pwr_ex.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(halBLD)stm32h7xx_hal_rcc.o: $(halSRC)stm32h7xx_hal_rcc.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_rcc.cpp -o $(halBLD)stm32h7xx_hal_rcc.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(halBLD)stm32h7xx_hal_rcc_ex.o: $(halSRC)stm32h7xx_hal_rcc_ex.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_rcc_ex.cpp -o $(halBLD)stm32h7xx_hal_rcc_ex.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)	
$(halBLD)stm32h7xx_hal_tim.o: $(halSRC)stm32h7xx_hal_tim.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_tim.cpp -o $(halBLD)stm32h7xx_hal_tim.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(halBLD)stm32h7xx_hal_tim_ex.o: $(halSRC)stm32h7xx_hal_tim_ex.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_tim_ex.cpp -o $(halBLD)stm32h7xx_hal_tim_ex.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(halBLD)stm32h7xx_it.o: $(halSRC)stm32h7xx_it.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_it.cpp -o $(halBLD)stm32h7xx_it.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(halBLD)stm32h7xx_ll_delayblock.o: $(halSRC)stm32h7xx_ll_delayblock.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_ll_delayblock.cpp -o $(halBLD)stm32h7xx_ll_delayblock.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(halBLD)stm32h7xx_ll_usb.o: $(halSRC)stm32h7xx_ll_usb.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_ll_usb.cpp -o $(halBLD)stm32h7xx_ll_usb.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)	
#_________________________________________________________________________________________________________________________
#_______________________________	LWIP	______________________________________________________________________________
#_________________________________________________________________________________________________________________________
$(lwipBLD)lan8742.o: $(SRC)lan8742.cpp
	$(CC) $(SRC)lan8742.cpp -o $(lwipBLD)lan8742.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) $(CPPFLAGS)
$(lwipBLD)ethernetif.o: $(lwipSRC)ethernetif.cpp
	$(CC) $(lwipSRC)ethernetif.cpp -o $(lwipBLD)ethernetif.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) -I$(lwipINC) $(CPPFLAGS)	
#	$(OBJD) -D	$(lwipBLD)ethernetif.o > $(lwipBLD)ethernetif.lst
$(lwipBLD)lwip.o: $(lwipSRC)lwip.cpp
	$(CC) $(lwipSRC)lwip.cpp -o $(lwipBLD)lwip.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) -I$(lwipINC) $(CPPFLAGS)	
$(lwipBLD)memp.o: $(lwipSrcCore)memp.cpp
	$(CC) $(lwipSrcCore)memp.cpp -o $(lwipBLD)memp.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) -I$(lwipINC) $(CPPFLAGS)	
$(lwipBLD)pbuf.o: $(lwipSrcCore)pbuf.cpp
	$(CC) $(lwipSrcCore)pbuf.cpp -o $(lwipBLD)pbuf.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)	
$(lwipBLD)netif.o: $(lwipSrcCore)netif.cpp
	$(CC) $(lwipSrcCore)netif.cpp -o $(lwipBLD)netif.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)	
$(lwipBLD)init.o: $(lwipSrcCore)init.cpp
	$(CC) $(lwipSrcCore)init.cpp -o $(lwipBLD)init.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)	
$(lwipBLD)mem.o: $(lwipSrcCore)mem.cpp
	$(CC) $(lwipSrcCore)mem.cpp -o $(lwipBLD)mem.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)
$(lwipBLD)def.o: $(lwipSrcCore)def.cpp
	$(CC) $(lwipSrcCore)def.cpp -o $(lwipBLD)def.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)	
$(lwipBLD)tcp.o: $(lwipSrcCore)tcp.cpp
	$(CC) $(lwipSrcCore)tcp.cpp -o $(lwipBLD)tcp.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)
$(lwipBLD)tcp_out.o: $(lwipSrcCore)tcp_out.cpp
	$(CC) $(lwipSrcCore)tcp_out.cpp -o $(lwipBLD)tcp_out.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)
$(lwipBLD)tcp_in.o: $(lwipSrcCore)tcp_in.cpp
	$(CC) $(lwipSrcCore)tcp_in.cpp -o $(lwipBLD)tcp_in.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)		
$(lwipBLD)inet_chksum.o: $(lwipSrcCore)inet_chksum.cpp
	$(CC) $(lwipSrcCore)inet_chksum.cpp -o $(lwipBLD)inet_chksum.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)
$(lwipBLD)udp.o: $(lwipSrcCore)udp.cpp
	$(CC) $(lwipSrcCore)udp.cpp -o $(lwipBLD)udp.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)	
$(lwipBLD)ip.o: $(lwipSrcCore)ip.cpp
	$(CC) $(lwipSrcCore)ip.cpp -o $(lwipBLD)ip.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)
$(lwipBLD)timeouts.o: $(lwipSrcCore)timeouts.cpp
	$(CC) $(lwipSrcCore)timeouts.cpp -o $(lwipBLD)timeouts.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)	

$(lwipBLD)etharp.o: $(lwipSrcCoreIP4)etharp.cpp
	$(CC) $(lwipSrcCoreIP4)etharp.cpp -o $(lwipBLD)etharp.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)
$(lwipBLD)ip4.o: $(lwipSrcCoreIP4)ip4.cpp
	$(CC) $(lwipSrcCoreIP4)ip4.cpp -o $(lwipBLD)ip4.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)	
$(lwipBLD)ip4_frag.o: $(lwipSrcCoreIP4)ip4_frag.cpp
	$(CC) $(lwipSrcCoreIP4)ip4_frag.cpp -o $(lwipBLD)ip4_frag.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)	
$(lwipBLD)icmp.o: $(lwipSrcCoreIP4)icmp.cpp
	$(CC) $(lwipSrcCoreIP4)icmp.cpp -o $(lwipBLD)icmp.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)	
$(lwipBLD)ip4_addr.o: $(lwipSrcCoreIP4)ip4_addr.cpp
	$(CC) $(lwipSrcCoreIP4)ip4_addr.cpp -o $(lwipBLD)ip4_addr.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)
$(lwipBLD)autoip.o: $(lwipSrcCoreIP4)autoip.cpp
	$(CC) $(lwipSrcCoreIP4)autoip.cpp -o $(lwipBLD)autoip.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)			

$(lwipBLD)api_lib.o: $(lwipSrcApi)api_lib.cpp
	$(CC) $(lwipSrcApi)api_lib.cpp -o $(lwipBLD)api_lib.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)
	
$(lwipBLD)err.o: $(lwipSrcApi)err.cpp
	$(CC) $(lwipSrcApi)err.cpp -o $(lwipBLD)err.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)

$(lwipBLD)ethernet.o: $(lwipSrcNetif)ethernet.cpp
	$(CC) $(lwipSrcNetif)ethernet.cpp -o $(lwipBLD)ethernet.o -I$(INC) -I$(LIB) -I$(halINC) -I$(lwipINC) \
	-I$(lwipSYSTEM) -I$(lwipINCLUDE) $(CPPFLAGS)

$(BLD)net.o: $(SRC)net.cpp 
	$(CC) $(SRC)net.cpp -o $(BLD)net.o -I$(halINC) -I$(INC) -I$(usbINC) -I$(halINC) -I$(LIB) -I$(ffsINC) \
	-I$(lwipINCLUDE) -I$(lwipSYSTEM) -I$(lwipINC) -I$(mrtINC) $(CPPFLAGS) 

#-----------------------------------------------------------------------------------------------------
#--------------------------------------- MRT PROTOCOL-------------------------------------------------
#-----------------------------------------------------------------------------------------------------
$(mrtBLD)mrt_proto.o: $(mrtSRC)mrt_proto.cpp 
	$(CC) $(mrtSRC)mrt_proto.cpp -o $(mrtBLD)mrt_proto.o -I$(halINC) -I$(INC) -I$(usbINC) -I$(halINC) -I$(LIB) \
	-I$(mrtINC) -I$(ffsINC) -I$(lwipINCLUDE) -I$(lwipSYSTEM) -I$(lwipINC) $(CPPFLAGS)
$(mrtBLD)mrt_proto_slots.o: $(mrtSRC)mrt_proto_slots.cpp 
	$(CC) $(mrtSRC)mrt_proto_slots.cpp -o $(mrtBLD)mrt_proto_slots.o -I$(halINC) -I$(INC) -I$(usbINC) -I$(halINC) \
	-I$(LIB) -I$(mrtINC) -I$(ffsINC) -I$(lwipINCLUDE) -I$(lwipSYSTEM) -I$(lwipINC) $(CPPFLAGS)	

#-----------------------------------------------------------------------------------------------------
#--------------------------------------- MAIN --------------------------------------------------------
#-----------------------------------------------------------------------------------------------------
$(BLD)main.o: $(SRC)main.cpp 
	$(CC) $(SRC)main.cpp -o $(BLD)main.o -I$(halINC) -I$(INC) -I$(usbINC) -I$(halINC) -I$(LIB) -I$(ffsINC) \
	-I$(lwipINCLUDE) -I$(mrtINC) -I$(lwipSYSTEM) -I$(lwipINC) $(CPPFLAGS) 
	
clean:
	rm -rf build 
	mkdir $(BLD) $(halBLD) $(ffsBLD) $(lwipBLD) $(mrtBLD)

	
