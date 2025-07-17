;****************************************************************
;	Saturn Sample Program
;	SH2 Master Main
;
;	Copyright SEGA of America inc. 1994
;---------------------------------------------------------------*
;****************************************************************

	include	"sh2.mac"
	include	"globals.h"
	include	"equ.h"
	include "regs.h"
	include "spcmd.h"

	section	.text

	xref	spritelist
	xref	_frontclip

	xref	_make_vertices_list

	align 4


; add a face of a poly object to the sprite list
; input
;	r1 = address of object struct
;	r2 = address of rotated point list
;	r3 = address of the face to do
;	r4 = sprite address
;	r5 = Average z distance
;
	xdef	zsortface
zsortface

	mov	r4,r11
	mov.l	#_spritebuf,r0
	sub	r0,r11
	shlr2	r11			; scale sprite addres into a word
	add	#1,r11

	mov.l	#_zbuffer+ZBUFSIZE-2,r10
	shll	r5
	sub	r5,r10
	mov.w	@r10,r0

	mov.w	r0,@(2,r4)		; store the address of the sprite already there
	
	rts
	mov.w	r11,@r10


	lits
	align 4


; add a face of a poly object to the sprite list
; input		   
;	r1 = address of object struct
;	r2 = address of rotated point list
;	r3 = address of the face to do
;
	xdef	addtexface
addtexface
	push	pr
	mov.l	#_frontclip,r13
	mov.l	@r13,r13
	
	mov.w	@(f_p1,r3),r0
	shll2	r0
	shll2	r0
	add	#8,r0
	mov.l	@(r2,r0),r14
	cmp/gt	r14,r13
	bt	@zclipped

	shlr16	r14
	mov	r14,r12			; r12 = accumulated z value

	add	#4,r0

	mov.l	@(r2,r0),r5
	swap.w	r5,r4
	extu.w	r4,r4		; r4 = x1 of point
	extu.w	r5,r5		; r5 = y1 of point

	mov.w	@(f_p2,r3),r0
	shll2	r0
	shll2	r0
	add	#8,r0
	mov.l	@(r2,r0),r14
	cmp/gt	r14,r13
	bt	@zclipped

	shlr16	r14
	add	r14,r12			; r12 = accumulated z value

	add	#4,r0

	mov.l	@(r2,r0),r7
	swap.w	r7,r6
	extu.w	r6,r6		; r6 = x2 of point
	extu.w	r7,r7		; r7 = y2 of point


	mov.w	@(f_p3,r3),r0
	shll2	r0
	shll2	r0
	add	#8,r0
	mov.l	@(r2,r0),r14
	cmp/gt	r14,r13
	bt	@zclipped

	shlr16	r14
	add	r14,r12			; r12 = accumulated z value


	add	#4,r0

	mov.l	@(r2,r0),r9
	swap.w	r9,r8
	extu.w	r8,r8		; r8 = x3 of point
	extu.w	r9,r9		; r9 = y3 of point

	mov.w	@(f_p4,r3),r0
	shll2	r0
	shll2	r0
	add	#8,r0
	mov.l	@(r2,r0),r14
	cmp/gt	r14,r13
	bt	@zclipped

	shlr16	r14
	add	r12,r14			; r12 = accumulated z value

	shlr2	r14

	mov	r6,r10		
	sub	r4,r10		; r10 = Dx1

	mov	r7,r11
	sub	r5,r11		; r11 = Dy1

	mov	r8,r12
	sub	r6,r12		; r12 = Dx2

	mov	r9,r13
	sub	r7,r13		; r13 = Dy2

	
	muls	r10,r13		; mul X1 * Y2
	sts	macl,r10
	muls	r12,r11		; mul X2 * Y1
	sts	macl,r11
	sub	r11,r10
	cmp/pl	r10
	bf	@continue
@zclipped:
	rts
	pop	pr
@continue:

	add	#4,r0

	mov.l	@(r2,r0),r11
	swap.w	r11,r10
	extu.w	r10,r10		; r10 = x4 of point
	extu.w	r11,r11		; r11 = y4 of point

	mov.l	#cliptable,r0

	mov.l	@r0,r12
	cmp/gt	r4,r12
	bf	@next1

	mov.l	@(4,r0),r12
	cmp/gt	r4,r12
	bt	@next1

	mov.l	@(8,r0),r12
	cmp/gt	r5,r12
	bt	@next1

	mov.l	@(12,r0),r12
	cmp/gt	r5,r12
	bt	@notclipped

@next1:

	mov.l	@r0,r12
	cmp/gt	r6,r12
	bf	@next2

	mov.l	@(4,r0),r12
	cmp/gt	r6,r12
	bt	@next2

	mov.l	@(8,r0),r12
	cmp/gt	r7,r12
	bt	@next2

	mov.l	@(12,r0),r12
	cmp/gt	r7,r12
	bt	@notclipped

@next2:

	mov.l	@r0,r12
	cmp/gt	r8,r12
	bf	@next3

	mov.l	@(4,r0),r12
	cmp/gt	r8,r12
	bt	@next3

	mov.l	@(8,r0),r12
	cmp/gt	r9,r12
	bt	@next3

	mov.l	@(12,r0),r12
	cmp/gt	r9,r12
	bt	@notclipped

@next3:

	mov.l	@r0,r12
	cmp/gt	r10,r12
	bf	@zclipped

	mov.l	@(4,r0),r12
	cmp/gt	r10,r12
	bt	@zclipped

	mov.l	@(8,r0),r12
	cmp/gt	r11,r12
	bt	@zclipped

	mov.l	@(12,r0),r12
	cmp/gt	r11,r12
	bf	@zclipped

@notclipped:

	mov.l	#_numsprites,r0	; increment sprite list size
	mov.l	@r0,r4
	add	#1,r4
	mov.l	r4,@r0
	
	mov.l	#_lastsprite,r0	; get the current free sprite entry
	mov.l	@r0,r4		; R4 = address of available sprite

	mov	r4,r5		; add sprite size to last sprite pointer
	add	#32,r5
	mov.l	r5,@r0

	mov	r14,r5		; R5 = average z distance
	call	zsortface

	mov.l	@(f_surface,r3),r6	; R6 = address of surface structure

	mov.w	@(s_type,r6),r0		; type of sprite into r0
	cmp/eq	#1,r0
	bf	@nottext
	mov.w	@(s_control,r6),r0
	mov.w	r0,@r4

	mov.w	@(s_color,r6),r0
	mov.w	r0,@(6,r4)

	mov.w	@(s_drawmode,r6),r0
	mov.w	r0,@(4,r4)

	mov.w	@(s_charaddr,r6),r0
	mov.w	r0,@(8,r4)

	mov.w	@(s_charsize,r6),r0
	mov.w	r0,@(10,r4)

	bra	@dorest
	nop
@nottext:
	mov.w	@(s_type,r6),r0		; type of sprite into r0
	cmp/eq	#2,r0
	bf	@notpoly

	mov.w	@(s_control,r6),r0	; set draw function
	mov.w	r0,@r4

	mov.w	@(s_drawmode,r6),r0	; set draw mode
	mov.w	r0,@(4,r4)

	mov.w	@(s_color,r6),r0 	; set color
	mov.w	r0,@(6,r4)

	bra	@dorest
	nop
@notpoly:
		
@dorest:

	add	#12,r4		; R4 = address of point list of sprite

	mov	#4,r5
@pointloop:

	mov.w	@r3+,r0
	shll2	r0
	shll2	r0
	add	#12,r0
	mov.l	@(r2,r0),r0
	mov.l	r0,@r4

	dt	r5
	bf/s	@pointloop
	add	#4,r4

	pop	pr

	
	rts
	nop

	align 4
	lits


cliptable:
	dc.l	352
	dc.l	0
	dc.l	0
	dc.l	224

; add a face of a poly object to the sprite list
; input
;	r4 = address of object struct
;	r5 = address of point list buffer
;
	xdef	_addobject
_addobject
	push	pr

	push	r8,r9,r10,r11,r12,r13,r14

	mov	r4,r1
	mov	r5,r2

	mov.l	@(o_xpos,r1),r0
	mov.l	#_objectx,r4
	mov.l	r0,@r4

	mov.l	@(o_ypos,r1),r0
	mov.l	#_objecty,r4
	mov.l	r0,@r4

	mov.l	@(o_zpos,r1),r0
	mov.l	#_objectz,r4
	mov.l	r0,@r4

	mov.l	@(o_shape,r1),r0

	push	r1

	mov	r1,r3
	add	#o_pitch,r3
	mov	r0,r1
	call	addshape

	pop	r1

	pop	r14,r13,r12,r11,r10,r9,r8

	pop	pr

	rts
	nop
	


; add a face of a poly object to the sprite list
; input
;	r1 = address of shape struct
;	r2 = address of point list buffer
;	r3 = address of the angles to use
;
	xdef	addshape
addshape
	push	pr
	
	push	r1
	push	r2

	mov.w	@(sh_numpoints,r1),r0
	mov	r0,r7

	mov.l	#256,r6

	mov	r2,r5
	mov	r1,r4
	add	#sh_pointlist,r4

	call	_make_vertices_list

	pop	r2
	pop	r1

	mov.w	(sh_numfaces,r1),r0
	mov	r0,r4

	mov	r1,r3
	add	#sh_pointlist,r3
	mov.w	@(sh_numpoints,r1),r0
	mov	#12,r5
	mulu	r0,r5
	sts	macl,r0
	add	r0,r3		; R3 = address of face list
	
@faceloop:
	push	r1
	push	r2
	push	r3
	push	r4

	call	addtexface

	pop	r4
	pop	r3
	pop	r2
	pop	r1

	dt	r4
	bf/s	@faceloop
	add	#FACE_SIZE,r3

	
	pop	pr

	rts
	nop

	align 4
	lits
	
	align 4

matrix1	dcb.l	9,0
tpoint	dcb.l	9,0


	end

