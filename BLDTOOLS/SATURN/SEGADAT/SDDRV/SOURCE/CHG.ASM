;=======================================================================;
;	SEGASATURN sound driver						;
; file name								;
;	chg.asm								;
; copyright								;
;	SEGA ENTERPRISES						;
; purpose								;
;	mode change							;
; author(s)								;
;	1995-11-14	A.Miyazawa	Ver.2.00			;
; mod history								;
;	1996-02-23	A.Miyazawa	Ver.2.06			;
;	1996-07-02	A.Miyazawa	Ver.2.10L			;
;	1997-06-30	Y.Kashima	Ver.2.20			;
;=======================================================================;

		.include	scsp.lib

			.extern	send_user		;
			.extern	BANK_init		;
			.extern	EFSDL_all_off		; MIXER[EFREG0Å`F] clear
			.extern	er_1B			;
			.extern	drv_10			;
			.extern	all_note_off		;
			.extern	noise_reduction		;
			.extern	master_volume_down	;
			.extern	master_volume_return	;
			.extern	er_08			;

;-----------------------------------------------------------------------;
;	#87	mixer change						;
;		P1:tone bank # ($00-$0f)				;
;		P2:mixer # ($00-$7f)					;
;	--------------------------------------------------------	;
;	in	a0	host interface pointer				;
;		d0	command code					;
;-----------------------------------------------------------------------;
			.public	drv_87
drv_87:		move.b	(a0)+,d0			;P1:tone bank #
		move.b	(a0)+,d3			;P2:mixer #
		andi.w	#$7F,d3				;
		move.w	d3,_activ_MIX_NO(a6)		;set to active mixer#
		jsr	get_BANK_addr(pc)		;read tone bank start address
		bcs	return_2			;<tone bank not found>
		move.l	d2,_activ_MIX_BNK(a6)		;set to active tone bank start adrs
		jmp	MIXER_wr(pc)			;mixer refresh

;-----------------------------------------------------------------------;
;	read start addres from tone bank #x				;
;	--------------------------------------------------------	;
;	in	d0	bank #						;
;	out	d2	start address					;
;	break	a0,d0,d2,d3,d7						;
;-----------------------------------------------------------------------;
			.public		get_BANK_addr
get_BANK_addr:	andi.w	#$000f,d0			; d0 = bank number
		lea	bs_AMAPC,a0			;current map area($0500)
		moveq	#32-1,d7			;area size
?areaSizeLoop:	move.b	(a0),d2				;
		bmi.s	er_IF_bit2			;<data id not found>
		move.b	d2,d3				;data id,id #
		andi.b	#$70,d2				;data id
		bne.s	?notToneBank			;
		andi.b	#$0F,d3				;id #
		cmp.b	d0,d3				;bank # check
		beq.s	?toneBankExist			;
?notToneBank:	lea	8(a0),a0			;next id id pointer
		dbra	d7,?areaSizeLoop		;
		bra.s	er_IF_bit2			;<data id unmatched>
	;------- 
?toneBankExist:	move.l	(a0)+,d2			;start address
		andi.l	#$000fffff,d2			;
		move.b	(a0),d3				;down load bit check
		bpl.s	er_IF_bit2			;<bank data not ready>
		clc					;
		rts					;
	;-------------------------------------------------------;
er_IF_bit2:	bset.b	#ERRb_2,Mem_err_bit+3		;
		stc					;
return_2:	rts					;

;-----------------------------------------------------------------------;
;	MIDI CC16	(control change $10:mixer change)		;
;	--------------------------------------------------------	;
;	in	a5	scsp register top				;
;		a6	management work top				;
;		d1.w	midi ch # ($00-$0f)				;
;		d3.b	mixer # ($00-$7f)				;
;-----------------------------------------------------------------------;
			.public	CTRL_10
CTRL_10:	move.w	_tmp_kanri(a6),d2		;management # x $200
		lsl.w	#5,d1				;
		add.w	d1,d2				;
		move.l	bank_address(a6,d2.w),d0	;tone bank start address
		beq	CTRL_10_er0			;<tone bank not found>
		move.l	d0,_activ_MIX_BNK(a6)		;set to active tone start adrs.
		andi.w	#$7F,d3				;
		move.w	d3,_activ_MIX_NO(a6)		;set to active mixer #
	;-------------------------------------------------------;
	;	mixer refresh					;
	;-------------------------------------------------------;
			.public	MIXER_wr
MIXER_wr:	move.l	_activ_MIX_BNK(a6),d0		;active tone bank start adrs.
		beq.s	er_IF_bit0			;<tone bank not found>
		movea.l	d0,a0				;
		move.w	_activ_MIX_NO(a6),d0		;active mixer #
		movea.l	a0,a1				;
		adda.w	BIN_VL(a0),a1			;velosity table top offset
		adda.w	BIN_MIX(a0),a0			;mixer data top
		;------- mixer # x 18 ------------------;
		add.w	d0,d0				;
		adda.w	d0,a0				;
		lsl.w	#3,d0				;
		adda.w	d0,a0				;mixer table top
		cmpa.l	a1,a0				;check address
		bcc.s	er_IF_bit0			;<illegal mixer #>
		;------- copy to work ------------------;
		lea	mixer_wk_SCSP(a6),a2		;mixer work
		move.l	(a0)+,(a2)+			;copy mixer data to work
		move.l	(a0)+,(a2)+			;
		move.l	(a0)+,(a2)+			;
		move.l	(a0)+,(a2)+			;
	;------- mixer refresh (set to scsp) -------------------;
			.public	MIXER_wk_wr
MIXER_wk_wr:	move.b	EFCT_CHG_CNT(a6),d0		;check efx mode
		bne.s	?mixUnrefresh			;now efx change
		lea	mixer_wk_SCSP(a6),a0		;mixer work
		moveq	#16-1,d7			;
		move.b	SND_OUT_ST(a6),d4		;check output mode
		bmi.s	mixMonaural			;
		;------- stereo mode -------------------;
		clr.w	d4				;
?mixInLoop:	move.b	(a0)+,SCSP_EFSDLPN(a5,d4.w)	;
		addi.w	#SCSP_slot_unit,d4		;
		dbra	d7,?mixInLoop			;
?mixUnrefresh:	rts					;
		;------- monaural mode -----------------;
mixMonaural:	clr.w	d4				;
?mixInLoop:	move.b	(a0)+,d0			;
		andi.b	#$E0,d0				;set center
		move.b	d0,SCSP_EFSDLPN(a5,d4.w)	;(direct pan)
		addi.w	#SCSP_slot_unit,d4		;
		dbra	d7,?mixInLoop			;
		clc					;
		rts					;
	;-------------------------------------------------------;
	;	mixer change error				;
	;-------------------------------------------------------;
er_IF_bit0:	bset.b	#ERRb_0,Mem_err_bit+3		;mixer error(not found/illegal#)
		jmp	EFSDL_all_off(pc)		;mixer initialize
	;-------------------------------------------------------;
CTRL_10_er0:	bset.b	#ERRb_3,Mem_err_bit+3		;tone bank not ready
		move.b	#ILLEGAL_MIXER_CHANGE,d0	;
		bsr	send_user			;set to host i/f work
		jmp	EFSDL_all_off(pc)		;mixer initialize

;-----------------------------------------------------------------------;
;	#08	map change						;
;		P1:map # ($00-$ff)					;
;	--------------------------------------------------------	;
;	in	a0	host interface pointer				;
;		d0	command code					;
;-----------------------------------------------------------------------;
			.public	seq_map
seq_map:	move.b	(a0)+,d0			;P1:map #
		andi.w	#$00ff,d0			;
		bsr	MAP_chg				;
		bsr	BANK_init			;
		clc					;
		rts					;

;-----------------------------------------------------------------------;
;	set map data to current map area				;
;	--------------------------------------------------------	;
;	in	a6	slot work($7000)				;
;		d0.w	map # ($00Å`$ff)				;
;-----------------------------------------------------------------------;
			.public	MAP_chg
MAP_chg:	bsr	all_note_off			;
		bsr	master_volume_down		;
		bsr	noise_reduction			;
	;------- current map area clear ------------------------;
		lea	bs_AMAPC,a0			;($0500)
		moveq	#$100/4-1,d7			;
		moveq	#0,d4				;
?mapClearLoop:	move.l	d4,(a0)+			;current map area clear
		dbra	d7,?mapClearLoop		;
	;------- clear tone bank start address work ------------;
		lea	bank_address(a6),a0		;tone bank start adrs work
		move.w	#8*16-1,d7			;management # x midi ch #
		moveq	#0,d4				;
?manWorkLoop:	move.l	d4,(a0)				;clear tone bank start adrs work
		lea	knr_unit(a0),a0			;
		dbra	d7,?manWorkLoop			;
	;------- clear slot flgas & layer address work ---------;
		lea	slot_work(a6),a0		;($8000)
		move.w	#slot_size-1,d7			;
		moveq	#0,d4				;
?slotLoop:	move.b	d4,sl_flag1(a0)			;clear slot flags 1
		move.b	d4,PSPN(a0)			;clear slot flags 2
		move.l	d4,sl_layer_adr(a0)		;clear layer abs adrs work
		lea	slot_wk_unit(a0),a0		;next slot
		dbra	d7,?slotLoop			;
	;------- search map data -------------------------------;
		lea	bs_AREAM,a0			;($a000)
		lea	bs_AREAM+sz_AREAM,a1		;($b000)
		andi.w	#$FF,d0				;
		beq	MAP_chg_ex			; jump if Map#0
		subq.w	#1,d0				;map # -1
?mapNumLoop:	move.b	(a0),d1				;data id,id #
		bmi.s	?nextMap			;one map end
		lea	8(a0),a0			;next unit
		cmpa.l	a0,a1				;check all map end
		bcs.s	er_IF_bit1			;<invalid map #>
		bra	?mapNumLoop			;
?nextMap:	addq.l	#1,a0				;
		dbra	d0,?mapNumLoop			;
	;------- copy to current map area ----------------------;
MAP_chg_ex:	lea	bs_AMAPC,a2			;current map area($0500)
		moveq	#256/8-1,d7			;max size
?mapSizeLoop:	move.b	(a0)+,(a2)+			;
		move.b	(a0)+,(a2)+			;
		move.b	(a0)+,(a2)+			;
		move.b	(a0)+,(a2)+			;
		move.b	(a0)+,(a2)+			;
		move.b	(a0)+,(a2)+			;
		move.b	(a0)+,(a2)+			;
		move.b	(a0)+,(a2)+			;
		dbra	d7,?mapSizeLoop			;
	;------- clear dsp program -----------------------------;
		lea	DSP_COEF(a5),a0			;($100700)
		move.w	#$500/4-1,d7			;program size
		moveq	#0,d0				;
?dspClrLoop:	move.l	d0,(a0)+			;
		dbra	d7,?dspClrLoop			;
	;------- 
	.if	ENGN
			.extern	ENGN_init		;trgt.asm
		bsr.w	ENGN_init			;
	.endif
	;------- 
		bsr	master_volume_return		;
		rts					;
	;-------------------------------------------------------;
	;	map change error				;
	;-------------------------------------------------------;
er_IF_bit1:	bset.b	#ERRb_1,Mem_err_bit+3		;<invalid map #>
		rts					;

;-----------------------------------------------------------------------;
;	MIDI CC32	(control change $20:bank change)		;
;	--------------------------------------------------------	;
;	in	a5	scsp register top				;
;		a6	management work top				;
;		d1.w	midi ch # ($00-$0f)				;
;		d3.b	bank # ($00-$7f)				;
;-----------------------------------------------------------------------;
			.public	CTRL_20
CTRL_20:	move.w	d3,d0				;
		bsr	get_BANK_addr			;read tone bank start address
		bcs	?error				;<tone bank not found>
		move.w	d1,d0				;midi chnannel #
		lsl.w	#5,d0				;
		add.w	_tmp_kanri(a6),d0		;(management # x$200)+(midi ch # x$20)
		move.l	d2,bank_address(a6,d0.w)	;set tone bank start address
	;-------------------------------------------------------;
		movem.l	d6/d7/a0/a1,-(sp)		;
		movea.l	d2,a0				;tone bank start address
		moveq	#0,d2				;
?programLoop:	moveq	#BIN_VOICE,d3			;voice address table offset
		add.w	d2,d3				;
		cmp.w	BIN_MIX(a0),d3			;check voice max #
		bcc	?end				;
		moveq	#0,d6				;
		move.w	(a0,d3.w),d6			;voice data offset address
		movea.l	a0,a1				;
		adda.l	d6,a1				;voice data absolute address
		moveq	#0,d7				;
		move.b	$02(a1),d7			;layer numbers
		lea.l	$04(a1),a1			;layer data #0 top address
	;------- 
		moveq	#0,d6				;
		move.l	d7,-(sp)			;
		moveq	#0,d0				;
?layerLoop0:	or.w	LY_MDL(a1,d6.w),d0		;moduration check
		addi.w	#$0020,d6			;next layer
		dbra	d7,?layerLoop0			;
		move.l	(sp)+,d7			;layer numbers
		tst.w	d0				;
		beq.s	?nextPrgVoice			;
	;------- 
		moveq	#0,d6				;
?layerLoop1:	ori.b	#$80,LY_base_note(a1,d6.w)	;base note check
		addi.w	#$0020,d6			;next layer
		dbra	d7,?layerLoop1			;
?nextPrgVoice:	addq.w	#2,d2				;next program voice table
		bra	?programLoop			;
	;------- bank change end -------------------------------;
?end:		movem.l	(sp)+,d6/d7/a0/a1		;
		clc					;
		rts					;
	;-------------------------------------------------------;
	;	bank change error				;
	;-------------------------------------------------------;
?error:		move.b	#BANK_NOT_FOUND,d0		;<tone bank not found>
		bsr	send_user			;set to host i/f work
		rts					;

;-----------------------------------------------------------------------;
;	midi program change						;
;	--------------------------------------------------------	;
;	in	a5	scsp register top				;
;		a6	management work top				;
;		d1.w	midi ch # ($00-$0f)				;
;		d2.b	program # ($00-$7f)				;
;	break	
;-----------------------------------------------------------------------;
			.public	PRG_chg
PRG_chg:	andi.w	#$007f,d2			;program #
		move.w	knr_kanri_ofst(a6),d0		;
		move.l	bank_address(a6,d0.w),d3	;tone bank start address
		beq	error1				;<tone bank not ready>
		movea.l	d3,a0				;
		moveq	#BIN_VOICE,d3			;voice address table offset
		add.w	d2,d3				;
		add.w	d2,d3				;voice #x data abs address ptr
		cmp.w	BIN_MIX(a0),d3			;check voice max #
		bcc	error2				;<invalid voice #>
		move.w	knr_kanri_ofst(a6),d0		;(management # x$200)+(midi ch# x$20)
		move.b	d2,program_number(a6,d0.w)	;set program #
		moveq	#0,d3				;
		move.w	d3,knr_PBend_BF(a6,d0.w)	;clear pitch bend work
		move.b	midi_panpot(a6,d0.w),d3		;check sequence pan flag
		btst	#6,d3				;
		bne.w	?seqPanOn			;
		andi.b	#$7F,d3				;
		move.b	d3,midi_panpot(a6,d0.w)		;
?seqPanOn:
	.if	sw_MODEL_M
		cmpi.w	#$E00,_tmp_kanri(a6)		;management # =7 ?
		bne	?end				;
		lea	bs_MONTR,a0			;work ram top for monitor
		add.w	d1,d1				;
		add.w	d1,d1				;
		andi.w	#$7C,d1				;
		move.b	d2,Mem_MVC(a0,d1.w)		;set program # to monitor work
?end:
	.endif
		clc					;
		rts					;
	;-------------------------------------------------------;
	;	program change error				;
	;-------------------------------------------------------;
error2:		move.b	#PROGRAM_NOT_FOUND,d0		;<program # not found>
		bsr	send_user			;
		bset.b	#ERRb_5,Mem_err_bit+3		;
		stc					;
		rts					;
	;-------------------------------------------------------;
error1:		move.b	#BANK_NOT_AVAIRABLE,d0		;<tone bank not ready>
		bsr	send_user			;
		bset.b	#ERRb_4,Mem_err_bit+3		;
		stc					;
		rts					;

;=======================================================================;
;	end of file							;
;=======================================================================;
