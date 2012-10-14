/*
* LedArrayDriver.cpp
*
* Created: 29/08/2012 21:07:41
*  Author: tony walmsley
*/
#define F_CPU 16000000
#include <avr/io.h>
#include "AVRPortPin.h"
#include "LedArrayDriver.h"
#include "Message.h"
#include "AsciiMessage.h"
#include "RepeatedGraphic.h"
#include "AnimatedGraphic.h"
#include "LeftScroller.h"
#include "StaticDisplay.h"
#include "GraphicBitmaps.h"

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
	AVRPortPin clock(MY_DDRC, MY_PORTC, MY_PINC, PORTC7);
	AVRPortPin shift(MY_DDRA, MY_PORTA, MY_PINA, PORTA7);
	AVRPortPin chipSelect(MY_DDRA, MY_PORTA, MY_PINA, PORTA1);
	AVRPortPin a(MY_DDRA, MY_PORTA, MY_PINA, PORTA3);
	AVRPortPin b(MY_DDRA, MY_PORTA, MY_PINA, PORTA4);
	AVRPortPin c(MY_DDRA, MY_PORTA, MY_PINA, PORTA5);
	//
	// Create and initialise the LedArrayDriver
	//
	LedArrayDriver led(&clock, &chipSelect, &shift, &a,&b,&c, NBR_OF_DISPLAY_ROWS, NBR_OF_DISPLAY_COLUMNS);
	led.init();

	Message &welcome = *(new AsciiMessage("**** Welcome to the Swindon Hackspace at the Museum of Computing - Wednesdays 6:30pm to 10pm ****"));
	Message &multiInvader = *(new RepeatedGraphic(invader, 10, 8));
	Message &pacman = *(new AnimatedGraphic(animPacman, 17, 7));// columns * frames
	Message &invader = *(new AnimatedGraphic(animInvader, 11, 4));// columns * frames

	Animation &scrollLeft1 = *(new LeftScroller(led, invader, 10));
	Animation &scrollLeft2 = *(new LeftScroller(led, multiInvader, 5));
	Animation &scrollLeft3 = *(new LeftScroller(led, pacman, 5));
	Animation &scrollLeft4 = *(new LeftScroller(led, welcome, 5));
	do
	{
		while(scrollLeft1.animate());
		while(scrollLeft2.animate());
		while(scrollLeft3.animate());
		while(scrollLeft4.animate());
	} while(1);
}
