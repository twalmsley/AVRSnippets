/*
 * _4313_bot.c
 *
 * Created: 03/01/2014 16:34:53
 *  Author: tony
 */

#define F_CPU 4000000

#include <avr/io.h>
#include <util/delay.h>

static uint8_t steps[] = {
		0b0001,
		0b0011,
		0b0010,
		0b0110,
		0b0100,
		0b1100,
		0b1000,
		0b1001
	};

static uint8_t m1 = 0, m2 = 0;
static uint8_t m1dir = 1, m2dir = 0xFF;// 0xFF is effectively -1

void nextM1Step() {
	PORTB = (PORTB & 0xF0) | steps[m1 & 0x07];
	m1+=m1dir;
}

void nextM2Step() {
	PORTB = (PORTB & 0x0F) | (steps[m2 & 0x07]<<4);
	m2+=m2dir;
}

void move(uint16_t m1count, uint16_t m2count) {
	while(m1count > 0 || m2count > 0) {
		if(m1count > 0) {
			nextM1Step();
			m1count--;
		}
		if(m2count > 0) {
			nextM2Step();
			m2count--;
		}
		_delay_ms(5);
	}
}

void stepperTest() 
{
    while(1)
    {
		//
		// m1
		//
        for(uint16_t i = 0; i < 512; i++) {
			nextM1Step();
			_delay_ms(5);
		}
		_delay_ms(1000);
		//
		// m2
		//
        for(uint16_t i = 0; i < 512; i++) {
	        nextM2Step();
	        _delay_ms(5);
        }
        _delay_ms(1000);
    }
}

int main(void)
{
	DDRB = 0xFF;// Two 4-bit steppers on port B
	PORTB = 0x00;// All off

	//stepperTest();
	move(4096, 4096);
	move(0, 4096);
	move(4096, 4096);
	move(0, 4096);
	move(4096, 4096);
	move(0, 4096);
	move(4096, 4096);
}