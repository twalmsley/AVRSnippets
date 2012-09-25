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
.EQU fq = 16000000 ; XTal frequency definition
;
; ============================================
;      P O R T S   A N D   P I N S 
; ============================================
;
.EQU SYSTEM_LED_PORT = PORTB
.EQU SYSTEM_LED_MASK = (1<<PORTB7)
.EQU SYSTEM_LED_PIN = PORTB7
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
.DEF rmp = R16 ; Multipurpose register
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
.org WDTaddr
        rjmp ISR_WATCHDOG
;
; ============================================
;     I N T E R R U P T   S E R V I C E S
; ============================================
.ORG INT_VECTORS_SIZE
;
ISR_WATCHDOG:
		in rmp,SREG
		push rmp
		sbi PINB, PORTB7
		pop rmp
		out SREG,rmp
		reti
;
; ============================================
;     M A I N    P R O G R A M    I N I T
; ============================================
;
RESET:
; Init stack
        ldi rmp, LOW(RAMEND) ; Set up the stack pointer
        out SPL,rmp
		ldi rmp, HIGH(RAMEND)
		out SPH, rmp
; Init Port B
        ldi rmp,(1<<PORTB7) ; Direction of Port B - arduino board LED is port B pin 7 
        out DDRB,rmp
; Enable sleep mode POWER DOWN
		ldi rmp,(1<<SE)|(1<<SM1) ; enable sleep with power down.
        out SMCR,rmp
		ldi rmp, 0xFF
		sts PRR0, rmp; Turn off unused devices
		sts PRR1, rmp; Turn off unused devices;
		cli
;
; ============================================
;         P R O G R A M    L O O P
; ============================================
;
		rcall init_watchdog;
Loop:
		sleep
        rjmp loop ; repeat forever
;
; ============================================
;         Subroutines
; ============================================
;
init_watchdog:
		cli
		; Start timed sequence
		lds r16, WDTCSR
		ori r16, (1<<WDCE) | (1<<WDE)
		sts WDTCSR, r16
		; -- Got four cycles to set the new values from here -
		; Set new prescaler(time-out) 
		ldi r16, (1<<WDIE) | (1<<WDP2) | (1<<WDP1); enable interrupt mode and prescaler for ~1s
		sts WDTCSR, r16
		; -- Finished setting new values, used 2 cycles -
		; Reset Watchdog Timer
		wdr
		; Clear WDRF in MCUSR
		in r16, MCUSR
		andi r16, (0xff & (0<<WDRF))
		out MCUSR, r16
		; Turn on global interrupt
		sei
		ret
;
; End of source code
;		