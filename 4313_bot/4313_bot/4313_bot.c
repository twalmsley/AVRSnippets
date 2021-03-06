/*
 * _4313_bot.c
 *
 * Created: 03/01/2014 16:34:53
 *  Author: tony
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#define NINETY_DEGREES 2200/2
#define STEP_DELAY 		while(!(PINA & 0x01));if((PIND & (1<<2)) == 0) shouldRun = 0;_delay_us(750)

static uint8_t steps[] = { 0b0001, 0b0011, 0b0010, 0b0110, 0b0100, 0b1100,
		0b1000, 0b1001 };

static long x = 0, y = 0;
static short int xdir = 0, ydir = 1; // Assume we start in the Y direction
static uint8_t m1 = 0, m2 = 0;
static uint8_t m1dir = 0xFF, m2dir = 1; // 0xFF is effectively -1
static uint8_t tmpPortB = 0; //Used to prepare the next value for PORTB
static uint8_t shouldRun = 0;

void nextM1Step() {
	tmpPortB = (tmpPortB & 0xF0) | steps[m1 & 0x07];
	m1 += m1dir;
}

void nextM2Step() {
	tmpPortB = (tmpPortB & 0x0F) | (steps[m2 & 0x07] << 4);
	m2 += m2dir;
}

void reverseM1Step() {
	tmpPortB = (tmpPortB & 0xF0) | steps[m1 & 0x07];
	m1 -= m1dir;
}

void reverseM2Step() {
	tmpPortB = (tmpPortB & 0x0F) | (steps[m2 & 0x07] << 4);
	m2 -= m2dir;
}

void go() {
	PORTB = tmpPortB;
}
void move(uint16_t distance) {
	while (distance > 0 && shouldRun) {
		nextM1Step();
		nextM2Step();
		distance--;
		x += xdir;
		y += ydir;
		go();
		STEP_DELAY;
	}
}

void turnRight90() {
	uint16_t distance = NINETY_DEGREES;
	while (distance > 0 && shouldRun) {
		nextM2Step();
		reverseM1Step();
		distance--;
		go();
		STEP_DELAY;
	}

	if (xdir == 1 && ydir == 0) {
		xdir = 0, ydir = -1;
	} else if (xdir == -1 && ydir == 0) {
		xdir = 0, ydir = 1;
	} else if (xdir == 0 && ydir == 1) {
		xdir = 1, ydir = 0;
	} else if (xdir == 0 && ydir == -1) {
		xdir = -1, ydir = 0;
	}
}

void turnLeft90() {
	uint16_t distance = NINETY_DEGREES;
	while (distance > 0 && shouldRun) {
		nextM1Step();
		reverseM2Step();
		distance--;
		go();
		STEP_DELAY;
	}

	if (xdir == 0 && ydir == 1) {
		xdir = -1, ydir = 0;
	} else if (xdir == 0 && ydir == -1) {
		xdir = 1, ydir = 0;
	} else if (xdir == 1 && ydir == 0) {
		xdir = 0, ydir = 1;
	} else if (xdir == -1 && ydir == 0) {
		xdir = 0, ydir = -1;
	}
}

void USART_Transmit(unsigned char data) {
	while (!(UCSRA & (1 << UDRE)))
		;
	UDR = data;
}

unsigned char USART_Receive() {
	while (!(UCSRA & (1 << RXC)))
		;

	return UDR ;
}

int main(void) {
	DDRB = 0xFF; // Two 4-bit steppers on port B
	PORTB = 0x00; // All off
	DDRA = 0x00;
	PORTA |= 0x01;

	DDRD = 0x00;
	PORTD |= 1<<2;
	//
	// Init the USART
	//
	UBRRH = 0;
	UBRRL = 25;	//19.2Kbaud
	UCSRA = _BV(U2X);
	UCSRB = _BV(RXEN) | _BV(TXEN);
	UCSRC = _BV(UCSZ0) | _BV(UCSZ1);

	_delay_ms(1000);

	while(1) {
		while((PIND & (1<<2)) || shouldRun);// wait for the switch on PA0
		shouldRun = 1;

		USART_Transmit('R');// Say we're ready

		while (shouldRun) {
			unsigned char c = USART_Receive();
			switch (c) {
			case 'L':
				turnLeft90();
				USART_Transmit('R');// Say we're ready
				break;
			case 'R':
				turnRight90();
				USART_Transmit('R');// Say we're ready
				break;
			case 'F':
				c = USART_Receive();
				uint16_t distance = c<<8;
				distance |= USART_Receive();
				move(distance);
				USART_Transmit('R');// Say we're ready
				break;
			}
		}
	}
}
