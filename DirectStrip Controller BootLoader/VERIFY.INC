;-------------------------------------------------------------------------
;		 		Verify User Flash
;-------------------------------------------------------------------------
;
_ver1:
.if FLASHEND > 0x7FFF
	out	RAMPZ, zx
	elpm	r0, z
	adiw	zh:zl, 1
	adc	zx, zerol		; 24 bit addition
.else
	lpm	r0, z+
.endif
.ifndef FirstBootStart
	cpi	zl, 3
	cpc	zh, zerol
	brcs	VerifyFlash		; exclude jump to bootloader
.endif
	cpse	r0, a0
	set
    
	brts _ver2
	  ClearPortBit Led_Port,Led_Red_Bit
      SetPortBit Led_Port,Led_Yellow_Bit
	  jmp VerifyFlash
	_ver2:
      SetPortBit Led_Port,Led_Red_Bit
      ClearPortBit Led_Port,Led_Yellow_Bit


VerifyFlash:
	rcall	getchar
	brne	_ver1			; not COMMAND ?
	rcall	getchar
	subi	a0, ESC_SHIFT
	brne	_ver1			; COMMMAND + not COMMAND = End
	brts	main_error		; error, Flash not equal
	rjmp	main_ok
;-------------------------------------------------------------------------
