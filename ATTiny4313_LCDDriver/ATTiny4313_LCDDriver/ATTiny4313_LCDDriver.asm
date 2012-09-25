/*
 * ATTiny4313_LCDDriver.asm
 *
 *  Created: 17/08/2012 08:48:43
 *   Author: tony
 *
 * Sends messages to an LCD based on data received on the usart
 * Can respond to commands ledon and ledoff to control portb7 LED
 *
 */ 
;
; ============================================
;   H A R D W A R E   I N F O R M A T I O N   
; ============================================
;
.NOLIST ; Don't list the following in the list file
.INCLUDE "tn4313def.inc" ; Import of the file
.LIST ; Switch list on again
;
; ============================================
;      P O R T S   A N D   P I N S 
; ============================================
;
;
; ============================================
;    C O N S T A N T S   T O   C H A N G E 
; ============================================
;
.EQU fq = 8000000 ; XTal frequency definition
;
; ============================================
;  F I X + D E R I V E D   C O N S T A N T S 
; ============================================
;
.EQU bufferSize = 80 ; size of LCD memory
;
; ============================================
;   R E G I S T E R   D E F I N I T I O N S
; ============================================
;
.DEF	tmr1 = r24
.DEF	tmr2 = r25
.DEF	tmr3 = r26
;
; ============================================
;       S R A M   D E F I N I T I O N S
; ============================================
;
.DSEG
buffer:				.BYTE (bufferSize+1) ; extra byte for null terminator
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
.ORG URXCaddr
		rjmp USARTRx; Read a byte
;
; ============================================
;     I N T E R R U P T   S E R V I C E S
; ============================================
.ORG INT_VECTORS_SIZE
MESSAGE1:
.DB	"Swindon Hackspace...",0
MESSAGE2:
.DB	"   Hex Counter......",0

USARTRx:; Read a byte into the buffer, prevent overruns.
		push r16
		push r17
		push r18
		push r19
		in r16, sreg
		push r16

		in r16, UDR; Read a character
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
		brne send; buffer not empty ?
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
		;
		; Null-terminate the string
		;
		ldi r16, 0
		lds zh, bufferPtr+1
		lds zl, bufferPtr
		st Z, r16
		;
		; Send to LCD
		;
		; ldi r16, 0; First character (already 0 from prev load)
		rcall	LCD_SetAddressDD

		ldi zh, high(buffer)
		ldi zl, low(buffer)
		rcall LCD_WriteRAMString
		;
		; Reset the buffer for the next message
		;
		rcall checkBufferAction
		rcall init_rx_buffer
		ret

.INCLUDE "hd44780.asm"

;
;
; ============================================
;     M A I N    P R O G R A M    I N I T
; ============================================
;

ledonMessage:
		.DB "ledon",10
ledonOkMessage:
		.DB "Ok, LED switched on.", 10, 0
ledoffMessage:
		.DB "ledoff",10, 0
ledoffOkMessage:
		.DB "Ok, LED switched off.", 10

RESET:
;
; Set the stack pointer
;
		cli
		ldi r16, low(RAMEND)
		ldi r17, high(RAMEND)
		out SPL, R16
		out SPH, R17
;
; ============================================
;         P R O G R A M    L O O P
; ============================================
;
; Init USART to 9600 baud at 8MHz with U2Xn=0
		ldi r16, 51
		ldi r17, 0
		rcall USART_Init
		rcall init_rx_buffer
		ldi r16, 0xFF		; Wait after startup
		rcall delay_ms
		rcall LCD_Init
		sei
; Init Port B
        sbi DDRB,PORTB7; Direction of Port B - arduino board LED is port B pin 7 

		ldi r16, 0; First character
		rcall	LCD_SetAddressDD
		ldi	ZL, LOW(MESSAGE1 << 1)
		ldi	ZH, HIGH(MESSAGE1 << 1)
		rcall LCD_WriteString


		ldi r16, 0x40; First character, 2nd row
		rcall	LCD_SetAddressDD
		ldi	ZL, LOW(MESSAGE2 << 1)
		ldi	ZH, HIGH(MESSAGE2 << 1)
		rcall LCD_WriteString
		ldi r20,0
loop:
		ldi r16, 0x40; First character, 2nd row
		rcall	LCD_SetAddressDD
		mov r16, r20
		rcall LCD_WriteHex8
		ldi r16, 0x00
		rcall delay_ms
		inc r20
        rjmp loop ; repeat forever
;
; ============================================
;         Subroutines
; ============================================
USART_Init:
		; Set baud rate
		out UBRRH, r17
		out UBRRL, r16
		; Enable receiver
		ldi r16, (1<<RXEN)|(1<<RXCIE)
		out UCSRB,r16
		; Set frame format: 8data, 1stop bit
		ldi r16, (3<<UCSZ0)
		out UCSRC,r16
		ret

init_rx_buffer:
		ldi zh, high(buffer)
		ldi zl, low(buffer)
		sts bufferPtr, zl
		sts bufferPtr+1, zh
		ldi r16,0
		sts currentBufferLen, r16
		ret

;---------------------------------------------
; Delay r16 milliseconds
;---------------------------------------------
delay_ms:
		mov tmr3, r16
delay_next:
		ldi tmr1, 0xD0
		ldi tmr2, 0x07

delay_msLoop:
		sbiw tmr1:tmr2, 0x01
		brne delay_msLoop
		dec tmr3
		brne delay_next
		ret

Wait4xCycles:
		sbiw XH:XL, 0x01
		brne Wait4xCycles
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