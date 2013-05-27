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
;
; ============================================
;    C O N S T A N T S   T O   C H A N G E
; ============================================
;
.EQU	FORWARD = 0x0A
.EQU	BACKWARD = 0x05
.EQU	LEFT = 0x09
.EQU	RIGHT = 0x06
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
.DEF	mainLoop = r17
.def	switches = r18
.def	direction = r19
.DEF	tmr1 = r24
.DEF	tmr2 = r25
.DEF	tmr3 = r23
;
; ============================================
;     MACROS
; ============================================
;
.MACRO	INC_DRIVE_COUNT
		lds gpReg, DRIVE_COUNT_LOC_LO
		inc gpReg
		sts DRIVE_COUNT_LOC_LO, gpReg
		brne skip_overflow
		lds gpReg, DRIVE_COUNT_LOC_HI
		inc gpReg
		sts DRIVE_COUNT_LOC_HI, gpReg
skip_overflow:
.ENDMACRO

.MACRO	RESET_DRIVE_COUNT
		ldi gpReg, 0x00
		sts DRIVE_COUNT_LOC_LO, gpReg
		sts DRIVE_COUNT_LOC_HI, gpReg
.ENDMACRO

.MACRO	CHECK_DRIVE_COUNT
		lds gpReg, DRIVE_COUNT_LOC_HI
		cpi gpReg, 0x60
.ENDMACRO

.MACRO ENABLE_PCI
		ldi gpReg, 1<<3		;i.e. PCIE1
		out GIMSK, gpReg
		ldi gpReg, 1<<PCINT9 | 1<<PCINT8
		out PCMSK1, gpReg
		sei					; enable interrupts
.ENDMACRO

.MACRO INIT_STACK
		cli
		ldi r16, low(RAMEND)
		ldi r17, high(RAMEND)
		out SPL, R16
		out SPH, R17
.ENDMACRO
; ============================================
;       S R A M   D E F I N I T I O N S
; ============================================
;
.DSEG
CUR_DIR_LOC:
			.DB 0x00
DRIVE_COUNT_LOC_LO:
			.DB 0x00
DRIVE_COUNT_LOC_HI:
			.DB 0x00
RECOVER_DIR_LOC:
			.DB 0x00

; Format: Label: .BYTE N ; reserve N Bytes from Label:
;
; ============================================
;   R E S E T   A N D   I N T   V E C T O R S
; ============================================
;
.CSEG
.ORG $0000
        rjmp RESET ; Reset vector
.ORG PCIAaddr
		rjmp EXTInt0; handle pin change interrupt
;
; ============================================
;     I N T E R R U P T   S E R V I C E S
; ============================================
.ORG INT_VECTORS_SIZE

; ============================================
;     STATIC DATA
; ============================================
;
; ============================================
;     M A I N    P R O G R A M    I N I T
; ============================================
;
RESET:
		INIT_STACK
;
; Set the IO Ports
;
		ldi gpReg, 0x1F
		out DDRB, gpReg		;PortB bits 0-3 control the motors, 4 is the led

		ldi gpReg, 0x00
		out PORTB, gpReg
		cbi PORTB, 4

		ldi gpReg, 0x03
		out DDRA, gpReg		;Port A bits 0 and 1 are for the bumper switches

		ENABLE_PCI
; Set the direction to forward and store it in SRAM
;
		ldi gpReg, FORWARD
		sts CUR_DIR_LOC, gpReg
;
; Set the recover direction to LEFT and store in SRAM
;
		ldi gpReg, LEFT
		sts RECOVER_DIR_LOC, gpReg
;
; ============================================
;         P R O G R A M    L O O P
; ============================================
;
loop:
;
; Start in the default direction
;
		lds direction, CUR_DIR_LOC
		rcall startmoving
;
; Increment the drive count
;
		INC_DRIVE_COUNT
		CHECK_DRIVE_COUNT
		brne loop
;
; After a number of iterations reset the count and switch the recovery direction
;
		RESET_DRIVE_COUNT

        rjmp loop
;
; ============================================
;         Subroutines
; ============================================
stopmoving:
		in gpReg, PINB
		andi gpReg, 0xF0
		out PORTB, gpReg
		ret
startmoving:
		in gpReg, PINB
		andi gpReg, 0xF0
		or gpReg, direction
		out PORTB, gpReg
		ret
;---------------------------------------------
; Delay r16
;---------------------------------------------
delay_ms:
		mov tmr3, gpReg
delay_next:
		ldi tmr1, 0x00
		ldi tmr2, 0x20

delay_msLoop:
		sbiw tmr1:tmr2, 0x01
		brne delay_msLoop
		dec tmr3
		brne delay_next
		ret
;---------------------------------------------
;---------------------------------------------
EXTInt0:
		cli
recover:
		ldi direction, BACKWARD
		rcall startmoving
		ldi gpReg, 0x60
		rcall delay_ms
		lds direction, RECOVER_DIR_LOC
		rcall startmoving
		ldi gpReg, 0x30
		rcall delay_ms
		rcall stopmoving
int_done:
		sei
		reti
;
; End of source code
;