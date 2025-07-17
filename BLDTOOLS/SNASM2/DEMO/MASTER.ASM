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

	section	.text

;	regs	pc=Master_Start,r15=$06050000

;---------------------------------------------------------------*
;	Program Start ($120)
;---------------------------------------------------------------*

	xref	spritelist, endsprites

	xref storesprites
	xref storebitmap
	xref init_irqs
	xref init_vdp1
	xref waitvbl

	xref clear_fb
	xref start_draw
	xref flip_fb

	xref sp1

	xref _main

	export Master_Start
Master_Start
	mov.l	#M_STACK,r15

	call	init_irqs		; turn on the interupts

	mov.l	#$25f800f0,r1		; set vdp2 so that vdp1's framebuffer can be seen
	mov.l	#$ffff,r0
	mov.w	r0,@r1
	add.l	#2,r1
	mov.w	r0,@r1
	add.l	#2,r1
	mov.w	r0,@r1
	add.l	#2,r1
	mov.w	r0,@r1

	mov.l	#lastsprite,r0
	mov.l	#spritelist,r1
	mov.l	r1,@r0

	mov.l	#picstart,r1
	mov.l	#65536,r2
	call	storebitmap

	call	init_vdp1

	mov.l	#(endsprites-spritelist)/32,r1
	mov.l	#numsprites,r0
	mov.l	r1,@r0

mainloop:

	mov.l	#numsprites,r0
	mov.l	@r0,r1
	mov.l	#spritelist,r2
	call	storesprites

	call	start_draw

	call	clear_fb

	call	waitvbl

	call	flip_fb

	call	waitvbl

	call	dojoy


	bra	mainloop
	nop


	lits



	rts
	nop

	lits

	.align 4


waitstart
	mov.l	#pad0,r0
	mov.l	#PADSTART,r1
@waitloop1:
	mov.l	@r0,r2
	tst	r1,r2
	bt	@waitloop1

@waitloop2:
	mov.l	@r0,r2
	tst	r1,r2
	bf	@waitloop2

	rts
	nop


dojoy

	mov.l	#sp1+20,r1

	mov.w	@r1+,r3
	mov.w	@r1+,r4

	mov.l	#pad0,r0
	mov.l	@r0,r0

	tst	#PADUP,r0
	bt	@tstdown
	add	#-2,r4
	bra	@tstleft
	nop
@tstdown:
	tst	#PADDOWN,r0
	bt	@tstleft
	add	#2,r4
@tstleft:
	tst	#PADLEFT,r0
	bt	@tstright
	add	#-2,r3
	bra	@paddone
	nop
@tstright:
	tst	#PADRIGHT,r0
	bt	@paddone
	add	#2,r3
@paddone:
	mov.w	r4,@-r1
	mov.w	r3,@-r1

	rts
	nop

	lits


;-------------------------------------------------------;
; Include picture of king tut
;-------------------------------------------------------;

	cnop	0,4
picstart
	incbin	tut160.bin
	cnop	0,4

	.align 4
stackstart
	dcb.l	256,0
M_STACK

	.align 4

	end


