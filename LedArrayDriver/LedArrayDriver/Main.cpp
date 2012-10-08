/*
* LedArrayDriver.cpp
*
* Created: 29/08/2012 21:07:41
*  Author: tony walmsley
*/
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/delay.h>
#include <string.h>
#include "LedArrayDriver.h"
#include "CharMap.h"

#define MY_DDRA ((volatile uint8_t *)0x21)
#define MY_DDRC ((volatile uint8_t *)0x27)
#define MY_PORTA ((volatile uint8_t *)0x22)
#define MY_PORTC ((volatile uint8_t *)0x28)
#define MY_PINA ((volatile uint8_t *)0x20)
#define MY_PINC ((volatile uint8_t *)0x26)

#define COLUMNS_PER_CHARACTER	5
#define NBR_OF_DISPLAY_COLUMNS  80
#define FOREVER					while(1)
#define SCROLL_DELAY			6

#define shiftRegBit PORTA7
#define clockBit PORTC7

#define BUFFERSIZE 1000

uint8_t displayBuffer[BUFFERSIZE];
uint16_t displayPtr = 0;

void buffer(char letter) {
	uint8_t *matrix = ascii[letter-32];
	//
	// Each character has a number of columns
	//
	for (uint8_t column = 0; column < COLUMNS_PER_CHARACTER;column++) {
		uint8_t pattern = matrix[column];
		//
		// The data byte needs to be reversed and shifted to the right 1 place
		//
		pattern = ((pattern * 0x0802LU & 0x22110LU) | (pattern * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
		pattern >>=1;
		//
		// Store it in the display buffer and move on.
		//
		displayBuffer[displayPtr] = pattern;
		displayPtr++;
	}
	//
	// Add a blank column as a spacer
	//
	displayBuffer[displayPtr] = 0;
	displayPtr++;
}

int main(void) {
	//
	// Create a set of objects representing the output pins
	//
	PortPin clock(MY_DDRC, MY_PORTC, MY_PINC, clockBit);
	PortPin shift(MY_DDRA, MY_PORTA, MY_PINA, shiftRegBit);
	PortPin chipSelect(MY_DDRA, MY_PORTA, MY_PINA, PORTA1);
	PortPin a(MY_DDRA, MY_PORTA, MY_PINA, PORTA3);
	PortPin b(MY_DDRA, MY_PORTA, MY_PINA, PORTA4);
	PortPin c(MY_DDRA, MY_PORTA, MY_PINA, PORTA5);
	//
	// Create and initialise the LedArrayDriver
	//
	LedArrayDriver led(&clock, &chipSelect, &shift, &a,&b,&c, (uint8_t)7, (uint16_t)80);
	led.init();

	char *message = "                **** Welcome to Swindon Hackspace at the Museum of Computing - Wednesdays 6:30pm to 10pm ****";
	uint16_t messageLen = strlen(message);
	uint16_t messageCols = messageLen*6;// 6 columns per character

	//
	// Convert each character to a bit pattern
	//
	char *messagePtr = message;
	while(*messagePtr) {
		buffer(*messagePtr++);
	}

	do
	{
		for(uint16_t offset = 0;offset<(messageCols*SCROLL_DELAY);offset++) {
			//
			// Set up and clock-in the first shift register bit
			//
			led.begin();
			//
			// Process each column in the buffer
			//
			uint16_t count = 0;
			while(count <= 80) {
				uint8_t pattern = displayBuffer[(offset/SCROLL_DELAY)+count];
				led.show(pattern);
				led.nextColumn();
				count++;
			}
		}
	} FOREVER;

}
