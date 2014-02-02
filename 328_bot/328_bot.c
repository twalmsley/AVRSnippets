/*
 * _4313_bot.c
 *
 * Created: 03/01/2014 16:34:53
 *  Author: tony
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>


void USART_Transmit(unsigned char data) {
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

unsigned char USART_Receive() {
	while(!(UCSR0A & (1<<RXC0)));

	return UDR0;
}

int main(void) {
	//
	// Init the USART
	//
	UBRR0H = 0;
	UBRR0L = 25;//19.2Kbaud
	UCSR0A = _BV(U2X0);
	UCSR0B =  _BV(TXEN0) | _BV(RXEN0);
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);

	USART_Receive();// Wait for any response from the ATTiny to mean its ready for commands.

	while(1) {
		USART_Transmit('F');
		USART_Transmit(0x10);
		USART_Transmit(0x00);
		USART_Receive();// Wait for any response from the ATTiny

		USART_Transmit('L');
		USART_Receive();// Wait for any response from the ATTiny

		USART_Transmit('F');
		USART_Transmit(0x10);
		USART_Transmit(0x00);
		USART_Receive();// Wait for any response from the ATTiny

		USART_Transmit('L');
		USART_Receive();// Wait for any response from the ATTiny

		USART_Transmit('F');
		USART_Transmit(0x10);
		USART_Transmit(0x00);
		USART_Receive();// Wait for any response from the ATTiny

		USART_Transmit('L');
		USART_Receive();// Wait for any response from the ATTiny

		USART_Transmit('F');
		USART_Transmit(0x10);
		USART_Transmit(0x00);
		USART_Receive();// Wait for any response from the ATTiny

		USART_Transmit('L');
		USART_Receive();// Wait for any response from the ATTiny
	}
}
