	
	include	"sh2.mac"
	include "equ.h"
	include "spcmd.h"

	section	.text

	align 4


	xdef	plane_shape1
plane_shape1
	dc.w	4			; number of points
	dc.w	2			; number of faces
	dc.l	0			; flags
; virtex's start here
	dc.l	-8*65536 , -8*65536 , 1*65536

	dc.l	8*65536  , -8*65536 , 1*65536
				     
	dc.l	8*65536  , 8*65536  , 1*65536

	dc.l	-8*65536 , 8*65536  , 1*65536

; faces start here
	dc.w	0,1,2,3
	dc.l	redboxsurface16
	dc.w	1,0,3,2 
	dc.l	blueboxsurface16
	

	align 4


	xdef	cube_shape16x16
cube_shape16x16
	dc.w	8			; number of points
	dc.w	6			; number of faces
	dc.l	0			; flags
; virtex's start here
	dc.l	-8*65536 , -8*65536 , 1*65536

	dc.l	8*65536  , -8*65536 , 1*65536
				     
	dc.l	8*65536  , 8*65536  , 1*65536

	dc.l	-8*65536 , 8*65536  , 1*65536

	dc.l	8*65536  , -8*65536 , 16*65536

	dc.l	8*65536  , 8*65536  , 16*65536

	dc.l	-8*65536 , -8*65536 , 16*65536

	dc.l	-8*65536 , 8*65536  , 16*65536
		
; faces start here
	dc.w	0,1,2,3
	dc.l	blueboxsurface16
	dc.w	1,4,5,2 
	dc.l	redboxsurface16
	dc.w	0,6,4,1
	dc.l	blueboxsurface16
	dc.w	4,6,7,5
	dc.l	greenboxsurface16
	dc.w	6,0,3,7
	dc.l	redboxsurface16
	dc.w	3,2,5,7
	dc.l	goldboxsurface16

	align 4

	xdef	cube_shape8x8
cube_shape8x8
	dc.w	8			; number of points
	dc.w	6			; number of faces
	dc.l	0			; flags
; virtex's start here
	dc.l	-4*65536 , -4*65536 , 1*65536

	dc.l	4*65536  , -4*65536 , 1*65536
				     
	dc.l	4*65536  , 4*65536  , 1*65536

	dc.l	-4*65536 , 4*65536  , 1*65536

	dc.l	4*65536  , -4*65536 , 8*65536

	dc.l	4*65536  , 4*65536  , 8*65536

	dc.l	-4*65536 , -4*65536 , 8*65536

	dc.l	-4*65536 , 4*65536  , 8*65536
		
; faces start here
	dc.w	0,1,2,3
	dc.l	blueboxsurface8
	dc.w	1,4,5,2 
	dc.l	redboxsurface8
	dc.w	0,6,4,1
	dc.l	blueboxsurface8
	dc.w	4,6,7,5
	dc.l	greenboxsurface8
	dc.w	6,0,3,7
	dc.l	redboxsurface8
	dc.w	3,2,5,7
	dc.l	goldboxsurface8

	align 4



polyboxsurface1
	dc.w	2
	dc.w	FUNC_POLYGON
	dc.w	$10E8
	dc.w	$81e0
	dc.w	redboxADDR
	dc.w	4*256+32
	dc.w	0
	dc.w	0

polyboxsurface2
	dc.w	2
	dc.w	FUNC_POLYGON
	dc.w	$10E8
	dc.w	$800f
	dc.w	redboxADDR
	dc.w	4*256+32
	dc.w	0
	dc.w	0


redboxsurface16
	dc.w	1
	dc.w	FUNC_DISTORSP
	dc.w	$18E8
	dc.w	0
	dc.w	redboxADDR
	dc.w	2*256+16
	dc.w	0
	dc.w	0

greenboxsurface16
	dc.w	1
	dc.w	FUNC_DISTORSP
	dc.w	$18E8
	dc.w	0
	dc.w	greenboxADDR
	dc.w	2*256+16
	dc.w	0
	dc.w	0

blueboxsurface16
	dc.w	1
	dc.w	FUNC_DISTORSP
	dc.w	$18E8
	dc.w	0
	dc.w	blueboxADDR
	dc.w	2*256+16
	dc.w	0
	dc.w	0

goldboxsurface16
	dc.w	1
	dc.w	FUNC_DISTORSP
	dc.w	$18E8
	dc.w	0
	dc.w	goldboxADDR
	dc.w	2*256+16
	dc.w	0
	dc.w	0


redboxsurface8
	dc.w	1
	dc.w	FUNC_DISTORSP
	dc.w	$18E8
	dc.w	0
	dc.w	redboxADDR
	dc.w	1*256+8
	dc.w	0
	dc.w	0

greenboxsurface8
	dc.w	1
	dc.w	FUNC_DISTORSP
	dc.w	$18E8
	dc.w	0
	dc.w	greenboxADDR
	dc.w	1*256+8
	dc.w	0
	dc.w	0

blueboxsurface8
	dc.w	1
	dc.w	FUNC_DISTORSP
	dc.w	$18E8
	dc.w	0
	dc.w	blueboxADDR
	dc.w	1*256+8
	dc.w	0
	dc.w	0

goldboxsurface8
	dc.w	1
	dc.w	FUNC_DISTORSP
	dc.w	$18E8
	dc.w	0
	dc.w	goldboxADDR
	dc.w	1*256+8
	dc.w	0
	dc.w	0


	
	align 4



	end
