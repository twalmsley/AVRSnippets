/*
 * ATTiny4313_BoilerWatchdog.asm
 *
 * This program counts pin toggles on PORTB1 and sends a pulse on
 * PORTD0 if the count reaches 240.
 *
 * TIMER1 will reset this count every 6 minutes, so if the boiler
 * is in lockout mode, PORTD0 can be used to reset it via a relay.
 * Otherwise, if not in reset mode, the timer will reset the count
 * to prevent spurious boiler resets.
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
.equ wait = 255;milliseconds
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
.DEF	gpReg = r16
.DEF	gpCount = r17
.DEF	numberOfResetsL = r20
.DEF	numberOfResetsH = r21
.DEF	digitBits = r22
.DEF	tmr1 = r24
.DEF	tmr2 = r25
.DEF	tmr3 = r23
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
.ORG OVF1addr
		rjmp reset_redcounter; when the 16-bit timer overflows
;
; ============================================
;     I N T E R R U P T   S E R V I C E S
; ============================================
.ORG INT_VECTORS_SIZE

; ============================================
;     STATIC DATA
; ============================================
;
; The numberOfResets value is used as an index into this list, and each value set the LED display segments to show the
; correct value.
;
digits:; The 7-seg bits
.DB 0b00010100, 0b01111110, 0b00110001, 0b00111000, 0b01011010, 0b10011000, 0b10010000, 0b00111110, 0b00010000, 0b00011000
.DB 0b00000100, 0b01101110, 0b00100001, 0b00101000, 0b01001010, 0b10001000, 0b10000000, 0b00101110, 0b00000000, 0b00001000
;
; ============================================
;     M A I N    P R O G R A M    I N I T
; ============================================
;
RESET:
;
; Set the stack pointer so that 'call's can be used correctly.
;
		ldi r16, low(RAMEND)
		ldi r17, high(RAMEND)
		out SPL, R16
		out SPH, R17
;
; Set the IO Ports
;
		ldi gpReg, 0xFF; enable the LED display pins and turn on all segments. (active low)
		out DDRB, gpReg
		ldi gpReg, 0x00
		out PORTB, gpReg
;
; Initialise everything
;
		rcall init_16bit_timer
		;
		; Set initial values
		;
		ldi numberOfResetsL, 0
		ldi numberOfResetsH, 0
		;
		; Enable the interrupts
		;
		sei
;
; ============================================
;         P R O G R A M    L O O P
; ============================================
		ldi gpReg, (1<<SE)
		out MCUCR, gpReg
loop:
		sleep
        rjmp loop;
;
; ============================================
;         Subroutines
; ============================================
init_16bit_timer:
		ldi gpReg, 0
		out TCCR1A, gpReg
		out TCCR1C, gpReg
		ldi gpReg, (1<<CS11)
		out TCCR1B, gpReg

		ldi gpReg, (1<<TOIE1); enable the timer overflow interrupt
		out TIMSK, gpReg
		ret
;-------------------------------------------------------
; This timer overflow interrupt sets the reset counter to zero
; periodically to prevent spurious boiler resets.
;-------------------------------------------------------
reset_redcounter:
		inc numberOfResetsL; Update the number of resets and call a routing to display the value
		rcall show_reset_count
		reti

;-------------------------------------------------------
; Display the number of resets on a 2-digit LED display
; (can only show 0 to 19)
;-------------------------------------------------------
show_reset_count:
		cpi numberOfResetsL, 20
		brne ok
		ldi numberOfResetsL, 0 ; reset if we go over 19 as that's all we can display
ok:
	;Load the lookup table offset into Z
	;Add the current count value to index the correct byte for this value (bits are 0 to enable corresponding segments)
	;
		ldi zl, low(digits<<1)
		ldi zh, high(digits<<1)
		add zl, numberOfResetsL
		adc zh, numberOfResetsH
		lpm digitBits, z		; Load the 7-seg value
		out PORTB, digitBits	; set it on the output port.
		ret
;
; End of source code
;