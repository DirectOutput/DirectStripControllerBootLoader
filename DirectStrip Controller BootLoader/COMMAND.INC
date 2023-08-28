;-------------------------------------------------------------------------
;				Receive commands
;-------------------------------------------------------------------------
;00	get bootloader revision
;01	get buffer size
;02	get target signature
;03	get user flash size
;04	program flash
;05	start application
;06	check crc
;07	verify flash
;-------------------------------------------------------------------------
main_ok:
	    rcall putack

main_waitcommand:

	waitforcommand:
     	 
	cp RL0, RL1
	brlo waitforcommand_fadeleds_off
      ClearPortBit Led_Port,Led_Red_Bit
      SetPortBit Led_Port,Led_Yellow_Bit
	  rjmp waitforcommand_fadeleds_cont
    waitforcommand_fadeleds_off:
      //ClearPortBit Led_Port,Led_Red_Bit
      ClearPortBit Led_Port,Led_Yellow_Bit
	waitforcommand_fadeleds_cont:

	ldi RTemp,8
	add RL0,RTemp
	brne waitforcommand_fadeleds_delaycont
	  inc RL2
	  brne waitforcommand_fadeleds_delaycont
	    inc RL1
	waitforcommand_fadeleds_delaycont:
	 

	  
         sbic FT245_StatusPin, FT245_RxfBit
	 rjmp waitforcommand

	rcall	getchar
	cpi a0,00 //Exit condition
	brne waitforcommand_cont
	    rcall putnack
     	rjmp BootLoaderStart
	waitforcommand_cont:
	cpi a0,'Z' 
	brne waitforcommand_cont1
	    rcall putack
     	rjmp bootloaderstart
	waitforcommand_cont1:
    cpi a0,COMMAND	
	brne	main_waitcommand			; loop until COMMAND char

main_waitcommandvalue:
	
	waitforcommandvalue:
     	 
	cp RL0, RL1
	brlo waitforcommandvalue_fadeleds_off
      ClearPortBit Led_Port,Led_Red_Bit
      SetPortBit Led_Port,Led_Yellow_Bit
	  rjmp waitforcommandvalue_fadeleds_cont
    waitforcommandvalue_fadeleds_off:
      SetPortBit Led_Port,Led_Red_Bit
      ClearPortBit Led_Port,Led_Yellow_Bit
	waitforcommandvalue_fadeleds_cont:

	ldi RTemp,8
	add RL0,RTemp
	brne waitforcommandvalue_fadeleds_delaycont
	  inc RL2
	  brne waitforcommandvalue_fadeleds_delaycont
	    inc RL1
	waitforcommandvalue_fadeleds_delaycont:
	 

	  
         sbic FT245_StatusPin, FT245_RxfBit
	 rjmp waitforcommandvalue
	
	
	
	
	rcall	getchar

	cpi a0,00 //Exit condition
	brne commandvalue_cont
	    rcall putnack
     	rjmp BootLoaderStart
	commandvalue_cont:
	cpi a0,'Z' 
	brne commandvalue_cont1
	    rcall putack
     	rjmp bootloaderstart
	commandvalue_cont1:

.if FLASHEND > 0x7FFF
	ldi	zx, 0
.endif
	movw	zh:zl, zeroh:zerol      ; Z = 0x0000,
	clt				; T = 0 (for program, verify)
	cpi	a0, 5
	brcs	SendMessage		; command 1 ... 4
	brne	NotProgram
	  rjmp program			; command 5

    NotProgram:
	cpi	a0, 6
	brne	NotRead
	  rjmp ReadFlash			; command 6
	NotRead:
	cpi	a0, 7
	brne NotVerifyFlash	
	   rjmp VerifyFlash		; command 7
    NotVerifyFlash:


main_error:
	rcall putnack
	rjmp	main_waitcommand
;-------------------------------------------------------------------------
bootloader_exit:				; command 5
.if FlashEnd > 0x0FFF
		jmp	Application
.else
	rjmp	Application		; run application
.endif

;-------------------------------------------------------------------------
.include "message.inc"			; command 0 ... 3
;-------------------------------------------------------------------------

.include "verify.inc"			; command 7

;-------------------------------------------------------------------------
.include "readflash.inc"
.include "progmega.inc"		; mega with bootstart fuse set

;-------------------------------------------------------------------------
