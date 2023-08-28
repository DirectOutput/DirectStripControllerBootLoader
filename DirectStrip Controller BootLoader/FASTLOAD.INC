;*************************************************************************
;*									 *
;*				AVR universal Bootloader		 *
;*									 *
;*			Author: Peter Dannegger				 *
;*									 *
;*************************************************************************

.list
	.org	BootStart
init:
	cli						; no interrupts allow
	
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
; Turn on global int
	
	
	ldi	a0, low (RamEnd)	; initialize stack
	out	SPL, a0
	ldi	a0, high(RamEnd)
	out	SPH, a0

	clr RL0
	clr RL1
	clr RL2


	  ;Set led port bits to output & turn red led on
  SetPortBit Led_DDR,Led_Yellow_Bit
  SetPortBit Led_DDR,Led_Red_Bit

  ;Turn Red led on
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

  ;Turn yellow led on
  //SetPortBit Led_Port,Led_Yellow_Bit
  //ClearPortBit Led_Port,Led_Red_Bit


  ; ============================= 
;   Warteschleifen-Generator 
;     60000000 Zyklen:
; ----------------------------- 
; warte 49939965 Zyklen:
          ldi  R17, $FF
WGLOOP0:  ldi  R18, $FF
WGLOOP1:  ldi  R19, $FF
WGLOOP2:  dec  R19
          brne WGLOOP2
          dec  R18
          brne WGLOOP1
          dec  R17
          brne WGLOOP0
; ----------------------------- 
; warte 10060014 Zyklen:
          ldi  R17, $83
WGLOOP3:  ldi  R18, $8F
WGLOOP4:  ldi  R19, $B2
WGLOOP5:  dec  R19
          brne WGLOOP5
          dec  R18
          brne WGLOOP4
          dec  R17
          brne WGLOOP3

; ============================= 
  


  ;Clear FT245 input buffer
  nop
  nop
  sbic FT245_StatusPin, FT245_RxfBit
  rjmp FT245_ClearInputBufferExit
  FT245_ClearInputBufferLoop:
  cbi FT245_StatusPort, FT245_RdBit
  nop
  nop

  nop
  nop
  sbi FT245_StatusPort, FT245_RdBit
  nop
  nop

  nop
  nop
  sbis FT245_StatusPin, FT245_RxfBit
  rjmp FT245_ClearInputBufferLoop
  FT245_ClearInputBufferExit:

    //SetPortBit Led_Port,Led_Yellow_Bit
    //ClearPortBit Led_Port,Led_Red_Bit

BootLoaderStart:

Set
waitforfirstchar:
     	 
	cp RL0, RL1
	brlo fadeleds_off
      SetPortBit Led_Port,Led_Red_Bit
      SetPortBit Led_Port,Led_Yellow_Bit
	  rjmp fadeleds_cont
    fadeleds_off:
      ClearPortBit Led_Port,Led_Red_Bit
      ClearPortBit Led_Port,Led_Yellow_Bit
	fadeleds_cont:

	ldi RTemp,8
	add RL0,RTemp
	brne fadeleds_delaycont
	  inc RL2
	  brne fadeleds_delaycont
	    inc RL1
	fadeleds_delaycont:
	 

	  
         sbic FT245_StatusPin, FT245_RxfBit
	 rjmp waitforfirstchar



     FT245_WaitForRead
	 nop
     nop
     FT245_ReadByte RCommandByte


	 cpi RCommandByte,0
	 brne bootloadercontinue1  
	   FT245_SendNack
	   Rjmp waitforfirstchar
     bootloadercontinue1:


	 cpi RCommandByte,'Z'			;exit bootloader if we have not received Z
     breq bootloadercontinue
	   rjmp  bootloader_exit
	 bootloadercontinue:
	 ldi a0,'P'
	 rcall	putchar
	
;-------------------------------------------------------------------------

.include "password.inc"			; check password
;-------------------------------------------------------------------------
connected:
   


;-------------------------------------------------------------------------
.include "command.inc"			; execute commands
.include "ft245.inc"			; UART subroutines
;-------------------------------------------------------------------------
fadeleds:
	;RL0 PWM counter
	;RL1 Fade counter
	;RL2 Delay



ret




Password:
	.db	"LedstripController", 0, 0		
.list
Messages:
	.db \
	3, high(Version), low(Version), \
	3, high(2*BufferSize), low(2*BufferSize), \
	4, SIGNATURE_000, SIGNATURE_001, SIGNATURE_002, \
	4, byte3(UserFlash), byte2(UserFlash), byte1(UserFlash)
;-------------------------------------------------------------------------

//is this required?
	.org	Flashend
	ret
;-------------------------------------------------------------------------
