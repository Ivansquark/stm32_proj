.PHONY: all clean load 
#_________________________ GCC compilator commands _______________________________________________		
CPP = arm-none-eabi-gcc
OBJC = arm-none-eabi-objcopy
OBJD = arm-none-eabi-objdump
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
GCC_PATH = arm-none-eabi

INC = inc/
LIB = lib/
BLD = build/
SRC = src/
halSRC = HAL/src/
halINC = HAL/inc/

CPPFLAGS = -c -Wall -g -O0 --specs=nosys.specs -specs=nano.specs \
	   -nostdlib -nostartfiles -fno-common -D"assert_param(x)=" \
	   -mcpu=cortex-m3 -mthumb -march=armv7-m -fno-exceptions \
	   -Wno-pointer-arith -fno-rtti -mfloat-abi=soft \
	   -fno-math-errno -fno-unwind-tables \
	   -ffunction-sections -Xlinker -Map=$(BLD)main.map
CFLAGS = -c -Wall -g -O0 --specs=nosys.specs -specs=nano.specs \
	   -nostdlib -nostartfiles -fno-common -D"assert_param(x)=" \
	   -mcpu=cortex-m3 -mthumb \
	   -Wno-pointer-arith -mfloat-abi=soft \
	   -fno-math-errno -fno-unwind-tables \
	   -ffunction-sections -Xlinker -Map=$(BLD)main.map
# -u_printf_float
LFLAGS = -mcpu=cortex-m3 -mthumb -march=armv7-m -nostartfiles \
		-specs=nano.specs -specs=nosys.specs -fno-exceptions -fno-rtti \
		-mfloat-abi=soft -fno-use-cxa-atexit \
		-Xlinker -Map=$(BLD)main.map -Xlinker --gc-sections		
	
all:	$(BLD)main.bin $(BLD)main.lst $(BLD)main.elf
#___________________ BIN & HEX ____________________________
$(BLD)main.bin: $(BLD)main.elf
	arm-none-eabi-objcopy $(BLD)main.elf $(BLD)main.bin -O binary
	arm-none-eabi-objcopy $(BLD)main.elf $(BLD)main.hex -O ihex 
$(BLD)main.lst: $(BLD)main.elf
	arm-none-eabi-objdump -D $(BLD)main.elf > $(BLD)main.lst
#___________________ LINKER _________________________________
$(BLD)main.elf: $(BLD)main.o $(BLD)startup.o $(BLD)uart.o 
$(BLD)main.elf: $(BLD)can.o $(BLD)button.o $(BLD)flash.o
$(BLD)main.elf: $(BLD)adc.o $(BLD)leds.o $(BLD)buzzer.o
$(BLD)main.elf: $(BLD)system_stm32f1xx.o $(BLD)stm32f1xx_it.o $(BLD)stm32f1xx_hal_msp.o 
$(BLD)main.elf: $(BLD)core_cm3.o 
$(BLD)main.elf: $(BLD)stm32f1xx_hal.o $(BLD)stm32f1xx_hal_can.o $(BLD)stm32f1xx_hal_cortex.o 
$(BLD)main.elf: $(BLD)stm32f1xx_hal_gpio.o $(BLD)stm32f1xx_hal_rcc.o
	$(CPP) -o $@ -T$(LIB)stm32f107.ld $^ -I$(LIB) $(LFLAGS)
#    $@ — target name
#    $< — first dependancy name
#    $? — all dependancies names, thats newer than target
#    $^ — all dependencies names of the target
#	 %	- template for all files
#-------------------------------------- SIZE ----------------------------------	
	@echo "*******************************************************"
	@echo "*******************   SIZE   **************************"
	@echo "*******************************************************"
	$(GCC_PATH)-size $(BLD)main.elf
	@echo "*******************************************************"
#__________________________________________________________________
#_________________________ BUILD __________________________________	
#__________________________________________________________________

#_________________________ system _______________________________
$(BLD)%.o: $(LIB)%.cpp
	@ # **** mutex for parallel execution (mkdir must be locked) ****
	@if mkdir /var/lock/.myscript.exclusivelock; \
	then if [ ! -d "$(BLD)" ]; then mkdir $(BLD); fi; \
	rmdir /var/lock/.myscript.exclusivelock; \
	fi
	$(CPP) $< -o $@ -I$(INC) -I$(LIB) $(CPPFLAGS)
$(BLD)%.o: $(LIB)%.c
	@ # **** mutex for parallel execution (mkdir must be locked) ****
	@if mkdir /var/lock/.myscript.exclusivelock; \
	then if [ ! -d "$(BLD)" ]; then mkdir $(BLD); fi; \
	rmdir /var/lock/.myscript.exclusivelock; \
	fi
	$(CPP) $< -o $@ -I$(INC) -I$(LIB) -I$(halINC) $(CFLAGS)
#_________________________________________________________________________________________________							
#__________________________ HAL LIBRARY __________________________________________________________
#_________________________________________________________________________________________________
$(BLD)%.o: $(halSRC)%.c
	@ # **** mutex for parallel execution (mkdir must be locked) ****
	@if mkdir /var/lock/.myscript.exclusivelock; \
	then if [ ! -d "$(BLD)" ]; then mkdir $(BLD); fi; \
	rmdir /var/lock/.myscript.exclusivelock; \
	fi
	$(CPP) $< -o $@ -I$(INC) -I$(LIB) -I$(halINC) $(CFLAGS)
#________________________ functions _________________________
$(BLD)%.o: $(SRC)%.cpp
	@ # **** mutex for parallel execution (mkdir must be locked) ****
	@if mkdir /var/lock/.myscript.exclusivelock; \
	then if [ ! -d "$(BLD)" ]; then mkdir $(BLD); fi; \
	rmdir /var/lock/.myscript.exclusivelock; \
	fi
	$(CPP) $< -o $@ -I$(INC) -I$(LIB) -I$(halINC) $(CPPFLAGS)
load:	$(BLD)main.bin
	openocd -s /usr/local/share/openocd/scripts -f lib/stlink.cfg -f lib/stm32f1x.cfg \
	"-c program $(BLD)main.bin"
# verify exit reset 0x08000000"
clean:
	rm -rf build
	mkdir build
