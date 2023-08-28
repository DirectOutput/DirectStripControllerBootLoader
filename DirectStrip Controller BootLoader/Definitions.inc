
;*************************************************************************
;-------------------------------------------------------------------------
;				Constant definitions
;-------------------------------------------------------------------------
.equ	VERSION		 = 0x0201

//.equ	XTAL		= 20000000	; 8MHz, not critical 
//.equ	BootDelay	= XTAL / 3	; 0.33s


;----------------------------- FT245 port defs ---------------------------

 ;FT245RL
 ;Use ports with adresses<0x20 for the FT245 to allow for most efficient communication
 .EQU FT245_DataPort = PORTC
 .equ FT245_DataPin = PINC
 .equ FT245_DataDDR = DDRC

 .equ FT245_StatusPin=PIND
 .equ FT245_StatusPort=PORTD
 .equ FT245_StatusDDR=DDRD

 .equ FT245_TXEBit=7
 .equ FT245_RXFBit=6
 .equ FT245_WRBit=4
 .equ FT245_RDBit=5

  ;Leds
 .equ Led_DDR=DDRD
 .equ Led_Port=PORTD
 .equ Led_Yellow_Bit=2
 .equ Led_Red_Bit=3

;------------------------------	select bootloader size -------------------



//.equ	BootSize	=   500 ;richtiger wert?
//.equ	BootSize	= CRC + VERIFY + ONEWIRE + WDTRIGGER + MinSize	;org version

.equ	BootStart	= FirstBootStart   ; = 0xfe00
.set	BufferSize	= 512  
.equ	UserFlash	= (2*BootStart) ;;==130048 == 0x1fc00
.equ	Application	= 0

/*;------------------------------	Bootloader fuse setting ------------------
.ifdef	FirstBootStart
  .if	(FlashEnd - FirstBootStart) >= 255	; 256 Words needed
    .equ	BootStart	= FirstBootStart   ;für m1284 wird dieser gewählt = 0xfe00
  .else
    .equ	BootStart	= SecondBootStart
  .endif
  ;----------------------------	max possible buffer size -----------------
  .set	BufferSize	= SRAM_SIZE / 2 - PAGESIZE   ;=8064

  .macro testpage
    .if		BootStart % BufferSize         ;=512 (true) @ first try
      .set	BufferSize = BufferSize - PAGESIZE ;=7936 @first try
      .if	BootStart % BufferSize ;=1536 @first try
        .set    BufferSize = BufferSize - PAGESIZE 
        testpage
      .endif
    .endif
  .endmacro
	testpage	; calculate Buffersize to fit into BootStart  ==== ergibt 512 für buffersize
  ;-----------------------------------------------------------------------
  .equ	UserFlash	= (2*BootStart)  ;==130048 == 0x1fc00
  .equ	Application	= 0
.else
  .equ	BootStart	= ((FlashEnd - BootSize) / PageSize * PageSize)
  .equ	BufferSize	= PageSize
  .equ	UserFlash	= (2*BootStart - 2)
  .equ	Application	= BootStart - 1
.endif
*/;-------------------------------------------------------------------------
;				Using register
;-------------------------------------------------------------------------
.def	zerol		= r2
.def	zeroh		= r3
//.def	baudl		= r4		; baud divider
//.def	baudh		= r5
//.def	crcl		= r6
//.def	crch		= r7

;-------------------------------------------------------------------------
//.def	appl		= r16		; rjmp to application
//.def	apph		= r17
.def RL2=r17
.def RL1=r16
.def RL0=r19
.def	RTemp = r20
.def RCommandByte=r18
.def RData=R4

.def	zx		= r21		; 3 byte Z pointer
.def	a0		= r22		; working registers
.def	a1		= r23
.def	RLoopCounterL		= r24		;Loopcounter low
.def	RLoopCounterH		= r25       ;Loopcounterhigh
;-------------------------------------------------------------------------
;				Using SRAM
;-------------------------------------------------------------------------
.dseg
	.org		SRAM_START
PROGBUFF:		.byte 2*BufferSize
PROGBUFFEND:
.cseg


;-------------------------------------------------------------------------

;------------------------------------------------------------------------
;			redefinitions for compatibility
;------------------------------------------------------------------------
.ifndef	WDTCSR
.equ	WDTCSR	= WDTCR
.endif
;---------------------------
.ifndef	WDCE
.equ	WDCE	= WDTOE
.endif
;---------------------------
.ifndef	SPMCSR
.equ    SPMCSR	= SPMCR
.endif
;---------------------------
.ifndef	RWWSRE
.ifdef  ASRE
.equ    RWWSRE  = ASRE
.endif
.endif
;---------------------------
.ifndef SPMEN
.equ    SPMEN	= SELFPRGEN
.endif

;------------------------------------------------------------------------
//*************************************************************************
//				Protocol definitions
//-------------------------------------------------------------------------
.equ	COMMAND		= 'C'		// Command sequence start
.equ ESCAPE	=	COMMAND



.equ	CONTINUE	= 0xA9



.equ	ESC_SHIFT	= 0x80		// offset escape char
