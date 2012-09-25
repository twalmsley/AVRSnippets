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
.INCLUDE "tn4313def.inc" ; Import of the file
.LIST ; Switch list on again
;
; ============================================
;      P O R T S   A N D   P I N S 
; ============================================
;
.EQU fq = 8000000 ; XTal frequency definition
.equ wait = 8;milliseconds
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
.DEF	tmr1 = r24
.DEF	tmr2 = r25
.DEF	tmr3 = r26
;
; ============================================
;       S R A M   D E F I N I T I O N S
; ============================================
;
.DSEG
; Format: Label: .BYTE N ; reserve N Bytes from Label:
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
RESET:
;
; Set the stack pointer
;
		cli
		ldi r16, low(RAMEND)
		ldi r17, high(RAMEND)
		out SPL, R16
		out SPH, R17

		sbi DDRB, PORTB0
		sbi DDRB, PORTB1
		sbi DDRB, PORTB2
		sbi DDRB, PORTB3
;
; ============================================
;         P R O G R A M    L O O P
; ============================================
		rcall stopMotor
		ldi r16, wait; delay
		rcall delay_ms
		rcall anticlockwise
		ldi r16, 0x64
		mov r0, r16
loop:
		ldi r16, wait; delay
		rcall delay_ms
		sbi PINB, PORTB0
		sbi PINB, PORTB1
		ldi r16, wait; delay
		rcall delay_ms
		sbi PINB, PORTB2
		sbi PINB, PORTB3
		dec r0
		brne loop
		rcall stopMotor
stop:
        rjmp stop
;
; ============================================
;         Subroutines
; ============================================
;
stopMotor:
		cbi PORTB, PORTB0
		cbi PORTB, PORTB1
		cbi PORTB, PORTB2
		cbi PORTB, PORTB3
		ret
clockwise:
		cbi PORTB, PORTB0
		sbi PORTB, PORTB1
		sbi PORTB, PORTB2
		cbi PORTB, PORTB3
		ret
anticlockwise:
		cbi PORTB, PORTB0
		sbi PORTB, PORTB1
		cbi PORTB, PORTB2
		sbi PORTB, PORTB3
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
;
; End of source code
;		