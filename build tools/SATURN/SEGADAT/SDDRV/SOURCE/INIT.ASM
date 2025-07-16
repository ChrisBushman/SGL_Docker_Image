;=======================================================================;
;	SEGASATURN sound driver						;
; file name								;
;	init.asm							;
; copyright								;
;	SEGA ENTERPRISES						;
; purpose								;
;	Real time MIDI input control					;
; author(s)								;
;	1995-11-14	A.Miyazawa	Ver.2.00			;
; mod history								;
;	1996-02-23	A.Miyazawa	Ver.2.06			;
;	1996-07-02	A.Miyazawa	Ver.2.10L			;
;	1997-06-30	Y.Kashima	Ver.2.20			;
;=======================================================================;

		.include	SCSP.LIB

			.extern	get_BANK_addr
			.extern	level_2
			.extern	level_5
			.extern	level_6
			.extern	er_1F
			.extern	er_01
			.extern	er_0B
			.extern	er_0C
			.extern	er_0D
			.extern	er_0E
			.extern	er_0F
			.extern	er_10
			.extern	er_11
			.extern	er_12
			.extern	er_13
			.extern	er_14
			.extern	er_15
			.extern	er_16
			.extern	er_17
			.extern	er_18
			.extern	er_19
	.if ENGN		;
			.extern	ENGN_init_x
	.endif			;

;-----------------------------------------------------------------------;
;	vector table copy						;
;		don't rewrite scsi vector(level1:$64)			;
;-----------------------------------------------------------------------;
	.if	sw_MODEL_M
			.global	vector_tb_init
vector_tb_init:	lea	8,a1				;vector #02 address
		lea	vecter_tb(pc),a0		;copy source
		moveq	#30-1,d7			;
?loop:		move.l	(a0)+,(a1)+			;set vector#02Å`#31
		dbra	d7,?loop			;
		moveq	#16-1,d7			;
		jsr	vtbinit_sb(pc)			;set vector#32Å`#47
		moveq	#16-1,d7			;
		jsr	vtbinit_sb(pc)			;set vector#48Å`#63
		move.w	#192-1,d7			;
		jsr	vtbinit_sb(pc)			;set vector#64Å`#255
	;-------------------------------------------------------;
		lea	$000064,a0			;SCSI vector address
		lea	IO_SRAM1,a1			;SSbug Memory = E00000~E07FFF
		move.l	$000064,d1			;d1:scsi vector
		cmp.l	$64(a1),d1			;SSbug Vecter $64 equal ?
		bne	?skip				;
		move.l	#"SSBg",d0			;id "SSBg"
		cmp.l	$80(a1),d0			;id check
		bne	?skip				;
		move.l	d1,(a0)				;copy SSBug vector
		bra	?end				;
?skip:		move.l	$600064,(a0)			;copy ROM vector
?end:		rts					;
	;-------------------------------------------------------;
vtbinit_sb:	move.l	(a0)+,d0			;
?loop:		move.l	d0,(a1)+			;
		dbra	d7,?loop			;
		rts					;
	;-------------------------------------------------------;
vecter_tb:	dc.l	er_1F			; CPU vector#02 buss error
		dc.l	er_01			; CPU vector#03 address error
		dc.l	er_0E			; CPU vector#04 ïsìññΩóﬂ
		dc.l	er_0F			; CPU vector#05 ÇOèúéZ
		dc.l	er_10			; CPU vector#06 CHKñΩóﬂ
		dc.l	er_11			; CPU vector#07 TRAPVñΩóﬂ
		dc.l	er_12			; CPU vector#08 ì¡å†à·îΩ
		dc.l	er_13			; CPU vector#09 TRACEó·äOèàóù
		dc.l	er_14			; CPU vector#10 ñ¢é¿ëïñΩóﬂ
		dc.l	er_14			; 	//  #11    //
		dc.l	er_15			; CPU vector#12 reserved
		dc.l	er_15			;	//  #13	   //
		dc.l	er_15			;	//  #14	   //
		dc.l	er_16			; CPU vector#15 uninitialized
		dc.l	er_15			; CPU vector#16 reserved
		dc.l	er_15			;	//  #17	   //
		dc.l	er_15			;	//  #18	   //
		dc.l	er_15			;	//  #19	   //
		dc.l	er_15			;	//  #20	   //
		dc.l	er_15			;	//  #21	   //
		dc.l	er_15			;	//  #22	   //
		dc.l	er_15			;	//  #23	   //
		dc.l	er_17			; CPU vector#24 ÉXÉvÉäÉAÉX
VCTR_SCSI:	dc.l	0			; CPU vector#25 Level#1 SCSI
		dc.l	level_2			; 	    #26 Level#2 Timer-B
		dc.l	er_0B			; 	    #27 Level#3 no asigned
		dc.l	er_0C			; 	    #28 Level#4 no asigned
		dc.l	level_5			; 	    #29 Level#5 MIDI #1
		dc.l	level_6			; 	    #30 Level#6 MIDI #2
		dc.l	er_0D			;	    #31 Level#7 debugger

		dc.l	er_18			; CPU vector#32Å`47  TRAPñΩóﬂ
		dc.l	er_15			; CPU vector#48Å`63  reserved
		dc.l	er_19			; CPU vector#64Å`255 user vector

	.endif
;-----------------------------------------------------------------------;
;	midi i/o (YM3802) initialize					;
;-----------------------------------------------------------------------;
	.if	sw_MODEL_M
			global	YM3802_init
YM3802_init:	lea	IO_MIDI1+1,a0			;
		jsr	MCS_init(pc)			;
	;-------------------------------------------------------;
	;	set input direction I/O port : MCS-1 only >>>
	;-------------------------------------------------------;
		move.b	#9,R01(a0)			;
		move.b	#init_R94,Rx4(a0)		; R94 <-- all I/O input @001
		lea	IO_MIDI2+1,a0			;
	;-------------------------------------------------------;
	;	reset						;
	;-------------------------------------------------------;
MCS_init:	move.b	#$80,R01(a0)			;bit7 = H ( MCS initial clear )
		rol.w	#8,d0				;wait
		rol.w	#8,d0				;wait	(2 #22)
		move.b	#0,R01(a0)			;reset âèú
	;-------------------------------------------------------;
	;	set IRQ vector offset 0				
	;-------------------------------------------------------;
		move.b	#0,R01(a0)			;
		move.b	#init_R04,Rx4(a0)		; 04 <-- IRQ vector offset
	;-------------------------------------------------------;
	;	initial ëóéÛêM/FIFO
	;-------------------------------------------------------;
		move.b	#3,R01(a0)			;
RxBusy0:	move.b	Rx4(a0),d0			; R34 read
		lsr.b	#1,d0				; receive busy ?
		bcs.s	RxBusy0				; jump if busy
		move.b	#2,R01(a0)			;
		move.b	#init_R24,Rx4(a0)		; R24 <-- éÛêMrateê›íË
		move.b	#init_R25,Rx5(a0)		; R25 <-- éÛêMÉÇÅ[Éhê›íË
		move.b	#5,R01(a0)			;
TxBusy0:	move.b	Rx4(a0),d0			; R54 read
		lsr.b	#1,d0				; transmitter busy ?
		bcs.s	TxBusy0				;
		move.b	#4,R01(a0)			;
		move.b	#init_R44,Rx4(a0)		; R44 <-- ëóêMrateê›íË
		move.b	#init_R45,Rx5(a0)		; R45 <-- ëóêMÉÇÅ[Éhê›íË
		move.b	#3,R01(a0)			;
		move.b	#init_R35,Rx5(a0)		; R35 <-- FIFO-Rx ê›íË
		move.b	#5,R01(a0)			;
		move.b	#init_R55,Rx5(a0)		; R55 <-- FIFO-Tx ê›íË
		move.b	#0,R01(a0)			;
		move.b	#init_R06,Rx6(a0)		; IRQ enable select
		rts					;
	.endif
;-----------------------------------------------------------------------;
;	monitor work clear						;
;	in	none							;
;	out	none							;
;	break	a0,d0,d7
;-----------------------------------------------------------------------;
	.if	sw_MODEL_M
			.global	MONITOR_init
MONITOR_init:	lea	bs_MONTR,a0			;
		moveq	#slot_size-1,d7			;
		moveq	#0,d0				;
?slotLoop:	move.l	d0,(a0)+			;
		dbra	d7,?slotLoop			;
		rts					;
	.endif		;sw_MODEL_M

;-----------------------------------------------------------------------;
;	host interface work clear (for model m)				;
;	in	none							;
;	out	none							;
;	break	a0,d0,d7						;
;-----------------------------------------------------------------------;
	.if	sw_MODEL_M
			.global	HOST_IF_init
HOST_IF_init:	lea	host_if_work,a0			;host i/f($0700)
		moveq	#0,d0				;
		moveq	#256/4-1,d7			;
?hostIfLoop:	move.l	d0,(a0)+			;
		dbra	d7,?hostIfLoop			;
		move.w	#$000f,(scsp_mvol)		;
		rts					;
	.endif		;sw_MODEL_M

;-----------------------------------------------------------------------;
;Åyã@î\Åz[k_BANK_adr] initial						*
;	 [k_PRG_no],[k_MIDI_PAN],[k_PBend_BF]Ç 0 clear			*
;	in	a6:work topaddress($7000)				;
;-----------------------------------------------------------------------;
			.public		BANK_init
BANK_init:	movem.l	d0-d7/a0/a1,-(sp)		;
		movea.l	#management_work,a0		;($9900)
		moveq	#0,d5				;
		move.w	d5,_tmp_kanri(a6)		;
		move.w	d5,knr_kanri_ofst(a6)		;
		moveq	#7-1,d7				;
?manNumLoop:	moveq	#0,d1				;
		move.w	d1,up_down_flag(a0)		;
		move.w	d1,seq_volume_arrival(a0)	;
		move.w	d1,appointed_fade_rate(a0)	;
		move.w	d1,fade_count_variable(a0)	;
		move.w	#$0080,sequence_volume(a0)	;
		move.w	d1,performance_flag(a0)		;
		move.w	d5,d4				;
	;------- management work ($7000-) clear ----------------;
		moveq	#16-1,d6			;
?midiChLoop:	moveq	#default_volume,d1		;
		move.w	d1,midi_master_volume(a6,d4.w)	;
		move.w	d1,midi_volume(a6,d4.w)		;
		move.w	d1,midi_expression(a6,d4.w)	;
		move.w	d1,noteon_master_volume(a6,d4.w)
		move.w	d1,total_volume(a6,d4.w)	;
		move.w	a0,d1				;
		move.l	d1,management(a6,d4.w)		;
		addi.w	#$0020,d4			;next midi channel
		dbra	d6,?midiChLoop			;
	;-------------------------------------------------------;
		adda.w	#SQ_SIZ,a0			;next management work
		addi.w	#$0200,d5			;next management work(kanri_wk)
		dbra	d7,?manNumLoop			;
	;-------------------------------------------------------;
	;	management #7 work initialize			;
	;-------------------------------------------------------;
only_management_number7:
		lea	bs_AMAPC,a1			;current map area($0500)
		moveq	#32-1,d0			;
?mapSizeLoop:	move.b	(a1),d1				;tone bank exist check
		andi.b	#$70,d1				;
		beq.s	?toneBkExist			;
		addq.w	#8,a1				;next id check
		dbra	d0,?mapSizeLoop			;
		bset.b	#ERRb_2,Mem_err_bit+3		;<not find tone bank id>
		bra.s	?end				;
	;-------------------------------------------------------;
?toneBkExist:	move.l	(a1)+,d2			;
		andi.l	#$000fffff,d2			;start address
		moveq	#0,d1				;
		move.w	d1,up_down_flag(a0)		;
		move.w	d1,seq_volume_arrival(a0)	;
		move.w	d1,appointed_fade_rate(a0)	;
		move.w	d1,fade_count_variable(a0)	;
		move.w	#$0080,sequence_volume(a0)	;
		move.w	d1,performance_flag(a0)		;
		moveq	#16-1,d6			; loop size
		move.w	d5,d4				;
?loop:		move.l	d2,bank_address(a6,d4.w)	;
		sf.b	program_number(a6,d4.w)		;program #=0
		clr.w	knr_PBend_BF(a6,d4.w)		;pitch bend init
		moveq	#default_volume,d1		;default=$7f
		move.w	d1,midi_master_volume(a6,d4.w)	;
		move.w	d1,midi_volume(a6,d4.w)		;
		move.w	d1,midi_expression(a6,d4.w)	;
		move.w	d1,noteon_master_volume(a6,d4.w)
		move.w	d1,total_volume(a6,d4.w)	;
		move.w	a0,d1				;management #7 work($9ba0)
		move.l	d1,management(a6,d4.w)		;
		addi.w	#$0020,d4			;
		dbra	d6,?loop			;
?end:		movem.l	(sp)+,d0-d7/a0/a1		;
		rts					;

;-----------------------------------------------------------------------;
;	ÇrÇbÇrÇo âπ åπ èâ ä˙ âª						;
;	Åyì¸óÕÅz a5   : SCSP[FH1005] I/O addr				;
;	ÅyîjâÛÅz all							;
;-----------------------------------------------------------------------;
			.global	FH1005_init

FH1005_init:	move.b	#2,RG_M4DB(a5)		; SCSP 4Mbit D-RAM mode
		move.b	RG_MIBUF(a5),d0		; dammy read
		move.b	RG_MIBUF(a5),d0		;
		move.b	RG_MIBUF(a5),d0		;
		move.b	RG_MIBUF(a5),d0		;
		jsr	MPRO_all_clr(pc)	; DSP clear 95/02/20
		jsr	MADR_all_clr(pc)	;
		jsr	COEF_all_clr(pc)	;
		jsr	TEMP_all_clr(pc)	;
		jsr	SLOT_all_clr(pc)
		jsr	all_TL_off(pc)		;
		jsr	all_key_off(pc)		;

;		<<<< Master volume -0 dB >>>>
		move.b	#$0F,RG_MVOL(a5)	; Master Volume set 0 dB 
		move.w	#$000f,(scsp_mvol)


;		<<<< Timer ê›íË >>>>

		lea	SCSP_init_TB(pc),a0	;
		move.w	(a0)+,d0		;
		adda.w	d0,a5			;
		move.w	(a0)+,d7		; data size ( loop size )
FH1005_init_3:	move.b	(a0)+,(a5)+		;
		dbra	d7,FH1005_init_3	;
		lea	IO_SCSP,a5		;

		move.w	#TIMA_base,RG_TACTL(a5)	; 2msec [TACTL] & [TIMA]
		move.w	#$40,RG_SCIRE(a5)	; timer-A reset
		rts

			.global	iiii
SCSP_init_TB:	dc.w	RG_TBCTL		; transmit start Reg
		dc.w	SCSP_init_TB_e-SCSP_init_TB_s-1
SCSP_init_TB_s:	dc.w	TIMB_VL		;[TBCTL,TIMB] Timer-B value
		dc.w	TIMC_VL		;[TCCTL,TIMC] Timer-C value
iiii:		dc.w	INT_EN		;[SCIEB]      Interrupt EN
		dc.w	INT_RD		;[SCIPD]      status read ( dammy )
		dc.w	INT_RST		;[SCIRE]      Interrupt reset
		dc.w	SCSP_IPL0	;[SCILV0]
		dc.w	SCSP_IPL1	;[SCILV1]
		dc.w	SCSP_IPL2	;[SCILV2]
SCSP_init_TB_e:
		;-------------------------------;
			.global	all_TL_off
all_TL_off:	lea	SCSP_TLVL(a5),a0
		move.b	#$FF,d0
		bra.s	FH1005_init_x
		;-------------------------------;
			.global	all_key_off
all_key_off:	movea.l	a5,a0			;
		moveq	#%00010000,d0		;
FH1005_init_x:	moveq	#slot_size-1,d7		;
FH1005_init_2:	move.b	d0,(a0)			;
		lea	SCSP_slot_unit(a0),a0	;
		dbra	d7,FH1005_init_2	;
		rts
		;-------------------------------;
			.global	SLOT_all_clr
SLOT_all_clr:	movea.l	a5,a0			;
		moveq	#0,d0			;
		move.w	#$400/4-1,d7		; loop size
FH1005_init_1:	move.l	d0,(a0)+		;
		dbra	d7,FH1005_init_1	;
		rts
;-----------------------------------------------------------------------;
;	clear sequence mode & status					;
;-----------------------------------------------------------------------;
			.global	clr_sng_mdst
clr_sng_mdst:	moveq	#0,d0				;
		lea	host_if_work+OF_HI_STAT1,a0	;seq mode & status
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		rts
;-----------------------------------------------------------------------;
;	all slot rerease off						;
;-----------------------------------------------------------------------;
			.global	all_RR_off
all_RR_off:	lea	IO_SCSP,a5			;
		lea	SCSP_FH_RR(a5),a1		;
		moveq	#32-1,d7			;
?loop:		ori.b	#$1f,(a1)			;rerease off
		lea	SCSP_slot_unit(a1),a1		;next slot
		dbra	d7,?loop			;
		rts					;
;-----------------------------------------------------------------------;
;	input # & input mix level init.					;
;-----------------------------------------------------------------------;
			.global	IMXL_all_off
IMXL_all_off:	lea	SCSP_ISEL+1(a5),a1		;
		moveq	#32-1,d7			;
		moveq	#0,d0				;in# 0,level off
?loop:		move.b	d0,(a1)				;
		lea	SCSP_slot_unit(a1),a1		;next slot
		dbra	d7,?loop			;
		rts					;
;-----------------------------------------------------------------------;
;	effect send level & panpot init.				;
;-----------------------------------------------------------------------;
			.global	EFSDL_all_off
EFSDL_all_off:	lea	SCSP_EFSDLPN(a5),a1		;
		moveq	#32-1,d7			;
		moveq	#0,d0				;level 0,pan center
?loop:		move.b	d0,(a1)				;
		lea	SCSP_slot_unit(a1),a1		;next slot
		dbra	d7,?loop			;
		move.l	d0,mixer_wk_SCSP+0(a6)		;
		move.l	d0,mixer_wk_SCSP+4(a6)		;
		move.l	d0,mixer_wk_SCSP+8(a6)		;
		move.l	d0,mixer_wk_SCSP+12(a6)		;
		rts					;
;-----------------------------------------------------------------------;
;	clear dsp micro program area	($100800 - $100bff)		;
;-----------------------------------------------------------------------;
			.global	MPRO_all_clr
MPRO_all_clr:	lea	DSP_MPRO(a5),a1		;
		move.w	#$400/4-1,d7		;size
		bra.s	all_clr_lp		;0 clear
;-----------------------------------------------------------------------;
;	clear memory address regester	($100780 - $1007ff)		;
;-----------------------------------------------------------------------;
			.global	MADR_all_clr
MADR_all_clr:	lea	DSP_MADRS(a5),a1	;
		moveq	#$80/4-1,d7		;size
		bra.s	all_clr_lp		;0 clear
;-----------------------------------------------------------------------;
;	clear coef register		($100700 - $10077f)		;
;-----------------------------------------------------------------------;
			.global	COEF_all_clr
COEF_all_clr:	lea	DSP_COEF(a5),a1		;
		moveq	#$80/4-1,d7		;size
		bra.s	all_clr_lp		;0 clear
;-----------------------------------------------------------------------;
;	clear dsp buffer		($100c00 - $100dff)		;
;-----------------------------------------------------------------------;
			.global	TEMP_all_clr
TEMP_all_clr:	lea	DSP_TEMP(a5),a1		;
		moveq	#$200/4-1,d7		;size
	;-------------------------------------------------------;
all_clr_lp:	moveq	#0,d0			;0 clear
?loop:		move.l	d0,(a1)+		;
		dbra	d7,?loop		;
		rts				;
;-----------------------------------------------------------------------;
;			slot_work [sl_layer_adr]			
;Åyì¸óÕÅz a6 : work ram top addr				94/07/26*
;-----------------------------------------------------------------------;
			.global	sl_layer_adr_init
sl_layer_adr_init:
		moveq	#0,d5				;
		movea.l	bank_address(a6,d5.w),a1	;tone bank start address
		moveq	#0,d0				;
		move.w	BIN_VOICE(a1),d0		;voice 0 offset
		adda.l	d0,a1				;voice 0 data top
		addq.l	#4,a1				;layer 0 data top
		moveq	#slot_wk_unit,d1		;
		lea	slot_work+sl_layer_adr(a6),a4	;layer address store buf.
		moveq	#slot_size-1,d7			;
?slotLoop:	move.l	a1,(a4)				;layer address store
		adda.l	d1,a4				;next slot
		dbra	d7,?slotLoop			;
		rts					;

;-----------------------------------------------------------------------;
;			work RAM clear					*
;Åyì¸óÕÅz a6   : work ram top addr				94/07/26*
;-----------------------------------------------------------------------;
			.global	wk_RAM_init

wk_RAM_init:	move.l	#sz_Prg_wk/4-16,d7	; = 68k Program work size
		moveq	#0,d0			;	- stack area
		movea.l	a6,a0			;
wk_RAM_clr_lp:	move.l	d0,(a0)+		;
		dbra	d7,wk_RAM_clr_lp	;

		; <<<<< key HIStory initial >>>>>>

		moveq	#0,d2			;
		lea	_KEYHISTB(a6),a1	;
		moveq	#4-1,d7			; loop size
wk_RAM_2:	moveq	#8-1,d6			;
wk_RAM_1:	andi.w	#$3E,d2			;
		move.w	d0,(a1,d2.w)		;
		addi.w	#$40,d0			;
		addq.w	#8,d2			;
		dbra	d6,wk_RAM_1		;
		subi.w	#$3E,d2			;
		dbra	d7,wk_RAM_2		;

		move.w	#slot_size,off_slot_cnt(a6)

		lea	Pcm_Strm(a6),a3		; PCM stream# stack area
		moveq	#0,d0			;
		moveq	#16-1,d7		;
Pcm_Strm_lp:	move.b	d0,(a3)+		;
		addq.b	#1,d0			;
		dbra	d7,Pcm_Strm_lp		;

		lea	_MIDI_OUT_BF(a6),a2	;
		move.w	#$400/4-1,d7		; loop size
		moveq	#0,d0			;
MIDI_OUT_BF_lp:	move.l	d0,(a2)+		;
		dbra	d7,MIDI_OUT_BF_lp	;
	.if	ENGN
		lea	ENGN_wk_cmn(a6),a3		;
		move.l	d0,ENGN_wk_TONE(a3)		;clear tone start address
	.endif		;ENGN
		rts

;=======================================================================;
;	end of file							;
;=======================================================================;
