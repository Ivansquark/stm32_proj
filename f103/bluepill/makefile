.PHONY: all clean load

#TARGET = src/main.c
TARGET = src/main.cpp
INC = inc/
LIB = lib/

CC=arm-none-eabi-gcc
LD = arm-none-eabi-ld 
CFLAGS = -Wall -g -O2 -specs=nosys.specs -specs=nano.specs \
		-c -fno-common -D"assert_param(x)=" \
		-mcpu=cortex-m3 -mthumb -march=armv7-m
#-std=c99 -lm -lc
#-nostartfiles 
#--specs=nosys.specs
LDFLAGS =   -marmelf --gc-sections 
#-lgcc
#-L/usr/lib/arm-none-eabi/lib/thumb/v7e-m/ -L/usr/lib/gcc/arm-none-eabi/7.3.1/ \
#-L/usr/lib/arm-none-eabi/lib/thumb/v7e-m/fpv4-sp/hard/
#LFLAGS = -nostartfiles -lm -lc \
		-L/usr/lib/arm-none-eabi/lib/thumb/v7e-m/fpv4-sp/hard/ #для FPU
LFLAGS_END = -lm -lc
#-L/usr/lib/arm-none-eabi/bin/
#-Xlinker
MCU = cortex-m4

all: main.bin main.lst main.elf 

main.bin: main.elf
	arm-none-eabi-objcopy main.elf main.bin -O binary
	
load: main.bin
	st-flash write main.bin 0x08000000

main.lst: main.elf
	arm-none-eabi-objdump -D main.elf > main.lst

main.elf: startup.o main.o
	$(LD) -o main.elf -T$(LIB)stm32f103.ld startup.o main.o -I$(LIB) $(LDFLAGS) 
#$(CC) -o main.elf -T$(LIB)stm32f407vg.ld startup.o system_stm32f4xx.o main.o -I$(LIB)  
	arm-none-eabi-size main.elf
#-L/usr/lib/arm-none-eabi/lib/thumb/v7e-m/ -L/usr/lib/gcc/arm-none-eabi/7.3.1/ -L/usr/lib/arm-none-eabi/bin/
#	--data-sections

startup.o: 	$(LIB)startup.s
	arm-none-eabi-as -mthumb -mcpu=cortex-m3 $(LIB)startup.s -o startup.o -I$(LIB) 
#$(CFLAGS) 
	
	
main.o: $(TARGET) $(INC)
	$(CC)  -c $(TARGET) -o main.o -I$(LIB) -I$(INC) $(CFLAGS) 
#	arm-none-eabi-objdump main.o -h

clean:
	rm -rf *.o *.elf *.lst *.bin *.map
	
