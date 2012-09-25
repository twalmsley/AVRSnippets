/*
 * AssemblerApplication1.asm
 *
 *  Created: 17/08/2012 08:48:43
 *   Author: tony
 */ 
;
; ============================================
;   H A R D W A R E   I N F O R M A T I O N   
; ============================================
;
.NOLIST ; Don't list the following in the list file
.INCLUDE "m2560def.inc" ; Import of the file
.LIST ; Switch list on again
;
; ============================================
;      P O R T S   A N D   P I N S 
; ============================================
;
.EQU fq = 16000000 ; XTal frequency definition
.EQU bufferSize = 30
;
; ============================================
;    C O N S T A N T S   T O   C H A N G E 
; ============================================
;
;
; ============================================
;  F I X + D E R I V E D   C O N S T A N T S 
; ============================================
;
; [Add all constants here that are not subject
;  to change or calculated from constants]
; Format: .EQU const = $ABCD
;
; ============================================
;   R E G I S T E R   D E F I N I T I O N S
; ============================================
;
;
; ============================================
;       S R A M   D E F I N I T I O N S
; ============================================
;
.DSEG
; Format: Label: .BYTE N ; reserve N Bytes from Label:
buffer:				.BYTE bufferSize
bufferPtr:			.DW 0
currentBufferLen:	.DW 0
;
; ============================================
;   R E S E T   A N D   I N T   V E C T O R S
; ============================================
;
.CSEG
.ORG $0000
        rjmp RESET ; Reset vector
.ORG URXC0addr
		rjmp USART0Rx; Read a byte
;
; ============================================
;     I N T E R R U P T   S E R V I C E S
; ============================================
.ORG INT_VECTORS_SIZE

USART0Rx:; Read a byte into the buffer, prevent overruns.
		push r16
		push r17
		push r18
		push r19
		in r16, sreg
		push r16
		
		lds r16, UDR0; Read a character
		;
		; If its a terminator and the buffer isn't empty then transmit the message and reset the buffer
		;
		cpi r16, '.'; Command ended with full stop
		breq checkBufferLen
		cpi r16, 0xd; Command ended with carriage return
		brne storeCharacter
checkBufferLen:
		lds r16,currentBufferLen
		cpi r16, 0
		brne send; buffer empty ?
storeCharacter:
		;
		; ignore some characters
		;
		cpi r16, 0xd; <CR>
		breq popAndReturn
		cpi r16, 0xa; <LF>
		breq popAndReturn
		;
		;Store the character
		;
		lds zh, bufferPtr+1
		lds zl, bufferPtr
		st Z+, r16
		;
		; Update the pointer
		;
		sts bufferPtr, zl
		sts bufferPtr+1, zh
		;
		; Increment the currentBufferLen
		;
		lds r16,currentBufferLen
		inc r16
		sts currentBufferLen, r16
		;
		; If the buffer is full then send what we have and reset the buffer
		;
		cpi r16, bufferSize
		brne popAndReturn
send:
		rcall sendAndResetBuffer
popAndReturn:
		pop r16
		out sreg, r16
		pop r19
		pop r18
		pop r17
		pop r16
		reti

sendAndResetBuffer:
		ldi zh, high(buffer)
		ldi zl, low(buffer)
		lds r18, currentBufferLen
		rcall checkBufferAction
		rcall init_rx_buffer
		ret

;
;
; ============================================
;     M A I N    P R O G R A M    I N I T
; ============================================
;
message:	
		.DB "Hello> ",10
ledonMessage:
		.DB "ledon",10
ledonOkMessage:
		.DB "Ok, LED switched on.", 10, 0
ledoffMessage:
		.DB "ledoff",10, 0
ledoffOkMessage:
		.DB "Ok, LED switched off.", 10
RESET:
; Init stack
        ldi r16, LOW(RAMEND) ; Set up the stack pointer
        out SPL,r16
		ldi r16, HIGH(RAMEND)
		out SPH, r16
; Init Port B
        sbi DDRB,PORTB7; Direction of Port B - arduino board LED is port B pin 7 
; Enable sleep mode IDLE
		ldi r16,(1<<SE) ; enable sleep
        out MCUCR,r16
;
; ============================================
;         P R O G R A M    L O O P
; ============================================
;
; Init USART to 9600 baud at 16MHz with U2Xn=0
		ldi r16, 103
		ldi r17, 0
		rcall USART_Init
		rcall init_rx_buffer
		sei
; Set up Z to point to the message, and r18 with the length of the message
;
		ldi zh, high(message<<1)
		ldi zl, low(message<<1)
		ldi r18, 8
		rcall transmitFixedMessage
loop:
		sleep
        rjmp loop ; repeat forever
;
; ============================================
;         Subroutines
; ============================================
;
USART_Init:
	; Set baud rate
	sts UBRR0H, r17
	sts UBRR0L, r16
	; Enable receiver and transmitter
	ldi r16, (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)
	sts UCSR0B,r16
	; Set frame format: 8data, 1stop bit
	ldi r16, (2<<UMSEL0)|(3<<UCSZ00)
	sts UCSR0C,r16
	ret

USART_Transmit:
	push r17
	; Wait for empty transmit buffer
USART_Wait:
	lds r17, UCSR0A
	sbrs r17, UDRE0
	rjmp USART_Wait
	; Put data (r16) into buffer, sends the data
	sts UDR0,r16
	pop r17
	ret

transmitMessage:
; Load the z pointer with the address of the message
; and r18 with the length of the message in bytes
;
		push zh
		push zl
		push r18
nextChar1:
; Send the chars 1 by 1
		ld r16, Z+
		rcall USART_Transmit
		dec r18
		brne nextChar1
		pop r18
		pop zl
		pop zh
		ret
transmitFixedMessage:
; Load the z pointer with the address of the message
; and r18 with the length of the message in bytes
;
		push zh
		push zl
		push r18
nextChar2:
; Send the chars 1 by 1
		lpm r16, Z+
		rcall USART_Transmit
		dec r18
		brne nextChar2
		pop r18
		pop zl
		pop zh
		ret

init_rx_buffer:
		ldi zh, high(buffer)
		ldi zl, low(buffer)
		sts bufferPtr, zl
		sts bufferPtr+1, zh
		ldi r16,0
		sts currentBufferLen, r16
		ret


setSystemLed:
		sbi PORTB, PORTB7
		ret

clearSystemLed:
		cbi PORTB, PORTB7
		ret

checkBufferAction:
		;
		; Do something based on what is in the buffer
		;
		ldi zh, high(ledonMessage<<1)
		ldi zl, low(ledonMessage<<1)
		ldi xh, high(buffer)
		ldi xl, low(buffer)
		ldi r18, 5						; length of the ledon command
		rcall compareStrings
		cpi r16, 1
		brne checkNextCommand
		rcall setSystemLed			;ledon

		ldi zh, high(ledonOkMessage<<1)
		ldi zl, low(ledonOkMessage<<1)
		ldi r18, 21
		rcall transmitFixedMessage
		rjmp returnCheckBufferAction
checkNextCommand:
		ldi zh, high(ledoffMessage<<1)
		ldi zl, low(ledoffMessage<<1)
		ldi xh, high(buffer)
		ldi xl, low(buffer)
		ldi r18, 6						; length of the ledon command
		rcall compareStrings
		cpi r16, 1
		brne returnCheckBufferAction
		rcall clearSystemLed		;ledoff
		ldi zh, high(ledoffOkMessage<<1)
		ldi zl, low(ledoffOkMessage<<1)
		ldi r18, 22
		rcall transmitFixedMessage
returnCheckBufferAction:
		ret

compareStrings:
		;
		; Compare two strings - set r16 to 1 if same, 0 otherwise
		; X points to RAM buffer
		; Z points to program flash memory
		; r18 holds the number of bytes to compare
		;
		push r17
		push r18
nextByteCompare:
		ld r16, X+
		lpm r17, Z+
		cp r16, r17
		brne setFalseThenPopAndReturnFromCompare
		dec r18
		brne nextByteCompare
		ldi r16, 1					; Set match to true
		rjmp returnFromCompare
setFalseThenPopAndReturnFromCompare:
		ldi r16, 0					; Set match to false
returnFromCompare:
		pop r18
		pop r17
		ret
;
; End of source code
;		