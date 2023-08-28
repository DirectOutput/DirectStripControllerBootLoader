/*
 * macros_delay.inc
 *
 *  Created: 05.09.2014 09:53:16
 *   Author: Tom
 */ 


 .macro Delay3000ms
 	  ; ============================= 
	;   Warteschleifen-Generator 
	;     60000000 Zyklen:
	; ----------------------------- 
	; warte 49939965 Zyklen:
			  ldi  RData0, $FF
	WGLOOP0:  ldi  RData1, $FF
	WGLOOP1:  ldi  RData2, $FF
	WGLOOP2:  dec  RData2
			  brne WGLOOP2
			  dec  RData1
			  brne WGLOOP1
			  dec  RData0
			  brne WGLOOP0
	; ----------------------------- 
	; warte 10060014 Zyklen:
			  ldi  RData0, $83
	WGLOOP3:  ldi  RData1, $8F
	WGLOOP4:  ldi  RData2, $B2
	WGLOOP5:  dec  RData2
			  brne WGLOOP5
			  dec  RData1
			  brne WGLOOP4
			  dec  RData0
			  brne WGLOOP3
	; =============================
.endm