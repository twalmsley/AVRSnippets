/*
* LedArrayDriver.cpp
*
* Created: 29/08/2012 21:07:41
*  Author: tony walmsley
*/
#include "LedArrayDriver.h"

LedArrayDriver::LedArrayDriver(	PortPin *clock,
								PortPin *chipSelect,
								PortPin *shift,
								PortPin *a,
								PortPin *b,
								PortPin *c,
								uint8_t rows,
								uint16_t columns)
	:	_clock(clock),
		_chipSelect(chipSelect),
		_shift(shift),
		_a(a),
		_b(b),
		_c(c),
		_rows(rows),
		_columns(columns) {

}

LedArrayDriver::~LedArrayDriver() {
}

void LedArrayDriver::init() {
	_clock->setAsOutput();
	_clock->clear();
	_chipSelect->setAsOutput();
	_chipSelect->clear();
	_shift->setAsOutput();
	_shift->clear();
	_a->setAsOutput();
	_a->clear();
	_b->setAsOutput();
	_b->clear();
	_c->setAsOutput();
	_c->clear();

	clearDisplay();
}

void LedArrayDriver::clearDisplay() {
	_shift->clear();
	for(uint16_t col = 0;col <_columns;col++) {
		_clock->pulse(PULSE_MILLIS);
	}
}

void LedArrayDriver::begin() {
	_shift->set();
	_clock->pulse(PULSE_MILLIS);
	_shift->clear();
}

void LedArrayDriver::nextColumn() {
	_clock->pulse(PULSE_MILLIS);
}

void LedArrayDriver::show(uint8_t bits) {

	for(uint8_t row = 1; row <= _rows; row++) {
		if(bits & _BV(row-1)) {
			//
			// The bit is set so light the LED in the row
			//
			_a->set(row);
		}
		_delay_us(LED_PAUSE);
	}
	//
	// Clear the column and move to the next one
	//
	_a->set(0);
	_delay_us(LED_PAUSE);
}
