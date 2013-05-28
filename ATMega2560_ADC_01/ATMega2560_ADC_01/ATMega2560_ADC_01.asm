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

.DEF	tmr1 = r24
.DEF	tmr2 = r25
.DEF	tmr3 = r26
.DEF	tmr4 = r27

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
;
.ORG INT_VECTORS_SIZE
;
; ============================================
;     M A I N    P R O G R A M    I N I T
; ============================================
;
RESET:
        ldi r16, LOW(RAMEND) ; Set up the stack pointer
        out SPL,r16
		ldi r16, HIGH(RAMEND)
		out SPH, r16
; Init Port B
        ; Direction of Port B - arduino board LED is port B pin 7
		sbi DDRB, PORTB7
		ldi r16, 255
		out DDRC, r16
		ldi r16, 255
		out PORTC, r16
;
; ============================================
;         P R O G R A M    L O O P
; ============================================
;
		rcall setup_adc
Loop:
		rcall read_adc
		out PORTC, r16
		rcall delay_ms
		sbi PINB, PORTB7
        rjmp loop ; repeat forever
;
; ============================================
;         Subroutines
; ============================================
;
setup_adc:
		ldi r16, (1<<REFS1)|(1<<REFS0)
		sts ADMUX, r16

		ldi r16, (1<<ADEN)
		sts ADCSRA, r16

		ldi r16, 0
		sts DIDR2, r16
		sts DIDR0, r16
		ret

read_adc:
		ldi r16, (1<<ADEN)|(1<<ADSC)|(1<<ADPS2)|(0<<ADPS1)|(1<<ADPS0)
		sts ADCSRA, r16
not_ready:
		lds r16, ADCSRA
		andi r16, (1<<ADSC)
		brne not_ready
		lds r16, ADCL
		lds r17, ADCH
		ret
;---------------------------------------------
; Delay r17, r16 milliseconds
;---------------------------------------------
delay_ms:
;
; ignore a value of zero
;
		cpi r17, 0
		brne delay_start
		cpi r16, 0
		brne delay_start
		ret
delay_start:
		mov tmr4, r17
reload_tmr3:
		mov tmr3, r16
delay_next:
		ldi tmr1, 0xD0
		ldi tmr2, 0x07

delay_msLoop:
		sbiw tmr1:tmr2, 0x01
		brne delay_msLoop
		sbiw tmr3:tmr4, 0x01
		brne delay_next
		ret
;
; End of source code
;