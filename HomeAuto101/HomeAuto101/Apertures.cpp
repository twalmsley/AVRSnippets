#include "apertures.h"
#include <avr\sfr_defs.h>

Aperture::Aperture(uint16_t *ddr,uint16_t *port,uint8_t pin) {
	this->_ddr = ddr;
	this->_port = port;
	this->_pin = pin;
	//
	// Set the pin to input
	//
	*_ddr &= ~(_BV(_pin));
	//
	// Get the current state
	//
	_state = (*_port) & (_BV(_pin));
}

Aperture::~Aperture() {
	
}

uint8_t Aperture::isOpen() {
	return ((*_port) & (_BV(_pin))) == 0;
}

Door::Door(uint16_t *ddr, uint16_t *port,uint8_t pin, char* doorName) : Aperture(ddr, port, pin) {
	_name = doorName;
}

Door::~Door() {
	
}