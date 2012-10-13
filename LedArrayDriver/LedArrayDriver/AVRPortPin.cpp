#include <avr/delay.h>
#include "AVRPortPin.h"

#define TRUE ((uint8_t)1)
#define FALSE ((uint8_t)0)

AVRPortPin::AVRPortPin(volatile uint8_t *ddr, volatile uint8_t *port, volatile uint8_t *inPort, uint8_t pin)
:_ddr(ddr),_port(port),_inPort(inPort), _pinMask(_BV(pin)), _pin(pin) {
}

AVRPortPin::~AVRPortPin(){

}

void AVRPortPin::setAsOutput(){
	*_ddr |= _pinMask;
}

void AVRPortPin::setAsInput(){
	*_ddr &= ~_pinMask;
}

void AVRPortPin::set(){
	*_port |= _pinMask;
}

void AVRPortPin::set(uint8_t val){
	val <<= _pin;
	*_port = val;
}

void AVRPortPin::clear(){
	*_port &= ~_pinMask;
}

uint8_t AVRPortPin::read(){
	return (*_inPort & _pinMask)?TRUE:FALSE;
}

void AVRPortPin::toggle(){
	*_inPort |= _pinMask;
}

void AVRPortPin::pulse(uint16_t millis) {
	toggle();
	for(uint16_t count =0;count <millis;count++) {
		_delay_us(1);
	}
	toggle();
}