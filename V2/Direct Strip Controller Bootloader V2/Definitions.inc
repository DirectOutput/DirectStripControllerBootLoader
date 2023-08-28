/*
 * Definitions.inc
 *
 *  Created: 05.09.2014 09:29:37
 *   Author: Tom
 */ 


 .equ	BootStart	= SECONDBOOTSTART   ; = 0xfc00
 .equ PAGESIZEB = PAGESIZE*2
.equ	Application	= 0
.equ FirmwareMessageStart=0x70 ;words
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

;----------------------------- Led port defs ---------------------------
  ;Leds
 .equ Led_DDR=DDRD
 .equ Led_Port=PORTD
 .equ Led_Yellow_Bit=2
 .equ Led_Red_Bit=3

;----------------------------- Registers ---------------------------
;R0 - reserved for SPM
;R1 - reserved for SPM

.def RData0=R16
.def RData1=R17
.def RData2=R18
.def RDataReady=R19
.def RTemp=R20
.def RL0=R21
.def RL1=R22
.def RL2=R23
.def LoopLo=R24
.def LoopHi=R25