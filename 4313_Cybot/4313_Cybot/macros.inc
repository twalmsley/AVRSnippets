#ifndef _MACROS_INC_
#define _MACROS_INC_
;
; ============================================
;     MACROS
; ============================================
;
.MACRO	INC_DRIVE_COUNT
		lds gpReg, DRIVE_COUNT_LOC_BYTE0
		inc gpReg
		sts DRIVE_COUNT_LOC_BYTE0, gpReg
		cpi gpReg, 0x00
		brne skip_overflow
		lds gpReg, DRIVE_COUNT_LOC_BYTE1
		inc gpReg
		sts DRIVE_COUNT_LOC_BYTE1, gpReg
		cpi gpReg, 0x00
		brne skip_overflow
		lds gpReg, DRIVE_COUNT_LOC_BYTE2
		inc gpReg
		sts DRIVE_COUNT_LOC_BYTE2, gpReg
		cpi gpReg, 0x00
		brne skip_overflow
		lds gpReg, DRIVE_COUNT_LOC_BYTE3
		inc gpReg
		sts DRIVE_COUNT_LOC_BYTE3, gpReg
		cpi gpReg, 0x00
		brne skip_overflow
skip_overflow:
.ENDMACRO

.MACRO	RESET_DRIVE_COUNT
		ldi gpReg, 0x00
		sts DRIVE_COUNT_LOC_BYTE0, gpReg
		sts DRIVE_COUNT_LOC_BYTE1, gpReg
		sts DRIVE_COUNT_LOC_BYTE2, gpReg
		sts DRIVE_COUNT_LOC_BYTE3, gpReg
.ENDMACRO

.MACRO	CHECK_DRIVE_COUNT
		lds gpReg, DRIVE_COUNT_LOC_BYTE2
		cpi gpReg, 0x16
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

.MACRO INIT_TIMER
		ldi gpReg, 0
		out TCCR1A, gpReg
		out TCCR1C, gpReg
		ldi gpReg, (1<<CS01)
		out TCCR1B, gpReg

		ldi gpReg, (1<<TOIE1); enable the timer overflow interrupt
		out TIMSK, gpReg
.ENDMACRO
.MACRO INIT_8BIT_TIMER
		ldi gpReg, 0
		out TCCR0A, gpReg
		ldi gpReg, (1<<CS01)
		out TCCR0B, gpReg

		ldi gpReg, (1<<TOIE0); enable the timer overflow interrupt
		out TIMSK, gpReg
.ENDMACRO
.MACRO  SET_FORWARD_DIRECTION
; Set the direction to forward and store it in SRAM
;
		ldi gpReg, FORWARD
		sts CUR_DIR_LOC, gpReg
.ENDMACRO
.MACRO  SET_STOP
; Set the direction to forward and store it in SRAM
;
		ldi gpReg, STOP
		sts CUR_DIR_LOC, gpReg
.ENDMACRO
.MACRO  SET_LEFT_RECOVER
;
; Set the recover direction to LEFT and store in SRAM
;
		ldi gpReg, LEFT
		sts RECOVER_DIR_LOC, gpReg
.ENDMACRO
.MACRO  SET_UP_IO_PORTS
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
.ENDMACRO
.MACRO GO
;
; Start in the default direction
;
		lds direction, CUR_DIR_LOC
		rcall startmoving
.ENDMACRO

#endif
