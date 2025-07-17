;------------------------------------------------
; equates       
;------------------------------------------------


;------------------------------------------------
;       Matrix data buffer assign       
;------------------------------------------------
MSTACK_SIZE					equ	8

MATRIX_DATA_SIZE        equ    4
MATRIX_SIZE             equ    MATRIX_DATA_SIZE*12

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





;*****************************************************************
; polygon data structure
;*****************************************************************

POLYSIZE	equ	56

p_px1		equ	0
p_py1		equ	4
p_pz1		equ	8

p_px2		equ	12
p_py2		equ	16
p_pz2		equ	20

p_px3		equ	24
p_py3		equ	28
p_pz3		equ	32

p_px4		equ	36
p_py4		equ	40
p_pz4		equ	44

p_bitmap	equ	48
p_flags		equ	52


