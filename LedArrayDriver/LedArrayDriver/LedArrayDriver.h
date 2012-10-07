#ifndef LEDARRAYDRIVER_H
#define LEDARRAYDRIVER_H

#include <avr/delay.h>
#include "PortPin.h"

#define PULSE_MILLIS	15
#define LED_PAUSE		75
#define LED_FADE_PAUSE	25

class LedArrayDriver {
	public:
		LedArrayDriver(	PortPin *clock, 
						PortPin *chipSelect, 
						PortPin *shift, 
						PortPin *a, 
						PortPin *b, 
						PortPin *c, 
						uint8_t rows, 
						uint16_t columns);
		~LedArrayDriver();

		void init();
		void clearDisplay();

		void begin();
		void show(uint8_t bits);
		void nextColumn();
	private:
		PortPin *_clock;
		PortPin *_chipSelect;
		PortPin *_shift;
		PortPin *_a;
		PortPin *_b;
		PortPin *_c;
		uint8_t _rows;
		uint16_t _columns;
	};
#endif