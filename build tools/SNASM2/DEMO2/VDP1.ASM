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

	xref _waitvbl

	align 4



	xdef	_store_sorted_sprites
; move a sprite list into VDP1 VRAM
; input
_store_sorted_sprites
	push	pr

	push	r8,r9,r11


	mov.l	#_spritebuf,r1			; r1 = address of sprite buffer
	mov.l	#_numsprites,r0			 
	mov.l	@r0,r2				; r2 = number of sprites to move


	mov.l	#ZDISTMAX,r3
	mov.l	#_zbuffer,r4
	mov.l	#VDP1RAM+(FIRSTSPRITE*32),r5	; r5 = current sprite in vram
	mov	#0,r7
	mov.l	#$ffff0000,r11
@storeloop
	mov.w	@r4,r0
@listloop
	cmp/eq	#0,r0
	bt	@skip

	mov.w	r7,@r4

	extu.w	r0,r8
	add	#-1,r8
	shll2	r8
	add	r1,r8

	mov.l	@r8+,r0
	mov.l	r0,@r5
	extu.w	r0,r0

	add	#4,r5
	mov	#7,r9

@moveloop:
	mov.l	@r8+,r6
	mov.l	r6,@r5

	dt	r9
	bf/s	@moveloop
	add	#4,r5

	bra	@listloop
	nop

@skip

	dt	r3
	bf/s	@storeloop
	add	#2,r4
			
@fini	
	mov.l	#$8000,r0
	mov.w	r0,@r5

	pop	r11,r9,r8

	pop	pr

	rts
	nop

	align 4

	lits


	xdef	wait_draw_end
; wait for VDP1 to finish drawing 
wait_draw_end
	mov.l	#EDSR,r1		; start the draw
@waitloop:
	mov.w	@r1,r0
	tst	#2,r0
	bt	@waitloop

	rts
	nop

	xdef	wait_last
; wait for VDP1 to finish drawing 
wait_last
	mov.l	#EDSR,r1		; start the draw
@waitloop:
	mov.w	@r1,r0
	tst	#2,r0
	bt	@waitloop

	rts
	nop




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


	xdef	_storesprites
; move a sprite list into VDP1 VRAM
; input
;	r4 = number of sprites to move
;	r5 = address of sprite list
;	r6 = address in VRAM to put list
_storesprites
	shll2	r4
	shll	r4

@moveloop:
	mov.l	@r5+,r0
	mov.l	r0,@r6

	dt	r4
	bf/s	@moveloop
	add	#4,r6

	rts
	nop

	align 4

	lits

	align 4

; initialize VDP1
	xdef _init_vdp1
_init_vdp1

	push	pr

	call	_waitvbl

	mov.l	#TVMR,r0
	mov	#VDP1MODE,r1
	mov.w	r1,@r0

	mov.l	#EWDR,r0
	mov	#0,r1
	mov.w	r1,@r0

	mov.l	#EWLR,r0
	mov	#0,r1
	mov.w	r1,@r0

	mov.l	#EWRR,r0
	mov	#((SCRNWIDTH/8)*512)+SCRNHEIGHT,r1
	mov.w	r1,@r0

	pop	pr

	rts
	nop

	lits

	align 4

	xdef	_storebitmap
; move a 32x color bitmap into VDP1 VRAM
; inputs
;	R4= address of bitmap
;	R5= address in VRAM to put bitmap
;	R6= type
_storebitmap

	mov	r6,r0
	cmp/eq	#0,r0
	bt	@16bit

	mov.w	@r4+,r3
	mov.w	@r4+,r0
	mulu	r3,r0
	sts	macl,r3
	shlr2	r3
	mov.l	#VDP1RAM,r0
	add	r0,r5
	mov.l	#$80008000,r1
	mov.l	#512,r0
	add	r0,r4
@moveloop1:
	mov.l	@r4+,r0
	or	r1,r0
	mov.l	r0,@r5
	dt	r3
	bf/s	@moveloop1
	add	#4,r5

	rts
	nop

	
@16bit:
	mov.w	@r4+,r3
	mov.w	@r4+,r0
	shll	r3
	mulu	r3,r0
	sts	macl,r3
	shlr2	r3
	mov.l	#VDP1RAM,r0
	add	r0,r5
	mov.l	#$80008000,r1
@moveloop2:
	mov.l	@r4+,r0
	or	r1,r0
	mov.l	r0,@r5

	dt	r3
	bf/s	@moveloop2
	add	#4,r5

	rts
	nop

	align 4

	lits



	
	xdef	storebitmap
; move a 32x color bitmap into VDP1 VRAM
; inputs
;	R1 = address of bitmap
;	R2 = address in VRAM to put bitmap
;	R3 = type
storebitmap

	mov	r3,r0
	cmp/eq	#0,r0
	bt	@16bit

	mov.w	@r1+,r3
	mov.w	@r1+,r0
	mulu	r3,r0
	sts	macl,r3
	shlr	r3
	mov.l	#VDP1RAM,r0
	add	r0,r2
	mov.l	#$8000,r4
	mov.l	#512,r0
	add	r0,r1
@moveloop1:
	mov.w	@r1+,r0
	or	r4,r0
	mov.w	r0,@r2
	dt	r3
	bf/s	@moveloop1
	add	#2,r2

	rts
	nop

	
@16bit:
	mov.w	@r1+,r3
	mov.w	@r1+,r0
	shll	r3
	mulu	r3,r0
	sts	macl,r3
	shlr	r3
	mov.l	#VDP1RAM,r0
	add	r0,r2
	mov.l	#$8000,r4
@moveloop2:
	mov.w	@r1+,r0
	or	r4,r0
	mov.w	r0,@r2
	dt	r3
	bf/s	@moveloop2
	add	#2,r2

	rts
	nop

	align 4

	lits


	align 4
	xdef	_spritelist, endsprites
_spritelist
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
	dc.w	SCRNWIDTH-1
	dc.w	SCRNHEIGHT-1
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
	dc.w	SCRNWIDTH-1
	dc.w	SCRNHEIGHT-1
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

	dc.w	FUNC_POLYGON		; control
	dc.w    0			; link     
	dc.w    $10E8			; drawMode 
	dc.w    $0000			; color    
	dc.w    0			; charAddr 
	dc.w    0			; charSize 
	dc.w    0, 178			; ax, ay   
	dc.w    SCRNWIDTH-1, 178	; bx, by   
	dc.w    SCRNWIDTH-1, SCRNHEIGHT-1		; cx, cy   
	dc.w    0, SCRNHEIGHT-1			; dx, dy   
	dc.w    0			; grshAddr 
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

	align 4

	end
