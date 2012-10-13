#ifndef PORTPIN_H
#define PORTPIN_H

#include <avr/io.h>

class PortPin {
	public:
		PortPin();
		~PortPin();

		virtual void setAsOutput() = 0;
		virtual void setAsInput() = 0;
		virtual void set() = 0;
		virtual void set(uint8_t val) = 0;
		virtual void clear() = 0;
		virtual uint8_t read() = 0;
		virtual void toggle() = 0;
		virtual void pulse(uint16_t millis) = 0;
};
#endif