cmake_minimum_required(VERSION 3.18)
set(PROJECT stm32h743_tcp_udp)

project(${PROJECT})

set(EXECUTABLE_NAME ${PROJECT})
set(CMAKE_CXX_COMPILER /usr/bin/arm-none-eabi-g++)
set(CMAKE_OBJCOPY /usr/bin/arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP /usr/bin/arm-none-eabi-objdump)
set(CMAKE_CXX_FLAGS "-std=c++17 -mthumb -mcpu=cortex-m7 -c -g3 -O0 -Wall \
-mfpu=fpv5-d16 -mfloat-abi=hard -fno-exceptions -fno-rtti -fno-math-errno \
-ffunction-sections -fdata-sections --specs=nosys.specs --specs=nano.specs -fno-common -D\"assert_param(x)=\""
)
set(CMAKE_LD_FLAGS "-std=c++17 -mcpu=cortex-m7 -march=armv7e-m -mthumb -nostartfiles \
-mfloat-abi=hard  -mfpu=fpv5-d16 -specs=nosys.specs -specs=nano.specs -fno-use-cxa-atexit \
-Xlinker -Map=${PROJECT}.map -Xlinker --gc-sections -Xlinker -EL"
)

include_directories(inc/ lib/ hal/inc/ hal/inc/Legacy/ ffs/inc/ lwip/inc/ 
    lwip/src/include/ lwip/system/ /usr/lib/arm-none-eabi/include/
)
file(GLOB_RECURSE SRCS src/*.cpp lib/*.cpp hal/src/*.cpp ffs/src/*.cpp lwip/src/*.cpp 
    lwip/src/api/*.cpp lwip/src/core/*.cpp lwip/src/core/ipv4/*.cpp
    lwip/src/netif/*.cpp lwip/src/netif/ppp/*.cpp lwip/system/*.cpp 
)
add_executable(${PROJECT} ${SRCS})

set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> -T/home/debian/embedded/stm32/vim/Control/lib/stm32h743.ld \
-o ${PROJECT_NAME}  <OBJECTS>  ${CMAKE_LD_FLAGS} \
 -Ilib/ "
)
add_custom_command(
    TARGET ${PROJECT}
    POST_BUILD    
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT} ${PROJECT}.bin
)
#add_custom_command(
#    TARGET ${PROJECT}
#    COMMAND ${CMAKE_OBJDUMP} -D ${PROJECT} > ${PROJECT}.lst
#)
