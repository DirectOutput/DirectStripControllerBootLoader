;-------------------------------------------------------------------------
;		 		Program User Flash
;-------------------------------------------------------------------------
program_continue:
	ldi	a0, CONTINUE			;Send CONTINUE char
	rcall	putchar
program:
	set							;Set T flag, indicates exit allowed
	ldi	xl, low (ProgBuff)
	ldi	xh, high(ProgBuff)
	ldi	yh, high(ProgBuffEnd)
;------------------------------ Receive data into buffer -----------------
pogram_receivechar:
	rcall	getchar				;Receive character
	brne	program_storebyte	;Branch if not the COMMAND char
	rcall	getchar
	
	subi	a0, ESC_SHIFT
	brne	program_storebyte	; command,80 = end mark, branch if last char have not been ESC_SHIFT/80
	brts	program_exit		; Exit programmer code if T flag is set
	set							; Set T flag, indicates exit allowed
	rjmp	program_prog			
program_storebyte:
	rcall putchar
	clt							;Clear T flag, indicates exit not allowed
	st	x+, a0					;Store received byte in ram
	cpi	xl, low (ProgBuffEnd)	;Check if buffer is full
	cpc	xh, yh
	brne	pogram_receivechar	;Receive next char if buffer is not full
;-------------------------------------------------------------------------
program_prog:
	ldi	xl, low (ProgBuff)
	ldi	xh, high(ProgBuff)
program_progpages:
	rcall	prog_page				; CY = 1: o.k
	brcc	program_error_exit		; exit to  main_error if page programming has returned a error
	//ldi R24,'X'
	//FT245_SendByte R24
	subi	zl, low (-2*PageSize)
	sbci	zh, high(-2*PageSize)	; point to next page
.if FLASHEND > 0x7FFF
	sbci    zx, byte3(-2*BufferSize)
.endif
	cpi	xl, low (ProgBuffEnd)
	cpc	xh, yh
	brne	program_progpages		; until buffer end
	brtc	program_continue		;Continue if T flag is not set
;-------------------------------------------------------------------------
program_exit:
	rjmp	main_ok
program_error_exit:
  rjmp main_error
;-------------------------------------------------------------------------
;			Program page in Flash
;-------------------------------------------------------------------------
;use:	r0, r1, a0, xl, xh, zl, zh
;
;input:  X  = buffer to RAM
;        Z  = page to program
;output: CY = 0: error, attempt to overwrite itself
;
.equ    PAGEMASK	= (PAGESIZE * 2 - 1) & ~1

prog_page:
;------------------------------ Avoid self destruction ! -----------------
	cpi	zl, low (2*BootStart)
	ldi	a0, high(2*BootStart)
	cpc	zh, a0                  	; below bootloader ?
.if FLASHEND > 0x7FFF
	ldi     a0, byte3(2*BootStart)
	cpc     zx, a0
.endif
	brcc	_prp3				; CY = 0: error
;------------------------------ Fill page buffer -------------------------
_prp1:	ld	r0, x+
	ld	r1, x+
	ldi	a0, 1<<SPMEN
	rcall	do_spm
	adiw	zh:zl, 2
	mov	a0, zl
	andi	a0, low(PAGEMASK)
	brne	_prp1
;------------------------------ Erase page -------------------------------
	subi	zl, low (2*PAGESIZE)
	sbci	zh, high(2*PAGESIZE)
	ldi	a0, 1<<PGERS^1<<SPMEN		; erase page command
	rcall	do_spm
;------------------------------ Program page -----------------------------
	ldi	a0, 1<<PGWRT^1<<SPMEN		; write page command
	rcall	do_spm
	ldi	a0, 1<<RWWSRE^1<<SPMEN
do_spm:
	xout	SPMCSR, a0
.if FLASHEND > 0x7FFF
	out     RAMPZ, zx               ; 3 byte Z pointer
	xout    SPMCSR, a0
.endif
	spm
_prp2:
	xin	a0, SPMCSR
	sbrc	a0, SPMEN
	rjmp	_prp2
	sec					; CY = 1: successful
_prp3:
	ret
;-------------------------------------------------------------------------
