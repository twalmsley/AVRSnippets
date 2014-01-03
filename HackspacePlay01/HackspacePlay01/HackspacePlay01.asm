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
;
; ============================================
;     I N T E R R U P T   S E R V I C E S
; ============================================
.ORG INT_VECTORS_SIZE

; ============================================
;     STATIC DATA
; ============================================
;
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
		sei
;
; ============================================
;         P R O G R A M    L O O P
; ============================================
loop:
		ldi gpReg, 0x00
		out PORTB, gpReg
		ldi gpReg, 0x20
		rcall delay_ms

		ldi gpReg, 0xFF
		out PORTB, gpReg
		ldi gpReg, 0x20
		rcall delay_ms
        rjmp loop
;
; ============================================
;         Subroutines
; ============================================
;---------------------------------------------
; Delay r16 milliseconds
;---------------------------------------------
delay_ms:
		mov tmr3, gpReg
delay_next:
		ldi tmr1, 0x20
		ldi tmr2, 0x00

delay_msLoop:
		sbiw tmr1:tmr2, 0x01
		brne delay_msLoop
		dec tmr3
		brne delay_next
		ret
;
; End of source code
;