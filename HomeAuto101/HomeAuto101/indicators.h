#ifndef _INDICATORS_H
#define _INDICATORS_H

#include <stdint.h>

class Indicator
{
	public:
	Indicator(uint16_t *ddr,uint16_t *port,uint8_t pin);
	~Indicator();
	
	void set();
	void clear();
	void toggle();
	protected:
	private:
	uint16_t *_ddr;
	uint16_t *_port;
	uint8_t _pin;
};

#endif