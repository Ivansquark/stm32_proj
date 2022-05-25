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


CFLAGS = -Wall -g -O2 -specs=nosys.specs -specs=nano.specs \
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
	arm-none-eabi-objcopy $(BLD)main.elf $(BLD)main.bin -O binary
	arm-none-eabi-objcopy $(BLD)main.elf $(BLD)main.hex -O ihex 
$(BLD)main.lst: $(BLD)main.elf
	arm-none-eabi-objdump -D $(BLD)main.elf > $(BLD)main.lst
#___________________ LINKER _________________________________
$(BLD)main.elf: $(BLD)main.o $(BLD)startup.o
$(BLD)main.elf: $(BLD)adc.o $(BLD)rcc.o $(BLD)led.o 
$(BLD)main.elf: $(BLD)lcd_par.o $(BLD)sed1335-stm32.o $(BLD)sed1335.o $(BLD)figures.o $(BLD)font_8x8.o
$(BLD)main.elf: $(BLD)ds3231.o $(BLD)buttons.o $(BLD)menu.o $(BLD)alarm.o 
$(BLD)main.elf: $(BLD)bang.o $(BLD)flash.o $(BLD)ds1822.o $(BLD)scheduler.o
	$(CC) -o $(BLD)main.elf -T$(LIB)stm32g070.ld $(BLD)startup.o \
	$(BLD)main.o $(BLD)adc.o $(BLD)rcc.o $(BLD)led.o $(BLD)bang.o $(BLD)ds1822.o \
	$(BLD)lcd_par.o $(BLD)sed1335-stm32.o $(BLD)sed1335.o $(BLD)figures.o $(BLD)font_8x8.o \
	$(BLD)ds3231.o $(BLD)buttons.o $(BLD)menu.o $(BLD)alarm.o $(BLD)flash.o $(BLD)scheduler.o \
	$(LFLAGS)
	arm-none-eabi-size $(BLD)main.elf

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
$(BLD)adc.o: $(SRC)adc.c 
	$(CC)  $(SRC)adc.c -o $(BLD)adc.o -I$(INC) -I$(LIB) $(CFLAGS)	
$(BLD)led.o: $(SRC)led.c 
	$(CC)  $(SRC)led.c -o $(BLD)led.o -I$(INC) -I$(LIB) $(CFLAGS)	
$(BLD)lcd_par.o: $(SRC)lcd_par.c 
	$(CC)  $(SRC)lcd_par.c -o $(BLD)lcd_par.o -I$(INC) -I$(LIB) $(CFLAGS)	
$(BLD)sed1335.o: $(SRC)sed1335.c 
	$(CC)  $(SRC)sed1335.c -o $(BLD)sed1335.o -I$(INC) -I$(LIB) $(CFLAGS)	
$(BLD)sed1335-stm32.o: $(SRC)sed1335-stm32.c 
	$(CC)  $(SRC)sed1335-stm32.c -o $(BLD)sed1335-stm32.o -I$(INC) -I$(LIB) $(CFLAGS)	
$(BLD)figures.o: $(SRC)figures.c 
	$(CC)  $(SRC)figures.c -o $(BLD)figures.o -I$(INC) -I$(LIB) $(CFLAGS)
$(BLD)font_8x8.o: $(SRC)font_8x8.c 
	$(CC)  $(SRC)font_8x8.c -o $(BLD)font_8x8.o -I$(INC) -I$(LIB) $(CFLAGS)
$(BLD)ds3231.o: $(SRC)ds3231.c 
	$(CC)  $(SRC)ds3231.c -o $(BLD)ds3231.o -I$(INC) -I$(LIB) $(CFLAGS)
$(BLD)buttons.o: $(SRC)buttons.c 
	$(CC)  $(SRC)buttons.c -o $(BLD)buttons.o -I$(INC) -I$(LIB) $(CFLAGS)
$(BLD)menu.o: $(SRC)menu.c 
	$(CC)  $(SRC)menu.c -o $(BLD)menu.o -I$(INC) -I$(LIB) $(CFLAGS)
$(BLD)alarm.o: $(SRC)alarm.c 
	$(CC)  $(SRC)alarm.c -o $(BLD)alarm.o -I$(INC) -I$(LIB) $(CFLAGS)
$(BLD)bang.o: $(SRC)bang.c 
	$(CC)  $(SRC)bang.c -o $(BLD)bang.o -I$(INC) -I$(LIB) $(CFLAGS)		
$(BLD)flash.o: $(SRC)flash.c 
	$(CC)  $(SRC)flash.c -o $(BLD)flash.o -I$(INC) -I$(LIB) $(CFLAGS)	
$(BLD)ds1822.o: $(SRC)ds1822.c 
	$(CC)  $(SRC)ds1822.c -o $(BLD)ds1822.o -I$(INC) -I$(LIB) $(CFLAGS)		
$(BLD)scheduler.o: $(SRC)scheduler.c 
	$(CC)  $(SRC)scheduler.c -o $(BLD)scheduler.o -I$(INC) -I$(LIB) $(CFLAGS)				

clean:
	rm -rf $(BLD)*.o $(BLD)*.elf $(BLD)*.lst $(BLD)*.bin $(BLD)*.hex $(BLD)*.map