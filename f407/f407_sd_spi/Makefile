.PHONY: dirBuild all clean load 

TARGET = src/main.cpp
INC = inc/
LIB = lib/
SRC = src/
srcFFS = FATFS/src/
incFFS = FATFS/inc/
BLD = build/

CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
#for freeRTOS
CFLAGS = -Wall -g -O0 -specs=nosys.specs -specs=nano.specs \
	 -c -fno-common -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
	 -Wno-pointer-arith 
CPPFLAGS = -c -Wall -g -O0 --specs=nosys.specs -specs=nano.specs \
	   -nostartfiles -fno-common -D"assert_param(x)=" \
	   -mcpu=cortex-m4 -mthumb -march=armv7e-m -fno-exceptions \
	   -Wno-pointer-arith -fno-rtti -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
	   -ffast-math -fno-math-errno \
	   -ffunction-sections -L"C:\SysGCC\arm-eabi\lib\gcc\arm-none-eabi\9.3.1\thumb\v7e-m+fp\hard"
#-u_printf_float

LFLAGS = -mcpu=cortex-m4 -mthumb -nostartfiles -lm -lc -lgcc \
		 -specs=nano.specs -specs=nosys.specs -fno-exceptions -fno-rtti \
		 -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
		 -L/usr/lib/arm-none-eabi/newlib/thumb/v7e-m/fpv4-sp/hard/ \
		 -Xlinker -Map=$(BLD)main.map
#-u_printf_float
load:	$(BLD)main.bin
	openocd -f lib/stlink.cfg -f lib/stm32f4x.cfg -c "program $(BLD)main.bin \
	verify reset exit 0x08000000"

all:  $(BLD)main.bin $(BLD)main.lst $(BLD)main.elf 
$(BLD)main.bin: $(BLD)main.elf
	arm-none-eabi-objcopy $(BLD)main.elf $(BLD)main.bin -O binary
$(BLD)main.lst: $(BLD)main.elf
	arm-none-eabi-objdump -D $(BLD)main.elf > $(BLD)main.lst	

$(BLD)main.elf: $(BLD)startup.o $(BLD)ethernet.o $(BLD)main.o $(BLD)rcc.o
$(BLD)main.elf: $(BLD)gptimers.o $(BLD)flash.o $(BLD)sd_spi.o
$(BLD)main.elf: $(BLD)diskio.o $(BLD)pff.o
	$(CC) -o $(BLD)main.elf -T$(LIB)stm32f4.ld \
	$(BLD)startup.o $(BLD)ethernet.o $(BLD)main.o $(BLD)rcc.o \
	$(BLD)gptimers.o $(BLD)flash.o $(BLD)sd_spi.o \
	$(BLD)diskio.o $(BLD)pff.o \
	$(LFLAGS)  
# -z  muldefs
	arm-none-eabi-size $(BLD)main.elf	
	
$(BLD)startup.o: $(LIB)startup.cpp
	$(CC) $(LIB)startup.cpp -o $(BLD)startup.o $(CPPFLAGS)	
#arm-none-eabi-objdump $(BLD)startup.o -h

$(BLD)ethernet.o: $(SRC)ethernet.cpp
	$(CC) $(SRC)ethernet.cpp -o $(BLD)ethernet.o -I$(INC) -I$(incFFS) -I$(LIB) $(CPPFLAGS)
$(BLD)rcc.o: $(SRC)rcc.cpp
	$(CC) $(SRC)rcc.cpp -o $(BLD)rcc.o -I$(INC) -I$(LIB) -I$(incFFS) $(CPPFLAGS)
$(BLD)gptimers.o: $(SRC)gptimers.cpp
	$(CC) $(SRC)gptimers.cpp -o $(BLD)gptimers.o -I$(INC) -I$(incFFS) -I$(LIB) $(CPPFLAGS)	
$(BLD)flash.o: $(SRC)flash.cpp
	$(CC) $(SRC)flash.cpp -o $(BLD)flash.o -I$(INC) -I$(incFFS) -I$(LIB) $(CPPFLAGS)
$(BLD)sd_spi.o: $(SRC)sd_spi.cpp
	$(CC) $(SRC)sd_spi.cpp -o $(BLD)sd_spi.o -I$(INC) -I$(incFFS) -I$(LIB) $(CPPFLAGS)

#______________ FFS ___________________________
$(BLD)diskio.o: $(srcFFS)diskio.cpp
	$(CC) $(srcFFS)diskio.cpp -o $(BLD)diskio.o -I$(INC) -I$(incFFS) -I$(LIB) $(CPPFLAGS)	
$(BLD)pff.o: $(srcFFS)pff.cpp
	$(CC) $(srcFFS)pff.cpp -o $(BLD)pff.o -I$(INC) -I$(incFFS) -I$(LIB) $(CPPFLAGS)				

$(BLD)main.o: $(TARGET) 
	$(CC) $(TARGET) -o $(BLD)main.o -I$(INC) -I$(incFFS) -I$(LIB) $(CPPFLAGS)
#arm-none-eabi-objdump $(BLD)main.o -h 
				
clean: dirbuildTemp remove dirbuild 

dirbuildTemp:
	mkdir -p build;
remove:
	rm -R $(BLD)
dirbuild:
	mkdir -p build;
#	rm -rf *.o *.elf *.lst *.bin *.map
