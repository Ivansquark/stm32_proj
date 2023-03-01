# Compiler:
# arm-none-eabi-gcc --version: === 8.3.1 20190703
# System debugger:
# gdb-multiarch --version:	===	GNU gdb 10.1.90.20210103
# Open On-Chip Debugger:	
# openocd --version === Open On-Chip Debugger 0.11.0 (2021-10-07-10:32)
.PHONY: all clean load erase
#_________________________ GCC compilator commands ____________________________
CC = arm-none-eabi-g++
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

usbBLD = build/usb/
usbINC = usb/inc/
usbSRC = usb/src/

CPPFLAGS = -c -std=c++17 -Wall -Wextra -g -O0 \
	   -fno-common -D"assert_param(x)=" \
	   -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-d16 \
	   -fno-exceptions -Wno-pointer-arith -fno-rtti \
	   -ffast-math -fno-math-errno -ffunction-sections -fdata-sections

LFLAGS = -mthumb -march=armv7e-m -nostartfiles \
		 -specs=nano.specs -specs=nosys.specs -fno-exceptions \
		 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fno-use-cxa-atexit \
		 -Xlinker -Map=$(BLD)main.map -Xlinker --gc-sections
#-u_printf_float 
#для FPU
	
all:	$(BLD)main.bin $(BLD)main.lst $(BLD)main.elf
$(BLD)main.bin: $(BLD)main.elf
	arm-none-eabi-objcopy $(BLD)main.elf $(BLD)main.bin -O binary
	arm-none-eabi-objcopy $(BLD)main.elf $(BLD)main.hex -O ihex 
$(BLD)main.lst: $(BLD)main.elf
	arm-none-eabi-objdump -D $(BLD)main.elf > $(BLD)main.lst

#------------------------------------  LINKER ---------------------------------
$(BLD)main.elf: $(BLD)startup.o $(BLD)main.o $(BLD)timer.o $(BLD)encoder.o
$(BLD)main.elf: $(BLD)rcc_init.o $(BLD)normalqueue.o $(BLD)rtc.o $(BLD)sd.o
$(BLD)main.elf: $(BLD)pardac.o $(BLD)fmc.o $(BLD)malloc.o
$(BLD)main.elf: $(BLD)system_stm32h7xx.o $(BLD)hrtim.o 
$(BLD)main.elf: $(halBLD)stm32h7xx_hal.o $(halBLD)stm32h7xx_hal_cortex.o 
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_dma.o $(halBLD)stm32h7xx_hal_dma_ex.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_exti.o $(halBLD)stm32h7xx_hal_flash.o 
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_flash_ex.o $(halBLD)stm32h7xx_hal_mdma.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_gpio.o $(halBLD)stm32h7xx_hal_hsem.o 
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_msp.o $(halBLD)stm32h7xx_hal_pcd.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_pcd_ex.o $(halBLD)stm32h7xx_hal_rcc.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_pwr.o $(halBLD)stm32h7xx_hal_pwr_ex.o 
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_rcc_ex.o $(halBLD)stm32h7xx_hal_tim.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_tim_ex.o $(halBLD)stm32h7xx_ll_usb.o
$(BLD)main.elf: $(halBLD)stm32h7xx_it.o $(halBLD)stm32h7xx_ll_delayblock.o
$(BLD)main.elf: $(usbBLD)usb_device.o $(usbBLD)usbd_conf.o $(usbBLD)usbd_core.o 
$(BLD)main.elf: $(usbBLD)usbd_ctlreq.o $(usbBLD)usbd_msc.o
$(BLD)main.elf:	$(usbBLD)usbd_msc_bot.o $(usbBLD)usbd_storage_if.o
$(BLD)main.elf: $(usbBLD)usbd_desc.o $(usbBLD)usbd_ioreq.o
$(BLD)main.elf: $(usbBLD)usbd_msc_data.o $(usbBLD)usbd_msc_scsi.o 
$(BLD)main.elf: $(BLD)main.o			
	$(CC) -o $@ -T$(LIB)stm32h743.ld $^ -I$(LIB) $(LFLAGS)
#    $@ — target name
#    $< — first dependancy name
#    $? — all dependancies names, thats newer than target
#    $^ — all dependencies names of the target
#	 %	- template for all files

#-------------------------------------- HEX -----------------------------------	
	arm-none-eabi-objcopy -j .text -j .data -j .bss -O ihex \
		$(BLD)main.elf $(BLD)main.hex
#-------------------------------------- SIZE ----------------------------------	
	@echo "*******************************************************"
	@echo "*******************   SIZE   **************************"
	@echo "*******************************************************"
	arm-none-eabi-size $(BLD)main.elf
	@echo "*******************************************************"
#___________________ BUILD __________________________________	
$(BLD)%.o: $(LIB)%.cpp
	@ # **** mutex for parallel execution (mkdir must be locked) ****
	@if mkdir /var/lock/.myscript.exclusivelock; \
	then if [ ! -d "$(BLD)" ]; then mkdir $(BLD) $(halBLD) $(usbBLD); fi; \
	rmdir /var/lock/.myscript.exclusivelock; \
	fi
	$(CC) $< -o $@ -I$(INC) -I$(halINC) -I$(LIB) $(CPPFLAGS)	
#______________________________________________________________________________
#_________________________ HAL functions ______________________________________
#______________________________________________________________________________
$(halBLD)%.o: $(halSRC)%.cpp	
	$(CC) $< -o $@ -I$(INC) -I$(halINC) -I$(LIB) $(CPPFLAGS)
#______________________________________________________________________________
#_________________________ USB functions ______________________________________
#______________________________________________________________________________
$(usbBLD)%.o: $(usbSRC)%.cpp
	$(CC) $< -o $@ -I$(INC) -I$(usbINC) -I$(halINC) -I$(LIB) $(CPPFLAGS)
#______________________________________________________________________________
#_________________________ Basic functions ____________________________________
#______________________________________________________________________________
$(BLD)%.o: $(SRC)%.cpp
	@ # **** mutex for parallel execution (mkdir must be locked) ****
	@if mkdir /var/lock/.myscript.exclusivelock; \
	then if [ ! -d "$(BLD)" ]; then mkdir $(BLD) $(halBLD) $(usbBLD); fi; \
	rmdir /var/lock/.myscript.exclusivelock; \
	fi
	$(CC) $< -o $@ -I$(INC) -I$(halINC) -I$(LIB)  \
	-I$(usbINC) $(CPPFLAGS)
clean:
	rm -rf build 
	
load:	$(BLD)main.bin
	openocd -s /usr/local/share/openocd/scripts \
	-f lib/stlink.cfg -f lib/stm32h7x.cfg -c "program $(BLD)main.bin \
	verify reset exit 0x08000000"

erase:
	openocd -s /usr/local/share/openocd/scripts \
	-f lib/stlink.cfg -f lib/stm32h7x.cfg -c "init" -c "halt" -c \
	"wait_halt" -c "stm32f4x mass_erase 0" -c "exit"
