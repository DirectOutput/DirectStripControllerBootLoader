;-------------------------------------------------------------------------
;				Send Messages
;-------------------------------------------------------------------------
;input: a0 = number of message 0 .. 3
;
SendMessage:
   dec a0
.if FLASHEND > 0x7FFF				//Load address of first message
	ldi	zx, byte3(2*Messages)
	out	RAMPZ, zx
.endif
	ldi	zl, low (2*Messages)
	ldi	zh, high(2*Messages)
	ldi	yl, 0
_sme1:
	add	zl, yl			; add offset to next message
	adc	zh, zerol
	XLPM	yl, z
	subi	a0, 1			; count down until message found
	brcc	_sme1

	ldi	a0, '>'		; first byte of message
    
_sme2:
	rcall	putchar
	XLPM	a0, z+
	subi	yl, 1
	brcc	_sme2
	


	rjmp	main_ok
;-------------------------------------------------------------------------
