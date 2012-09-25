#ifndef _APERTURES_H
#define _APERTURES_H

#include <stdint.h>

class Aperture
{
	public:
	Aperture(uint16_t *ddr,uint16_t *port,uint8_t pin);
	~Aperture();
	
	uint8_t isOpen();
	
	protected:
	private:
	uint16_t *_ddr;
	uint16_t *_port;
	uint8_t _pin;
	uint8_t _state;
};

class Door : public Aperture
{
	public:
	Door(uint16_t *ddr,uint16_t *port, uint8_t pin, char* doorName);
	~Door();
	protected:
	private:
	char* _name;
};
#endif