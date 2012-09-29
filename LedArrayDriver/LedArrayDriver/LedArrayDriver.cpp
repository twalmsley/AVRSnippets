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

// $00 - Character: ' '          CHR$(0)
uint8_t space[] = {
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000
};
// $0B - Character: '"'          CHR$(11)
uint8_t letter_quotes[] = {
	0b00000000,
	0b00100100,
	0b00100100,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000
};
// $0B - Character:  £           CHR$(12)
uint8_t letter_pound[] = {
	0b00000000,
	0b00011100,
	0b00100010,
	0b01111000,
	0b00100000,
	0b00100000,
	0b01111110
};
// $0B - Character: '$'          CHR$(13)
uint8_t letter_dollar[] = {
	0b00000000,
	0b00001000,
	0b00111110,
	0b00101000,
	0b00111110,
	0b00001010,
	0b00111110
};
// $0B - Character: ':'          CHR$(14)
uint8_t letter_colon[] = {
	0b00000000,
	0b00000000,
	0b00000000,
	0b00010000,
	0b00000000,
	0b00000000,
	0b00010000
};
// $0B - Character: '?'          CHR$(15)
uint8_t letter_question[] = {
	0b00000000,
	0b00111100,
	0b01000010,
	0b00000100,
	0b00001000,
	0b00000000,
	0b00001000
};
// $10 - Character: '('          CHR$(16)
uint8_t letter_leftbrace[] = {
	0b00000000,
	0b00000100,
	0b00001000,
	0b00001000,
	0b00001000,
	0b00001000,
	0b00000100
};
// $11 - Character: ')'          CHR$(17)
uint8_t letter_rightbrace[] = {
	0b00000000,
	0b00100000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00100000
};
// $12 - Character: '>'          CHR$(18)
uint8_t letter_greater[] = {
	0b00000000,
	0b00000000,
	0b00010000,
	0b00001000,
	0b00000100,
	0b00001000,
	0b00010000
};
// $13 - Character: '<'          CHR$(19)
uint8_t letter_lesser[] = {
	0b00000000,
	0b00000000,
	0b00000100,
	0b00001000,
	0b00010000,
	0b00001000,
	0b00000100
};
// $14 - Character: '='          CHR$(20)
uint8_t letter_equal[] = {
	0b00000000,
	0b00000000,
	0b00000000,
	0b00111110,
	0b00000000,
	0b00111110,
	0b00000000
};
// $15 - Character: '+'          CHR$(21)
uint8_t letter_plus[] = {
	0b00000000,
	0b00000000,
	0b00001000,
	0b00001000,
	0b00111110,
	0b00001000,
	0b00001000
};
// $16 - Character: '-'          CHR$(22)
uint8_t letter_minus[] = {
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00111110,
	0b00000000,
	0b00000000
};
// $17 - Character: '*'          CHR$(23)
uint8_t letter_star[] = {
	0b00000000,
	0b00000000,
	0b00010100,
	0b00001000,
	0b00111110,
	0b00001000,
	0b00010100
};
// $18 - Character: '/'          CHR$(24)
uint8_t letter_slash[] = {
	0b00000000,
	0b00000000,
	0b00000010,
	0b00000100,
	0b00001000,
	0b00010000,
	0b00100000
};
// $19 - Character: '//'          CHR$(25)
uint8_t letter_semicolon[] = {
	0b00000000,
	0b00000000,
	0b00010000,
	0b00000000,
	0b00000000,
	0b00010000,
	0b00010000
};
// $1A - Character: ','          CHR$(26)
uint8_t letter_comma[] = {
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00001000,
	0b00001000
};
// $1B - Character: '"'          CHR$(27)
uint8_t letter_fullstop[] = {
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00011000,
	0b00011000
};
// $1C - Character: '0'          CHR$(28)
uint8_t letter_0[] = {
	0b00000000,
	0b00111100,
	0b01000110,
	0b01001010,
	0b01010010,
	0b01100010,
	0b00111100
};
// $1D - Character: '1'          CHR$(29)
uint8_t letter_1[] = {
	0b00000000,
	0b00011000,
	0b00101000,
	0b00001000,
	0b00001000,
	0b00001000,
	0b00111110
};
// $1E - Character: '2'          CHR$(30)
uint8_t letter_2[] = {
	0b00000000,
	0b00111100,
	0b01000010,
	0b00000010,
	0b00111100,
	0b01000000,
	0b01111110
};
// $1F - Character: '3'          CHR$(31)
uint8_t letter_3[] = {
	0b00000000,
	0b00111100,
	0b01000010,
	0b00001100,
	0b00000010,
	0b01000010,
	0b00111100
};
// $20 - Character: '4'          CHR$(32)
uint8_t letter_4[] = {
	0b00000000,
	0b00001000,
	0b00011000,
	0b00101000,
	0b01001000,
	0b01111110,
	0b00001000
};
// $21 - Character: '5'          CHR$(33)
uint8_t letter_5[] = {
	0b00000000,
	0b01111110,
	0b01000000,
	0b01111100,
	0b00000010,
	0b01000010,
	0b00111100
};
// $22 - Character: '6'          CHR$(34)
uint8_t letter_6[] = {
	0b00000000,
	0b00111100,
	0b01000000,
	0b01111100,
	0b01000010,
	0b01000010,
	0b00111100
};
// $23 - Character: '7'          CHR$(35)
uint8_t letter_7[] = {
	0b00000000,
	0b01111110,
	0b00000010,
	0b00000100,
	0b00001000,
	0b00010000,
	0b00010000
};
// $24 - Character: '8'          CHR$(36)
uint8_t letter_8[] = {
	0b00000000,
	0b00111100,
	0b01000010,
	0b00111100,
	0b01000010,
	0b01000010,
	0b00111100
};
// $25 - Character: '9'          CHR$(37)
uint8_t letter_9[] = {
	0b00000000,
	0b00111100,
	0b01000010,
	0b01000010,
	0b00111110,
	0b00000010,
	0b00111100
};
// $26 - Character: 'A'          CHR$(38)
uint8_t letter_A[] = {
	0b00000000,
	0b00111100,
	0b01000010,
	0b01000010,
	0b01111110,
	0b01000010,
	0b01000010
};
// $27 - Character: 'B'          CHR$(39)
uint8_t letter_B[] = {
	0b00000000,
	0b01111100,
	0b01000010,
	0b01111100,
	0b01000010,
	0b01000010,
	0b01111100
};
// $28 - Character: 'C'          CHR$(40)
uint8_t letter_C[] = {
	0b00000000,
	0b00111100,
	0b01000010,
	0b01000000,
	0b01000000,
	0b01000010,
	0b00111100
};
// $29 - Character: 'D'          CHR$(41)
uint8_t letter_D[] = {
	0b00000000,
	0b01111000,
	0b01000100,
	0b01000010,
	0b01000010,
	0b01000100,
	0b01111000
};
// $2A - Character: 'E'          CHR$(42)
uint8_t letter_E[] = {
	0b00000000,
	0b01111110,
	0b01000000,
	0b01111100,
	0b01000000,
	0b01000000,
	0b01111110
};
// $2B - Character: 'F'          CHR$(43)
uint8_t letter_F[] = {
	0b00000000,
	0b01111110,
	0b01000000,
	0b01111100,
	0b01000000,
	0b01000000,
	0b01000000
};
// $2C - Character: 'G'          CHR$(44)
uint8_t letter_G[] = {
	0b00000000,
	0b00111100,
	0b01000010,
	0b01000000,
	0b01001110,
	0b01000010,
	0b00111100
};
// $2D - Character: 'H'          CHR$(45)
uint8_t letter_H[] = {
	0b00000000,
	0b01000010,
	0b01000010,
	0b01111110,
	0b01000010,
	0b01000010,
	0b01000010
};
// $2E - Character: 'I'          CHR$(46)
uint8_t letter_I[] = {
	0b00000000,
	0b00111110,
	0b00001000,
	0b00001000,
	0b00001000,
	0b00001000,
	0b00111110
};
// $2F - Character: 'J'          CHR$(47)
uint8_t letter_J[] = {
	0b00000000,
	0b00000010,
	0b00000010,
	0b00000010,
	0b01000010,
	0b01000010,
	0b00111100
};
// $30 - Character: 'K'          CHR$(48)
uint8_t letter_K[] = {
	0b00000000,
	0b01000100,
	0b01001000,
	0b01110000,
	0b01001000,
	0b01000100,
	0b01000010
};
// $31 - Character: 'L'          CHR$(49)
uint8_t letter_L[] = {
	0b00000000,
	0b01000000,
	0b01000000,
	0b01000000,
	0b01000000,
	0b01000000,
	0b01111110
};
// $32 - Character: 'M'          CHR$(50)
uint8_t letter_M[] = {
	0b00000000,
	0b01000010,
	0b01100110,
	0b01011010,
	0b01000010,
	0b01000010,
	0b01000010
};
// $33 - Character: 'N'          CHR$(51)
uint8_t letter_N[] = {
	0b00000000,
	0b01000010,
	0b01100010,
	0b01010010,
	0b01001010,
	0b01000110,
	0b01000010
};
// $34 - Character: 'O'          CHR$(52)
uint8_t letter_O[] = {
	0b00000000,
	0b00111100,
	0b01000010,
	0b01000010,
	0b01000010,
	0b01000010,
	0b00111100
};
// $35 - Character: 'P'          CHR$(53)
uint8_t letter_P[] = {
	0b00000000,
	0b01111100,
	0b01000010,
	0b01000010,
	0b01111100,
	0b01000000,
	0b01000000
};
// $36 - Character: 'Q'          CHR$(54)
uint8_t letter_Q[] = {
	0b00000000,
	0b00111100,
	0b01000010,
	0b01000010,
	0b01010010,
	0b01001010,
	0b00111100
};
// $37 - Character: 'R'          CHR$(55)
uint8_t letter_R[] = {
	0b00000000,
	0b01111100,
	0b01000010,
	0b01000010,
	0b01111100,
	0b01000100,
	0b01000010
};
// $38 - Character: 'S'          CHR$(56)
uint8_t letter_S[] = {
	0b00000000,
	0b00111100,
	0b01000000,
	0b00111100,
	0b00000010,
	0b01000010,
	0b00111100
};
// $39 - Character: 'T'          CHR$(57)
uint8_t letter_T[] = {
	0b00000000,
	0b11111110,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010000,
	0b00010000
};
// $3A - Character: 'U'          CHR$(58)
uint8_t letter_U[] = {
	0b00000000,
	0b01000010,
	0b01000010,
	0b01000010,
	0b01000010,
	0b01000010,
	0b00111100
};
// $3B - Character: 'V'          CHR$(59)
uint8_t letter_V[] = {
	0b00000000,
	0b01000010,
	0b01000010,
	0b01000010,
	0b01000010,
	0b00100100,
	0b00011000
};
// $3C - Character: 'W'          CHR$(60)
uint8_t letter_W[] = {
	0b00000000,
	0b01000010,
	0b01000010,
	0b01000010,
	0b01000010,
	0b01011010,
	0b00100100
};
// $3D - Character: 'X'          CHR$(61)
uint8_t letter_X[] = {
	0b00000000,
	0b01000010,
	0b00100100,
	0b00011000,
	0b00011000,
	0b00100100,
	0b01000010
};
// $3E - Character: 'Y'          CHR$(62)
uint8_t letter_Y[] = {
	0b00000000,
	0b10000010,
	0b01000100,
	0b00101000,
	0b00010000,
	0b00010000,
	0b00010000
};
// $3F - Character: 'Z'          CHR$(63)
uint8_t letter_Z[] = {
	0b00000000,
	0b01111110,
	0b00000100,
	0b00001000,
	0b00010000,
	0b00100000,
	0b01111110
};
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
