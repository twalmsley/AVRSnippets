/*
 * _4313_bot.c
 *
 * Created: 03/01/2014 16:34:53
 *  Author: tony
 */

#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#undef LCD_PRESENT

#define	LCD_PORT 	PORTD
#define	LCD_DDR		DDRD
#define LCD_PIN		PIND

#define	LCD_D4 		PORTD0// lcd pin 11
#define	LCD_D5 		PORTD1// lcd pin 12
#define LCD_D6 		PORTD2// lcd pin 13
#define	LCD_D7 		PORTD3// lcd pin 14

#define	LCD_RS		PORTD4// lcd pin 4
#define	LCD_EN		PORTD6// lcd pin 6
#define	LCD_RW		PORTD5// lcd pin 5

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

#define NINETY_DEGREES 2200/2
#define STEP_DELAY _delay_ms(2)

static uint8_t steps[] = {
		0b0001,
		0b0011,
		0b0010,
		0b0110,
		0b0100,
		0b1100,
		0b1000,
		0b1001
	};

static long x = 0,y = 0;
static short int xdir = 0,ydir = 1;// Assume we start in the Y direction
static uint8_t m1 = 0, m2 = 0;
static uint8_t m1dir = 0xFF, m2dir = 1;// 0xFF is effectively -1

static char text[20];

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
	_delay_ms(300);// Give the LCD time to initialise
	DISABLE_LCD
	//
	// Set to 4-bit operation
	//
	SELECT_INSTRUCTION_REG
	for(int count = 0;count < 3;count++) {
		writeNibble(3);
		_delay_us(300);
	}
	writeNibble(2);
	_delay_us(300);
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

void nextM1Step() {
	PORTB = (PORTB & 0xF0) | steps[m1 & 0x07];
	m1+=m1dir;
}

void nextM2Step() {
	PORTB = (PORTB & 0x0F) | (steps[m2 & 0x07]<<4);
	m2+=m2dir;
}

void reverseM1Step() {
	PORTB = (PORTB & 0xF0) | steps[m1 & 0x07];
	m1-=m1dir;
}

void reverseM2Step() {
	PORTB = (PORTB & 0x0F) | (steps[m2 & 0x07]<<4);
	m2-=m2dir;
}

void showLocation() {
#ifdef LCD_PRESENT
	writeIntructionByte(INST_CLEAR_DISPLAY);
	ltoa(x, text,10);
	setPosition(0,0);
	display(text);
	
	ltoa(y, text,10);
	setPosition(1,0);
	display(text);
#endif
}

void move(uint16_t distance) {
	while(distance > 0) {
		nextM1Step();
		nextM2Step();
		distance--;
		x += xdir;
		y += ydir;
		STEP_DELAY;
	}
	showLocation();
}

void turnRight90() {
	uint16_t distance = NINETY_DEGREES;
	while(distance > 0) {
		nextM2Step();
		reverseM1Step();
		distance--;
		STEP_DELAY;
	}
	
	if(xdir == 1 && ydir == 0) {
		xdir = 0,ydir = -1;
		} else if(xdir == -1 && ydir == 0) {
		xdir = 0,ydir = 1;
		} else if(xdir == 0 && ydir == 1) {
		xdir = 1, ydir = 0;
		} else if(xdir == 0 && ydir == -1) {
		xdir = -1, ydir = 0;
	}
	showLocation();
}

void turnLeft90() {
	uint16_t distance = NINETY_DEGREES;
	while(distance > 0) {
		nextM1Step();
		reverseM2Step();
		distance--;
		STEP_DELAY;
	}
	
	if(xdir == 0 && ydir == 1) {
		xdir = -1,ydir = 0;
		} else if(xdir == 0 && ydir == -1) {
		xdir = 1,ydir = 0;
		} else if(xdir == 1 && ydir == 0) {
		xdir = 0, ydir = 1;
		} else if(xdir == -1 && ydir == 0) {
		xdir = 0, ydir = -1;
	}
	showLocation();
}

void stepperTest() 
{
    while(1)
    {
		//
		// m1
		//
        for(uint16_t i = 0; i < 512; i++) {
			nextM1Step();
			STEP_DELAY;
		}
		_delay_ms(1000);
		//
		// m2
		//
        for(uint16_t i = 0; i < 512; i++) {
	        nextM2Step();
	        STEP_DELAY;
        }
        _delay_ms(1000);
    }
}

void zigzag() 
{
	move(4096);
	turnRight90();
	move(4096);
	turnLeft90();
	move(4096);
	turnRight90();
	move(4096);
	turnLeft90();
	move(4096);
	turnRight90();
	move(4096);
	turnLeft90();
	move(4096);
	turnRight90();
	move(4096);
	turnLeft90();
}

void alex() {
move(4096);
turnLeft90();
move(4096);
turnRight90();
move(4096);
turnLeft90();
move(4096);
turnRight90();	
}

void emma() {
move(4096);
turnRight90();
move(4096);
turnRight90();
move(4096);
turnRight90();
move(4096);
turnRight90();	
}

int main(void)
{
	DDRB = 0xFF;// Two 4-bit steppers on port B
	PORTB = 0x00;// All off

	_delay_ms(5000); // Time to disconnect the programming cable
	//stepperTest();
#ifdef LCD_PRESENT
	init_lcd();
#endif
	zigzag();
	alex();
	emma();
}