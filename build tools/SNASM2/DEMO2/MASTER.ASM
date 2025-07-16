;****************************************************************
;	Saturn Sample Program
;	SH2 Master Main
;
;	Copyright SEGA of America inc. 1994
;---------------------------------------------------------------*
;****************************************************************

	include	"sh2.mac"
	include	"equ.h"
	include	"globals.h"
	include "regs.h"
	include "spcmd.h"
	include "vdp2.h"

	section	.text

	xref	_main

	section.l .bss

	align 4
stackstart
	ds.l	1024
M_STACK

	align 4

	section	.text
	export Master_Start
Master_Start
	mov.l	#M_STACK,r15

	mov.l	#$300000,r0
@waitloop:
	nop
	dt	r0
	bf	@waitloop

	mov	#1,r4
	mov.l	#SYS_CHGSYSCK,r0
	mov.l	@r0,r0
	jsr	@r0
	nop

	mov.l	#M_STACK,r15

	mov.l	#objbase(.bss),r2
	mov.l	#(objlimit(.bss)-objbase(.bss))/2,r1
	mov	#0,r0
@clearloop:
	mov.w	r0,@r2

	dt	r1
	bf/s	@clearloop
	add	#2,r2

	mov.l	#_main,r0

	jmp	@r0			; main C entry point
	nop

	lits

	align 4

;-------------------------------------------------------;
; Include picture of king tut
;-------------------------------------------------------;

_oldvcounter	dc.l	0
	xdef _oldvcounter, _mathcounter
_mathcounter	dc.l	0

	cnop	0,4
	xdef	_redbox, _greenbox, _bluebox, _goldbox, _numbers
_redbox
	incbin	data\redbox.bin
_greenbox
	incbin	data\grnbox.bin
_bluebox
	incbin	data\bluebox.bin
_goldbox
	incbin	data\goldbox.bin

_numbers
	incbin	data\numbers.bin
	xdef	_grass
_grass
	incbin	data\grass.bin



	xdef	_pueblo			
_pueblo
	incbin	data\pueblo.bin
				
	cnop	0,4

	align 4

hexstr	dcb.b	16,0

	align 4

	end

                                                                                  
