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

.org OC1Aaddr
		rjmp OC1Interrupt
; ============================================
;     I N T E R R U P T   S E R V I C E S
; ============================================
.ORG INT_VECTORS_SIZE
;
OC1Interrupt:
		rcall ToggleSystemLED
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
		rcall init_Ex4;
Loop:
		sleep
        rjmp loop ; repeat forever
;
; ============================================
;         Subroutines
; ============================================
;
init_Ex4:
;
; Set up a 16-bit counter to trigger interrupt on OCR1A overflow
;
		in r18, sreg
		cli

		ldi r16, 0x00	;
		ldi r17, 0x80	;
		sts OCR1AH,r17
		sts OCR1AL,r16

		ldi r16, (1<<WGM11)|(1<<WGM10)
		sts TCCR1A, r16

		ldi r16, (1<<CS11) | (1<<WGM12)| (1<<WGM13)
		sts TCCR1B, r16

		ldi r16, (1<<OCIE1A) ; enable interrupt
		sts TIMSK1, r16

		out SREG,r18
		sei
		ret;

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

; End of source code
;		