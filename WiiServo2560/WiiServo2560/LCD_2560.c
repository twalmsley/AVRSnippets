/*
 * LCD_2560.c
 *
 * Created: 07/07/2013 09:33:49
 *  Author: tony
 */
#define F_CPU 16000000UL  // 16 MHz

#include <avr/io.h>
#include <util/delay.h>

#define	LCD_PORT 	PORTC
#define	LCD_DDR		DDRC
#define LCD_PIN		PINC

#define	LCD_D4 		PORTC0// lcd pin 11
#define	LCD_D5 		PORTC1// lcd pin 12
#define LCD_D6 		PORTC2// lcd pin 13
#define	LCD_D7 		PORTC3// lcd pin 14

#define	LCD_RS		PORTC6// lcd pin 4
#define	LCD_EN		PORTC4// lcd pin 6
#define	LCD_RW		PORTC5// lcd pin 5

#define DDR_INIT_WRITE LCD_DDR = _BV(LCD_D4) | _BV(LCD_D5) | _BV(LCD_D6) | _BV(LCD_D7) | _BV(LCD_RS) | _BV(LCD_EN) | _BV(LCD_RW);
#define DDR_INIT_READ  LCD_DDR = _BV(LCD_RS) | _BV(LCD_EN) | _BV(LCD_RW);

#define ENABLE_READ LCD_PORT |= _BV(LCD_RW);
#define ENABLE_WRITE LCD_PORT &= ~_BV(LCD_RW);

#define ENABLE_LCD LCD_PORT |= _BV(LCD_EN);
#define DISABLE_LCD LCD_PORT &= ~(_BV(LCD_EN));

#define SELECT_INSTRUCTION_REG LCD_PORT &= ~(_BV(LCD_RS));
#define SELECT_DATA_REG LCD_PORT |= _BV(LCD_RS);

#define INST_CLEAR_DISPLAY 0x01
#define INST_RETURN_HOME 0x02
#define INST_INCMODE_NOSHIFT 0x06
#define INST_DISPLAYON_CURSORON_CURSORBLINK 0x0F
#define INST_DISPLAYON_CURSOROF_CURSORNOBLINK 0x0c
#define INST_DISPLAYOFF_CURSOROFF_BLINKOFF 0x08
#define INST_4BIT_2LINE_5X8FONT 0x28

uint8_t readByte() {
	DDR_INIT_READ
	SELECT_INSTRUCTION_REG
	ENABLE_READ
	ENABLE_LCD
	_delay_us(10);
	uint8_t result = (LCD_PIN)<<4;
	DISABLE_LCD
	_delay_us(10);
	ENABLE_LCD
	_delay_us(10);
	result |= (LCD_PIN & 0x0F);
	DISABLE_LCD
	return result;
}

void writeNibble(const uint8_t value) {
	DDR_INIT_WRITE
	ENABLE_WRITE
	LCD_PORT &= 0xF0;
	LCD_PORT |= (value & 0x0f);
	ENABLE_LCD
	_delay_us(20);
	DISABLE_LCD
}

void waitUntilNotBusy() {
	uint8_t value;
	do {
		value = readByte();
	}while(value & 0x80);
}

void writeDataByte(const uint8_t value) {
	DDR_INIT_WRITE
	SELECT_DATA_REG

	writeNibble((value & 0xF0)>>4);
	writeNibble(value & 0x0F);
	waitUntilNotBusy();
}

void writeIntructionByte(const uint8_t value) {
	DDR_INIT_WRITE
	SELECT_INSTRUCTION_REG
	writeNibble((value & 0xF0)>>4);
	writeNibble(value & 0x0F);
	waitUntilNotBusy();
}

void init_lcd() {
	_delay_ms(100);// Give the LCD time to initialise
	DISABLE_LCD
	//
	// Set to 4-bit operation
	//
	SELECT_INSTRUCTION_REG
	for(int count = 0;count < 3;count++) {
		writeNibble(3);
	_delay_us(100);
	}
	writeNibble(2);
	_delay_us(100);
	writeIntructionByte(INST_4BIT_2LINE_5X8FONT);

	writeIntructionByte(INST_DISPLAYOFF_CURSOROFF_BLINKOFF);
	writeIntructionByte(INST_CLEAR_DISPLAY);
	writeIntructionByte(INST_INCMODE_NOSHIFT);
	writeIntructionByte(INST_RETURN_HOME);
	writeIntructionByte(INST_DISPLAYON_CURSOROF_CURSORNOBLINK);
}

void setPosition(const uint8_t line, const uint8_t column) {
	uint8_t address = (64*line + column) | 0x80;
	writeIntructionByte(address);
}

void display(char* value) {
	char *ptr = value;
	while(*ptr != '\0') {
		writeDataByte(*ptr++);
	}
}

void displayAt(const uint8_t line, const uint8_t column, char* value) {
	setPosition(line,column);
	display(value);
}

static char *hex_chars = "0123456789ABCDEF";
void to_hex(unsigned char c, char *hex) {
	hex[0] = hex_chars[c>>4 & 0x0F];
	hex[1] = hex_chars[c & 0x0F];
}
