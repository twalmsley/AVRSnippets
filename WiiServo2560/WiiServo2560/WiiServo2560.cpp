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

/*
 * (DO = Digital Out)
 *
 *	Arduino Pin Id		AVR Pin Id		Used As		Destination
 *	--------------		----------		-------		-----------
 *	2					PE4				DO			LED
 *	3					PE5				DO			LED
 *	5					PE3				PWM			Servo1
 *	11					PB5				PWM			Servo2
 *	12					PB6				PWM			Servo3
 *	13					PB7				PWM			Servo4
 *	20					PD1				SDA			Nunchuk green
 *	21					PD0				SCL			Nunchuk yellow
 *	31					PC6				DO			LCD 4	Register Select (RS). RS=0: Command, RS=1: Data
 *	32					PC5				DO			LCD 5	Read/Write (R/W). R/W=0: Write, R/W=1: Read
 *	33					PC4				DO			LCD 6	Clock (Enable). Falling edge triggered
 *	34					PC3				DO			LCD 14	Bit 4
 *	35					PC2				DO			LCD 13	Bit 5
 *	36					PC1				DO			LCD 12	Bit 6
 *	37					PC0				DO			LCD 11	Bit 7
 *
 *	Nunchuk Pinout
 *	--------------
 *	white		GND
 *	red			3.3V (possibly ok at 5V)
 *	green		SDA	+ pullup resistor to red
 *	yellow		CLK	+ pullup resistor to red
 *
 *	LCD Pinout (lifted from wikipedia)
 *	----------------------------------
 *
 *	1 Ground
 *	2 VCC (+3.3 to +5V)
 *	3 Contrast adjustment (VO) -						*******Wire to VCC for maximum contrast  *********
 *	4 Register Select (RS). RS=0: Command, RS=1: Data
 *	5 Read/Write (R/W). R/W=0: Write, R/W=1: Read
 *	6 Clock (Enable). Falling edge triggered
 *	7 Bit 0 (Not used in 4-bit operation)
 *	8 Bit 1 (Not used in 4-bit operation)
 *	9 Bit 2 (Not used in 4-bit operation)
 *	10 Bit 3 (Not used in 4-bit operation)
 *	11 Bit 4
 *	12 Bit 5
 *	13 Bit 6
 *	14 Bit 7
 *	15 Backlight Anode (+)
 *	16 Backlight Cathode (-)
 *
*/

//#define SERVOS

#define ICR1_MAX 39999
#define SERVO_9G_MIN 1172
#define SERVO_9G_MAX 5063
#define SERVO_9G_RANGE (SERVO_9G_MAX - SERVO_9G_MIN)

#define ACC_MIN 70
#define ACC_MAX 180
#define ACC_RANGE (ACC_MAX-ACC_MIN)

#define JOY_MIN 70
#define JOY_MAX 180
#define JOY_RANGE (JOY_MAX-JOY_MIN)

#define ACC_M_VAL ((float)SERVO_9G_RANGE/(float)ACC_RANGE)
#define ACC_C_VAL ((float)SERVO_9G_MIN-ACC_MIN*ACC_M_VAL)

#define JOY_M_VAL ((float)SERVO_9G_RANGE/(float)JOY_RANGE)
#define JOY_C_VAL ((float)SERVO_9G_MIN-JOY_MIN*JOY_M_VAL)

#define REPEAT 1
#define STEP_DELAY 500

#define TWI_CMD_MASTER_READ  0x20

void showHex(uint8_t value)
{
	//
	// Display the status
	//
	char hex[3] = {' ',' ',0x00};
	to_hex(value, hex);
	display(hex);
}

uint16_t accToServoRange(uint8_t val) {
	float fval = (float)val;
	uint16_t result = fval*ACC_M_VAL+ACC_C_VAL;
	return result;
}

uint16_t joyToServoRange(uint8_t val) {
	float fval = (float)val;
	uint16_t result = fval*JOY_M_VAL+JOY_C_VAL;
	return result;
}

void showNunchukData( WiiNunchuk &wiiClassy )
{

		displayAt(0,0,"x: ");
		showHex(wiiClassy.x());

		displayAt(1,0,"y: ");
		showHex(wiiClassy.y());

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
}

int main(void)
{
	uint8_t statusCount = 0;
	//
	// Set up the LCD
	//
	init_lcd();

	// Set up PE4 + PE5 (arduino pins 2 and 3) to show the wii nunchuk z and c buttons
	DDRE |= _BV(PORTE4) | _BV(PORTE5);
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

#ifdef SERVOS
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
		wiiClassy.update();

		showNunchukData(wiiClassy);


		OCR1A = ICR1_MAX - accToServoRange(wiiClassy.xacc() & 0xFC);
		OCR3A = ICR3 - accToServoRange(wiiClassy.yacc() & 0xFC);

		OCR1B = ICR1_MAX - joyToServoRange(wiiClassy.x());
		OCR1C = ICR1 - joyToServoRange(wiiClassy.y());

		if(wiiClassy.z()) {
			PORTE |= _BV(PORTE4);
		} else {
			PORTE &= ~_BV(PORTE4);
		}

		if(wiiClassy.c()) {
			PORTE |= _BV(PORTE5);
		} else {
			PORTE &= ~_BV(PORTE5);
		}
    }
}