/*
 * _4313_cnc.c
 *
 * Created: 03/01/2014 16:34:53
 *  Author: tony
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#define NINETY_DEGREES 2200/2
#define STEP_DELAY _delay_ms(1)

static uint8_t steps[] = { 0b0001, 0b0011, 0b0010, 0b0110, 0b0100, 0b1100,
		0b1000, 0b1001 };

static long x = 0, y = 0;
static short int xdir = 0, ydir = 1; // Assume we start in the Y direction
static uint8_t m1 = 0, m2 = 0;
static uint8_t m1dir = 0xFF, m2dir = 1; // 0xFF is effectively -1
static uint8_t tmpPortB = 0; //Used to prepare the next value for PORTB

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
	while (distance > 0) {
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
	while (distance > 0) {
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
	while (distance > 0) {
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

int main(void) {
	DDRB = 0xFF; // Two 4-bit steppers on port B
	PORTB = 0x00; // All off
	DDRA = 0x00;
	PORTA |= 0x01;

	_delay_ms(1000);

	uint8_t count = 3;
	while(count-- > 0) {
		turnLeft90();
		turnRight90();
		move(2000);
	}
	while(1);
}
