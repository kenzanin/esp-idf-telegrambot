./qemu/bin/qemu-system-xtensa -nographic -machine esp32 -drive file=flash_image.bin,if=mtd,format=raw -global driver=timer.esp32.timg,property=wdt_disable,value=true -nic user,model=open_eth $@
