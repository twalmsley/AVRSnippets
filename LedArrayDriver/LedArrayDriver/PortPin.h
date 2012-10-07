#ifndef PORTPIN_H
#define PORTPIN_H

#include <avr/io.h>

class PortPin {
	public:
		PortPin(volatile uint8_t *ddr, volatile uint8_t *port, volatile uint8_t *inPort, uint8_t pin);
		~PortPin();

		void setAsOutput();
		void setAsInput();
		void set();
		void set(uint8_t val);
		void clear();
		uint8_t read();
		void toggle();
		void pulse(uint16_t millis);
	private:
		volatile uint8_t *_ddr;
		volatile uint8_t *_port;
		volatile uint8_t *_inPort;
		uint8_t _pinMask;
		uint8_t _pin;
};
#endif