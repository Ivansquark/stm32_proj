.PHONY: all clean load 
#_________________________ GCC compilator commands _______________________________________________		
CC = arm-none-eabi-gcc
OBJC = arm-none-eabi-objcopy
OBJD = arm-none-eabi-objdump
AS = arm-none-eabi-as
LD = arm-none-eabi-ld

INC = inc/
LIB = lib/
BLD = build/
SRC = src/
halSRC = HAL/src/
halINC = HAL/inc/

CPPFLAGS = -c -Wall -g -O1 --specs=nosys.specs -specs=nano.specs \
	   -nostdlib -nostartfiles -fno-common -D"assert_param(x)=" \
	   -mcpu=cortex-m0 -mthumb -march=armv6-m -fno-exceptions \
	   -Wno-pointer-arith -fno-rtti -mfloat-abi=soft \
	   -fno-math-errno -fno-unwind-tables \
	   -ffunction-sections -Xlinker -Map=$(BLD)main.map
# -u_printf_float
LFLAGS = -mcpu=cortex-m0 -mthumb -nostdlib -nostartfiles -lm -lc -lgcc -lnosys \
		 -specs=nano.specs -specs=nosys.specs -fno-exceptions -fno-rtti \
		 -mfloat-abi=soft -fno-unwind-tables \
		 -L"C:\SysGCC\arm-eabi\lib\gcc\arm-none-eabi\9.3.1\thumb\v6-m\nofp" \
		 -L"C:\SysGCC\arm-eabi\arm-none-eabi\lib\thumb\v6-m\nofp" \
		 -Xlinker -Map=$(BLD)main.map #-u_printf_float
		#для FPU
		
load:	$(BLD)main.bin
	openocd -f lib/stlink.cfg -f lib/stm32f0x.cfg -c "program $(BLD)main.bin \
	verify exit reset 0x08000000"
	
all:	$(BLD)main.bin $(BLD)main.lst $(BLD)main.elf
#___________________ BIN & HEX ____________________________
$(BLD)main.bin: $(BLD)main.elf
	arm-none-eabi-objcopy $(BLD)main.elf $(BLD)main.bin -O binary
	arm-none-eabi-objcopy $(BLD)main.elf $(BLD)main.hex -O ihex 
$(BLD)main.lst: $(BLD)main.elf
	arm-none-eabi-objdump -D $(BLD)main.elf > $(BLD)main.lst
#___________________ LINKER _________________________________
$(BLD)main.elf: $(BLD)main.o $(BLD)startup.o $(BLD)uart.o $(BLD)can.o $(BLD)button.o $(BLD)flash.o
$(BLD)main.elf: $(BLD)system_stm32f0xx.o $(BLD)stm32f0xx_it.o $(BLD)stm32f0xx_hal_msp.o 
$(BLD)main.elf: $(BLD)stm32f0xx_hal.o $(BLD)stm32f0xx_hal_can.o $(BLD)stm32f0xx_hal_cortex.o 
$(BLD)main.elf: $(BLD)stm32f0xx_hal_gpio.o $(BLD)stm32f0xx_hal_rcc.o 
	$(CC) -o $(BLD)main.elf -T$(LIB)stm32f042.ld $(BLD)startup.o \
	$(BLD)main.o $(BLD)uart.o $(BLD)can.o $(BLD)button.o $(BLD)flash.o \
	$(BLD)system_stm32f0xx.o $(BLD)stm32f0xx_it.o $(BLD)stm32f0xx_hal_msp.o \
	$(BLD)stm32f0xx_hal.o $(BLD)stm32f0xx_hal_can.o $(BLD)stm32f0xx_hal_cortex.o \
	$(BLD)stm32f0xx_hal_gpio.o $(BLD)stm32f0xx_hal_rcc.o \
	$(LFLAGS)

	@echo "\r\n____________________ SIZE ____________________________"
	@arm-none-eabi-size $(BLD)main.elf
	@echo "______________________________________________________"
#__________________________________________________________________
#_________________________ BUILD __________________________________	
#__________________________________________________________________

#_________________________ system _______________________________
$(BLD)startup.o: $(LIB)startup.cpp
	$(CC) $(LIB)startup.cpp -o $(BLD)startup.o $(CPPFLAGS)
    #arm-none-eabi-objdump $(BLD)startup.o -h
$(BLD)system_stm32f0xx.o: $(SRC)system_stm32f0xx.cpp
	$(CC) $(SRC)system_stm32f0xx.cpp -o $(BLD)system_stm32f0xx.o -I$(INC) -I$(LIB) $(CPPFLAGS)	
$(BLD)stm32f0xx_it.o: $(SRC)stm32f0xx_it.cpp
	$(CC) $(SRC)stm32f0xx_it.cpp -o $(BLD)stm32f0xx_it.o -I$(INC) -I$(LIB) -I$(halINC) $(CPPFLAGS)
$(BLD)stm32f0xx_hal_msp.o: $(SRC)stm32f0xx_hal_msp.cpp
	$(CC) $(SRC)stm32f0xx_hal_msp.cpp -o $(BLD)stm32f0xx_hal_msp.o -I$(INC) -I$(LIB) -I$(halINC) $(CPPFLAGS)	
#_________________________________________________________________________________________________							
#__________________________ HAL LIBRARY __________________________________________________________
#_________________________________________________________________________________________________
$(BLD)stm32f0xx_hal.o: $(halSRC)stm32f0xx_hal.cpp 
	$(CC) $(halSRC)stm32f0xx_hal.cpp -o $(BLD)stm32f0xx_hal.o -I$(INC) -I$(LIB) -I$(halINC) \
	$(CPPFLAGS)	
$(BLD)stm32f0xx_hal_can.o: $(halSRC)stm32f0xx_hal_can.cpp 
	$(CC) $(halSRC)stm32f0xx_hal_can.cpp -o $(BLD)stm32f0xx_hal_can.o -I$(INC) -I$(LIB) \
	-I$(halINC) $(CPPFLAGS)		
$(BLD)stm32f0xx_hal_cortex.o: $(halSRC)stm32f0xx_hal_cortex.cpp 
	$(CC) $(halSRC)stm32f0xx_hal_cortex.cpp -o $(BLD)stm32f0xx_hal_cortex.o -I$(INC) -I$(LIB) \
	-I$(halINC) $(CPPFLAGS)				
$(BLD)stm32f0xx_hal_gpio.o: $(halSRC)stm32f0xx_hal_gpio.cpp 
	$(CC) $(halSRC)stm32f0xx_hal_gpio.cpp -o $(BLD)stm32f0xx_hal_gpio.o -I$(INC) -I$(LIB) \
	-I$(halINC) $(CPPFLAGS)		
$(BLD)stm32f0xx_hal_rcc.o: $(halSRC)stm32f0xx_hal_rcc.cpp 
	$(CC) $(halSRC)stm32f0xx_hal_rcc.cpp -o $(BLD)stm32f0xx_hal_rcc.o -I$(INC) -I$(LIB) \
	-I$(halINC) $(CPPFLAGS)		
#________________________ functions _________________________
$(BLD)main.o: $(SRC)main.cpp 
	$(CC)  $(SRC)main.cpp -o $(BLD)main.o -I$(INC) -I$(LIB) -I$(halINC) $(CPPFLAGS)
#arm-none-eabi-objdump main.o -h 				
$(BLD)uart.o: $(SRC)uart.cpp 
	$(CC)  $(SRC)uart.cpp -o $(BLD)uart.o -I$(INC) -I$(LIB) -I$(halINC) $(CPPFLAGS)
$(BLD)can.o: $(SRC)can.cpp 
	$(CC)  $(SRC)can.cpp -o $(BLD)can.o -I$(INC) -I$(LIB) -I$(halINC) $(CPPFLAGS)	
$(BLD)button.o: $(SRC)button.cpp 
	$(CC)  $(SRC)button.cpp -o $(BLD)button.o -I$(INC) -I$(LIB) -I$(halINC) $(CPPFLAGS)	
$(BLD)flash.o: $(SRC)flash.cpp 
	$(CC)  $(SRC)flash.cpp -o $(BLD)flash.o -I$(INC) -I$(LIB) -I$(halINC) $(CPPFLAGS)	
clean:
	rm -rf $(BLD)*.o $(BLD)*.elf $(BLD)*.lst $(BLD)*.bin $(BLD)*.hex $(BLD)*.map
	mkdir build