/*
* HomeAuto101.cpp
*
* Created: 19/08/2012 15:11:50
*  Author: tony
*/
# define F_CPU 16000000UL

#include <avr/io.h>
#include "apertures.h"
#include "indicators.h"
#include <util/delay.h>

#define FRONT_DOOR_PIN (unsigned int)0

int main(void)
{
	//
	// Set up the output pin for the system LED (pin 13)
	//
	Indicator i = Indicator((uint16_t *)0x24, (uint16_t *)0x25, PORTB7);
	
	Aperture frontDoor = Door((uint16_t *)0x21, (uint16_t *)0x20, FRONT_DOOR_PIN, "Front Door");
	while(1)
	{
		_delay_ms(500);
		if(frontDoor.isOpen()) {
			i.clear();
		} else {
			i.set();
		}
	}
}