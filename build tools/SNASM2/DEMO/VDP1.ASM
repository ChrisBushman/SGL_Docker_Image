;****************************************************************
;	Saturn Sample Program
;	VDP1 routines
;
;	Copyright SEGA of America inc. 1994
;---------------------------------------------------------------*
;****************************************************************

	include	"sh2.mac"
	include	"equ.h"
	include	"globals.h"
	include "regs.h"
	include "spcmd.h"

	section	.text

	xref waitvbl

	.align 4

SP1ADDR		equ	8192


	xdef	start_draw
; start VDP1 drawing in the next frame
start_draw
	mov.l	#PTMR,r0		; start the draw
	mov	#2,r1
	mov.w	r1,@r0

	rts
	nop


	xdef	clear_fb
; clear the frame buffer being displayed during the next frame
clear_fb
	mov.l	#FBCR,r0
	mov	#2,r1
	mov.w	r1,@r0

	rts
	nop


	xdef	flip_fb
; flip the frame buffer in the next frame
flip_fb
	mov.l	#FBCR,r0
	mov	#3,r1
	mov.w	r1,@r0

	rts
	nop


	xdef	storesprites
; move a sprite list into VDP1 VRAM
; input
;	r1 = number of sprites to move
;	r2 = address of sprite list
storesprites
	shll2	r1
	shll	r1

	mov.l	#VDP1RAM,r2

	mov.l	#spritelist,r0
@moveloop:
	mov.l	@r0+,r3
	mov.l	r3,@r2

	dt	r1
	bf/s	@moveloop
	add	#4,r2

	rts
	nop

	.align 4

	lits

	.align 4

; initialize VDP1
	xdef init_vdp1
init_vdp1

	push	pr

	call	waitvbl

	mov.l	#TVMR,r0
	mov	#0,r1
	mov.w	r1,@r0

	mov.l	#FBCR,r0
	mov	#2,r1
	mov.w	r1,@r0

	mov.l	#EWDR,r0
	mov	#0,r1
	mov.w	r1,@r0

	mov.l	#EWLR,r0
	mov	#0,r1
	mov.w	r1,@r0

	mov.l	#EWRR,r0
	mov	#(40*512)+224,r1
	mov.w	r1,@r0

	pop	pr

	rts
	nop

	lits

	.align 4

	xdef	storebitmap
; move a 32x color bitmap into VDP1 VRAM
; inputs
;	R1 = address of bitmap
;	R2 = address in VRAM to put bitmap
storebitmap
	mov.w	@r1+,r3
	mov.w	@r1+,r0
	shll	r3
	mulu	r3,r0
	sts	macl,r3
	shlr	r3
	mov.l	#VDP1RAM,r0
	add	r0,r2
	mov.l	#$8000,r4
@moveloop:
	mov.w	@r1+,r0
	or	r4,r0
	mov.w	r0,@r2
	dt	r3
	bf/s	@moveloop
	add	#2,r2

	rts
	nop

	.align 4

	lits


	.align 4
	xdef	spritelist, endsprites
spritelist
	dc.w	$0009
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	319
	dc.w	223
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000

	dc.w	$0008
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	319
	dc.w	223
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$FFFF

	dc.w	$000A
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$FFFF

	xdef	sp1
sp1

	dc.w	FUNC_DISTORSP		; control
	dc.w    0			; link
	dc.w    $10E8			; drawMode
	dc.w    $8000			; color
	dc.w    SP1ADDR			; charAddr
	dc.w    (20*256)+160		; charSize
	dc.w    0, 0			; ax, ay
	dc.w    159, 0			; bx, by
	dc.w    159, 159		; cx, cy
	dc.w    0, 159			; dx, dy
	dc.w    128			; grshAddr
	dc.w    0			; dummy


endsprite
	dc.w	$8000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000
	dc.w	$0000

endsprites

	.align 4

	end
