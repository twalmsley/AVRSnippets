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
; Init Port B
        ldi rmp,(1<<PORTB5)|(1<<PORTB7) ; Direction of Port B - arduino board LED is port B pin 7 
        out DDRB,rmp
; Enable sleep mode IDLE
		ldi rmp,(1<<SE) ; enable sleep
        out MCUCR,rmp
;
; ============================================
;         P R O G R A M    L O O P
; ============================================
;
;
; Set the OCR1A value
;
		ldi rmp, 0x01; high byte
		sts OCR1AH, rmp
		ldi rmp, 0xFF; low byte
		sts OCR1AL, rmp
;
; Set the pre-scaler
;
		ldi rmp,(1<<CS11)|(1<<CS10)
		sts TCCR1B,rmp
;
; Set the WGM and output compare to toggle OCR1A
;
		ldi rmp, (1<<WGM11)|(1<<WGM10)|(1<<COM1A1)
		sts TCCR1A, rmp
Loop:
		sleep
        rjmp loop ; repeat forever
;
; ============================================
;         Subroutines
; ============================================
;
;
; End of source code
;		