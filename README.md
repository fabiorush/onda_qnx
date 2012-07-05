onda_qnx
========

mmc rescan 0; fatload mmc 0 0x80100000 ifs-omap3730-beagle.bin; go 0x80100000
fatload mmc 0 0x80100000 bsp-TI-omap3730-Beagle-xm.ifs; go 0x80100000
fatload mmc 0 0x81000000 bootstrap_hello.uimage; go 0x81000000 
fatload mmc 0 0x80ffffc0 l4linux; go 0x80100000
fatload mmc 0 0x80ffffc0 bootstrap_L4Linux_ARM.uimage; go 0x81000000
ifconfig en0 192.168.1.2 netmask 255.255.255.0 up
ifconfig en0 172.27.73.1 netmask 255.255.192.0 up
ifconfig en0 192.168.137.2 netmask 255.255.255.0 up

GPIO_139
[31:0] I/O gpio_[159:128]

bit 11
mem = (1<<11)


GPIO5: 0x49056000

4 bytes cada registrador

GPIO_DATAOUT: 0x03C 0x49056034
GPIO_CLEARDATAOUT : 0x090 0x49056090
GPIO_SETDATAOUT: 0x094 0x49056094

rising detect 0x49056048
falling detect 0x4905604C
IRQ 33?

GPIO5_MPU_IRQ M_IRQ_33 Destination is the MPU INTC

GPIO_IRQSTATUS1 RW 32 0x018
GPIO_IRQENABLE1 RW 32 0x01C


Falar da:
- configuração do GPIO
- timer e o problema da resolucão: tick de 1ms
- nanospin e o problema do processamento

GPTIMER3 GPT3_IRQ M_IRQ_39

while true; do echo teste; done

