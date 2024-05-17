#    $@ — target name
#    $< — first dependancy name
#    $? — all dependancies names, thats newer than target
#    $^ — all dependencies names of the target
#	 %	- template for all files

.PHONY: all clean load 
#_________________________ GCC compilator commands ____________________________	
GCC_PATH = arm-none-eabi
CC = $(GCC_PATH)-gcc
CXX = $(GCC_PATH)-g++
OBJC = $(GCC_PATH)-objcopy
OBJD = $(GCC_PATH)-objdump
AS = $(GCC_PATH)-as
LD = $(GCC_PATH)-ld

INC = inc/
C_INC = c/inc/
LIB = lib/
BLD = build/
SRC = src/
C_SRC = c/src/
inc = -I$(INC) -I$(C_INC) -I$(LIB)

CFLAGS = -Wall -g -O0 -specs=nosys.specs -specs=nano.specs \
	 -c -fno-common -mcpu=cortex-m0 -mthumb -mfloat-abi=soft \
	 -Wno-pointer-arith
CXXFLAGS = -c -Wall -g -O0 --specs=nosys.specs -specs=nano.specs \
	   -nostdlib -nostartfiles -fno-common -D"assert_param(x)=" \
	   -mcpu=cortex-m0 -mthumb -march=armv6-m -fno-exceptions \
	   -Wno-pointer-arith -fno-rtti -mfloat-abi=soft \
	   -ffast-math -fno-math-errno \
	   -ffunction-sections -u_printf_float
LFLAGS = -mcpu=cortex-m0 -mthumb -nostartfiles -lm \
		 -specs=nano.specs -specs=nosys.specs -fno-exceptions -fno-rtti \
		 -mfloat-abi=soft \
		 -Xlinker -Map=$(BLD)main.map -Xlinker --gc-sections
#-u_printf_float
#для FPU
#___________________ OBJECTS & DEPENDENCIES ____________________________
CXXSRC = $(LIB)startup.cpp $(SRC)main.cpp
CXXOBJ := $(addprefix $(BLD),$(CXXSRC:.cpp=.o))
DEPS_CXX = $(CXXOBJ:.o=.d)
CSRC = c/src/delay.c
COBJ := $(addprefix $(BLD),$(CSRC:.c=.o))
DEPS_C = $(COBJ:.o=.d)
	
all:	$(BLD)main.bin $(BLD)main.lst $(BLD)main.elf
#___________________ BIN & HEX ____________________________
$(BLD)main.bin: $(BLD)main.elf
	arm-none-eabi-objcopy $(BLD)main.elf $(BLD)main.bin -O binary
	arm-none-eabi-objcopy $(BLD)main.elf $(BLD)main.hex -O ihex 
$(BLD)main.lst: $(BLD)main.elf
	arm-none-eabi-objdump -D $(BLD)main.elf > $(BLD)main.lst
#___________________ LINKER _________________________________
$(BLD)main.elf: $(COBJ) $(CXXOBJ)
	$(CC) $^ -o  $@ -T$(LIB)stm32g031.ld $(LFLAGS)
	@echo ''
	@echo "*******************   SIZE   **************************"
	$(GCC_PATH)-size $(BLD)main.elf
	@echo "*******************************************************"

#__________________________________________________________________
#_________________________ BUILD __________________________________	
#__________________________________________________________________

#includes = $(wildcard $(c/inc)/*.h)
$(COBJ) : $(BLD)%.o : %.c 
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@ $(inc)
$(DEPS_C) : $(BLD)%.d : %.c
	mkdir -p $(@D)
	$(CC) -E $(CFLAGS) $< -MM -MT $(@:.d=.o) $(inc)  > $@
$(CXXOBJ) : $(BLD)%.o : %.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(inc)
$(DEPS_CXX) : $(BLD)%.d : %.cpp 
	mkdir -p $(@D)
	$(CXX) -E $(CXXFLAGS) $< -MM -MT $(@:.d=.o) $(inc) > $@ 

# find headers dependencies
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
include $(DEPS_C)
include $(DEPS_CXX)
endif

clean:
	rm -rf build/

load:	$(BLD)main.bin
	openocd -s /usr/local/share/openocd/scripts \
	-f interface/stlink.cfg -f target/stm32g0x.cfg -c "program $(BLD)main.bin \
	verify exit reset 0x08000000"
