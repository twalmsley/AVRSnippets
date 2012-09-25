.macro	INIT_STACK
; Init stack
        ldi rmp, LOW(RAMEND) ; Set up the stack pointer
        out SPL,rmp
		ldi rmp, HIGH(RAMEND)
		out SPH, rmp
.endmacro