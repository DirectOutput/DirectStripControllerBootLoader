;-------------------------------------------------------------------------
;				Check password
;-------------------------------------------------------------------------
checkpwd:
.if FLASHEND > 0x7FFF
	ldi	a0, byte3(2*Password)
	out	RAMPZ, a0
.endif
	ldi	zl, low (2*Password)
	ldi	zh, high(2*Password)
checkpwd_nextchar:
	XLPM	r0, z+
	tst	r0
	breq	checkpwd_exit			; end if zero byte
	
	waitforpwchar:
     	 
	cp RL0, RL1
	brlo waitforpwchar_fadeleds_off
      SetPortBit Led_Port,Led_Red_Bit
      ClearPortBit Led_Port,Led_Yellow_Bit
	  rjmp waitforpwchar_fadeleds_cont
    waitforpwchar_fadeleds_off:
      ClearPortBit Led_Port,Led_Red_Bit
      ClearPortBit Led_Port,Led_Yellow_Bit
	waitforpwchar_fadeleds_cont:

	ldi RTemp,8
	add RL0,RTemp
	brne waitforpwchar_fadeleds_delaycont
	  inc RL2
	  brne waitforpwchar_fadeleds_delaycont
	    inc RL1
	waitforpwchar_fadeleds_delaycont:
	 

	  
         sbic FT245_StatusPin, FT245_RxfBit
	 rjmp waitforpwchar
	
	rcall	getchar
	cp	r0, a0
	breq	checkpwd_nextchar			; compare next byte
	    rcall putnack
	rjmp	BootLoaderStart
checkpwd_exit:
;-------------------------------------------------------------------------
