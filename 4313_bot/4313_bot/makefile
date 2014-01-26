4313_bot : 4313_bot.c
	avr-gcc -O -std=c99 -o 4313_bot.elf 4313_bot.c

prog:
	avrdude -c avrisp2 -p m328p -P usb 4313_bot.elf

