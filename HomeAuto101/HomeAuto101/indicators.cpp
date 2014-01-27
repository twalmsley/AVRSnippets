#include "indicators.h"
#include <avr/sfr_defs.h>

Indicator::Indicator(uint16_t *ddr,uint16_t *port,uint8_t pin) {
	this->_ddr = ddr;
	this->_port = port;
	this->_pin = pin;
	//
	// Set the pin to output
	//
	*_ddr |= _BV(_pin);
}

Indicator::~Indicator() {
	
}

void Indicator::set() {
	*_port |= _BV(_pin);
}

void Indicator::clear() {
	*_port &= ~(_BV(_pin));
}

void Indicator::toggle() {
	*_port ^= _BV(_pin);
}
