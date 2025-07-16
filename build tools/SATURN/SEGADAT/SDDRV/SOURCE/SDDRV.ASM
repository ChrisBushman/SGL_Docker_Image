;=======================================================================;
;	SEGASATURN sound driver						;
; file name								;
;	sdcom.asm							;
; copyright								;
;	SEGA ENTERPRISES						;
; purpose								;
;	sequence control						;
; author(s)								;
;	1995-11-14	A.Miyazawa	Ver.2.00			;
; mod history								;
;	1996-02-23	A.Miyazawa	Ver.2.06			;
;	1996-07-02	A.Miyazawa	Ver.2.10L			;
;	1997-06-30	Y.Kashima	Ver.2.20			;
;=======================================================================;
		.include	scsp.lib

			.extern	com_seq_noteoff		;sdcom
			.extern	com_eventout		;sdcom
			.extern	com_set_seqmode		;sdcom
			.extern	com_notecue_set		;sdcom
			.extern	com_get_workaddress	;sdcom
			.extern	volume_analize		;sdcom
			.extern	pcm_address_search	;sdcom
			.extern	com_set_status		;sdcom
			.extern	ret_error		;sdmain
			.extern	sq_decoder		;dec2
			.extern	dsp3d_op		;dsp3d

;-----------------------------------------------------------------------;
;	sequence decoder 2msec interval control				;
;	in	none							;
;	out	none							;
;	break	d0-d7,a0-a6						;
;-----------------------------------------------------------------------;
			.public	xseq_2msec
xseq_2msec:	bsr	com_get_workaddress		;a4=9800/a5=9c00/a6=9900
	;-------------------------------------------------------;
	;	3d sound module check				;
	;-------------------------------------------------------;
		movem.l	d4/a0,-(sp)			;
		movea.w	#bs_SIFWK,a0			;a0:$0480
		move.b	OF_3D_FLAG(a0),d4		;
		andi.b	#$80,d4				;Y-SOUND use check
		beq.w	?skip1				;
		bsr	dsp3d_op			;
?skip1:		movem.l	(sp)+,d4/a0			;
	;-------------------------------------------------------;
	;	cd audio volume anarize control			;
	;-------------------------------------------------------;
		move.b	SY_TVOLCTL(a4),d0		;volume analize control
		bpl.w	?skip2				; test bit7=SY_VANCTL_BITPOS
		bsr	volume_analize			; CD/DA input volume analize
	;-------------------------------------------------------;
	;	pcm play block renewal check			;
	;		and interrupt request to main cpu	;
	;-------------------------------------------------------;
?skip2:		bsr	pcm_address_search		;
	;-------------------------------------------------------;
	;	sequence control				;
	;-------------------------------------------------------;
		move.w	#NC_NUM-1,d7			; max 32 cue
?loop:		bsr	noteoncue_control		; === note on cue control ===
		lea	NC_SIZ(a5),a5			;next address
		dbra.w	d7,?loop			;
		bsr	com_get_workaddress		;a4=9800/a5=9c00/a6=9900
		;------- management #0 -----------------;
		bsr	sequence_control		;
		;------- management #1 -----------------;
		lea	SQ_SIZ(a6),a6			;next management # work
		bsr	sequence_control		;
		;------- management #2 -----------------;
		lea	SQ_SIZ(a6),a6			;
		bsr	sequence_control		;
		;------- management #3 -----------------;
		lea	SQ_SIZ(a6),a6			;
		bsr	sequence_control		;
		;------- management #4 -----------------;
		lea	SQ_SIZ(a6),a6			;
		bsr	sequence_control		;
		;------- management #5 -----------------;
		lea	SQ_SIZ(a6),a6			;
		bsr	sequence_control		;
		;------- management #6 -----------------;
		lea	SQ_SIZ(a6),a6			;
		bsr	sequence_control		;
		;------- management #7 -----------------;
		lea	SQ_SIZ(a6),a6			;
		bsr	sequence_control		;
		clc					;
		rts					;

;-----------------------------------------------------------------------;
;	SEQUENCE CONTROL
;	input	a4.l: Decoder work		top address
;		a5.l: Note ON Cue table		top address
;		a6.l: Sequence control block	top address
;	output	nothing
;	break	a0-a3
;-----------------------------------------------------------------------;
			.public	sequence_control
sequence_control:
		move.b	(a6),d0				; bit7 = SQ_CTL_BITPOS
		bpl.w	ret_02				; Control bit OFF?  yes
		add.b	d0,d0				; bit6 = SQ_PAUSE_BITPOS
		bmi.w	ret_02				; Pause bit ON?  yes
		move.w	performance_flag(a6),d0		;seq stop exec mode check
		cmpi.w	#$8000,d0			;
		beq	stop_sequence_208		;
		move.l	CB_CT_TIME(a6),d0		; d0=count value(of 2msec)
		sub.l	d0,CB_TP_TIMER(a6)		; ===== counter replace =====
		bcc	seq_control			; TEMPO counter over?  no
	;-------------------------------------------------------;
	;	get next tempo					;
	;-------------------------------------------------------;
get_next_tempo:	moveq.l	#DEC_TEMPO_CONT,d0		; mode: Tempo-Continue
		clr.l	d1				;
		move.b	CB_SQ_NUM(a6),d1		;management #
		movem.l	a4/a5/a6,-(sp)			;
		bsr	sq_decoder			;decoder call
		movem.l	(sp)+,a4/a5/a6			;
		move.b	d0,d0				;
		beq	sqc110				;DEC_RET_NORMAL
	;------- 
		cmpi.b	#DEC_RET_LOOPBACK,d0		;
		bcs	seq_control			; jump if no data
		beq	sqc110				; jump if loop back
		bra.w	sqc_dec_error			; d0: 2-255(decoder error)
	;------- tempo value overflow check --------------------;
sqc110:		cmpi.l	#SQ_TVAL_MIN,d2			;
		bcs	sqc_tval_error			;under 200000(T300)?
		cmpi.l	#SQ_TVAL_MAX+1,d2		;
		bcc	sqc_tval_error			;over 1500000(T40)?
	;-------------------------------------------------------;
		move.l	d1,CB_TP_CNT(a6)		;for debug
		lsl.l	#8,d1				;delta time x 256
		move.l	d1,CB_TP_TIMER(a6)		;
	;------- tempo mode check ------------------------------;
		tst.b	CB_TEMPO_MODE(a6)		;tempo mode check
		beq	?tp_norm			;00:normal mode
		bpl	seq_control			;plus:ignore mode
	;------- tempo mode ratio ------------------------------;
		move.w	CB_SQ_RESO(a6),d0		;resolution(24-960)
		divu.w	d0,d2				;60000000/(bpm x res)
		move.l	#IPT_COUNT_BASE,d0		;2000usec x 256
		divu.w	d2,d0				;(2000 x 256 x bpm x res)/60000000
		andi.l	#$0000ffff,d0			;
		move.l	d0,CB_CT_ORGTIME(a6)		;set 1 count time to store work
		move.w	CB_TEMPO_RATIO(a6),d2		;
		tst.w	d2				;
		bpl	?ratio_fast			;slow down?  no
	;------- ratio slow ------------------------------------;
		neg.w	d2				;minus --> plus(0001h-8000h)
		addi.w	#SQ_TPCHG_BASE,d2		;d2(0000h-7FFFh)+1000h
		move.l	CB_CT_ORGTIME(a6),d7		;d7=1 count original time(usec)
		move.w	#SQ_TPCHG_BASE,d6		;
		mulu.w	d6,d7				;count time x 1000h
		divu.w	d2,d7				;
		andi.l	#$0000ffff,d7			;clear divide rest
		move.l	d7,CB_CT_TIME(a6)		;set new count time to work
		bra	seq_control			;
	;------- ratio fast ------------------------------------;
?ratio_fast:	addi.w	#SQ_TPCHG_BASE,d2		;d1(0001h-8000h)+1000h
		move.l	CB_CT_ORGTIME(a6),d7		;d7=1 count time(usec)
		mulu.w	d2,d7				;
		lsr.l	#8,d7				;
		lsr.l	#4,d7				;divide 4096(12bit)
		move.l	d7,CB_CT_TIME(a6)		;>>>>>  set to NEW count time
		bra	seq_control			;
	;------- tempo mode normal -----------------------------;
?tp_norm:	move.l	d2,CB_TP_VAL(a6)		;60000000usec/bpm(for debug)
		move.w	CB_SQ_RESO(a6),d0		;reslution(24-960)
		divu.w	d0,d2				;60000000/(bpm x res)
		move.l	#IPT_COUNT_BASE,d0		;2000usec x 256
		divu.w	d2,d0				;(2000 x 256 x bpm x res)/60000000
		andi.l	#$0000ffff,d0			;
		move.l	d0,CB_CT_TIME(a6)		;set 1 count time to work
		move.l	d0,CB_CT_ORGTIME(a6)		;set 1 count time to store work
	;-------------------------------------------------------;
	;	sequence data control				;
	;-------------------------------------------------------;
seq_control:	subq.b	#1,CB_PTR_CT(a6)		; wait counter -1
		bcc	?skip1				; 100ms wait?  no
		move.b	#SQ_PTR_WAIT,CB_PTR_CT(a6)	; counter init(2x50=100ms)
		movea.l	CB_PTR_ADRS(a6),a0		; a0: sequence pointer address
		addq.w	#1,(a0)				; 100ms counter +1
?skip1:		btst.b	#SQ_EMPTY_BITPOS,(a6)		;
		bne.w	ret_02				; Empty bit ON?  yes
		move.l	CB_CT_TIME(a6),d0		;read 1 count time/int
		sub.l	d0,CB_SQ_TIMER(a6)		;
		beq	?skip2				; >>> timer = 0
		bcc.w	ret_02				; >>> wait timer overflow
?skip2:		move.b	CB_EV_COM(a6),d0		; MIDI command
		move.b	CB_EV_S1(a6),d1			; MIDI data 1
		move.b	CB_EV_S2(a6),d2			; MIDI data 2
		move.b	CB_EV_PORT(a6),d3		; MIDI port#
		bsr	com_eventout			; ///// OUTPUT wait event /////
		cmpi.b	#MIDI_COM_NON,d0		; d0=MIDI command
		bne	get_next_event				; NOTE ON command?  no
		move.b	CB_EV_COM(a6),d0		; MIDI command
		move.l	CB_EV_GATE(a6),d4		; Gate time
		add.l	CB_SQ_TIMER(a6),d4		; >>>>> adjust gate time
		bsr	com_notecue_set			;note on cue
		addq.b	#1,CB_NT_CNT(a6)		; ===== Note ON counter +1 =====
	;-------------------------------------------------------;
	;	get next event					;
	;-------------------------------------------------------;
get_next_event:	moveq.l	#DEC_SONG_CONT,d0		; mode: Song-Continue
		clr.l	d1				;
		move.b	CB_SQ_NUM(a6),d1		; Song Number(0-7)
		movem.l	a4/a5/a6,-(sp)			;
		bsr	sq_decoder			;decoder call
		movem.l	(sp)+,a4/a5/a6			;
		cmpi.b	#DEC_RET_NORMAL,d0		;
		beq	make_event			; normal?  yes
		cmpi.b	#DEC_RET_NODATA,d0		;
		beq	sqc310				; no data?  yes
		bra.w	sqc_dec_error			; d0: 2-255(decoder error)
sqc310:		bset.b	#SQ_EMPTY_BITPOS,(a6)		; ===== SET empty bit =====
		tst.b	CB_NT_CNT(a6)			;
		beq	stop_sequence			; note on=0
		bra.w	ret_02				; note on=1-32
	;-------------------------------------------------------;
	;	make event					;
	;-------------------------------------------------------;
make_event:	andi.b	#11110000b,d3			;
		move.b	d3,CB_EV_COM(a6)		; MIDI command
		move.b	d4,CB_EV_S2(a6)			; MIDI data 2
		swap	d4				;
		move.b	d4,CB_EV_S1(a6)			; MIDI data 1
		lsr.w	#8,d0				; xx
		move.w	d0,d3				; xx
		andi.b	#00001111b,d0			; xx
		or.b	d0,CB_EV_COM(a6)		; xx
		lsr.w	#4,d3				; xx
		andi.b	#00000001b,d3			; xx
		move.b	d3,CB_EV_PORT(a6)		; xx
		lsl.l	#8,d2				; gate  x 8
		move.l	d2,CB_EV_GATE(a6)		; >>> SET gate Time
		lsl.l	#8,d1				; delta x 8
		add.l	d1,CB_SQ_TIMER(a6)		; >>> REPLACE sequence counter
		beq	out_event			; ZERO: output
		bpl.w	ret_02				; PLUS: wait
	;-------------------------------------------------------;
	;	OUTPUT event					;
	;-------------------------------------------------------;
out_event:	move.b	CB_EV_COM(a6),d0		; MIDI command
		move.b	CB_EV_S1(a6),d1			; MIDI data 1
		move.b	CB_EV_S2(a6),d2			; MIDI data 2
		move.b	CB_EV_PORT(a6),d3		; MIDI port#
		bsr	com_eventout			; ///// OUTPUT wait event /////
		cmpi.b	#MIDI_COM_NON,d0		; d0=MIDI command
		bne	sqc610				; [ NOTE ON ] command?  no
		move.b	CB_EV_COM(a6),d0		; MIDI command
		move.l	CB_EV_GATE(a6),d4		; Gate time
		add.l	CB_SQ_TIMER(a6),d4		; >>>>> adjust gate time
		bsr	com_notecue_set			;note on cue
		addq.b	#1,CB_NT_CNT(a6)		; ===== Note ON counter +1 =====
sqc610:		bra	get_next_event			; ===== Get NEXT event =====
	;-------------------------------------------------------;
	;	STOP sequence					;
	;-------------------------------------------------------;
stop_sequence_208:
		clr.w	up_down_flag(a6)		;
		clr.w	seq_volume_arrival(a6)		;
		clr.w	appointed_fade_rate(a6)		;
		clr.w	fade_count_variable(a6)		;
		clr.w	sequence_volume(a6)		;
stop_sequence:	move.b	#RET_NORMAL,CB_STAT(a6)		;normal
	;------- end of sequence control -----------------------;
sqc850:		move.b	#SQ_MODE_STOP,CB_MODE(a6)	;stop
		move.b	#$00,(a6)			;clear control flag
		bsr	com_set_seqmode			;set seq mode to host i/f work
		clr.w	performance_flag(a6)		;seq stop exec mode clear
ret_02:		rts					;
	;-------------------------------------------------------;
	;	Error Handle					;
	;-------------------------------------------------------;
sqc_dec_error:	move.b	d0,CB_STAT(a6)			;
		bra	sqc_error			;
sqc_tval_error:	move.b	#RET_ERR_TEMPOVAL,CB_STAT(a6)	;
		bra	sqc_error			;
sqc_error:	move.b	#1,d7				; 1:control OFF
		bsr	com_seq_noteoff			; ///// All note OFF /////
		bra.b	sqc850				;

;-----------------------------------------------------------------------;
;	sequence start ( ,sequence stop and start )			;
;	input	d0.b	management # (0-7)				;
;		d1.b	priority level (0-31)				;
;		d2.b	play mode (0-1)					;
;		a0.l	sequence data top address			;
;		a4.l	decoder work top address			;
;		a5.l	note ON cue table top address			;
;		a6.l	sequence control block top address		;
;	output	d0	status	00 normal				;
;				01 resolution out of range(24-960)	;
;				02 tempo data not found			;
;	break	d0,d1,d2,d3,d4,d5,d6,d7,a0,a1,a2,a3,a4,a5,a6		;
;-----------------------------------------------------------------------;
			.public	stop_and_start
			.public	stop_to_start
stop_and_start:	movem.l	a0/d0/d1/d2,-(sp)		;
		move.b	#$00,(a6)			;control bit all OFF
		move.b	#1,d7				; 1: note off & control off
		bsr	com_seq_noteoff			;all note off
		move.b	#0,CB_NT_CNT(a6)		;note on counter clear
		movem.l	(sp)+,a0/d0/d1/d2		;
	;------- work init -------------------------------------;
stop_to_start:	andi.l	#$00000007,d0			;management #
		andi.l	#$0000001f,d1			;priority
		andi.l	#$00000001,d2			;sequence mode
		move.b	d0,CB_SQ_NUM(a6)		;set sequence play number
		move.b	d1,CB_PR_LEV(a6)		;set sequence priority level
		move.b	d2,CB_SQ_MODE(a6)		;set sequence play mode
		move.l	a0,CB_SQ_TOP(a6)		;set sequence data top address
		move.b	#SQ_MODE_PLAY,CB_MODE(a6)	;play mode
		btst.b	#SQ_FADE_BITPOS,(a6)		;fade bit check
		beq	?skip				;
		move.b	#SQ_MODE_FADE,CB_MODE(a6)	;fade mode set
?skip:		move.b	#RET_NORMAL,CB_STAT(a6)		;normal status
		bsr	com_set_seqmode			;mode & status set
	;-------------------------------------------------------;
		lsl.l	#1,d0				;management # x2
		movea.w	#host_if_work,a1		;$0700
		adda.l	d0,a1				;add offset(0-14)
		lea	OF_HI_STAT1(a1),a2		;
		move.l	a2,CB_STS_ADRS(a6)		;set sequence status pointer
		lea	OF_HI_SEQ0(a1),a2		;
		move.l	a2,CB_PTR_ADRS(a6)		;set sequence position adrs
		move.w	#0,(a2)				;clear sequence position
		move.w	(a0),d0				;resolution(24-960)
		cmpi.w	#SQ_RESO_MIN,d0			;
		bcs	err_reso_val			;under 24?  yes
		cmpi.w	#SQ_RESO_MAX+1,d0		;
		bcc	err_reso_val			;over 960?  yes
		move.w	d0,CB_SQ_RESO(a6)		;set sequence resolution
		move.b	#0,CB_NT_CNT(a6)		;set note on counter(=0)
	;-------------------------------------------------------;
	;	tempo initial					;
	;-------------------------------------------------------;
		move.l	#DEC_TEMPO_INIT,d0		;tempo initial
		clr.l	d1				;
		move.b	CB_SQ_NUM(a6),d1		;management #(0-7)
		move.l	CB_SQ_TOP(a6),a0		;Sequence data top address
		movem.l	a4/a5/a6,-(sp)			;
		bsr	sq_decoder			;decoder call
		movem.l	(sp)+,a4/a5/a6			;
		cmpi.b	#DEC_RET_NORMAL,d0		;
		beq	stt100				; normal?  yes
		bra	err_dec_error			; d0: 2-255(decoder error)
stt100:		cmpi.l	#SQ_TVAL_MIN,d2			;
		bcs	err_tempo_val			; under 200000(T300)?  yes
		cmpi.l	#SQ_TVAL_MAX+1,d2		;
		bcc	err_tempo_val			; over 1500000(T40)?  yes
		move.l	d1,CB_TP_CNT(a6)		; for debug
		move.l	d2,CB_TP_VAL(a6)		; for debug
		lsl.l	#8,d1				; delta time x 256
		move.l	d1,CB_TP_TIMER(a6)		; >>> SET Tempo counter
		move.w	CB_SQ_RESO(a6),d0		; d0=24-960
		divu.w	d0,d2				; d2--->208-62500
		move.l	#IPT_COUNT_BASE,d0		; d0=2000x256(512000)
		divu.w	d2,d0				; d0--->2461-8
		andi.l	#$0000ffff,d0			; clear rest(bit8-15)
		move.l	d0,CB_CT_TIME(a6)		; >>> SET 1 Count time
		move.l	d0,CB_CT_ORGTIME(a6)		; >>> SET 1 Count original time
		moveq	#0,d0				;
		move.b	d0,CB_TEMPO_MODE(a6)		;tempo mode flag clear
		move.w	d0,CB_TEMPO_RATIO(a6)		;tempo ratio clear
	;-------------------------------------------------------;
	;	Set first event					;
	;-------------------------------------------------------;
stt150:		moveq.l	#DEC_SONG_INIT,d0		; mode: Song/Initial
		clr.l	d1				;
		move.b	CB_SQ_NUM(a6),d1		; Song Number(0-7)
		move.l	CB_SQ_TOP(a6),a0		; Sequence data top address
		movem.l	a4/a5/a6,-(sp)			;
		bsr	sq_decoder			; ///// decoder call /////
		movem.l	(sp)+,a4/a5/a6			;
		cmpi.b	#DEC_RET_NORMAL,d0		;
		beq	stt200				; normal?  yes
		cmpi.b	#DEC_RET_NODATA,d0		;
		beq	err_no_event			; no data?  yes
		bra	err_dec_error			; d0: 2-255(decoder error)
stt200:		andi.b	#11110000b,d3			;
		move.b	d3,CB_EV_COM(a6)		; MIDI Command
		move.b	d4,CB_EV_S2(a6)			; MIDI data 2
		swap	d4				;
		move.b	d4,CB_EV_S1(a6)			; MIDI data 1
		lsl.l	#8,d1				; delta x 8
		lsl.l	#8,d2				; gate  x 8
		move.l	d1,CB_SQ_TIMER(a6)		; >>> SET sequence counter
		move.l	d2,CB_EV_GATE(a6)		; >>> SET gate Time
		lsr.w	#8,d0				;
	;	move.w	d0,d3				;
		andi.b	#00001111b,d0			;
		or.b	d0,CB_EV_COM(a6)		;midi channel #
	;	lsr.w	#4,d3				;
	;	andi.b	#00000001b,d3			;
	;	move.b	d3,CB_EV_PORT(a6)		;midi port
		andi.b	#00010000b,(a6)			; control word clear
		ori.b	#10000000b,(a6)			; >>> CTL bit on
		clc					;
		rts					;

;-----------------------------------------------------------------------;
;	Error Handle							;
;-----------------------------------------------------------------------;
err_reso_val:	move.b	#RET_ERR_RESOVAL,CB_STAT(a6)	;
		bra	stt_error			;
err_no_event:	move.b	#RET_ERR_NOEVENT,CB_STAT(a6)	;
		bra	stt_error			;
err_dec_error:	move.b	d0,CB_STAT(a6)			;
		bra	stt_error			;
err_tempo_val:	move.b	#RET_ERR_TEMPOVAL,CB_STAT(a6)	;
stt_error:	bsr	com_set_status			; ///// Set seq MODE/STATUS /////
		bra.w	ret_error			; ===== return to main =====


;-----------------------------------------------------------------------;
;	sequence pause							;
;	in	a4	decoder work top address			;
;		a5	note on cue table top address			;
;		a6	sequence control block top address		;
;	out	none							;
;	break	d0-d7,a0-a6,flag
;-----------------------------------------------------------------------;
			.public	play_to_pause
			.public	fade_to_pause
	;------- play mode to pause mode -----------------------;
play_to_pause:	move.b	#SQ_MODE_PLAYPAUSE,CB_MODE(a6)	;$03:play pause
		bra.b	pause				;
	;------- fade mode to pause mode -----------------------;
fade_to_pause:	move.b	#SQ_MODE_FADEPAUSE,CB_MODE(a6)	;$07:fade pause
pause:		bset.b	#SQ_PAUSE_BITPOS,(a6)		;bit6:
		move.b	#0,d7				;
		bsr	com_seq_noteoff			;all note off
		move.b	#RET_NORMAL,CB_STAT(a6)		;normal status
		bsr	com_set_seqmode			;seq mode set to host i/f
		clc					;
		rts					;

;-----------------------------------------------------------------------;
;	sequence stop							;
;	in	a4	decoder work top address			;
;		a5	note on cue table top address			;
;		a6	sequence control block top address		;
;	out	none							;
;	break	d0-d7,a0-a6,flag
;-----------------------------------------------------------------------;
			.public	to_stop
to_stop:	move.b	#$00,(a6)			; >>> control bit all off
		move.b	#1,d7				; 1:control off
		bsr	com_seq_noteoff			; ===== all note off =====
		move.b	#SQ_MODE_STOP,CB_MODE(a6)	; mode:   stop
		move.b	#RET_NORMAL,CB_STAT(a6)		; status: normal
		bsr	com_set_seqmode			; ===== set seq mode/status =====
		clc					;
		rts					;

;-----------------------------------------------------------------------;
;	sequence resume							;
;	input	d0	management number(0-7)				;
;		a4	decoder work top address			;
;		a5	note ON cue table top address			;
;		a6	sequence control block top address		;
;	output	none							;
;	break	d0-d7,a0-a6,flag
;-----------------------------------------------------------------------;
			.public	playpause_to_play
			.public	fadepause_to_fade
	;------- pause to play ---------------------------------;
playpause_to_play:
		move.b	#SQ_MODE_PLAY,CB_MODE(a6)	; mode: play
		bra.b	continue			;
	;------- pause to fade ---------------------------------;
fadepause_to_fade:
		move.b	#SQ_MODE_FADE,CB_MODE(a6)	; mode: fade
continue:	bclr.b	#SQ_PAUSE_BITPOS,(a6)		; ===== pause bit off =====
		move.b	#RET_NORMAL,CB_STAT(a6)		; status: normal
		bsr	com_set_seqmode			; ///// set seq mode/status /////
		clc					;
		rts					;

;-----------------------------------------------------------------------;
;	note on cue control						;
;	input	a4.l: Decoder work top address				;
;		a5.l: Note ON Cue table top address			;
;		a6.l: Sequence control block top address		;
;	output	none							;
;	break	a0-a3,a6,d0-d3
;-----------------------------------------------------------------------;
noteoncue_control:
		btst.b	#NC_CTL_BITPOS,(a5)		;
		beq	?end				;control bit OFF?  yes
		movea.l	NC_PTR(a5),a6			;a6: control block address
		btst.b	#SQ_PAUSE_BITPOS,(a6)		;
		bne	?end				;Pause bit ON?  yes
		move.l	CB_CT_TIME(a6),d0		;d0=count value(of 2msec)
		sub.l	d0,NC_CNT(a5)			;gate time
		beq	?skip				;time = 0
		bpl	?end				;wait time over flow
	;-------------------------------------------------------;
	;	NOTE OFF event output				;
	;-------------------------------------------------------;
?skip:		move.b	NC_CMD(a5),d0			;midi command
		move.b	NC_S1(a5),d1			;midi data 1
		move.b	NC_S2(a5),d2			;midi data 2
		move.b	NC_CTL(a5),d3			;midi port#
		andi.b	#NC_PORT_MASK,d3		;port# mask
		andi.b	#MIDI_COM_CHMASK,d0		;channel mask
		ori.b	#MIDI_COM_NOFF,d0		;note off command
		bsr	com_eventout			;set midi event to buffer
		bclr.b	#NC_CTL_BITPOS,(a5)		;
		subq.b	#1,CB_NT_CNT(a6)		;note on counter -1
		bne	?end				;
		btst.b	#SQ_EMPTY_BITPOS,(a6)		;test empty bit
		beq	?end				;
	;-------------------------------------------------------;
	;	STOP sequence					;
	;-------------------------------------------------------;
		move.b	#$00,(a6)			;control bit all off
		move.b	#SQ_MODE_STOP,CB_MODE(a6)	;sequence stop mode
		move.b	#RET_NORMAL,CB_STAT(a6)		;normal status
		bsr	com_set_seqmode			;set sequence mode to host i/f
?end:		rts					;

;-----------------------------------------------------------------------;
;	sequence temp change						;
;	in	d0.b	management number (0-7)				;
;		d2.w	tempo data (-32768-->+32767)			;
;	out	none							;
;	break	d0-d7,a0-a6,flag
;-----------------------------------------------------------------------;
			.public	xseq_tempo
xseq_tempo:	bsr	com_get_workaddress		;a4=9800/a5=9c00/a6=9900
		andi.w	#$0007,d0			; d0=play number(0-7)
		move.w	#SQ_SIZ,d7			; d7=64
		mulu.w	d0,d7				;
		adda.l	d7,a6				; a6: sequence work top address
from_tp_ratio:	tst.w	d2				;
		bpl	tp200				; slow down?  no
tp100:		neg.w	d2				; minus --> plus(0001h-8000h)
		addi.w	#SQ_TPCHG_BASE,d2		; d2(0000h-7FFFh)+1000h
		move.l	CB_CT_ORGTIME(a6),d7		; d7=1 count original time(usec)
		move.w	#SQ_TPCHG_BASE,d6		;
		mulu.w	d6,d7				; count time x 1000h
		divu.w	d2,d7				;
		andi.l	#$0000ffff,d7			; clear divide rest
		move.l	d7,CB_CT_TIME(a6)		; >>>>>  set to NEW count time
		bra	tp900				;
tp200:		addi.w	#SQ_TPCHG_BASE,d2		; d1(0001h-8000h)+1000h
		move.l	CB_CT_ORGTIME(a6),d7		; d7=1 count time(usec)
		mulu.w	d2,d7				;
		lsr.l	#8,d7				;
		lsr.l	#4,d7				; divide 4096(12bit)
		move.l	d7,CB_CT_TIME(a6)		; >>>>>  set to NEW count time
tp900:		clc					;
		rts					;

;-----------------------------------------------------------------------;
;	set tempo mode 							;
;	in	d0.b	management Number (0-7)				;
;		d1.b	mode (0:normal,plus:ignore,minus:ratio)		;
;	out	none							;
;	break	d0-d7,a0-a6,flag
;-----------------------------------------------------------------------;
			.public	xtempo_mode
xtempo_mode:	bsr	com_get_workaddress		;a4=9800/a5=9c00/a6=9900
		andi.w	#$0007,d0			;management #
		move.w	#SQ_SIZ,d7			;
		mulu.w	d0,d7				;x$60
		adda.l	d7,a6				;sequence work top address
		move.b	d1,CB_TEMPO_MODE(a6)		;tempo mode set
		clc					;
		rts					;

;-----------------------------------------------------------------------;
;	set tempo ratio							;
;	in	d0.b	management # (0-7)				;
;		d1.b	dummy						;
;		d2.w	tempo ratio (-32768-->+32767)			;
;	out	none							;
;	break	d0-d7,a0-a6,flag
;-----------------------------------------------------------------------;
			.public	xtempo_ratio
xtempo_ratio:	bsr	com_get_workaddress		;a4=9800/a5=9c00/a6=9900
		andi.w	#$0007,d0			;management #
		move.w	#SQ_SIZ,d7			;
		mulu.w	d0,d7				;x$60
		adda.l	d7,a6				;sequence work top address
		move.w	d2,CB_TEMPO_RATIO(a6)		;
		bra.w	from_tp_ratio			;

;=======================================================================;
;	end of file							;
;=======================================================================;
