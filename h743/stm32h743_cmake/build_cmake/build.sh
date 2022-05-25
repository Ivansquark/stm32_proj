#! /bin/sh
rm -R stm32h743_tcp_udp_build
mkdir stm32h743_tcp_udp_build
cd stm32h743_tcp_udp_build
cmake ../..
make -j8
