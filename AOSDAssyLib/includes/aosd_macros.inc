.macro	INIT_STACK
; Init stack
        ldi r16, LOW(RAMEND) ; Set up the stack pointer
        out SPL,r16
		ldi r16, HIGH(RAMEND)
		out SPH, r16
.endmacro