/*
 * ReadFlash.inc
 *
 *  Created: 06.05.2014 14:41:40
 *   Author: Tom
 */ 



 readflash:
   


   rcall getchar
   mov ZH,a0

   rcall getchar
   mov ZL,a0


		rcall getchar
   mov RLoopCounterH,a0
   		rcall getchar
   mov RLoopCounterL,a0



	readflashloop:

		.if FLASHEND > 0x7FFF
			out	RAMPZ, zx
			elpm	a0, z
			adiw	zh:zl, 1
			adc	zx, zerol		; 24 bit addition
		.else
			lpm	a0, z+
		.endif

		rcall putchar

		sbiw RLoopCounterH:RLoopCounterL,1
    brne readflashloop

		
     	rjmp Main_ok