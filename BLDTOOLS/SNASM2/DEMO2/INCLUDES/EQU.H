;------------------------------------------------
; equates       
;------------------------------------------------


;------------------------------------------------
;	Game frame rate
;------------------------------------------------
FRAMERATE		equ	2

;------------------------------------------------
;       Z sort buffer assign
;------------------------------------------------

ZDISTMAX			equ	$800


ZBUFSIZE			equ	ZDISTMAX*2

FIRSTSPRITE		equ	4			; number of the first unused sprite def in vram

NUMOBJECTS		equ	1

SPRITEBASE		equ	65536*2

BOXSIZE			equ	(((16*16)*2)/8)
NUMBERSIZE		equ	(((32*32)*2)/8)
redboxADDR		equ	SPRITEBASE/8
greenboxADDR	equ	redboxADDR+BOXSIZE
blueboxADDR		equ	greenboxADDR+BOXSIZE
goldboxADDR		equ	blueboxADDR+BOXSIZE
numberADDR		equ	goldboxADDR+BOXSIZE
grassADDR		equ	numberADDR+(NUMBERSIZE*6)


SCRNWIDTH	equ	352
SCRNHEIGHT	equ	224

;
; ==========================================================================
; FRAME-BUFFER
; ==========================================================================
;

VDP1MODE					equ	0

VideoFrameSizeX   	equ	512
VideoFrameSizeY   	equ	224

;------------------------------------------------
;       Matrix data buffer assign       
;------------------------------------------------

MATRIX_DATA_SIZE        equ    4
MATRIX_SIZE             equ    MATRIX_DATA_SIZE*9


;*****************************************************************
; object data structure
;*****************************************************************

	rsreset

o_type		rs.w	1
o_visible	rs.w	1
o_shape		rs.l	1
o_xpos		rs.l	1
o_ypos		rs.l	1
o_zpos		rs.l	1
o_pitch		rs.l	1
o_yaw			rs.l	1
o_bank		rs.l	1

O_SIZE		rs.l	0

	rsreset	

;*****************************************************************
; shape data structure
;*****************************************************************

	rsreset

sh_numpoints	rs.w	1
sh_numfaces		rs.w	1
sh_flags			rs.l	1
sh_pointlist	rs.l	0

	rsreset


;*****************************************************************
; face data structure
;*****************************************************************

	rsreset

f_p1			rs.w	1
f_p2			rs.w	1
f_p3			rs.w	1
f_p4			rs.w	1
f_surface	rs.l	1

FACE_SIZE	rs.l	0
	rsreset


;*****************************************************************
; surface data structure
;*****************************************************************

	rsreset

s_type		rs.w	1	; type of surface
s_control	rs.w	1	; 0  control
s_drawmode	rs.w	1	; 4  drawMode 
s_color		rs.w	1	; 6  color    
s_charaddr	rs.w	1	; 8  charAddr 
s_charsize	rs.w	1	; 10 charSize 
s_shade		rs.w	1	; 20 grshAddr 
s_bitmap		rs.w	1	; bitmap number if textured

	rsreset


;*****************************************************************
; sprite data structure
;*****************************************************************

	rsreset

p_type		rs.w	1		; control
p_control	rs.w	1		; link     
p_drawmode	rs.w  1		; drawMode 
p_color		rs.w  1		; color    
p_charaddr	rs.w  1		; charAddr 
p_charsize	rs.w  1		; charSize 
p_pointA		rs.w  2		; ax, ay   
p_pointB		rs.w  2		; bx, by   
p_pointC		rs.w  2		; cx, cy   
p_pointD		rs.w  2		; dx, dy   
p_shade		rs.w  1		; grshAddr 
p_dummy		rs.w  1		; dummy    

	rsreset

