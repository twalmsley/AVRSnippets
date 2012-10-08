/*
* LedArrayDriver.cpp
*
* Created: 29/08/2012 21:07:41
*  Author: tony walmsley
*/
#define F_CPU 16000000
#include <avr/io.h>
#include "LedArrayDriver.h"
#include "AsciiMessage.h"

#define MY_DDRA ((volatile uint8_t *)0x21)
#define MY_DDRC ((volatile uint8_t *)0x27)
#define MY_PORTA ((volatile uint8_t *)0x22)
#define MY_PORTC ((volatile uint8_t *)0x28)
#define MY_PINA ((volatile uint8_t *)0x20)
#define MY_PINC ((volatile uint8_t *)0x26)

#define NBR_OF_DISPLAY_COLUMNS  ((uint8_t)80)
#define NBR_OF_DISPLAY_ROWS		((uint8_t)7)
#define SCROLL_DELAY			5

#define shiftRegBit PORTA7
#define clockBit PORTC7
//
// The raster buffer - much bigger than the actual display to allow scrolling messages.
//
#define BUFFERSIZE 1000
uint8_t displayBuffer[BUFFERSIZE];
uint16_t displayPtr = 0;

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
	LedArrayDriver led(&clock, &chipSelect, &shift, &a,&b,&c, NBR_OF_DISPLAY_ROWS, NBR_OF_DISPLAY_COLUMNS);
	led.init();

	AsciiMessage message("                **** Welcome to the Swindon Hackspace at the Museum of Computing - Wednesdays 6:30pm to 10pm ****");
	uint16_t messageLen = message.getLength();
	uint16_t messageCols = messageLen*6;// 6 columns per character

	//
	// Convert each character to a bit pattern
	//
	message.buffer(displayBuffer);

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
	} while(1);

}
