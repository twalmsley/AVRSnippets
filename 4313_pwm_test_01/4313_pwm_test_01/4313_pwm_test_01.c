/*
 * _4313_pwm_test_01.c
 *
 * Test program for four 9g servos
 *
 * Outputs 2 pwms on PB3 and PB4 (pins 15 and 16), and 2pwms on PB2 and PD5 (pins 14 and 9)
 *
 * Created: 26/12/2013 10:06:09
 *  Author: tony
 */
#define F_CPU 8000000UL
#define REPEAT 3

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	//
	//Set the output ports (4xoutput compare toggle pins)
	//
	OC1A_DDR |= (1<<OC1A_BIT) | (1<<OC1B_BIT);
	OC0A_DDR |= (1<<OC0A_BIT);
	OC0B_DDR |= (1<<OC0B_BIT);
	//
	// Set up 8-bit timer counter 0 for Fast PWM, 8MHz/1024, TOP is 255, time period is ~30ms but seems ok for these servos.
	//
	TCCR0A |= (1<<COM0A0) | (1<<COM0A1) | (1<<COM0B0) | (1<<COM0B1) | (1 << WGM00) | (1 << WGM01);
	TCCR0B |= (1<<CS00) | (1<<CS02);
	//
	// Set up 16-bit timer/counter 1 for Fast PWM, *MHz/8, TOP is 19999, time period is 20ms
	//
	TCCR1A |= (1<<WGM11) | (1<<COM1A1) | (1<<COM1A0) | (1<<COM1B1) | (1<<COM1B0);
	TCCR1B |= (1<<WGM12) | (1<<WGM13) | (1<<CS11);
	TCCR1C |= 0;
	ICR1 = 19999;//Sets the cycle to 20ms

//
// Send two servos from one end to the other every 0.5s, REPEAT times, in anti-phase.
//
	uint8_t i = 0;
    while(i++ < REPEAT)
    {
		OCR0A = 256-6;// Inverted, hence 256-6. 6*0.1192ms = 0.7ms
		OCR0B = 256-19;// Inverted, hence 256-19. 19*0.1192ms = 2.26ms
		OCR1A = ICR1 - 600;//pulse width 0.66ms
		OCR1B = ICR1 - 2300;//pulse width 2.3ms
		_delay_ms(500);
		OCR0A = 256-19;
		OCR0B = 256-6;
		OCR1A = ICR1 - 2300;
		OCR1B = ICR1 - 600;
		_delay_ms(500);
	}
    while(1)
    {
		//do nothing
    }
}