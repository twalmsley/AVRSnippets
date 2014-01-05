/*
 * _4313_bot.c
 *
 * Created: 03/01/2014 16:34:53
 *  Author: tony
 */


#include <avr/io.h>
#include <avr/delay.h>

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

int main(void)
{
	DDRB = 0xFF;// Two 4-bit steppers on port B
	PORTB = 0x00;// All off

	uint8_t m1 = 0, m2 = 0;
    while(1)
    {
		//
		// m1
		//
        for(uint16_t i = 0; i < 512; i++) {
			PORTB = (PORTB & 0xF0) | steps[m1 & 0x07];
			m1++;
			_delay_ms(10);
		}
		_delay_ms(1000);
		//
		// m2
		//
        for(uint16_t i = 0; i < 512; i++) {
	        PORTB = (PORTB & 0x0F) | (steps[m2 & 0x07]<<4);
	        m2++;
	        _delay_ms(10);
        }
        _delay_ms(1000);
    }
}