/*
 * AssemblerApplication1.asm
 *
 *  Created: 17/08/2012 08:48:43
 *   Author: tony
 */ 
.NOLIST ; Don't list the following in the list file
.INCLUDE "m2560def.inc" ; Import of the file
.LIST ; Switch list on again
.EQU fq = 16000000 ; XTal frequency definition
;
; ============================================
;   H A R D W A R E   I N F O R M A T I O N   
; ============================================
;
; [Add all hardware information here]
;
; ============================================
;      P O R T S   A N D   P I N S 
; ============================================
;
; [Add names for hardware ports and pins here]
; Format: .EQU Controlportout = PORTA
;         .EQU Controlportin = PINA
;         .EQU LedOutputPin = PORTA2
;
; ============================================
;    C O N S T A N T S   T O   C H A N G E 
; ============================================
;
; [Add all constants here that can be subject
;  to change by the user]
; Format: .EQU const = $ABCD
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
; [Add all register names here, include info on
;  all used registers without specific names]
; Format: .DEF rmp = R16
.DEF rmp = R16 ; Multipurpose register

.DEF ctra = R17 ; Counter low byte
.DEF ctrb = R18 ; Counter medium byte
.DEF ctrc = R19 ; Counter high byte
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
.ORG WDTaddr
        rjmp TogglePinInterrupt; Watchdog timeout
;
; ============================================
;     I N T E R R U P T   S E R V I C E S
; ============================================
;
.ORG INT_VECTORS_SIZE
TogglePinInterrupt:
		in rmp,PORTB			; Get the current state of Port B
		ANDI rmp, 0b10000000	; Isolate the LED bit
		breq SetBit				; If its zero jump to SetBit
		cbi PORTB, PORTB7		; Otherwise it was set, so clear it (turn off the LED).
SetBit:
		sbi PORTB,PORTB7
		reti					; Return to the main loop
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
        ldi rmp,0b10000000 ; Direction of Port B - arduino board LED is port B pin 7 
        out DDRB,rmp
; Enable sleep model IDLE
		ldi rmp,(1<<SE | 1<<SM1) ; enable sleep
        out MCUCR,rmp
		sei
;
; ============================================
;         P R O G R A M    L O O P
; ============================================
;
		rcall WDT_Prescaler_Change		;Set delay to about 0.5s
Loop:
		sleep
        rjmp loop ; repeat forever
;
; ============================================
;         Subroutines
; ============================================
;
WDT_Prescaler_Change:
; Turn off global interrupt
		cli
; Reset Watchdog Timer
		wdr
; Start timed sequence
		lds r16, WDTCSR
		ori r16, (1<<WDCE) | (1<<WDE)
		sts WDTCSR, r16
; -- Got four cycles to set the new values from here -
; Set new prescaler(time-out) value = 64K cycles (~0.5 s)
		ldi r16, (1<<WDE) | (1<<WDP2) | (1<<WDP0)
		sts WDTCSR, r16
; -- Finished setting new values, used 3 cycles -
; Turn on global interrupt
		sei
		ret
;
; End of source code
;		