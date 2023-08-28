/****************************************************
 * Bootloader for
 * Direct Strip Controller
 ***************************************************/ 


.org	BootStart
;****************************************************
init:
	cli						; no interrupts allowed
	
	wdr
	;Turn watchdog off
	; Clear WDRF in MCUSR
	ReadPort MCUSR, RTemp
	andi RTemp, (0xff & (0<<WDRF))
	WritePort MCUSR, RTemp
	; Write logical one to WDCE and WDE
	; Keep old prescaler setting to prevent unintentional time-out
	ReadPort WDTCSR, RTemp 
	ori RTemp, (1<<WDCE) | (1<<WDE)
	WritePort WDTCSR, RTemp
	; Turn off WDT
	ldi RTemp, (0<<WDE)
	WritePort WDTCSR, RTemp
	

	
	ldi	RTemp, low (RamEnd)	; initialize stack
	out	SPL, RTemp
	ldi	RTemp, high(RamEnd)
	out	SPH, RTemp

	;Set led port bits to output & turn red led on
	SetPortBit Led_DDR,Led_Yellow_Bit
	SetPortBit Led_DDR,Led_Red_Bit

	;Turn Red led on, yellow led off
	ClearPortBit Led_Port,Led_Yellow_Bit
	SetPortBit Led_Port,Led_Red_Bit

	;Set IO direction for FT245 data port (input)
	FT245_SetInput
	
	;Set IO Directtion for FT245 status connectors
	SetPortBit FT245_StatusDDR,FT245_WRBit
	SetPortBit FT245_StatusDDR,FT245_RDBit
	ClearPortBit FT245_StatusDDR,FT245_RxfBit
	ClearPortBit FT245_StatusDDR,FT245_TXEBit


	
	;Set Rd and WR to initial values
	SetPortBit FT245_StatusPort,FT245_RDBit
	SetPortBit FT245_StatusPort,FT245_WRBit

	;Wait a bit to ensure that the USB port is properly initialized
    Delay3000ms
	
	FT245_ClearInputBuffer 

	ldi RL0,0
	ldi RL1,0
	ldi RL2,0
	ldi RData0,0
	ldi RData1,0
	ldi RData2,0

;****************************************************

BootLoaderStart:

	FT245_WaitForRead_PayLoad Leds_Both_Fade_On

	rcall ReadByte

	;Check if we have received 0
	cpi RData0,0
	brne BootLoaderStart_Cont1  
	   ;on 0 respond with Nack and wait for next char
	   rcall SendNack
	   Rjmp BootLoaderStart
	BootLoaderStart_Cont1:

	;Check if we have received 'Z' (Reset, bootloaderstart)
	cpi RData0,'Z'
	brne BootLoaderStart_Cont2  
	   ;Bootloader startcommand received, no action required
	   rcall SendOk
	   Rjmp BootLoaderStart
	BootLoaderStart_Cont2:

	;Check if we have received B (bootloader start command)
	cpi RData0,'B'			
	breq BootLoaderStart_Cont3
		;exit bootloader if we have not received Z
		jmp	Application
	BootLoaderStart_Cont3:
    
	rcall BootloaderMain
	
rjmp BootloaderStart
;*******************************************************
BootloaderMain:
	;Check PW and exit if incorrect PW received
	rcall CheckPassword
	brtc BootloaderMain_FailExit

	;PW check OK, send OK
	rcall SendOK

    ;No data available
	ldi RDataReady,0

	BootLoadMain_CommandLoop:
		;Get command value
		FT245_WaitForRead_PayLoad Leds_Yellow_Fade_On
		rcall ReadByte

		;Exit if 0 received
		cpi RData0,0
		brne BootloaderMain_NotExit
		    BootloaderMain_FailExit:
			rcall SendNack
			ret
		BootloaderMain_NotExit:

        cpi RData0,'Z'
		brne BootloaderMain_NotReset
		  rcall SendOK
		  ret
		BootloaderMain_NotReset:

		cpi RData0,'>'
		brne BootloaderMain_NotReceive
		  rcall Receive 	
		  rjmp BootLoadMain_CommandLoop
		BootloaderMain_NotReceive:

		tst RDataReady
		breq BootloaderMain_NoDataReady
			cpi RData0,'!'
			brne BootloadMain_NotUpdateCommand
              rcall Programm
			  rjmp BootLoadMain_CommandLoop
			BootloadMain_NotUpdateCommand:

			cpi RData0,'?'
			brne BootloadMain_NotVerifyCommand
                rcall Verify
				rjmp BootLoadMain_CommandLoop
			BootloadMain_NotVerifyCommand:

		BootloaderMain_NoDataReady:
		
		cpi RData0,'0'
		brlo BootloaderMain_NotMessage
		  cpi RData0,('9'+1)
		  brsh BootloaderMain_NotMessage
		  rcall Message
		  rjmp BootLoadMain_CommandLoop
		BootloaderMain_NotMessage:
		
		
		rcall SendError
    rjmp BootLoadMain_CommandLoop

;*******************************************************
Message: ;Requires command in RData0

	subi RData0,'0'	;Get message number between 0-9
	//Check if a firmware message has to be sent
	cpi RData0,8
	brsh FirmwareMessage


	ldi	RTemp, byte3(2*Messages)
	out	RAMPZ, RTemp
	ldi	zl, low (2*Messages)
	ldi	zh, high(2*Messages)

	ldi RData1,0
	ldi RData2,0
    Message_FindMessageLoop:
		add	zl, RData1					; add offset to next message
		adc	zh, RData2
		ELPM	RData1, z
		tst RData1
		breq Message_Failed

		subi	RData0, 1			; count down until message found
	brcc Message_FindMessageLoop

	subi RData1,1

	Message_SendLoop:
      ELPM RData0,z+
	  FT245_SendByte RData0
	  subi RData1,1
   brcc Message_SendLoop
   rcall SendOk
ret
 
  
Message_Failed:
  rcall SendError
ret
 
FirmwareMessage:
	ldi	RTemp, byte3(2*FirmwareMessageStart)
	out	RAMPZ, RTemp
	ldi	zl, low (2*FirmwareMessageStart)
	ldi	zh, high(2*FirmwareMessageStart)

	ldi RData1,31
	ldi RData2,0x55
	FirmwareMessage_CheckLoop:
      ELPM	RTemp, z+
      eor RData2,RTemp
	  subi RData1,1
	brne FirmwareMessage_CheckLoop
	
	tst RData2
	brne Message_Failed
	

	//load addr of message 0
	ldi	RTemp, byte3(2*FirmwareMessageStart)
	out	RAMPZ, RTemp
	ldi	zl, low (2*FirmwareMessageStart)
	ldi	zh, high(2*FirmwareMessageStart)
	
	subi RData0,8
    breq FirmwareMessage_Message0
    	//Load addr of message 1
		ldi	zl, low ((2*FirmwareMessageStart)+16)
	FirmwareMessage_Message0: 

	ldi RData1,14
	FormwareMessage_OutputLoop:
	   ELPM	RData0, z+
	   tst RData0
	   breq FormwareMessage_OutputLoop_Exit
       FT245_SendByte RData0
	  subi RData1,1
   brne FormwareMessage_OutputLoop
   FormwareMessage_OutputLoop_Exit:
   
   rcall SendOk
ret   	   



Messages:
	.db \
	35, "Direct Strip Controller Bootloader", \
	4, "1.2", \
	3, high(PageSizeB), low(PageSizeB), \
	4, SIGNATURE_000, SIGNATURE_001, SIGNATURE_002, \
	4, byte3(2*BootStart), byte2(2*BootStart), byte1(2*BootStart),\
	4,"5.0",\
	0



;*******************************************************
Receive:
    ldi RDataReady,0
	ldi LoopLo,low(ReceiveBufferEnd-ReceiveBuffer)
	ldi LoopHi,high(ReceiveBufferEnd-ReceiveBuffer)
    LDI XH,HIGH(ReceiveBuffer)
    LDI XL,LOW(ReceiveBuffer)
    
	Ldi RData1,0x55
	Receive_Loop:

		FT245_WaitForRead_Payload Leds_Yellow_Fade_Off

		rcall ReadByte
	
		st x+,RData0
		eor RData1,RData0

		sbiw loophi:looplo, 1
	 brne Receive_Loop

	 Tst RData1  ;is 0 if checksum is ok
     brne Receive_Failed



	 rcall ProgRegistersLoad

     ;Check if address in in bootloader space
	 ldi	RData0, high(2*BootStart)
	 ldi	RData1, byte3(2*BootStart)
     in RData2,RampZ
	 cpi	zl, low (2*BootStart)
	 cpc	zh, RData0                  	
	 cpc    RData2, RData1
	 brcc Receive_Failed

	 ldi RDataReady,0xff
	 rcall SendOK
ret

Receive_Failed:
    ldi RDataReady,0
	rcall SendError
ret
;*******************************************************
Verify:

    SetPortBit Led_Port,Led_Red_Bit
	ldi RData0, (1<<RWWSRE) | (1<<SPMEN)
	rcall Do_spm

	rcall ProgRegistersLoad
    
	Verify_verify:
		elpm RData0, Z+
		ld RData1,x+
        cp RData0,RData1
		brne Verify_Failed
		sbiw loophi:looplo, 1
     brne Verify_verify
	 rcall SendOk
     ClearPortBit Led_Port,Led_Red_Bit  
ret

Verify_Failed:
  rcall SendError
  ClearPortBit Led_Port,Led_Red_Bit  
ret
;*******************************************************

Programm:
     SetPortBit Led_Port,Led_Red_Bit


	;Delete page
    rcall ProgRegistersLoad
	ldi RData0, (1<<PGERS) | (1<<SPMEN)
	rcall Do_spm
	; re-enable the RWW section
	ldi RData0, (1<<RWWSRE) | (1<<SPMEN)
	rcall Do_spm

     ;Fill page buffer
  	 rcall ProgRegistersLoad
	 Programm_Fillpagebuffer:
		ld	r0, x+
		ld	r1, x+
		ldi	RData0, 1<<SPMEN
    	rcall	do_spm
		adiw	zh:zl, 2
		sbiw loophi:looplo, 2
	brne Programm_Fillpagebuffer

	;Write page
    rcall ProgRegistersLoad
	ldi RData0, (1<<PGWRT) | (1<<SPMEN)
	rcall Do_spm
	; re-enable the RWW section
	ldi RData0, (1<<RWWSRE) | (1<<SPMEN)
	rcall Do_spm
	;wait for spm to complete
	rcall Wait_spm

	;Verify
	rjmp Verify

;*****************************************************************************
;Requires SPMCSR value in RData0
Do_spm:
	rcall Wait_spm

	;store status
	in RTemp, SREG
	
	; SPM timed sequence
	out SPMCSR, RData0
	spm
	; restore SREG (to enable interrupts if originally enabled)
	out SREG, RTemp
ret

;Wait for SPM to complete
Wait_spm:
	in RTemp, SPMCSR
	sbrc RTemp, SPMEN
	rjmp Wait_spm
	;wait for eeprom access to finish
	Wait_ee:
		sbic EECR, EEPE
	rjmp Wait_ee

	ret

;*******************************************************
ProgRegistersLoad:
	 ;Load address of block  
     lds RTemp,PROGADDR
	 out Rampz,RTemp
	 lds zh,PROGADDR+1
	 ldi zl,0
	 ldi Looplo, low(PageSizeB)
	 ldi Loophi, high(PageSizeB)
	 LDI XH,HIGH(PROGBUFF)
     LDI XL,LOW(PROGBUFF)
ret


;*******************************************************
CheckPassword:
	Set
	
	ldi	RData0, byte3(2*Password)
	out	RAMPZ, RData0
	ldi	zl, low (2*Password)
	ldi	zh, high(2*Password)
	CheckPassword_NextChar:
		;Get next pw char, if 0 exit
		ELPM	RData1, z+
		tst	RData1
		breq CheckPassword_Exit
		
		FT245_WaitForRead_PayLoad Leds_Red_Fade_On
		rcall ReadByte

        ;Exit if we have received 0
		cpi RData0,0
		breq CheckPassword_Failed

        ;Compare received char with pwchar, exit if wrong char received
		cp RData0, RData1
	breq CheckPassword_NextChar
CheckPassword_Failed:
  Clt
CheckPassword_Exit:
  ret

Password:
 .db	"Direct Strip Controller", 0, 0	

;*******************************************************
ReadByte:
  FT245_ReadByte RData0
ret

SendAck:
  FT245_SendAck
ret

SendNack:
  FT245_SendNack
ret

SendOK:
  FT245_WaitForWrite
  FT245_SetOutput
  ldi RTemp,'O'
  FT245_WriteByte RTemp
  FT245_SetInput
ret

SendError:
  FT245_WaitForWrite
  FT245_SetOutput
  ldi RTemp,'E'
  FT245_WriteByte RTemp
  FT245_SetInput
ret


Leds_Incr_Fade_Values:
	ldi RTemp,8
	add RL0,RTemp
	brne Leds_Incr_Fade_Values_Cont
	  inc RL2
	  brne Leds_Incr_Fade_Values_Cont
	    inc RL1
	Leds_Incr_Fade_Values_Cont:
ret

Leds_Red_Fade_On:
    ClearPortBit Led_Port,Led_Yellow_Bit
	cp RL0, RL1
	brlo Leds_Red_Fade_On_Cont1
      ClearPortBit Led_Port,Led_Red_Bit
 	  rjmp Leds_Red_Fade_On_Cont2
    Leds_Red_Fade_On_Cont1:
      SetPortBit Led_Port,Led_Red_Bit
	Leds_Red_Fade_On_Cont2:

    rcall Leds_Incr_Fade_Values
ret

Leds_Yellow_Fade_Off:
    ClearPortBit Led_Port,Led_Red_Bit
	cp RL0, RL1
	brlo Leds_Yellow_Fade_Off_Cont1
      SetPortBit Led_Port,Led_Yellow_Bit
 	  rjmp Leds_Yellow_Fade_Off_Cont2
    Leds_Yellow_Fade_Off_Cont1:
      ClearPortBit Led_Port,Led_Yellow_Bit
	Leds_Yellow_Fade_Off_Cont2:

    rcall Leds_Incr_Fade_Values
ret


Leds_Yellow_Fade_On:
    ClearPortBit Led_Port,Led_Red_Bit
	cp RL0, RL1
	brlo Leds_Yellow_Fade_On_Cont1
      ClearPortBit Led_Port,Led_Yellow_Bit
 	  rjmp Leds_Yellow_Fade_On_Cont2
    Leds_Yellow_Fade_On_Cont1:
      SetPortBit Led_Port,Led_Yellow_Bit
	Leds_Yellow_Fade_On_Cont2:

    rcall Leds_Incr_Fade_Values

ret

Leds_Both_Fade_On:
   
    rcall Leds_Incr_Fade_Values

	cp RL0, RL1
	brlo Leds_Both_Fade_On_Cont1
      ClearPortBit Led_Port,Led_Red_Bit
	  ClearPortBit Led_Port,Led_Yellow_Bit
  	  rjmp Leds_Both_Fade_On_Cont2
    Leds_Both_Fade_On_Cont1:
      SetPortBit Led_Port,Led_Red_Bit
	  SetPortBit Led_Port,Led_Yellow_Bit
	Leds_Both_Fade_On_Cont2:

   
ret

.db 0,0,0,0,0,0,0,0


.dseg
	.org		SRAM_START

ReceiveBuffer:
PROGADDR:		.byte 2
PROGBUFF:		.byte PageSizeB
CheckSum:		.byte 1
ReceiveBufferEnd: