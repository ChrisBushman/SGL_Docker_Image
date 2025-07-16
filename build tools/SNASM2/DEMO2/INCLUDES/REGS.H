;****************************************************************
;	Saturn register equates
;
;	Copyright SEGA of America inc. 1994
;---------------------------------------------------------------*
;****************************************************************

;******************************************************************************
; SH2 divide registers
;******************************************************************************

divu_reg        equ    h'ffffff00      ; paralell divide base address

divu_jr         equ    h'00            ; Divisor register
divu_hrl32      equ    h'04            ; Dividend register L for 32 bit divide
divu_cont       equ    h'08            ; control register
divu_vct        equ    h'0c            ; Interupt vector register
divu_hrh        equ    h'10            ; Dividend High 32 bits
divu_hrl        equ    h'14            ; Dividend Low 32 bits
divu_rar        equ    h'18            ; Remainder register
divu_rsr        equ    h'1c            ; Quotient register


;******************************************************************************
; VDP1 system registers
;******************************************************************************

VDP1RAM		equ	$25C00000			; VDP1 VRAM

VDP1_FBUF	equ	$5C80000			; VDP1 framebuffer address in sh2 memory

VDP1REGS		equ	$25D00000			; VDP1 register base address
TVMR			equ	VDP1REGS			; tv mode register
FBCR			equ	VDP1REGS+2		; Frame buffer flip register
PTMR			equ	VDP1REGS+4		; Draw trigger register
EWDR			equ	VDP1REGS+6		; 
EWLR			equ	VDP1REGS+8		; 
EWRR			equ	VDP1REGS+10		; 
ENDR			equ	VDP1REGS+12		; 
EDSR			equ	VDP1REGS+16		; 
LOPR			equ	VDP1REGS+18		; 
COPR			equ	VDP1REGS+20		; 
MODR			equ	VDP1REGS+22		; 



;******************************************************************************
; SMPC controls registers
;******************************************************************************

PADADR 		equ	$20100070		; address of control pad registers
PAD1CTR 		equ	$09
PAD1DAT 		equ	$05

PAD2CTR 		equ	$0b
PAD2DAT 		equ	$07

SMPADMD 		equ	$0d

;   8button : 1101 0001 1111 1111 111L RXYZ SACB RLDU

PADUP			equ	$0001
PADDOWN		equ	$0002
PADLEFT		equ	$0004
PADRIGHT		equ	$0008

PADABUT		equ	$0040
PADBBUT		equ	$0010
PADCBUT		equ	$0020
PADSTART		equ	$0080

PADZBUT		equ	$0100
PADYBUT		equ	$0200
PADXBUT		equ	$0400
PADRBUT		equ	$0800
PADLBUT		equ	$1000


; boot rom function call address's

SYS_SETUINT		equ	$6000300

SYS_GETUINT		equ	$6000304

SYS_SETSINT		equ	$6000310
SYS_GETSINT		equ	$6000314

SYS_TASSEM		equ	$6000330
SYS_CLRSEM		equ	$6000334

SYS_SETSCUIM	equ	$6000340
SYS_CHGSCUIM	equ	$6000344
SYS_GETSCUIM	equ	$6000348

SYS_CHGSYSCK	equ	$6000320
SYS_GETSYSCK	equ	$6000324

SYS_CHGUIPR		equ	$6000280

SYS_EXECDMP		equ	$600026C

