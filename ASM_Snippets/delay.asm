delay:; Uses 3 registers, r16, r17 and r18 - preserves them on the stack
		push r16
		push r17
		push r18
		ldi r16, 0xFF
		ldi r17, 0xFF
		ldi r18, 0x07
delayLoop:
		dec r16
		brne delayLoop
		dec r17
		brne delayLoop
		dec r18
		brne delayLoop
		pop r16
		pop r17
		pop r18
		ret
;
