#define F_CPU 16000000
#define ARDUINO 100
#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DOT 1
#define DASH 5
#define intertickGap 3
#define charGap 10
#define wordGap 30

void delay_10ths(uint8_t count) {
	do {
		_delay_ms(100);
	} while(--count);
}

inline void send(const uint8_t ticks) {
	uint8_t count = (ticks & 0xF0)>>4;
	uint8_t bits = ticks & 0x0F;
	do 
	{
		PORTB |= _BV(PORTB7);
		(bits & (_BV(count)))?delay_10ths(DOT):delay_10ths(DASH);
		PORTB &= ~_BV(PORTB7);
		delay_10ths(intertickGap);
	} while (count--);
	
	delay_10ths(charGap);
}

/*
inline void send(const uint8_t ticks[]) {
	uint8_t i = 1;
	do {
		PORTB |= _BV(PORTB7);
		delay_10ths(ticks[i]);
		PORTB &= ~_BV(PORTB7);
		delay_10ths(intertickGap);
	} while(++i<=ticks[0]);
	
	delay_10ths(charGap);
}


const uint8_t h[] = {4, DOT, DOT, DOT, DOT};
const uint8_t e[] = {1, DOT};
const uint8_t l[] = {4, DOT, DASH, DOT, DOT};
const uint8_t o[] = {3, DASH, DASH, DASH};
const uint8_t w[] = {3, DOT, DASH, DASH};
const uint8_t r[] = {3, DOT, DASH, DOT};
const uint8_t d[] = {3, DASH, DOT, DOT};

*/
// 4MSB = count, 4LSB = dots and dashes (1 = dot)
#define h 0b00111111
#define e 0b00000001
#define l 0b00111011
#define o 0b00100000
#define w 0b00100100
#define r 0b00100101
#define d 0b00100011
uint8_t message[] = {h,e,l,l,o,w,o,r,l,d};
#define messageLen 10
/*
uint8_t a[] = {2, DOT, DASH};
uint8_t b[] = {4, DASH,DOT, DOT, DOT};
uint8_t c[] = {4, DASH, DOT, DASH, DOT};
uint8_t d[] = {3, DASH, DOT, DOT};
uint8_t e[] = {1, DOT};
uint8_t f[] = {4, DOT, DOT, DASH, DOT};
uint8_t g[] = {3, DASH, DASH, DOT};
uint8_t h[] = {4, DOT, DOT, DOT, DOT};
uint8_t i[] = {2, DOT, DOT};
uint8_t j[] = {4, DOT, DASH, DASH, DASH};
uint8_t k[] = {3, DASH, DOT, DASH};
uint8_t l[] = {4, DOT, DASH, DOT, DOT};
uint8_t m[] = {2, DASH, DASH};
uint8_t n[] = {2, DASH, DOT};
uint8_t o[] = {3, DASH, DASH, DASH};
uint8_t p[] = {4, DOT, DASH, DASH, DOT};
uint8_t q[] = {4, DASH, DASH, DOT, DASH};
uint8_t r[] = {3, DOT, DASH, DOT};
uint8_t s[] = {3, DOT, DOT, DOT};
uint8_t t[] = {1, DASH};
uint8_t u[] = {3, DOT, DOT, DASH};
uint8_t v[] = {4, DOT, DOT, DOT, DASH};
uint8_t w[] = {3, DOT, DASH, DASH};
uint8_t x[] = {4, DASH, DOT, DOT, DASH};
uint8_t y[] = {4, DASH, DOT, DASH, DASH};
uint8_t z[] = {4, DASH, DASH, DOT, DOT};
uint8_t d1[] = {5, DOT, DASH, DASH, DASH, DASH};
uint8_t d2[] = {5, DOT, DOT, DASH, DASH, DASH};
uint8_t d3[] = {5, DOT, DOT, DOT, DASH, DASH};
uint8_t d4[] = {5, DOT, DOT, DOT, DOT, DASH};
uint8_t d5[] = {5, DOT, DOT, DOT, DOT, DOT};
uint8_t d6[] = {5, DASH, DOT, DOT, DOT, DOT};
uint8_t d7[] = {5, DASH, DASH, DOT, DOT, DOT};
uint8_t d8[] = {5, DASH, DASH, DASH, DOT, DOT};
uint8_t d9[] = {5, DASH, DASH, DASH, DASH, DOT};
uint8_t d0[] = {5, DASH, DASH, DASH, DASH, DASH};
*/

inline void loop() {
	int count = 0;
	do 
	{
		send(message[count]);
	} while (++count<messageLen);

	delay_10ths(wordGap);              
}

int main () {
	DDRB |= _BV(PORTB7);
	while(1)
		loop();
}

