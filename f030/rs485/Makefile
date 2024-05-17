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

CPPFLAGS = -std=c++17 -mthumb -mcpu=cortex-m0 -c -g3 -O2 -Wall \
	 -fno-exceptions -fno-rtti \
	-fno-math-errno -ffunction-sections -fdata-sections \
	--specs=nosys.specs --specs=nano.specs -fno-common -D"assert_param(x)="	
#-mfloat-abi=soft -u_printf_float -fpic

LCPPFLAGS = -std=c++17 -mcpu=cortex-m0 -march=armv7 -mthumb -nostartfiles \
		 -mfloat-abi=soft -specs=nosys.specs -specs=nano.specs \
		 -fno-use-cxa-atexit -Xlinker -Map=$(BLD)main.map -Xlinker --gc-sections #-Xlinker -EL 		 
#-z muldefs  -u_printf_float -pie

	
all: $(BLD)main.bin $(BLD)main.elf $(BLD)main.lst

$(BLD)main.bin: $(BLD)main.elf
	$(OBJC) $(BLD)main.elf $(BLD)main.bin -O binary

$(BLD)main.lst: $(BLD)main.elf
	$(OBJD) -D $(BLD)main.elf > $(BLD)main.lst

#------------------------------------  LINKER -------------------------------------------------------------------------

$(BLD)main.elf: $(BLD)startup.o	
$(BLD)main.elf:	$(BLD)main.o	$(BLD)rs485.o	$(BLD)leds.o	$(BLD)modbus.o 	
$(BLD)main.elf:	$(BLD)flash.o	$(BLD)motor.o	$(BLD)timer.o		 	
	$(CC) -o $@ -T$(LIB)stm32f030.ld $^ -I$(LIB) $(LCPPFLAGS)
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
#$(BLD)system_stm32f0xx.o: $(LIB)system_stm32f0xx.cpp
#	$(CC) $(LIB)system_stm32f0xx.cpp -o $(BLD)system_stm32f0xx.o $(CPPFLAGS)	

#_________________________________________________________________________________________________
#_________________________ Basic functions _______________________________________________________	
#_________________________________________________________________________________________________

$(BLD)%.o: $(SRC)%.cpp
	@ # **** mutex for parallel execution (mkdir must be locked) ****
	@if mkdir /var/lock/.myscript.exclusivelock; \
	then if [ ! -d "$(BLD)" ]; then mkdir $(BLD); fi; \
	rmdir /var/lock/.myscript.exclusivelock; \
	fi
	$(CC) -c $< -o $@ -I$(INC) -I$(LIB) $(CPPFLAGS)
clean:
	rm -rf build 
	
load: $(BLD)main.bin
	openocd -f lib/stlink.cfg -f lib/stm32f0x.cfg -c "init" -c "halt" -c "wait_halt" -c "program $(BLD)main.bin verify reset exit 0x08000000"

erase:
	openocd -f lib/stlink.cfg -f lib/stm32f0x.cfg -c "init" -c "halt" -c "wait_halt" -c "stm32f0x mass_erase 0" -c "exit"
