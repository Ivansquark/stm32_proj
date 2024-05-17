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
#FRS = freeRTOS/src/
#FRH = freeRTOS/inc/

CPPFLAGS = -mthumb -mcpu=cortex-m7 -c -g -O0 -Wall \
	-mfpu=fpv5-d16 -mfloat-abi=hard -fno-exceptions \
	-fno-math-errno -ffunction-sections -fdata-sections -u_printf_float \
	-fno-rtti --specs=nosys.specs --specs=nano.specs -fno-common -D"assert_param(x)=" \
	-L"C:\Program Files (x86)\GNUArmEmbeddedToolchain\9_2020-q2-update\lib\gcc\arm-none-eabi\9.3.1\thumb\v7e-m+dp\hard"

CFLAGS = -mthumb -mcpu=cortex-m7 -c -g -O0 -Wall \
	-mfpu=fpv5-d16 -mfloat-abi=hard \
	-fno-math-errno -ffunction-sections -fdata-sections -u_printf_float \
	--specs=nosys.specs --specs=nano.specs -fno-common -D"assert_param(x)=" \
	-L"C:\Program Files (x86)\GNUArmEmbeddedToolchain\9_2020-q2-update\lib\gcc\arm-none-eabi\9.3.1\thumb\v7e-m+dp\hard"	 

LCPPFLAGS = -mcpu=cortex-m7 -march=armv7e-m -mthumb -g -nostartfiles -lm -lc -lstdc++ \
		 -mfloat-abi=hard  -mfpu=fpv5-d16 -specs=nosys.specs -specs=nano.specs \
		 -fno-use-cxa-atexit -Xlinker -Map=$(BLD)main.map \
		 -L"C:\Program Files (x86)\GNUArmEmbeddedToolchain\9_2020-q2-update\arm-none-eabi\lib\thumb\v7e-m+dp\hard" \
		 -L"C:\Program Files (x86)\GNUArmEmbeddedToolchain\9_2020-q2-update\lib\gcc\arm-none-eabi\9.3.1\thumb\v7e-m+dp\hard"
		 #-L"C:/Program Files (x86)/GNUArmEmbeddedToolchain/9_2020-q2-update/arm-none-eabi/" \
		 -L"C:\Program Files (x86)\GNUArmEmbeddedToolchain\9_2020-q2-update\lib\gcc\arm-none-eabi\9.3.1" \
		 -L"C:\Program Files (x86)\GNUArmEmbeddedToolchain\9_2020-q2-update\arm-none-eabi\lib" \
		 #-z muldefs  -u_printf_float 

load: $(BLD)main.bin
	openocd -f lib/stlink.cfg -f lib/stm32h7x.cfg -s /usr/share/openocd/scripts -c "program $(BLD)main.bin verify reset exit 0x08000000"
	
all: $(BLD)main.bin $(BLD)main.elf $(BLD)main.lst
$(BLD)main.bin: $(BLD)main.elf
	$(OBJC) $(BLD)main.elf $(BLD)main.bin -O binary

$(BLD)main.lst: $(BLD)main.elf
	$(OBJD) -D $(BLD)main.elf > $(BLD)main.lst

#------------------------------------  LINKER ------------------------------------------------------------------

$(BLD)main.elf: $(BLD)startup.o $(BLD)main.o $(BLD)timer.o $(BLD)encoder.o 			$(BLD)rcc_init.o 	$(BLD)normalqueue.o
$(BLD)main.elf: $(BLD)pardac.o  $(BLD)fmc.o  $(BLD)font.o  $(BLD)F_28x30_Digit.o 	$(BLD)rtc.o 		$(BLD)sd.o
$(BLD)main.elf: $(BLD)system_stm32h7xx.o $(BLD)parser.o $(BLD)hrtim.o $(BLD)malloc.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal.o $(halBLD)stm32h7xx_hal_cortex.o $(halBLD)stm32h7xx_hal_dma.o $(halBLD)stm32h7xx_hal_dma_ex.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_exti.o $(halBLD)stm32h7xx_hal_flash.o $(halBLD)stm32h7xx_hal_flash_ex.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_gpio.o $(halBLD)stm32h7xx_hal_hsem.o $(halBLD)stm32h7xx_hal_mdma.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_msp.o $(halBLD)stm32h7xx_hal_pcd.o $(halBLD)stm32h7xx_hal_pcd_ex.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_pwr.o $(halBLD)stm32h7xx_hal_pwr_ex.o $(halBLD)stm32h7xx_hal_rcc.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_rcc_ex.o $(halBLD)stm32h7xx_hal_tim.o $(halBLD)stm32h7xx_hal_tim_ex.o
$(BLD)main.elf: $(halBLD)stm32h7xx_it.o $(halBLD)stm32h7xx_ll_delayblock.o $(halBLD)stm32h7xx_ll_usb.o
$(BLD)main.elf: $(usbBLD)usb_device.o $(usbBLD)usbd_conf.o $(usbBLD)usbd_core.o $(usbBLD)usbd_ctlreq.o
$(BLD)main.elf: $(usbBLD)usbd_desc.o $(usbBLD)usbd_ioreq.o $(usbBLD)usbd_msc.o $(usbBLD)usbd_msc_bot.o
$(BLD)main.elf: $(usbBLD)usbd_msc_data.o $(usbBLD)usbd_msc_scsi.o $(usbBLD)usbd_storage_if.o
$(BLD)main.elf: $(ffsBLD)diskio.o 		 $(ffsBLD)ff.o 			  $(ffsBLD)ffsystem.o
	$(CC) -o $(BLD)main.elf -T$(LIB)stm32h743.ld \
	$(BLD)startup.o $(BLD)timer.o $(BLD)encoder.o $(BLD)main.o $(BLD)rcc_init.o \
	$(BLD)pardac.o $(BLD)fmc.o $(BLD)font.o $(BLD)F_28x30_Digit.o $(BLD)rtc.o $(BLD)sd.o \
	$(BLD)normalqueue.o $(BLD)parser.o $(BLD)hrtim.o $(BLD)malloc.o \
	$(BLD)system_stm32h7xx.o \
	$(halBLD)stm32h7xx_hal.o $(halBLD)stm32h7xx_hal_cortex.o $(halBLD)stm32h7xx_hal_dma.o $(halBLD)stm32h7xx_hal_dma_ex.o \
	$(halBLD)stm32h7xx_hal_exti.o   $(halBLD)stm32h7xx_hal_flash.o  	$(halBLD)stm32h7xx_hal_flash_ex.o 	\
	$(halBLD)stm32h7xx_hal_gpio.o   $(halBLD)stm32h7xx_hal_hsem.o   	$(halBLD)stm32h7xx_hal_mdma.o 		\
	$(halBLD)stm32h7xx_hal_msp.o    $(halBLD)stm32h7xx_hal_pcd.o    	$(halBLD)stm32h7xx_hal_pcd_ex.o 	\
	$(halBLD)stm32h7xx_hal_pwr.o    $(halBLD)stm32h7xx_hal_pwr_ex.o 	$(halBLD)stm32h7xx_hal_rcc.o 		\
	$(halBLD)stm32h7xx_hal_rcc_ex.o $(halBLD)stm32h7xx_hal_tim.o    	$(halBLD)stm32h7xx_hal_tim_ex.o 	\
	$(halBLD)stm32h7xx_it.o 		$(halBLD)stm32h7xx_ll_delayblock.o  $(halBLD)stm32h7xx_ll_usb.o 		\
	$(usbBLD)usb_device.o 		$(usbBLD)usbd_conf.o 		$(usbBLD)usbd_core.o 	$(usbBLD)usbd_ctlreq.o 	\
	$(usbBLD)usbd_desc.o 		$(usbBLD)usbd_ioreq.o 		$(usbBLD)usbd_msc.o 	$(usbBLD)usbd_msc_bot.o \
	$(usbBLD)usbd_msc_data.o 	$(usbBLD)usbd_msc_scsi.o 	$(usbBLD)usbd_storage_if.o 						\
	$(ffsBLD)diskio.o 			$(ffsBLD)ff.o 				$(ffsBLD)ffsystem.o 							\
	-I$(LIB) $(LCPPFLAGS)

# malloc.o tasks.o heap_2.o timers.o list.o port.o queue.o \
#-------------------------------------- HEX -------------------------------------------------------	
	arm-none-eabi-objcopy -j .text -j .data -j .bss -O ihex $(BLD)main.elf $(BLD)main.hex  
	arm-none-eabi-size $(BLD)main.elf

#______________________________________________________________________________________________
#______________________ SYSTEM FUNCTIONS_______________________________________________________
#______________________________________________________________________________________________
$(BLD)startup.o: $(LIB)startup.cpp
	$(CC) $(LIB)startup.cpp -o $(BLD)startup.o $(CPPFLAGS)
$(BLD)system_stm32h7xx.o: $(LIB)system_stm32h7xx.cpp
	$(CC) $(LIB)system_stm32h7xx.cpp -o $(BLD)system_stm32h7xx.o $(CPPFLAGS)	
$(BLD)malloc.o: $(SRC)malloc.cpp
	$(CC) $(SRC)malloc.cpp -o $(BLD)malloc.o -I$(INC) -I$(FRH) $(CPPFLAGS)
#_________________________________________________________________________________________________
#_________________________ FreeRtos  _____________________________________________________________	
#_________________________________________________________________________________________________	
#port.o: freeRTOS/src/port.c 
#	$(CC) freeRTOS/src/port.c -o port.o -I$(FRH) -I$(INC) $(CFLAGS)
#tasks.o: freeRTOS/src/tasks.c
#	$(CC) freeRTOS/src/tasks.c -o tasks.o -I$(FRH) -I$(INC) $(CFLAGS)
#queue.o: freeRTOS/src/queue.c
#	$(CC) freeRTOS/src/queue.c -o queue.o -I$(FRH) -I$(INC) $(CFLAGS)	
#list.o: freeRTOS/src/list.c
#	$(CC) freeRTOS/src/list.c -o list.o -I$(FRH) -I$(INC) $(CFLAGS)	
#timers.o: freeRTOS/src/timers.c
#	$(CC) freeRTOS/src/timers.c -o timers.o -I$(FRH) -I$(INC) $(CFLAGS)	
#heap_2.o: freeRTOS/src/heap_2.c $(INC)
#	$(CC) freeRTOS/src/heap_2.c -o heap_2.o -I$(FRH) -I$(INC) $(CFLAGS)	
#_________________________________________________________________________________________________
#_________________________ Basic functions _______________________________________________________	
#_________________________________________________________________________________________________
$(BLD)timer.o: $(SRC)timer.cpp #$(INC) #$(FRH)
	$(CC) $(SRC)timer.cpp -o $(BLD)timer.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
$(BLD)rcc_init.o: $(SRC)rcc_init.cpp #$(INC) #$(FRH)
	$(CC) $(SRC)rcc_init.cpp -o $(BLD)rcc_init.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
$(BLD)encoder.o: $(SRC)encoder.cpp #$(INC) #$(FRH)
	$(CC) $(SRC)encoder.cpp -o $(BLD)encoder.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
$(BLD)pardac.o: $(SRC)pardac.cpp #$(INC) #$(FRH)
	$(CC) $(SRC)pardac.cpp -o $(BLD)pardac.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
$(BLD)fmc.o: $(SRC)fmc.cpp #$(INC) #$(FRH)
	$(CC) $(SRC)fmc.cpp -o $(BLD)fmc.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
$(BLD)rtc.o: $(SRC)rtc.cpp #$(INC) #$(FRH)
	$(CC) $(SRC)rtc.cpp -o $(BLD)rtc.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)	
#$(BLD)lcd_par.o: $(SRC)lcd_par.cpp #$(INC) #$(FRH)
#	$(CC) $(SRC)lcd_par.cpp -o $(BLD)lcd_par.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)	
$(BLD)font.o: $(SRC)font.cpp #$(INC) #$(FRH)
	$(CC) $(SRC)font.cpp -o $(BLD)font.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
$(BLD)F_28x30_Digit.o: $(SRC)F_28x30_Digit.cpp #$(INC) #$(FRH)
	$(CC) $(SRC)F_28x30_Digit.cpp -o $(BLD)F_28x30_Digit.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
$(BLD)sd.o: $(SRC)sd.cpp #$(INC) #$(FRH)
	$(CC) $(SRC)sd.cpp -o $(BLD)sd.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
#$(BLD)usb_device.o: $(SRC)usb_device.cpp #$(INC) #$(FRH)
#	$(CC) $(SRC)usb_device.cpp -o $(BLD)usb_device.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
#$(BLD)scsi.o: $(SRC)scsi.cpp #$(INC) #$(FRH)
#	$(CC) $(SRC)scsi.cpp -o $(BLD)scsi.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
$(BLD)normalqueue.o: $(SRC)normalqueue.cpp #$(INC) #$(FRH)
	$(CC) $(SRC)normalqueue.cpp -o $(BLD)normalqueue.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
$(BLD)parser.o: $(SRC)parser.cpp #$(INC) #$(FRH)
	$(CC) $(SRC)parser.cpp -o $(BLD)parser.o -I$(INC) -I$(ffsINC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
$(BLD)hrtim.o: $(SRC)hrtim.cpp
	$(CC) $(SRC)hrtim.cpp -o $(BLD)hrtim.o -I$(INC) -I$(ffsINC) -I$(LIB) -I$(FRH) $(CPPFLAGS)		
#_________________________________________________________________________________________________
#_________________________ FAT_FS Library ver 0.14a ______________________________________________	
#_________________________________________________________________________________________________
$(ffsBLD)diskio.o: $(ffsSRC)diskio.cpp 
	$(CC) $(ffsSRC)diskio.cpp -o $(ffsBLD)diskio.o -I$(INC) -I$(ffsINC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(ffsBLD)ff.o: $(ffsSRC)ff.cpp 
	$(CC) $(ffsSRC)ff.cpp -o $(ffsBLD)ff.o -I$(INC) -I$(ffsINC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(ffsBLD)ffsystem.o: $(ffsSRC)ffsystem.cpp 
	$(CC) $(ffsSRC)ffsystem.cpp -o $(ffsBLD)ffsystem.o -I$(ffsINC) -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)		
#$(ffsBLD)ffunicode.o: $(ffsSRC)ffunicode.cpp 
#	$(CC) $(ffsSRC)ffunicode.cpp -o $(ffsBLD)ffunicode.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
#_________________________________________________________________________________________________
#_________________________ USB Mass Storage Device LIBRARY _______________________________________	
#_________________________________________________________________________________________________
$(usbBLD)usb_device.o: $(usbSRC)usb_device.cpp #$(INC) #$(FRH)
	$(CC) $(usbSRC)usb_device.cpp -o $(usbBLD)usb_device.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(usbBLD)usbd_conf.o: $(usbSRC)usbd_conf.cpp #$(INC) #$(FRH)
	$(CC) $(usbSRC)usbd_conf.cpp -o $(usbBLD)usbd_conf.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(usbBLD)usbd_core.o: $(usbSRC)usbd_core.cpp #$(INC) #$(FRH)
	$(CC) $(usbSRC)usbd_core.cpp -o $(usbBLD)usbd_core.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(usbBLD)usbd_ctlreq.o: $(usbSRC)usbd_ctlreq.cpp #$(INC) #$(FRH)
	$(CC) $(usbSRC)usbd_ctlreq.cpp -o $(usbBLD)usbd_ctlreq.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(usbBLD)usbd_desc.o: $(usbSRC)usbd_desc.cpp #$(INC) #$(FRH)
	$(CC) $(usbSRC)usbd_desc.cpp -o $(usbBLD)usbd_desc.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(usbBLD)usbd_ioreq.o: $(usbSRC)usbd_ioreq.cpp #$(INC) #$(FRH)
	$(CC) $(usbSRC)usbd_ioreq.cpp -o $(usbBLD)usbd_ioreq.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(usbBLD)usbd_msc.o: $(usbSRC)usbd_msc.cpp #$(INC) #$(FRH)
	$(CC) $(usbSRC)usbd_msc.cpp -o $(usbBLD)usbd_msc.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(usbBLD)usbd_msc_bot.o: $(usbSRC)usbd_msc_bot.cpp #$(INC) #$(FRH)
	$(CC) $(usbSRC)usbd_msc_bot.cpp -o $(usbBLD)usbd_msc_bot.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(usbBLD)usbd_msc_data.o: $(usbSRC)usbd_msc_data.cpp #$(INC) #$(FRH)
	$(CC) $(usbSRC)usbd_msc_data.cpp -o $(usbBLD)usbd_msc_data.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(usbBLD)usbd_msc_scsi.o: $(usbSRC)usbd_msc_scsi.cpp #$(INC) #$(FRH)
	$(CC) $(usbSRC)usbd_msc_scsi.cpp -o $(usbBLD)usbd_msc_scsi.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)
$(usbBLD)usbd_storage_if.o: $(usbSRC)usbd_storage_if.cpp #$(INC) #$(FRH)
	$(CC) $(usbSRC)usbd_storage_if.cpp -o $(usbBLD)usbd_storage_if.o -I$(INC) -I$(LIB) -I$(usbINC) -I$(halINC) -I$(FRH) $(CPPFLAGS)

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
$(halBLD)stm32h7xx_hal_exti.o: $(halSRC)stm32h7xx_hal_exti.cpp #$(INC) #$(FRH)
	$(CC) $(halSRC)stm32h7xx_hal_exti.cpp -o $(halBLD)stm32h7xx_hal_exti.o -I$(INC) -I$(LIB) -I$(halINC) -I$(FRH) $(CPPFLAGS)
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
#-----------------------------------------------------------------------------------------------------
#--------------------------------------- MAIN --------------------------------------------------------
#-----------------------------------------------------------------------------------------------------
$(BLD)main.o: $(SRC)main.cpp 
	$(CC) $(SRC)main.cpp -o $(BLD)main.o -I$(halINC) -I$(INC) -I$(usbINC) -I$(halINC) -I$(LIB) -I$(ffsINC) $(CPPFLAGS)
	
clean:
	rm -rf $(BLD)*.o $(BLD)*.elf $(BLD)*.lst $(BLD)*.bin $(BLD)*.map $(BLD)*.hex \
	$(halBLD)*.o $(usbBLD)*.o


	
