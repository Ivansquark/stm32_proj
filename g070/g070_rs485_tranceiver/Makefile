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


CFLAGS = -Wall -g -O0 -specs=nosys.specs -specs=nano.specs \
	 -c -fno-common -mcpu=cortex-m0 -mthumb -mfloat-abi=soft \
	 -Wno-pointer-arith \
	 -L"C:\SysGCC\arm-eabi\lib\gcc\arm-none-eabi\9.3.1\thumb\v6-m\nofp" \
	 -Xlinker -Map=$(BLD)main.map
CPPFLAGS = -c -Wall -g -O0 --specs=nosys.specs -specs=nano.specs \
	   -nostdlib -nostartfiles -fno-common -D"assert_param(x)=" \
	   -mcpu=cortex-m0 -mthumb -march=armv6-m -fno-exceptions \
	   -Wno-pointer-arith -fno-rtti -mfloat-abi=soft \
	   -ffast-math -fno-math-errno \
	   -ffunction-sections -u_printf_float -Xlinker -Map=$(BLD)main.map
LFLAGS = -mcpu=cortex-m0 -mthumb -nostartfiles -lm -lc -lgcc \
		 -specs=nano.specs -specs=nosys.specs -fno-exceptions -fno-rtti \
		 -mfloat-abi=soft \
		 -L"C:\SysGCC\arm-eabi\lib\gcc\arm-none-eabi\9.3.1\thumb\v6-m\nofp" \
		 -L"C:\SysGCC\arm-eabi\arm-none-eabi\lib\thumb\v6-m\nofp" \
		 -Xlinker -Map=$(BLD)main.map #-u_printf_float
		#для FPU
		
load:	$(BLD)main.bin
	openocd -f interface/stlink.cfg -f target/stm32g0x.cfg -c "program $(BLD)main.bin \
	verify exit reset 0x08000000"
	
all:	$(BLD)main.bin $(BLD)main.lst $(BLD)main.elf
#___________________ BIN & HEX ____________________________
$(BLD)main.bin: $(BLD)main.elf
	$(OBJC) $(BLD)main.elf $(BLD)main.bin -O binary
	$(OBJC) $(BLD)main.elf $(BLD)main.hex -O ihex 
$(BLD)main.lst: $(BLD)main.elf
	$(OBJD) -D $(BLD)main.elf > $(BLD)main.lst
#___________________ LINKER _________________________________
$(BLD)main.elf: $(BLD)main.o $(BLD)startup.o
$(BLD)main.elf: $(BLD)uart.o $(BLD)rcc.o $(BLD)queue.o
	$(CC) -o $(BLD)main.elf -T$(LIB)stm32g070.ld $(BLD)startup.o \
	$(BLD)main.o $(BLD)uart.o $(BLD)rcc.o $(BLD)queue.o $(LFLAGS)
	@echo "\r\n____________________ SIZE ____________________________"
	@arm-none-eabi-size $(BLD)main.elf
	@echo "__________________________________________________________"

#__________________________________________________________________
#_________________________ BUILD __________________________________	
#__________________________________________________________________

#_________________________ system _______________________________
$(BLD)startup.o: $(LIB)startup.c
	$(CC) $(LIB)startup.c -o $(BLD)startup.o $(CFLAGS)	 
	#arm-none-eabi-objdump $(BLD)startup.o -h	
#________________________ functions _________________________
$(BLD)main.o: $(SRC)main.c 
	$(CC)  $(SRC)main.c -o $(BLD)main.o -I$(INC) -I$(LIB) $(CFLAGS)
#arm-none-eabi-objdump main.o -h 
$(BLD)rcc.o: $(SRC)rcc.c 
	$(CC)  $(SRC)rcc.c -o $(BLD)rcc.o -I$(INC) -I$(LIB) $(CFLAGS)
$(BLD)uart.o: $(SRC)uart.c 
	$(CC)  $(SRC)uart.c -o $(BLD)uart.o -I$(INC) -I$(LIB) $(CFLAGS)			
$(BLD)queue.o: $(SRC)queue.c 
	$(CC)  $(SRC)queue.c -o $(BLD)queue.o -I$(INC) -I$(LIB) $(CFLAGS)		

clean:
	rm -rf $(BLD)*.o $(BLD)*.elf $(BLD)*.lst $(BLD)*.bin $(BLD)*.hex $(BLD)*.map