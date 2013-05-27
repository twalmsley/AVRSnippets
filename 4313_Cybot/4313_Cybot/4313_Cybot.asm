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
;
; Set the stack pointer
;
		cli
		ldi r16, low(RAMEND)
		ldi r17, high(RAMEND)
		out SPL, R16
		out SPH, R17
;
; Set the IO Ports
;
		ldi gpReg, 0x1F
		out DDRB, gpReg		;PortB bits 0-3 control the motors, 4 is the led

		ldi gpReg, 0x00
		out PORTB, gpReg
		cbi PORTB, 4

		ldi gpReg, 0x03
		out DDRA, gpReg		;Port A bits 0 and 1 are for the bumber switches
;
; Initialise everything
;
		rcall init_ext_interrupt
		sei					; enable interrupts
;
; ============================================
;         P R O G R A M    L O O P
; ============================================
loop:
		rcall forward
        rjmp loop
;
; ============================================
;         Subroutines
; ============================================
stop:
		in gpReg, PINB
		andi gpReg, 0xF0
		out PORTB, gpReg
		ret
back:
		in gpReg, PINB
		andi gpReg, 0xF0
		ori gpReg, 0x05
		out PORTB, gpReg
		ret
left:
		in gpReg, PINB
		andi gpReg, 0xF0
		ori gpReg, 0x09
		out PORTB, gpReg
		ret
right:
		in gpReg, PINB
		andi gpReg, 0xF0
		ori gpReg, 0x06
		out PORTB, gpReg
		ret
forward:
		in gpReg, PINB
		andi gpReg, 0xF0
		ori gpReg, 0x0a
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
init_ext_interrupt:
;
; Enable PCI for port A1 and A0
;
		ldi gpReg, 1<<3		;i.e. PCIE1
		out GIMSK, gpReg
		ldi gpReg, 1<<PCINT9 | 1<<PCINT8
		out PCMSK1, gpReg
		ret
;---------------------------------------------
EXTInt0:
		cli
		rcall back
		ldi gpReg, 0x60
		rcall delay_ms
		rcall stop
		rcall left
		ldi gpReg, 0x60
		rcall delay_ms
		rcall stop
int_done:
		sei
		reti
;
; End of source code
;