;****************************************************************
;	Saturn Sample Program
;	VDP2 routines
;
;	Copyright SEGA of America inc. 1994
;---------------------------------------------------------------*
;****************************************************************

	include	"sh2.mac"
	include	"equ.h"
	include	"globals.h"
	include "regs.h"
	include	"vdp2.h"



	section.l .bss

	align 4

	xdef	_VDP2_Regs_Shadow
_VDP2_Regs_Shadow	ds.w	$120


	section	.text

	align 4


; draw a picture on the screen
; input
;	R4 = X pos on screen
;	R5 = Y pos on screen
;	R6 = address of picture to display
	xdef	_blit256
_blit256:
	mov.w	@r6+,r7		; R8 = Width in pixels
	mov.w	@r6+,r0		; R9 = Height in scanlines

	mov.l	#512,r1
	add	r1,r6		; R6 = address of source bitmap

	mulu	r1,r5
	sts	macl,r2
	add	r4,r2

	mov.l	#_VDP2_VRAM,r3
	add	r3,r2		; R2 = address on screen

	sub	r7,r1		; R1 = offset to next scanline

@lineloop:
	mov	r7,r5		; copy width into temp counter
@rowloop:
	mov.b	@r6+,r3		; fetch an 8bit pixel
	mov.b	r3,@r2		; stuff it into the frame buffer

	dt	r5		; loop for rest of scanline
	bf/s	@rowloop
	add	#1,r2		; increment the destination address

	dt	r0		; loop for height
	bf/s	@lineloop
	add	r1,r2		; go down to next scanline

	rts
	nop
	cnop	0,4
	lits


; draw a picture on the screen
; input
;	R4 = X pos on screen
;	R5 = Y pos on screen
;	R6 = address of picture to display
	xdef	_blit16mil
_blit16mil:
	mov.w	@(8,r6),r0		; R8 = Width in pixels
	mov	r0,r7
	mov.w	@(10,r6),r0		; R9 = Height in scanlines

	add	#32,r6

	mov.l	#2048,r1
	mulu	r1,r5
	sts	macl,r2

	shll2	r4
	add	r4,r2

	mov	r7,r3
	shll2	r7
	sub	r3,r1

	mov.l	#_VDP2_VRAM,r3
	add	r3,r2		; R2 = address on screen

@lineloop:
	mov	r7,r5		; copy width into temp counter
@rowloop:
	add	#1,r2
	mov.b	@r6+,r3		; fetch an 8bit pixel
	mov.b	r3,@r2		; stuff it into the frame buffer
	add	#1,r2

	mov.b	@r6+,r3		; fetch an 8bit pixel
	mov.b	r3,@r2		; stuff it into the frame buffer
	add	#1,r2

	mov.b	@r6+,r3		; fetch an 8bit pixel
	mov.b	r3,@r2		; stuff it into the frame buffer


	dt	r5		; loop for rest of scanline
	bf/s	@rowloop
	add	#1,r2		; increment the destination address

	dt	r0		; loop for height
	bf/s	@lineloop
	add	r1,r2		; go down to next scanline

	rts
	nop
	cnop	0,4
	lits



; draw a picture on the screen
; input
;	R1 = address of picture to display
;	R2 = X pos on screen
;	R3 = Y pos on screen
	xdef	_oblit256
_oblit256:
	mov.w	@r1+,r8	; R8 = Width in pixels
	mov.w	@r1+,r9	; R9 = Height in scanlines
	mov	r1,r0		; R0 = address of palette

	mov.l	#512,r4
	add	r4,r1		; R1 = address of source bitmap


	mov.l	#512,r5		; Calculate target address in frame buffer
	mulu	r5,r3
	sts	macl,r3
	add	r2,r3

	mov.l	#_VDP2_VRAM,r4
	add	r4,r3		; R3 = address on screen

	mov	r8,r6		; Calculate target address in frame buffer
	sub	r6,r5		; R5 = offset to next scanline

@lineloop:
	mov	r8,r6		; copy width into temp counter
@rowloop:
	mov.b	@r1+,r2		; fetch an 8bit pixel
	mov.b	r2,@r3		; stuff it into the frame buffer
	add	#1,r3		; increment the destination address

	dt	r6		; loop for rest of scanline
	bf	@rowloop

	add	r5,r3		; go down to next scanline

	dt	r9		; loop for height
	bf	@lineloop

	rts
	nop
	cnop	0,4
	lits


; move a palette to vdp2 palette ram
; inputs
;	R4 = address of palette data
;	R5 = number of colors to store
	xdef	_storepalette
_storepalette
	push	pr

	mov.l	#_VDP2_CRAM,r3
	mov.l	#$8000,r1
@moveloop
	mov.w	@r4+,r0
	or	r1,r0
	mov.w	r0,@r3
	dt	r5
	bf/s	@moveloop
	add	#2,r3

	pop	pr

	rts
	nop

	lits

_Store_RGBs

@loop
	mov.w	r4,@r6
	add.l	r5,r4
	dt	r7

	bf/s	@loop
	add.l	#2,r6

	rts
	nop

	lits

; set a vdp2 video mode
; inputs
;	R4 = mode number
	xdef _setvdp2mode
_setvdp2mode
	push	pr

	shll2	r4
	mov.l	#vdp2modes,r0
	mov.l	@(r4,r0),r0
	jsr	@r0
	nop

	pop	pr

	rts
	nop


	align 4
vdp2modes

	dc.l	TestMode0AB	;(mode 0) NBG0 & NBG1
			 	;320x224 resolution non-interlace
			 	;16 color palette mode
			 	;1 word PNT
			 	;1x1 cell mode
			 	;1x1 page per plane
			 	;same plane is used for each plane in 2x2 map


	dc.l	TestMode1AB	;(mode 1) NBG0 & NBG1
				;320x224 resolution non-interlace
				;256 color palette mode
				;1 word PNT
				;1x1 cell mode
				;1x1 page per plane
				;same plane is used for each plane in 2x2 map


	dc.l	TestMode2AB	;(mode 2) NBG0 & NBG1
				;320x224 resolution non-interlace
				;2048 color palette mode
				;1 word PNT
				;1x1 cell mode
				;1x1 page per plane
				;same plane is used for each plane in 2x2 map


	dc.l	TestMode3AB	;(mode 3) NBG0 & NBG1
				;320x224 resolution non-interlace
				;32786 color RGB mode
				;1 word PNT
				;1x1 cell mode
				;1x1 page per plane
				;same plane is used for each plane in 2x2 map


	dc.l	TestMode4A	;(mode 4) NBG0
				;320x224 resolution non-interlace
				;16,770.000 color RGB mode
				;1 word PNT
				;1x1 cell mode
				;1x1 page per plane
				;same plane is used for each plane in 2x2 map


	dc.l	TestMode0CD	;(mode 5) NBG2 & NBG3
				;320x224 resolution non-interlace
				;16 color palette mode
				;1 word PNT
				;1x1 cell mode
				;1x1 page per plane
				;same plane is used for each plane in 2x2 map


	dc.l	TestMode1CD	;(mode 6) NBG2 & NBG3
				;320x224 resolution non-interlace
				;256 color palette mode
				;1 word PNT
				;1x1 cell mode
				;1x1 page per plane
				;same plane is used for each plane in 2x2 map


	dc.l	TestMode0ABCD	;(mode 7) NBG0 & NBG1 & NBG2 & NBG3
				;320x224 resolution non-interlace
				;16 color palette mode
				;1 word PNT
				;1x1 cell mode
				;1x1 page per plane
				;same plane is used for each plane in 2x2 map


	dc.l	TestMode1ABCD	;(mode 8) NBG0 & NBG1 & NBG2 & NBG3
				;320x224 resolution non-interlace
				;256 color palette mode
				;1 word PNT
				;1x1 cell mode
				;1x1 page per plane
				;same plane is used for each plane in 2x2 map


	dc.l	TestMode8AB	;(mode 9) NBG0 & NBG1
				;320x224 resolution non-interlace
				;32786 RGB mode
				;Bit Map mode
				;1x1 page per plane
				;same plane is used for each plane in 2x2 map
				

	dc.l	TestMode9A	;(mode 10) NBG0 & NBG1
				;320x224 resolution non-interlace
				;16,770,000 color RGB mode
				;bit map mode
				;1x1 page per plane
				;same plane is used for each plane in 2x2 map
				

	dc.l	TestMode7AB	;(mode 11) NBG0 & NBG1
				;320x224 resolution non-interlace
				;2048 color palette mode
				;bit map mode
				;1x1 page per plane
				;same plane is used for each plane in 2x2 map
				

	dc.l	TestMode6AB	;(mode 12) NBG0 & NBG1
				;320x224 resolution non-interlace
				;256 color palette mode
				;bit map mode
				;1x1 page per plane
				;same plane is used for each plane in 2x2 map
				

	dc.l	TestMode5AB	;(mode 13) NBG0 & NBG1
				;320x224 resolution non-interlace
				;16 color palette mode
				;bit map mode
				;1x1 page per plane
				;same plane is used for each plane in 2x2 map
				
	dc.l	TestRGBmode1A	;(mode 14) NBG0 & NBG1
				;320x224 resolution non-interlace
				;32786 RGB mode
				;Bit Map mode
				;1x1 page per plane
				;same plane is used for each plane in 2x2 map
				


	lits

;-------------------------------------------->
TestMode6AB
;BG0 2048 palette color bit map mode
;BG1 2048 palette color bit map mode

@BackAdr	equ	$40000-224*2
@CharAdr0	equ	$00000
@CharAdr1	equ	$40000

	push	pr
	 
	mov.l	#BGMode6AB,r4			;init vdp2
	bsr	_Store_VDP2
	nop

	mov.l	#0,r4				;clear out vram
	mov.l	#0,r4
	mov.l	#0,r5
	mov.l	#_VDP2_VRAM+@CharAdr0,r6
	mov.l	#$80000/2,r7
	bsr	_Store_RGBs
	nop
	
	mov.l	#0,r4				;set back screen
	mov.l	#1,r5
	mov.l	#_VDP2_VRAM+@BackAdr,r6
	bsr	_Store_RGBs
	mov.l	#112,r7
	
	mov.l	#0,r4
	mov.l	#1<<5,r5
	bsr	_Store_RGBs
	mov.l	#112,r7

	mov.l	#$7fff,r4			;fill color ram with white
	mov.l	#0,r5
	mov.l	#_VDP2_CRAM,r6
	mov.l	#2048,r7
	bsr	_Store_RGBs
	nop

	mov.l	#_VDP2_CRAM,r6			;store blue as color 2
	mov.l	#31<<10,r0
	mov.w	r0,@(4,r6)	

	mov.l	#_VDP2_CRAM,r6			;store red and green as color 3
	mov.l	#(31)+(31<<5),r0
	mov.w	r0,@(6,r6)	

	mov.l	#_VDP2_CRAM,r6			;store red and blue as color 4
	mov.l	#(31)+(31<<10),r0
	mov.w	r0,@(8,r6)	



	pop	pr

	rts
	nop

	lits
	

;-------------------------------------------->
TestMode5AB
;BG0 16 palette color bit map mode
;BG1 16 palette color bit map mode

@BackAdr	equ	$40000-224*2
@CharAdr0	equ	$00000
@CharAdr1	equ	$40000

	push	pr
	 
	mov.l	#BGMode5AB,r4			;init vdp2
	bsr	_Store_VDP2
	nop
	

	pop	pr
	
	rts
	nop

	lits


;-------------------------------------------->
TestMode7AB
;BG0 2048 palette color bit map mode
;BG1 2048 palette color bit map mode

@BackAdr	equ	$40000-224*2
@CharAdr0	equ	$00000
@CharAdr1	equ	$40000

	push	pr
	 
	mov.l	#BGMode7AB,r4			;init vdp2
	bsr	_Store_VDP2
	nop
	
	pop	pr
	
	rts
	nop

	lits



	xdef TestMode9A		
;-------------------------------------------->
TestMode9A
;BG0 256 palette color bit map mode
;BG1 256 palette color bit map mode

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$00000
@CharAdr1	equ	$40000

	push	pr
	 
	mov.l	#BGMode9A,r4			;init vdp2
	bsr	_Store_VDP2
	nop
	

	pop	pr
	
	rts
	nop

	lits

;-------------------------------------------->
TestMode8AB
;BG0 256 palette color bit map mode
;BG1 256 palette color bit map mode

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$00000
@CharAdr1	equ	$40000

	push	pr
	 
	mov.l	#BGMode8AB,r4			;init vdp2
	bsr	_Store_VDP2
	nop

	mov.l	#0,r4				;clear out BG0 map
	mov.l	#0,r4
	mov.l	#0,r5
	mov.l	#_VDP2_VRAM+@CharAdr0,r6
	mov.l	#$40000/2,r7
	bsr	_Store_RGBs
	nop
	
	mov.l	#0,r4				;clear out BG1 map
	mov.l	#0,r4
	mov.l	#0,r5
	mov.l	#_VDP2_VRAM+@CharAdr1,r6
	mov.l	#$40000/2,r7
	bsr	_Store_RGBs
	nop

	mov.l	#0,r4				;set back screen
	mov.l	#1,r5
	mov.l	#_VDP2_VRAM+@BackAdr,r6
	bsr	_Store_RGBs
	mov.l	#112,r7
	
	mov.l	#0,r4
	mov.l	#1<<5,r5
	bsr	_Store_RGBs
	mov.l	#112,r7
	mov.l	#0,r4				;draw 1st line of chars to bit map of BG0
	
	

	pop	pr
	
	rts
	nop
	
	lits

;-------------------------------------------->
TestMode1ABCD
;BG0 Palette 256 colors
;BG1 Palette 256 colors
;BG2 Palette 256 colors
;BG3 Palette 256 colors

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$40000
@CharAdr1	equ	$60000
@CharAdr2	equ	$50000
@CharAdr3	equ	$70000
@PNTAdr0	equ	$00000
@PNTAdr1	equ	$10000
@PNTAdr2	equ	$20000
@PNTAdr3	equ	$30000

	push	pr
	 
	mov.l	#BGMode1ABCD,r4			;init vdp2
	bsr	_Store_VDP2
	nop
	
	pop	pr
	
	rts
	nop
	
	lits


;-------------------------------------------->
TestMode0ABCD
;BG0 Palette 16 colors
;BG1 Palette 16 colors
;BG2 Palette 16 colors
;BG3 Palette 16 colors

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$40000
@CharAdr1	equ	$60000
@CharAdr2	equ	$50000
@CharAdr3	equ	$70000
@PNTAdr0	equ	$00000
@PNTAdr1	equ	$10000
@PNTAdr2	equ	$20000
@PNTAdr3	equ	$30000

	push	pr
	 
	mov.l	#BGMode0ABCD,r4			;init vdp2
	bsr	_Store_VDP2
	nop
	
	pop	pr
	
	rts
	nop
	
	lits


;-------------------------------------------->
TestMode0CD
;BG2 Palette 16 colors
;BG3 Palette 16 colors

@BackAdr	equ	$20000-224*2
@CharAdr2	equ	$40000
@CharAdr3	equ	$60000
@PNTAdr2	equ	$00000
@PNTAdr3	equ	$10000

	push	pr
	 
	mov.l	#BGMode0CD,r4			;init vdp2
	bsr	_Store_VDP2
	nop

	pop	pr
	
	rts
	nop
	
	lits

	

;-------------------------------------------->
TestMode1CD
;BG2 Palette 256 colors
;BG3 Palette 256 colors

@BackAdr	equ	$20000-224*2
@CharAdr2	equ	$40000
@CharAdr3	equ	$60000
@PNTAdr2	equ	$00000
@PNTAdr3	equ	$10000

	push	pr
	 
	mov.l	#BGMode1CD,r4			;init vdp2
	bsr	_Store_VDP2
	nop
	
	pop	pr
	
	rts
	nop
	
	lits


;-------------------------------------------->
TestMode0AB
;BG0 Palette 16 colors
;BG1 Palette 16 colors

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$40000
@CharAdr1	equ	$60000
@PNTAdr0	equ	$00000
@PNTAdr1	equ	$10000

	push	pr
	 
	mov.l	#BGMode0AB,r4			;init vdp2
	bsr	_Store_VDP2
	nop
	
	pop	pr
	
	rts
	nop
	
	lits


;-------------------------------------------->
TestMode1AB
;BG0 Palette 256 colors
;BG1 Palette 256 colors

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$40000
@CharAdr1	equ	$60000
@PNTAdr0	equ	$00000
@PNTAdr1	equ	$10000

	push	pr
	 
	mov.l	#BGMode1AB,r4			;init vdp2
	bsr	_Store_VDP2
	nop
	
	pop	pr
	
	rts
	nop
	
	lits


;-------------------------------------------->
TestMode2AB
;BG0 Palette 2048 colors
;BG1 Palette 2048 colors

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$40000
@CharAdr1	equ	$60000
@PNTAdr0	equ	$00000
@PNTAdr1	equ	$10000

	push	pr
	 
	mov.l	#BGMode2AB,r4			;init vdp2
	bsr	_Store_VDP2
	nop
	
	pop	pr
	
	rts
	nop
	
	lits


;-------------------------------------------->
TestMode3AB
;BG0 RGB 32768 colors
;BG1 RGB 32768 colors

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$40000
@CharAdr1	equ	$60000
@PNTAdr0	equ	$00000
@PNTAdr1	equ	$10000

	push	pr
	 
	mov.l	#BGMode3AB,r4			;init vdp2
	bsr	_Store_VDP2
	nop
	
	pop	pr
	
	rts
	nop
	
	lits


;-------------------------------------------->
TestMode4A
;BG0 RGB 16,770,000 colors

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$40000
@PNTAdr0	equ	$00000

	push	pr
	 
	mov.l	#BGMode4A,r4			;init vdp2
	bsr	_Store_VDP2
	nop
	
	pop	pr
	
	rts
	nop
	
	lits

;-------------------------------------------->
TestRGBmode1A
;BG0 256 palette color bit map mode
;BG1 256 palette color bit map mode

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$40000
@PNTAdr0	equ	$00000

	push	pr
	 
	mov.l	#RGBmode1A,r4			;init vdp2
	bsr	_Store_VDP2
	nop

	mov.l	#0,r4				;clear out vram
	mov.l	#0,r4
	mov.l	#0,r5
	mov.l	#_VDP2_VRAM+@CharAdr0,r6
	mov.l	#$80000/2,r7
	bsr	_Store_RGBs
	nop
	
	mov.l	#0,r4				;set back screen
	mov.l	#1,r5
	mov.l	#_VDP2_VRAM+@BackAdr,r6
	bsr	_Store_RGBs
	mov.l	#112,r7
	
	mov.l	#0,r4
	mov.l	#1<<5,r5
	bsr	_Store_RGBs
	mov.l	#112,r7

	mov.l	#$7fff,r4			;fill color ram with white
	mov.l	#0,r5
	mov.l	#_VDP2_CRAM,r6
	mov.l	#2048,r7
	bsr	_Store_RGBs
	nop

	mov.l	#_VDP2_CRAM,r6			;store blue as color 2
	mov.l	#31<<10,r0
	mov.w	r0,@(4,r6)	

	mov.l	#_VDP2_CRAM,r6			;store red and green as color 3
	mov.l	#(31)+(31<<5),r0
	mov.w	r0,@(6,r6)	

	mov.l	#_VDP2_CRAM,r6			;store red and blue as color 4
	mov.l	#(31)+(31<<10),r0
	mov.w	r0,@(8,r6)	

		mov.l		#RotParmTable,r0		;src
		mov.l		#_VDP2_BASE+$1f000,r1	;dest - A1
		mov.l		#96/4,r2
@Parmloop
		mov.l		(r0)+,r3
		mov.l		r3,@(r1)
		dt		r2
		bf/s		@Parmloop
		add.l		#4,r1

	

	pop	pr
	
	rts
	nop
	
	lits




;þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ  þ	_Move_Mem
; What: 	moves long word memory from source to dest
;   In: -	r4 = source
;		r5 = dest
;		r6 = len in long words
;		r7 = mask to and with data
;  Out: -	r0 = 0
;		r4 = source+len*4
;		r5 = dest+len*4
;		r6 = 0
; Uses: 	?
;============================================================================

_Move_Longs
	push	pr

		shlr2	r6
		tst.l	r6,r6
		bt	@Exit	;none to move

@Loop		mov.l	@r4+,r0
		dt	r6
		and.l	r7,r0
		mov.l	r0,@r5
		bf/s	@Loop
		add.l	#4,r5

@Exit		rts
		mov.l	#0,r0
		


;þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ  þ	_Store_VDP2
; What:		initializes vdp2 registers and shadows them at _VDP2_Regs_Shadow 
;   In: -	r4 = table of VDP2 register offsets and values
;  Out: -	none
; Uses: 	?
;============================================================================

_Store_VDP2

		mov.l	#_VDP2_Regs,r1		;r1 = address of VDP2 hardware registers
		mov.l	#_VDP2_Regs_Shadow,r2	;r2 = address of VDP2 hardware shadow registers

@Loop		mov.w	@r4+,r0			;r0 = offset
		cmp/eq	#-1,r0			;if = to -1 then end of list
		bt/s	@Exit
		mov.w	@r4+,r3			;r3 = value to store
		mov.w	r3,@(r0,r1)		;store into VDP2 hardware register
		bra	@Loop
		mov.w	r3,@(r0,r2)		;store into VDP2 hardware shadow register

@Exit		rts
		nop

		lits
		
;======================================================================>
BGMode0AB					;16 color cell mode

;NBG0 & NBG1
;320x224 resolution non-interlace
;16 color palette mode
;1 word PNT
;1x1 cell mode
;1x1 page per plane
;same plane is used for each plane in 2x2 map

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$40000
@CharAdr1	equ	$60000
@PNTAdr0	equ	$00000
@PNTAdr1	equ	$10000

		_vdpDataBegin

@Step1		_vdpData	TVMD,	$8001			;tv on,border black,non-interlace,320x224 resolution

@Step2		_vdpData	BGON,	$0003			;BG0 & BG1 on
		_vdpData	BKTAU,	$8000+@BackAdr>>17	;back screen on,bits 16-18 of Back Screen Table Address
		_vdpData	BKTAL,	(@BackAdr>>1)&$ffff	;bits 0-15 of Back Screen Table Address

@Step3		_vdpData	CHCTLA,	$0000			;BG0 & BG1 - Palette 16 colors,cell format,1x1 cell
;		_vdpData	CHCTLB,	$0000			;BG2 & BG3 - RGB 256 color mode palette format

		_vdpData	PNCN0,	(1<<15)+@CharAdr0/$8000	;BG0 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PNCN1,	(1<<15)+@CharAdr1/$8000	;BG1 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN2,	(1<<15)+@CharAdr2/$8000	;BG2 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN3,	(1<<15)+@CharAdr3/$8000	;BG3 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PLSZ,	0			;1x1 pages in plane

		_vdpData	MPOFN,	0			;BG0 & BG1 & BG2 & BG3 map offset register bits
		_vdpData	MPABN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes C & D to VDP2 VRAM 0
		_vdpData	MPABN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG3 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG4 - point planes C & D to VDP2 VRAM 0
		
		_vdpData	MZCTL,	0			;BG0 & BG1 & BG2 & BG3 - no mosaic process

		_vdpData	SCXIN0,	0			;horz scroll integer part for BG0
		_vdpData	SCXDN0,	0			;horz scroll fraction part for BG0
		_vdpData	SCYIN0,	0			;vert scroll integer part for BG0
		_vdpData	SCYDN0,	0			;vert scroll fraction part for BG0
		_vdpData	ZMXIN0,	1			;horz co-ord increment integer part for BG0
		_vdpData	ZMXDN0,	0			;horz co-ord increment fraction part for BG0
		_vdpData	ZMYIN0,	1			;vert co-ord increment integer part for BG0
		_vdpData	ZMYDN0,	0			;vert co-ord increment fraction part for BG0
		
		_vdpData	SCXIN1,	0			;horz scroll integer part for BG1
		_vdpData	SCXDN1,	0			;horz scroll fraction part for BG1
		_vdpData	SCYIN1,	0			;vert scroll integer part for BG1
		_vdpData	SCYDN1,	0			;vert scroll fraction part for BG1
		_vdpData	ZMXIN1,	1			;horz co-ord increment integer part for BG1
		_vdpData	ZMXDN1,	0			;horz co-ord increment fraction part for BG1
		_vdpData	ZMYIN1,	1			;vert co-ord increment integer part for BG1
		_vdpData	ZMYDN1,	0			;vert co-ord increment fraction part for BG1
		
		_vdpData	ZMCTL,	0			;BG0 & BG1 - reduction turned off
		_vdpData	SCRCTL,	0			;BG0 & BG1 - horz and vert scrolling turned off
		_vdpData	LSTA0U,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA0L,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1U,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1L,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	VCSTAU,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1
		_vdpData	VCSTAL,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1

@Step4		_vdpData	RAMCTL,	$1300			;coeff table in vram,color mode 1,VRAM banks partioned into 4 banks

@Step5		_vdpData	WCTLA,	0			;no windows active
		_vdpData	WCTLB,	0			;no windows active
		_vdpData	WCTLC,	0			;no windows active
		_vdpData	WCTLD,	0			;no windows active
@Step6								;calc size of vram tables

@Step7
		_vdpData	CYCA0L,	$01ff			;access for 1 BG0 pnt,2 BG0 chars
		_vdpData	CYCA0U,	$ffff
		_vdpData	CYCA1L,	$ffff
		_vdpData	CYCA1U,	$ffff
		_vdpData	CYCB0L,	$4fff
		_vdpData	CYCB0U,	$ffff
		_vdpData	CYCB1L,	$5fff
		_vdpData	CYCB1U,	$ffff

@OtherSteps	_vdpData	CRAOFA,	0			;BG0 & BG1 & BG2 & BG3 - color offset
		_vdpData	PRINA,	$0302			;BG0 & BG1 priority = 1
;		_vdpData	PRINA,	$0504			;BG2 & BG3 priority = 1
		_vdpData	PRISA,	1			;sprite priority = 1
		_vdpData	CLOFEN,	0			;no color offsets for any screen

@Unknown	_vdpData	SPCTL,_SCL_SPCLMD		;?	

		_vdpDataEnd		

;======================================================================>
BGMode1AB					;256 color cell mode

;NBG0 & NBG1
;320x224 resolution non-interlace
;256 color palette mode
;1 word PNT
;1x1 cell mode
;1x1 page per plane
;same plane is used for each plane in 2x2 map


@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$40000
@CharAdr1	equ	$60000
@PNTAdr0	equ	$00000
@PNTAdr1	equ	$10000

		_vdpDataBegin

@Step1		_vdpData	TVMD,	$8001			;tv on,border black,non-interlace,320x224 resolution

@Step2		_vdpData	BGON,	$0003			;BG0 & BG1 on
		_vdpData	BKTAU,	$8000+@BackAdr>>17	;back screen on,bits 16-18 of Back Screen Table Address
		_vdpData	BKTAL,	(@BackAdr>>1)&$ffff	;bits 0-15 of Back Screen Table Address

@Step3		_vdpData	CHCTLA,	$1010			;BG0 & BG1 - Palette 256 colors,cell format,1x1 cell
;		_vdpData	CHCTLB,	$0000			;BG2 & BG3 - RGB 256 color mode palette format

		_vdpData	PNCN0,	(1<<15)+@CharAdr0/$8000	;BG0 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PNCN1,	(1<<15)+@CharAdr1/$8000	;BG1 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN2,	(1<<15)+@CharAdr2/$8000	;BG2 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN3,	(1<<15)+@CharAdr3/$8000	;BG3 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PLSZ,	0			;1x1 pages in plane

		_vdpData	MPOFN,	0			;BG0 & BG1 & BG2 & BG3 map offset register bits
		_vdpData	MPABN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes C & D to VDP2 VRAM 0
		_vdpData	MPABN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG3 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG4 - point planes C & D to VDP2 VRAM 0
		
		_vdpData	MZCTL,	0			;BG0 & BG1 & BG2 & BG3 - no mosaic process

		_vdpData	SCXIN0,	0			;horz scroll integer part for BG0
		_vdpData	SCXDN0,	0			;horz scroll fraction part for BG0
		_vdpData	SCYIN0,	0			;vert scroll integer part for BG0
		_vdpData	SCYDN0,	0			;vert scroll fraction part for BG0
		_vdpData	ZMXIN0,	1			;horz co-ord increment integer part for BG0
		_vdpData	ZMXDN0,	0			;horz co-ord increment fraction part for BG0
		_vdpData	ZMYIN0,	1			;vert co-ord increment integer part for BG0
		_vdpData	ZMYDN0,	0			;vert co-ord increment fraction part for BG0
		
		_vdpData	SCXIN1,	0			;horz scroll integer part for BG1
		_vdpData	SCXDN1,	0			;horz scroll fraction part for BG1
		_vdpData	SCYIN1,	0			;vert scroll integer part for BG1
		_vdpData	SCYDN1,	0			;vert scroll fraction part for BG1
		_vdpData	ZMXIN1,	1			;horz co-ord increment integer part for BG1
		_vdpData	ZMXDN1,	0			;horz co-ord increment fraction part for BG1
		_vdpData	ZMYIN1,	1			;vert co-ord increment integer part for BG1
		_vdpData	ZMYDN1,	0			;vert co-ord increment fraction part for BG1
		
		_vdpData	ZMCTL,	0			;BG0 & BG1 - reduction turned off
		_vdpData	SCRCTL,	0			;BG0 & BG1 - horz and vert scrolling turned off
		_vdpData	LSTA0U,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA0L,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1U,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1L,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	VCSTAU,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1
		_vdpData	VCSTAL,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1

@Step4		_vdpData	RAMCTL,	$1300			;coeff table in vram,color mode 1,VRAM banks partioned into 4 banks

@Step5		_vdpData	WCTLA,	0			;no windows active
		_vdpData	WCTLB,	0			;no windows active
		_vdpData	WCTLC,	0			;no windows active
		_vdpData	WCTLD,	0			;no windows active
@Step6								;calc size of vram tables

@Step7
		_vdpData	CYCA0L,	$01ff			;access for 1 BG0 pnt,2 BG0 chars
		_vdpData	CYCA0U,	$ffff
		_vdpData	CYCA1L,	$ffff
		_vdpData	CYCA1U,	$ffff
		_vdpData	CYCB0L,	$44ff
		_vdpData	CYCB0U,	$ffff
		_vdpData	CYCB1L,	$55ff
		_vdpData	CYCB1U,	$ffff

@OtherSteps	_vdpData	CRAOFA,	0			;BG0 & BG1 & BG2 & BG3 - color offset
		_vdpData	PRINA,	$0302			;BG0 & BG1 priority = 1
;		_vdpData	PRINA,	$0504			;BG2 & BG3 priority = 1
		_vdpData	PRISA,	1			;sprite priority = 1
		_vdpData	CLOFEN,	0			;no color offsets for any screen

@Unknown	_vdpData	SPCTL,_SCL_SPCLMD		;?	

		_vdpDataEnd		

;======================================================================>
BGMode2AB					;2048 color cell mode

;NBG0 & NBG1
;320x224 resolution non-interlace
;2048 color palette mode
;1 word PNT
;1x1 cell mode
;1x1 page per plane
;same plane is used for each plane in 2x2 map

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$40000
@CharAdr1	equ	$60000
@PNTAdr0	equ	$00000
@PNTAdr1	equ	$10000

		_vdpDataBegin

@Step1		_vdpData	TVMD,	$8001			;tv on,border black,non-interlace,320x224 resolution

@Step2		_vdpData	BGON,	$0003			;BG0 & BG1 on
		_vdpData	BKTAU,	$8000+@BackAdr>>17	;back screen on,bits 16-18 of Back Screen Table Address
		_vdpData	BKTAL,	(@BackAdr>>1)&$ffff	;bits 0-15 of Back Screen Table Address

@Step3		_vdpData	CHCTLA,	$2020			;BG0 & BG1 - Palette 2048 colors,cell format,1x1 cell
;		_vdpData	CHCTLB,	$0000			;BG2 & BG3 - RGB 256 color mode palette format

		_vdpData	PNCN0,	(1<<15)+@CharAdr0/$8000	;BG0 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PNCN1,	(1<<15)+@CharAdr1/$8000	;BG1 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN2,	(1<<15)+@CharAdr2/$8000	;BG2 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN3,	(1<<15)+@CharAdr3/$8000	;BG3 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PLSZ,	0			;1x1 pages in plane

		_vdpData	MPOFN,	0			;BG0 & BG1 & BG2 & BG3 map offset register bits
		_vdpData	MPABN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes C & D to VDP2 VRAM 0
		_vdpData	MPABN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG3 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG4 - point planes C & D to VDP2 VRAM 0
		
		_vdpData	MZCTL,	0			;BG0 & BG1 & BG2 & BG3 - no mosaic process

		_vdpData	SCXIN0,	0			;horz scroll integer part for BG0
		_vdpData	SCXDN0,	0			;horz scroll fraction part for BG0
		_vdpData	SCYIN0,	0			;vert scroll integer part for BG0
		_vdpData	SCYDN0,	0			;vert scroll fraction part for BG0
		_vdpData	ZMXIN0,	1			;horz co-ord increment integer part for BG0
		_vdpData	ZMXDN0,	0			;horz co-ord increment fraction part for BG0
		_vdpData	ZMYIN0,	1			;vert co-ord increment integer part for BG0
		_vdpData	ZMYDN0,	0			;vert co-ord increment fraction part for BG0
		
		_vdpData	SCXIN1,	0			;horz scroll integer part for BG1
		_vdpData	SCXDN1,	0			;horz scroll fraction part for BG1
		_vdpData	SCYIN1,	0			;vert scroll integer part for BG1
		_vdpData	SCYDN1,	0			;vert scroll fraction part for BG1
		_vdpData	ZMXIN1,	1			;horz co-ord increment integer part for BG1
		_vdpData	ZMXDN1,	0			;horz co-ord increment fraction part for BG1
		_vdpData	ZMYIN1,	1			;vert co-ord increment integer part for BG1
		_vdpData	ZMYDN1,	0			;vert co-ord increment fraction part for BG1
		
		_vdpData	ZMCTL,	0			;BG0 & BG1 - reduction turned off
		_vdpData	SCRCTL,	0			;BG0 & BG1 - horz and vert scrolling turned off
		_vdpData	LSTA0U,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA0L,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1U,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1L,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	VCSTAU,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1
		_vdpData	VCSTAL,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1

@Step4		_vdpData	RAMCTL,	$1300			;coeff table in vram,color mode 1,VRAM banks partioned into 4 banks

@Step5		_vdpData	WCTLA,	0			;no windows active
		_vdpData	WCTLB,	0			;no windows active
		_vdpData	WCTLC,	0			;no windows active
		_vdpData	WCTLD,	0			;no windows active
@Step6								;calc size of vram tables

@Step7
		_vdpData	CYCA0L,	$01ff			;access for 1 BG0 pnt,2 BG0 chars
		_vdpData	CYCA0U,	$ffff
		_vdpData	CYCA1L,	$ffff
		_vdpData	CYCA1U,	$ffff
		_vdpData	CYCB0L,	$4444
		_vdpData	CYCB0U,	$ffff
		_vdpData	CYCB1L,	$555f
		_vdpData	CYCB1U,	$ffff

@OtherSteps	_vdpData	CRAOFA,	0			;BG0 & BG1 & BG2 & BG3 - color offset
		_vdpData	PRINA,	$0302			;BG0 & BG1 priority = 1
;		_vdpData	PRINA,	$0504			;BG2 & BG3 priority = 1
		_vdpData	PRISA,	1			;sprite priority = 1
		_vdpData	CLOFEN,	0			;no color offsets for any screen

@Unknown	_vdpData	SPCTL,_SCL_SPCLMD		;?	

		_vdpDataEnd		

;======================================================================>
BGMode3AB					;32768 color cell mode

;NBG0 & NBG1
;320x224 resolution non-interlace
;32786 color RGB mode
;1 word PNT
;1x1 cell mode
;1x1 page per plane
;same plane is used for each plane in 2x2 map

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$40000
@CharAdr1	equ	$60000
@PNTAdr0	equ	$00000
@PNTAdr1	equ	$10000

		_vdpDataBegin

@Step1		_vdpData	TVMD,	$8001			;tv on,border black,non-interlace,320x224 resolution

@Step2		_vdpData	BGON,	$0003			;BG0 & BG1 on
		_vdpData	BKTAU,	$8000+@BackAdr>>17	;back screen on,bits 16-18 of Back Screen Table Address
		_vdpData	BKTAL,	(@BackAdr>>1)&$ffff	;bits 0-15 of Back Screen Table Address

@Step3		_vdpData	CHCTLA,	$3030			;BG0 & BG1 - RGB 32786 colors,cell format,1x1 cell
;		_vdpData	CHCTLB,	$0000			;BG2 & BG3 - RGB 256 color mode palette format

		_vdpData	PNCN0,	(1<<15)+@CharAdr0/$8000	;BG0 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PNCN1,	(1<<15)+@CharAdr1/$8000	;BG1 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN2,	(1<<15)+@CharAdr2/$8000	;BG2 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN3,	(1<<15)+@CharAdr3/$8000	;BG3 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PLSZ,	0			;1x1 pages in plane

		_vdpData	MPOFN,	0			;BG0 & BG1 & BG2 & BG3 map offset register bits
		_vdpData	MPABN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes C & D to VDP2 VRAM 0
		_vdpData	MPABN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG3 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG4 - point planes C & D to VDP2 VRAM 0
		
		_vdpData	MZCTL,	0			;BG0 & BG1 & BG2 & BG3 - no mosaic process

		_vdpData	SCXIN0,	0			;horz scroll integer part for BG0
		_vdpData	SCXDN0,	0			;horz scroll fraction part for BG0
		_vdpData	SCYIN0,	0			;vert scroll integer part for BG0
		_vdpData	SCYDN0,	0			;vert scroll fraction part for BG0
		_vdpData	ZMXIN0,	1			;horz co-ord increment integer part for BG0
		_vdpData	ZMXDN0,	0			;horz co-ord increment fraction part for BG0
		_vdpData	ZMYIN0,	1			;vert co-ord increment integer part for BG0
		_vdpData	ZMYDN0,	0			;vert co-ord increment fraction part for BG0
		
		_vdpData	SCXIN1,	0			;horz scroll integer part for BG1
		_vdpData	SCXDN1,	0			;horz scroll fraction part for BG1
		_vdpData	SCYIN1,	0			;vert scroll integer part for BG1
		_vdpData	SCYDN1,	0			;vert scroll fraction part for BG1
		_vdpData	ZMXIN1,	1			;horz co-ord increment integer part for BG1
		_vdpData	ZMXDN1,	0			;horz co-ord increment fraction part for BG1
		_vdpData	ZMYIN1,	1			;vert co-ord increment integer part for BG1
		_vdpData	ZMYDN1,	0			;vert co-ord increment fraction part for BG1
		
		_vdpData	ZMCTL,	0			;BG0 & BG1 - reduction turned off
		_vdpData	SCRCTL,	0			;BG0 & BG1 - horz and vert scrolling turned off
		_vdpData	LSTA0U,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA0L,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1U,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1L,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	VCSTAU,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1
		_vdpData	VCSTAL,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1

@Step4		_vdpData	RAMCTL,	$1300			;coeff table in vram,color mode 1,VRAM banks partioned into 4 banks

@Step5		_vdpData	WCTLA,	0			;no windows active
		_vdpData	WCTLB,	0			;no windows active
		_vdpData	WCTLC,	0			;no windows active
		_vdpData	WCTLD,	0			;no windows active
@Step6								;calc size of vram tables

@Step7
		_vdpData	CYCA0L,	$01ff			;access for 1 BG0 pnt,2 BG0 chars
		_vdpData	CYCA0U,	$ffff
		_vdpData	CYCA1L,	$ffff
		_vdpData	CYCA1U,	$ffff
		_vdpData	CYCB0L,	$4444
		_vdpData	CYCB0U,	$ffff
		_vdpData	CYCB1L,	$5555
		_vdpData	CYCB1U,	$ffff

@OtherSteps	_vdpData	CRAOFA,	0			;BG0 & BG1 & BG2 & BG3 - color offset
		_vdpData	PRINA,	$0302			;BG0 & BG1 priority = 1
;		_vdpData	PRINA,	$0504			;BG2 & BG3 priority = 1
		_vdpData	PRISA,	1			;sprite priority = 1
		_vdpData	CLOFEN,	0			;no color offsets for any screen

@Unknown	_vdpData	SPCTL,_SCL_SPCLMD		;?	

		_vdpDataEnd		

;======================================================================>
BGMode4A					;16 million color cell mode

;NBG0
;320x224 resolution non-interlace
;16,770.000 color RGB mode
;1 word PNT
;1x1 cell mode
;1x1 page per plane
;same plane is used for each plane in 2x2 map

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$40000
@CharAdr1	equ	$60000
@PNTAdr0	equ	$00000
@PNTAdr1	equ	$10000

		_vdpDataBegin

@Step1		_vdpData	TVMD,	$8001			;tv on,border black,non-interlace,320x224 resolution

@Step2		_vdpData	BGON,	$0001			;BG0 & BG1 on
		_vdpData	BKTAU,	$8000+@BackAdr>>17	;back screen on,bits 16-18 of Back Screen Table Address
		_vdpData	BKTAL,	(@BackAdr>>1)&$ffff	;bits 0-15 of Back Screen Table Address

@Step3		_vdpData	CHCTLA,	$0040			;BG0 & BG1 - RGB 16,770,000 colors,cell format,1x1 cell
;		_vdpData	CHCTLB,	$0000			;BG2 & BG3 - RGB 256 color mode palette format

		_vdpData	PNCN0,	(1<<15)+@CharAdr0/$8000	;BG0 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN1,	(1<<15)+@CharAdr1/$8000	;BG1 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN2,	(1<<15)+@CharAdr2/$8000	;BG2 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN3,	(1<<15)+@CharAdr3/$8000	;BG3 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PLSZ,	0			;1x1 pages in plane

		_vdpData	MPOFN,	0			;BG0 & BG1 & BG2 & BG3 map offset register bits
		_vdpData	MPABN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG3 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG4 - point planes C & D to VDP2 VRAM 0
		
		_vdpData	MZCTL,	0			;BG0 & BG1 & BG2 & BG3 - no mosaic process

		_vdpData	SCXIN0,	0			;horz scroll integer part for BG0
		_vdpData	SCXDN0,	0			;horz scroll fraction part for BG0
		_vdpData	SCYIN0,	0			;vert scroll integer part for BG0
		_vdpData	SCYDN0,	0			;vert scroll fraction part for BG0
		_vdpData	ZMXIN0,	1			;horz co-ord increment integer part for BG0
		_vdpData	ZMXDN0,	0			;horz co-ord increment fraction part for BG0
		_vdpData	ZMYIN0,	1			;vert co-ord increment integer part for BG0
		_vdpData	ZMYDN0,	0			;vert co-ord increment fraction part for BG0
		
;		_vdpData	SCXIN1,	0			;horz scroll integer part for BG1
;		_vdpData	SCXDN1,	0			;horz scroll fraction part for BG1
;		_vdpData	SCYIN1,	0			;vert scroll integer part for BG1
;		_vdpData	SCYDN1,	0			;vert scroll fraction part for BG1
;		_vdpData	ZMXIN1,	1			;horz co-ord increment integer part for BG1
;		_vdpData	ZMXDN1,	0			;horz co-ord increment fraction part for BG1
;		_vdpData	ZMYIN1,	1			;vert co-ord increment integer part for BG1
;		_vdpData	ZMYDN1,	0			;vert co-ord increment fraction part for BG1
		
		_vdpData	ZMCTL,	0			;BG0 & BG1 - reduction turned off
		_vdpData	SCRCTL,	0			;BG0 & BG1 - horz and vert scrolling turned off
		_vdpData	LSTA0U,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA0L,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1U,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1L,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	VCSTAU,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1
		_vdpData	VCSTAL,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1

@Step4		_vdpData	RAMCTL,	$1300			;coeff table in vram,color mode 1,VRAM banks partioned into 4 banks

@Step5		_vdpData	WCTLA,	0			;no windows active
		_vdpData	WCTLB,	0			;no windows active
		_vdpData	WCTLC,	0			;no windows active
		_vdpData	WCTLD,	0			;no windows active
@Step6								;calc size of vram tables

@Step7
		_vdpData	CYCA0L,	$0fff			;access for 1 BG0 pnt,2 BG0 chars
		_vdpData	CYCA0U,	$ffff
		_vdpData	CYCA1L,	$ffff
		_vdpData	CYCA1U,	$ffff
		_vdpData	CYCB0L,	$4444
		_vdpData	CYCB0U,	$4444
		_vdpData	CYCB1L,	$ffff
		_vdpData	CYCB1U,	$ffff

@OtherSteps	_vdpData	CRAOFA,	0			;BG0 & BG1 & BG2 & BG3 - color offset
		_vdpData	PRINA,	$0302			;BG0 & BG1 priority = 1
;		_vdpData	PRINA,	$0504			;BG2 & BG3 priority = 1
		_vdpData	PRISA,	1			;sprite priority = 1
		_vdpData	CLOFEN,	0			;no color offsets for any screen

@Unknown	_vdpData	SPCTL,_SCL_SPCLMD		;?	

		_vdpDataEnd		

;======================================================================>
BGMode0CD					;16 color cell mode NBG2 & NBG3 only

;NBG2 & NBG3
;320x224 resolution non-interlace
;16 color palette mode
;1 word PNT
;1x1 cell mode
;1x1 page per plane
;same plane is used for each plane in 2x2 map

@BackAdr	equ	$20000-224*2
@CharAdr2	equ	$40000
@CharAdr3	equ	$60000
@PNTAdr2	equ	$00000
@PNTAdr3	equ	$10000

		_vdpDataBegin

@Step1		_vdpData	TVMD,	$8001			;tv on,border black,non-interlace,320x224 resolution

@Step2		_vdpData	BGON,	$000c			;BG2 & BG3 on
		_vdpData	BKTAU,	$8000+@BackAdr>>17	;back screen on,bits 16-18 of Back Screen Table Address
		_vdpData	BKTAL,	(@BackAdr>>1)&$ffff	;bits 0-15 of Back Screen Table Address

@Step3		_vdpData	CHCTLA,	$0000			;BG0 & BG1 - Palette 16 colors,cell format,1x1 cell
		_vdpData	CHCTLB,	$0000			;BG2 & BG3 - RGB 16 color mode palette format

;		_vdpData	PNCN0,	(1<<15)+@CharAdr0/$8000	;BG0 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN1,	(1<<15)+@CharAdr1/$8000	;BG1 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PNCN2,	(1<<15)+@CharAdr2/$8000	;BG2 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PNCN3,	(1<<15)+@CharAdr3/$8000	;BG3 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PLSZ,	0			;1x1 pages in plane

		_vdpData	MPOFN,	0			;BG0 & BG1 & BG2 & BG3 map offset register bits
;		_vdpData	MPABN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes C & D to VDP2 VRAM 0
		_vdpData	MPABN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes C & D to VDP2 VRAM 0
		_vdpData	MPABN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG3 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG4 - point planes C & D to VDP2 VRAM 0
		
		_vdpData	MZCTL,	0			;BG0 & BG1 & BG2 & BG3 - no mosaic process

		_vdpData	SCXIN0,	0			;horz scroll integer part for BG0
		_vdpData	SCXDN0,	0			;horz scroll fraction part for BG0
		_vdpData	SCYIN0,	0			;vert scroll integer part for BG0
		_vdpData	SCYDN0,	0			;vert scroll fraction part for BG0
		_vdpData	ZMXIN0,	1			;horz co-ord increment integer part for BG0
		_vdpData	ZMXDN0,	0			;horz co-ord increment fraction part for BG0
		_vdpData	ZMYIN0,	1			;vert co-ord increment integer part for BG0
		_vdpData	ZMYDN0,	0			;vert co-ord increment fraction part for BG0
		
		_vdpData	SCXIN1,	0			;horz scroll integer part for BG1
		_vdpData	SCXDN1,	0			;horz scroll fraction part for BG1
		_vdpData	SCYIN1,	0			;vert scroll integer part for BG1
		_vdpData	SCYDN1,	0			;vert scroll fraction part for BG1
		_vdpData	ZMXIN1,	1			;horz co-ord increment integer part for BG1
		_vdpData	ZMXDN1,	0			;horz co-ord increment fraction part for BG1
		_vdpData	ZMYIN1,	1			;vert co-ord increment integer part for BG1
		_vdpData	ZMYDN1,	0			;vert co-ord increment fraction part for BG1
		
		_vdpData	ZMCTL,	0			;BG0 & BG1 - reduction turned off
		_vdpData	SCRCTL,	0			;BG0 & BG1 - horz and vert scrolling turned off
		_vdpData	LSTA0U,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA0L,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1U,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1L,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	VCSTAU,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1
		_vdpData	VCSTAL,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1

@Step4		_vdpData	RAMCTL,	$1300			;coeff table in vram,color mode 1,VRAM banks partioned into 4 banks

@Step5		_vdpData	WCTLA,	0			;no windows active
		_vdpData	WCTLB,	0			;no windows active
		_vdpData	WCTLC,	0			;no windows active
		_vdpData	WCTLD,	0			;no windows active
@Step6								;calc size of vram tables

@Step7
		_vdpData	CYCA0L,	$23ff			;access for 1 BG0 pnt,2 BG0 chars
		_vdpData	CYCA0U,	$ffff
		_vdpData	CYCA1L,	$ffff
		_vdpData	CYCA1U,	$ffff
		_vdpData	CYCB0L,	$6fff
		_vdpData	CYCB0U,	$ffff
		_vdpData	CYCB1L,	$7fff
		_vdpData	CYCB1U,	$ffff

@OtherSteps	_vdpData	CRAOFA,	0			;BG0 & BG1 & BG2 & BG3 - color offset
;		_vdpData	PRINA,	$0302			;BG0 & BG1 priority = 1
		_vdpData	PRINB,	$0504			;BG2 & BG3 priority = 1
		_vdpData	PRISA,	1			;sprite priority = 1
		_vdpData	CLOFEN,	0			;no color offsets for any screen

@Unknown	_vdpData	SPCTL,_SCL_SPCLMD		;?	

		_vdpDataEnd		

;======================================================================>
BGMode1CD

;NBG2 & NBG3
;320x224 resolution non-interlace
;256 color palette mode
;1 word PNT
;1x1 cell mode
;1x1 page per plane
;same plane is used for each plane in 2x2 map

@BackAdr	equ	$20000-224*2
@CharAdr2	equ	$40000
@CharAdr3	equ	$60000
@PNTAdr2	equ	$00000
@PNTAdr3	equ	$10000

		_vdpDataBegin

@Step1		_vdpData	TVMD,	$8001			;tv on,border black,non-interlace,320x224 resolution

@Step2		_vdpData	BGON,	$000c			;BG2 & BG3 on
		_vdpData	BKTAU,	$8000+@BackAdr>>17	;back screen on,bits 16-18 of Back Screen Table Address
		_vdpData	BKTAL,	(@BackAdr>>1)&$ffff	;bits 0-15 of Back Screen Table Address

@Step3		_vdpData	CHCTLA,	$0000			;BG0 & BG1 - Palette 16 colors,cell format,1x1 cell
		_vdpData	CHCTLB,	$0022			;BG2 & BG3 - RGB 256 color mode palette format

;		_vdpData	PNCN0,	(1<<15)+@CharAdr0/$8000	;BG0 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN1,	(1<<15)+@CharAdr1/$8000	;BG1 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PNCN2,	(1<<15)+@CharAdr2/$8000	;BG2 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PNCN3,	(1<<15)+@CharAdr3/$8000	;BG3 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PLSZ,	0			;1x1 pages in plane

		_vdpData	MPOFN,	0			;BG0 & BG1 & BG2 & BG3 map offset register bits
;		_vdpData	MPABN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes C & D to VDP2 VRAM 0
		_vdpData	MPABN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes C & D to VDP2 VRAM 0
		_vdpData	MPABN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG3 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG4 - point planes C & D to VDP2 VRAM 0
		
		_vdpData	MZCTL,	0			;BG0 & BG1 & BG2 & BG3 - no mosaic process

		_vdpData	SCXIN0,	0			;horz scroll integer part for BG0
		_vdpData	SCXDN0,	0			;horz scroll fraction part for BG0
		_vdpData	SCYIN0,	0			;vert scroll integer part for BG0
		_vdpData	SCYDN0,	0			;vert scroll fraction part for BG0
		_vdpData	ZMXIN0,	1			;horz co-ord increment integer part for BG0
		_vdpData	ZMXDN0,	0			;horz co-ord increment fraction part for BG0
		_vdpData	ZMYIN0,	1			;vert co-ord increment integer part for BG0
		_vdpData	ZMYDN0,	0			;vert co-ord increment fraction part for BG0
		
		_vdpData	SCXIN1,	0			;horz scroll integer part for BG1
		_vdpData	SCXDN1,	0			;horz scroll fraction part for BG1
		_vdpData	SCYIN1,	0			;vert scroll integer part for BG1
		_vdpData	SCYDN1,	0			;vert scroll fraction part for BG1
		_vdpData	ZMXIN1,	1			;horz co-ord increment integer part for BG1
		_vdpData	ZMXDN1,	0			;horz co-ord increment fraction part for BG1
		_vdpData	ZMYIN1,	1			;vert co-ord increment integer part for BG1
		_vdpData	ZMYDN1,	0			;vert co-ord increment fraction part for BG1
		
		_vdpData	ZMCTL,	0			;BG0 & BG1 - reduction turned off
		_vdpData	SCRCTL,	0			;BG0 & BG1 - horz and vert scrolling turned off
		_vdpData	LSTA0U,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA0L,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1U,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1L,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	VCSTAU,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1
		_vdpData	VCSTAL,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1

@Step4		_vdpData	RAMCTL,	$1300			;coeff table in vram,color mode 1,VRAM banks partioned into 4 banks

@Step5		_vdpData	WCTLA,	0			;no windows active
		_vdpData	WCTLB,	0			;no windows active
		_vdpData	WCTLC,	0			;no windows active
		_vdpData	WCTLD,	0			;no windows active
@Step6								;calc size of vram tables

@Step7
		_vdpData	CYCA0L,	$23ff			;access for 1 BG0 pnt,2 BG0 chars
		_vdpData	CYCA0U,	$ffff
		_vdpData	CYCA1L,	$ffff
		_vdpData	CYCA1U,	$ffff
		_vdpData	CYCB0L,	$66ff
		_vdpData	CYCB0U,	$ffff
		_vdpData	CYCB1L,	$77ff
		_vdpData	CYCB1U,	$ffff

@OtherSteps	_vdpData	CRAOFA,	0			;BG0 & BG1 & BG2 & BG3 - color offset
;		_vdpData	PRINA,	$0302			;BG0 & BG1 priority = 1
		_vdpData	PRINB,	$0504			;BG2 & BG3 priority = 1
		_vdpData	PRISA,	1			;sprite priority = 1
		_vdpData	CLOFEN,	0			;no color offsets for any screen

@Unknown	_vdpData	SPCTL,_SCL_SPCLMD		;?	

		_vdpDataEnd		

;======================================================================>
BGMode0ABCD

;NBG0 & NBG1 & NBG2 & NBG3
;320x224 resolution non-interlace
;16 color palette mode
;1 word PNT
;1x1 cell mode
;1x1 page per plane
;same plane is used for each plane in 2x2 map

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$40000
@CharAdr1	equ	$60000
@CharAdr2	equ	$50000
@CharAdr3	equ	$70000
@PNTAdr0	equ	$00000
@PNTAdr1	equ	$10000
@PNTAdr2	equ	$20000
@PNTAdr3	equ	$30000

	
		_vdpDataBegin

@Step1		_vdpData	TVMD,	$8001			;tv on,border black,non-interlace,320x224 resolution

@Step2		_vdpData	BGON,	$000f			;BG0 & BG1 on
		_vdpData	BKTAU,	$8000+@BackAdr>>17	;back screen on,bits 16-18 of Back Screen Table Address
		_vdpData	BKTAL,	(@BackAdr>>1)&$ffff	;bits 0-15 of Back Screen Table Address

@Step3		_vdpData	CHCTLA,	$0000			;BG0 & BG1 - Palette 16 colors,cell format,1x1 cell
		_vdpData	CHCTLB,	$0000			;BG2 & BG3 - Palette 16 colors,1x1 cell

		_vdpData	PNCN0,	(1<<15)+@CharAdr0/$8000	;BG0 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PNCN1,	(1<<15)+@CharAdr1/$8000	;BG1 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PNCN2,	(1<<15)+@CharAdr2/$8000	;BG2 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PNCN3,	(1<<15)+@CharAdr3/$8000	;BG3 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PLSZ,	0			;1x1 pages in plane

		_vdpData	MPOFN,	0			;BG0 & BG1 & BG2 & BG3 map offset register bits
		_vdpData	MPABN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes C & D to VDP2 VRAM 0
		_vdpData	MPABN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes C & D to VDP2 VRAM 0
		_vdpData	MPABN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes C & D to VDP2 VRAM 0
		_vdpData	MPABN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG3 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG4 - point planes C & D to VDP2 VRAM 0
		
		_vdpData	MZCTL,	0			;BG0 & BG1 & BG2 & BG3 - no mosaic process

		_vdpData	SCXIN0,	0			;horz scroll integer part for BG0
		_vdpData	SCXDN0,	0			;horz scroll fraction part for BG0
		_vdpData	SCYIN0,	0			;vert scroll integer part for BG0
		_vdpData	SCYDN0,	0			;vert scroll fraction part for BG0
		_vdpData	ZMXIN0,	1			;horz co-ord increment integer part for BG0
		_vdpData	ZMXDN0,	0			;horz co-ord increment fraction part for BG0
		_vdpData	ZMYIN0,	1			;vert co-ord increment integer part for BG0
		_vdpData	ZMYDN0,	0			;vert co-ord increment fraction part for BG0
		
		_vdpData	SCXIN1,	0			;horz scroll integer part for BG1
		_vdpData	SCXDN1,	0			;horz scroll fraction part for BG1
		_vdpData	SCYIN1,	0			;vert scroll integer part for BG1
		_vdpData	SCYDN1,	0			;vert scroll fraction part for BG1
		_vdpData	ZMXIN1,	1			;horz co-ord increment integer part for BG1
		_vdpData	ZMXDN1,	0			;horz co-ord increment fraction part for BG1
		_vdpData	ZMYIN1,	1			;vert co-ord increment integer part for BG1
		_vdpData	ZMYDN1,	0			;vert co-ord increment fraction part for BG1
		
		_vdpData	ZMCTL,	0			;BG0 & BG1 - reduction turned off
		_vdpData	SCRCTL,	0			;BG0 & BG1 - horz and vert scrolling turned off
		_vdpData	LSTA0U,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA0L,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1U,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1L,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	VCSTAU,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1
		_vdpData	VCSTAL,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1

@Step4		_vdpData	RAMCTL,	$1300			;coeff table in vram,color mode 1,VRAM banks partioned into 4 banks

@Step5		_vdpData	WCTLA,	0			;no windows active
		_vdpData	WCTLB,	0			;no windows active
		_vdpData	WCTLC,	0			;no windows active
		_vdpData	WCTLD,	0			;no windows active
@Step6								;calc size of vram tables

@Step7
		_vdpData	CYCA0L,	$01ff			;access for 1 BG0 pnt,2 BG0 chars
		_vdpData	CYCA0U,	$ffff
		_vdpData	CYCA1L,	$23ff
		_vdpData	CYCA1U,	$ffff
		_vdpData	CYCB0L,	$4fff
		_vdpData	CYCB0U,	$fff6
		_vdpData	CYCB1L,	$5fff
		_vdpData	CYCB1U,	$fff7

@OtherSteps	_vdpData	CRAOFA,	0			;BG0 & BG1 & BG2 & BG3 - color offset
		_vdpData	PRINA,	$0302			;BG0 & BG1 priority = 1
;		_vdpData	PRINA,	$0504			;BG2 & BG3 priority = 1
		_vdpData	PRISA,	1			;sprite priority = 1
		_vdpData	CLOFEN,	0			;no color offsets for any screen

@Unknown	_vdpData	SPCTL,_SCL_SPCLMD		;?	

		_vdpDataEnd		

;======================================================================>
BGMode1ABCD

;NBG0 & NBG1 & NBG2 & NBG3
;320x224 resolution non-interlace
;256 color palette mode
;1 word PNT
;1x1 cell mode
;1x1 page per plane
;same plane is used for each plane in 2x2 map

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$40000
@CharAdr1	equ	$60000
@CharAdr2	equ	$50000
@CharAdr3	equ	$70000
@PNTAdr0	equ	$00000
@PNTAdr1	equ	$10000
@PNTAdr2	equ	$20000
@PNTAdr3	equ	$30000

	
		_vdpDataBegin

@Step1		_vdpData	TVMD,	$8001			;tv on,border black,non-interlace,320x224 resolution

@Step2		_vdpData	BGON,	$000f			;BG0 & BG1 on
		_vdpData	BKTAU,	$8000+@BackAdr>>17	;back screen on,bits 16-18 of Back Screen Table Address
		_vdpData	BKTAL,	(@BackAdr>>1)&$ffff	;bits 0-15 of Back Screen Table Address

@Step3		_vdpData	CHCTLA,	$1010			;BG0 & BG1 - Palette 256 colors,cell format,1x1 cell
		_vdpData	CHCTLB,	$0022			;BG2 & BG3 - Palette 256 colors,1x1 cell

		_vdpData	PNCN0,	(1<<15)+@CharAdr0/$8000	;BG0 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PNCN1,	(1<<15)+@CharAdr1/$8000	;BG1 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PNCN2,	(1<<15)+@CharAdr2/$8000	;BG2 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PNCN3,	(1<<15)+@CharAdr3/$8000	;BG3 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		_vdpData	PLSZ,	0			;1x1 pages in plane

		_vdpData	MPOFN,	0			;BG0 & BG1 & BG2 & BG3 map offset register bits
		_vdpData	MPABN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes C & D to VDP2 VRAM 0
		_vdpData	MPABN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes C & D to VDP2 VRAM 0
		_vdpData	MPABN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes C & D to VDP2 VRAM 0
		_vdpData	MPABN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG3 - point planes A & B to VDP2 VRAM 0
		_vdpData	MPCDN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG4 - point planes C & D to VDP2 VRAM 0
		
		_vdpData	MZCTL,	0			;BG0 & BG1 & BG2 & BG3 - no mosaic process

		_vdpData	SCXIN0,	0			;horz scroll integer part for BG0
		_vdpData	SCXDN0,	0			;horz scroll fraction part for BG0
		_vdpData	SCYIN0,	0			;vert scroll integer part for BG0
		_vdpData	SCYDN0,	0			;vert scroll fraction part for BG0
		_vdpData	ZMXIN0,	1			;horz co-ord increment integer part for BG0
		_vdpData	ZMXDN0,	0			;horz co-ord increment fraction part for BG0
		_vdpData	ZMYIN0,	1			;vert co-ord increment integer part for BG0
		_vdpData	ZMYDN0,	0			;vert co-ord increment fraction part for BG0
		
		_vdpData	SCXIN1,	0			;horz scroll integer part for BG1
		_vdpData	SCXDN1,	0			;horz scroll fraction part for BG1
		_vdpData	SCYIN1,	0			;vert scroll integer part for BG1
		_vdpData	SCYDN1,	0			;vert scroll fraction part for BG1
		_vdpData	ZMXIN1,	1			;horz co-ord increment integer part for BG1
		_vdpData	ZMXDN1,	0			;horz co-ord increment fraction part for BG1
		_vdpData	ZMYIN1,	1			;vert co-ord increment integer part for BG1
		_vdpData	ZMYDN1,	0			;vert co-ord increment fraction part for BG1
		
		_vdpData	ZMCTL,	0			;BG0 & BG1 - reduction turned off
		_vdpData	SCRCTL,	0			;BG0 & BG1 - horz and vert scrolling turned off
		_vdpData	LSTA0U,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA0L,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1U,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1L,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	VCSTAU,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1
		_vdpData	VCSTAL,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1

@Step4		_vdpData	RAMCTL,	$1300			;coeff table in vram,color mode 1,VRAM banks partioned into 4 banks

@Step5		_vdpData	WCTLA,	0			;no windows active
		_vdpData	WCTLB,	0			;no windows active
		_vdpData	WCTLC,	0			;no windows active
		_vdpData	WCTLD,	0			;no windows active
@Step6								;calc size of vram tables

@Step7
		_vdpData	CYCA0L,	$01ff			;access for 1 BG0 pnt,2 BG0 chars
		_vdpData	CYCA0U,	$ffff
		_vdpData	CYCA1L,	$23ff
		_vdpData	CYCA1U,	$ffff
		_vdpData	CYCB0L,	$44ff
		_vdpData	CYCB0U,	$ff66
		_vdpData	CYCB1L,	$55ff
		_vdpData	CYCB1U,	$ff77

@OtherSteps	_vdpData	CRAOFA,	0			;BG0 & BG1 & BG2 & BG3 - color offset
		_vdpData	PRINA,	$0302			;BG0 & BG1 priority = 1
;		_vdpData	PRINA,	$0504			;BG2 & BG3 priority = 1
		_vdpData	PRISA,	1			;sprite priority = 1
		_vdpData	CLOFEN,	0			;no color offsets for any screen

@Unknown	_vdpData	SPCTL,_SCL_SPCLMD		;?	

		_vdpDataEnd		

;======================================================================>
BGMode8AB							;32768 RBG bit map mode

;NBG0 & NBG1
;320x224 resolution non-interlace
;32786 RGB mode
;Bit Map mode
;1x1 page per plane
;same plane is used for each plane in 2x2 map

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$00000
@CharAdr1	equ	$40000

		_vdpDataBegin

@Step1		_vdpData	TVMD,	$8001			;tv on,border black,non-interlace,320x224 resolution

@Step2		_vdpData	BGON,	$0003			;BG0 & BG1 on
		_vdpData	BKTAU,	$8000+@BackAdr>>17	;back screen on,bits 16-18 of Back Screen Table Address
		_vdpData	BKTAL,	(@BackAdr>>1)&$ffff	;bits 0-15 of Back Screen Table Address

@Step3		_vdpData	CHCTLA,	$3232			;BG0 & BG1 - 256 Palette color bit map mode
;		_vdpData	CHCTLB,	$0000			;BG2 & BG3 - unused

;		_vdpData	PNCN0,	(1<<15)+@CharAdr0/$8000	;BG0 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN1,	(1<<15)+@CharAdr1/$8000	;BG1 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN2,	(1<<15)+@CharAdr2/$8000	;BG2 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN3,	(1<<15)+@CharAdr3/$8000	;BG3 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		
		_vdpData	PLSZ,	0			;1x1 pages in plane

		_vdpData	MPOFN,	((@CharAdr1/$20000)<<4)+(@CharAdr0/$20000)	;BG0 & BG1 & BG2 & BG3 map offset register bits

;		_vdpData	MPABN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG3 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG4 - point planes C & D to VDP2 VRAM 0
		
		_vdpData	MZCTL,	0			;BG0 & BG1 & BG2 & BG3 - no mosaic process

		_vdpData	SCXIN0,	0			;horz scroll integer part for BG0
		_vdpData	SCXDN0,	0			;horz scroll fraction part for BG0
		_vdpData	SCYIN0,	0			;vert scroll integer part for BG0
		_vdpData	SCYDN0,	0			;vert scroll fraction part for BG0
		_vdpData	ZMXIN0,	1 			;horz co-ord increment integer part for BG0
		_vdpData	ZMXDN0,	0			;horz co-ord increment fraction part for BG0
		_vdpData	ZMYIN0,	1			;vert co-ord increment integer part for BG0
		_vdpData	ZMYDN0,	0			;vert co-ord increment fraction part for BG0
		
		_vdpData	SCXIN1,	0			;horz scroll integer part for BG1
		_vdpData	SCXDN1,	0			;horz scroll fraction part for BG1
		_vdpData	SCYIN1,	0			;vert scroll integer part for BG1
		_vdpData	SCYDN1,	0			;vert scroll fraction part for BG1
		_vdpData	ZMXIN1,	1			;horz co-ord increment integer part for BG1
		_vdpData	ZMXDN1,	0			;horz co-ord increment fraction part for BG1
		_vdpData	ZMYIN1,	1			;vert co-ord increment integer part for BG1
		_vdpData	ZMYDN1,	0			;vert co-ord increment fraction part for BG1
		
		_vdpData	ZMCTL,	0			;BG0 & BG1 - reduction turned off
		_vdpData	SCRCTL,	0			;BG0 & BG1 - horz and vert scrolling turned off
		_vdpData	LSTA0U,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA0L,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1U,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1L,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	VCSTAU,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1
		_vdpData	VCSTAL,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1

@Step4		_vdpData	RAMCTL,	$1300			;coeff table in vram,color mode 1,VRAM banks partioned into 4 banks

@Step5		_vdpData	WCTLA,	0			;no windows active
		_vdpData	WCTLB,	0			;no windows active
		_vdpData	WCTLC,	0			;no windows active
		_vdpData	WCTLD,	0			;no windows active
@Step6								;calc size of vram tables

@Step7
		_vdpData	CYCA0L,	$4444			;access for 1 BG0 pnt,2 BG0 chars
		_vdpData	CYCA0U,	$ffff
		_vdpData	CYCA1L,	$ffff
		_vdpData	CYCA1U,	$ffff
		_vdpData	CYCB0L,	$5555
		_vdpData	CYCB0U,	$ffff
		_vdpData	CYCB1L,	$ffff
		_vdpData	CYCB1U,	$ffff

@OtherSteps	_vdpData	CRAOFA,	0			;BG0 & BG1 & BG2 & BG3 - color offset
		_vdpData	PRINA,	$0302			;BG0 & BG1 priority = 1
;		_vdpData	PRINA,	$0504			;BG2 & BG3 priority = 1
		_vdpData	PRISA,	1			;sprite priority = 1
		_vdpData	CLOFEN,	0			;no color offsets for any screen

@Unknown	_vdpData	SPCTL,_SCL_SPCLMD		;?	

		_vdpDataEnd		
;======================================================================>
BGMode9A							;16million RBG bit map mode

;NBG0 & NBG1
;320x224 resolution non-interlace
;16,770,000 color RGB mode
;bit map mode
;1x1 page per plane
;same plane is used for each plane in 2x2 map

@BackAdr	equ	$20000-224*2
@CharAdr0	equ	$00000

		_vdpDataBegin

@Step1		_vdpData	TVMD,	$8001			;tv on,border black,non-interlace,320x224 resolution

@Step2		_vdpData	BGON,	$0003			;BG0 & BG1 on
		_vdpData	BKTAU,	$8000+@BackAdr>>17	;back screen on,bits 16-18 of Back Screen Table Address
		_vdpData	BKTAL,	(@BackAdr>>1)&$ffff	;bits 0-15 of Back Screen Table Address

@Step3		_vdpData	CHCTLA,	$0042			;BG0 16,770,000 RGB color bit map mode
;		_vdpData	CHCTLB,	$0000			;BG2 & BG3 - unused

;		_vdpData	PNCN0,	(1<<15)+@CharAdr0/$8000	;BG0 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN1,	(1<<15)+@CharAdr1/$8000	;BG1 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN2,	(1<<15)+@CharAdr2/$8000	;BG2 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN3,	(1<<15)+@CharAdr3/$8000	;BG3 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		
		_vdpData	PLSZ,	0			;1x1 pages in plane

		_vdpData	MPOFN,	(@CharAdr0/$20000)	;BG0 & BG1 & BG2 & BG3 map offset register bits

;		_vdpData	MPABN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG3 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG4 - point planes C & D to VDP2 VRAM 0
		
		_vdpData	MZCTL,	0			;BG0 & BG1 & BG2 & BG3 - no mosaic process

		_vdpData	SCXIN0,	0			;horz scroll integer part for BG0
		_vdpData	SCXDN0,	0			;horz scroll fraction part for BG0
		_vdpData	SCYIN0,	0			;vert scroll integer part for BG0
		_vdpData	SCYDN0,	0			;vert scroll fraction part for BG0
		_vdpData	ZMXIN0,	1			;horz co-ord increment integer part for BG0
		_vdpData	ZMXDN0,	0			;horz co-ord increment fraction part for BG0
		_vdpData	ZMYIN0,	1			;vert co-ord increment integer part for BG0
		_vdpData	ZMYDN0,	0			;vert co-ord increment fraction part for BG0
		
		_vdpData	SCXIN1,	0			;horz scroll integer part for BG1
		_vdpData	SCXDN1,	0			;horz scroll fraction part for BG1
		_vdpData	SCYIN1,	0			;vert scroll integer part for BG1
		_vdpData	SCYDN1,	0			;vert scroll fraction part for BG1
		_vdpData	ZMXIN1,	1			;horz co-ord increment integer part for BG1
		_vdpData	ZMXDN1,	0			;horz co-ord increment fraction part for BG1
		_vdpData	ZMYIN1,	1			;vert co-ord increment integer part for BG1
		_vdpData	ZMYDN1,	0			;vert co-ord increment fraction part for BG1
		
		_vdpData	ZMCTL,	0			;BG0 & BG1 - reduction turned off
		_vdpData	SCRCTL,	0			;BG0 & BG1 - horz and vert scrolling turned off
		_vdpData	LSTA0U,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA0L,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1U,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1L,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	VCSTAU,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1
		_vdpData	VCSTAL,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1

@Step4		_vdpData	RAMCTL,	$1300			;coeff table in vram,color mode 1,VRAM banks partioned into 4 banks

@Step5		_vdpData	WCTLA,	0			;no windows active
		_vdpData	WCTLB,	0			;no windows active
		_vdpData	WCTLC,	0			;no windows active
		_vdpData	WCTLD,	0			;no windows active
@Step6								;calc size of vram tables

@Step7
		_vdpData	CYCA0L,	$4444			;access for 1 BG0 pnt,2 BG0 chars
		_vdpData	CYCA0U,	$4444
		_vdpData	CYCA1L,	$4444
		_vdpData	CYCA1U,	$4444
		_vdpData	CYCB0L,	$4444
		_vdpData	CYCB0U,	$4444
		_vdpData	CYCB1L,	$4444
		_vdpData	CYCB1U,	$4444

@OtherSteps	_vdpData	CRAOFA,	0			;BG0 & BG1 & BG2 & BG3 - color offset
		_vdpData	PRINA,	$0302			;BG0 & BG1 priority = 1
;		_vdpData	PRINA,	$0504			;BG2 & BG3 priority = 1
		_vdpData	PRISA,	$ffff			;sprite priority = 1
		_vdpData	PRISB,	$ffff			;sprite priority = 1
		_vdpData	PRISC,	$ffff			;sprite priority = 1
		_vdpData	PRISD,	$ffff			;sprite priority = 1
		_vdpData	CLOFEN,	0			;no color offsets for any screen

@Unknown	_vdpData	SPCTL,_SCL_SPCLMD		;?	

		_vdpDataEnd		
;======================================================================>
BGMode7AB							;Palette 2048 color bit map mode

;NBG0 & NBG1
;320x224 resolution non-interlace
;2048 color palette mode
;bit map mode
;1x1 page per plane
;same plane is used for each plane in 2x2 map

@BackAdr	equ	$40000-224*2
@CharAdr0	equ	$00000
@CharAdr1	equ	$40000

		_vdpDataBegin

@Step1		_vdpData	TVMD,	$8001			;tv on,border black,non-interlace,320x224 resolution

@Step2		_vdpData	BGON,	$0003			;BG0 & BG1 on
		_vdpData	BKTAU,	$8000+@BackAdr>>17	;back screen on,bits 16-18 of Back Screen Table Address
		_vdpData	BKTAL,	(@BackAdr>>1)&$ffff	;bits 0-15 of Back Screen Table Address

@Step3		_vdpData	CHCTLA,	$2222			;BG0 & BG1 - 2048 Palette color bit map mode
;		_vdpData	CHCTLB,	$0000			;BG2 & BG3 - unused

;		_vdpData	PNCN0,	(1<<15)+@CharAdr0/$8000	;BG0 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN1,	(1<<15)+@CharAdr1/$8000	;BG1 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN2,	(1<<15)+@CharAdr2/$8000	;BG2 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN3,	(1<<15)+@CharAdr3/$8000	;BG3 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		
		_vdpData	PLSZ,	0			;1x1 pages in plane

		_vdpData	MPOFN,	((@CharAdr1/$20000)<<4)+(@CharAdr0/$20000)	;BG0 & BG1 & BG2 & BG3 map offset register bits

;		_vdpData	MPABN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG3 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG4 - point planes C & D to VDP2 VRAM 0
		
		_vdpData	MZCTL,	0			;BG0 & BG1 & BG2 & BG3 - no mosaic process

		_vdpData	SCXIN0,	0			;horz scroll integer part for BG0
		_vdpData	SCXDN0,	0			;horz scroll fraction part for BG0
		_vdpData	SCYIN0,	0			;vert scroll integer part for BG0
		_vdpData	SCYDN0,	0			;vert scroll fraction part for BG0
		_vdpData	ZMXIN0,	1			;horz co-ord increment integer part for BG0
		_vdpData	ZMXDN0,	0			;horz co-ord increment fraction part for BG0
		_vdpData	ZMYIN0,	1			;vert co-ord increment integer part for BG0
		_vdpData	ZMYDN0,	0			;vert co-ord increment fraction part for BG0
		
		_vdpData	SCXIN1,	0			;horz scroll integer part for BG1
		_vdpData	SCXDN1,	0			;horz scroll fraction part for BG1
		_vdpData	SCYIN1,	0			;vert scroll integer part for BG1
		_vdpData	SCYDN1,	0			;vert scroll fraction part for BG1
		_vdpData	ZMXIN1,	1			;horz co-ord increment integer part for BG1
		_vdpData	ZMXDN1,	0			;horz co-ord increment fraction part for BG1
		_vdpData	ZMYIN1,	1			;vert co-ord increment integer part for BG1
		_vdpData	ZMYDN1,	0			;vert co-ord increment fraction part for BG1
		
		_vdpData	ZMCTL,	0			;BG0 & BG1 - reduction turned off
		_vdpData	SCRCTL,	0			;BG0 & BG1 - horz and vert scrolling turned off
		_vdpData	LSTA0U,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA0L,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1U,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1L,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	VCSTAU,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1
		_vdpData	VCSTAL,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1

@Step4		_vdpData	RAMCTL,	$1300			;coeff table in vram,color mode 1,VRAM banks partioned into 4 banks

@Step5		_vdpData	WCTLA,	0			;no windows active
		_vdpData	WCTLB,	0			;no windows active
		_vdpData	WCTLC,	0			;no windows active
		_vdpData	WCTLD,	0			;no windows active
@Step6								;calc size of vram tables

@Step7
		_vdpData	CYCA0L,	$4444			;access for 1 BG0 pnt,2 BG0 chars
		_vdpData	CYCA0U,	$ffff
		_vdpData	CYCA1L,	$ffff
		_vdpData	CYCA1U,	$ffff
		_vdpData	CYCB0L,	$5555
		_vdpData	CYCB0U,	$ffff
		_vdpData	CYCB1L,	$ffff
		_vdpData	CYCB1U,	$ffff

@OtherSteps	_vdpData	CRAOFA,	0			;BG0 & BG1 & BG2 & BG3 - color offset
		_vdpData	PRINA,	$0302			;BG0 & BG1 priority = 1
;		_vdpData	PRINB,	$0504			;BG2 & BG3 priority = 1
		_vdpData	PRISA,	1			;sprite priority = 1
		_vdpData	CLOFEN,	0			;no color offsets for any screen

@Unknown	_vdpData	SPCTL,_SCL_SPCLMD		;?	

		_vdpDataEnd		
;======================================================================>
BGMode6AB							;Palette 256 color bit map mode

;NBG0 & NBG1
;320x224 resolution non-interlace
;256 color palette mode
;bit map mode
;1x1 page per plane
;same plane is used for each plane in 2x2 map

@BackAdr	equ	$40000-224*2
@CharAdr0	equ	$00000
@CharAdr1	equ	$40000

		_vdpDataBegin

@Step1		_vdpData	TVMD,	$8001			;tv on,border black,non-interlace,320x224 resolution

@Step2		_vdpData	BGON,	$0003			;BG0 & BG1 on
		_vdpData	BKTAU,	$8000+@BackAdr>>17	;back screen on,bits 16-18 of Back Screen Table Address
		_vdpData	BKTAL,	(@BackAdr>>1)&$ffff	;bits 0-15 of Back Screen Table Address

@Step3		_vdpData	CHCTLA,	$1212			;BG0 & BG1 - 256 Palette color bit map mode
;		_vdpData	CHCTLB,	$0000			;BG2 & BG3 - unused

;		_vdpData	PNCN0,	(1<<15)+@CharAdr0/$8000	;BG0 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN1,	(1<<15)+@CharAdr1/$8000	;BG1 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN2,	(1<<15)+@CharAdr2/$8000	;BG2 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN3,	(1<<15)+@CharAdr3/$8000	;BG3 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		
		_vdpData	PLSZ,	0			;1x1 pages in plane

		_vdpData	MPOFN,	((@CharAdr1/$20000)<<4)+(@CharAdr0/$20000)	;BG0 & BG1 & BG2 & BG3 map offset register bits

;		_vdpData	MPABN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG3 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG4 - point planes C & D to VDP2 VRAM 0
		
		_vdpData	MZCTL,	0			;BG0 & BG1 & BG2 & BG3 - no mosaic process

		_vdpData	SCXIN0,	0			;horz scroll integer part for BG0
		_vdpData	SCXDN0,	0			;horz scroll fraction part for BG0
		_vdpData	SCYIN0,	0			;vert scroll integer part for BG0
		_vdpData	SCYDN0,	0			;vert scroll fraction part for BG0
		_vdpData	ZMXIN0,	1			;horz co-ord increment integer part for BG0
		_vdpData	ZMXDN0,	0			;horz co-ord increment fraction part for BG0
		_vdpData	ZMYIN0,	1			;vert co-ord increment integer part for BG0
		_vdpData	ZMYDN0,	0			;vert co-ord increment fraction part for BG0
		
		_vdpData	SCXIN1,	0			;horz scroll integer part for BG1
		_vdpData	SCXDN1,	0			;horz scroll fraction part for BG1
		_vdpData	SCYIN1,	0			;vert scroll integer part for BG1
		_vdpData	SCYDN1,	0			;vert scroll fraction part for BG1
		_vdpData	ZMXIN1,	1			;horz co-ord increment integer part for BG1
		_vdpData	ZMXDN1,	0			;horz co-ord increment fraction part for BG1
		_vdpData	ZMYIN1,	1			;vert co-ord increment integer part for BG1
		_vdpData	ZMYDN1,	0			;vert co-ord increment fraction part for BG1
		
		_vdpData	ZMCTL,	0			;BG0 & BG1 - reduction turned off
		_vdpData	SCRCTL,	0			;BG0 & BG1 - horz and vert scrolling turned off
		_vdpData	LSTA0U,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA0L,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1U,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1L,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	VCSTAU,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1
		_vdpData	VCSTAL,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1

@Step4		_vdpData	RAMCTL,	$1300			;coeff table in vram,color mode 1,VRAM banks partioned into 4 banks

@Step5		_vdpData	WCTLA,	0			;no windows active
		_vdpData	WCTLB,	0			;no windows active
		_vdpData	WCTLC,	0			;no windows active
		_vdpData	WCTLD,	0			;no windows active
@Step6								;calc size of vram tables

@Step7
		_vdpData	CYCA0L,	$44EE			;access for 1 BG0 pnt,2 BG0 chars
		_vdpData	CYCA0U,	$EEEE
		_vdpData	CYCA1L,	$EEEE
		_vdpData	CYCA1U,	$EEEE
		_vdpData	CYCB0L,	$55EE
		_vdpData	CYCB0U,	$EEEE
		_vdpData	CYCB1L,	$EEEE
		_vdpData	CYCB1U,	$EEEE

@OtherSteps	_vdpData	CRAOFA,	0			;BG0 & BG1 & BG2 & BG3 - color offset
		_vdpData	PRINA,	$0302			;BG0 & BG1 priority = 1
;		_vdpData	PRINB,	$0504			;BG2 & BG3 priority = 1
		_vdpData	PRISA,	$ffff			;sprite priority = 1
		_vdpData	PRISB,	$ffff			;sprite priority = 1
		_vdpData	PRISC,	$ffff			;sprite priority = 1
		_vdpData	PRISD,	$ffff			;sprite priority = 1
		_vdpData	CLOFEN,	0			;no color offsets for any screen

@Unknown	_vdpData	SPCTL,_SCL_SPCLMD		;?	

		_vdpDataEnd		
;======================================================================>
BGMode5AB							;Palette 2048 color bit map mode

;NBG0 & NBG1
;320x224 resolution non-interlace
;16 color palette mode
;bit map mode
;1x1 page per plane
;same plane is used for each plane in 2x2 map

@BackAdr	equ	$40000-224*2
@CharAdr0	equ	$00000
@CharAdr1	equ	$40000

		_vdpDataBegin

@Step1		_vdpData	TVMD,	$8001			;tv on,border black,non-interlace,320x224 resolution

@Step2		_vdpData	BGON,	$0003			;BG0 & BG1 on
		_vdpData	BKTAU,	$8000+@BackAdr>>17	;back screen on,bits 16-18 of Back Screen Table Address
		_vdpData	BKTAL,	(@BackAdr>>1)&$ffff	;bits 0-15 of Back Screen Table Address

@Step3		_vdpData	CHCTLA,	$0202			;BG0 & BG1 - 16 Palette color bit map mode
;		_vdpData	CHCTLB,	$0000			;BG2 & BG3 - unused

;		_vdpData	PNCN0,	(1<<15)+@CharAdr0/$8000	;BG0 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN1,	(1<<15)+@CharAdr1/$8000	;BG1 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN2,	(1<<15)+@CharAdr2/$8000	;BG2 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
;		_vdpData	PNCN3,	(1<<15)+@CharAdr3/$8000	;BG3 - 1 word pnt data,no sp or cc,0 for extra pal bits,chars base
		
		_vdpData	PLSZ,	0			;1x1 pages in plane

		_vdpData	MPOFN,	((@CharAdr1/$20000)<<4)+(@CharAdr0/$20000)	;BG0 & BG1 & BG2 & BG3 map offset register bits

;		_vdpData	MPABN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN0,	(@PNTAdr0>>5)&$ff00+(@PNTAdr0>>13)&$00ff	;BG0 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN1,	(@PNTAdr1>>5)&$ff00+(@PNTAdr1>>13)&$00ff	;BG1 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN2,	(@PNTAdr2>>5)&$ff00+(@PNTAdr2>>13)&$00ff	;BG2 - point planes C & D to VDP2 VRAM 0
;		_vdpData	MPABN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG3 - point planes A & B to VDP2 VRAM 0
;		_vdpData	MPCDN3,	(@PNTAdr3>>5)&$ff00+(@PNTAdr3>>13)&$00ff	;BG4 - point planes C & D to VDP2 VRAM 0
		
		_vdpData	MZCTL,	0			;BG0 & BG1 & BG2 & BG3 - no mosaic process

		_vdpData	SCXIN0,	0			;horz scroll integer part for BG0
		_vdpData	SCXDN0,	0			;horz scroll fraction part for BG0
		_vdpData	SCYIN0,	0			;vert scroll integer part for BG0
		_vdpData	SCYDN0,	0			;vert scroll fraction part for BG0
		_vdpData	ZMXIN0,	1			;horz co-ord increment integer part for BG0
		_vdpData	ZMXDN0,	0			;horz co-ord increment fraction part for BG0
		_vdpData	ZMYIN0,	1			;vert co-ord increment integer part for BG0
		_vdpData	ZMYDN0,	0			;vert co-ord increment fraction part for BG0
		
		_vdpData	SCXIN1,	0			;horz scroll integer part for BG1
		_vdpData	SCXDN1,	0			;horz scroll fraction part for BG1
		_vdpData	SCYIN1,	0			;vert scroll integer part for BG1
		_vdpData	SCYDN1,	0			;vert scroll fraction part for BG1
		_vdpData	ZMXIN1,	1			;horz co-ord increment integer part for BG1
		_vdpData	ZMXDN1,	0			;horz co-ord increment fraction part for BG1
		_vdpData	ZMYIN1,	1			;vert co-ord increment integer part for BG1
		_vdpData	ZMYDN1,	0			;vert co-ord increment fraction part for BG1
		
		_vdpData	ZMCTL,	0			;BG0 & BG1 - reduction turned off
		_vdpData	SCRCTL,	0			;BG0 & BG1 - horz and vert scrolling turned off
		_vdpData	LSTA0U,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA0L,	0			;BG0 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1U,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	LSTA1L,	0			;BG1 - unused as no line and vertical scrolling for BG0
		_vdpData	VCSTAU,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1
		_vdpData	VCSTAL,	0			;BG0 & BG1 - unused as no vert cell scrolling for BG0 and BG1

@Step4		_vdpData	RAMCTL,	$1300			;coeff table in vram,color mode 1,VRAM banks partioned into 4 banks

@Step5		_vdpData	WCTLA,	0			;no windows active
		_vdpData	WCTLB,	0			;no windows active
		_vdpData	WCTLC,	0			;no windows active
		_vdpData	WCTLD,	0			;no windows active
@Step6								;calc size of vram tables

@Step7
		_vdpData	CYCA0L,	$4fff			;access for 1 BG0 pnt,2 BG0 chars
		_vdpData	CYCA0U,	$ffff
		_vdpData	CYCA1L,	$ffff
		_vdpData	CYCA1U,	$ffff
		_vdpData	CYCB0L,	$5fff
		_vdpData	CYCB0U,	$ffff
		_vdpData	CYCB1L,	$ffff
		_vdpData	CYCB1U,	$ffff

@OtherSteps	_vdpData	CRAOFA,	0			;BG0 & BG1 & BG2 & BG3 - color offset
		_vdpData	PRINA,	$0302			;BG0 & BG1 priority = 1
;		_vdpData	PRINB,	$0504			;BG2 & BG3 priority = 1
		_vdpData	PRISA,	1			;sprite priority = 1
		_vdpData	CLOFEN,	0			;no color offsets for any BG

@Unknown	_vdpData	SPCTL,_SCL_SPCLMD		;?	

		_vdpDataEnd		




;======================================================================>

RGBmode1A							;Palette 2048 color bit map mode

@BackAdr	equ	_VDP2_VRAM+$20000-224*2
@CharAdr0	equ	_VDP2_VRAM+$40000
@PNTAdr0	equ	_VDP2_VRAM+$00000

		_vdpDataBegin

@Step1		_vdpData	TVMD,	$8001			;tv on,border black,non-interlace,320x224 resolution

		_vdpData	PRIR,$0005   	;RBG0 Priority
		_vdpData	CYCA0L,$440F	;VRAM Cycle Pattern (Bank A0) Lower
		_vdpData	CYCA0U,$FFFF	;VRAM Cycle Pattern (Bank A0) Upper
		_vdpData	CYCA1L,$FFFF	;VRAM Cycle Pattern (Bank A1) Lower
		_vdpData	CYCA1U,$FFFF	;VRAM Cycle Pattern (Bank A1) Upper
	
		_vdpData	CYCB0L,$FFFF	;VRAM Cycle Pattern (Bank B0) Lower
		_vdpData	CYCB0U,$FFFF	;VRAM Cycle Pattern (Bank B0) Upper
		_vdpData	CYCB1L,$FFFF	;VRAM Cycle Pattern (Bank B1) Lower
		_vdpData	CYCB1U,$FFFF	;VRAM Cycle Pattern (Bank B1) Upper
	
	
		_vdpData	RAMCTL,$23C8	;
		_vdpData	BGON,$0010	;RBG0 on
		_vdpData	CHCTLB,$1200 	;
		_vdpData	PLSZ,$0000 	;1 H Page x 1 V Page (NBG0)
		_vdpData	RPRCTL,$0000 	;Rot Read ctrl
	
		_vdpData	KTCTL,$0000	;
		_vdpData	RPTAU,$12F0	;Rotation Param Table Address
		_vdpData	RPTAL,$F800	;Rotation Param Table Address
		_vdpData	CRAOFB,$0000 	;Color Offset
		_vdpData	MPOFR,$0000 	;Map Offset
		_vdpData	RPMD,$0000 	;Rot ctrl
	
		_vdpData	MPABRA,$1010	;Rotation Pane Address
		_vdpData	MPCDRA,$1010	;Rotation Pane Address
		_vdpData	MPEFRA,$1010	;Rotation Pane Address
		_vdpData	MPGHRA,$1010	;Rotation Pane Address
		_vdpData	MPIJRA,$1010	;Rotation Pane Address
		_vdpData	MPKLRA,$1010	;Rotation Pane Address
		_vdpData	MPMNRA,$1010	;Rotation Pane Address
		_vdpData	MPOPRA,$1010	;Rotation Pane Address
		_vdpData	PNCR,$C00C 	;1 word size,has flip,no special priority
	
		_vdpData	PRISA,	$ffff			;sprite priority = 1
		_vdpData	PRISB,	$ffff			;sprite priority = 1
		_vdpData	PRISC,	$ffff			;sprite priority = 1
		_vdpData	PRISD,	$ffff			;sprite priority = 1
	

@Unknown	_vdpData	SPCTL,_SCL_SPCLMD		;?	

		_vdpDataEnd		
	
		align		4
RotParmTable
		DC.L	   	$00000000	;00 X 1st
		DC.L		$00000000	;04 Y 1st
		DC.L		$00000000	;08 Z 1st
		DC.L		$00000000	;0C X Inc Per Line
		DC.L		$00010000	;10 Y Inc Per Line
		DC.L		$00010000	;14 X Inc Per Dot
		DC.L		$00000000	;18 Y Inc Per Dot
		DC.L		$00010000	;1C Matrix Parm A
		DC.L		$00000000	;20 Matrix Parm B
		DC.L		$00000000	;24 Matrix Parm C
		DC.L		$00000000	;28 Matrix Parm D
		DC.L		$00010000	;2C Matrix Parm E
		DC.L		$00000000	;30 Matrix Parm F
		DC.W		$00AA		;34 Px (View Point X)
		DC.W		$0070		;36 Py (View Point Y)
		DC.W		$003C		;38 Pz (View Point Z)
		DC.W		$0000		;3A Not Used
		DC.W		$00AA		;3C Cx (Center X Coord)
		DC.W		$0070		;3E Cx (Center Y Coord)
		DC.W		$0000		;40 Cx (Center Z Coord)
		DC.W		$0000		;42 Not Used
		DC.L		$00000000	;44 Mx (Shift in X Direction)
		DC.L		$00000000	;48 My (Shift in Y Direction)
		DC.L		$00010000	;4C kx (Scale Coefficient in X Direction)
		DC.L		$00010000	;50 ky (Scale Coefficient in Y Direction)
		DC.L		$00000000	;54 KAst (Table Start Address when using Coefficients)
		DC.L		$00010000	;58 KAst (Address inc per each line when using Coefficients)
		DC.L		$00010000	;5C KAx  (Address inc per each dot when using Coefficients)



	align 4


	xdef	init_vdp2
init_vdp2

	mov.l	#_VDP2_REGS,r1
	mov.l	#(VDP2regsend-VDP2regs)/4,r2
	mov.l	#VDP2regs,r3
@initloop:
	mov.w	@r3+,r4
	extu.w	r4,r4
	add	r1,r4
	mov.w	@r3+,r0
	mov.w	r0,@r4

	dt	r2
	bf	@initloop

	rts
	nop


	lits


	align 4


; secSysRegAddr.
; Addresses of VDP2 system registers
 
VDP2regs
	dc.w	$0000,$8001	; TVMD	   TV Screen mode 
	dc.w	$0002,$0000	; EXTEN	   Extern signal enable reg 
	dc.w	$0004,$0000	; TVSTAT   Screen status 
	dc.w	$0006,$0000	; VRSIZE   VRAM size 
	dc.w	$0008,$0000	; HCNT	   H-Counter 
	dc.w	$000A,$0000	; VCNT	   V-Counter 
	dc.w	$000E,$0000	; RAMCTL   Ram control 
	dc.w	$0010,$0000	; CYCA0L   Bank A0 VRAM cycle pattern 
	dc.w	$0012,$0000	; CYCA0U   Bank A0 VRAM cycle pattern 
	dc.w	$0014,$0000	; CYCA1L   Bank A1 VRAM cycle pattern 
	dc.w	$0016,$0000	; CYCA1U   Bank A1 VRAM cycle pattern 
	dc.w	$0018,$0000	; CYCB0L   Bank B0 VRAM cycle pattern 
	dc.w	$001A,$0000	; CYCB0U   Bank B0 VRAM cycle pattern 
	dc.w	$001C,$0000	; CYCB1L   Bank B1 VRAM cycle pattern 
	dc.w	$001E,$0000	; CYCB1U   Bank B1 VRAM cycle pattern 
	dc.w	$0020,$0000	; BGON	   Screen display enable 
	dc.w	$0022,$0000	; MZCTL	   Mosaic control 
	dc.w	$0024,$0000	; SFSEL	   Special fnc code select 
	dc.w	$0026,$0000	; SFCODE   Special fnc code 
	dc.w	$0028,$0000	; CHCTLA   Character control (NBG0, NBG1) 
	dc.w	$002A,$0000	; CHCTLB   Character control (NBG2, NBG3, RBG0) 
	dc.w	$002C,$0000	; BMPNA	   Bitmap palette number (NBG0, NBG1) 
	dc.w	$002E,$0000	; BMPNB	   Bitmap palette number (RBG0) 
	dc.w	$0030,$0000	; PNCN0	   Pattern name ctl (NBG0) 
	dc.w	$0032,$0000	; PNCN1	   Pattern name ctl (NBG1) 
	dc.w	$0034,$0000	; PNCN2	   Pattern name ctl (NBG2) 
	dc.w	$0036,$0000	; PNCN3	   Pattern name ctl (NBG3) 
	dc.w	$0038,$0000	; PNCR	   Pattern name ctl (RBG0) 
	dc.w	$003A,$0000	; PLSZ	   Plane size ctl 
	dc.w	$003C,$0000	; MPOFN	   Map offset (NBG0-NBG3) 
	dc.w	$003E,$0000	; MPOFR	   Map offset (Rot param A,B) 
	dc.w	$0040,$0000	; MPABN0   pat-name offset reg (NBG0,Plane AB) 
	dc.w	$0042,$0000	; MPCDN0   pat-name offset reg (NBG0,Plane CD) 
	dc.w	$0044,$0000	; MPABN1   pat-name offset reg (NBG1,Plane AB) 
	dc.w	$0046,$0000	; MPCDN1   pat-name offset reg (NBG1,Plane CD) 
	dc.w	$0048,$0000	; MPABN2   pat-name offset reg (NBG2,Plane AB) 
	dc.w	$004A,$0000	; MPCDN2   pat-name offset reg (NBG2,Plane CD) 
	dc.w	$004C,$0000	; MPABN3   pat-name offset reg (NBG3,Plane AB) 
	dc.w	$004E,$0000	; MPCDN3   pat-name offset reg (NBG3,Plane CD) 
	dc.w	$0050,$0000	; MPABRA   pat-name offset reg (rot parmA)
	dc.w	$0052,$0000	; MPCDRA   pat-name offset reg (rot parmA)
	dc.w	$0054,$0000	; MPEFRA   pat-name offset reg (rot parmA)
	dc.w	$0056,$0000	; MPGHRA   pat-name offset reg (rot parmA)
	dc.w	$0058,$0000	; MPIJRA   pat-name offset reg (rot parmA)
	dc.w	$005A,$0000	; MPKLRA   pat-name offset reg (rot parmA)
	dc.w	$005C,$0000	; MPMNRA   pat-name offset reg (rot parmA)
	dc.w	$005E,$0000	; MPOPRA  
	dc.w	$0060,$0000	; MPABRB   pat-name offset reg (rot parmB)
	dc.w	$0062,$0000	; MPCDRB   pat-name offset reg (rot parmB)
	dc.w	$0064,$0000	; MPEFRB   pat-name offset reg (rot parmB)
	dc.w	$0066,$0000	; MPGHRB   pat-name offset reg (rot parmB)
	dc.w	$0068,$0000	; MPIJRB   pat-name offset reg (rot parmB)
	dc.w	$006A,$0000	; MPKLRB   pat-name offset reg (rot parmB)
	dc.w	$006C,$0000	; MPMNRB   pat-name offset reg (rot parmB)
	dc.w	$006E,$0000	; MPOPRB   pat-name offset reg (rot parmB)
	dc.w	$0070,$0000	; SCXIN0   screen scroll H int val (NBG0) 
	dc.w	$0072,$0000	; SCXDN0   screen scroll H dec val (NBG0) 
	dc.w	$0074,$0000	; SCYIN0   screen scroll V int val (NBG0) 
	dc.w	$0076,$0000	; SCYDN0   screen scroll V dec val (NBG0) 
	dc.w	$0078,$0000	; ZMXIN0   coord incr H int val (NBG0) 
	dc.w	$007A,$0000	; ZMXDN0   coord incr H dec val (NBG0) 
	dc.w	$007C,$0000	; ZMYIN0   coord incr V int val (NBG0) 
	dc.w	$007E,$0000	; ZMYDN0   coord incr V dec val (NBG0) 
	dc.w	$0080,$0000	; SCXIN1   screen scroll H int val (NBG1) 
	dc.w	$0082,$0000	; SCXDN1   screen scroll H dec val (NBG1) 
	dc.w	$0084,$0000	; SCYIN1   screen scroll V int val (NBG1) 
	dc.w	$0086,$0000	; SCYDN1   screen scroll V dec val (NBG1) 
	dc.w	$0088,$0000	; ZMXIN1   coord incr H int val (NBG1) 
	dc.w	$008A,$0000	; ZMXDN1   coord incr H dec val (NBG1) 
	dc.w	$008C,$0000	; ZMYIN1   coord incr V int val (NBG1) 
	dc.w	$008E,$0000	; ZMYDN1   coord incr V dec val (NBG1) 
	dc.w	$0090,$0000	; SCXN2	   screen scroll horiz reg(NBG2) 
	dc.w	$0092,$0000	; SCYN2	   screen scroll vert reg(NBG2) 
	dc.w	$0094,$0000	; SCXN3	   screen scroll horiz reg(NBG3) 
	dc.w	$0096,$0000	; SCYN3	   screen scroll vert reg(NBG3) 
	dc.w	$0098,$0000	; ZMCTL	   Reduction enable register 
	dc.w	$009A,$0000	; SCRCTL   line scroll ctl reg 
	dc.w	$009C,$0000	; VCSTAU   vert cell scroll table reg 
	dc.w	$009E,$0000	; VCSTAL   vert cell scroll table reg 
	dc.w	$00A0,$0000	; LSTA0U   line scroll table reg(NBG0) 
	dc.w	$00A2,$0000	; LSTA0L   line scroll table reg(NBG0) 
	dc.w	$00A4,$0000	; LSTA1U   line scroll table reg(NBG1) 
	dc.w	$00A6,$0000	; LSTA1L   line scroll table reg(NBG1) 
	dc.w	$00A8,$0000	; LCTAU	   line color screen table reg 
	dc.w	$00AA,$0000	; LCTAL	   line color screen table reg 
	dc.w	$00AC,$0000	; BKTAU	   back screen table addr reg 
	dc.w	$00AE,$0000	; BKTAL	   back screen table addr reg 
	dc.w	$00B0,$0000	; RPMD	   rotation mode ctl reg 
	dc.w	$00B2,$0000	; RPRCTL   rotation param read ctl reg 
	dc.w	$00B4,$0000	; KTCTL	   rotation K table ctl reg 
	dc.w	$00B6,$0000	; KTAOF	   rotation K table offset reg 
	dc.w	$00B8,$0000	; OVPNRA   screen-over process pn reg 
	dc.w	$00BA,$0000	; OVPNRB   screen-over process pn reg 
	dc.w	$00BC,$0000	; RPTAU	   rotation param table reg 
	dc.w	$00BE,$0000	; RPTAL	   rotation param table reg 
	dc.w	$00C0,$0000	; WPSX0	   normal window (W0) horiz start 
	dc.w	$00C2,$0000	; WPSY0	   normal window (W0) vert start 
	dc.w	$00C4,$0000	; WPEX0	   normal window (W0) horiz end 
	dc.w	$00C6,$0000	; WPEY0	   normal window (W0) vert end 
	dc.w	$00C8,$0000	; WPSX1	   normal window (W1) horiz start 
	dc.w	$00CA,$0000	; WPSY1	   normal window (W1) vert start 
	dc.w	$00CC,$0000	; WPEX1	   normal window (W1) horiz end 
	dc.w	$00CE,$0000	; WPEY1	   normal window (W1) vert end 
	dc.w	$00D0,$0000	; WCTLA	   window control reg 
	dc.w	$00D2,$0000	; WCTLB	   window control reg 
	dc.w	$00D4,$0000	; WCTLC	   window control reg 
	dc.w	$00D6,$0000	; WCTLD	   window control reg 
	dc.w	$00D8,$0000	; LWTA0U   line window table reg (W0) 
	dc.w	$00DA,$0000	; LWTA0L   line window table reg (W0) 
	dc.w	$00DC,$0000	; LWTA1U   line window table reg (W1) 
	dc.w	$00DE,$0000	; LWTA1L   line window table reg (W1) 
	dc.w	$00E0,$3020	; SPCTL	   sprite control reg 
	dc.w	$00E2,$0000	; SDCTL	   shadow control reg 
	dc.w	$00E4,$0000	; CRAOFA   color RAM offset reg 
	dc.w	$00E6,$0000	; CRAOFB   color RAM offset reg 
	dc.w	$00E8,$0000	; LNCLEN   line color screen underlaying enable 
	dc.w	$00EA,$0000	; SFPRMD   special priority func ctl reg 
	dc.w	$00EC,$0000	; CCCTL	   color mixing control reg 
	dc.w	$00EE,$0000	; SFCCMD   special color mixing func ctl reg 
	dc.w	$00F0,$FFFF	; PRISA	   sprite priority number reg 
	dc.w	$00F2,$FFFF	; PRISB	   sprite priority number reg 
	dc.w	$00F4,$FFFF	; PRISC	   sprite priority number reg 
	dc.w	$00F6,$FFFF	; PRISD	   sprite priority number reg 
	dc.w	$00F8,$0000	; PRINA	   priority number reg 
	dc.w	$00FA,$0000	; PRINB	   priority number reg 
	dc.w	$00FC,$0000	; PRIR	   priority number reg 
	dc.w	$0100,$0000	; CCRSA	   color calculation (for sprite) reg 
	dc.w	$0102,$0000	; CCRSB	   color calculation (for sprite) reg 
	dc.w	$0104,$0000	; CCRSC	   color calculation (for sprite) reg 
	dc.w	$0106,$0000	; CCRSD	   color calculation (for sprite) reg 
	dc.w	$0108,$0000	; CCRNA	   color calc ratio (NBG0, NBG1) 
	dc.w	$010A,$0000	; CCRNB	   color calc ratio (NBG0, NBG1) 
	dc.w	$010C,$0000	; CCRR	   color calc ratio (RBG0) 
	dc.w	$010E,$0000	; CCRLB	   color calc ratio (Line clr, BACK) 
	dc.w	$0110,$0000	; CLOFEN   color offset enable 
	dc.w	$0112,$0000	; CLOFSL   color value offset select reg 
	dc.w	$0114,$0000	; COAR	   color offset R value (set A) 
	dc.w	$0116,$0000	; COAG	   color offset G value (set A) 
	dc.w	$0118,$0000	; COAB	   color offset B value (set A) 
	dc.w	$011A,$0000	; COBR	   color offset R value (set B) 
	dc.w	$011C,$0000	; COBG	   color offset G value (set B) 
	dc.w	$011E,$0000	; COBB	   color offset B value (set B) 
VDP2regsend

	align 4

	end
