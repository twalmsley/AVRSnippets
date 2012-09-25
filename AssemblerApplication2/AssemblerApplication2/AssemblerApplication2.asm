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
.EQU SYSTEM_LED_MASK = 0b10000000
.EQU SYSTEM_LED_PIN = PORTB7
;
; ============================================
;    C O N S T A N T S   T O   C H A N G E 
; ============================================
;
.EQU COUNTER_INIT = 255
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
.DEF cnt = R0; Counter
.DEF init_count = R1;
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
.org OVF0addr
        rjmp ISR_TOV0 ; Int vector 23 - Timer0 Overflow
;
; ============================================
;     I N T E R R U P T   S E R V I C E S
; ============================================
.ORG INT_VECTORS_SIZE
;
ISR_TOV0:
		in rmp,SREG
		push rmp
		dec cnt
		brne skipToggle
		rcall ToggleSystemLED
		dec init_count
		brne reloadCounter
		ldi rmp, COUNTER_INIT
		mov init_count, rmp
reloadCounter:
		mov cnt, init_count
skipToggle:
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
        ldi rmp,0b10000000 ; Direction of Port B - arduino board LED is port B pin 7 
        out DDRB,rmp
; Enable sleep mode IDLE
		ldi rmp,(1<<SE) ; enable sleep
        out MCUCR,rmp
		cli
;
; ============================================
;         P R O G R A M    L O O P
; ============================================
;
		rcall init_8bit_clock0; Start the interrupt timer on clock 0 at about 30.5Hz
		ldi rmp, COUNTER_INIT
		mov init_count, rmp
		mov cnt, init_count
		sei
Loop:
		sleep
        rjmp loop ; repeat forever
;
; ============================================
;         Subroutines
; ============================================
;
ToggleSystemLED:
		push rmp
		in rmp,SYSTEM_LED_PORT					; Get the current state of Port B
		ANDI rmp, SYSTEM_LED_MASK				; Isolate the LED bit
		breq ToggleSystemLEDSetBit				; If its zero jump to SetBit
		cbi SYSTEM_LED_PORT, SYSTEM_LED_PIN		; Otherwise it was set, so clear it (turn off the LED).
		rjmp ToggleSystemLEDReturn
ToggleSystemLEDSetBit:
		sbi SYSTEM_LED_PORT, SYSTEM_LED_PIN
ToggleSystemLEDReturn:
		pop rmp
		ret

init_8bit_clock0:;
		push rmp
		ldi rmp,(1<<CS01)|(1<<CS00)
		out TCCR0B,rmp ; Timer clock = system clock / 64
		ldi rmp,1<<TOV0
		out TIFR0,rmp ; Clear TOV0/ clear pending interrupts
		ldi rmp,1<<TOIE0
		sts TIMSK0,rmp ; Enable Timer/Counter0 Overflow Interrupt
		pop rmp
		ret
;
; End of source code
;		