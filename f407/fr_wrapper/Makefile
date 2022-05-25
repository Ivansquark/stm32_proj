.PHONY: all clean load 

TARGET = src/main.cpp
INC = inc/
LIB = lib/
SRC = src/
FRH = freeRTOS/inc/
FRS = freeRTOS/src/    

CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
#for freeRTOS
CFLAGS = -Wall -g -O1 -specs=nosys.specs -specs=nano.specs \
	 -c -fno-common -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard \
	 -Wno-pointer-arith 
CPPFLAGS = -c -Wall -g -O0 --specs=nosys.specs -specs=nano.specs \
	   -nostartfiles -fno-common -D"assert_param(x)=" \
	   -mcpu=cortex-m4 -mthumb -march=armv7e-m -fno-exceptions \
	   -Wno-pointer-arith -fno-rtti -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
	   -u_printf_float -ffast-math -fno-math-errno \
	   -ffunction-sections
LDFLAGS = -specs=nano.specs -specs=nosys.specs \
	  -marmelf --gc-sections -lgcc\
	  -L/usr/lib/gcc/arm-none-eabi/7.3.1/ \
	  -L/usr/lib/arm-none-eabi/newlib/thumb/v7e-m/fpv4-sp/hard/ \
	  -I/lib/arm-none-eabi/newlib/thumb/v7e-m/
LFLAGS = -mcpu=cortex-m4 -mthumb -nostartfiles -lm -lc -lgcc \
		 -specs=nano.specs -specs=nosys.specs -fno-exceptions -fno-rtti \
		 -u_printf_float -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
		 -L/usr/lib/arm-none-eabi/newlib/thumb/v7e-m/fpv4-sp/hard/
		#для FPU
load:	main.bin
	st-flash write main.bin 0x08000000
	
all:	main.bin main.lst main.elf
main.bin: main.elf
	arm-none-eabi-objcopy main.elf main.bin -O binary
main.lst: main.elf
	arm-none-eabi-objdump -D main.elf > main.lst

main.elf: startup.o  malloc.o tasks.o port.o queue.o list.o timers.o heap_2.o  main.o
	$(CC) -o main.elf -T$(LIB)stm32f4.ld startup.o malloc.o  \
	port.o tasks.o queue.o list.o timers.o heap_2.o main.o \
	$(LFLAGS) -Xlinker -Map=main.map -z muldefs
	arm-none-eabi-size main.elf
	
startup.o: $(LIB)startup.cpp
	$(CC) $(LIB)startup.cpp -o startup.o $(CPPFLAGS) 
	#arm-none-eabi-objdump startup.o -h
malloc.o: $(SRC)malloc.cpp $(INC) $(FRH)
	$(CC) $(SRC)malloc.cpp -o malloc.o -I$(INC) -I$(FRH) $(CPPFLAGS)

port.o: freeRTOS/src/port.c 
	$(CC) freeRTOS/src/port.c -o port.o -I$(FRH) -I$(INC) $(CFLAGS)
tasks.o: freeRTOS/src/tasks.c
	$(CC) freeRTOS/src/tasks.c -o tasks.o -I$(FRH) -I$(INC) $(CFLAGS)
queue.o: freeRTOS/src/queue.c
	$(CC) freeRTOS/src/queue.c -o queue.o -I$(FRH) -I$(INC) $(CFLAGS)	
list.o: freeRTOS/src/list.c
	$(CC) freeRTOS/src/list.c -o list.o -I$(FRH) -I$(INC) $(CFLAGS)	
timers.o: freeRTOS/src/timers.c
	$(CC) freeRTOS/src/timers.c -o timers.o -I$(FRH) -I$(INC) $(CFLAGS)	
heap_2.o: freeRTOS/src/heap_2.c $(INC)
	$(CC) freeRTOS/src/heap_2.c -o heap_2.o -I$(FRH) -I$(INC) $(CFLAGS)		
	
main.o: $(TARGET) $(INC) $(FRH)
	$(CC) $(TARGET) -o main.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
	#arm-none-eabi-objdump main.o -h 
				
clean:
	rm -rf *.o *.elf *.lst *.bin *.map
