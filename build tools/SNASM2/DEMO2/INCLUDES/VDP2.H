


;--------------------------------------------------------------------------->
__base__	EQUS	"_VDP2_REGS"

_DEQU		MACRO	*,addr
\*		 EQU	\addr
_\*		 EQU   \__base__+\addr
		ENDM


_BEQU		MACRO	*,val
\*		EQU	\val
_\*	EQU   \1<<\val
		ENDM

;þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ memory map þ

_VDP2_BASE	EQU	$25e00000		;Cache through address

_VDP2_VRAM	EQU	_VDP2_BASE+0
_VDP2_CRAM	EQU	_VDP2_BASE+$100000
_VDP2_REGS	EQU	_VDP2_BASE+$180000
;þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ registers þ
;
; (R)  - means READ Allowed
; (RO) - means READ Only
; (WO) - means WRITE only
;
TVMD	       		_DEQU		$00
SCL_DISPEN    		_BEQU		15
SCL_BDCLMD    		_BEQU		14
;þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ Interlace Mode  þ
_SCL_NON_INTER		EQU            	%0000000000000000
_SCL_SINGLE_INTER       EQU             %0000000010000000
_SCL_DOUBLE_INTER       EQU		%0000000011000000
;þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ Vertical Resolution þ
_SCL_224LINE		EQU		%0000000000000000
_SCL_240LINE		EQU		%0000000000010000
_SCL_256LINE		EQU		%0000000000100000

;þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ Cycle Pattern Access þ
NBG0_PN_R		EQU		0
NBG1_PN_R               EQU		1
NBG2_PN_R               EQU		2
NBG3_PN_R               EQU		3
NBG0_CP_R               EQU		4
NBG1_CP_R               EQU		5
NBG2_CP_R               EQU		6
NBG3_CP_R               EQU		7

NBG0_VCS_R              EQU		12
NBG1_VCS_R              EQU		13
CPU_R_W		        EQU		14
VCP_NA			EQU		15

VCP_T0			EQU		12
VCP_T1			EQU		8
VCP_T2			EQU		4
VCP_T3			EQU		0
VCP_T4			EQU		12
VCP_T5			EQU		8
VCP_T6			EQU		4
VCP_T7			EQU		0


EXTEN		_DEQU		$02		;External Signal Enable (R)
TVSTAT		_DEQU		$04		;Screen Status (RO)
VRSIZE		_DEQU		$06		;VRAM Size (R)
HCNT		_DEQU		$08             ;H-Counter (RO)
VCNT		_DEQU		$0A             ;V-Counter (RO)
;N/U		_DEQU		$0C             ;Reserve
RAMCTL		_DEQU		$0E             ;RAM Control (R)


;þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ map regs þ

CYCA0L		_DEQU		$10             ;VRAM Cycle Pattern Bank A0 Lo
CYCA0U		_DEQU		$12             ;VRAM Cycle Pattern Bank A0 Hi
CYCA1L		_DEQU		$14             ;VRAM Cycle Pattern Bank A1 Lo
CYCA1U		_DEQU		$16             ;VRAM Cycle Pattern Bank A1 Hi
CYCB0L		_DEQU		$18             ;VRAM Cycle Pattern Bank B0 Lo
CYCB0U		_DEQU		$1A             ;VRAM Cycle Pattern Bank B0 Hi
CYCB1L		_DEQU		$1C             ;VRAM Cycle Pattern Bank B1 Lo
CYCB1U		_DEQU		$1E             ;VRAM Cycle Pattern Bank B1 Hi

BGON		_DEQU		$20             ;Screen Display Enable
MZCTL		_DEQU		$22             ;Mosaic Control
SFSEL		_DEQU		$24             ;Special Function Code Select
SFCODE		_DEQU		$26             ;Special Function Code
CHCTLA		_DEQU		$28             ;Character Control (NBG0, NBG1)
CHCTLB		_DEQU		$2A             ;Character Control (NBG2, NBG3, RBG0)
BMPNA		_DEQU		$2C             ;Bitmap Palette Number (NBG0, NBG1)
BMPNB		_DEQU		$2E             ;Bitmap Palette Number (RBG0)


PNCN0		_DEQU		$30		;Pattern Name Control (NBG0)
PNCN1		_DEQU		$32		;Pattern Name Control (NBG1)
PNCN2		_DEQU		$34		;Pattern Name Control (NBG2)
PNCN3		_DEQU		$36		;Pattern Name Control (NBG3)
PNCR		_DEQU		$38		;Pattern Name Control (RBG0)
PLSZ		_DEQU		$3A		;Plane Size
MPOFN		_DEQU		$3C		;Map Offset (NBG0-NBG3)
MPOFR		_DEQU		$3E		;Map Offset (Rotation Parameter A,B)

MPABN0		_DEQU		$40		;Map (NBG0, Plane A,B)
MPCDN0		_DEQU		$42		;Map (NBG0, Plane C,D)
MPABN1		_DEQU		$44		;Map (NBG1, Plane A,B)
MPCDN1		_DEQU		$46		;Map (NBG1, Plane C,D)
MPABN2		_DEQU		$48		;Map (NBG2, Plane A,B)
MPCDN2		_DEQU		$4A		;Map (NBG2, Plane C,D)
MPABN3		_DEQU		$4C		;Map (NBG3, Plane A,B)
MPCDN3		_DEQU		$4E		;Map (NBG3, Plane C,D)

MPABRA		_DEQU		$50		;Map (Rotation Parameter A, Plane A,B)
MPCDRA		_DEQU		$52		;Map (Rotation Parameter A, Plane C,D)
MPEFRA		_DEQU		$54		;Map (Rotation Parameter A, Plane E,F)
MPGHRA		_DEQU		$56		;Map (Rotation Parameter A, Plane G,H)
MPIJRA		_DEQU		$58		;Map (Rotation Parameter A, Plane I,J)
MPKLRA		_DEQU		$5A		;Map (Rotation Parameter A, Plane K,L)
MPMNRA		_DEQU		$5C		;Map (Rotation Parameter A, Plane M,N)
MPOPRA		_DEQU		$5E		;Map (Rotation Parameter A, Plane O,P)

MPABRB		_DEQU		$60		;Map (Rotation Parameter B, Plane A,B)
MPCDRB		_DEQU		$62		;Map (Rotation Parameter B, Plane C,D)
MPEFRB		_DEQU		$64		;Map (Rotation Parameter B, Plane E,F)
MPGHRB		_DEQU		$66		;Map (Rotation Parameter B, Plane G,H)
MPIJRB		_DEQU		$68		;Map (Rotation Parameter B, Plane I,J)
MPKLRB		_DEQU		$6A		;Map (Rotation Parameter B, Plane K,L)
MPMNRB		_DEQU		$6C		;Map (Rotation Parameter B, Plane M,N)
MPOPRB		_DEQU		$6E		;Map (Rotation Parameter B, Plane O,P)

SCXIN0		_DEQU		$70		;Screen Scroll Value (NBG0, Horizontal Integer Part)
SCXDN0		_DEQU		$72		;Screen Scroll Value (NBG0, Horizontal Fractional Part)
SCYIN0		_DEQU		$74		;Screen Scroll Value (NBG0, Vertical Integer Part)
SCYDN0		_DEQU		$76		;Screen Scroll Value (NBG0, Vertical Fractional Part)
ZMXIN0		_DEQU		$78		;Coordinate Increment (NBG0, Horizontal Integer Part)
ZMXDN0		_DEQU		$7A		;Coordinate Increment (NBG0, Horizontal Frac Part)
ZMYIN0		_DEQU		$7C		;Coordinate Increment (NBG0, Vertical Integer Part)
ZMYDN0		_DEQU		$7E		;Coordinate Increment (NBG0, Vertical Frac Part)

SCXIN1		_DEQU		$80		;Screen Scroll Value (NBG1, Horizontal Integer Part)
SCXDN1		_DEQU		$82		;Screen Scroll Value (NBG1, Horizontal Fractional Part)
SCYIN1		_DEQU		$84		;Screen Scroll Value (NBG1, Vertical Integer Part)
SCYDN1		_DEQU		$86		;Screen Scroll Value (NBG1, Vertical Fractional Part)
ZMXIN1		_DEQU		$88		;Coordinate Increment (NBG1, Horizontal Integer Part)
ZMXDN1		_DEQU		$8A		;Coordinate Increment (NBG1, Horizontal Frac Part)
ZMYIN1		_DEQU		$8C		;Coordinate Increment (NBG1, Vertical Integer Part)
ZMYDN1		_DEQU		$8E		;Coordinate Increment (NBG1, Vertical Frac Part)

SCXN2		_DEQU		$90		;Screen Scroll Value (NBG2, Horizontal)
SCYN2		_DEQU		$92		;Screen Scroll Value (NBG2, Vertical)
SCXN3		_DEQU		$94		;Screen Scroll Value (NBG3, Horizontal)
SCYN3		_DEQU		$96		;Screen Scroll Value (NBG3, Vertical)
ZMCTL		_DEQU		$98		;Reduction enable
SCRCTL		_DEQU		$9A		;Line and Vertical Cell Scroll Control (NBG0, NBG1)
VCSTAU		_DEQU		$9C		;Vertical Cell Scroll Table Address (NBG0, NBG1) Upper
VCSTAL		_DEQU		$9E		;Vertical Cell Scroll Table Address (NBG0, NBG1) Lower

LSTA0U		_DEQU		$A0		;Line Scroll Table Address (NBG0) Upper
LSTA0L		_DEQU		$A2		;Line Scroll Table Address (NBG0) Lower
LSTA1U		_DEQU		$A4		;Line Scroll Table Address (NBG1) Upper
LSTA1L		_DEQU		$A6		;Line Scroll Table Address (NBG1) Lower
LCTAU		_DEQU		$A8		;Line Color Screen Table Address Upper
LCTAL		_DEQU		$AA		;Line Color Screen Table Address Lower
BKTAU		_DEQU		$AC		;Back Screen Table Address Upper
BKTAL		_DEQU		$AE		;Back Screen Table Address Lower

RPMD		_DEQU		$B0		;Rotation Parameter Mode
RPRCTL		_DEQU		$B2		;Rotation Parameter Read Control
KTCTL		_DEQU		$B4		;Coefficient Table Control
KTAOF		_DEQU		$B6		;Coefficient Address Offset (Rotation Parameter A, B)
OVPNRA		_DEQU		$B8		;Screen OVer Pattern Name (Rotation Parm A)
OVPNRB		_DEQU		$BA		;Screen OVer Pattern Name (Rotation Parm B)
RPTAU		_DEQU		$BC		;Rotation Param Table Address (Rotation) Upper
RPTAL		_DEQU		$BE		;Rotation Param Table Address (Rotation) Lower

WPSX0		_DEQU		$C0		;Window Position (W0, Horiz Start Point)
WPSY0		_DEQU		$C2		;Window Position (W0, Vert Start Point)
WPEX0		_DEQU		$C4		;Window Position (W0, Horiz End Point)
WPEY0		_DEQU		$C6		;Window Position (W0, Vert End Point)
WPSX1		_DEQU		$C8		;Window Position (W1, Horiz Start Point)
WPSY1		_DEQU		$CA		;Window Position (W1, Vert Start Point)
WPEX1		_DEQU		$CC		;Window Position (W1, Horiz End Point)
WPEY1		_DEQU		$CE		;Window Position (W1, Vert End Point)

WCTLA		_DEQU		$D0		;Window Control (NBG0, NBG1)
WCTLB		_DEQU		$D2		;Window Control (NBG2, NBG3)
WCTLC		_DEQU		$D4		;Window Control (RBG0, SPRITE)
WCTLD		_DEQU		$D6		;Window Control (Parameter Window, Color Calc. Window)
LWTA0U		_DEQU		$D8		;Line Window Table Address (W0) Upper
LWTA0L		_DEQU		$DA		;Line Window Table Address (W0) Lower
LWTA1U		_DEQU		$DC		;Line Window Table Address (W1) Upper
LWTA1L		_DEQU		$DE		;Line Window Table Address (W1) Lower

;þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ Sprite Control Reg þ
SPCTL		_DEQU		$E0		;Sprite Control
SCL_SPCLMD	_BEQU		5		;Sprite Color Mode: 0=Palette Format, 1=palette and RGB
SCL_SPWINEN	_BEQU		4		;Sprite Window Enable
_SCL_STYPE0	 EQU		%0000000000000000	;Sprite Types
_SCL_STYPE1	 EQU		%0000000000000001
_SCL_STYPE2	 EQU		%0000000000000010
_SCL_STYPE3	 EQU		%0000000000000011
_SCL_STYPE4	 EQU		%0000000000000100
_SCL_STYPE5	 EQU		%0000000000000101
_SCL_STYPE6	 EQU		%0000000000000110
_SCL_STYPE7	 EQU		%0000000000000111
_SCL_STYPE8	 EQU		%0000000000001000
_SCL_STYPE9	 EQU		%0000000000001001
_SCL_STYPEA	 EQU		%0000000000001010
_SCL_STYPEB	 EQU		%0000000000001011
_SCL_STYPEC	 EQU		%0000000000001100
_SCL_STYPED	 EQU		%0000000000001101
_SCL_STYPEE	 EQU		%0000000000001110
_SCL_STYPEF	 EQU		%0000000000001111

SDCTL		_DEQU		$E2		;Shadow Control
CRAOFA		_DEQU		$E4		;Color Ram Address Offset (NBG0-NBG3)
CRAOFB		_DEQU		$E6		;Color Ram Address Offset (RBG0, SPRITE)
LNCLEN		_DEQU		$E8		;Line Color Screen Enable
SFPRMD		_DEQU		$EA		;Special Priority Mode
CCCTL		_DEQU		$EC		;Color Calculation Control
SFCCMD		_DEQU		$EE		;Special Color Calculation Mode

PRISA		_DEQU		$F0		;Priority Number (Sprite 0,1)
PRISB		_DEQU		$F2		;Priority Number (Sprite 2,3)
PRISC		_DEQU		$F4		;Priority Number (Sprite 4,5)
PRISD		_DEQU		$F6		;Priority Number (Sprite 6,7)
PRINA		_DEQU		$F8		;Priority Number (NBG0, NBG1)
PRINB		_DEQU		$FA		;Priority Number (NBG2, NBG3)
PRIR		_DEQU		$FC		;Priority Number (RBG0)
;Reserve	_DEQU		$FE

CCRSA		_DEQU		$100		;Color Calculation Ratio (Sprite 0,1)
CCRSB		_DEQU		$102		;Color Calculation Ratio (Sprite 2,3)
CCRSC		_DEQU		$104		;Color Calculation Ratio (Sprite 4,5)
CCRSD		_DEQU		$106		;Color Calculation Ratio (Sprite 6,7)
CCRNA		_DEQU		$108		;Color Calculation (NBG0, NBG1)
CCRNB		_DEQU		$10A		;Color Calculation (NBG2, NBG3)
CCRR		_DEQU		$10C		;Color Calculation (RBG0)
CCRLB		_DEQU		$10E		;Color Calculation Ratio (Line Color Screen, Back Screen)

CLOFEN		_DEQU		$110		;Color Offset Enable
CLOFSL		_DEQU		$112		;Color Offset Select
COAR		_DEQU		$114		;Color Offset A (RED)
COAG		_DEQU		$116		;Color Offset A (GREEN)
COAB		_DEQU		$118		;Color Offset A (BLUE)
COBR		_DEQU		$11A		;Color Offset B (RED)
COBG		_DEQU		$11C		;Color Offset B (GREEN)
COBB		_DEQU		$11E		;Color Offset B (BLUE)

;þþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþþ MACROS þ
_LGBR		MACRO	val
		  mov.l	\val,temp_reg
		  ldc.l	temp_reg,gbr
		ENDM

_SetVDP2Reg	MACRO	reg,val
		 _move2reg.l	\val,r0
		  mov.l		#_VDP2_Regs_Shadow+\reg,r1
		  mov.l		#_VDP2_Regs+\reg,r2
		  mov.\0       	r0,(r1)
		  mov.\0       	r0,(r2)
		ENDM

_SetTVMD	MACRO	val
		 _SetVDP2Reg.w	TVMD,\val,\2
		ENDM

_ScreenOff	MACRO
		  mov.l		#_TVMD,temp_reg
		  mov.w		(temp_reg),r1
		  mov.l		#$007F,r2
		  and.l		r2,r1
		  mov.w		r1,@(temp_reg)
		ENDM

_ScreenOn	MACRO
		  mov.l		#_TVMD,temp_reg
		  mov.w		(temp_reg),r1
		  mov.l		#$8000,r2
		  or.l		r2,r1
		  mov.w		r1,@(temp_reg)
		ENDM
;
;temp macros used by SetPriority
;
_sptemp1	MACRO		val,reg
		    mov.l	#_VDP2_Regs_Shadow+\reg,r1
		    mov.w	(r1),r0
		    mov.l	#$FF00,r2
		    and.l	r2,r0
		    mov.l	#\val,r2
		    or.l	r2,r0
		    mov.l   	#_VDP2_Regs+\reg,r2
		    mov.l	r0,(r1)
		    mov.l	r0,(r2)
		ENDM

_sptemp2	MACRO		val,reg
		    mov.l	#_VDP2_Regs_Shadow+\reg,r1
		    mov.w	(r1),r0
		    mov.l	#$00FF,r2
		    and.l	r2,r0
		    mov.l	#\val,r2
		    shll8	r2
		    or.l	r2,r0
		    mov.l   	#_VDP2_Regs+\reg,r2
		    mov.l	r0,(r1)
		    mov.l	r0,(r2)
		ENDM

_SetPriority	MACRO		screen,val
		  if		stricmp('\1','nbg0')
		   _sptemp1	\val,PRINA
		  elseif	stricmp('\1','nbg1')
		   _sptemp2	\val,PRINA
		  elseif	stricmp('\1','nbg2')
		   _sptemp1	\val,PRINB
		  elseif	stricmp('\1','nbg3')
		   _sptemp2	\val,PRINB
		  elseif	stricmp('\1','rbg0')
		   _sptemp1	\val,PRIR
		  elseif	stricmp('\1','spr0')
		   _sptemp1	\val,PRISA
		  elseif	stricmp('\1','spr1')
		   _sptemp2	\val,PRISA
		  elseif	stricmp('\1','spr2')
		   _sptemp1	\val,PRISB
		  elseif	stricmp('\1','spr3')
		   _sptemp2	\val,PRISB
		  elseif	stricmp('\1','spr4')
		   _sptemp1	\val,PRISC
		  elseif	stricmp('\1','spr5')
		   _sptemp2	\val,PRISC
		  elseif	stricmp('\1','spr6')
		   _sptemp1	\val,PRISD
		  elseif	stricmp('\1','spr7')
		   _sptemp2	\val,PRISD
		  else
		   INFORM	3,"Illegal Screen Name %s",\screen
		  endif
		ENDM

_StoreVDP2Reg	MACRO	reg,val
		 global		_VDP2_Regs_Shadow
		 global		_VDP2_Regs
		_move2reg.l	\val,r1
		 mov.l		#_VDP2_Regs_Shadow+\reg,r2
		 mov.l		#_VDP2_Regs+\reg,r3
		 mov.\0		r1,(r2)
		 mov.\0		r1,(r3)
		ENDM

_OrVDP2Reg	MACRO	reg,val
		_move2reg.l	\val,r1			;r1 = value to or into register
		 mov.l		#_VDP2_Regs_Shadow+\reg,r2	;get VDP2 register Shadow base ptr
		 mov.l		#_VDP2_Regs+\reg,r3		;get VDP2 register base ptr
		 mov.\0		(r2),r4		;get shadow register value
		 or.l		r4,r1			;or value into shadow register
		 mov.\0		r1,(r2)		;store back shadow register
		 mov.\0		r1,(r3)		;store into hardware register
		ENDM

_AndVDP2Reg	MACRO	reg,val
		_move2reg.l	\val,r1			;r1 = value to or into register
		 mov.l		#_VDP2_Regs_Shadow+\reg,r2	;get VDP2 register Shadow base ptr
		 mov.l		#_VDP2_Regs+\reg,r3		;get VDP2 register base ptr
		 mov.\0		(r2),r4		;get shadow register value
		 and.l		r4,r1			;or value into shadow register
		 mov.\0		r1,(r2)		;store back shadow register
		 mov.\0		r1,(r3)		;store into hardware register
		ENDM

_XorVDP2Reg	MACRO	reg,val
		_move2reg.l	\val,r1			;r1 = value to or into register
		 mov.l		#_VDP2_Regs_Shadow+\reg,r2	;get VDP2 register Shadow base ptr
		 mov.l		#_VDP2_Regs+\reg,r3		;get VDP2 register base ptr
		 mov.\0		(r2),r4		;get shadow register value
		 xor.l		r4,r1			;or value into shadow register
		 mov.\0		r1,(r2)		;store back shadow register
		 mov.\0		r1,(r3)		;store into hardware register
		ENDM

_LoadVDP2Reg	MACRO	offset,reg
		 global	_VDP2_Regs_Shadow,_VDP2_Regs
		 if narg=1
		  mov.l	#_VDP2_Regs_Shadow+\offset,r1
		  mov.\0	(r1),r0
		 else
		  mov.l	#_VDP2_Regs_Shadow+\offset,r1
		 _move2reg.\0	(r1),\reg
		 endif
		 ENDM

_ReadVRAM2	MACRO	addr,reg
		_move2reg.l	\addr,r0
		 mov.l	#_VDP2_VRAM,r1
		 if narg=1
		  mov.\0	(r0,r1),r0
		 else
		  mov.\0	(r0,r1),\reg
		 endif
		ENDM

_WriteVRAM2	MACRO	addr,value
		_move2reg.l	\value,r4
		 mov.l		#_VDP2_VRAM,r1
		_move2reg.l	\addr,r0
		 mov.\0		r4,(r0,r1)
		ENDM

_vdpDataBegin	MACRO
		align	4
		ENDM

_vdpData	MACRO	offset,value
		 dc.w	\offset
		 dc.w	\value
		ENDM

_vdpDataEnd	MACRO
		 dc.l	-1
		ENDM



