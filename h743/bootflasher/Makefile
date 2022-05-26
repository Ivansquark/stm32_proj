# Compiler:
# arm-none-eabi-gcc --version: === 8.3.1 20190703
# Open On-Chip Debugger:	
# openocd --version === Open On-Chip Debugger 0.11.0 (2021-10-07-10:32)
# System debugger:
# gdb-multiarch --version:	===	GNU gdb 10.1.90.20210103
.PHONY: all, clean, load, erase

#_________________________________________________________________________________________________
#_________________________ GCC compilator commands _______________________________________________	
#_________________________________________________________________________________________________	
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

CPPFLAGS = -std=c++17 -mthumb -mcpu=cortex-m7 -c -g3 -O0 -Wall \
	-mfpu=fpv5-d16 -mfloat-abi=hard -fno-exceptions -fno-rtti \
	-fno-math-errno -ffunction-sections -fdata-sections \
	--specs=nosys.specs --specs=nano.specs -fno-common -D"assert_param(x)="	
#-u_printf_float -fpic

LCPPFLAGS = -std=c++17 -mcpu=cortex-m7 -march=armv7e-m -mthumb -nostartfiles \
		 -mfloat-abi=hard  -mfpu=fpv5-d16 -specs=nosys.specs -specs=nano.specs \
		 -fno-use-cxa-atexit -Xlinker -Map=$(BLD)main.map -Xlinker --gc-sections #-Xlinker -EL 		 
#-z muldefs  -u_printf_float -pie

	
all: $(BLD)main.bin $(BLD)main.elf $(BLD)main.lst

$(BLD)main.bin: $(BLD)main.elf
	$(OBJC) $(BLD)main.elf $(BLD)main.bin -O binary

$(BLD)main.lst: $(BLD)main.elf
	$(OBJD) -D $(BLD)main.elf > $(BLD)main.lst

#------------------------------------  LINKER -------------------------------------------------------------------------

$(BLD)main.elf: $(BLD)startup.o $(BLD)system_stm32h7xx.o $(BLD)cache.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_cortex.o $(halBLD)stm32h7xx_hal_dma.o 		$(halBLD)stm32h7xx_hal_dma_ex.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_exti.o 	$(halBLD)stm32h7xx_hal_flash.o 		$(halBLD)stm32h7xx_hal_flash_ex.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_gpio.o 	$(halBLD)stm32h7xx_hal_hsem.o 		$(halBLD)stm32h7xx_hal_mdma.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_msp.o 	
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_pwr.o 	$(halBLD)stm32h7xx_hal_pwr_ex.o		$(halBLD)stm32h7xx_hal_rcc.o
$(BLD)main.elf: $(halBLD)stm32h7xx_hal_rcc_ex.o $(halBLD)stm32h7xx_hal_tim.o		$(halBLD)stm32h7xx_hal_tim_ex.o
$(BLD)main.elf: $(halBLD)stm32h7xx_it.o 		$(halBLD)stm32h7xx_ll_delayblock.o	
$(BLD)main.elf: $(BLD)main.o			$(BLD)sysinit.o 	
$(BLD)main.elf: $(BLD)hal_init.o		$(BLD)flash.o			$(BLD)leds.o 	
	$(CC) -o $@ -T$(LIB)stm32h743.ld $^ -I$(LIB) $(LCPPFLAGS)
#    $@ — target name
#    $< — first dependancy name
#    $? — all dependancies names, thats newer than target
#    $^ — all dependencies names of the target
#	 %	- template for all files

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
#_________________________________________________________________________________________________							
#__________________________ HAL LIBRARY __________________________________________________________
#_________________________________________________________________________________________________
$(halBLD)%.o: $(halSRC)%.cpp
	$(CC) -c $< -o $@ -I$(INC) -I$(halINC) -I$(LIB) $(CPPFLAGS)

#_________________________________________________________________________________________________
#_________________________ Basic functions _______________________________________________________	
#_________________________________________________________________________________________________

$(BLD)%.o: $(SRC)%.cpp
	@ # **** mutex for parallel execution (mkdir must be locked) ****
	@if mkdir /var/lock/.myscript.exclusivelock; \
	then if [ ! -d "$(BLD)" ]; then mkdir $(BLD) $(halBLD) $(lwipBLD); fi; \
	rmdir /var/lock/.myscript.exclusivelock; \
	fi
	$(CC) -c $< -o $@ -I$(INC) -I$(halINC) -I$(LIB) $(CPPFLAGS)
clean:
	rm -rf build 
	
load: $(BLD)main.bin
	openocd -f lib/stlink.cfg -f lib/stm32h7x.cfg -c "program $(BLD)main.bin verify reset exit 0x08000000"

erase:
	openocd -f lib/stlink.cfg -f lib/stm32h7x.cfg -c "init" -c "halt" -c "wait_halt" -c "stm32h7x mass_erase 0" -c "exit"
	openocd -f lib/stlink.cfg -f lib/stm32h7x.cfg -c "init" -c "halt" -c "wait_halt" -c "stm32h7x mass_erase 1" -c "exit"