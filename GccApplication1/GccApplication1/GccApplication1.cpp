#define F_CPU 16000000
 #define ARDUINO 100
 #include "Arduino.h"

// SOS
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led = 13;
int DOT = 75;
int DASH = 500;
int intertickGap = 300;
int charGap = 1000;
int wordGap = 3000;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
}

static void send(uint8_t ticks[], uint8_t nTicks, uint8_t theLed) {
	for(int i = 0;i<nTicks;i++) {
		digitalWrite(theLed, HIGH);   // turn the LED on (HIGH is the voltage level)
		delay(ticks[i]);
		digitalWrite(theLed, LOW);    // turn the LED off by making the voltage LOW
		delay(intertickGap);
	}
	delay(charGap);
}

static uint8_t a[] = {DOT, DASH}; static uint8_t aLen = 2;
static uint8_t b[] = {DASH,DOT, DOT, DOT}; static uint8_t bLen = 4;
static uint8_t c[] = {DASH, DOT, DASH, DOT}; static uint8_t cLen = 4;
static uint8_t d[] = {DASH, DOT, DOT}; static uint8_t dLen = 3;
static uint8_t e[] = {DOT}; static uint8_t eLen = 1;
static uint8_t f[] = {DOT, DOT, DASH, DOT}; static uint8_t fLen = 4;
static uint8_t g[] = {DASH, DASH, DOT}; static uint8_t gLen = 3;
static uint8_t h[] = {DOT, DOT, DOT, DOT}; static uint8_t hLen = 4;
static uint8_t i[] = {DOT, DOT}; static uint8_t iLen = 2;
static uint8_t j[] = {DOT, DASH, DASH, DASH}; static uint8_t jLen = 4;
static uint8_t k[] = {DASH, DOT, DASH}; static uint8_t kLen = 3;
static uint8_t l[] = {DOT, DASH, DOT, DOT}; static uint8_t lLen = 4;
static uint8_t m[] = {DASH, DASH}; static uint8_t mLen = 2;
static uint8_t n[] = {DASH, DOT}; static uint8_t nLen = 2;
static uint8_t o[] = {DASH, DASH, DASH}; static uint8_t oLen = 3;
static uint8_t p[] = {DOT, DASH, DASH, DOT}; static uint8_t pLen = 4;
static uint8_t q[] = {DASH, DASH, DOT, DASH}; static uint8_t qLen = 4;
static uint8_t r[] = {DOT, DASH, DOT}; static uint8_t rLen = 3;
static uint8_t s[] = {DOT, DOT, DOT}; static uint8_t sLen = 3;
static uint8_t t[] = {DASH}; static uint8_t tLen = 1;
static uint8_t u[] = {DOT, DOT, DASH}; static uint8_t uLen = 3;
static uint8_t v[] = {DOT, DOT, DOT, DASH}; static uint8_t vLen = 4;
static uint8_t w[] = {DOT, DASH, DASH}; static uint8_t wLen = 3;
static uint8_t x[] = {DASH, DOT, DOT, DASH}; static uint8_t xLen = 4;
static uint8_t y[] = {DASH, DOT, DASH, DASH}; static uint8_t yLen = 4;
static uint8_t z[] = {DASH, DASH, DOT, DOT}; static uint8_t zLen = 4;
static uint8_t d1[] = {DOT, DASH, DASH, DASH, DASH}; static uint8_t d1Len = 5;
static uint8_t d2[] = {DOT, DOT, DASH, DASH, DASH}; static uint8_t d2Len = 5;
static uint8_t d3[] = {DOT, DOT, DOT, DASH, DASH}; static uint8_t d3Len = 5;
static uint8_t d4[] = {DOT, DOT, DOT, DOT, DASH}; static uint8_t d4Len = 5;
static uint8_t d5[] = {DOT, DOT, DOT, DOT, DOT}; static uint8_t d5Len = 5;
static uint8_t d6[] = {DASH, DOT, DOT, DOT, DOT}; static uint8_t d6Len = 5;
static uint8_t d7[] = {DASH, DASH, DOT, DOT, DOT}; static uint8_t d7Len = 5;
static uint8_t d8[] = {DASH, DASH, DASH, DOT, DOT}; static uint8_t d8Len = 5;
static uint8_t d9[] = {DASH, DASH, DASH, DASH, DOT}; static uint8_t d9Len = 5;
static uint8_t d0[] = {DASH, DASH, DASH, DASH, DASH}; static uint8_t d0Len = 5;

// the loop routine runs over and over again forever:
void loop() {
	send(h, hLen, led);
	send(e, eLen, led);
	send(l, lLen, led);
	send(l, lLen, led);
	send(o, oLen, led);
	send(w, wLen, led);
	send(o, oLen, led);
	send(r, rLen, led);
	send(l, lLen, led);
	send(d, dLen, led);
  delay(wordGap);              
}


