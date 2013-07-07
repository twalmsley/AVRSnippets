/*
 * temporary copy for later deletion
 *   Author: tony
 */
;
; ============================================
;   H A R D W A R E   I N F O R M A T I O N
; ============================================
;
.NOLIST ; Don't list the following in the list file
.INCLUDE "tn4313def.inc" ; Import of the file
.INCLUDE "macros.inc"
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
.EQU	STOP = 0x00
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
.def	mask = r20
.DEF	tmr1 = r24
.DEF	tmr2 = r25
.DEF	tmr3 = r23
; ============================================
;       S R A M   D E F I N I T I O N S
; ============================================
;
.DSEG
CUR_DIR_LOC:
			.DB 0x00
DRIVE_COUNT_LOC_BYTE0:
			.DB 0x00
DRIVE_COUNT_LOC_BYTE1:
			.DB 0x00
DRIVE_COUNT_LOC_BYTE2:
			.DB 0x00
DRIVE_COUNT_LOC_BYTE3:
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
.ORG OVF0addr
		rjmp PWMInterrupt; when the 8-bit timer overflows
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
		SET_UP_IO_PORTS
		ENABLE_PCI
		SET_FORWARD_DIRECTION
		SET_LEFT_RECOVER
		INIT_8BIT_TIMER
;
; ============================================
;         P R O G R A M    L O O P
; ============================================
;
		ldi mask, 0xff
loop:
		GO					;Start moving
		INC_DRIVE_COUNT		;Increment a counter so we don't run for too long
		CHECK_DRIVE_COUNT	;Check the counter
		brne loop			;Loop if not timed-out
		SET_STOP			;Set the direction to STOP
		GO					;Err, actually stop because the current direction was just set to stop
		RESET_DRIVE_COUNT	;Not really necessary as it has no effect
halt:
        rjmp halt			;Busywait
;
; ============================================
;         Subroutines
; ============================================
;
; Stop the motors
;
stopmoving:
		in gpReg, PINB
		andi gpReg, 0xF0
		out PORTB, gpReg
		ret
;
; Enable the motors in 'direction'
;
startmoving:
		in gpReg, PINB
		andi gpReg, 0xF0
		or gpReg, direction
		and gpReg, mask		;This provides crude pwm
		out PORTB, gpReg
		ret
;---------------------------------------------
; Delay r16 - simple timer
;---------------------------------------------
delay_ms:
		mov tmr3, gpReg			; 1 tick + 3 for the rcall
delay_next:
		ldi tmr1, 0xD0
		ldi tmr2, 0x07

delay_msLoop:
		sbiw tmr1:tmr2, 0x01	; 2 ticks * 2000
		brne delay_msLoop		; 1 + 1999 ticks
		dec tmr3				; 1 tick
		brne delay_next			; 1 + 2*gpReg ticks
		ret						; 4 ticks
;---------------------------------------------
; One of the bumpers was hit
;---------------------------------------------
EXTInt0:
		cli
		;
		; Check that one of the buffers has been hit and do a quick reverse and turn
		; before continuing
		;
		in gpReg,PORTD
		andi gpReg, 0x03
		breq recover
		rjmp int_done
recover:
		ldi direction, BACKWARD
		rcall startmoving
		ldi gpReg, 0250
		rcall delay_ms
		lds direction, RECOVER_DIR_LOC
		rcall startmoving
		ldi gpReg, 250
		rcall delay_ms
		rcall stopmoving
int_done:
		sei
		reti
;
; PWM Interrupt event
; Mask alternates between 0xFF and 0xF0 to do a crude 50% pwm on the drive motors.
; except during the recover operation.
;
PWMInterrupt:
		ldi r21, 0x0F
		eor mask, r21
		reti
;
; End of source code
;