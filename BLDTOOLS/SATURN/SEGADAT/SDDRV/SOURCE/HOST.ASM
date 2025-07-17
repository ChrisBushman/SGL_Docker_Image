;=======================================================================;
;	SEGASATURN sound driver						;
; file name								;
;	host.asm							;
; copyright								;
;	SEGA ENTERPRISES						;
; purpose								;
;	host command control						;
; author(s)								;
;	1995-11-14	A.Miyazawa	Ver.2.00			;
; mod history								;
;	1996-02-23	A.Miyazawa	Ver.2.06			;
;	1996-07-02	A.Miyazawa	Ver.2.10L			;
;	1997-06-30	Y.Kashima	Ver.2.20			;
;=======================================================================;

		.include	SCSP.LIB

			.extern	MIXER_wr		;chg
			.extern	MIXER_wk_wr		;chg
			.extern	drv_87			;chg
			.extern	seq_map			;chg
			.extern	IMXL_all_off		;init
			.extern	EFSDL_all_off		;init
			.extern	MPRO_all_clr		;init
			.extern	MADR_all_clr		;init
			.extern	COEF_all_clr		;init
			.extern	TEMP_all_clr		;init
			.extern	all_RR_off		;init
			.extern	clr_sng_mdst		;init
			.extern	FH1005_init		;init
			.extern	wk_RAM_init		;init
			.extern	CTRL_5B			;midi
			.extern	com_get_workaddress	;sdcom
			.extern	com_set_seqmode		;sdcom
			.extern	com_seq_noteoff		;sdcom
			.extern	all_damper_off		;sdcom
			.extern	send_user		;sdcom
			.extern	master_volume_down	;sdcom
			.extern	noise_reduction		;sdcom
			.extern	master_volume_return	;sdcom
			.extern	xseq_tempo		;sddrv
			.extern	xtempo_mode		;sddrv
			.extern	xtempo_ratio		;sddrv
			.extern	command_interpriter	;sdmain
			.extern	xseq_qsound		;sdmain
			.extern	xseq_YMH3Dw		;sdmain
			.extern	dsp3d_init		;dsp3d

;-----------------------------------------------------------------------;
;	in	a0	host interface work($700 to $770)		;
;		d0	command code					;
;	out	none							;
;-----------------------------------------------------------------------;
			.global	HOST_IF
HOST_IF:	addq.l	#2,a0				;a0 <--- P1 address
		movem.l	d0,-(SP)			;push
	;-------------------------------------------------------;
	;	command history store				;
	;-------------------------------------------------------;
		move.w	COMHIS_offset(a6),d0		;
		lea	bs_CMDHS,a1			;$0800
		andi.w	#$7f0,d0			;
		adda.w	d0,a1				;
		move.w	-2(a0),(a1)+			;counter set
		move.l	0(a0),(a1)+			;
		move.l	4(a0),(a1)+			;
		move.l	8(a0),(a1)+			;
		addi.w	#$10,d0				;
		move.w	d0,COMHIS_offset(a6)		;
		move.w	d0,Mem_CMDHS_pt			; HOST Commnd bufferring point
		move.w	COMHIS_cnt(a6),(a1)		;
		move.w	#0,COMHIS_cnt(a6)		;
	;------- end mark set ----------------------------------;
		andi.w	#$7f0,d0			;
		lea	bs_CMDHS,a1			;
		adda.w	d0,a1				;
		moveq	#-1,d0				;
		move.l	d0,(a1)+			;
		move.l	d0,(a1)+			;
		move.l	d0,(a1)+			;
		move.l	d0,(a1)+			;
	;-------------------------------------------------------;
		movem.l	(SP)+,d0			;pop
		move.w	d0,d7				;command
		andi.w	#$FF,d7				;
		move.b	d7,d7				;
		bpl.w	?skip				;command=$01 to $7f then skip
		cmpi.w	#$90+1,d7			;
		bcc.w	CM_ret				;command=over $90 then return
		andi.w	#$0f,d7				;
		add.w	d7,d7				;
		add.w	d7,d7				;
		jsr	CM_DRV(pc,d7.w)			;
		bra.w	CM_ret				;
	;-------------------------------------------------------;
?skip:		cmpi.w	#$20,d7				;
		bcc.w	CM_ret				;
		add.w	d7,d7				;
		add.w	d7,d7				;
		movea.l	a6,a4				;
		movem.l	a5/a6,-(SP)			;reg. push
		jsr	CM_SEQ(pc,d7.w)			;
		movem.l	(SP)+,a5/a6			;reg. pop
		rts					;
	;-------------------------------------------------------;
CM_ret:		clc					;
		rts					;
	;-------------------------------------------------------;
	;	command $80 - $8f jump table			;
	;-------------------------------------------------------;
CM_DRV:		jmp	drv_CDlvl(pc)			;#80 cd audio level
		jmp	drv_CDpan(pc)			;#81 cd audio panpot
		jmp	drv_vol(pc)			;#82 master volume
		jmp	drv_effect(pc)			;#83 effect change
		jmp	drv_84(pc)			;#84 
		jmp	drv_PCM_st(pc)			;#85 pcm stream play
		jmp	drv_PCM_ed(pc)			;#86 pcm stream stop
		jmp	drv_87(pc)			;#87 mixer change
		jmp	drv_88(pc)			;#88 mixer Parameter change
		jmp	drv_89(pc)			;#89 hard-Ware check
		jmp	drv_8A(pc)			;#8A pcm stream param change
		jmp	pcmSlotAlloc(pc)		;#8B pcm slot allocation
		jmp	pcmSlotRel(pc)			;#8C pcm slot release
		jmp	CM_ret(pc)			;#8D 
		jmp	CM_ret(pc)			;#8E 
		jmp	CM_ret(pc)			;#8F 
	;-------------------------------------------------------;
	;	command $00 - $1f jump table			;
	;-------------------------------------------------------;
CM_SEQ:		rts					;#00 
		nop					;
		jmp	seq_start(pc)			;#01 sequence play
		jmp	seq_stop(pc)			;#02 sequence stop
		jmp	seq_pause(pc)			;#03 sequence pause
		jmp	seq_cntnu(pc)			;#04 sequence resume
		jmp	sequence_volume_control(pc)	;#05 sequence volume
		jmp	seq_allstop(pc)			;#06 sequence all stop
		jmp	seq_tempo(pc)			;#07 sequence tempo
		jmp	seq_map(pc)			;#08 map change
		jmp	HOST_MIDI(pc)			;#09 
		jmp	seq_vlan_st(pc)			;#0A volume analize start
		jmp	seq_vlan_ed(pc)			;#0B volume analize stop
		jmp	CM_ret(pc)			;#0C 
		jmp	CM_ret(pc)			;#0D 
		jmp	SEQ_PAN(pc)			;#0E sequence panpot
		jmp	CM_ret(pc)			;#0F 
		jmp	drv_10(pc)			;#10 sound initial
		jmp	drv_11(pc)			;#11 3d sound
		jmp	drv_12(pc)			;#12 q sound
		jmp	drv_13(pc)			;#13 yamaha 3d init.
		jmp	seq_tp_mode(pc)			;#14 tempo mode
		jmp	seq_tp_ratio(pc)		;#15 tempo ratio
		jmp	CM_ret(pc)			;#16 
		jmp	CM_ret(pc)			;#17 
		jmp	CM_ret(pc)			;#18 
		jmp	CM_ret(pc)			;#19 
		jmp	CM_ret(pc)			;#1A 
		jmp	CM_ret(pc)			;#1B 
		jmp	CM_ret(pc)			;#1C 
		jmp	CM_ret(pc)			;#1D 
		jmp	CM_ret(pc)			;#1E 
	.if sw_MODEL_M
		jmp	voice_max_info(pc)		;#1F 
	.else		;sw_MODEL_M
		jmp	CM_ret(pc)			;#1F 
	.endif		;sw_MODEL_M
;-----------------------------------------------------------------------;
;	#01	sequence start						;
;		P1:management #						;
;		P2:sequence data bank #					;
;		P3:sequence data #					;
;		P4:priority level					;
;-----------------------------------------------------------------------;
			.public	seq_start
seq_start:	moveq	#0,d0				;
		move.b	(a0),d0				;P1:management # check
		cmpi.w	#$0007,d0			;
		bhi	?end				;illegal #
		bsr	pause_off			;pause reset
		bsr	all_damper_off			;damper reset
		movem.l	d0-d7/a0/a1,-(sp)		;
		movea.l	#management_work,a0		;$9900 sequence control block
		movea.l	a6,a1				;$7000
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		adda.w	d0,a0				;
		adda.w	d0,a0				;
		adda.w	d0,a0				;+$60
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		adda.w	d0,a1				;+$200
	;-------------------------------------------------------;
		moveq	#0,d1				;
		move.w	d1,_tmp_kanri(a6)		;now processing management#
		move.w	d1,knr_kanri_ofst(a6)		;
		move.w	d1,up_down_flag(a0)		;
		move.w	d1,seq_volume_arrival(a0)	;
		move.w	d1,appointed_fade_rate(a0)	;
		move.w	d1,fade_count_variable(a0)	;
		move.w	d1,performance_flag(a0)		;
	;-------
		moveq	#16-1,d7			;loop size
?midi_ch_loop:	moveq	#0,d1				;
		move.b	d1,program_number(a1)		;
		move.b	d1,midi_panpot(a1)		;
		move.b	d1,midi_flag(a1)		;
		move.l	d1,bank_address(a1)		;
		move.w	d1,knr_PBend_BF(a1)		;
		moveq	#default_volume,d1		;
		move.w	d1,midi_master_volume(a1)	;
		move.w	d1,midi_volume(a1)		;
		move.w	d1,midi_expression(a1)		;
		move.w	d1,noteon_master_volume(a1)	;
		move.w	d1,total_volume(a1)		;
		move.w	a0,d1				;
		move.l	d1,management(a1)		;
		adda.w	#$0020,a1			;
		dbra	d7,?midi_ch_loop		;
		movem.l	(sp)+,d0-d7/a0/a1		;
		move.b	(a0)+,d0			;P1:management #
		move.b	(a0)+,d2			;P2:bank #
		move.b	(a0)+,d3			;P3:song #
		move.b	d3,d1				;
		bsr	get_seq_top_adr			; set a2=sequence data address.
		bcs.s	?end				;if CF=1 then error.skip disposal,from now on.
		move.b	d1,d3				;
		move.b	(a0)+,d1			;P4:priority level
		move.b	(a0)+,d2			;P5:
		movea.l	a2,a0				;
		moveq	#start_command,d7		;
		bsr	command_interpriter		;sequence start
?end:		clr.w	(max_voice)			;
		clc					;
		rts					;

;-----------------------------------------------------------------------;
;	#02	sequence stop						;
;		P1:management #						;
;		P2:all release off flag					;
;-----------------------------------------------------------------------;
			.public	seq_stop
seq_stop:	moveq	#0,d0				;
		move.b	(a0)+,d0			;P1:management #
		cmpi.w	#$0007,d0			;
		bhi	?end				;
		movea.w	#host_if_work+OF_HI_STAT1+1,a1	;
		adda.w	d0,a1				;
		adda.w	d0,a1				;
		sf.b	(a1)				;status set $00
		bsr	pause_off			;pause reset
		bsr	all_damper_off			;damper reset
		move.b	(a0)+,d1			;P2:all release off flag
		move.w	d0,d3				;
		bsr	get_seq_top_adr			;set a2=sequence data address.
		movea.l	a2,a0				;
		movem.l	d0/d1/a6,-(sp)			;
		moveq	#stop_command,d7		;
		bsr	command_interpriter		;
		movem.l	(sp)+,d0/d1/a6			;
		move.b	d1,d1				;P2:check
		bpl.s	?end				;
	;-------------------------------------------------------;
	;	all release off					;
	;-------------------------------------------------------;
		lsl.b	#5,d0				;management number*0x20
		movea.l	#management_work,a1		;
		adda.w	d0,a1				;
		adda.w	d0,a1				;
		adda.w	d0,a1				;
		clr.w	up_down_flag(a1)		;
	.if	0
		move.w	sr,-(sp)			;
		ori.w	#$2700,sr			;
	;------- set midi data to buffer -----------------------;
		moveq	#0,d1				;
		move.b	_MIDI_RCV_WRPT(a6),d1		;midi receive write pointer
		addq.b	#1,_MIDI_RCV_WRPT(a6)		;pointer inc.
		lea	_MIDI_RCV_BF(a6),a0		;saturn midi receive buf
		add.w	d1,d1				;
		add.w	d1,d1				;
		adda.w	d1,a0				;
		move.b	#3,(a0)+			;priority=0,command=cc
		move.b	d0,(a0)+			;management #,midi ch #
		move.b	#$78,(a0)+			;control change#
		move.b	#$40,(a0)+			;with release off
		move.w	(sp)+,sr			;
	.endif
?end:		rts					;

;-----------------------------------------------------------------------;
;	#03	sequence pause						;
;		P1:management #						;
;-----------------------------------------------------------------------;
			.public	seq_pause
seq_pause:	moveq	#0,d0				;
		move.b	(a0)+,d0			;P1:management #
		cmpi.w	#$0007,d0			;
		bhi	?end				;
		move.l	d0,-(sp)			;
		movea.l	#management_work,a1		;
		lsl.w	#5,d0				;
		adda.w	d0,a1				;
		adda.w	d0,a1				;
		adda.w	d0,a1				; a1 = a1+management number*SQ_SIZ(96)
		st.b	pause_flag(a1)			;set $ff
		move.l	(sp)+,d0			;
		bsr	all_damper_off			;
		moveq	#pause_command,d7		;
		bsr	command_interpriter		;
		clc					;
?end:		rts					;

;-----------------------------------------------------------------------;
;	#04	sequence resume						;
;		P1:management #						;
;-----------------------------------------------------------------------;
			.public	seq_cntnu
seq_cntnu:	moveq	#0,d0				;
		move.b	(a0)+,d0			;P1:management #
		cmpi.w	#$0007,d0			;
		bhi	?end				;
		bsr.s	pause_off			;
		bsr	all_damper_off			;
		moveq	#continue_command,d7		;
		bsr	command_interpriter		;
		clc					;
?end:		rts					;
	;-------------------------------------------------------;
pause_off:	movem.l	d0/a1,-(sp)			;
		movea.l	#management_work,a1		;
		lsl.w	#5,d0				;
		adda.w	d0,a1				;
		adda.w	d0,a1				;
		adda.w	d0,a1				;a1 = a1+management number*SQ_SIZ(96)
		sf.b	pause_flag(a1)			;set $00
		movem.l	(sp)+,d0/a1			;
		rts					;

;-----------------------------------------------------------------------;
;	#05	sequence volume control					;
;		P1:management #						;
;		P2:sequence volume					;
;		P3:fade rate						;
;-----------------------------------------------------------------------;
sequence_volume_control:
		movem.l	a1/a2,-(sp)			;
		movea.l	#management_work,a1		;$9900
		movea.w	#mode_status,a2			;
		moveq	#0,d0				;
		moveq	#0,d1				;
		moveq	#0,d2				;
		move.b	(a0)+,d0			;P1:management #
		cmpi.w	#$0007,d0			;
		bhi	?end				;
		add.w	d0,d0				;
		adda.w	d0,a2				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		adda.w	d0,a1				;
		adda.w	d0,a1				;
		adda.w	d0,a1				;a1 = a1+management #*SQ_SIZ(96)
		lsl.w	#4,d0				; d0 = management number*0x200
		moveq	#0,d1				;
		move.b	(a0)+,d1			;P2:sequence volume
		move.w	d1,seq_volume_arrival(a1)	;
		move.b	(a0)+,d2			;P3:fade rate
		beq.s	?noFade				;volume change
		move.w	d2,appointed_fade_rate(a1)	;
		move.w	d2,fade_count_variable(a1)	;
		moveq	#0,d3				;
		moveq	#1,d2				;
		sub.w	sequence_volume(a1),d1		;ZF=1 is error,XF=1 is fade out,XF=0 is fade in.
		beq	?end				;same volume
		addx.w	d3,d3				;
		add.w	d3,d3				;
		sub.w	d3,d2				;
		move.w	d2,up_down_flag(a1)		;
		bra.s	?end				;
?noFade:	tst.w	d1				;
		beq.s	?end				;
		clr.w	up_down_flag(a1)		; òAë±ÉäÉNÉGÉXÉgópï‚ã≠ 1996/2/21
		bclr.b	#fade_bit,(a2)			;
		move.w	seq_volume_arrival(a1),d1	;
		move.w	d1,sequence_volume(a1)		;
		bsr	volume_calculate		;
?end:		movem.l	(sp)+,a1/a2			;
		clc					;
		rts					;

;-----------------------------------------------------------------------;
;	volume calculate						;
;	in	a1:management work					;
;		d0:management # *0x200					;
;	out	none							;
;	break	none							;
;-----------------------------------------------------------------------;
			.global	volume_calculate
volume_calculate:
		movem.l	d0-d7/a0/a1,-(sp)		;
		move.l	d0,d1				;
		moveq	#0,d2				;
		moveq	#16-1,d3			;
?midi_ch_loop:	move.w	total_volume(a6,d0.w),d6	;
		move.w	sequence_volume(a1),d5		;
		eori.w	#$0080,d5			;
		ext.w	d5				;
		add.w	d5,d6				;
		bmi.s	?flow				;
		cmpi.w	#$0100,d6			;
		bcc.s	?flow				;
		bra.s	?result				;
?flow:		tst.w	d6				;
		spl.b	d6				;
?result:	not.b	d6				;
		moveq	#slot_size-1,d7			;
		moveq	#0,d4				;
		lea	slot_work(a6),a0		;
?slot_loop:	cmp.w	_sl_kanri(a0,d4.w),d1		;management # check
		bne.s	?nextSlot			;
		cmp.b	sl_MIDI(a0,d4.w),d2		;midi channel # check
		bne.s	?nextSlot			;
		btst.b	#PCM_flg,PSPN(a0,d4.w)		;pcm stream play check
		bne.s	?nextSlot			;
	.if ENGN
		btst.b	#ENGN_flg,sl_flag1(a0,d4.w)	;engine control use check
		bne.s	?nextSlot			;
	.endif
		btst.b	#flg_FMCR,sl_flag2(a0,d4.w)	;fm carrier check
		beq.s	?nextSlot			;
		;---------------------------------------;
		lsr.w	#1,d4				;
		move.b	d6,SCSP_TLVL(a5,d4.w)		;set to scsp
		add.w	d4,d4				;
?nextSlot:	addi.w	#slot_wk_unit,d4		;+$40
		dbra	d7,?slot_loop			;
		addi.w	#$0020,d0			;
		addq.w	#1,d2				;
		dbra	d3,?midi_ch_loop		;
		movem.l	(sp)+,d0-d7/a0/a1		;
		rts					;

;-----------------------------------------------------------------------;
;	#06	sequence all stop					;
;-----------------------------------------------------------------------;
			.global	seq_allstop
seq_allstop:	bsr	xseq_allstop			;all sequence all note off
		bsr	all_RR_off			;all slot RR off
		bsr	init_sequence_work		;
		clc					;
		rts					;

;-----------------------------------------------------------------------;
;	#07	tempo change						;
;-----------------------------------------------------------------------;
			.global	seq_tempo
seq_tempo:	move.b	(a0)+,d0			;P1:management #
		move.b	(a0)+,d1			;P2:dummy
		move.w	(a0)+,d2			;P3-4:tempo value
		jsr	xseq_tempo(pc)			;
		clc					;
		rts					;
;-----------------------------------------------------------------------;
;	#09	midi direct						;
;	in	
;-----------------------------------------------------------------------;
			.global	HOST_MIDI
HOST_MIDI:	lea	bs_PWKTP,a6			;program work top($7000)
	sr_push						;
	int_di						;
		clr.w	d0				;
		move.b	_MIDI_RCV_WRPT(a6),d0		;write offset pointer
		addq.b	#1,_MIDI_RCV_WRPT(a6)		;set new write offset pointer
		add.w	d0,d0				;
		add.w	d0,d0				;
		lea	_MIDI_RCV_BF(a6),a6		;saturn midi receive buf
		adda.w	d0,a6				;
		move.l	(a0)+,(a6)+			;P1-4:midi data
	sr_pop						;
		clc					;
		rts					;
;-----------------------------------------------------------------------;
;	#0A	volume analize start					;
;-----------------------------------------------------------------------;
seq_vlan_st:	bsr	com_get_workaddress		;a4=9800/a5=9c00/a6=9900
		bset.b	#SY_VANCTL_BITPOS,SY_TVOLCTL(a4)	; set volume analize bit
		clc					;
		rts					;
;-----------------------------------------------------------------------;
;	#0B	volume analize stop					;
;-----------------------------------------------------------------------;
seq_vlan_ed:	bsr	com_get_workaddress		;a4=9800/a5=9c00/a6=9900
		bclr.b	#SY_VANCTL_BITPOS,SY_TVOLCTL(a4)	; clear volume analize bit
		clc					;
		rts					;
;-----------------------------------------------------------------------;
;	#0E	sequence pan control					;
;-----------------------------------------------------------------------;
			global	SEQ_PAN
SEQ_PAN:	lea	bs_PWKTP,a6			;68K prg work top address
		move.w	(a0),d0				;P1:management #
		andi.w	#$700,d0			;
		add.w	d0,d0				;000,200,400,..,E00
		move.b	1(a0),d1			;P2:panpot data
		bpl.s	SEQ_PAN_ON			;
	;-------------------------------------------------------;
	;	panpot control off				;
	;-------------------------------------------------------;
SEQ_PAN_OFF:	lea	kanri_wk+midi_panpot(a6,d0.w),a0
		moveq	#16-1,d7			;
		moveq	#$3F,d0				;set layer pan on & seq pan off
?midiChLoop:	and.b	d0,(a0)				;
		lea	knr_unit(a0),a0			;
		dbra	d7,?midiChLoop			;
		rts					;
	;-------------------------------------------------------;
	;	panpot control on				;
	;-------------------------------------------------------;
SEQ_PAN_ON:	lsr.b	#2,d1				;
		subi.b	#$10,d1				;
		bcc.s	?skip				;
		not.b	d1				;
		addi.b	#$10,d1				;
?skip:		move.b	d1,d3				;
		ori.b	#$C0,d1				;set layer pan off & seq pan on
		lea	kanri_wk+midi_panpot(a6,d0.w),a1
		moveq	#16-1,d7			;
?midiChLoop:	move.b	d1,(a1)				;set pan flags & data
		lea	knr_unit(a1),a1			;next management work
		dbra	d7,?midiChLoop			;
	;-------------------------------------------------------;
	;	set panpot data					;
	;-------------------------------------------------------;
		move.b	SND_OUT_ST(a6),d4		;stereo/monaural flag
		bpl.s	?stereo				;
		moveq	#0,d3				;center
?stereo:	lea	slot_work(a6),a0		;
		moveq	#slot_size-1,d7			;
		moveq	#0,d4				;reg initial
?slotLoop:	cmp.w	_sl_kanri(a0,d4.w),d0		;management # check
		bne.s	?nextSlot			;
		btst.b	#PCM_flg,PSPN(a0,d4.w)		;pcm stream use check
		bne.s	?nextSlot			;
	;-------
		lsr.w	#1,d4				;
		move.b	SCSP_DISDLPN(a5,d4.w),d2	;read scsp
		andi.b	#$E0,d2				;mask direct level[DISDL}
		or.b	d3,d2				;
		move.b	d2,SCSP_DISDLPN(a5,d4.w)	;set to scsp
		add.w	d4,d4				;
		move.b	d2,sl_DISDLPAN(a0,d4.w)		;set to work
?nextSlot:	addi.w	#slot_wk_unit,d4		;
		dbra	d7,?slotLoop			;
		rts					;

;-----------------------------------------------------------------------;
;	#10	Sound initial						;
;-----------------------------------------------------------------------;
			.global	drv_10
drv_10:	sr_push						;
	int_di						;
		move.b	(a0)+,d0			;P1:all seq stop
		subq.b	#1,d0				;
		bne.s	?p2Check			;
	;------- all sequence stop -----------------------------;
		movem.l	a0,-(SP)			;
		movem.l	a5,-(SP)			;
		jsr	xseq_allstop(pc)		;
		movem.l	(SP)+,a5			;
		jsr	all_RR_off(pc)			;all slot RR off
		jsr	clr_sng_mdst(pc)		;clear seq mode & status
		movem.l	(SP)+,a0			;
	;-------------------------------------------------------;
?p2Check:	move.b	(a0)+,d0			;P2:all pcm stream stop
		subq.b	#1,d0				;
		bne.s	?p3Check			;
	;------- all pcm stream stop ---------------------------;
		moveq	#8-1,d1				;loop size
		moveq	#0,d7				;d7:management #
?loop:		jsr	PCM_strm_stop(pc)		;pcm stream stop
		addq.w	#2,d7				;
		dbra	d1,?loop			;
	;-------------------------------------------------------;
?p3Check:	move.b	(a0)+,d0			;P3:stop cdaudio
		subq.b	#1,d0				;
		bne.s	?p4Check			;
	;------- stop cd audio ---------------------------------;
		moveq	#0,d0				;
		move.b	d0,SCSP_EFSDLPN+SCSP_slot_unit*16(a5)	;cd audio ch0 level off
		move.b	d0,SCSP_EFSDLPN+SCSP_slot_unit*17(a5)	;cd audio ch1 level off
	;-------------------------------------------------------;
?p4Check:	lea	bs_PWKTP,a6			;
		lea	IO_SCSP,a5			;
		move.b	(a0)+,d0			;P4:dsp stop
		subq.b	#1,d0				;
		bne.s	?p5Check			;
	;------- dsp stop --------------------------------------;
		jsr	IMXL_all_off(pc)		;input # & level init.
		jsr	EFSDL_all_off(pc)		;efx send level & pan init.
		jsr	MPRO_all_clr(pc)		;clear dsp micro prg area
		jsr	MADR_all_clr(pc)		;clear madr reg.
		jsr	COEF_all_clr(pc)		;clear coef reg.
		jsr	TEMP_all_clr(pc)		;clear dsp buffer
		jsr	MIXER_wk_wr(pc)			;mixer resume
	;-------------------------------------------------------;
?p5Check:	move.b	(a0)+,d0			;P5:mixer init
		subq.b	#1,d0				;
		bne.s	?end				;
	;------- mixer initialize ------------------------------;
		jsr	EFSDL_all_off(pc)		;efx send level & pan init.
		bsr.s	init_sequence_work		;
	;-------------------------------------------------------;
?end:	sr_pop						;
		rts					;
	;-------------------------------------------------------;
	;	
	;-------------------------------------------------------;
init_sequence_work:
		movem.l	d0-d7/a0/a1,-(sp)		
		movea.l	#management_work,a0		
		movea.l	a6,a1				
		moveq	#0,d1				
		move.w	d1,_tmp_kanri(a6)		
		move.w	d1,knr_kanri_ofst(a6)		
		moveq	#8-1,d6				
?man_num_loop:	moveq	#0,d1				
		move.w	d1,up_down_flag(a0)		
		move.w	d1,seq_volume_arrival(a0)	
		move.w	d1,appointed_fade_rate(a0)	
		move.w	d1,fade_count_variable(a0)	
		move.w	d1,sequence_volume(a0)		
		move.w	d1,performance_flag(a0)		
		move.w	d1,pause_flag(a0)		
		moveq	#16-1,d7			; loop size
?midi_ch_loop:	moveq	#0,d1				;
		move.b	d1,program_number(a1)		;
		move.b	d1,midi_panpot(a1)		;
		move.b	d1,midi_flag(a1)		;
		move.l	d1,bank_address(a1)		;
		move.w	d1,knr_PBend_BF(a1)		;
		moveq	#default_volume,d1		;
		move.w	d1,midi_master_volume(a1)	;
		move.w	d1,midi_volume(a1)		;
		move.w	d1,midi_expression(a1)		;
		move.w	d1,noteon_master_volume(a1)	;
		move.w	d1,total_volume(a1)		;
		move.w	a0,d1				;
		move.l	d1,management(a1)		;
		adda.w	#$0020,a1			;
		dbra	d7,?midi_ch_loop		;
		adda.w	#SQ_SIZ,a0			;
		dbra	d6,?man_num_loop		;
		movem.l	(sp)+,d0-d7/a0/a1		;
		rts					;

;-----------------------------------------------------------------------;
;	all sequence stop(management #0 to #7				;
;	in	none							;
;	out	none							;
;	break	a0,a1,a2,a3,a4,a5,a6,d0,d1,d2,d3,d4,d5,d6,d7		;
;-----------------------------------------------------------------------;
xseq_allstop:	bsr	com_get_workaddress		;a4=9800/a5=9c00/a6=9900
		move.w	#SQ_NUM-1,d6			;management size
?loop:		move.b	#$00,(a6)			;clear CB_CTL
		move.b	#1,d7				; 1:control off
		bsr	com_seq_noteoff			;write midi all note off event
		move.b	#SQ_MODE_STOP,CB_MODE(a6)	;stop mode=$00
		move.b	#RET_NORMAL,CB_STAT(a6)		;normal status=$00
		bsr	com_set_seqmode			;sequence status set($780)
		adda.l	#SQ_SIZ,a6			;next management #
		dbra.w	d6,?loop			;
		rts					;

;-----------------------------------------------------------------------;
;	#11	y-sound control						;
;		P1:distance						;
;		P2:azimuth						;
;		P3:elevation						;
;-----------------------------------------------------------------------;
			.global	drv_11,drv_13
drv_11:		move.b	(a0)+,d0			;P1:distance
		move.b	(a0)+,d1			;P2:azimuth
		move.b	(a0)+,d2			;P3:elevation
		bsr	xseq_YMH3Dw			;
		rts					;

;-----------------------------------------------------------------------;
;	#12	q-sound control						;
;		P1:channel						;
;		P2:pan							;
;-----------------------------------------------------------------------;
			.global	drv_12
drv_12:		move.b	(a0)+,d0			;P1:channel
		move.b	(a0)+,d1			;P2:pan
		bsr	xseq_qsound			;
		rts					;

;-----------------------------------------------------------------------;
;	#13	y-sound initial						;
;-----------------------------------------------------------------------;
drv_13:		bsr	dsp3d_init			;3D initialize
		rts					;

;-----------------------------------------------------------------------;
;	#14	tempo mode set						;
;-----------------------------------------------------------------------;
			.global	seq_tp_mode
seq_tp_mode:	move.b	(a0)+,d0			; P1 ready d0 : ( 0 Å` 7 )
		move.b	(a0)+,d1			; P2 mode
		jsr	xtempo_mode(pc)			; ready a6,d0,d1
		clc					;
		rts					;

;-----------------------------------------------------------------------;
;	#15	tempo ratio set						;
;-----------------------------------------------------------------------;
			.global	seq_tp_ratio
seq_tp_ratio:	move.b	(a0)+,d0			; P1 ready d0 : ( 0 Å` 7 )
		move.b	(a0)+,d1			; P2 dummy
		move.w	(a0)+,d2			; P3,P4 ready d2
		jsr	xtempo_ratio(pc)		; ready a6,d0,d1
		clc					;
		rts					;

;-----------------------------------------------------------------------;
;	#1f	retern number of note on voices				;
;-----------------------------------------------------------------------;
	.if sw_MODEL_M
voice_max_info:	move.w	(max_voice),d0			;
		bsr	send_user			;
		rts					;
	.endif

;-----------------------------------------------------------------------;
;	#80	cd audio level control					;
;		P1:cd audio level 1st channel(left)			;
;		P2:cd audio level 2nd channel(right)			;
;-----------------------------------------------------------------------;
			.global	drv_CDlvl
drv_CDlvl:	move.b	(a0)+,d0			;P1:cd level (left)
		andi.b	#$E0,d0				;
		move.b	mixer_wk_SCSP+16(a6),d2		;read work
		andi.b	#$1F,d2				;mask panpot
		or.b	d2,d0				;
		move.b	SND_OUT_ST(a6),d1		;stereo/monaural flag
		bpl.s	?stereoL			;
		andi.b	#$E0,d0				;center
?stereoL:	move.b	d0,mixer_wk_SCSP+16(a6)		;set to work
		move.b	d0,$200+SCSP_EFSDLPN(a5)	;set to scsp
	;------- 2nd channel (right) ---------------------------;
		move.b	(a0)+,d0			;P2:cd level (right)
		andi.b	#$E0,d0				;
		move.b	mixer_wk_SCSP+17(a6),d2		;read work
		andi.b	#$1F,d2				;mask panpot
		or.b	d2,d0				;
		move.b	d1,d1				;stereo/monaural flag
		bpl.s	?stereoR			;
		andi.b	#$E0,d0				;center
?stereoR:	move.b	d0,mixer_wk_SCSP+17(a6)		;set to work
		move.b	d0,$220+SCSP_EFSDLPN(a5)	;set to scsp
		rts					;

;-----------------------------------------------------------------------;
;	#81	cd audio panpot control					;
;		P1:cd audio panpot 1st channel(left)			;
;		P2:cd audio panpot 2nd channel(right)			;
;-----------------------------------------------------------------------;
			.global	drv_CDpan
drv_CDpan:	move.b	(a0)+,d0			;P1:cd panpot (left)
		andi.b	#$1F,d0				;
		move.b	mixer_wk_SCSP+16(a6),d2		;read work
		andi.b	#$E0,d2				;mask level
		or.b	d2,d0				;
		move.b	SND_OUT_ST(a6),d1		;stereo/monaural flag
		bpl.s	?stereoL			;
		andi.b	#$E0,d0				;center
?stereoL:	move.b	d0,mixer_wk_SCSP+16(a6)		;set to work
		move.b	d0,$200+SCSP_EFSDLPN(a5)	;set to scsp
	;------- 2nd channel (right) ---------------------------;
		move.b	(a0)+,d0			;P2:cd panpot right
		andi.b	#$1F,d0				;
		move.b	mixer_wk_SCSP+17(a6),d2		;read work
		andi.b	#$E0,d2				;mask level
		or.b	d2,d0				;
		move.b	d1,d1				;stereo/monaural flag
		bpl.s	?stereoR			;
		andi.b	#$E0,d0				;center
?stereoR:	move.b	d0,mixer_wk_SCSP+17(a6)		;set to work
		move.b	d0,$220+SCSP_EFSDLPN(a5)	;set to scsp
		rts					;

;-----------------------------------------------------------------------;
;	#82	scsp master volume control				;
;		P1:master volume					;
;-----------------------------------------------------------------------;
			.global	drv_vol
drv_vol:	move.b	(a0)+,d0			;P1:master volume
		andi.w	#$000f,d0			;
		move.b	d0,RG_MVOL(a5)			;set to scsp
		move.w	d0,(scsp_mvol)			;set to work
		rts					;

;-----------------------------------------------------------------------;
;	#83	dsp micro program change				;
;		P1:dsp micro program #					;
;-----------------------------------------------------------------------;
			.global	drv_effect
drv_effect:	clr.w	d1				;
		move.b	(a0)+,d3			;P1:dsp program #
		jmp	CTRL_5B(pc)			;

;-----------------------------------------------------------------------;
;	#84	not deffinition						;
;-----------------------------------------------------------------------;
			.global	drv_84
drv_84:		rts					;

;-----------------------------------------------------------------------;
;	#85	pcm stream play						;
;	--------------------------------------------------------	;
;	P1:bit7  = stereo(0:monaural,1:stereo)				;
;	   bit4  = format(0:16bit,1:8bit)				;
;	   bit2-0= pcm stream #						;
;	P2:bit7-5= direct level						;
;	   bit4-0= direct pan						;
;	P3:pcm start address (bit19-12)					;
;	P4:pcm strat address (bit11-4)					;
;	P5:pcm play ring buffer size MSB				;
;	P6:pcm play ring buffer size LSB				;
;	P7:play pitch MSB						;
;	P8:play pitch LSB						;
;	P9:right channel effect level & panpot				;
;	P10:left channel effect level & panpot				;
;	P11:total level							;
;-----------------------------------------------------------------------;
			.global	drv_PCM_st
drv_PCM_st:	lea	Pcm_Strm(a6),a3			;($8890)
		lea	slot_work(a6),a2		;($8000)
		move.b	HOST_P1(a0),d7			;P1:stream format
		bpl	drv_85_mono			;
	;-------------------------------------------------------;
	;	stereo stream					;
	;-------------------------------------------------------;
drv_85_stereo:	andi.w	#7,d7				;management #
		add.w	d7,d7				;
	;------- 1st(right) channel
		move.b	0(a3,d7.w),d4			;
		bset.b	#PCM_CTL_BITPOS,0(a3,d7.w)	;play flag set
		andi.w	#$1F,d4				;1st channel use slot#
		lsl.w	#6,d4				;
	;------- 2nd(left) channel
		move.b	1(a3,d7.w),d5			;
		bset.b	#PCM_CTL_BITPOS,1(a3,d7.w)	;play flag set
		andi.w	#$1F,d5				;2nd channel use slot#
		lsl.w	#6,d5				;
	;-------------------------------------------------------;
drv_85_st1:	move.b	HOST_P1(a0),d0			;P1:stream format
		move.b	d0,d7				;
		andi.w	#7,d0				;management #
		ori.b	#$80,d0				;
		move.b	d0,PSPN(a2,d4.w)		;1st ch management# & flag store
		move.b	d0,PSPN(a2,d5.w)		;2nd ch management# & flag store
		bset.b	#streamInUse,slot_status+1(a2,d4.w)	;stream in use flag set
		bset.b	#streamInUse,slot_status+1(a2,d5.w)	;
		lsr.w	#1,d4				;
		lsr.w	#1,d5				;
		move.b	#$10,$00(a5,d4.w)		;1st ch slot key off
		move.b	#$10,$00(a5,d5.w)		;2nd ch slot key off
		andi.w	#00010000B,d7			;format check(8 or 16bit)
		ori.w	#00100000B,d7			; set Forward loop
		swap	d7				;
		clr.w	d7				;
	;------- 1st(right channel buffer address set ----------;
		moveq	#0,d0				;
		move.w	d0,SCSP_PCM_LSA(a5,d4.w)	;1st ch loop start adrs 0 set
		move.w	d0,SCSP_PCM_LSA(a5,d5.w)	;2nd ch loop start adrs 0 set
		move.w	HOST_P3(a0),d0			;P3-4:start address MSB
		lsl.l	#4,d0				;
		or.l	d7,d0				;
		move.l	d0,(a5,d4.w)			;1st ch start address
		moveq	#0,d1				;
		move.b	d1,SCSP_SISD(a5,d4.w)		;[SI,SD] reset
		move.b	HOST_P11(a0),SCSP_TLVL(a5,d4.w)	;P11:total level
		move.w	d1,SCSP_MDLSL(a5,d4.w)		;1st ch fm modurator reset
		move.b	d1,SCSP_SISD(a5,d5.w)		;[SI,SD] reset
		move.b	HOST_P11(a0),SCSP_TLVL(a5,d5.w)	;P11:total level
		move.w	d1,SCSP_MDLSL(a5,d5.w)		;2nd ch fm modurator reset
		move.w	d1,SCSP_RELFO(a5,d4.w)		;1st ch lfo status reset
		move.w	d1,SCSP_RELFO(a5,d5.w)		;2nd ch lfo status reset
		move.w	HOST_P5(a0),d1			;P5-6:buffer size
		subq.w	#1,d1				;
		move.w	d1,SCSP_PCM_LEA(a5,d4.w)	;1st ch loop end address set
		move.w	d1,SCSP_PCM_LEA(a5,d5.w)	;2nd ch loop end address set
		addq.l	#1,d1				;
	;------- 2nd(left) channel buffer start address set -----;
		add.l	d1,d0				;2nd ch buffer top
		btst.l	#20,d7				;format check 8/16 bit ?
		bne.s	drv_85_1			; jump if 8bit data
		add.l	d1,d0				; + data sample size
drv_85_1:	move.l	d0,(a5,d5.w)			;start address 
		move.b	HOST_P2(a0),d0			;P2:direct level/pan
		andi.b	#$E0,d0				;mask direct leve
		ori.b	#$0F,d0				;pan = [R]
		btst.b	#7,SND_OUT_ST(a6)		;check monaural/stereo
		beq.s	?stereo				;
		andi.b	#$F0,d0				;center
?stereo:	move.b	d0,SCSP_DISDLPN(a5,d4.w)	;1st ch direct level/panpot set
		add.w	d4,d4				;
		move.b	d0,sl_DISDLPAN(a2,d4.w)		;set to work
		lsr.w	#1,d4				;
		ori.b	#$10,d0				;set Pan = [L]
		move.b	d0,SCSP_DISDLPN(a5,d5.w)	;2nd ch direct level/panpot set
		add.w	d5,d5				;
		move.b	d0,sl_DISDLPAN(a2,d5.w)		;
		lsr.w	#1,d5				;

		move.l	#$001F001F,d0		; AR,RR = 1FH
		move.l	d0,$08(a5,d4.w)		; <<< [D2R][D1R][EHLD][AR] >>>
		move.l	d0,$08(a5,d5.w)		; <<< [D2R][D1R][EHLD][AR] >>>
		move.b	HOST_P9(a0),d0
		move.b	d0,$15(a5,d4.w)		; <<< [ISEL],[IMXL] : R >>>
		move.b	HOST_P10(a0),d0
		move.b	d0,$15(a5,d5.w)		; <<< [ISEL],[IMXL] : L >>>
		move.w	HOST_P7(a0),d0
		move.w	d0,$10(a5,d4.w)		; <<< [OCT],[FNS] : R >>>
		move.w	d0,$10(a5,d5.w)		; <<< [OCT],[FNS] : L >>>
		move.b	#$08,$00(a5,d4.w)	; <<< key on èÄîı >>>
		move.b	#$18,$00(a5,d5.w)	; <<< ìØéû key on >>>
		rts
	;-------------------------------------------------------;
	;	monaural stream					;
	;-------------------------------------------------------;
		;===============================;
		; d7.b = P1:mode
drv_85_mono:	andi.w	#7,d7			; = PCM Stream#
		add.w	d7,d7			;
		move.b	0(a3,d7.w),d4		; R : play flag & slot#
		bset.b	#PCM_CTL_BITPOS,0(a3,d7.w)		;
		andi.w	#$1F,d4			;
		lsl.w	#6,d4			;
		move.b	1(a3,d7.w),d5		; L : play flag & slot#
		bclr.b	#PCM_CTL_BITPOS,1(a3,d7.w)		;
		andi.w	#$1F,d5			;
		lsl.w	#6,d5			;
		;-------------------------------;
		; d4.w = ( R ) slot work offset
		; d5.w = ( L ) slot work offset
drv_85_mn1:	move.b	HOST_P1(a0),d0		; P1 : D4 = 8bit / D210 = #
		move.b	d0,d7			;
		andi.w	#7,d0			; = PCM Stream#
		ori.b	#$80,d0			;

		move.b	d0,PSPN(a2,d4.w)	; save PCM Stream# & play flag
		bset.b	#streamInUse,slot_status+1(a2,d4.w)

		move.b	PSPN(a2,d5.w),d0	; L ê¨ï™

		move.b	#0,PSPN(a2,d5.w)	; clear PCM Stream# & play flag
		bclr.b	#streamInUse,slot_status+1(a2,d5.w)

		lsr.w	#1,d4			; $00,$20,..,$3E0
		lsr.w	#1,d5			;

		move.b	#$10,$00(a5,d4.w)	; <<< key off >>>
		move.b	#$10,$00(a5,d5.w)	; <<< key off >>>

		andi.w	#00010000B,d7		; 16/8 bit PCM ?
		ori.w	#00100000B,d7		; set forward loop
		swap	d7			;
		clr.w	d7			;
		moveq	#0,d0			;
		move.w	d0,SCSP_PCM_LSA(a5,d4.w)	; <<< [LSA] >>>
		move.w	HOST_P3(a0),d0		; P3,4 : [SA] è„à 16bits
		lsl.l	#4,d0			;
		or.l	d7,d0			;
		move.l	d0,(a5,d4.w)		; <<< [SA] >>>
		moveq	#0,d1			;

		move.b	d1,SCSP_SISD(a5,d4.w)	; <<< [SI],[SD] = 0 >>>
		move.b	HOST_P11(a0),SCSP_TLVL(a5,d4.w)	;
		move.w	d1,SCSP_MDLSL(a5,d4.w)	; <<< [MSL],[MDXYSL] = 0 >>>
		move.w	d1,SCSP_RELFO(a5,d4.w)	; <<< [LFO] >>>
		move.w	HOST_P5(a0),d1		; = 1,2,...,FFFF,0000
		subq.w	#1,d1			; = 0,1,...,FFFE,FFFF
		move.w	d1,SCSP_PCM_LEA(a5,d4.w)	; <<< [LEA] >>>

		move.b	HOST_P2(a0),d0		; P2   : [DISDL],[DIPAN]
		btst.b	#7,SND_OUT_ST(a6)	; MONO/STEREO status
		beq.s	drv_85_6ST		; jump if STEREO mode
		andi.b	#$F0,d0			; set Center
drv_85_6ST:
		move.b	d0,SCSP_DISDLPN(a5,d4.w)		; <<< [DISDL],[DIPAN] >>>
		move.l	#$001F001F,SCSP_D2R1R(a5,d4.w)		; <<< AR,RR = 1FH >>>
		move.b	HOST_P9(a0),SCSP_ISEL+1(a5,d4.w)	; <<< [ISEL],[IMXL] >>>
		move.w	HOST_P7(a0),SCSP_OCTFNS(a5,d4.w)	; <<< [OCT],[FNS] >>>
		move.b	#$18,SCSP_KXKB(a5,d4.w)			; <<< key on >>>
		add.w	d4,d4
		move.b	d0,sl_DISDLPAN(a2,d4.w)
		rts
;-----------------------------------------------------------------------;
;	#8A	pcm stream parameter change				;
;	in	a0:host comand buffer pointer				;
;	out	none							;
;	break	a2,a4,d0,d4,d5,d7					;
;-----------------------------------------------------------------------;
			.global	drv_8A
drv_8A:		lea	slot_work(a6),a2		;($8000)
		lea	Pcm_Strm(a6),a3			;($8890)
		move.b	HOST_P1(a0),d7			;P1:management #
		andi.w	#7,d7				;
		add.w	d7,d7				;
		move.b	(a3,d7.w),d4			;1st(right) channel use slot#
		bpl.s	?end				;jump if non play
		move.b	1(a3,d7.w),d5			;left channel use slot#
		bpl.s	?monaural			;
	;-------------------------------------------------------;
	;	stereo stream					;
	;-------------------------------------------------------;
		andi.w	#$1F,d4				;slot #(r)
		andi.w	#$1F,d5				;slot #(l)
		lsl.w	#5,d4				;
		lsl.w	#5,d5				;
		move.b	HOST_P2(a0),d0			;P2:direct component(level,pan)
		andi.b	#$E0,d0				;direct level
	;------- pan right channel -----------------------------;
		btst.b	#7,SND_OUT_ST(a6)		;system mono/stereo mode check
		bne.s	?skip1				; jump if MONO mode
		ori.b	#$0F,d0				;direct pan set to right
?skip1:		move.b	d0,SCSP_DISDLPN(a5,d4.w)	;right ch direct component set
		move.b	HOST_P7(a0),SCSP_TLVL(a5,d4.w)	;P7:total level
		add.w	d4,d4				;
		move.b	d0,sl_DISDLPAN(a2,d4.w)		;set to work
		lsr.w	#1,d4				;
	;------- pan left channel ------------------------------;
		ori.b	#$10,d0				;direct pan set to left
		move.b	d0,SCSP_DISDLPN(a5,d5.w)	;left ch direct component set
		move.b	HOST_P7(a0),SCSP_TLVL(a5,d5.w)	;P7:total level
		add.w	d5,d5				;
		move.b	d0,sl_DISDLPAN(a2,d5.w)		;set to work
		lsr.w	#1,d5				;
	;------- pitch set -------------------------------------;
		move.w	HOST_P3(a0),d0			;P3-4:pitch
		move.w	d0,SCSP_OCTFNS(a5,d4.w)		;right channel pitch set
		move.w	d0,SCSP_OCTFNS(a5,d5.w)		;left channel pitch set
		move.b	HOST_P5(a0),SCSP_ISEL+1(a5,d4.w)	;P5:right ch efx component
		move.b	HOST_P6(a0),SCSP_ISEL+1(a5,d5.w)	;P6:left ch efx component
?end:		rts					;
	;-------------------------------------------------------;
	;	monaural stream					;
	;-------------------------------------------------------;
?monaural:	andi.w	#$1F,d4				;slot #
		lsl.w	#5,d4				;
		move.b	HOST_P2(a0),d0			;P2:direct component(level,pan)
		btst.b	#7,SND_OUT_ST(a6)		;system mono/stereo mode check
		beq.s	?skip2				;jump if STEREO mode
		andi.b	#$E0,d0				;direct pan set to center
?skip2:		move.b	d0,SCSP_DISDLPN(a5,d4.w)	;direct component set
		move.b	HOST_P7(a0),SCSP_TLVL(a5,d4.w)	;P7:total level
		add.w	d4,d4				;
		move.b	d0,sl_DISDLPAN(a2,d4.w)		;set to work
		lsr.w	#1,d4				;
		move.w	HOST_P3(a0),SCSP_OCTFNS(a5,d4.w)	;P3-4:pitch
		move.b	HOST_P5(a0),SCSP_ISEL+1(a5,d4.w)	;P5:efx component
		rts					;
;-----------------------------------------------------------------------;
;	#86	pcm stream stop						;
;	in	a0:host command buffer pointer				;
;	out	none							;
;	break	a2,a3,a5,a6,d4						;
;-----------------------------------------------------------------------;
			.global	drv_PCM_ed
drv_PCM_ed:	move.b	(a0),d7			;P1:PCM stream# ( 0 Å` 7 )
		andi.w	#7,d7			;
		add.w	d7,d7			;x2
	;-------------------------------------------------------;
	;	in	d7:management #				;
	;-------------------------------------------------------;
PCM_strm_stop:	lea	IO_SCSP,a5			;($100000)
		lea	bs_PWKTP,a6			;($7000)
		lea	Pcm_Strm(a6),a3			;($8890)
		lea	slot_work(a6),a2		;($8000)
	;------- right ch play check ---------------------------;
		move.b	(a3,d7.w),d4			;right channel use slot #
		bpl.s	?skip				;jump if non play
		bclr.b	#PCM_CTL_BITPOS,(a3,d7.w)			;
		jsr	?clear(pc)			;
	;------- left ch play check ----------------------------;
?skip:		move.b	1(a3,d7.w),d4			;left channel use slot #
		bpl.s	?end				;jump if non play
		bclr.b	#PCM_CTL_BITPOS,1(a3,d7.w)			;
	;------- clear slot work & key off ---------------------;
?clear:		andi.w	#$1F,d4				;slot #
		lsl.w	#6,d4				;
;		move.b	#0,PSPN(a2,d4.w)		;clear management #,flags
;		bclr.b	#streamInUse,slot_status+1(a2,d4.w)	;clear stream use flag
		lsr.w	#1,d4				;
		move.b	#$10,$00(a5,d4.w)		;key off slot
?end:		rts					;
;-----------------------------------------------------------------------;
;	#88	mixer parameter change					;
;		P1:effect send channel #($00 to $12)			;
;		P2:effect level & panpot([EFSDL],[EFPAN])		;
;-----------------------------------------------------------------------;
			.global	drv_88
drv_88:		clr.w	d0				;
		move.b	(a0)+,d0			;P1:effect send channel #
		cmpi.w	#18,d0				;
		bcc	?end				;illegal channel # then end
		move.b	(a0),d1				;P2:effect level & panpot
		lea	mixer_wk_SCSP(a6),a1		;
		move.b	d1,(a1,d0.w)			;set to work
		lsl.w	#5,d0				;
		move.b	d1,SCSP_EFSDLPN(a5,d0.w)	;set to scsp
?end:		rts					;


;-----------------------------------------------------------------------;
;	#89	hard ware check						;
;-----------------------------------------------------------------------;
			.global	drv_89
drv_89:		move.b	(a0)+,d0			;P1:hard check id #
		cmpi.b	#7+1,d0				;
		bcc.w	CM_ret				;
		andi.w	#$07,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		jsr	HD_CHK_TB(pc,d0.w)		;
		bra.w	CM_ret				;
HD_CHK_TB:	jmp	HD_CHK_00(pc)			;00:D-RAM 4Mbit R/W check
		jmp	HD_CHK_01(pc)			;01:D-RAM 8Mbit R/W check
		jmp	HD_CHK_02(pc)			;02:SCSP MIDI comunication check
		jmp	HD_CHK_03(pc)			;03:sound out L&R check
		jmp	HD_CHK_04(pc)			;04:sound out L only check
		jmp	HD_CHK_05(pc)			;05:sound out R only check
		jmp	CM_ret(pc)			;06:
		jmp	CM_ret(pc)			;07:
	;-------------------------------------------------------;
	;	#00	d-ram 4Mbit r/w check			;
	;-------------------------------------------------------;
			.global	HD_CHK_00
HD_CHK_00:	moveq	#7-1,d6				;loop size ( $10000Å`$7ffff)
		bra.s	HD_CHK_00_s			;
	;-------------------------------------------------------;
	;	#01	d-ram 8Mbit r/w check			;
	;-------------------------------------------------------;
			.global	HD_CHK_01
HD_CHK_01:	moveq	#15-1,d6			;loop size ( $10000Å`$fffff)
	;-------------------------------------------------------;
	;	r/w check routine				;
	;-------------------------------------------------------;
HD_CHK_00_s:
	sr_push						;
	int_di						;
		lea	IO_SCSP,a5			;
		lea	DSP_COEF(a5),a0			;
		move.w	#$500/4-1,d7			;
		moveq	#0,d0				;
?dspClear:	move.l	d0,(a0)+			;
		dbra	d7,?dspClear			;
		move.w	#$7FFF,d3			;error code
		lea	$10000,a0			;
HD_CHK_00_lp:	jsr	D_RAM_RW(pc)			;$10000 ~ r/w check
		bcs.s	HD_CHK_00_er			;
		dbra	d6,HD_CHK_00_lp			;
		addq.w	#1,d3				; complete code
HD_CHK_00_er:	move.w	d3,Mem_HD_chk			;
	sr_pop						;
		rts					;
	;-------------------------------------------------------;
	;	d-ram r/w check					;
	;-------------------------------------------------------;
			.global	D_RAM_RW
D_RAM_RW:	move.l	#$55555555,d1			;
		move.l	#$aaaaaaaa,d2			;
		move.w	#$10000/4-1,d7			; loop size
D_RAM_RW_lp:	move.l	(a0),d0				;
		move.l	d1,(a0)				;
		cmp.l	(a0),d1				;
		bne.s	D_RAM_error			;
		move.l	d2,(a0)				;
		cmp.l	(a0),d2				;
		bne.s	D_RAM_error			;
		move.l	d0,(a0)+			;
		dbra	d7,D_RAM_RW_lp			;
		clc					;carry clear
		rts					;
D_RAM_error:	stc					;carry set
		rts					;
	;-------------------------------------------------------;
	;	#02	scsp midi comunication check		;
	;-------------------------------------------------------;
			.global	HD_CHK_02
HD_CHK_02:	lea	IO_SCSP,a5			;
		move.w	#$7FFF,d3			;error code
		move.b	RG_MIBUF(a5),d0			;read midi in buffer
		move.b	RG_MIBUF(a5),d0			;
		move.b	RG_MIBUF(a5),d0			;
		move.b	RG_MIBUF(a5),d0			;
		move.b	#$55,d0				;
		jsr	HD_CHK_02_sb(pc)		;
		bcs.s	?end				;
		move.b	#$aa,d0				;
		jsr	HD_CHK_02_sb(pc)		;
		bcs.s	?end				;
		addq.w	#1,d3				;complete code
?end:		move.w	d3,Mem_HD_chk			;
		rts					;
	;-------------------------------------------------------;
HD_CHK_02_sb:	move.b	d0,RG_MOBUF(a5)			;write to midi out buffer
		move.w	#$80,d7				;loop size
?loop:		move.w	RG_SCIPD(a5),d1			;midi input int check
		andi.b	#1000b,d1			;
		bne.s	?skip				;
		dbra	d7,?loop			;
		bra.s	?errorEnd			;
?skip:		move.b	RG_MIBUF(a5),d1			;read midi in buffer
		cmp.b	d0,d1				;
		bne.s	?errorEnd			;
		clc					;
		rts					;
?errorEnd:	stc					;
		rts					;
	;-------------------------------------------------------;
	;	#03	sound out l&r check			;
	;-------------------------------------------------------;
			.global	HD_CHK_03
HD_CHK_03:	move.b	#$E0,d2				; L&R out
		bra.s	HD_CHK_0345			;
	;-------------------------------------------------------;
	;	#04	sound out l only check			;
	;-------------------------------------------------------;
			.global	HD_CHK_04
HD_CHK_04:	move.b	#$FF,d2				; L only out
		bra.s	HD_CHK_0345			;
	;-------------------------------------------------------;
	;	#05	sound out r only check			;
	;-------------------------------------------------------;
			.global	HD_CHK_05
HD_CHK_05:	move.b	#$EF,d2				; R only out
	;-------------------------------------------------------;
	;	
	;-------------------------------------------------------;
HD_CHK_0345:	lea	TESTSDTB,a0			;
		movea.l	a5,a1				;
		move.l	(a0)+,(a1)+			;
		move.l	(a0)+,(a1)+			;
		move.l	(a0)+,(a1)+			;
		move.l	(a0)+,(a1)+			;
		move.l	(a0)+,(a1)+			;
		move.l	(a0)+,(a1)+			;
		move.b	d2,SCSP_DISDLPN(a5)		;direct vol & pan
		move.b	#$18,(a5)			;key on
		move.w	#$2000,d7			;
HD_CHK_03_2:	move.w	#$10,d6				;
HD_CHK_03_1:	lsl.l	#8,d0				;
		dbra	d6,HD_CHK_03_1			;
		dbra	d7,HD_CHK_03_2			;
		move.b	#$10,(a5)			; key off
		rts					;
	;-------------------------------------------------------;
	;	regisetr data for check				;
	;-------------------------------------------------------;
TESTSDTB:	dc.l	SQRTB+$10200000			;[KX,KB,SBCTL,SSCTL,8B,SA]
		dc.w	0				;[LSA]
		dc.w	$10-1				;[LEA]
		dc.w	$001f				;[D2R,D1R,HO,AR]
		dc.w	$001f				;[LS,KRS,DL,RR]
		dc.w	$0000				;[SI,SD,TL]
		dc.w	0				;[MDL,MDXS,MDYS]
		dc.w	$7000				;[OCT,FNS]
		dc.w	0				;[RE,LFOF,PLFWS,PLFOS,ALFOWS,ALFOS]
		dc.w	0				;[ISEL,IMXS]
		dc.w	$E000				;[DISDL,DIPAN,EFSDL,EFPAN]
	;-------------------------------------------------------;
	;	square wave for check				;
	;-------------------------------------------------------;
SQRTB:		dc.w	$c000,$c000,$c000,$c000,$c000,$c000,$c000,$c000
		dc.w	$4000,$4000,$4000,$4000,$4000,$4000,$4000,$4000


;-----------------------------------------------------------------------;
;	#8B	pcm stream use slot allocation				;
;		P1:stereo/mono flag,pcm management #			;
;-----------------------------------------------------------------------;
pcmSlotAlloc:	lea	Pcm_Strm(a6),a3			;($8890)
		lea	slot_work(a6),a2		;($8000)
		move.b	HOST_P1(a0),d7			;P1:stream format
		bpl.s	?mono				;
		andi.w	#7,d7				;management #
		add.w	d7,d7				;
	;------- 1st(right) channel
		move.b	0(a3,d7.w),d4			;
		andi.w	#$1F,d4				;1st channel use slot#
		lsl.w	#6,d4				;
		move.b	#$80,PSPN(a2,d4.w)		;1st ch management# & flag store
		bset.b	#streamInUse,slot_status+1(a2,d4.w)	;stream in use flag set
	;------- 2nd(left) channel
		move.b	1(a3,d7.w),d5			;
		andi.w	#$1F,d5				;2nd channel use slot#
		lsl.w	#6,d5				;
		move.b	#$80,PSPN(a2,d5.w)		;2nd ch management# & flag store
		bset.b	#streamInUse,slot_status+1(a2,d5.w)	;
		rts					;
?mono:		andi.w	#7,d7				;management #
		add.w	d7,d7				;
	;------- 1st(right) channel
		move.b	0(a3,d7.w),d4			;
		andi.w	#$1F,d4				;1st channel use slot#
		lsl.w	#6,d4				;
		move.b	#$80,PSPN(a2,d4.w)		;1st ch management# & flag store
		bset.b	#streamInUse,slot_status+1(a2,d4.w)	;stream in use flag set
	;------- 2nd(left) channel
		move.b	1(a3,d7.w),d5			;
		andi.w	#$1F,d5				;2nd channel use slot#
		lsl.w	#6,d5				;
		move.b	#$00,PSPN(a2,d5.w)		;2nd ch management# & flag clear
		bclr.b	#streamInUse,slot_status+1(a2,d5.w)	;stream in use flag clear
		rts					;

;-----------------------------------------------------------------------;
;	#8C	pcm stream use slot release				;
;		P1:stereo/mono flag,pcm management #			;
;-----------------------------------------------------------------------;
pcmSlotRel:	lea	Pcm_Strm(a6),a3			;($8890)
		lea	slot_work(a6),a2		;($8000)
		move.b	HOST_P1(a0),d7			;P1:stream format
		andi.w	#7,d7				;management #
		add.w	d7,d7				;
	;------- 1st(right) channel
		move.b	0(a3,d7.w),d4			;
		andi.w	#$1F,d4				;1st channel use slot#
		lsl.w	#6,d4				;
		move.b	#$00,PSPN(a2,d4.w)		;1st ch management# & flag store
		bclr.b	#streamInUse,slot_status+1(a2,d4.w)	;stream in use flag set
	;------- 2nd(left) channel
		move.b	1(a3,d7.w),d5			;
		andi.w	#$1F,d5				;2nd channel use slot#
		lsl.w	#6,d5				;
		move.b	#$00,PSPN(a2,d5.w)		;2nd ch management# & flag clear
		bclr.b	#streamInUse,slot_status+1(a2,d5.w)	;stream in use flag clear
		rts					;

;-----------------------------------------------------------------------;
;	in	d2.b : SEQ Bank# ( 0 Å` F )				;
;		d3.b : SEQ Song# ( 0 Å` FF )				;
;	out	a2 : Seq. top address					;
;		CY : error						;
;	break	d2/d3/d4/d5/d6						;
;-----------------------------------------------------------------------;
			.global	get_seq_top_adr
get_seq_top_adr:
		lea	bs_AMAPC,a2			; area map current work top
		moveq	#256/8-1,d5			; loop size
gsta_lp:	move.b	(a2),d4				; 
		bmi.s	seq_strt_er1			; jump if Area Map data end
		move.b	d4,d6				;
		andi.b	#$70,d4				; = Data ID * $10
		cmpi.b	#SEQ_ID*$10,d4			;
		bne.s	gsta_1				;
		andi.b	#$0F,d6				;
		cmp.b	d6,d2				; SEQ Bank# equal ?
		beq.s	gsta_pass			; jump if yes
gsta_1:		lea	8(a2),a2			;
		dbra	d5,gsta_lp			;
seq_strt_er1:	bset.b	#ERRb_6,Mem_err_bit+3		; cannot find äYìñ SEQ Bank#
		stc					;
		rts					;
seq_strt_er2:	bset.b	#ERRb18,Mem_err_bit+1		; cannot find äYìñ SEQ Song#
		stc					;
		rts					;
seq_strt_er3:	bset.b	#ERRb_8,Mem_err_bit+2		; cannot down load SEQ data
		stc					;
		rts					;

gsta_pass:	bclr.b	#ERRb_6,Mem_err_bit+3	; find äYìñ SEQ Bank#
		move.l	(a2)+,d2		;
		move.b	(a2)+,d4		; Seq.Bank down load çœ ?
		bpl.s	seq_strt_er3		; jump if Seq.data not ready
		bclr.b	#ERRb_8,Mem_err_bit+2	; SEQ data down load çœ
		andi.l	#$FFFFF,d2		;
		movea.l	d2,a2			; = seq. Bank top
		andi.w	#$ff,d3			;
		cmp.w	(a2),d3			; ÇaÇfÇlêî > d3 ?
		bcc.s	seq_strt_er2		; jump if Seq song# too large
		bclr.b	#ERRb18,Mem_err_bit+1	; cannot find äYìñ SEQ Song#
		add.w	d3,d3			;
		add.w	d3,d3			;
		adda.l	2(a2,d3.w),a2		; = dest. seq# data top
		clc
		rts

;=======================================================================;
;	end of file							;
;=======================================================================;
