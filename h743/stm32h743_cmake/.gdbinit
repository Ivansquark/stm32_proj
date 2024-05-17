directory src
directory lwip/src/
directory lwip/src/api/
directory lwip/src/core/
directory lwip/src/core/ipv4/
directory lwip/src/netif/
directory lwip/src/netif/ppp/
directory ffs/src/
directory hal/src/
target remote :50000
source gdb-svd.py
svd lib/STM32H743x.svd
file build_cmake/stm32h743_tcp_udp_build/stm32h743_tcp_udp
