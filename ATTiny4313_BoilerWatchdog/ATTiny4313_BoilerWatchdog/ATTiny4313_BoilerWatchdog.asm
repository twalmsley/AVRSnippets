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
.equ wait = 255;milliseconds
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
.DEF	gpCount = r17
.DEF	redFlashCount = r18
.DEF	resetRedDelayCount = r19
.DEF	numberOfResetsL = r20
.DEF	numberOfResetsH = r21
.DEF	digitBits = r22
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
.ORG OVF1addr
		rjmp reset_redcounter
.ORG PCIDaddr
		rjmp EXTInt0; handle pin change interrupt
;
; ============================================
;     I N T E R R U P T   S E R V I C E S
; ============================================
.ORG INT_VECTORS_SIZE

; ============================================
;     STATIC DATA
; ============================================
digits:; The 7-seg bits
.DB 0b00010100, 0b01111110, 0b00110001, 0b00111000, 0b01011010, 0b10011000, 0b10010000, 0b00111110, 0b00010000, 0b00011000
.DB 0b00000100, 0b01101110, 0b00100001, 0b00101000, 0b01001010, 0b10001000, 0b10000000, 0b00101110, 0b00000000, 0b00001000
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
		sbi DDRD, PORTD0
		cbi DDRD, PORTD1
		sbi DDRD, PORTD2
		cbi PORTD, PORTD2
		cbi PORTD, PORTD0
		ldi gpReg, 0xFF
		out DDRB, gpReg
		ldi gpReg, 0x00
		out PORTB, gpReg
;
; Initialise everything
;
		rcall init_ext_interrupt
		rcall init_16bit_timer
		rcall three_flashes

		ldi redFlashCount, 0
		ldi numberOfResetsL, 0
		ldi numberOfResetsH, 0

		rcall show_reset_count

		sei
;
; ============================================
;         P R O G R A M    L O O P
; ============================================
loop:
        rjmp loop
;
; ============================================
;         Subroutines
; ============================================
init_16bit_timer:
		ldi gpReg, 0
		out TCCR1A, gpReg
		out TCCR1C, gpReg
		ldi gpReg, (1<<CS12)|(1<<CS10)
		out TCCR1B, gpReg

		ldi gpReg, (1<<TOIE1)
		out TIMSK, gpReg
		ldi resetRedDelayCount, 0
		ret
;
reset_redcounter:
		inc resetRedDelayCount
		cpi resetRedDelayCount, 43 ; 43 * 8.388608secs = 360 secs or 6 mins
		brne skip_reset_red
		ldi redFlashCount, 0
		ldi resetRedDelayCount, 0
skip_reset_red:
		rcall three_flashes
		reti
;
init_ext_interrupt:
;
; Enable PCI for Pin 13 (port B1)
;
		ldi gpReg, 1<<4
		out GIMSK, gpReg
		ldi gpReg, 1<<PCINT12
		out PCMSK2, gpReg
		ret
;---------------------------------------------
three_flashes:
		ldi gpCount, 6
three_flashes_loop:
		ldi gpReg, 128; delay
		rcall delay_ms
		sbi PIND, PORTD2
		dec gpCount
		breq three_flashes_done
        rjmp three_flashes_loop
three_flashes_done:
		ret
;---------------------------------------------
EXTInt0:
		ldi gpReg, 0x01
		inc redFlashCount
		cpi redFlashCount, 20; should be 240 for about 4 minutes
		brlo int_done
		;
		; Toggle PORTD0 to reset the boiler
		;
		sbi PIND, PORTD0
		ldi gpReg, 255
		rcall delay_ms
		sbi PIND, PORTD0
		ldi redFlashCount, 0x00
		inc numberOfResetsL
		rcall show_reset_count
int_done:
		reti
;---------------------------------------------
; Delay r16 milliseconds
;---------------------------------------------
delay_ms:
		mov tmr3, gpReg
delay_next:
		ldi tmr1, 0xD0
		ldi tmr2, 0x07

delay_msLoop:
		sbiw tmr1:tmr2, 0x01
		brne delay_msLoop
		dec tmr3
		brne delay_next
		ret
;
; Display the number of resets on a 2-digit LED display
; (can only show 0 to 19)
;
show_reset_count:
		cpi numberOfResetsL, 20
		brne ok
		ldi numberOfResetsL, 0
ok:
		ldi zl, low(digits<<1)
		ldi zh, high(digits<<1)
		add zl, numberOfResetsL
		adc zh, numberOfResetsH
		lpm digitBits, z
		out PORTB, digitBits
		ret
;
; End of source code
;