# Compiler:
# arm-none-eabi-gcc --version: === 8.3.1 20190703
# System debugger:
# gdb-multiarch --version:	===	GNU gdb 10.1.90.20210103
# Open On-Chip Debugger:	
# openocd --version === Open On-Chip Debugger 0.11.0 (2021-10-07-10:32)
.PHONY: all clean load erase
#_________________________ GCC compilator commands _______________________________________________		
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
lwipHTTPD = lwip/src/apps/http/
#///////////////////////////////////
CPPFLAGS = -c -std=c++17 -Wall -Wextra -g -O0 \
	   -fno-common -D"assert_param(x)=" \
	   -mcpu=cortex-m4 -mthumb -fno-exceptions \
	   -Wno-pointer-arith -fno-rtti -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
	   -ffast-math -fno-math-errno -ffunction-sections -fdata-sections

LFLAGS = -mthumb -march=armv7e-m -nostartfiles \
		 -specs=nano.specs -specs=nosys.specs -fno-exceptions \
		 -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
		 -fno-use-cxa-atexit -Xlinker -Map=$(BLD)main.map -Xlinker --gc-sections
#-u_printf_float 
#для FPU
	
all:	$(BLD)main.bin $(BLD)main.lst $(BLD)main.elf
$(BLD)main.bin: $(BLD)main.elf
	arm-none-eabi-objcopy $(BLD)main.elf $(BLD)main.bin -O binary
	arm-none-eabi-objcopy $(BLD)main.elf $(BLD)main.hex -O ihex 
$(BLD)main.lst: $(BLD)main.elf
	arm-none-eabi-objdump -D $(BLD)main.elf > $(BLD)main.lst

#------------------------------------  LINKER -------------------------------------------------------------------------
$(BLD)main.elf: $(BLD)startup.o $(BLD)system_stm32f4xx.o $(BLD)rcc.o
$(BLD)main.elf: $(halBLD)stm32f4xx_hal.o
$(BLD)main.elf: $(halBLD)stm32f4xx_hal_cortex.o $(halBLD)stm32f4xx_hal_dma.o 		$(halBLD)stm32f4xx_hal_dma_ex.o
$(BLD)main.elf: $(halBLD)stm32f4xx_hal_eth.o 	
$(BLD)main.elf: $(halBLD)stm32f4xx_hal_exti.o 	$(halBLD)stm32f4xx_hal_flash.o 		$(halBLD)stm32f4xx_hal_flash_ex.o
$(BLD)main.elf: $(halBLD)stm32f4xx_hal_gpio.o 	$(halBLD)stm32f4xx_it.o
$(BLD)main.elf: $(halBLD)stm32f4xx_hal_msp.o 	
$(BLD)main.elf: $(halBLD)stm32f4xx_hal_pwr.o 	$(halBLD)stm32f4xx_hal_pwr_ex.o		$(halBLD)stm32f4xx_hal_rcc.o
$(BLD)main.elf: $(halBLD)stm32f4xx_hal_rcc_ex.o $(halBLD)stm32f4xx_hal_tim.o		$(halBLD)stm32f4xx_hal_tim_ex.o
$(BLD)main.elf:	$(lwipBLD)ethernetif.o	$(lwipBLD)lwip.o 		$(lwipBLD)memp.o 		$(lwipBLD)lan8742.o
$(BLD)main.elf:	$(lwipBLD)pbuf.o		$(lwipBLD)etharp.o		$(lwipBLD)netif.o 		$(lwipBLD)init.o 	
$(BLD)main.elf: $(lwipBLD)def.o			$(lwipBLD)tcp.o 		$(lwipBLD)tcp_out.o		$(lwipBLD)ip4.o 
$(BLD)main.elf: $(lwipBLD)icmp.o		$(lwipBLD)inet_chksum.o	$(lwipBLD)ip4_addr.o 	$(lwipBLD)udp.o 
$(BLD)main.elf: $(lwipBLD)tcp_in.o		$(lwipBLD)timeouts.o 	$(lwipBLD)ethernet.o 	$(lwipBLD)err.o
$(BLD)main.elf:	$(lwipBLD)mem.o			$(lwipBLD)ip4_frag.o	$(lwipBLD)ip.o			$(lwipBLD)fs.o
$(BLD)main.elf:	$(lwipBLD)httpd.o			
$(BLD)main.elf: $(BLD)main.o			$(BLD)net.o				
$(BLD)main.elf: $(BLD)flash.o			$(BLD)i2c.o				$(BLD)uart.o			
$(BLD)main.elf: $(BLD)dac.o				$(BLD)gptimers.o
$(BLD)main.elf: $(BLD)irq.o
	$(CC) -o $@ -T$(LIB)stm32f4.ld $^ -I$(LIB) $(LFLAGS)
#    $@ — target name
#    $< — first dependancy name
#    $? — all dependancies names, thats newer than target
#    $^ — all dependencies names of the target
#	 %	- template for all files

#-------------------------------------- HEX -------------------------------------------------------	
	arm-none-eabi-objcopy -j .text -j .data -j .bss -O ihex $(BLD)main.elf $(BLD)main.hex
#-------------------------------------- SIZE -------------------------------------------------------	
	@echo "*******************************************************"
	@echo "*******************   SIZE   **************************"
	@echo "*******************************************************"
	arm-none-eabi-size $(BLD)main.elf
	@echo "*******************************************************"
#___________________ BUILD __________________________________	
$(BLD)%.o: $(LIB)%.cpp
	@ # **** mutex for parallel execution (mkdir must be locked) ****
	@if mkdir /var/lock/.myscript.exclusivelock; \
	then if [ ! -d "$(BLD)" ]; then mkdir $(BLD) $(halBLD) $(lwipBLD); fi; \
	rmdir /var/lock/.myscript.exclusivelock; \
	fi
	$(CC) $< -o $@ -I$(INC) -I$(halINC) -I$(LIB)  \
	-I$(lwipINCLUDE) -I$(lwipSYSTEM) -I$(lwipINC) $(CPPFLAGS)	
#_________________________________________________________________________________________________
#_________________________ HAL functions _______________________________________________________	
#_________________________________________________________________________________________________
$(halBLD)%.o: $(halSRC)%.cpp	
	$(CC) $< -o $@ -I$(INC) -I$(halINC) -I$(LIB)  \
	-I$(lwipINCLUDE) -I$(lwipSYSTEM) -I$(lwipINC) $(CPPFLAGS)
#_________________________________________________________________________________________________
#_________________________ LWIP functions _______________________________________________________	
#_________________________________________________________________________________________________
$(lwipBLD)%.o: $(lwipSRC)%.cpp	
	$(CC) $< -o $@ -I$(INC) -I$(halINC) -I$(LIB)  \
	-I$(lwipINCLUDE) -I$(lwipSYSTEM) -I$(lwipINC) $(CPPFLAGS)
$(lwipBLD)%.o: $(lwipSrcCore)%.cpp	
	$(CC) $< -o $@ -I$(INC) -I$(halINC) -I$(LIB)  \
	-I$(lwipINCLUDE) -I$(lwipSYSTEM) -I$(lwipINC) $(CPPFLAGS)
$(lwipBLD)%.o: $(lwipSrcApi)%.cpp	
	$(CC) $< -o $@ -I$(INC) -I$(halINC) -I$(LIB)  \
	-I$(lwipINCLUDE) -I$(lwipSYSTEM) -I$(lwipINC) $(CPPFLAGS)
$(lwipBLD)%.o: $(lwipSrcCoreIP4)%.cpp	
	$(CC) $< -o $@ -I$(INC) -I$(halINC) -I$(LIB)  \
	-I$(lwipINCLUDE) -I$(lwipSYSTEM) -I$(lwipINC) $(CPPFLAGS)
$(lwipBLD)%.o: $(lwipSrcNetif)%.cpp	
	$(CC) $< -o $@ -I$(INC) -I$(halINC) -I$(LIB)  \
	-I$(lwipINCLUDE) -I$(lwipSYSTEM) -I$(lwipINC) $(CPPFLAGS)
$(lwipBLD)%.o: $(lwipSrcNetifPPP)%.cpp	
	$(CC) $< -o $@ -I$(INC) -I$(halINC) -I$(LIB)  \
	-I$(lwipINCLUDE) -I$(lwipSYSTEM) -I$(lwipINC) $(CPPFLAGS)
$(lwipBLD)%.o: $(lwipHTTPD)%.cpp	
	$(CC) $< -o $@ -I$(INC) -I$(halINC) -I$(LIB)  \
	-I$(lwipINCLUDE) -I$(lwipSYSTEM) -I$(lwipINC) -I$(lwipHTTPD) $(CPPFLAGS)

$(lwipBLD)%.o: $(SRC)%.cpp	
	$(CC) $< -o $@ -I$(INC) -I$(halINC) -I$(LIB)  \
	-I$(lwipINCLUDE) -I$(lwipSYSTEM) -I$(lwipINC) $(CPPFLAGS)
#_________________________________________________________________________________________________
#_________________________ Basic functions _______________________________________________________	
#_________________________________________________________________________________________________
$(BLD)%.o: $(SRC)%.cpp
	@ # **** mutex for parallel execution (mkdir must be locked) ****
	@if mkdir /var/lock/.myscript.exclusivelock; \
	then if [ ! -d "$(BLD)" ]; then mkdir $(BLD) $(halBLD) $(lwipBLD); fi; \
	rmdir /var/lock/.myscript.exclusivelock; \
	fi
	$(CC) $< -o $@ -I$(INC) -I$(halINC) -I$(LIB)  \
	-I$(lwipINCLUDE) -I$(lwipSYSTEM) -I$(lwipINC) $(CPPFLAGS)
clean:
	rm -rf build 
	
load:	$(BLD)main.bin
	openocd -f lib/stlink.cfg -f lib/stm32f4x.cfg -c "program $(BLD)main.bin \
	verify reset exit 0x08000000"

erase:
	openocd -f lib/stlink.cfg -f lib/stm32f4x.cfg -c "init" -c "halt" -c "wait_halt" -c "stm32f4x mass_erase 0" -c "exit"
