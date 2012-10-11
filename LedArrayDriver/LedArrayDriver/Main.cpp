/*
* LedArrayDriver.cpp
*
* Created: 29/08/2012 21:07:41
*  Author: tony walmsley
*/
#define F_CPU 16000000
#include <avr/io.h>
#include "LedArrayDriver.h"
#include "Message.h"
#include "AsciiMessage.h"
#include "GraphicMessage.h"
#include "LeftScroller.h"
#include "StaticDisplay.h"

#define MY_DDRA ((volatile uint8_t *)0x21)
#define MY_DDRC ((volatile uint8_t *)0x27)
#define MY_PORTA ((volatile uint8_t *)0x22)
#define MY_PORTC ((volatile uint8_t *)0x28)
#define MY_PINA ((volatile uint8_t *)0x20)
#define MY_PINC ((volatile uint8_t *)0x26)

#define NBR_OF_DISPLAY_COLUMNS  ((uint8_t)80)
#define NBR_OF_DISPLAY_ROWS		((uint8_t)7)
#define SCROLL_DELAY			5

//
// Fix a compile error - see http://stackoverflow.com/questions/920500/what-is-the-purpose-of-cxa-pure-virtual for details.
//
extern "C" void __cxa_pure_virtual() { while (1); }
//
// The main entry point
//
int main(void) {
	//
	// Create a set of objects representing the output pins
	//
	PortPin clock(MY_DDRC, MY_PORTC, MY_PINC, PORTC7);
	PortPin shift(MY_DDRA, MY_PORTA, MY_PINA, PORTA7);
	PortPin chipSelect(MY_DDRA, MY_PORTA, MY_PINA, PORTA1);
	PortPin a(MY_DDRA, MY_PORTA, MY_PINA, PORTA3);
	PortPin b(MY_DDRA, MY_PORTA, MY_PINA, PORTA4);
	PortPin c(MY_DDRA, MY_PORTA, MY_PINA, PORTA5);
	//
	// Create and initialise the LedArrayDriver
	//
	LedArrayDriver led(&clock, &chipSelect, &shift, &a,&b,&c, NBR_OF_DISPLAY_ROWS, NBR_OF_DISPLAY_COLUMNS);
	led.init();

	uint8_t graphic[100] = {
		0b00011000,
		0b01111001,
		0b01111110,
		0b01011000,
		0b01111000,
		0b01011110,
		0b01111001,
		0b00011000,
		0b00000000,
		0b00000000,
		0b00011000,
		0b01111001,
		0b01111110,
		0b01011000,
		0b01111000,
		0b01011110,
		0b01111001,
		0b00011000,
		0b00000000,
		0b00000000,
		0b00011000,
		0b01111001,
		0b01111110,
		0b01011000,
		0b01111000,
		0b01011110,
		0b01111001,
		0b00011000,
		0b00000000,
		0b00000000,
		0b00011000,
		0b01111001,
		0b01111110,
		0b01011000,
		0b01111000,
		0b01011110,
		0b01111001,
		0b00011000,
		0b00000000,
		0b00000000,
		0b00011000,
		0b01111001,
		0b01111110,
		0b01011000,
		0b01111000,
		0b01011110,
		0b01111001,
		0b00011000,
		0b00000000,
		0b00000000,
		0b00011000,
		0b01111001,
		0b01111110,
		0b01011000,
		0b01111000,
		0b01011110,
		0b01111001,
		0b00011000,
		0b00000000,
		0b00000000,
		0b00011000,
		0b01111001,
		0b01111110,
		0b01011000,
		0b01111000,
		0b01011110,
		0b01111001,
		0b00011000,
		0b00000000,
		0b00000000,
		0b00011000,
		0b01111001,
		0b01111110,
		0b01011000,
		0b01111000,
		0b01011110,
		0b01111001,
		0b00011000,
		0b00000000,
		0b00000000,
		0b00011000,
		0b01111001,
		0b01111110,
		0b01011000,
		0b01111000,
		0b01011110,
		0b01111001,
		0b00011000,
		0b00000000,
		0b00000000,
		0b00011000,
		0b01111001,
		0b01111110,
		0b01011000,
		0b01111000,
		0b01011110,
		0b01111001,
		0b00011000,
		0b00000000,
		0b00000000
		};
	Message &message1 = *(new AsciiMessage("                **** Welcome to the Swindon Hackspace at the Museum of Computing - Wednesdays 6:30pm to 10pm ****"));
	Message &message2 = *(new GraphicMessage(graphic, 100));

	Animation &scrollLeft1 = *(new LeftScroller(led, message1, 3));
	Animation &staticDisplay = *(new StaticDisplay(led, message2, 255));
	do
	{
		while(staticDisplay.animate());
		while(scrollLeft1.animate());
	} while(1);
}
