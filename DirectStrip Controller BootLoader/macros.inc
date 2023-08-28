;-------------------------------------------------------------------------
;				Macros
;-------------------------------------------------------------------------






.macro FT245_SetInput
  //WritePortValue FT245_DataPort,0x00
  WritePortValue FT245_DataDDR,0x00
.endm

.macro FT245_SetOutput
  WritePortValue FT245_DataDDR,0xff
.endm

.macro FT245_WaitForRead
  FT245_WaitForReadLoop:
         sbic FT245_StatusPin, FT245_RxfBit
	 rjmp FT245_WaitForReadLoop
.endm


.macro FT245_ReadByte  ;(Register) 

	 cbi FT245_StatusPort, FT245_RdBit
	 nop
     nop
	 nop
	 nop
	 in @0, FT245_DataPin
	 sbi FT245_StatusPort, FT245_RdBit
.endm



.macro FT245_WaitForWrite
  FT245_WaitForWriteLoop:
         sbic FT245_StatusPin, FT245_TxeBit
	 rjmp FT245_WaitForWriteLoop
.endm


.macro FT245_WriteByte;(Register)

     nop
	 nop
	 sbi FT245_StatusPort, FT245_WrBit
	 nop
	 nop
	 nop
	 out FT245_DataPort,@0
	 nop
	 nop
	 nop
	 cbi FT245_StatusPort, FT245_WrBit
.endm


.macro FT245_SendByte;(Register)
  FT245_WaitForWrite
  FT245_SetOutput
  FT245_WriteByte @0
  FT245_SetInput
.endm

.macro FT245_SendAck
  FT245_WaitForWrite
  FT245_SetOutput
  ldi RTemp,'A'
  FT245_WriteByte RTemp
  FT245_SetInput
.endm

.macro FT245_SendNAck
  FT245_WaitForWrite
  FT245_SetOutput
  ldi RTemp,'N'
  FT245_WriteByte RTemp
  FT245_SetInput
.endm

.macro ReadPort  ;(Port, Register) 
  .if @0 < 0x40
	in	@1, @0
  .else
  	lds	@1, @0
  .endif
.endm

.macro WritePort   ;(Port, Register)
  .if @0 < 0x40
	out	@0, @1
  .else
  	sts	@0, @1
  .endif
.endm


.macro WritePortValue ;(Port,Value)
  ldi Rtemp,@1
  WritePort @0, RTemp
.endm

.MACRO SetPortBit ;(Port, Bit)
        .if @0<0x20
		  sbi @0,@1
		.else
		  ReadPort @0,RTemp
		  ori RTemp,(1<<@1)
		  WritePort @0,RTemp
		.endif
.ENDMACRO

.MACRO ClearPortBit ;(Port, Bit)
        .if @0<0x20
		  cbi @0,@1
		.else
		  ReadPort @0,RTemp
		  andi RTemp,-(1<<@1)
		  WritePort @0,RTemp
		.endif
.ENDMACRO

;----------------------	macros for extended IO access -------------------
.macro	xout
.if	@0 >  0x3F
	sts	@0, @1
.else
	out	@0, @1
.endif
.endmacro
;---------------------------
.macro	xin
.if	@1 > 0x3F
	lds	@0, @1
.else
	in	@0, @1
.endif
.endmacro
;---------------------------
.macro  xlpm
.if FLASHEND >  0x7FFF
	elpm	@0, @1
.else
	lpm	@0, @1
.endif
.endmacro

 
