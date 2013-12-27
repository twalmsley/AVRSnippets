/*
 * WiiServer2560.c
 *
 * Created: 26/12/2013 10:06:09
 *  Author: tony
 */
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>
#include "Wire.h"
#include "LCD_2560.h"
#include "WiiNunchuk.h"

//#define SERVOS

#define ICR1_MAX 39999
#define SERVO_9G_MIN 1200
#define SERVO_9G_MAX 4600
#define REPEAT 1
#define STEP_DELAY 500

#define TWI_CMD_MASTER_READ  0x20

void showTWIStatus(uint8_t statusCount)
{
		//
		// Display the status
		//
		char hex[3] = {' ',' ',0x00};
		setPosition(0,0);
		display("Count:              ");
		setPosition(0,8);
		to_hex(statusCount, hex);
		display(hex);
		display(" ");
		to_hex(TW_STATUS, hex);
		display(hex);
		_delay_ms(500);
}

void showHex(uint8_t value)
{
	//
	// Display the status
	//
	char hex[3] = {' ',' ',0x00};
	to_hex(value, hex);
	display(hex);
}

int main(void)
{
	uint8_t statusCount = 0;
	//
	// Set up the LCD
	//
	init_lcd();

#ifdef SERVOS
	setPosition(0,0);
	display("Moving Servos...");
	//
	//Set the output ports
	//
	DDRB |= _BV(PORTB5) | _BV(PORTB6) | _BV(PORTB7); // OC1A, OC1B and OC1C, PB5, PB6 and PB7, Arduino pins 11, 12 and 13
	DDRB |= _BV(PORTB4);
	DDRE |= _BV(PORTE3);

	//
	// Set up 16-bit timer/counter 1 for Fast PWM, 16MHz/8, TOP is ICR1_MAX, time period is 20ms
	//
	TCCR1A |= (1<<WGM11) | (1<<COM1A1) | (1<<COM1A0) | (1<<COM1B1) | (1<<COM1B0) | (1<<COM1C1) | (1<<COM1C0);
	TCCR1B |= (1<<WGM12) | (1<<WGM13) | (1<<CS11);
	TCCR1C |= 0;
	ICR1 = ICR1_MAX;//Sets the cycle to 20ms
	//
	// Set up 16-bit timer/counter 3 for Fast PWM, 16MHz/8, TOP is ICR1_MAX, time period is 20ms
	//
	TCCR3A |= (1<<WGM31) | (1<<COM3A1) | (1<<COM3A0);
	TCCR3B |= (1<<WGM32) | (1<<WGM33) | (1<<CS31);
	TCCR3C |= 0;
	ICR3 = ICR1_MAX;//Sets the cycle to 20ms

//
// Send two servos from one end to the other every 0.5s, REPEAT times, in anti-phase.
//
	uint8_t i = 0;
    while(i++ < REPEAT)
    {
		OCR1A = ICR1 - SERVO_9G_MIN;
		OCR3A = ICR3 - SERVO_9G_MIN;
		OCR1B = ICR1 - SERVO_9G_MIN;
		OCR1C = ICR1 - SERVO_9G_MIN;
		_delay_ms(STEP_DELAY);
		OCR1A = ICR1 - SERVO_9G_MAX;
		OCR3A = ICR3 - SERVO_9G_MAX;
		OCR1B = ICR1 - SERVO_9G_MAX;
		OCR1C = ICR1 - SERVO_9G_MAX;
		_delay_ms(STEP_DELAY);
	}
#endif
	//
	// init nunchuk
	//
	sei();
	WiiNunchuk wiiClassy = WiiNunchuk();
	wiiClassy.begin();

    while(1)
    {
		_delay_ms(100);
		wiiClassy.update();

		displayAt(0,0,"x: ");
		showHex(wiiClassy.x());

		displayAt(0,6,"Xa:");
		showHex(wiiClassy.xacc());

		displayAt(0,11,",Ya:");
		showHex(wiiClassy.yacc());

		displayAt(1,6,"Za:");
		showHex(wiiClassy.zacc());

		setPosition(1,12);
		showHex(wiiClassy.z());

		setPosition(1,15);
		showHex(wiiClassy.c());

		displayAt(1,0,"y: ");
		showHex(wiiClassy.y());
    }
}