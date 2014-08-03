4313_cnc : 4313_cnc.c
	avr-gcc -O -std=c99 -o 4313_cnc.elf 4313_cnc.c

prog:
	avrdude -c avrisp2 -p m328p -P usb 4313_cnc.elf

