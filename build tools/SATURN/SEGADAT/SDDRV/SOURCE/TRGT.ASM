;=======================================================================;
;	SEGASATURN sound driver						;
; file name								;
;	trgt.asm							;
; copyright								;
;	SEGA ENTERPRISES						;
; purpose								;
;	target ID/Interupt						;
; author(s)								;
;	1995-11-14	A.Miyazawa	Ver.2.00			;
; mod history								;
;	1996-02-23	A.Miyazawa	Ver.2.06			;
;	1996-07-02	A.Miyazawa	Ver.2.10L			;
;	1997-06-30	Y.Kashima	Ver.2.20			;
;=======================================================================;

		.include	SCSP.LIB

			.globals	on

			.extern	volume_calculate	;
			.extern	MIDI_ctrl		;
			.extern	HOST_IF			;
			.extern	FH1005_init		;init
			.extern	wk_RAM_init		;init
			.extern	MONITOR_init		;init
			.extern	HOST_IF_init		;init
			.extern	BANK_init		;init
			.extern	sl_layer_adr_init	;init
			.extern	vector_tb_init		;init
			.extern	SEGA_MARK		;
			.extern	xseq_2msec		;
			.extern	MIXER_wr		;chg
			.extern	MIXER_wk_wr		;chg
			.extern	MAP_chg			;
			.extern	YM3802_init		;init
			.extern	get_BANK_addr		;
			.extern	er_0A			;
			.extern	er_1F			;
			.extern	er_01			;
			.extern	er_0B			;
			.extern	er_0C			;
			.extern	er_0D			;
			.extern	er_0E			;
			.extern	er_0F			;
			.extern	er_10			;
			.extern	er_11			;
			.extern	er_12			;
			.extern	er_13			;
			.extern	er_14			;
			.extern	er_15			;
			.extern	er_16			;
			.extern	er_17			;
			.extern	er_18			;
			.extern	er_19			;
			.extern	er_27			;
			.extern	send_frequency		;

;-----------------------------------------------------------------------;
;	vector								;
;-----------------------------------------------------------------------;
	.if sw_MODEL_M

			.org		bs_PRGTP

	.else			; sw_MODEL_M

		dc.l	bs_SSP				; #00 stack pointer
		dc.l	program_start			; #01 program counter
		dc.l	er_1F				; #02 buss error
		dc.l	er_01				; #03 address error
		dc.l	er_0E				; #04 illegal function
		dc.l	er_0F				; #05 zero divide
		dc.l	er_10				; #06 chk function
		dc.l	er_11				; #07 trapv function
		dc.l	er_12				; #08 
		dc.l	er_13				; #09 
		dc.l	er_14				; #10 un
		dc.l	er_14				; #11 
		dc.l	er_15				; #12 reserved
		dc.l	er_15				; #13 reserved
		dc.l	er_15				; #14 reserved
		dc.l	er_16				; #15 uninitialized
		dc.l	er_15				; #16 reserved
		dc.l	er_15				; #17 reserved
		dc.l	er_15				; #18 reserved
		dc.l	er_15				; #19 reserved
		dc.l	er_15				; #20 reserved
		dc.l	er_15				; #21 reserved
		dc.l	er_15				; #22 reserved
		dc.l	er_15				; #23 reserved
		dc.l	er_17				; #24 
		dc.l	er_27				; #25 Level#1 (SCSI)
		dc.l	level_2				; #26 Level#2 Timer-B
		dc.l	er_0B				; #27 Level#3 ..........
		dc.l	er_0C				; #28 Level#4 ..........
		dc.l	level_5				; #29 Level#5 MIDI #1
		dc.l	er_0A				; #30 Level#6 MIDI #2
		dc.l	er_0D				; #31 Level#7 debugger

		dc.l	er_18,er_18,er_18,er_18,er_18,er_18,er_18,er_18
		dc.l	er_18,er_18,er_18,er_18,er_18,er_18,er_18,er_18

		dc.l	er_15,er_15,er_15,er_15,er_15,er_15,er_15,er_15
		dc.l	er_15,er_15,er_15,er_15,er_15,er_15,er_15,er_15

		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #064-#071
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #072-#079
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #080-#087
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #088-#095
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #096-#103
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #104-#111
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #112-#119
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #120-#127
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #128-#135
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #136-#143
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #144-#151
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #152-#159
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #160-#167
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #168-#175
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #176-#183
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #184-#191
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #192-#199
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #200-#207
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #208-#215
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #216-#223
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #224-#231
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #232-#239
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #240-#247
		dc.l	er_19,er_19,er_19,er_19,er_19,er_19,er_19,er_19	; #248-#255
;-----------------------------------------------------------------------;
;	system interface table ( 400H Å` )				;
;-----------------------------------------------------------------------;
		long	bs_SINFO		; System information table pointer
		long	host_if_work		; Host interface work pointer
		long	bs_AMAPC		; Area Map Current Work top
		long	bs_TIFWK		; Sound Tool Interface Work Pointer
		byte	0			; DSP program load fiag(BootROM work)
		byte	0			; Sound driver load flag
		long	bs_SIFWK		; System interface work pointer
		BLKB	42,0			; reserved
;-----------------------------------------------------------------------;
;	system information table ( 440H Å` )				;
;-----------------------------------------------------------------------;
		dc.l	program_start		; 68K Program top
		dc.l	sz_PRG			;	//    size
		dc.l	bs_AREAM		; Area Map top
		dc.l	sz_AREAM		;    //    size
		dc.l	bs_PWKTP		; 68K Program work top
		dc.l	sz_Prg_wk		; 	//	   size
		BLKB	40,0			; reserved
;
		BLKB	128,0			; dummy blank	System I/F work  ( 480H Å` )
		BLKB	128,0FFH		; dammy data	Area Map Current ( 500H Å` )
		BLKW	64,0d0aH		; dammy data		//	 ( 580H Å` )
		BLKB	256,0			; dammy blank	TOOL I/F work    ( 600H Å` )
		BLKB	256,0			; dummy blank	HOST I/F work    ( 700H Å` )
		BLKB	2048,0			; dammy blank	HOST Command óöó( 800H Å` )

	.endif			; sw_MODEL_M

;-----------------------------------------------------------------------;
;	START	!							;
;-----------------------------------------------------------------------;
program_start:
		move.w	#$2700,sr			; äÑçûã÷é~
		moveq	#0,d0
		move.l	d0,d1
		move.l	d0,d2
		move.l	d0,d3
		move.l	d0,d4
		move.l	d0,d5
		move.l	d0,d6
		move.l	d0,d7
		movea.l	d0,a0
		movea.l	d0,a1
		movea.l	d0,a2
		movea.l	d0,a3
		movea.l	d0,a4

		bra.s	cpu_initialize

;-----------------------------------------------------------------------;
;	Credit of sound driver version					;
;-----------------------------------------------------------------------;

		dc.b	"Ver-2.2.0       "

	if ENGN
		dc.b	"with engine ctrl"
	else			; ENGN
		dc.b	"                "
	endif			; ENGN

;-----------------------------------------------------------------------;
;	Credit of Hardware Type ( on Middle box )		
;-----------------------------------------------------------------------;

	if sw_MODEL_M
		dc.b	"<< MIDDLE BOX >>"	;
	else			; sw_MODEL_M
	    .if SW_TITAN
		dc.b	"<<    ST-V    >>"	;
	    .else
		dc.b	"<< SEGASATURN >>"	;
	    .endif
	endif			; sw_MODEL_M

			.date			; mod date

		dc.b	      "          "	;
		dc.b	"                "	;

;-----------------------------------------------------------------------;
;	initializing							;
;-----------------------------------------------------------------------;
cpu_initialize:
		movea.l	#IO_SCSP,a5			;($100000)
		movea.l	#bs_PWKTP,a6			;($7000)
		movea.l	#bs_SSP,a7			;stack pointer($a000)
		bsr	FH1005_init			;scsp init
		bsr	wk_RAM_init			;work ram clear
		cmpi.l	#'SEGA',SEGA_MARK		;easy security
		bne.w	program_start			;
	.if ENGN
		move.l	#$7F000,(racing_interface+ENGN_TB_addr)	;tone start address
		move.l	#$800,(racing_interface+ENGN_TB_sz)	;tone size
	.endif			; ENGN
	.if sw_MODEL_M
		bsr	MONITOR_init			;monitor work clear
		bsr	HOST_IF_init			;host i/f clear
		moveq	#0,d0				;
		bsr	MAP_chg				;set map #0
		bsr	vector_tb_init			;vector init
		bsr	BANK_init			;set bank #0,prg #0
		bsr	sl_layer_adr_init		;set layer #0
		move.w	#0,_activ_MIX_NO(a6)		;set active mixer #0
		movea.l	bank_address(a6),_activ_MIX_BNK(a6)	;set active tone bank #0
		bsr	MIXER_wr			;mixer refresh
		bsr	YM3802_init			;midi i/o init
	.endif			; sw_MODEL_M
		move.b	#2,RG_M4DB(a5)			; SCSP 4Mbit D-RAM mode
		bsr	midiOutModeCheck		;midi out mode check
		bsr	CR_check			;media copy miss check
		move.w	#$2000,SR			;enable interupt

;-----------------------------------------------------------------------;
;	main routine							;
;-----------------------------------------------------------------------;
main_loop:	addq.w	#2,CPU_CAPA_cnt(a6)		; equ 0x25a0885c
		addq.w	#1,FUKA_cnt(a6)			;
	;-------------------------------------------------------;
	;	check MIDI buffer				;
	;-------------------------------------------------------;
?existChkLoop:	moveq	#0,d0				;
		move.b	_MIDI_RCV_RDPT(a6),d0		;
		cmp.b	_MIDI_RCV_WRPT(a6),d0		; 0 Å` ff
		beq.w	?exit				; jump if input MIDI nothing
		bsr	MIDI_ctrl			; ready d0.w
		bra.s	?existChkLoop			;
?exit:		bsr.w	MIDI_OUT_BF_ex			; MIDI èàóù data SCSP OUT
	;-------------------------------------------------------;
	;	check host command				;
	;-------------------------------------------------------;
main_2_pt:	move.b	(handShakeMode),d0		;timing flag mode check
		bpl.b	?cmdHandShake			;
		move.b	(handShakeFlag),d0		;timing flag check
		bpl.w	?Timebase			;
?cmdHandShake:	movea.l	#host_if_work,a0		;($0700)
		moveq	#8-1,d7				;
?cmdChkLoop:	move.b	(a0),d0				;
		beq.w	?nextBuffer			;no request
		move.l	(a0),-(sp)			;
		movem.l	a0/d7,-(sp)			;
		bsr	HOST_IF				;command function
		movem.l	(sp)+,a0/d7			;
		move.l	(sp)+,d0			;
		cmp.l	(a0),d0				;same command?
		beq.s	?clrCmd				;
		bset.b	#ERRa11,Mem_DRVERR_FLG		;new command faster
		bra.s	?nextBuffer			;
?clrCmd:	move.w	#0,(a0)				;command clear
?nextBuffer:	lea	$10(a0),a0			; = 00,10,20,30,..,70H + 10H
		dbra	d7,?cmdChkLoop			;
		andi.b	#$7F,handShakeFlag		;timming flag clear
	;------- timer check -----------------------------------;
?Timebase:	move.w	RG_SCIPD(a5),d0			;read scsp
		add.b	d0,d0				;timer a overflow check
		bpl.w	main_loop			;
	;------- timer a overflow ------------------------------;
	.if sw_MODEL_M
			.extern		LED_CTRL
		bsr	LED_CTRL			;led control(sound box)
	.endif			; sw_MODEL_M
		move.w	#TIMA_base,RG_TACTL(a5)		;4msec
		move.w	#$40,RG_SCIRE(a5)		;timer a reset
		addq.b	#1,BIG_timer(a6)		;inc counter by timer a overflow
	.if sw_MODEL_M
		bsr	MAC_MIXER			;for tone editor
	.endif			; sw_MODEL_M
		bsr	fade_control_from_timer		;
		bsr	slot_ctrl			;plfo,prg,pbend control
		move.b	BIG_timer(a6),d0		;
		andi.b	#1,d0				; 8msec(2*4msec) ?
		bne.w	main_4				;
		bsr	DSP_CPU				;dsp change check
		move.b	monauralFlag,d0			;stereo/mono flag
		cmp.b	SND_OUT_ST(a6),d0		;now mode
		beq.w	main_4				;jump if bit7 no change
		bsr	TV_mono_chg			;
main_4:
	.if sw_MODEL_M
		bsr	DSP_LINK			;for dsp linker
	.endif			; sw_MODEL_M
	.if ENGN
		bsr	ENGN_CTRL			;
	.endif			; ENGN
		bra	main_loop			;

;-----------------------------------------------------------------------;
;	MIDI èàóù data SCSP OUT or YM-3802 OUT				;
;-----------------------------------------------------------------------;
MIDI_OUT_BF_ex:	lea	_MIDI_OUT_BF(a6),a2		;
		move.w	_MIDI_OUT_RDPT(a6),d4		; word 0Å`3FF
?midiOutLoop:	cmp.w	_MIDI_OUT_WRPT(a6),d4		; word
		beq.w	?end				;
	.if sw_MODEL_M
		lea	IO_MIDI1+1,a0			;for ym3802-a
		move.b	#5,R01(a0)			; MIDI port-A
		move.b	Rx4(a0),d0			; FIFO-Tx status <--- R54
		andi.b	#01000000B,d0			; TxRDY ?
		beq.w	?end				; jump if FI-FO full
		move.b	(a2,d4.w),Rx6(a0)		; R56 <-- d0.b
	.else			; sw_MODEL_M
		move.b	RG_MSTS(a5),d0			;for scsp midi
		andi.b	#$10,d0				;out fifo full check
		bne.w	?end				;
		move.b	(a2,d4.w),RG_MOBUF(a5)		;
	.endif			; sw_MODEL_M
		addq.w	#1,d4				;
		andi.w	#$3ff,d4			;
		move.w	d4,_MIDI_OUT_RDPT(a6)		;read ptr refresh
		bra.w	?midiOutLoop			;
?end:		rts					;

;-----------------------------------------------------------------------;
;	fade control
;-----------------------------------------------------------------------;
fade_control_from_timer:
		moveq	#0,d0				;
		moveq	#0,d4				;
		moveq	#8-1,d7				;
		movea.l	#management_work,a1		;
		move.l	a2,-(sp)			;
		movea.w	#mode_status,a2			;
?manNumLoop:	tst.b	pause_flag(a1)			;
		bne.s	?end_of_fade_control		; Is fade control enable?
		move.w	up_down_flag(a1),d1		;
		beq.s	?end_of_fade_control		; Is fade control enable?
		tst.b	(a2)				;
		beq.s	?100				; when seq is not in progress
		bset.b	#fade_bit,(a2)			; *ver-2.06
?100:		subq.w	#1,fade_count_variable(a1)	;
		bne.s	?end_of_fade_control		; branch when count down, next turn.
		cmpi.w	#$8000,d1			;
		beq.s	?end_of_fade_control		; wait for sequence stop.
		tst.b	pause_flag(a1)			;
		bne.s	?end_of_fade_control		; pause in progress
		add.w	sequence_volume(a1),d1		;
		move.w	d1,sequence_volume(a1)		;
		cmp.w	seq_volume_arrival(a1),d1	;
		bne.s	?voltage_control		;
		clr.w	up_down_flag(a1)		;
		bclr.b	#fade_bit,(a2)			; *ver-2.06
		tst.w	d1				; if level=0 then stop music!
		bne.s	?voltage_control		;
		move.w	#$8000,performance_flag(a1)	;
		bra.s	?end_of_fade_control		;
?voltage_control:
		bsr	volume_calculate		;
		move.w	appointed_fade_rate(a1),fade_count_variable(a1)
?end_of_fade_control:
		addi.w	#$200,d0			;
		adda.w	#SQ_SIZ,a1			;
		addq.w	#2,a2				;
		dbra	d7,?manNumLoop			;
		move.l	(sp)+,a2			;
		rts					;

;-----------------------------------------------------------------------;
;	sound output status (stereo/monaural) change			;
;	in	d0:stereo/monaural flag					;
;-----------------------------------------------------------------------;
TV_mono_chg:	move.b	d0,SND_OUT_ST(a6)		;
		bmi.s	?100				;
		bsr	mono_to_stereo			;bit7=0:to stereo
?100:		bsr	stereo_to_mono			;bit7=1:to monaural
		rts					;
	;-------------------------------------------------------;
	;	change monaural to stereo			;
	;-------------------------------------------------------;
mono_to_stereo:	bsr	MIXER_wk_wr			;mixer data write to scsp
		moveq	#0,d4				;
		lea	slot_work(a6),a4		;
		moveq	#slot_size-1,d7			; loop ( slot ) size
?slotLoop:	move.w	_sl_kanri(a4,d4.w),d5		;management # x $200
		move.b	sl_DISDLPAN(a4,d4.w),d6		;read original [DISDL],[DIPAN]
		lsr.w	#1,d4				;
		move.b	d6,SCSP_DISDLPN(a5,d4.w)	;write to scsp
		add.w	d4,d4				;
		addi.w	#slot_wk_unit,d4		;
		dbra	d7,?slotLoop			;
		rts					;
	;-------------------------------------------------------;
	;	change stereo to monaural			;
	;-------------------------------------------------------;
stereo_to_mono:	moveq	#0,d4				;
		moveq	#18-1,d7			;
?to_mono_lp1:	andi.w	#$E0E0,SCSP_DISDLPN(a5,d4.w)	;DIPAN & EFPAN to center
		addi.w	#SCSP_slot_unit,d4		;
		dbra	d7,?to_mono_lp1			;
		moveq	#14-1,d7			;
?to_mono_lp2:	andi.b	#$E0,SCSP_DISDLPN(a5,d4.w)	;DIPAN to center
		addi.w	#SCSP_slot_unit,d4		;
		dbra	d7,?to_mono_lp2			;
		rts					;

;-----------------------------------------------------------------------;
;	direct download from dsp linker					;
;-----------------------------------------------------------------------;
	.if sw_MODEL_M
			.public		DSP_LINK
DSP_LINK:	move.b	Mem_DSP_FLG,d0			;direct down load from dsp linker
		bpl.s	?false				;
		andi.b	#$7F,d0				;
		move.b	d0,Mem_DSP_FLG			;
		lea	bs_TIFWK,a1			;tool i/f work ($600)
		move.w	OF_DSP_RBL(a1),d1		;
		move.w	d1,d0				;
		andi.w	#$7F,d0				;ring buffer pointer value
		swap	d0				;
		clr.w	d0				;
		lsr.l	#3,d0				;$0000,$2000,...,$FE000
		lsr.w	#7,d1
		andi.w	#3,d1				;ring buffer length value
		addq.w	#1,d1				;
		move.l	#$2000,d2			;
		lsl.l	d1,d2				;ring buffer size
		add.l	d2,d0				;dsp work top + work size
		move.l	d0,DFL_INST_addr(a6)		;real time buffer
		movea.l	d0,a0				;
		moveq	#32/2-1,d7			;real time buffer size
		moveq	#0,d0				;
?clearLoop:	move.l	d0,(a0)+			;
		dbra	d7,?clearLoop			;
?false:		rts					;
	.endif			; sw_MODEL_M

;-----------------------------------------------------------------------;
;	dsp initialize & set 						;
;-----------------------------------------------------------------------;
DSP_CPU:	move.b	BIG_timer(a6),d0		;
		andi.b	#3,d0				;8msec ?
		bne	?dspCtrlEnd			;
	;-------------------------------------------------------;
		moveq	#0,d7				;
		move.b	DFL_ELMNT_NO(a6),d7		;element # (0-32)
		move.b	EFCT_CHG_CNT(a6),d0		;efx change execute mode
		beq	?dspCtrlEnd			;
	;------- efx fade out ----------------------------------;
		moveq	#16-1,d7			; loop size
		lea	SCSP_EFSDLPN(a5),a0		;
?efxSendLoop:	move.b	(a0),d1				;
		andi.b	#$E0,d1				;efx level
		beq.s	?200				;
		subi.b	#$20,(a0)			;efx level fade out
	?200:	lea	SCSP_slot_unit(a0),a0		;
		dbra	d7,?efxSendLoop			;
		subq.b	#1,EFCT_CHG_CNT(a6)		;64msec after
		bne	?dspCtrlEnd			;
	;------- program & temporary area clear ----------------;
		lea	DSP_MPRO(a5),a0			;
		moveq	#0,d0				;
		move.w	#$600/256-1,d7			;
?dsp_nop_lp:	move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+

		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+

		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+

		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+

		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+

		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+

		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+

		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		move.l	d0,(a0)+
		dbra	d7,?dsp_nop_lp			;
	;------- dsp work ram initialize -----------------------;
		movea.l	DSP_PRG_top(a6),a0		;dsp program top
		movea.l	DSP_RW_top(a6),a2		;dsp work ram top
		move.l	DSP_RW_sz(a6),d3		;dsp work ram size
		move.l	#$60006000,d0			;
		lsr.l	#4,d3				;
		beq.s	?3dCheck			;
		subq	#1,d3				;
		move.w	d3,d3				;
		beq.s	?3dCheck			;
?fill_6000:	move.l	d0,(a2)+			;
		move.l	d0,(a2)+			;
		move.l	d0,(a2)+			;
		move.l	d0,(a2)+			;
		dbra	d3,?fill_6000			;
	;------- copy 3d efx coef ptr etc ----------------------;
?3dCheck:	move.b	DL_3DM(a0),d0			;
		move.b	d0,bs_SIFWK+OF_3D_FLAG		;
		beq.s	?efxRegClear			;no 3d module
		bpl.s	?skip				;q sound module
		;------- y sound -----------------------;
		move.b	DL_3DC(a0),bs_SIFWK+OF_3DCOEF_PTR	;copy coef ptr
		move.b	DL_3DA(a0),bs_SIFWK+OF_3DADRS_PTR	;copy adrs ptr
		bra.s	?efxRegClear			;
		;------- q sound -----------------------;
?skip:		move.b	DL_3DC(a0),bs_SIFWK+OF_QCOEF_PTR	;copy coef ptr
	;------- efx register clear ----------------------------;
?efxRegClear:	lea	DSP_EFREG(a5),a2		;
		moveq	#0,d0				;
		move.l	d0,(a2)+			;
		move.l	d0,(a2)+			;
		move.l	d0,(a2)+			;
		move.l	d0,(a2)+			;
		move.l	d0,(a2)+			;
		move.l	d0,(a2)+			;
		move.l	d0,(a2)+			;
		move.l	d0,(a2)+			;
	;------- ring buffer address & length set to scsp ------;
		move.l	DSP_RW_top(a6),d5		;dsp work ram top
		move.b	DL_RBL(a0),d0			;ring buffer length(0-3)
		andi.w	#3,d0				;
		lsl.w	#7,d0				;
		lsl.l	#3,d5				;
		swap	d5				;
		or.w	d5,d0				;
		move.w	d0,RG_RBP(a5)			;set to scsp
		move.w	d0,stack_RBLP(a6)		;
	;------- set coef,memory adrs & dsp prg. to scsp -------;
		lea	DL_CEF(a0),a2			;coef data top(.exb)
		lea	DSP_COEF(a5),a3			;set to scsp($100700-$100bff)
		move.w	#$500/256-1,d7			;
?dspRegLoop:	move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+

		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+

		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+

		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+

		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+

		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+

		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+

		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		move.l	(a2)+,(a3)+
		dbra	d7,?dspRegLoop
	;-------------------------------------------------------;
		moveq	#0,d4				;
		move.b	DL_NCT(a0),d4			;coef table #
		move.b	DL_RBL(a0),d2			;ring buffer length(0-3)
		move.l	#$4000,d3			;
		andi.w	#3,d2				;
		beq.s	?shortLen			;
		lsl.l	d2,d3				;d3=ring buffer size
	;-------------------------------------------------------;
?shortLen:	swap	d5				;
		lsr.l	#3,d5				;dsp work ram top
		add.l	d3,d5				;real time buffer top
		move.l	d5,DFL_INST_addr(a6)		;
	;------- real time buffer clear ------------------------;
		movea.l	d5,a0				;
		moveq	#0,d0				;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
		move.l	d0,(a0)+			;
	;------- coef table check (dynamic filter) -------------;
		moveq	#0,d0				;
		add.w	d4,d0				;
		add.w	d4,d0				;
		add.w	d4,d0				;
		add.w	d4,d0				;
		add.w	d4,d0				;
		move.l	d0,d4				;x5
		beq.s	?refreshMixer			;
	;------- copy coef table to dsp coef buffer ------------;
		add.w	d4,d4				;
		add.w	d4,d4				;
		add.w	d4,d4				;
		add.w	d4,d4				;
		subq.w	#1,d4				;
?coefNoLoop:	move.l	(a2)+,(a0)+			;
		move.l	(a2)+,(a0)+			;
		move.l	(a2)+,(a0)+			;
		move.l	(a2)+,(a0)+			;
		move.l	(a2)+,(a0)+			;
		move.l	(a2)+,(a0)+			;
		move.l	(a2)+,(a0)+			;
		move.l	(a2)+,(a0)+			;
		move.l	(a2)+,(a0)+			;
		move.l	(a2)+,(a0)+			;
		move.l	(a2)+,(a0)+			;
		move.l	(a2)+,(a0)+			;
		move.l	(a2)+,(a0)+			;
		move.l	(a2)+,(a0)+			;
		move.l	(a2)+,(a0)+			;
		move.l	(a2)+,(a0)+			;x64(d4x$0a00)
		dbra	d4,?coefNoLoop			;
	;------- refresh efx mixer -----------------------------;
?refreshMixer:	move.l	a2,DFL_ELMNT_addr(a6)		; EXB ELEMENT table top
		jmp	MIXER_wk_wr(pc)			;
?dspCtrlEnd:	rts					;

;-----------------------------------------------------------------------;
;	pitch lfo,pitch envelope & pitch bend control			;
;	don't destroy a4/d4.w/d7.w
;-----------------------------------------------------------------------;
slot_ctrl:	lea	slot_work(a6),a4		;($8000)
		movem.l	a4/a5,-(sp)			;
		moveq	#slot_size-1,d7			;
?slotLoop:	move.b	PSPN(a4),d0			;pcm stream check
		bmi.w	?next_slot			;
	.if ENGN
		move.b	sl_flag1(a4),d0			;engine control check
		bmi.w	?next_slot			;
	.endif			; ENGN
	;------- pitch bend ------------------------------------;
		moveq	#0,d5				;
		move.b	sl_MIDI(a4),d5			;midi channel #
		lsl.w	#5,d5				;x$20
		or.w	_sl_kanri(a4),d5		;+(management#x$200)
		move.w	knr_PBend_BF(a6,d5.w),d5	;pitch bend data
		move.l	sl_layer_adr(a4),d0		;layer address(absolute)
		bne.w	?pLfoCheck			;
		bra.w	?next_slot			;
	;------- pitch control function flag check -------------;
?pLfoCheck:	movea.l	d0,a2				;
		btst.b	#PLON_flg,sl_flag1(a4)		;pitch lfo check
		beq.w	?pEgCheck			;
		bsr	add_PLFO			;calclation for pitch lfo
?pEgCheck:	btst.b	#PEON_flg,sl_flag1(a4)		;pitch envelope check
		beq.w	?slot_ctrl_2			;
		bsr	add_PEG				;
	;------- same as old check & set freq. to scsp ---------;
?slot_ctrl_2:	cmp.w	sl_tune(a4),d5			;
		bne.w	?setToScsp			;
		bra.w	?next_slot			;
?setToScsp:	move.w	d5,sl_tune(a4)			;
		bsr	send_frequency			;d5.w
	;------- next slot -------------------------------------;
?next_slot:	adda.w	#$0040,a4			;
		adda.w	#$0020,a5			;
		dbra	d7,?slotLoop			;
		movem.l	(sp)+,a4/a5			;
		rts					;

;-----------------------------------------------------------------------;
;	calculation for pitch lfo delta scale per 4msec			;
;	--------------------------------------------------------	;
;	in	d4.w	slot offset ( $00,$20 Å` $7C0 )			;
;		d5.l	detune value					;
;		a4	slot work top					;
;	out	d5.w	detune value (added lfo delta scale)		;
;			$xxyy( xx=scale,yy=cent )			;
;-----------------------------------------------------------------------;
add_PLFO:	move.w	PLFO_Delay(a4),d0		;delay ctr(per 1msec)
		beq.w	add_PLFO_1			;
		subq.w	#_4msec,d0			;4msec dec.
		bcc.s	add_PLFO_2			;
		moveq	#0,d0				;delay end
add_PLFO_2:	move.w	d0,PLFO_Delay(a4)		;
		rts					;
	;-------------------------------------------------------;
	;	slope start					;
	;-------------------------------------------------------;
add_PLFO_1:	move.w	PLFO_HTCNT_wk(a4),d0		;reverse ctr(per 1msec)
		subq.w	#_4msec,d0			;4msec dec.
		bpl.w	addDataToBase			;
	;------- turn slope ------------------------------------;
		move.w	d0,d2				;copy reverse counter
		add.w	PLFO_HTCNT_bs(a4),d0		;
		move.w	d0,PLFO_HTCNT_wk(a4)		;
		moveq	#0,d1				;next increase data=0
		move.w	PLFO_FDCNT(a4),d0		;fade time(per 4msec)
		beq.s	?noFade				;no fade
	;------- fade mode -------------------------------------;
		subq.w	#1,d0				;fade counter dec.
		move.w	d0,PLFO_FDCNT(a4)		;fade time
		move.l	PLFO_FRQR_bs(a4),d1		;
		neg.l	d1				;
		move.l	d1,PLFO_FRQR_bs(a4)		;
		neg.l	d1				;
	;-------------------------------------------------------
?noFade:	move.l	PLFO_FRQR_wk(a4),d6		;freq. add data
		move.l	PLFO_cent(a4),d0		;base detune data
		addq.w	#_4msec,d2			;reverse ctr. overflow value
		andi.w	#3,d2				;
		add.w	d2,d2				;
		jmp	add_PLFO_JPTB(pc,d2.w)		;
	;-------------------------------------------------------;
	;	overflow value					;
	;-------------------------------------------------------;
add_PLFO_JPTB:	bra.s	?noOver				;no overflow
		bra.s	?over1				;1
		bra.s	?over2				;2
	;------- overflow 3 ------------------------------------;
		add.l	d6,d0				;
		add.l	d6,d0				;
		add.l	d6,d0				;+(add data x 3)
		add.l	d1,d6				; FRQR_wk <-- Å{ FRQR_bs
		neg.l	d6				;
		bra.s	?addDataSet3			;
	;------- overflow 2 ------------------------------------;
?over2:		add.l	d6,d0				;
		add.l	d6,d0				;+(add data x 2)
		add.l	d1,d6				; FRQR_wk <-- Å{ FRQR_bs
		neg.l	d6				;
		bra.s	?addDataSet2			;
	;------- overflow 1 ------------------------------------;
?over1:		add.l	d6,d0				;+(add data)
		add.l	d1,d6				; FRQR_wk <-- Å{ FRQR_bs
		neg.l	d6				;
		bra.s	?addDataSet1			;
	;------- no overflow -----------------------------------;
?noOver:	add.l	d1,d6				; FRQR_wk <-- Å{ FRQR_bs
		neg.l	d6				;
		add.l	d6,d0				;-(add data x 4) -(inc data x 4)
?addDataSet1:	add.l	d6,d0				;-(add data x 2) -(inc data x 3)
?addDataSet2:	add.l	d6,d0				;		 -(inc data x 2)
?addDataSet3:	add.l	d6,d0				;+(add data x 2) -(inc data x 1)
	;------- next add data set -----------------------------;
		move.l	d6,PLFO_FRQR_wk(a4)		;
		move.l	d0,PLFO_cent(a4)		;
		bra.s	addToBase			;
	;-------------------------------------------------------;
addDataToBase:	move.w	d0,PLFO_HTCNT_wk(a4)		;decremented reverce ctr.
		move.l	PLFO_FRQR_wk(a4),d6		;add data
		add.l	d6,d6				;
		add.l	d6,d6				;add data x4(for 4msec)
		add.l	d6,PLFO_cent(a4)		;
addToBase:	add.w	PLFO_cent(a4),d5		;add to detune base
		rts					;

;-----------------------------------------------------------------------;
;	pitch envelope generator control				;
;	--------------------------------------------------------	;
;	in	a2	destination layer data top addr			;
;		a4	slot work top					;
;		d5.w	detune value					;
;	out	d5.w	detune value (added eg delta scale)		;
;			$xxyy( xx=scale,yy=cent )			;
;	break   a1,d0,d1,d3						;
;-----------------------------------------------------------------------;
			.extern		get_EXPTB2
			.extern		get_LEVEL
add_PEG:	moveq	#0,d0				;
		move.b	PEG_SEG(a4),d0			;segment #
		move.l	#$80000000,d1			;
		andi.w	#7,d0				;0,1,2,3,...,7
		add.w	d0,d0				;
		add.w	d0,d0				;
		jmp	PEGJPTB(pc,d0.w)		;
PEGJPTB:	rts					;
		rts					;0:no pitch eg
		bra	PEG_1				;1:delay
		bra	PEG_2				;2:attack
		bra	PEG_3				;3:decay
		bra	PEG_4				;4:sustain
		bra	PEG_5				;5:stay
		bra	PEG_6				;6:rerease
		bra	PEG_7				;7:hold
	;-------------------------------------------------------;
	;	read for AR,AL					;
	;-------------------------------------------------------;
PEG_1:		subq.w	#_4msec,PEG_dly_cnt(a4)		;
		bcc.s	add_PEG_exit			;now in delay
		movea.l	PEG_addr(a4),a1			;
		addq	#2,a1				;ready a1 for[AR],[AL]
		bra.s	PEG_next_seg			;
	;-------------------------------------------------------;
	;	read for DR,DL					;
	;-------------------------------------------------------;
PEG_2:		movea.l	PEG_addr(a4),a1			;
		addq	#4,a1				;ready a1 for[DR],[DL]
		bra.s	PEG_23				;
	;-------------------------------------------------------;
	;	read for SR,SL					;
	;-------------------------------------------------------;
PEG_3:		movea.l	PEG_addr(a4),a1			;
		addq	#6,a1				;ready a1 for[SR],[SL]
		bra.s	PEG_23				;
	;-------------------------------------------------------;
	;	read for RR,RL					;
	;-------------------------------------------------------;
PEG_4:		movea.l	PEG_addr(a4),a1			;
		addq	#8,a1				;ready a1 for[SR],[SL]
	;-------------------------------------------------------;
PEG_23:		move.l	PEG_cent(a4),d0			;
		move.l	PEG_level(a4),d2		;
		add.l	d1,d0				; +$8000
		add.l	d1,d2				; +$8000
		move.l	PEG_RATE(a4),d6			;
		bpl.s	?peg_rate_pls			;
	;------- minus -----------------------------------------;
		add.l	d6,d0				; _cent + _RATE
		cmp.l	d0,d2				; new cent <cmp> level
		bcs.s	PEG_2A				;
		bra.s	?peg_rate_mp			;
	;------- plus ------------------------------------------;
?peg_rate_pls:	add.l	d6,d0				; _cent + _RATE
		cmp.l	d2,d0				; level <cmp> new cent
		bcs.s	PEG_2A				;
	;-------------------------------------------------------;
?peg_rate_mp:	sub.l	d1,d2				; -$8000
		move.l	d2,PEG_cent(a4)			;
	;-------------------------------------------------------;
PEG_next_seg:	addq.b	#1,PEG_SEG(a4)			;
		move.b	(a1)+,d0			;read rate
		bne.s	?100				;
		move.b	(a1)+,d0			;read level
		beq	add_PEG				;
?100:		bsr	get_EXPTB2			;data^2
		swap	d0				;
		clr.w	d0				;
		asr.l	#4,d0				; for 4msec
		move.l	d0,PEG_RATE(a4)			;-$03f01000 to +$03f01000
		move.b	(a1),d0				;read level
		jsr	get_LEVEL(pc)			;d0.w = -$5FFF to +$5FFF
		swap	d0				;
		clr.w	d0				;
		move.l	d0,PEG_level(a4)		;-$5fff0000 to +$5fff0000
		bra.s	add_PEG_exit			;
	;-------------------------------------------------------;
PEG_2A:		sub.l	d1,d0				;-$8000
		move.l	d0,PEG_cent(a4)			;
		bra.s	add_PEG_exit			;
	;-------------------------------------------------------;
PEG_5:
	;-------------------------------------------------------;
PEG_7:		nop					;
add_PEG_exit:	add.w	PEG_cent(a4),d5			;
		rts					;
	;-------------------------------------------------------;
PEG_6:		move.l	PEG_cent(a4),d0			;
		move.l	PEG_level(a4),d2		;
		add.l	d1,d0				; +$8000
		add.l	d1,d2				; +$8000
		move.l	PEG_RATE(a4),d6			;
		bpl.s	PEG_6D				;
PEG_6_mi:	add.l	d6,d0				; _cent + _RATE
		cmp.l	d0,d2				; new cent <cmp> level
		bcs.s	PEG_6B				;
		bra.s	PEG_6C				;
PEG_6D:		add.l	d6,d0				; _cent + _RATE
		cmp.l	d2,d0				; level <cmp> new cent
		bcs.s	PEG_6B				;
PEG_6C:		sub.l	d1,d2				; -$8000
		move.l	d2,PEG_cent(a4)			;
		addq.b	#1,PEG_SEG(a4)			;
		bra.s	PEG_6A				;
PEG_6B:		sub.l	d1,d0				; -$8000
		move.l	d0,PEG_cent(a4)			;
PEG_6A:		add.w	PEG_cent(a4),d5			;
		rts					;

;-----------------------------------------------------------------------;
;	level 6 midi b port(ym8302) interrupt (model m only)		;
;-----------------------------------------------------------------------;
	.if sw_MODEL_M
			.extern		MIDI_d0_ctrl
			.public		level_6
level_6:	movem.l	d0-d7/a0-a6,-(sp)		;
		lea	bs_PWKTP,a6			; 68K Prg work top address
		ori.b	#%10000000,LED_status(a6)	;
		move.b	#MIDI_LED_speed,RCVA_LED_cnt(a6)
		movea.l	#IO_MIDI1+1,a0			; ready a0 @005
		move.b	R00(a0),d0			; IRQ vector read
		andi.w	#00011110B,d0			; = 0,2,4,6,8,A,C,E
		cmpi.b	#$0A,d0				; FIFO-Rx ready ?
		bne.s	?level_6_exit			; jump if no
		move.b	#3,R01(a0)			;
		move.b	Rx4(a0),d0			; R34 = (RD) FIFO-Rx status
		andi.b	#01110000B,d0			; error ?
		beq.s	?MIDI_RX_pass			;
		move.b	Rx6(a0),d0			; dammy read
		bra.s	?level_6_exit			;
?MIDI_RX_pass:	move.b	Rx6(a0),d0			; R36 = (RD) FIFO-Rx data
		bsr.w	MIDI_d0_ctrl			;
?level_6_exit:	movem.l	(sp)+,d0-d7/a0-a6		;
		rte					;
	.endif
;-----------------------------------------------------------------------;
;	level 5 midi a port(ym8302) interrupt (model m only)		;
;-----------------------------------------------------------------------;
			.public		level_5
	.if sw_MODEL_M
level_5:	movem.l	d0/a0/a6,-(sp)			;
		lea	bs_PWKTP,a6			;($7000)
		ori.b	#%01000000,LED_status(a6)	;set led(level 5 int)
		move.b	#MIDI_LED_speed,RCVB_LED_cnt(a6)
		lea	IO_MIDI2+1,a0			; ready a0 @005
		move.b	R00(a0),d0			; IRQ vector read
		move.b	#3,R01(a0)			;
		move.b	Rx6(a0),d0			; dammy read
		movem.l	(sp)+,d0/a0/a6			;
		rte					;
	.else			; sw_MODEL_M
level_5:	nop					;
		rte					;
	.endif			; sw_MODEL_M

;-----------------------------------------------------------------------;
;	level 2 timer b interrupt (2msec)				;
;-----------------------------------------------------------------------;
			.public		level_2
level_2:	movem.l	d0-d7/a0-a6,-(SP)		;
	;-------------------------------------------------------;
	.if SCSI_RAM_chk
		moveq	#$80/4-1,d7			;
		lea	$800,a0				;
		lea	$F3FE00,a1			;
SCSI_chk_lp:	move.l	(a1)+,d0			;
		move.l	(a0),d1				;
		move.l	d0,(a0)+			;
		cmp.l	d0,d1				;
		beq.s	SCSI_chk_1			;
		nop					;
SCSI_chk_1:	dbra	d7,SCSI_chk_lp			;
	.endif			; SCSI_RAM_chk
	;-------------------------------------------------------;
		lea	IO_SCSP,a5			;($100000)
		lea	bs_PWKTP,a6			;($7000)
		move.w	#TIMB_VL,RG_TBCTL(a5)		;2msec
		move.w	#INT_RST,RG_SCIRE(a5)		;timer b reset
		addi.w	#SEQ_LED_speed,SEQ_LED_cnt(a6)	;led set for timer b int
		addi.w	#$da,FE_count(a6)		;
		addq.w	#1,COMHIS_cnt(a6)		; HOST Command HISTORY counter
		bsr	xseq_2msec			;
	.if SW_TITAN
		bsr	titan_coin			;
	.endif			; SW_TITAN

;-----------------------------------------------------------------------;
;	rerease level check			by A.Miyazawa		;
;	--------------------------------------------------------	;
;	slot_status format						;
;		bit15-8	0xff:FM						;
;		bit 7	1:pcm stream in use				;
;		bit 6	1:engine control in use				;
;		bit 5							;
;		bit 4							;
;		bit 3	1:note on					;
;		bit 2-0	rerease level					;
;-----------------------------------------------------------------------;
	.if rereaseCheck
release_check:	movea.l	#scsp_release_flag,a0		;secret register
		movea.l	#bs_PWKTP+slot_work,a1		;work
		movea.w	#status_counter,a2		;midi ch# counter
		movea.w	#test_work,a3			;monitor work(0x7c0)
		movea.l	#SCSP_FH_RR+IO_SCSP,a5		;
		moveq	#0,d0				;
		moveq	#0,d1				;
		moveq	#0,d2				;
	;-------
		move.w	(a2),d1				;read slot #
		add.w	d1,d1				;
		add.w	d1,d1				;
		add.w	d1,d1				;
		add.w	d1,d1				;
		add.w	d1,d1				;
		adda.w	d1,a5				;
		add.w	d1,d1				;
		adda.w	d1,a1				;
		move.w	slot_status(a1),d0		;
		andi.w	#%0000000011000000,d0		;pcm stream,engine check
		bne.s	?nextSlot			;
	;-------
		move.b	1(a0),d0			;read release data from monitor reg.
		eori.w	#%0000000000011100,d0		;release level.ïâò_óùÇ»ÇÃÇ≈îΩì]ÅB
		lsr.w	#2,d0				;
		andi.w	#%0000000000000110,d0		;
		move.w	slot_status(a1),d1		;
		andi.w	#%1111111100001000,d1		; ÉèÅ[ÉNÇ©ÇÁÉmÅ[ÉgÉIÉìÉrÉbÉgÇéÊÇËèoÇ∑ÅB
		or.w	d1,d0				;
		tst.b	d0				; ÉmÅ[ÉgÇÃèÛë‘Ç∆ÉäÉäÅ[ÉXÇÉ`ÉFÉbÉN.
		beq.s	?200				; jp when slot closed.
		btst.l	#3,d0				;
		bne.s	?100				;jp when "note on" bit in progress.
		subq.w	#1,release_counter(a1)		;*see the program document!
		bne.s	?100				;
		ori.b	#$1f,(a5)			;
?200:		moveq	#0,d0				;note offÇ≈release lv=0Ç»ÇÁFM flagÇclear
?100:		move.w	d0,slot_status(a1)		;åãâ Çäiî[.ÅiÇOà»äOÇÃÇ∆Ç´Ç‹Çæî≠ê∫íÜÅj
?nextSlot:	move.w	(a2),d1				;slot #
		addq.w	#1,d1				;next slot #
		andi.w	#$001f,d1			;#$00-$1f
		move.w	d1,(a2)				;slot # store
		add.w	d1,d1				;
		add.w	d1,d1				;
		add.w	d1,d1				;
		move.b	d1,(a0)				;set slot # monitor register
	;-------
		movea.l	#bs_PWKTP+slot_work+slot_status,a1
		moveq	#slot_size-1,d7			;
?slot_loop:	move.w	(a1),d0				;
		move.b	d0,(a3)+			;
		adda.w	#$0040,a1			;
		dbra	d7,?slot_loop			;
	.endif			; rereaseCheck
		movem.l	(sp)+,d0-d7/a0-a6		;
		rte					;

;-----------------------------------------------------------------------;
;	coin counter for st-v						;
;-----------------------------------------------------------------------;
	.if SW_TITAN
titan_coin:	lea	TTN_SW_DRAM,a0			;
		tst.b	TTN_flag68(a0)			;
		beq.s	sw_ex				;
		move.b	#1,TTN_execute(a0)		;
		move.w	IO_PORT_C,d0			;
		move.b	d0,d1				;
		and.b	#$3F,d0				;
		tst.b	TTN_coin_sw_md(a0)		;
		beq.s	sw_01				;
		lsl.b	#4,d1				;
		bra.s	sw_00				;
sw_01:		lsl.b	#2,d1				;
sw_00:		and.b	#$C0,d1				;
		or.b	d1,d0				;
		move.b	TTN_old_sw(a0),d1		;
		move.b	d0,TTN_old_sw(a0)		;
		eor.b	d0,d1				;
		and.b	d0,d1				;
		rol.b	#4,d1				;
		and.w	#$3C,d1				;
		move.l	swcnttbl(pc,d1.w),d1		;
		add.l	d1,TTN_off_count(a0)		;
		move.b	#0,TTN_execute(a0)		;
sw_ex:		rts					;
	;-----------------------------------------------;
swcnttbl:	dc.l	00000000H,00000100H,00000001H,00000101H
		dc.l	01000000H,01000100H,01000001H,01000101H
		dc.l	00010000H,00010100H,00010001H,00010101H
		dc.l	01010000H,01010100H,01010001H,01010101H
	;-----------------------------------------------;
	.endif			; SW_TITAN

;-----------------------------------------------------------------------;
;	direct mixer data change from tone editor			;
;-----------------------------------------------------------------------;
	.if	sw_MODEL_M
MAC_MIXER:	move.b	BIG_timer(a6),d0		;16x4msec
		andi.b	#$0F,d0				;
		bne	?mac_mixer_pass			;
	;-------
		move.l	_activ_MIX_BNK(a6),d0		;active tone bank address
		beq	?mac_mixer_pass			;
		movea.l	d0,a0				;
		move.w	_activ_MIX_NO(a6),d0		;active mixer #
		movea.l	a0,a1				;
		adda.w	BIN_VL(a0),a1			; = V-L data top
		adda.w	BIN_MIX(a0),a0			; = MIXER top addr
		add.w	d0,d0				;
		adda.w	d0,a0				; +  2*Mixer#
		lsl.w	#3,d0				; + 16*Mixer#
		adda.w	d0,a0				; = desti. Mixer addr
		cmpa.l	a1,a0				;
		bcc	?mac_mixer_pass			;
		movea.l	a0,a1				;
		lea	mixer_wk_edt(a6),a2		;
		cmpm.l	(a1)+,(a2)+			;
		bne.s	?mac_mixer_wr			;
		cmpm.l	(a1)+,(a2)+			;
		bne.s	?mac_mixer_wr			;
		cmpm.l	(a1)+,(a2)+			;
		bne.s	?mac_mixer_wr			;
		cmpm.l	(a1)+,(a2)+			;
		bne.s	?mac_mixer_wr			;
		cmpm.w	(a1)+,(a2)+			;
		beq	?mac_mixer_pass			;
	;-------------------------------------------------------;
?mac_mixer_wr:	lea	mixer_wk_edt(a6),a2		;
		lea	mixer_wk_SCSP(a6),a1		;
		move.l	(a0),(a1)+			;
		move.l	(a0)+,(a2)+			;
		move.l	(a0),(a1)+			;
		move.l	(a0)+,(a2)+			;
		move.l	(a0),(a1)+			;
		move.l	(a0)+,(a2)+			;
		move.l	(a0),(a1)+			;
		move.l	(a0)+,(a2)+			;
		move.w	(a0),(a1)+			;
		move.w	(a0)+,(a2)+			;
		lea	mixer_wk_SCSP(a6),a0		;
		moveq	#18-1,d7			; loop size
		move.b	SND_OUT_ST(a6),d4		; MONO/STEREO status
		bmi.s	?mac_mixer_mono			;
	;------- stereo mode -----------------------------------;
		clr.w	d4				;
?mac_mix_wr1_lp:
		move.b	(a0)+,SCSP_EFSDLPN(a5,d4.w)	;
		addi.w	#SCSP_slot_unit,d4		;
		dbra	d7,?mac_mix_wr1_lp		;
		rts					;
	;------- mono mode -------------------------------------;
?mac_mixer_mono:
		clr.w	d4				;
?mac_mix_wr2_lp:
		move.b	(a0)+,d0			;
		andi.b	#$E0,d0				;to center
		move.b	d0,SCSP_EFSDLPN(a5,d4.w)	;
		addi.w	#SCSP_slot_unit,d4		;
		dbra	d7,?mac_mix_wr2_lp		;
?mac_mixer_pass:
		rts					;
	.endif			; sw_MODEL_M
;-----------------------------------------------------------------------;
;	scsp midi out on/off check					;
;-----------------------------------------------------------------------;
			.extern		MIDI_ctrl_pt
midiOutModeCheck:
		move.b	bs_SIFWK+OF_HNDSHK_flg,d0	;host i/f hand shake flag
		add.b	d0,d0				;bit6 check
		bmi.s	?end				;
		lea	MIDI_ctrl_pt(pc),a0		;
		move.l	#$4E714E71,(a0)			;set nop,nop
?end:		rts					;

;-----------------------------------------------------------------------;
;	cr+lf code check (text convert check)				;
;-----------------------------------------------------------------------;
CR_check:	lea	CR_TB(pc),a0			;
		cmpi.b	#$0D,(a0)+			;cr
		bne.w	?error				;
		cmpi.b	#$0A,(a0)+			;lf
		bne.w	?error				;
		cmpi.b	#$0D,(a0)+			;cr
		bne.w	?error				;
		cmpi.b	#$0A,(a0)+			;lf
		beq.w	?end				;
?error:		bset.b	#ERRb20,Mem_err_bit+1		;media copy miss
?end:		rts					;
	;------- cr+lf code ------------------------------------;
CR_TB:		dc.b	$0D,$0A,$0D,$0A			;

;-----------------------------------------------------------------------;
;	engine control							;
;-----------------------------------------------------------------------;
	.if	ENGN
ENGN_CTRL:	lea	ENGN_wk_cmn(a6),a3		;
		move.l	ENGN_wk_TONE(a3),d0		;tone start address check
		beq.w	?end				;
		lea	ENGN_slot_TB(pc),a0		;scsp slot register ptr table
		lea	racing_interface,a1		;host interface for engine
		movea.l	(racing_interface+ENGN_TB_addr),a2
		move.w	ENGN_P1_rpm(a1),d0		;rpm ($0000-$2260)
		beq	ENGN_all_off_s			;engine off
		move.b	ENGN_P1_rpm(a1),d0		;rpm ($00H-$22)
		cmp.b	ENGN_wk_rpm(a3),d0		;compare old rpm data
		bne.s	?engn_ctrl_3			;if renewal then skip
		move.l	ENGN_P1_accl(a1),d1		;
		cmp.b	ENGN_wk_accl(a3),d1		; compare old accel data
		beq.w	?end				;if same then skip
		andi.b	#$0F,d1				;accel data
?engn_ctrl_3:	cmpi.b	#$23,d0				;
		bcs.w	?engn_ctrl_2			;
		move.b	#$22,d0				;rpm max data
?engn_ctrl_2:	move.b	d0,ENGN_wk_rpm(a3)		;refresh rpm work
		move.b	d1,ENGN_wk_accl(a3)		;refresh accel work
		moveq	#16-1,d7			;
?engine_control_loop:
		bsr.w	ENGN_exe			;
		lea	ENGN_TB_unit(a2),a2		; + $80 : next ENGIN PCM
		dbra	d7,?engine_control_loop		;
?end:		rts					;

;-----------------------------------------------------------------------;
;	<<<< âÒì]êî = ïœâª >>>>					;
;	in	a0	scsp slot reg pointer table top			;
;		a1	host interface work for engine control($6f00)	;
;		a2	engine information data pointer($7f000-)	;
;		a3	common work for engine control($8860)		;
;-----------------------------------------------------------------------;
ENGN_exe:	clr.w	d2				;
		move.l	ENGN_P1_accl(a1),d0		;read accel data
		andi.w	#$0F,d0				;
		move.b	ENGN_TB_accl(a2,d0.w),d2	;read total level shift data
		clr.w	d0				;
		move.b	ENGN_wk_rpm(a3),d0		;read rpm data($00-$22)
		moveq	#0,d1				;
		move.b	ENGN_TB_rpm(a2,d0.w),d1		;read total level data
		add.w	d0,d0				;
		move.w	ENGN_TB_pitch(a2,d0.w),d3	;read pitch data
		beq.w	Pitch_00			;engine off
	;------- pitch change ----------------------------------;
		move.b	ENGN_TB_flg(a2),d0		;engine in-use check
		bmi.w	ENGN_running			;
		clr.w	d0				;
		moveq	#8-1,d6				;
?slotLoop:	move.w	(a0,d0.w),d4			;in-use flag check
		bpl.w	ENGN_slt_find			;
		addq	#2,d0				;
		dbra	d6,?slotLoop			;
		rts					;
	;-------------------------------------------------------;
	;	
	;-------------------------------------------------------;
ENGN_slt_find:	bset.b	#7,(a0,d0.w)			;set in use flag
		move.w	d4,ENGN_TB_d4(a2)		;store scsp slot pointer
	;-------------------------------------------------------;
	;	engine tone data set to scsp			;
	;-------------------------------------------------------;
		movea.l	ENGN_TB_Layer(a2),a4		;layer top abs. address
		addq.l	#2,a4				;
		move.l	(a4)+,d0			;
		andi.l	#$07FFFFFF,d0			;
		add.l	ENGN_wk_TONE(a3),d0		;tone start address
		move.l	d0,SCSP_KXKB(a5,d4.w)		;start addres set to scsp
		move.l	(a4)+,SCSP_PCM_LSA(a5,d4.w)	;loop start set to scsp
		move.l	(a4)+,SCSP_D2R1R(a5,d4.w)	;[D2R] ...
		addq.l	#6,a4				;
		move.b	#$FF,SCSP_TLVL(a5,d4.w)		;
		move.l	(a4)+,SCSP_RELFO(a5,d4.w)	;
		move.w	(a4)+,SCSP_DISDLPN(a5,d4.w)	;
		bsr.w	ENGN_PITCH_WR			;oct/fns set to scsp
		bsr.w	ENGN_TL_WR			;tl set to scsp
ENGN_on:	bset.b	#7,ENGN_TB_flg(a2)		;set in-use flag
		move.b	(a5,d4.w),d0			;
		ori.b	#%00011000,d0			;
		move.b	d0,(a5,d4.w)			;set key-on to scsp
		rts					;
	;-------------------------------------------------------;
	;	in	d1.w	total level			;
	;		d2.w	level shift			;
	;		d3.w	oct/fns(0Å`3FFFH)		;
	;-------------------------------------------------------;
ENGN_running:	bsr.w	ENGN_PITCH_WR			;oct/fns set to scsp
	;------- total level set -------------------------------;
ENGN_TL_WR:	moveq	#0,d0				;
		ext.w	d2				;
		not.w	d2				;
		add.w	d2,d1				;new total level
		bmi.w	?TL_max				;
		not.b	d0				;
		cmpi.w	#$100,d1			;
		bcc.w	?TL_max				;
		move.b	d1,d0				;
?TL_max:	move.b	d0,SCSP_TLVL(a5,d4.w)		;write to scsp
		rts					;
	;-------------------------------------------------------;
	;	oct/fns set					;
	;-------------------------------------------------------;
ENGN_PITCH_WR:	move.w	ENGN_TB_d4(a2),d4		;
		andi.w	#$3FFF,d3			; 0000H to 2000H(44.1kHz) to 3FFFH
		subi.w	#$2000,d3			; -2000HÅ`0Å`+1FFFH
		move.w	d3,d0				;
		andi.w	#$3FF,d0			; = FNS
		andi.w	#$3C00,d3			; = OCT
		add.w	d3,d3				;
		or.w	d0,d3				;
		move.w	d3,SCSP_OCTFNS(a5,d4.w)		;write to scsp
		rts					;
	;-------------------------------------------------------;
	;	
	;-------------------------------------------------------;
Pitch_00:	bclr.b	#7,ENGN_TB_flg(a2)		;bit7 = PCM on
		beq.w	pitch_00_ret			;
ENGN_off:	move.w	ENGN_TB_d4(a2),d4		;slot reg. pointer
		move.b	SCSP_KXKB(a5,d4.w),d0		;read scsp
		andi.b	#%00000111,d0			;
		ori.b	#%00010000,d0			;
		move.b	d0,SCSP_KXKB(a5,d4.w)		;key off
		andi.w	#$0E0,d4			; 0,$20,$40,...,$E0
		lsr.w	#8,d4				; 0,2,4,...,$E
		bclr.b	#7,(a0,d4.w)			; a0 = ENGN_slot_TB
pitch_00_ret:	rts					
;---------------------------------------------------------------;
;	engine off						;
;	in	a0	ENGN_slot_TB				;
;		a5	IO_SCSP					;
;		d0	rpm=0					;
;---------------------------------------------------------------;
ENGN_all_off_s:	st.b	ENGN_wk_rpm(a3)			;set $ff
		moveq	#0,d1				;
		moveq	#16-1,d7			;
?300:		move.b	d1,ENGN_TB_flg(a2)		;clear slot in-use flag
		move.w	d1,ENGN_TB_d4(a2)		;clear slot reg. pointer
		lea	ENGN_TB_unit(a2),a2		;next engine pcm(+$80)
		dbra	d7,?300				;
	;------- Engine slots all key-off >>>>
		moveq	#8-1,d7				;
?200:		bclr.b	#7,(a0)				;now key-on ?
		beq.w	?100				;
		move.w	(a0),d4				;
		move.b	SCSP_KXKB(a5,d4.w),d0		;
		andi.b	#%00000111,d0			;
		ori.b	#%00010000,d0			;
		move.b	d0,SCSP_KXKB(a5,d4.w)		;set key-off to scsp
?100:		addq.l	#2,a0				;
		dbra	d7,?200				;
		rts					;

;-----------------------------------------------------------------------;
;	engine control initialize (call from map change)		;
;	in	d1:
;-----------------------------------------------------------------------;
ENGN_init:
	;-------------------------------------------------------;
	;	ENGN_wk_cmn initial				;
	;-------------------------------------------------------;
		moveq	#0,d0				;bank #0
		bsr	get_BANK_addr			;start address
		bcs.w	?end				;not find tone bank
		lea	ENGN_wk_cmn(a6),a3		;
		st.b	ENGN_wk_rpm(a3)			;set $ff
		move.l	d2,ENGN_wk_TONE(a3)		;set start address
	;-------------------------------------------------------;
	;	Engine table work initial			;
	;-------------------------------------------------------;
		movea.l	d2,a0				;tone data start address
		adda.w	8(a0),a0			;voice #0 top
		addq.l	#4,a0				;layer #1 top
		move.l	a0,d1				;
		movea.l	(racing_interface+ENGN_TB_addr),a2	;
		moveq	#0,d0				;
	;-------------------------------------------------------;
		moveq	#16-1,d7			;
?eng_init_lp_2:	move.l	d1,ENGN_TB_Layer(a2)		;voice #0 layer #x top addr
		move.w	d0,ENGN_TB_d4(a2)		;clear slot reg. pointer
		move.b	d0,ENGN_TB_flg(a2)		;clear slot in use flag
		lea	ENGN_TB_unit(a2),a2		;next engine pcm(+$80)
		addi.l	#32,d1				;next layer top
		dbra	d7,?eng_init_lp_2		;
	;-------------------------------------------------------;
		lea	slot_work(a6),a3		;($8000)
		lea	slot_wk_unit*24(a3),a3		;($8600)
		lea	ENGN_slot_TB(pc),a0		;
		move.w	#$300,d4			;
		moveq	#8-1,d7				;use slot numbers
?slotLoop:	bset.b	#ENGN_flg,sl_flag1(a3)		;set engine use flag each slots
		bset.b	#engineInUse,slot_status(a3)	;		"
		lea	slot_wk_unit(a3),a3		;next slot
		move.w	d4,(a0)+			;
		addi.w	#SCSP_slot_unit,d4		;next scsp slot
		dbra	d7,?slotLoop			;
?end:		rts					;

;-----------------------------------------------------------------------;
;	scsp slot register pointer table				;
;	bit15	1:engine in use						;
;-----------------------------------------------------------------------;
ENGN_slot_TB:	dc.w	$0300				;slot #25
		dc.w	$0320				;slot #26
		dc.w	$0340				;slot #27
		dc.w	$0360				;slot #28
		dc.w	$0380				;slot #29
		dc.w	$03A0				;slot #30
		dc.w	$03C0				;slot #31
		dc.w	$03E0				;slot #32
	.endif			; ENGN

;=======================================================================;
;	end of file							;
;=======================================================================;
