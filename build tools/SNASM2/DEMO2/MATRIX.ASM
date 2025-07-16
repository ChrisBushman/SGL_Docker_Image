
	include	"sh2.mac"
	include	"globals.h"
	include	"equ.h"
	include "regs.h"

	section	.text


;------------------------------------------------
;       Matrix data buffer assign       
;------------------------------------------------
SIZE_MATRIX             equ    MATRIX_DATA_SIZE*12
MSTACK_SIZE		equ	16

M00     equ    0*MATRIX_DATA_SIZE
M01     equ    1*MATRIX_DATA_SIZE
M02     equ    2*MATRIX_DATA_SIZE
M03     equ    3*MATRIX_DATA_SIZE
M04     equ    4*MATRIX_DATA_SIZE
M05     equ    5*MATRIX_DATA_SIZE
M06     equ    6*MATRIX_DATA_SIZE
M07     equ    7*MATRIX_DATA_SIZE
M08     equ    8*MATRIX_DATA_SIZE
M09     equ    9*MATRIX_DATA_SIZE
M10     equ    10*MATRIX_DATA_SIZE
M11     equ    11*MATRIX_DATA_SIZE


	xdef _frontclip
_frontclip	dc.l	0

mstack	dcb.b	MSTACK_SIZE*SIZE_MATRIX,0

mstacktop	dc.l	mstack




;****************************************************************
;*      input   r4      source normal X Y Z		        *
;*              r5	destination normal XYZ                  *
;****************************************************************
	xdef	_rotate_point
_rotate_point

	mov.l	#mstacktop,r0
	mov.l	@r0,r2

	clrmac				;X = x*m00 + y*m01 +z*m02 + 1*m03
	mac.l   @r2+,@r4+
	mac.l   @r2+,@r4+
	mac.l   @r2+,@r4+
	add     #-3*4,r4
	mov.l   @r2+,r3
	sts     mach,r0
	sts     macl,r1

	clrmac          
	xtrct   r0,r1
	add     r3,r1
	mov.l   r1,@(0,r5)

	mac.l   @r2+,@r4+		;Y = x*m04 + y*m05 +z*m06 + 1*m07
	mac.l   @r2+,@r4+
	mac.l   @r2+,@r4+

	mov.l   @r2+,r3
	sts     mach,r0
	sts     macl,r1
	xtrct   r0,r1
	add     r3,r1
	mov.l   r1,@(4,r5)

	clrmac                  	;Z = x*m08 + y*m09 +z*m10 + 1*m11
	mac.l   @r2+,@r4+
	mac.l   @r2+,@r4+
	mac.l   @r2+,@r4+
	add     #-3*4,r4
	mov.l   @r2+,r3
	sts     mach,r0
	sts     macl,r1
	xtrct   r0,r1
	add     r3,r1

	rts
	mov.l   r1,@(8,r5)


	lits
	
	align 4



;****************************************************************
;*      input   r4      source 3D point data address            *
;*              r5	vertices buffer address                 *
;*              r6      scale                                   *
;*              r7      number of points	                *
;****************************************************************
	xdef	_make_vertices_list
_make_vertices_list

	mov.l   #divu_reg,r11    	;r11 = divu register
	
	mov.l	#mstacktop,r0		
	mov.l	@r0,r9			; R9 = rotation matrix to use

	add	#M08,r9
@loop:
	clrmac                  	;Z = x*m08 + y*m09 +z*m10 + 1*m11
	mac.l   @r9+,@r4+
	mac.l   @r9+,@r4+
	mac.l   @r9+,@r4+
	mov.l   @r9+,r8
	sts     mach,r0
	sts     macl,r3
	xtrct   r0,r3
	add     r8,r3

	mov.l   r3,@(divu_jr,r11)	; scale the z pos
	mov.l   r6,@(divu_hrh,r11)
	mov     #0,r0
	mov.l   r0,@(divu_hrl,r11)
	
	add     #-3*4,r4

	mov.l   r3,@(8,r5)		; save rotated Z

	clrmac				;X = x*m00 + y*m01 +z*m02 + 1*m03
	add     #-SIZE_MATRIX,r9
	mac.l   @r9+,@r4+
	mac.l   @r9+,@r4+
	mac.l   @r9+,@r4+
	add     #-3*4,r4
	mov.l   @r9+,r8
	sts     mach,r0
	sts     macl,r1

	clrmac          
	xtrct   r0,r1
	add     r8,r1
	mov.l   r1,@(0,r5)		;save rotated X (just in case)

	mac.l   @r9+,@r4+		;Y = x*m04 + y*m05 +z*m06 + 1*m07
	mac.l   @r9+,@r4+
	mac.l   @r9+,@r4+

	mov.l   @r9+,r8
	sts     mach,r0
	sts     macl,r2
	xtrct   r0,r2
	add     r8,r2
	mov.l   r2,@(4,r5)		; save rotated Y (just in case)

	mov	#176,r8

	mov.l   @(divu_rsr,r11),r3	; R3 = result of divide
	dmuls.l r3,r1			; scale X using fractional mutiply
	sts     mach,r0
	dmuls.l r3,r2			; scale Y using fractional mutiply
	add	r8,r0
	mov.w   r0,@(12,r5)		; store projected X
	sts     mach,r0
	mov	#112,r8
	add	r8,r0
	mov.w   r0,@(14,r5)

	dt      r7
	bf/s    @loop
	add     #16,r5         ;vertices buffer + 16

	rts
	nop

	align 4
	lits

	align 4

; void push_matrix(void);
	xdef	_push_matrix
_push_matrix

	mov.l	#mstacktop,r6
	mov.l	@r6,r4

	mov     r4,r5
	add     #SIZE_MATRIX,r4

	mov.l	r4,@r6

	mov.l   @(M00,r5),r1
	mov.l   @(M01,r5),r2
	mov.l   @(M02,r5),r3
	mov.l   r1,@(M00,r4)
	mov.l   r2,@(M01,r4)
	mov.l   r3,@(M02,r4)

	mov.l   @(M03,r5),r1
	mov.l   @(M04,r5),r2
	mov.l   @(M05,r5),r3
	mov.l   r1,@(M03,r4)
	mov.l   r2,@(M04,r4)
	mov.l   r3,@(M05,r4)

	mov.l   @(M06,r5),r1
	mov.l   @(M07,r5),r2
	mov.l   @(M08,r5),r3
	mov.l   r1,@(M06,r4)
	mov.l   r2,@(M07,r4)
	mov.l   r3,@(M08,r4)

	mov.l   @(M09,r5),r1
	mov.l   @(M10,r5),r2
	mov.l   @(M11,r5),r3
	mov.l   r1,@(M09,r4)
	mov.l   r2,@(M10,r4)


	rts
	mov.l   r3,@(M11,r4)

	lits
	align 4

		
	xdef	_pop_matrix
_pop_matrix
	mov.l	#mstacktop,r0
	mov.l	@r0,r1
	add     #-SIZE_MATRIX,r1
	mov.l	r1,@r0

	rts
	nop

	lits
	align 4


; create an identity matrix
	xdef	_initial_matrix
_initial_matrix

	mov.l	#mstacktop,r0
	mov.l	@r0,r4
	
	mov     #0,r0                   ;r0 = $00000000
	mov     #1,r1                   ;r1 = $00010000
	swap.w  r1,r1                   ;       //
	
	mov.l   r1,@(M00,r4)
	mov.l   r0,@(M01,r4)
	mov.l   r0,@(M02,r4)
	mov.l   r0,@(M03,r4)

	mov.l   r0,@(M04,r4)
	mov.l   r1,@(M05,r4)
	mov.l   r0,@(M06,r4)
	mov.l   r0,@(M07,r4)

	mov.l   r0,@(M08,r4)
	mov.l   r0,@(M09,r4)
	mov.l   r1,@(M10,r4)

	rts
	mov.l   r0,@(M11,r4)

	lits
	align 4


;****************************************************************
;*      input   r4      translate x                             *
;*              r5      translate y                             *
;*              r6      translate z                             *
;****************************************************************
	xdef	_translate
_translate

	mov.l   #transtmp,r0
	mov.l   r4,@(0*4,r0)
	mov.l   r5,@(1*4,r0)
	mov.l   r6,@(2*4,r0)

	mov.l	#mstacktop,r4
	mov.l	@r4,r4
	

	mov     #3,r3                   ;r3 = loop counter
@loop:
	clrmac                          ;m03 = x*m00 + y*m01 + z*m02 + 1*m03
	mac.l   @r4+,@r0+
	mac.l   @r4+,@r0+
	mac.l   @r4+,@r0+
	add     #-3*4,r0
	mov.l   @r4,r5
	sts     mach,r1
	sts     macl,r2
	xtrct   r1,r2
	add     r5,r2
	mov.l   r2,@r4

	dt      r3
	bf/s    @loop
	add     #1*4,r4

	rts
	nop

	lits
	align 4



;****************************************************************
;*      input   r4      scale x                                 *
;*              r5      scale y                                 *
;*              r6      scale z                                 *
;****************************************************************
	xdef	_scale
_scale
	mov.l	#mstacktop,r0
	mov.l	@r0,r7
	
	mov     #3,r3

@loop:
	mov.l   @(0*4,r7),r0            ;m00 = x*m00
	dmuls.l r0,r4
	sts     mach,r1
	sts     macl,r2
	xtrct   r1,r2
	mov.l   r2,@(0*4,r7)

	mov.l   @(1*4,r7),r0            ;m01 = y*m01
	dmuls.l r0,r5
	sts     mach,r1
	sts     macl,r2
	xtrct   r1,r2
	mov.l   r2,@(1*4,r7)

	mov.l   @(2*4,r7),r0            ;m02 = z*m02
	dmuls.l r0,r6
	sts     mach,r1
	sts     macl,r2
	xtrct   r1,r2
	mov.l   r2,@(2*4,r7)

	dt      r3
	bf/s    @loop
	add     #4*4,r7

	rts
	nop

	lits
	align 4



;****************************************************************
;*      input   r4      x angle                                 *
;*      output  (r4)    matrix data                             *
;*      break                                                   *
;****************************************************************
	xdef	_rotate_x
_rotate_x

	swap.w	r4,r4
	extu.w	r4,r4
	mov.l   #h'800,r6               ;r6 = $800 (pai/2)
	mov.l   #h'1ffe,r1              ;r1 = $1ffe (2 pai mask)
	mov.l   #sintbl,r0		;r0 = sin table top address
	mov.l   #sincosbuf,r7           ;r7 = sincosbuf
			
	add     r4,r6                   ;r6 = r4 + $800 (for cos)
		
	and     r1,r4                   ;r4 = theta.and.$fff
	and     r1,r6                   ;r6 = (theta+$400).and.$fff

	mov.w   @(r0,r4),r4             ;r4 = sin theta
	mov.w   @(r0,r6),r6             ;r6 = cos theta
	shll2   r4
	shll2   r6

	neg     r4,r0                   ;r0 = -sin theta

	mov.l   r6,@(0*4,r7)            ;set cos theta
	mov.l   r0,@(1*4,r7)            ;set -sin theta
	mov.l   r4,@(2*4,r7)            ;set sin theta
	mov.l   r6,@(3*4,r7)            ;set cos theta

	mov.l	#mstacktop,r3
	mov.l	@r3,r4
	

	mov     #3,r3                   ;set loop counter
@loop:
	mov     r4,r5                   ;copy matrix address
	mov     r7,r6                   ;copy sincosbuf address
	
	clrmac
	add     #1*4,r5                 ;skip m00
	mac.l   @r6+,@r5+               ;cos * m01
	mac.l   @r6+,@r5+               ;-sin * m02
	add     #-2*4,r5                ;r5 = m01
	sts     mach,r0
	sts     macl,r1
	xtrct   r0,r1                   ;r1 = cos*m01-sin*m02
	
	clrmac
	mac.l   @r6+,@r5+               ;sin * m01
	mac.l   @r6+,@r5+               ;cos * m02
	sts     mach,r0
	sts     macl,r2
	xtrct   r0,r2                   ;r2 = sin*m01+cos*m02
	
	mov.l   r1,@(1*4,r4)            ;set m01
	mov.l   r2,@(2*4,r4)            ;set m02

	dt      r3
	bf/s    @loop
	add     #4*4,r4

	rts
	nop

	lits
	align 4


;****************************************************************
;*      input   r4      y angle                                 *
;*      output  (r4)    matrix data                             *
;*      break                                                   *
;****************************************************************
	xdef	_rotate_y
_rotate_y

	swap.w	r4,r4
	extu.w	r4,r4
	mov.l   #h'800,r6               ;r6 = $800 (pai/2)
	mov.l   #h'1ffe,r1              ;r1 = $1ffe (2 pai mask)
	mov.l   #sintbl,r0		;r0 = sin table top address
	mov.l   #sincosbuf,r7           ;r7 = sincosbuf
			
	add     r4,r6                   ;r6 = r4 + $800 (for cos)
		
	and     r1,r4                   ;r4 = theta.and.$fff
	and     r1,r6                   ;r6 = (theta+$400).and.$fff

	mov.w   @(r0,r4),r4             ;r4 = sin theta
	mov.w   @(r0,r6),r6             ;r6 = cos theta
	shll2   r4
	shll2   r6

	neg     r4,r0                   ;r0 = -sin theta

	mov.l   r6,@(0*4,r7)            ;set cos theta
	mov.l   r4,@(1*4,r7)            ;set -sin theta
	mov.l   r0,@(2*4,r7)            ;set sin theta
	mov.l   r6,@(3*4,r7)            ;set cos theta

	
	mov.l	#mstacktop,r3
	mov.l	@r3,r4
	
	mov     #3,r3                   ;set loop counter
@loop:
	mov     r4,r5                   ;copy matrix address

	clrmac
	mov     r7,r6                   ;copy sincosbuf address
	mac.l   @r6+,@r5+               ;cos * m00
	add     #1*4,r5
	mac.l   @r6+,@r5+               ;sin * m02
	add     #-3*4,r5                ;r5 = m00
	sts     mach,r0
	sts     macl,r1
	xtrct   r0,r1                   ;r1 = cos*m00+sin*m02
	
	clrmac
	mac.l   @r6+,@r5+               ;-sin * m00
	add     #1*4,r5
	mac.l   @r6+,@r5+               ;cos * m02
	sts     mach,r0
	sts     macl,r2
	xtrct   r0,r2                   ;r2 = -sin*m00+cos*m02
	
	mov.l   r1,@(0*4,r4)            ;set m00
	mov.l   r2,@(2*4,r4)            ;set m02
	
	dt      r3
	bf/s    @loop
	add     #4*4,r4
		
	rts
	nop

	lits
	align 4



;****************************************************************
;*      input   r4      angle                                 *
;****************************************************************

	xdef	_mysin
_mysin
	swap.w	r4,r4
	extu.w	r4,r4
	mov.l   #h'1ffe,r1              ;r1 = $1ffe (2 pai mask)
	mov.l	#sintbl,r0
	and	r1,r4
	mov.w	@(r0,r4),r0

	rts
	shll2	r0

;****************************************************************
;*      input   r4      angle                                 *
;****************************************************************

	xdef	_mycos
_mycos
	swap.w	r4,r4
	mov.l   #h'800,r2               ;r2 = $800 (pai/2)
	extu.w	r4,r4
	mov.l   #h'1ffe,r1              ;r1 = $1ffe (2 pai mask)
	add	r4,r2
	mov.l	#sintbl,r0
	and	r1,r2
	mov.w	@(r0,r2),r0

	rts
	shll2	r0
		
;****************************************************************
;*      £Ú¼´²óÅ¾                                                *
;*                                              02/07-1994      *
;****************************************************************
;*      input   r4      z angle                                 *
;****************************************************************

	xdef	_rotate_z
_rotate_z

	swap.w	r4,r4
	extu.w	r4,r4
	mov.l   #h'800,r6               ;r6 = $800 (pai/2)
	mov.l   #h'1ffe,r1              ;r1 = $1ffe (2 pai mask)
	mov.l   #sintbl,r0		;r0 = sin table top address
	mov.l   #sincosbuf,r7           ;r7 = sincosbuf
			
	add     r4,r6                   ;r6 = r4 + $800 (for cos)
		
	and     r1,r4                   ;r4 = theta.and.$fff
	and     r1,r6                   ;r6 = (theta+$400).and.$fff

	mov.w   @(r0,r4),r4             ;r4 = sin theta
	mov.w   @(r0,r6),r6             ;r6 = cos theta
	shll2   r4
	shll2   r6

	neg     r4,r0                   ;r0 = -sin theta

	mov.l   r6,@(0*4,r7)            ;set cos theta
	mov.l   r0,@(1*4,r7)            ;set -sin theta
	mov.l   r4,@(2*4,r7)            ;set sin theta
	mov.l   r6,@(3*4,r7)            ;set cos theta

	
	mov.l	#mstacktop,r3
	mov.l	@r3,r4
	
	mov     #3,r3                   ;set loop counter
@loop:
	mov     r4,r5                   ;copy matrix address
	
	clrmac
	mov     r7,r6                   ;copy sincosbuf address
	mac.l   @r6+,@r5+               ;cos * m00
	mac.l   @r6+,@r5+               ;-sin * m01
	add     #-2*4,r5                ;r5 = m01
	sts     mach,r0
	sts     macl,r1
	xtrct   r0,r1                   ;r1 = cos*m00-sin*m01
	
	clrmac
	mac.l   @r6+,@r5+               ;sin * m00
	mac.l   @r6+,@r5+               ;cos * m01
	sts     mach,r0
	sts     macl,r2
	xtrct   r0,r2                   ;r2 = sin*m00+cos*m01
	
	mov.l   r1,@(0*4,r4)            ;set m00
	mov.l   r2,@(1*4,r4)            ;set m01

	dt      r3
	bf/s    @loop
	add     #4*4,r4

	rts
	nop

	lits
	align 4


sintbl
	dc.w $0000,$0019,$0032,$004b,$0064,$007d,$0096,$00af
	dc.w $00c9,$00e2,$00fb,$0114,$012d,$0146,$015f,$0178
	dc.w $0192,$01ab,$01c4,$01dd,$01f6,$020f,$0228,$0241
	dc.w $025b,$0274,$028d,$02a6,$02bf,$02d8,$02f1,$030a
	dc.w $0323,$033d,$0356,$036f,$0388,$03a1,$03ba,$03d3
	dc.w $03ec,$0405,$041e,$0437,$0451,$046a,$0483,$049c
	dc.w $04b5,$04ce,$04e7,$0500,$0519,$0532,$054b,$0564
	dc.w $057d,$0596,$05af,$05c8,$05e1,$05fa,$0613,$062c
	dc.w $0645,$065e,$0677,$0690,$06a9,$06c2,$06db,$06f4
	dc.w $070d,$0726,$073f,$0758,$0771,$078a,$07a3,$07bc
	dc.w $07d5,$07ee,$0807,$0820,$0839,$0852,$086b,$0884
	dc.w $089c,$08b5,$08ce,$08e7,$0900,$0919,$0932,$094b
	dc.w $0964,$097c,$0995,$09ae,$09c7,$09e0,$09f9,$0a11
	dc.w $0a2a,$0a43,$0a5c,$0a75,$0a8d,$0aa6,$0abf,$0ad8
	dc.w $0af1,$0b09,$0b22,$0b3b,$0b54,$0b6c,$0b85,$0b9e
	dc.w $0bb6,$0bcf,$0be8,$0c01,$0c19,$0c32,$0c4b,$0c63
	dc.w $0c7c,$0c95,$0cad,$0cc6,$0cde,$0cf7,$0d10,$0d28
	dc.w $0d41,$0d59,$0d72,$0d8b,$0da3,$0dbc,$0dd4,$0ded
	dc.w $0e05,$0e1e,$0e36,$0e4f,$0e67,$0e80,$0e98,$0eb1
	dc.w $0ec9,$0ee2,$0efa,$0f12,$0f2b,$0f43,$0f5c,$0f74
	dc.w $0f8c,$0fa5,$0fbd,$0fd6,$0fee,$1006,$101f,$1037
	dc.w $104f,$1068,$1080,$1098,$10b0,$10c9,$10e1,$10f9
	dc.w $1111,$112a,$1142,$115a,$1172,$118a,$11a2,$11bb
	dc.w $11d3,$11eb,$1203,$121b,$1233,$124b,$1263,$127b
	dc.w $1294,$12ac,$12c4,$12dc,$12f4,$130c,$1324,$133c
	dc.w $1354,$136c,$1383,$139b,$13b3,$13cb,$13e3,$13fb
	dc.w $1413,$142b,$1443,$145a,$1472,$148a,$14a2,$14ba
	dc.w $14d1,$14e9,$1501,$1519,$1530,$1548,$1560,$1577
	dc.w $158f,$15a7,$15be,$15d6,$15ee,$1605,$161d,$1634
	dc.w $164c,$1664,$167b,$1693,$16aa,$16c2,$16d9,$16f1
	dc.w $1708,$171f,$1737,$174e,$1766,$177d,$1794,$17ac
	dc.w $17c3,$17da,$17f2,$1809,$1820,$1838,$184f,$1866
	dc.w $187d,$1895,$18ac,$18c3,$18da,$18f1,$1908,$1920
	dc.w $1937,$194e,$1965,$197c,$1993,$19aa,$19c1,$19d8
	dc.w $19ef,$1a06,$1a1d,$1a34,$1a4b,$1a62,$1a79,$1a8f
	dc.w $1aa6,$1abd,$1ad4,$1aeb,$1b02,$1b18,$1b2f,$1b46
	dc.w $1b5d,$1b73,$1b8a,$1ba1,$1bb7,$1bce,$1be5,$1bfb
	dc.w $1c12,$1c28,$1c3f,$1c55,$1c6c,$1c83,$1c99,$1caf
	dc.w $1cc6,$1cdc,$1cf3,$1d09,$1d20,$1d36,$1d4c,$1d63
	dc.w $1d79,$1d8f,$1da6,$1dbc,$1dd2,$1de8,$1dfe,$1e15
	dc.w $1e2b,$1e41,$1e57,$1e6d,$1e83,$1e99,$1eb0,$1ec6
	dc.w $1edc,$1ef2,$1f08,$1f1e,$1f34,$1f49,$1f5f,$1f75
	dc.w $1f8b,$1fa1,$1fb7,$1fcd,$1fe2,$1ff8,$200e,$2024
	dc.w $2039,$204f,$2065,$207b,$2090,$20a6,$20bb,$20d1
	dc.w $20e7,$20fc,$2112,$2127,$213d,$2152,$2168,$217d
	dc.w $2192,$21a8,$21bd,$21d2,$21e8,$21fd,$2212,$2228
	dc.w $223d,$2252,$2267,$227d,$2292,$22a7,$22bc,$22d1
	dc.w $22e6,$22fb,$2310,$2325,$233a,$234f,$2364,$2379
	dc.w $238e,$23a3,$23b8,$23cd,$23e1,$23f6,$240b,$2420
	dc.w $2434,$2449,$245e,$2473,$2487,$249c,$24b0,$24c5
	dc.w $24da,$24ee,$2503,$2517,$252c,$2540,$2554,$2569
	dc.w $257d,$2592,$25a6,$25ba,$25ce,$25e3,$25f7,$260b
	dc.w $261f,$2634,$2648,$265c,$2670,$2684,$2698,$26ac
	dc.w $26c0,$26d4,$26e8,$26fc,$2710,$2724,$2738,$274c
	dc.w $275f,$2773,$2787,$279b,$27af,$27c2,$27d6,$27ea
	dc.w $27fd,$2811,$2824,$2838,$284b,$285f,$2872,$2886
	dc.w $2899,$28ad,$28c0,$28d4,$28e7,$28fa,$290e,$2921
	dc.w $2934,$2947,$295a,$296e,$2981,$2994,$29a7,$29ba
	dc.w $29cd,$29e0,$29f3,$2a06,$2a19,$2a2c,$2a3f,$2a52
	dc.w $2a65,$2a77,$2a8a,$2a9d,$2ab0,$2ac2,$2ad5,$2ae8
	dc.w $2afa,$2b0d,$2b20,$2b32,$2b45,$2b57,$2b6a,$2b7c
	dc.w $2b8e,$2ba1,$2bb3,$2bc6,$2bd8,$2bea,$2bfc,$2c0f
	dc.w $2c21,$2c33,$2c45,$2c57,$2c6a,$2c7c,$2c8e,$2ca0
	dc.w $2cb2,$2cc4,$2cd6,$2ce8,$2cf9,$2d0b,$2d1d,$2d2f
	dc.w $2d41,$2d52,$2d64,$2d76,$2d88,$2d99,$2dab,$2dbc
	dc.w $2dce,$2de0,$2df1,$2e03,$2e14,$2e25,$2e37,$2e48
	dc.w $2e5a,$2e6b,$2e7c,$2e8d,$2e9f,$2eb0,$2ec1,$2ed2
	dc.w $2ee3,$2ef4,$2f05,$2f16,$2f27,$2f38,$2f49,$2f5a
	dc.w $2f6b,$2f7c,$2f8d,$2f9e,$2faf,$2fbf,$2fd0,$2fe1
	dc.w $2ff1,$3002,$3013,$3023,$3034,$3044,$3055,$3065
	dc.w $3076,$3086,$3096,$30a7,$30b7,$30c7,$30d8,$30e8
	dc.w $30f8,$3108,$3118,$3128,$3138,$3149,$3159,$3169
	dc.w $3178,$3188,$3198,$31a8,$31b8,$31c8,$31d8,$31e7
	dc.w $31f7,$3207,$3216,$3226,$3236,$3245,$3255,$3264
	dc.w $3274,$3283,$3293,$32a2,$32b1,$32c1,$32d0,$32df
	dc.w $32ee,$32fe,$330d,$331c,$332b,$333a,$3349,$3358
	dc.w $3367,$3376,$3385,$3394,$33a3,$33b2,$33c1,$33cf
	dc.w $33de,$33ed,$33fb,$340a,$3419,$3427,$3436,$3444
	dc.w $3453,$3461,$3470,$347e,$348c,$349b,$34a9,$34b7
	dc.w $34c6,$34d4,$34e2,$34f0,$34fe,$350c,$351a,$3528
	dc.w $3536,$3544,$3552,$3560,$356e,$357c,$3589,$3597
	dc.w $35a5,$35b3,$35c0,$35ce,$35dc,$35e9,$35f7,$3604
	dc.w $3612,$361f,$362c,$363a,$3647,$3654,$3662,$366f
	dc.w $367c,$3689,$3696,$36a4,$36b1,$36be,$36cb,$36d8
	dc.w $36e5,$36f1,$36fe,$370b,$3718,$3725,$3731,$373e
	dc.w $374b,$3757,$3764,$3771,$377d,$378a,$3796,$37a3
	dc.w $37af,$37bb,$37c8,$37d4,$37e0,$37ed,$37f9,$3805
	dc.w $3811,$381d,$3829,$3835,$3841,$384d,$3859,$3865
	dc.w $3871,$387d,$3889,$3894,$38a0,$38ac,$38b7,$38c3
	dc.w $38cf,$38da,$38e6,$38f1,$38fd,$3908,$3913,$391f
	dc.w $392a,$3935,$3941,$394c,$3957,$3962,$396d,$3978
	dc.w $3983,$398e,$3999,$39a4,$39af,$39ba,$39c5,$39d0
	dc.w $39da,$39e5,$39f0,$39fb,$3a05,$3a10,$3a1a,$3a25
	dc.w $3a2f,$3a3a,$3a44,$3a4f,$3a59,$3a63,$3a6d,$3a78
	dc.w $3a82,$3a8c,$3a96,$3aa0,$3aaa,$3ab4,$3abe,$3ac8
	dc.w $3ad2,$3adc,$3ae6,$3af0,$3afa,$3b03,$3b0d,$3b17
	dc.w $3b20,$3b2a,$3b34,$3b3d,$3b47,$3b50,$3b59,$3b63
	dc.w $3b6c,$3b75,$3b7f,$3b88,$3b91,$3b9a,$3ba3,$3bad
	dc.w $3bb6,$3bbf,$3bc8,$3bd1,$3bda,$3be2,$3beb,$3bf4
	dc.w $3bfd,$3c06,$3c0e,$3c17,$3c20,$3c28,$3c31,$3c39
	dc.w $3c42,$3c4a,$3c53,$3c5b,$3c63,$3c6c,$3c74,$3c7c
	dc.w $3c84,$3c8c,$3c95,$3c9d,$3ca5,$3cad,$3cb5,$3cbd
	dc.w $3cc5,$3ccc,$3cd4,$3cdc,$3ce4,$3cec,$3cf3,$3cfb
	dc.w $3d02,$3d0a,$3d12,$3d19,$3d21,$3d28,$3d2f,$3d37
	dc.w $3d3e,$3d45,$3d4d,$3d54,$3d5b,$3d62,$3d69,$3d70
	dc.w $3d77,$3d7e,$3d85,$3d8c,$3d93,$3d9a,$3da1,$3da7
	dc.w $3dae,$3db5,$3dbb,$3dc2,$3dc9,$3dcf,$3dd6,$3ddc
	dc.w $3de2,$3de9,$3def,$3df5,$3dfc,$3e02,$3e08,$3e0e
	dc.w $3e14,$3e1b,$3e21,$3e27,$3e2d,$3e33,$3e38,$3e3e
	dc.w $3e44,$3e4a,$3e50,$3e55,$3e5b,$3e61,$3e66,$3e6c
	dc.w $3e71,$3e77,$3e7c,$3e82,$3e87,$3e8c,$3e92,$3e97
	dc.w $3e9c,$3ea1,$3ea7,$3eac,$3eb1,$3eb6,$3ebb,$3ec0
	dc.w $3ec5,$3eca,$3ece,$3ed3,$3ed8,$3edd,$3ee1,$3ee6
	dc.w $3eeb,$3eef,$3ef4,$3ef8,$3efd,$3f01,$3f06,$3f0a
	dc.w $3f0e,$3f13,$3f17,$3f1b,$3f1f,$3f23,$3f27,$3f2b
	dc.w $3f2f,$3f33,$3f37,$3f3b,$3f3f,$3f43,$3f47,$3f4a
	dc.w $3f4e,$3f52,$3f55,$3f59,$3f5d,$3f60,$3f64,$3f67
	dc.w $3f6a,$3f6e,$3f71,$3f74,$3f78,$3f7b,$3f7e,$3f81
	dc.w $3f84,$3f87,$3f8a,$3f8d,$3f90,$3f93,$3f96,$3f99
	dc.w $3f9c,$3f9e,$3fa1,$3fa4,$3fa6,$3fa9,$3fac,$3fae
	dc.w $3fb1,$3fb3,$3fb5,$3fb8,$3fba,$3fbc,$3fbf,$3fc1
	dc.w $3fc3,$3fc5,$3fc7,$3fc9,$3fcb,$3fcd,$3fcf,$3fd1
	dc.w $3fd3,$3fd5,$3fd7,$3fd8,$3fda,$3fdc,$3fde,$3fdf
	dc.w $3fe1,$3fe2,$3fe4,$3fe5,$3fe7,$3fe8,$3fe9,$3feb
	dc.w $3fec,$3fed,$3fee,$3fef,$3ff0,$3ff1,$3ff2,$3ff3
	dc.w $3ff4,$3ff5,$3ff6,$3ff7,$3ff8,$3ff9,$3ff9,$3ffa
	dc.w $3ffb,$3ffb,$3ffc,$3ffc,$3ffd,$3ffd,$3ffe,$3ffe
	dc.w $3ffe,$3fff,$3fff,$3fff,$3fff,$3fff,$3fff,$3fff
	dc.w $4000,$3fff,$3fff,$3fff,$3fff,$3fff,$3fff,$3fff
	dc.w $3ffe,$3ffe,$3ffe,$3ffd,$3ffd,$3ffc,$3ffc,$3ffb
	dc.w $3ffb,$3ffa,$3ff9,$3ff9,$3ff8,$3ff7,$3ff6,$3ff5
	dc.w $3ff4,$3ff3,$3ff2,$3ff1,$3ff0,$3fef,$3fee,$3fed
	dc.w $3fec,$3feb,$3fe9,$3fe8,$3fe7,$3fe5,$3fe4,$3fe2
	dc.w $3fe1,$3fdf,$3fde,$3fdc,$3fda,$3fd8,$3fd7,$3fd5
	dc.w $3fd3,$3fd1,$3fcf,$3fcd,$3fcb,$3fc9,$3fc7,$3fc5
	dc.w $3fc3,$3fc1,$3fbf,$3fbc,$3fba,$3fb8,$3fb5,$3fb3
	dc.w $3fb1,$3fae,$3fac,$3fa9,$3fa6,$3fa4,$3fa1,$3f9e
	dc.w $3f9c,$3f99,$3f96,$3f93,$3f90,$3f8d,$3f8a,$3f87
	dc.w $3f84,$3f81,$3f7e,$3f7b,$3f78,$3f74,$3f71,$3f6e
	dc.w $3f6a,$3f67,$3f64,$3f60,$3f5d,$3f59,$3f55,$3f52
	dc.w $3f4e,$3f4a,$3f47,$3f43,$3f3f,$3f3b,$3f37,$3f33
	dc.w $3f2f,$3f2b,$3f27,$3f23,$3f1f,$3f1b,$3f17,$3f13
	dc.w $3f0e,$3f0a,$3f06,$3f01,$3efd,$3ef8,$3ef4,$3eef
	dc.w $3eeb,$3ee6,$3ee1,$3edd,$3ed8,$3ed3,$3ece,$3eca
	dc.w $3ec5,$3ec0,$3ebb,$3eb6,$3eb1,$3eac,$3ea7,$3ea1
	dc.w $3e9c,$3e97,$3e92,$3e8c,$3e87,$3e82,$3e7c,$3e77
	dc.w $3e71,$3e6c,$3e66,$3e61,$3e5b,$3e55,$3e50,$3e4a
	dc.w $3e44,$3e3e,$3e38,$3e33,$3e2d,$3e27,$3e21,$3e1b
	dc.w $3e14,$3e0e,$3e08,$3e02,$3dfc,$3df5,$3def,$3de9
	dc.w $3de2,$3ddc,$3dd6,$3dcf,$3dc9,$3dc2,$3dbb,$3db5
	dc.w $3dae,$3da7,$3da1,$3d9a,$3d93,$3d8c,$3d85,$3d7e
	dc.w $3d77,$3d70,$3d69,$3d62,$3d5b,$3d54,$3d4d,$3d45
	dc.w $3d3e,$3d37,$3d2f,$3d28,$3d21,$3d19,$3d12,$3d0a
	dc.w $3d02,$3cfb,$3cf3,$3cec,$3ce4,$3cdc,$3cd4,$3ccc
	dc.w $3cc5,$3cbd,$3cb5,$3cad,$3ca5,$3c9d,$3c95,$3c8c
	dc.w $3c84,$3c7c,$3c74,$3c6c,$3c63,$3c5b,$3c53,$3c4a
	dc.w $3c42,$3c39,$3c31,$3c28,$3c20,$3c17,$3c0e,$3c06
	dc.w $3bfd,$3bf4,$3beb,$3be2,$3bda,$3bd1,$3bc8,$3bbf
	dc.w $3bb6,$3bad,$3ba4,$3b9a,$3b91,$3b88,$3b7f,$3b75
	dc.w $3b6c,$3b63,$3b59,$3b50,$3b47,$3b3d,$3b34,$3b2a
	dc.w $3b20,$3b17,$3b0d,$3b03,$3afa,$3af0,$3ae6,$3adc
	dc.w $3ad2,$3ac8,$3abe,$3ab4,$3aaa,$3aa0,$3a96,$3a8c
	dc.w $3a82,$3a78,$3a6d,$3a63,$3a59,$3a4f,$3a44,$3a3a
	dc.w $3a2f,$3a25,$3a1a,$3a10,$3a05,$39fb,$39f0,$39e5
	dc.w $39da,$39d0,$39c5,$39ba,$39af,$39a4,$3999,$398e
	dc.w $3983,$3978,$396d,$3962,$3957,$394c,$3941,$3935
	dc.w $392a,$391f,$3913,$3908,$38fd,$38f1,$38e6,$38da
	dc.w $38cf,$38c3,$38b7,$38ac,$38a0,$3894,$3889,$387d
	dc.w $3871,$3865,$3859,$384d,$3841,$3835,$3829,$381d
	dc.w $3811,$3805,$37f9,$37ed,$37e0,$37d4,$37c8,$37bb
	dc.w $37af,$37a3,$3796,$378a,$377d,$3771,$3764,$3757
	dc.w $374b,$373e,$3731,$3725,$3718,$370b,$36fe,$36f1
	dc.w $36e5,$36d8,$36cb,$36be,$36b1,$36a4,$3696,$3689
	dc.w $367c,$366f,$3662,$3654,$3647,$363a,$362c,$361f
	dc.w $3612,$3604,$35f7,$35e9,$35dc,$35ce,$35c0,$35b3
	dc.w $35a5,$3597,$358a,$357c,$356e,$3560,$3552,$3544
	dc.w $3536,$3528,$351a,$350c,$34fe,$34f0,$34e2,$34d4
	dc.w $34c6,$34b7,$34a9,$349b,$348c,$347e,$3470,$3461
	dc.w $3453,$3444,$3436,$3427,$3419,$340a,$33fb,$33ed
	dc.w $33de,$33cf,$33c1,$33b2,$33a3,$3394,$3385,$3376
	dc.w $3367,$3358,$3349,$333a,$332b,$331c,$330d,$32fe
	dc.w $32ef,$32df,$32d0,$32c1,$32b1,$32a2,$3293,$3283
	dc.w $3274,$3264,$3255,$3245,$3236,$3226,$3216,$3207
	dc.w $31f7,$31e7,$31d8,$31c8,$31b8,$31a8,$3198,$3188
	dc.w $3179,$3169,$3159,$3149,$3139,$3128,$3118,$3108
	dc.w $30f8,$30e8,$30d8,$30c7,$30b7,$30a7,$3096,$3086
	dc.w $3076,$3065,$3055,$3044,$3034,$3023,$3013,$3002
	dc.w $2ff1,$2fe1,$2fd0,$2fbf,$2faf,$2f9e,$2f8d,$2f7c
	dc.w $2f6b,$2f5a,$2f49,$2f39,$2f28,$2f17,$2f05,$2ef4
	dc.w $2ee3,$2ed2,$2ec1,$2eb0,$2e9f,$2e8d,$2e7c,$2e6b
	dc.w $2e5a,$2e48,$2e37,$2e25,$2e14,$2e03,$2df1,$2de0
	dc.w $2dce,$2dbc,$2dab,$2d99,$2d88,$2d76,$2d64,$2d53
	dc.w $2d41,$2d2f,$2d1d,$2d0b,$2cf9,$2ce8,$2cd6,$2cc4
	dc.w $2cb2,$2ca0,$2c8e,$2c7c,$2c6a,$2c57,$2c45,$2c33
	dc.w $2c21,$2c0f,$2bfc,$2bea,$2bd8,$2bc6,$2bb3,$2ba1
	dc.w $2b8e,$2b7c,$2b6a,$2b57,$2b45,$2b32,$2b20,$2b0d
	dc.w $2afa,$2ae8,$2ad5,$2ac2,$2ab0,$2a9d,$2a8a,$2a77
	dc.w $2a65,$2a52,$2a3f,$2a2c,$2a19,$2a06,$29f3,$29e0
	dc.w $29cd,$29ba,$29a7,$2994,$2981,$296e,$295a,$2947
	dc.w $2934,$2921,$290e,$28fa,$28e7,$28d4,$28c0,$28ad
	dc.w $2899,$2886,$2873,$285f,$284c,$2838,$2824,$2811
	dc.w $27fd,$27ea,$27d6,$27c2,$27af,$279b,$2787,$2773
	dc.w $275f,$274c,$2738,$2724,$2710,$26fc,$26e8,$26d4
	dc.w $26c0,$26ac,$2698,$2684,$2670,$265c,$2648,$2634
	dc.w $261f,$260b,$25f7,$25e3,$25cf,$25ba,$25a6,$2592
	dc.w $257d,$2569,$2554,$2540,$252c,$2517,$2503,$24ee
	dc.w $24da,$24c5,$24b0,$249c,$2487,$2473,$245e,$2449
	dc.w $2434,$2420,$240b,$23f6,$23e1,$23cd,$23b8,$23a3
	dc.w $238e,$2379,$2364,$234f,$233a,$2325,$2310,$22fb
	dc.w $22e6,$22d1,$22bc,$22a7,$2292,$227d,$2267,$2252
	dc.w $223d,$2228,$2212,$21fd,$21e8,$21d3,$21bd,$21a8
	dc.w $2192,$217d,$2168,$2152,$213d,$2127,$2112,$20fc
	dc.w $20e7,$20d1,$20bb,$20a6,$2090,$207b,$2065,$204f
	dc.w $203a,$2024,$200e,$1ff8,$1fe2,$1fcd,$1fb7,$1fa1
	dc.w $1f8b,$1f75,$1f5f,$1f49,$1f34,$1f1e,$1f08,$1ef2
	dc.w $1edc,$1ec6,$1eb0,$1e99,$1e83,$1e6d,$1e57,$1e41
	dc.w $1e2b,$1e15,$1dff,$1de8,$1dd2,$1dbc,$1da6,$1d8f
	dc.w $1d79,$1d63,$1d4c,$1d36,$1d20,$1d09,$1cf3,$1cdc
	dc.w $1cc6,$1cb0,$1c99,$1c83,$1c6c,$1c56,$1c3f,$1c28
	dc.w $1c12,$1bfb,$1be5,$1bce,$1bb7,$1ba1,$1b8a,$1b73
	dc.w $1b5d,$1b46,$1b2f,$1b18,$1b02,$1aeb,$1ad4,$1abd
	dc.w $1aa6,$1a8f,$1a79,$1a62,$1a4b,$1a34,$1a1d,$1a06
	dc.w $19ef,$19d8,$19c1,$19aa,$1993,$197c,$1965,$194e
	dc.w $1937,$1920,$1908,$18f1,$18da,$18c3,$18ac,$1895
	dc.w $187d,$1866,$184f,$1838,$1820,$1809,$17f2,$17db
	dc.w $17c3,$17ac,$1794,$177d,$1766,$174e,$1737,$171f
	dc.w $1708,$16f1,$16d9,$16c2,$16aa,$1693,$167b,$1664
	dc.w $164c,$1634,$161d,$1605,$15ee,$15d6,$15be,$15a7
	dc.w $158f,$1577,$1560,$1548,$1530,$1519,$1501,$14e9
	dc.w $14d1,$14ba,$14a2,$148a,$1472,$145a,$1443,$142b
	dc.w $1413,$13fb,$13e3,$13cb,$13b3,$139b,$1383,$136c
	dc.w $1354,$133c,$1324,$130c,$12f4,$12dc,$12c4,$12ac
	dc.w $1294,$127c,$1263,$124b,$1233,$121b,$1203,$11eb
	dc.w $11d3,$11bb,$11a3,$118a,$1172,$115a,$1142,$112a
	dc.w $1111,$10f9,$10e1,$10c9,$10b0,$1098,$1080,$1068
	dc.w $104f,$1037,$101f,$1006,$0fee,$0fd6,$0fbd,$0fa5
	dc.w $0f8d,$0f74,$0f5c,$0f43,$0f2b,$0f13,$0efa,$0ee2
	dc.w $0ec9,$0eb1,$0e98,$0e80,$0e67,$0e4f,$0e36,$0e1e
	dc.w $0e05,$0ded,$0dd4,$0dbc,$0da3,$0d8b,$0d72,$0d59
	dc.w $0d41,$0d28,$0d10,$0cf7,$0cde,$0cc6,$0cad,$0c95
	dc.w $0c7c,$0c63,$0c4b,$0c32,$0c19,$0c01,$0be8,$0bcf
	dc.w $0bb6,$0b9e,$0b85,$0b6c,$0b54,$0b3b,$0b22,$0b09
	dc.w $0af1,$0ad8,$0abf,$0aa6,$0a8d,$0a75,$0a5c,$0a43
	dc.w $0a2a,$0a11,$09f9,$09e0,$09c7,$09ae,$0995,$097c
	dc.w $0964,$094b,$0932,$0919,$0900,$08e7,$08ce,$08b5
	dc.w $089d,$0884,$086b,$0852,$0839,$0820,$0807,$07ee
	dc.w $07d5,$07bc,$07a3,$078a,$0771,$0758,$073f,$0726
	dc.w $070d,$06f4,$06db,$06c2,$06a9,$0690,$0677,$065e
	dc.w $0645,$062c,$0613,$05fa,$05e1,$05c8,$05af,$0596
	dc.w $057d,$0564,$054b,$0532,$0519,$0500,$04e7,$04ce
	dc.w $04b5,$049c,$0483,$046a,$0451,$0437,$041e,$0405
	dc.w $03ec,$03d3,$03ba,$03a1,$0388,$036f,$0356,$033d
	dc.w $0323,$030a,$02f1,$02d8,$02bf,$02a6,$028d,$0274
	dc.w $025b,$0241,$0228,$020f,$01f6,$01dd,$01c4,$01ab
	dc.w $0192,$0179,$015f,$0146,$012d,$0114,$00fb,$00e2
	dc.w $00c9,$00af,$0096,$007d,$0064,$004b,$0032,$0019
	dc.w $0000,$ffe7,$ffce,$ffb5,$ff9c,$ff83,$ff6a,$ff51
	dc.w $ff37,$ff1e,$ff05,$feec,$fed3,$feba,$fea1,$fe88
	dc.w $fe6e,$fe55,$fe3c,$fe23,$fe0a,$fdf1,$fdd8,$fdbf
	dc.w $fda5,$fd8c,$fd73,$fd5a,$fd41,$fd28,$fd0f,$fcf6
	dc.w $fcdd,$fcc4,$fcaa,$fc91,$fc78,$fc5f,$fc46,$fc2d
	dc.w $fc14,$fbfb,$fbe2,$fbc9,$fbb0,$fb96,$fb7d,$fb64
	dc.w $fb4b,$fb32,$fb19,$fb00,$fae7,$face,$fab5,$fa9c
	dc.w $fa83,$fa6a,$fa51,$fa38,$fa1f,$fa06,$f9ed,$f9d4
	dc.w $f9bb,$f9a2,$f989,$f970,$f957,$f93e,$f925,$f90c
	dc.w $f8f3,$f8da,$f8c1,$f8a8,$f88f,$f876,$f85d,$f844
	dc.w $f82b,$f812,$f7f9,$f7e0,$f7c7,$f7ae,$f795,$f77c
	dc.w $f764,$f74b,$f732,$f719,$f700,$f6e7,$f6ce,$f6b5
	dc.w $f69d,$f684,$f66b,$f652,$f639,$f620,$f607,$f5ef
	dc.w $f5d6,$f5bd,$f5a4,$f58b,$f573,$f55a,$f541,$f528
	dc.w $f510,$f4f7,$f4de,$f4c5,$f4ad,$f494,$f47b,$f462
	dc.w $f44a,$f431,$f418,$f400,$f3e7,$f3ce,$f3b6,$f39d
	dc.w $f384,$f36c,$f353,$f33a,$f322,$f309,$f2f0,$f2d8
	dc.w $f2bf,$f2a7,$f28e,$f276,$f25d,$f244,$f22c,$f213
	dc.w $f1fb,$f1e2,$f1ca,$f1b1,$f199,$f180,$f168,$f14f
	dc.w $f137,$f11e,$f106,$f0ee,$f0d5,$f0bd,$f0a4,$f08c
	dc.w $f074,$f05b,$f043,$f02a,$f012,$effa,$efe1,$efc9
	dc.w $efb1,$ef99,$ef80,$ef68,$ef50,$ef37,$ef1f,$ef07
	dc.w $eeef,$eed7,$eebe,$eea6,$ee8e,$ee76,$ee5e,$ee45
	dc.w $ee2d,$ee15,$edfd,$ede5,$edcd,$edb5,$ed9d,$ed85
	dc.w $ed6d,$ed54,$ed3c,$ed24,$ed0c,$ecf4,$ecdc,$ecc4
	dc.w $ecac,$ec95,$ec7d,$ec65,$ec4d,$ec35,$ec1d,$ec05
	dc.w $ebed,$ebd5,$ebbd,$eba6,$eb8e,$eb76,$eb5e,$eb46
	dc.w $eb2f,$eb17,$eaff,$eae7,$ead0,$eab8,$eaa0,$ea89
	dc.w $ea71,$ea59,$ea42,$ea2a,$ea12,$e9fb,$e9e3,$e9cc
	dc.w $e9b4,$e99d,$e985,$e96d,$e956,$e93e,$e927,$e90f
	dc.w $e8f8,$e8e1,$e8c9,$e8b2,$e89a,$e883,$e86c,$e854
	dc.w $e83d,$e826,$e80e,$e7f7,$e7e0,$e7c8,$e7b1,$e79a
	dc.w $e783,$e76b,$e754,$e73d,$e726,$e70f,$e6f8,$e6e0
	dc.w $e6c9,$e6b2,$e69b,$e684,$e66d,$e656,$e63f,$e628
	dc.w $e611,$e5fa,$e5e3,$e5cc,$e5b5,$e59e,$e587,$e571
	dc.w $e55a,$e543,$e52c,$e515,$e4fe,$e4e8,$e4d1,$e4ba
	dc.w $e4a3,$e48d,$e476,$e45f,$e449,$e432,$e41b,$e405
	dc.w $e3ee,$e3d8,$e3c1,$e3ab,$e394,$e37e,$e367,$e351
	dc.w $e33a,$e324,$e30d,$e2f7,$e2e0,$e2ca,$e2b4,$e29d
	dc.w $e287,$e271,$e25a,$e244,$e22e,$e218,$e202,$e1eb
	dc.w $e1d5,$e1bf,$e1a9,$e193,$e17d,$e167,$e151,$e13a
	dc.w $e124,$e10e,$e0f8,$e0e2,$e0cd,$e0b7,$e0a1,$e08b
	dc.w $e075,$e05f,$e049,$e033,$e01e,$e008,$dff2,$dfdc
	dc.w $dfc7,$dfb1,$df9b,$df86,$df70,$df5a,$df45,$df2f
	dc.w $df19,$df04,$deee,$ded9,$dec3,$deae,$de99,$de83
	dc.w $de6e,$de58,$de43,$de2e,$de18,$de03,$ddee,$ddd8
	dc.w $ddc3,$ddae,$dd99,$dd84,$dd6e,$dd59,$dd44,$dd2f
	dc.w $dd1a,$dd05,$dcf0,$dcdb,$dcc6,$dcb1,$dc9c,$dc87
	dc.w $dc72,$dc5d,$dc48,$dc33,$dc1f,$dc0a,$dbf5,$dbe0
	dc.w $dbcc,$dbb7,$dba2,$db8e,$db79,$db64,$db50,$db3b
	dc.w $db27,$db12,$dafd,$dae9,$dad4,$dac0,$daac,$da97
	dc.w $da83,$da6e,$da5a,$da46,$da32,$da1d,$da09,$d9f5
	dc.w $d9e1,$d9cc,$d9b8,$d9a4,$d990,$d97c,$d968,$d954
	dc.w $d940,$d92c,$d918,$d904,$d8f0,$d8dc,$d8c8,$d8b4
	dc.w $d8a1,$d88d,$d879,$d865,$d852,$d83e,$d82a,$d816
	dc.w $d803,$d7ef,$d7dc,$d7c8,$d7b5,$d7a1,$d78e,$d77a
	dc.w $d767,$d753,$d740,$d72c,$d719,$d706,$d6f3,$d6df
	dc.w $d6cc,$d6b9,$d6a6,$d692,$d67f,$d66c,$d659,$d646
	dc.w $d633,$d620,$d60d,$d5fa,$d5e7,$d5d4,$d5c1,$d5ae
	dc.w $d59c,$d589,$d576,$d563,$d550,$d53e,$d52b,$d518
	dc.w $d506,$d4f3,$d4e1,$d4ce,$d4bb,$d4a9,$d496,$d484
	dc.w $d472,$d45f,$d44d,$d43a,$d428,$d416,$d404,$d3f1
	dc.w $d3df,$d3cd,$d3bb,$d3a9,$d397,$d384,$d372,$d360
	dc.w $d34e,$d33c,$d32a,$d318,$d307,$d2f5,$d2e3,$d2d1
	dc.w $d2bf,$d2ae,$d29c,$d28a,$d278,$d267,$d255,$d244
	dc.w $d232,$d220,$d20f,$d1fd,$d1ec,$d1db,$d1c9,$d1b8
	dc.w $d1a6,$d195,$d184,$d173,$d161,$d150,$d13f,$d12e
	dc.w $d11d,$d10c,$d0fb,$d0ea,$d0d9,$d0c8,$d0b7,$d0a6
	dc.w $d095,$d084,$d073,$d062,$d052,$d041,$d030,$d01f
	dc.w $d00f,$cffe,$cfed,$cfdd,$cfcc,$cfbc,$cfab,$cf9b
	dc.w $cf8a,$cf7a,$cf6a,$cf59,$cf49,$cf39,$cf28,$cf18
	dc.w $cf08,$cef8,$cee8,$ced8,$cec8,$ceb7,$cea7,$ce97
	dc.w $ce88,$ce78,$ce68,$ce58,$ce48,$ce38,$ce28,$ce19
	dc.w $ce09,$cdf9,$cdea,$cdda,$cdca,$cdbb,$cdab,$cd9c
	dc.w $cd8c,$cd7d,$cd6d,$cd5e,$cd4f,$cd3f,$cd30,$cd21
	dc.w $cd12,$cd02,$ccf3,$cce4,$ccd5,$ccc6,$ccb7,$cca8
	dc.w $cc99,$cc8a,$cc7b,$cc6c,$cc5d,$cc4e,$cc40,$cc31
	dc.w $cc22,$cc13,$cc05,$cbf6,$cbe7,$cbd9,$cbca,$cbbc
	dc.w $cbad,$cb9f,$cb90,$cb82,$cb74,$cb65,$cb57,$cb49
	dc.w $cb3a,$cb2c,$cb1e,$cb10,$cb02,$caf4,$cae6,$cad8
	dc.w $caca,$cabc,$caae,$caa0,$ca92,$ca84,$ca77,$ca69
	dc.w $ca5b,$ca4d,$ca40,$ca32,$ca24,$ca17,$ca09,$c9fc
	dc.w $c9ee,$c9e1,$c9d4,$c9c6,$c9b9,$c9ac,$c99e,$c991
	dc.w $c984,$c977,$c96a,$c95d,$c94f,$c942,$c935,$c928
	dc.w $c91c,$c90f,$c902,$c8f5,$c8e8,$c8db,$c8cf,$c8c2
	dc.w $c8b5,$c8a9,$c89c,$c88f,$c883,$c876,$c86a,$c85d
	dc.w $c851,$c845,$c838,$c82c,$c820,$c814,$c807,$c7fb
	dc.w $c7ef,$c7e3,$c7d7,$c7cb,$c7bf,$c7b3,$c7a7,$c79b
	dc.w $c78f,$c783,$c778,$c76c,$c760,$c754,$c749,$c73d
	dc.w $c731,$c726,$c71a,$c70f,$c703,$c6f8,$c6ed,$c6e1
	dc.w $c6d6,$c6cb,$c6bf,$c6b4,$c6a9,$c69e,$c693,$c688
	dc.w $c67d,$c672,$c667,$c65c,$c651,$c646,$c63b,$c630
	dc.w $c626,$c61b,$c610,$c605,$c5fb,$c5f0,$c5e6,$c5db
	dc.w $c5d1,$c5c6,$c5bc,$c5b1,$c5a7,$c59d,$c593,$c588
	dc.w $c57e,$c574,$c56a,$c560,$c556,$c54c,$c542,$c538
	dc.w $c52e,$c524,$c51a,$c510,$c506,$c4fd,$c4f3,$c4e9
	dc.w $c4e0,$c4d6,$c4cd,$c4c3,$c4b9,$c4b0,$c4a7,$c49d
	dc.w $c494,$c48b,$c481,$c478,$c46f,$c466,$c45d,$c453
	dc.w $c44a,$c441,$c438,$c42f,$c426,$c41e,$c415,$c40c
	dc.w $c403,$c3fa,$c3f2,$c3e9,$c3e0,$c3d8,$c3cf,$c3c7
	dc.w $c3be,$c3b6,$c3ad,$c3a5,$c39d,$c394,$c38c,$c384
	dc.w $c37c,$c374,$c36b,$c363,$c35b,$c353,$c34b,$c343
	dc.w $c33b,$c334,$c32c,$c324,$c31c,$c314,$c30d,$c305
	dc.w $c2fe,$c2f6,$c2ee,$c2e7,$c2df,$c2d8,$c2d1,$c2c9
	dc.w $c2c2,$c2bb,$c2b3,$c2ac,$c2a5,$c29e,$c297,$c290
	dc.w $c289,$c282,$c27b,$c274,$c26d,$c266,$c25f,$c259
	dc.w $c252,$c24b,$c245,$c23e,$c237,$c231,$c22a,$c224
	dc.w $c21e,$c217,$c211,$c20b,$c204,$c1fe,$c1f8,$c1f2
	dc.w $c1ec,$c1e5,$c1df,$c1d9,$c1d3,$c1cd,$c1c8,$c1c2
	dc.w $c1bc,$c1b6,$c1b0,$c1ab,$c1a5,$c19f,$c19a,$c194
	dc.w $c18f,$c189,$c184,$c17e,$c179,$c174,$c16e,$c169
	dc.w $c164,$c15f,$c159,$c154,$c14f,$c14a,$c145,$c140
	dc.w $c13b,$c136,$c132,$c12d,$c128,$c123,$c11f,$c11a
	dc.w $c115,$c111,$c10c,$c108,$c103,$c0ff,$c0fa,$c0f6
	dc.w $c0f2,$c0ed,$c0e9,$c0e5,$c0e1,$c0dd,$c0d9,$c0d5
	dc.w $c0d1,$c0cd,$c0c9,$c0c5,$c0c1,$c0bd,$c0b9,$c0b6
	dc.w $c0b2,$c0ae,$c0ab,$c0a7,$c0a3,$c0a0,$c09c,$c099
	dc.w $c096,$c092,$c08f,$c08c,$c088,$c085,$c082,$c07f
	dc.w $c07c,$c079,$c076,$c073,$c070,$c06d,$c06a,$c067
	dc.w $c064,$c062,$c05f,$c05c,$c05a,$c057,$c054,$c052
	dc.w $c04f,$c04d,$c04b,$c048,$c046,$c044,$c041,$c03f
	dc.w $c03d,$c03b,$c039,$c037,$c035,$c033,$c031,$c02f
	dc.w $c02d,$c02b,$c029,$c028,$c026,$c024,$c022,$c021
	dc.w $c01f,$c01e,$c01c,$c01b,$c019,$c018,$c017,$c015
	dc.w $c014,$c013,$c012,$c011,$c010,$c00f,$c00e,$c00d
	dc.w $c00c,$c00b,$c00a,$c009,$c008,$c007,$c007,$c006
	dc.w $c005,$c005,$c004,$c004,$c003,$c003,$c002,$c002
	dc.w $c002,$c001,$c001,$c001,$c001,$c001,$c001,$c001
	dc.w $c000,$c001,$c001,$c001,$c001,$c001,$c001,$c001
	dc.w $c002,$c002,$c002,$c003,$c003,$c004,$c004,$c005
	dc.w $c005,$c006,$c007,$c007,$c008,$c009,$c00a,$c00b
	dc.w $c00c,$c00d,$c00e,$c00f,$c010,$c011,$c012,$c013
	dc.w $c014,$c015,$c017,$c018,$c019,$c01b,$c01c,$c01e
	dc.w $c01f,$c021,$c022,$c024,$c026,$c028,$c029,$c02b
	dc.w $c02d,$c02f,$c031,$c033,$c035,$c037,$c039,$c03b
	dc.w $c03d,$c03f,$c041,$c044,$c046,$c048,$c04b,$c04d
	dc.w $c04f,$c052,$c054,$c057,$c05a,$c05c,$c05f,$c062
	dc.w $c064,$c067,$c06a,$c06d,$c070,$c073,$c076,$c079
	dc.w $c07c,$c07f,$c082,$c085,$c088,$c08c,$c08f,$c092
	dc.w $c096,$c099,$c09c,$c0a0,$c0a3,$c0a7,$c0ab,$c0ae
	dc.w $c0b2,$c0b6,$c0b9,$c0bd,$c0c1,$c0c5,$c0c9,$c0cd
	dc.w $c0d1,$c0d5,$c0d9,$c0dd,$c0e1,$c0e5,$c0e9,$c0ed
	dc.w $c0f2,$c0f6,$c0fa,$c0ff,$c103,$c108,$c10c,$c111
	dc.w $c115,$c11a,$c11f,$c123,$c128,$c12d,$c132,$c136
	dc.w $c13b,$c140,$c145,$c14a,$c14f,$c154,$c159,$c15f
	dc.w $c164,$c169,$c16e,$c174,$c179,$c17e,$c184,$c189
	dc.w $c18f,$c194,$c19a,$c19f,$c1a5,$c1ab,$c1b0,$c1b6
	dc.w $c1bc,$c1c2,$c1c8,$c1cd,$c1d3,$c1d9,$c1df,$c1e5
	dc.w $c1eb,$c1f2,$c1f8,$c1fe,$c204,$c20a,$c211,$c217
	dc.w $c21e,$c224,$c22a,$c231,$c237,$c23e,$c245,$c24b
	dc.w $c252,$c259,$c25f,$c266,$c26d,$c274,$c27b,$c282
	dc.w $c289,$c290,$c297,$c29e,$c2a5,$c2ac,$c2b3,$c2bb
	dc.w $c2c2,$c2c9,$c2d1,$c2d8,$c2df,$c2e7,$c2ee,$c2f6
	dc.w $c2fe,$c305,$c30d,$c314,$c31c,$c324,$c32c,$c334
	dc.w $c33b,$c343,$c34b,$c353,$c35b,$c363,$c36b,$c373
	dc.w $c37c,$c384,$c38c,$c394,$c39d,$c3a5,$c3ad,$c3b6
	dc.w $c3be,$c3c7,$c3cf,$c3d8,$c3e0,$c3e9,$c3f2,$c3fa
	dc.w $c403,$c40c,$c415,$c41e,$c426,$c42f,$c438,$c441
	dc.w $c44a,$c453,$c45c,$c466,$c46f,$c478,$c481,$c48b
	dc.w $c494,$c49d,$c4a7,$c4b0,$c4b9,$c4c3,$c4cc,$c4d6
	dc.w $c4e0,$c4e9,$c4f3,$c4fd,$c506,$c510,$c51a,$c524
	dc.w $c52e,$c538,$c542,$c54c,$c556,$c560,$c56a,$c574
	dc.w $c57e,$c588,$c592,$c59d,$c5a7,$c5b1,$c5bc,$c5c6
	dc.w $c5d1,$c5db,$c5e6,$c5f0,$c5fb,$c605,$c610,$c61b
	dc.w $c626,$c630,$c63b,$c646,$c651,$c65c,$c667,$c672
	dc.w $c67d,$c688,$c693,$c69e,$c6a9,$c6b4,$c6bf,$c6cb
	dc.w $c6d6,$c6e1,$c6ed,$c6f8,$c703,$c70f,$c71a,$c726
	dc.w $c731,$c73d,$c749,$c754,$c760,$c76c,$c777,$c783
	dc.w $c78f,$c79b,$c7a7,$c7b3,$c7bf,$c7cb,$c7d7,$c7e3
	dc.w $c7ef,$c7fb,$c807,$c813,$c820,$c82c,$c838,$c845
	dc.w $c851,$c85d,$c86a,$c876,$c883,$c88f,$c89c,$c8a8
	dc.w $c8b5,$c8c2,$c8cf,$c8db,$c8e8,$c8f5,$c902,$c90f
	dc.w $c91b,$c928,$c935,$c942,$c94f,$c95c,$c96a,$c977
	dc.w $c984,$c991,$c99e,$c9ac,$c9b9,$c9c6,$c9d4,$c9e1
	dc.w $c9ee,$c9fc,$ca09,$ca17,$ca24,$ca32,$ca40,$ca4d
	dc.w $ca5b,$ca69,$ca76,$ca84,$ca92,$caa0,$caae,$cabc
	dc.w $caca,$cad8,$cae6,$caf4,$cb02,$cb10,$cb1e,$cb2c
	dc.w $cb3a,$cb49,$cb57,$cb65,$cb74,$cb82,$cb90,$cb9f
	dc.w $cbad,$cbbc,$cbca,$cbd9,$cbe7,$cbf6,$cc05,$cc13
	dc.w $cc22,$cc31,$cc3f,$cc4e,$cc5d,$cc6c,$cc7b,$cc8a
	dc.w $cc99,$cca8,$ccb7,$ccc6,$ccd5,$cce4,$ccf3,$cd02
	dc.w $cd11,$cd21,$cd30,$cd3f,$cd4f,$cd5e,$cd6d,$cd7d
	dc.w $cd8c,$cd9c,$cdab,$cdbb,$cdca,$cdda,$cdea,$cdf9
	dc.w $ce09,$ce19,$ce28,$ce38,$ce48,$ce58,$ce68,$ce78
	dc.w $ce87,$ce97,$cea7,$ceb7,$cec7,$ced8,$cee8,$cef8
	dc.w $cf08,$cf18,$cf28,$cf39,$cf49,$cf59,$cf6a,$cf7a
	dc.w $cf8a,$cf9b,$cfab,$cfbc,$cfcc,$cfdd,$cfed,$cffe
	dc.w $d00f,$d01f,$d030,$d041,$d051,$d062,$d073,$d084
	dc.w $d095,$d0a6,$d0b7,$d0c7,$d0d8,$d0e9,$d0fa,$d10c
	dc.w $d11d,$d12e,$d13f,$d150,$d161,$d173,$d184,$d195
	dc.w $d1a6,$d1b8,$d1c9,$d1da,$d1ec,$d1fd,$d20f,$d220
	dc.w $d232,$d243,$d255,$d267,$d278,$d28a,$d29c,$d2ad
	dc.w $d2bf,$d2d1,$d2e3,$d2f5,$d307,$d318,$d32a,$d33c
	dc.w $d34e,$d360,$d372,$d384,$d396,$d3a9,$d3bb,$d3cd
	dc.w $d3df,$d3f1,$d403,$d416,$d428,$d43a,$d44d,$d45f
	dc.w $d471,$d484,$d496,$d4a9,$d4bb,$d4ce,$d4e0,$d4f3
	dc.w $d506,$d518,$d52b,$d53e,$d550,$d563,$d576,$d589
	dc.w $d59b,$d5ae,$d5c1,$d5d4,$d5e7,$d5fa,$d60d,$d620
	dc.w $d633,$d646,$d659,$d66c,$d67f,$d692,$d6a5,$d6b9
	dc.w $d6cc,$d6df,$d6f2,$d706,$d719,$d72c,$d740,$d753
	dc.w $d767,$d77a,$d78d,$d7a1,$d7b4,$d7c8,$d7dc,$d7ef
	dc.w $d803,$d816,$d82a,$d83e,$d851,$d865,$d879,$d88d
	dc.w $d8a0,$d8b4,$d8c8,$d8dc,$d8f0,$d904,$d918,$d92c
	dc.w $d940,$d954,$d968,$d97c,$d990,$d9a4,$d9b8,$d9cc
	dc.w $d9e0,$d9f5,$da09,$da1d,$da31,$da46,$da5a,$da6e
	dc.w $da83,$da97,$daac,$dac0,$dad4,$dae9,$dafd,$db12
	dc.w $db26,$db3b,$db50,$db64,$db79,$db8d,$dba2,$dbb7
	dc.w $dbcb,$dbe0,$dbf5,$dc0a,$dc1f,$dc33,$dc48,$dc5d
	dc.w $dc72,$dc87,$dc9c,$dcb1,$dcc6,$dcdb,$dcf0,$dd05
	dc.w $dd1a,$dd2f,$dd44,$dd59,$dd6e,$dd83,$dd99,$ddae
	dc.w $ddc3,$ddd8,$ddee,$de03,$de18,$de2d,$de43,$de58
	dc.w $de6e,$de83,$de98,$deae,$dec3,$ded9,$deee,$df04
	dc.w $df19,$df2f,$df45,$df5a,$df70,$df85,$df9b,$dfb1
	dc.w $dfc6,$dfdc,$dff2,$e008,$e01d,$e033,$e049,$e05f
	dc.w $e075,$e08b,$e0a1,$e0b6,$e0cc,$e0e2,$e0f8,$e10e
	dc.w $e124,$e13a,$e150,$e166,$e17d,$e193,$e1a9,$e1bf
	dc.w $e1d5,$e1eb,$e201,$e218,$e22e,$e244,$e25a,$e271
	dc.w $e287,$e29d,$e2b4,$e2ca,$e2e0,$e2f7,$e30d,$e324
	dc.w $e33a,$e350,$e367,$e37d,$e394,$e3aa,$e3c1,$e3d8
	dc.w $e3ee,$e405,$e41b,$e432,$e449,$e45f,$e476,$e48d
	dc.w $e4a3,$e4ba,$e4d1,$e4e8,$e4fe,$e515,$e52c,$e543
	dc.w $e55a,$e571,$e587,$e59e,$e5b5,$e5cc,$e5e3,$e5fa
	dc.w $e611,$e628,$e63f,$e656,$e66d,$e684,$e69b,$e6b2
	dc.w $e6c9,$e6e0,$e6f7,$e70f,$e726,$e73d,$e754,$e76b
	dc.w $e783,$e79a,$e7b1,$e7c8,$e7e0,$e7f7,$e80e,$e825
	dc.w $e83d,$e854,$e86b,$e883,$e89a,$e8b2,$e8c9,$e8e0
	dc.w $e8f8,$e90f,$e927,$e93e,$e956,$e96d,$e985,$e99c
	dc.w $e9b4,$e9cb,$e9e3,$e9fb,$ea12,$ea2a,$ea42,$ea59
	dc.w $ea71,$ea88,$eaa0,$eab8,$ead0,$eae7,$eaff,$eb17
	dc.w $eb2f,$eb46,$eb5e,$eb76,$eb8e,$eba6,$ebbd,$ebd5
	dc.w $ebed,$ec05,$ec1d,$ec35,$ec4d,$ec65,$ec7c,$ec94
	dc.w $ecac,$ecc4,$ecdc,$ecf4,$ed0c,$ed24,$ed3c,$ed54
	dc.w $ed6c,$ed84,$ed9d,$edb5,$edcd,$ede5,$edfd,$ee15
	dc.w $ee2d,$ee45,$ee5d,$ee76,$ee8e,$eea6,$eebe,$eed6
	dc.w $eeef,$ef07,$ef1f,$ef37,$ef50,$ef68,$ef80,$ef98
	dc.w $efb1,$efc9,$efe1,$effa,$f012,$f02a,$f043,$f05b
	dc.w $f073,$f08c,$f0a4,$f0bd,$f0d5,$f0ed,$f106,$f11e
	dc.w $f137,$f14f,$f168,$f180,$f199,$f1b1,$f1ca,$f1e2
	dc.w $f1fb,$f213,$f22c,$f244,$f25d,$f275,$f28e,$f2a7
	dc.w $f2bf,$f2d8,$f2f0,$f309,$f322,$f33a,$f353,$f36b
	dc.w $f384,$f39d,$f3b5,$f3ce,$f3e7,$f3ff,$f418,$f431
	dc.w $f449,$f462,$f47b,$f494,$f4ac,$f4c5,$f4de,$f4f7
	dc.w $f50f,$f528,$f541,$f55a,$f572,$f58b,$f5a4,$f5bd
	dc.w $f5d6,$f5ef,$f607,$f620,$f639,$f652,$f66b,$f684
	dc.w $f69c,$f6b5,$f6ce,$f6e7,$f700,$f719,$f732,$f74b
	dc.w $f763,$f77c,$f795,$f7ae,$f7c7,$f7e0,$f7f9,$f812
	dc.w $f82b,$f844,$f85d,$f876,$f88f,$f8a8,$f8c1,$f8da
	dc.w $f8f3,$f90c,$f925,$f93d,$f956,$f96f,$f988,$f9a1
	dc.w $f9bb,$f9d4,$f9ed,$fa06,$fa1f,$fa38,$fa51,$fa6a
	dc.w $fa83,$fa9c,$fab5,$face,$fae7,$fb00,$fb19,$fb32
	dc.w $fb4b,$fb64,$fb7d,$fb96,$fbaf,$fbc8,$fbe2,$fbfb
	dc.w $fc14,$fc2d,$fc46,$fc5f,$fc78,$fc91,$fcaa,$fcc3
	dc.w $fcdc,$fcf6,$fd0f,$fd28,$fd41,$fd5a,$fd73,$fd8c
	dc.w $fda5,$fdbe,$fdd8,$fdf1,$fe0a,$fe23,$fe3c,$fe55
	dc.w $fe6e,$fe87,$fea1,$feba,$fed3,$feec,$ff05,$ff1e
	dc.w $ff37,$ff50,$ff6a,$ff83,$ff9c,$ffb5,$ffce,$ffe7


	end
