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

int main(void)
{
	unsigned char messageBuf[6];// data from the nunchuk
	uint8_t statusCount = 0;
	//
	// Set up the LCD
	//
	init_lcd();
	setPosition(0,0);
	display("Initialise TWI...");
	//
	// Initialise TWI for the WII nunchuk
	//
	/* Initial TWI Peripheral */
	TWSR = 0x00;   // Select Prescaler of 1

	// SCL frequency = 16000000 / (16 + 2 * 72 * 1) = 100 khz
	TWBR = 72;

	showTWIStatus(statusCount++);
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
// Next step message
//
	setPosition(0,0);
	display("Initialising nunchuk");
	//
	// init nunchuk
	//

	messageBuf[0] = 0xA4;
	messageBuf[1] = 0xF0;
	messageBuf[2] = 0x55;
	TWI_Start_Transceiver_With_Data( messageBuf, 3 );
	showTWIStatus(statusCount++);
	messageBuf[0] = 0xA4;
	messageBuf[1] = 0xFB;
	messageBuf[2] = 0x00;
	TWI_Start_Transceiver_With_Data( messageBuf, 3 );
	showTWIStatus(statusCount++);
    while(1)
    {
		//
		// Request bytes
		//
		messageBuf[0] = 0xA4;
		messageBuf[1] = 0x00;
		TWI_Start_Transceiver_With_Data( messageBuf, 2 );
		showTWIStatus(statusCount++);
		messageBuf[0] = 0xA5;
		TWI_Start_Transceiver_With_Data( messageBuf, 1 );
		showTWIStatus(statusCount++);
		//
		// Read bytes
		//
		TWI_Get_Data_From_Transceiver( messageBuf, 1 );
		showTWIStatus(statusCount++);
		//
		// Decode Buffer
		//
		for(int i = 0;i<5;i++) {
			messageBuf[i] = (messageBuf[i]^0x17) + 0x17;
		}
		showTWIStatus(statusCount++);
		//
		// Do something with the data
		//
		/*
		char hex[3] = {' ',' ',0x00};
		setPosition(0,0);
		display("Data:               ");
		setPosition(0,6);
		to_hex(messageBuf[0], hex);
		display(hex);
		display(" ");
		to_hex(messageBuf[1], hex);
		display(hex);
		display(" ");
		to_hex(messageBuf[2], hex);
		display(hex);
		setPosition(1,0);
		display("Data: ");
		to_hex(messageBuf[3], hex);
		display(hex);
		display(" ");
		to_hex(messageBuf[4], hex);
		display(hex);
		display(" ");
		to_hex(messageBuf[5], hex);
		display(hex);
		*/
		_delay_ms(100);
    }
}