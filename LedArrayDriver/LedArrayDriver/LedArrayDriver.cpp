/*
* LedArrayDriver.cpp
*
* Created: 29/08/2012 21:07:41
*  Author: tony walmsley
*/
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/delay.h>
#include <string.h>
#include "CharMap.h"

#define SHIFT_PAUSE				10000
#define LED_PAUSE				10000
#define COLUMNS_PER_CHARACTER	8
#define ROWS_PER_CHARACTER		7
#define NBR_OF_DISPLAY_COLUMNS  80
#define FOREVER					while(1)

// pin 22 - PA0 - 74LS138 CS pin
#define chipSelect PORTA0
// pin 26 - PA4 - input A
#define databit0 PORTA4
// pin 25 - PA3 - input B
#define databit1 PORTA3
// pin 24 - PA2 - input C
#define databit2 PORTA2
//pin 28 = bit7 = PA7 - shiftRegBit
#define shiftRegBit PORTA6
//pin 31 = bit8 = PC6 - clock
#define clockBit PORTC6

#define CLOCK_PORT		PORTC
#define DATA_PORT		PORTA
#define DDR_DATA_PORT	DDRA
#define DDR_CLOCK_PORT	DDRC

#define CLOCK_OFF		CLOCK_PORT &= ~_BV(clockBit);
#define CLOCK_ON		CLOCK_PORT |= _BV(clockBit);
#define SHIFT_REG_ON	DATA_PORT |= _BV(shiftRegBit);
#define SHIFT_REG_OFF	DATA_PORT &= ~_BV(shiftRegBit);

#define SHIFT0			CLOCK_OFF SHIFT_REG_OFF _delay_us(SHIFT_PAUSE); CLOCK_ON
#define SHIFT1			CLOCK_OFF SHIFT_REG_ON _delay_us(SHIFT_PAUSE); CLOCK_ON

uint8_t * getMatrix(char letter) {
	switch(letter) {
		case 'A': return letter_A;
		case 'B': return letter_B;
		case 'C': return letter_C;
		case 'D': return letter_D;
		case 'E': return letter_E;
		case 'F': return letter_F;
		case 'G': return letter_G;
		case 'H': return letter_H;
		case 'I': return letter_I;
		case 'J': return letter_J;
		case 'K': return letter_K;
		case 'L': return letter_L;
		case 'M': return letter_M;
		case 'N': return letter_N;
		case 'O': return letter_O;
		case 'P': return letter_P;
		case 'Q': return letter_Q;
		case 'R': return letter_R;
		case 'S': return letter_S;
		case 'T': return letter_T;
		case 'U': return letter_U;
		case 'V': return letter_V;
		case 'W': return letter_W;
		case 'X': return letter_X;
		case 'Y': return letter_Y;
		case 'Z': return letter_Z;
		case '0': return letter_0;
		case '1': return letter_1;
		case '2': return letter_2;
		case '3': return letter_3;
		case '4': return letter_4;
		case '5': return letter_5;
		case '6': return letter_6;
		case '7': return letter_7;
		case '8': return letter_8;
		case '9': return letter_9;
		case ' ': return space;
		default: return letter_question;
	}
}
/*
void send(char letter) {
	uint8_t *matrix = getMatrix(letter);
	//
	// Each character has a number of columns
	//
	for (uint8_t column = 0; column < COLUMNS_PER_CHARACTER; column++) {
		//
		// Process each bit in the column
		//
		for(uint8_t row = 0; row < ROWS_PER_CHARACTER; row++) {
			uint8_t pattern = matrix[row];
			if(pattern & _BV(column)) {
				//
				// The bit is set so light the LED in the row
				//
				DATA_PORT = (row << 2) | _BV(chipSelect);
				_delay_us(LED_PAUSE);
			}
		}
		//
		// Clear the column and move to the next one
		//
		DATA_PORT = 0;
		SHIFT0
	}
	
}

int main(void) {
	char *message = "HELLO WORLD";
	//
	//Set up the DDR registers for output
	//
	DDR_DATA_PORT = _BV(chipSelect)|_BV(databit0)|_BV(databit1)|_BV(databit2)|_BV(shiftRegBit);
	DDR_CLOCK_PORT = _BV(clockBit);
	do
	{
		char *messagePtr = message;
		//
		// Set up and clock-in the first shift register bit
		//
		SHIFT1
		//
		// Process each character in the message
		//
		while(*messagePtr) {
			send(*messagePtr++);
		}
	} FOREVER;
}
*/
int main(void) {
	char *message = "HELLO WORLD";
	//
	//Set up the DDR registers for output
	//
	DDR_DATA_PORT = 0xFF;
	DDR_CLOCK_PORT = _BV(clockBit);
	DATA_PORT = 0;
	SHIFT1
	uint8_t val = 0;
	do
	{
		DATA_PORT = val & ~_BV(shiftRegBit);
		val++;
		_delay_ms(100);
	} FOREVER;
}
