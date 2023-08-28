/****************************************************
 * Macros for FT245 USB interface
 *
 ***************************************************/ 

.macro FT245_SetInput
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


.macro FT245_WaitForRead_PayLoad ;(Payload)
  FT245_WaitForRead_PayLoad_Loop:
         rcall @0 
         sbic FT245_StatusPin, FT245_RxfBit
	 rjmp FT245_WaitForRead_PayLoad_Loop
.endm


.macro FT245_ReadByte  ;(Register) 
     FT245_WaitForRead
	 nop
	 nop
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
  nop
  nop
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

.macro FT245_ClearInputBuffer
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
.endm