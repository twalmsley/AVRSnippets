#include <avr/delay.h>
#include "PortPin.h"

#define TRUE ((uint8_t)1)
#define FALSE ((uint8_t)0)

PortPin::PortPin(volatile uint8_t *ddr, volatile uint8_t *port, volatile uint8_t *inPort, uint8_t pin)
	:_ddr(ddr),_port(port),_inPort(inPort), _pinMask(_BV(pin)), _pin(pin) {
}

PortPin::~PortPin(){

}

void PortPin::setAsOutput(){
	*_ddr |= _pinMask;
}

void PortPin::setAsInput(){
	*_ddr &= ~_pinMask;
}

void PortPin::set(){
	*_port |= _pinMask;
}

void PortPin::set(uint8_t val){
	val <<= _pin;
	*_port = val;
}

void PortPin::clear(){
	*_port &= ~_pinMask;
}

uint8_t PortPin::read(){
	return (*_inPort & _pinMask)?TRUE:FALSE;
}

void PortPin::toggle(){
	*_inPort |= _pinMask;
}

void PortPin::pulse(uint16_t millis) {
	toggle();
	for(uint16_t count =0;count <millis;count++) {
		_delay_us(1);
	}
	toggle();
}