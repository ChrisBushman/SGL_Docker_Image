;****************************************************************
;	Saturn Sample Program
;	Master sh2 global variables
;
;	Copyright SEGA of America inc. 1994
;---------------------------------------------------------------*
;****************************************************************

	include	"sh2.mac"
	include	"equ.h"

	xdef	_temppoints, _spritebuf, _vbrsave

	section.l .bss


	align 4	


_temppoints	ds.b	65536*3
	
_spritebuf	ds.b	65536*3

_vbrsave		ds.l	1

	xdef	sincosbuf, transtmp
sincosbuf	ds.l	4
transtmp	ds.l	4


;------------------------------------------------
;       Z sort buffer assign
;------------------------------------------------
;       Z sort address buffer
	xdef _zbuffer
_zbuffer		ds.b	ZBUFSIZE

	section.l	.text

	align 4

	xdef	_randval
_randval		dc.l	$35735373

	xdef	_drawcounter
_drawcounter	dc.l	0

	xdef	_intmask
_intmask		dc.l	$0000DFFC
		
	xdef	_vcounter
_vcounter	dc.l	0
_tickcount	dc.l	0
	
	xdef	_VDP1busy, _drawflag, _clearflag
_VDP1busy	dc.l	0
_drawflag	dc.l	0
_clearflag	dc.l	0


	xdef	_pad0, _pad1
_pad0		dc.l	$FFFFFFF
_pad1		dc.l	$FFFFFFF

	xdef	_numsprites
_numsprites	dc.l	0

	xdef	_lastsprite
_lastsprite	dc.l	0


	xdef	_camerax, _cameray, _cameraz
	xdef	_objectx, _objecty, _objectz
	xdef	_pitch, _yaw, _bank

_camerax	dc.l	$00000000
_cameray	dc.l	$00000000
_cameraz	dc.l	$00000000		; 320*65536

_objectx	dc.l	$00000000
_objecty	dc.l	0
_objectz	dc.l	$B0000000		; 320*65536



_pitch	dc.l	$BE93E980
_yaw	dc.l	$422222E0
_bank	dc.l	$F8E39040

	lits


	end
