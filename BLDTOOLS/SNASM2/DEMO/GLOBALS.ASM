
;****************************************************************
;	Saturn Sample Program
;	Master sh2 global variables
;
;	Copyright SEGA of America inc. 1994
;---------------------------------------------------------------*
;****************************************************************

	include	"sh2.mac"
	include	"equ.h"

	section	.text

	.align 4

	xdef	intmask
intmask		dc.l	$0000BFFE

	xdef	vcounter
vcounter	dc.l	0

	xdef	pad0, pad1
pad0		dc.l	$FFFFFFF
pad1		dc.l	$FFFFFFF

	xdef	numsprites
numsprites	dc.l	0

	xdef	lastsprite
lastsprite	dc.l	0


	xdef	Mstack
Mstack	dcb.l	MSTACK_SIZE*MATRIX_SIZE,0

	xdef	camerax, cameray, cameraz
	xdef	xangle, yangle, zangle

camerax	dc.l	0
cameray	dc.l	0
cameraz	dc.l	0

xangle	dc.l	0
yangle	dc.l	0
zangle	dc.l	0


	end
