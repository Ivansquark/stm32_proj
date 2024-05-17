# BOOTFLASHER

this program is setted at address 0x08000000
and check boot_flag at address 0x08100000
if boot_flag = 0xFF => go to address 0x08020000 where are main firmware
if boot_flag = 0x01XX => rewrite XX sectors from address 0x08120000 to 0x08020000, clear boot flag and reboot
