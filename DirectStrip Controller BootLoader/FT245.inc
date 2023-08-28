/*
 * FT245.inc
 *
 *  Created: 01.05.2014 13:30:05
 *   Author: Tom
 */ 

 getchar:

		


     sbic FT245_StatusPin, FT245_RxfBit	;Check if there is data to read
	 rjmp getchar

	 FT245_ReadByte a0

	 cpi a0, COMMAND					;check if a0 contains the COMMAND char. Ugly, but taken from org code
ret

putchar:

		


	sbic FT245_StatusPin, FT245_TxeBit	;Check if data can be written
	rjmp putchar

	FT245_SendByte a0

ret

putack:
  ldi a0,'A'
  rjmp putchar

putnack:
  ldi a0,'N'
  rjmp putchar
