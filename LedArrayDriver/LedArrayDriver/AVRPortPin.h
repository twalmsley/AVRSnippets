#ifndef AVRPORTPIN_H
#define AVRPORTPIN_H

#include <avr/io.h>
#include "PortPin.h"

class AVRPortPin : public PortPin {
	public:
	AVRPortPin(volatile uint8_t *ddr, volatile uint8_t *port, volatile uint8_t *inPort, uint8_t pin);
	~AVRPortPin();

	virtual void setAsOutput();
	virtual void setAsInput();
	virtual void set();
	virtual void set(uint8_t val);
	virtual void clear();
	virtual uint8_t read();
	virtual void toggle();
	virtual void pulse(uint16_t millis);
	private:
	volatile uint8_t *_ddr;
	volatile uint8_t *_port;
	volatile uint8_t *_inPort;
	uint8_t _pinMask;
	uint8_t _pin;
};
#endif