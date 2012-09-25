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
.EQU bufferSize = 100
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
buffer:				.BYTE 100
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
;
; ============================================
;     I N T E R R U P T   S E R V I C E S
; ============================================
.ORG INT_VECTORS_SIZE
;
;
; ============================================
;     M A I N    P R O G R A M    I N I T
; ============================================
;
message:.DB "hello",0
ledonMessage:
		.DB "ledon",10
ledonOkMessage:
		.DB "Ok, LED switched on.", 10, 0
RESET:
; Init stack
        ldi r16, LOW(RAMEND) ; Set up the stack pointer
        out SPL,r16
		ldi r16, HIGH(RAMEND)
		out SPH, r16
; Enable sleep mode IDLE
		ldi r16,(1<<SE) ; enable sleep
        out MCUCR,r16
;
; ============================================
;         P R O G R A M    L O O P
; ============================================
;
; Set up Z to point to the message, and r18 with the length of the message
;
		ldi r16, 'l'
		sts buffer, r16
		ldi r16, 'e'
		sts buffer+1, r16
		ldi r16, 'd'
		sts buffer+2, r16
		ldi r16, 'o'
		sts buffer+3, r16
		ldi r16, 'n'
		sts buffer+4, r16

		ldi zh, high(message<<1)
		ldi zl, low(message<<1)
		ldi r18, 8
loop:
		rcall checkBufferAction
        rjmp loop ; repeat forever
;
; ============================================
;         Subroutines
; ============================================
;

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
		brne returnCheckBufferAction

		ldi zh, high(ledonOkMessage<<1)
		ldi zl, low(ledonOkMessage<<1)
		ldi r18, 21
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