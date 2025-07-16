;=======================================================================;
;	SEGASATURN sound driver						;
; file name								;
;	sdcom.asm							;
; copyright								;
;	SEGA ENTERPRISES						;
; purpose								;
;	system common subroutine					;
; author(s)								;
;	1995-11-14	A.Miyazawa	Ver.2.00			;
; mod history								;
;	1996-02-23	A.Miyazawa	Ver.2.06			;
;	1996-07-02	A.Miyazawa	Ver.2.10L			;
;	1997-06-30	Y.Kashima	Ver.2.20			;
;=======================================================================;
		.include	scsp.lib

			.extern	PEG_off

;-----------------------------------------------------------------------;
;	
;-----------------------------------------------------------------------;
			.public	master_volume_down
master_volume_down:
		movem.l	d1/d6/d7/a0,-(sp)		;
		movea.l	#IO_SCSP+RG_MVOL,a0		;
		move.w	(scsp_mvol),d1			;
		beq.s	?skip				;
?mvol_loop:	move.b	d1,(a0)				;
		subq.b	#1,d1				;
		moveq	#127,d6				;
		dbra	d6,$				;wait
		tst.w	d1				;
		bne.s	?mvol_loop			;
	;------- rerease off -----------------------------------;
?skip:		movea.l	#IO_SCSP,a0			;
		moveq	#slot_size-1,d7			;
?slot_loop0:	ori.b	#$1f,SCSP_FH_RR(a0)		;set DL&RR
		adda.w	#$0020,a0			;next slot
		dbra	d7,?slot_loop0			;
		movem.l	(sp)+,d1/d6/d7/a0		;
		rts					;

;-----------------------------------------------------------------------;
;	
;-----------------------------------------------------------------------;
			.public	noise_reduction
noise_reduction:
		movem.l	d0-d7/a0-a6,-(sp)		;
		moveq	#128-1,d7			;
		moveq	#0,d1				;
	;-------------------------------------------------------;
	;	dsp program area clear				;
	;-------------------------------------------------------;
		movea.l	#IO_SCSP+DSP_MPRO,a0		;
?400:		move.l	d1,(a0)+			;
		move.l	d1,(a0)+			;
		dbra	d7,?400				;
	;-------------------------------------------------------;
		moveq	#$40,d5				;
		move.w	#-2,d7				;
		movea.l	#bs_PWKTP,a6			;
?200:		moveq	#slot_size-1,d6			;
?100:		lea	slot_work(a6),a4		;
		moveq	#0,d1				;
		move.w	slot_status(a4),d0		;
		andi.w	#$000f,d0			;
		or.w	d0,d1				;
		adda.w	d5,a4				;
		dbra	d6,?100				;
		tst.w	d1				;
		beq.s	EFREG_lvl_down			;
		dbra	d7,?200				;
EFREG_lvl_down:	move.w	#$1000-1,d7			; safety counter
		move.w	#$8000,d4			;
?main_loop:	movea.l	#SCSP_EFREG0,a0			;$100ec0
		moveq	#16-1,d6			;
		moveq	#0,d5				;
?channel_loop:	move.w	(a0),d0				;
		beq.s	?100				;
		or.w	d0,d5				;
		moveq	#1,d1				;
		moveq	#0,d2				;
		sub.w	d4,d0				;
		addx.w	d2,d2				;
		add.w	d2,d2				;
		sub.w	d2,d1				;
		add.w	d4,d0				;
		add.w	d1,d0				;
?set_wave_data:	move.w	d0,(a0)				;
?100:		addq.w	#2,a0				;
		dbra	d6,?channel_loop		;
		tst.w	d5				;
		beq.s	?end_of_noise_reduction		;
		dbra	d7,?main_loop			;
?end_of_noise_reduction:
		movem.l	(sp)+,d0-d7/a0-a6		;
		rts					;

;-----------------------------------------------------------------------;
;	master volume return (quick fade in)				;
;	in	none							;
;	out	none							;
;	break	none							;
;-----------------------------------------------------------------------;
			.public	master_volume_return
master_volume_return:
		movem.l	d1/d6/d7/a0,-(sp)		;
		move.w	(scsp_mvol),d7			;
		movea.l	#IO_SCSP+RG_MVOL,a0		;
		moveq	#1,d1				;
?mvol_loop:	move.b	d1,(a0)				;
		moveq	#127,d6				;
		dbra	d6,$				;
		cmp.w	d1,d7				;
		beq.s	?end_of_mvol			;
		addq.w	#1,d1				;
		andi.w	#$000f,d1			;
		bra.s	?mvol_loop			;
?end_of_mvol:	movem.l	(sp)+,d1/d6/d7/a0		;
		rts					;

;-----------------------------------------------------------------------;
;	damper off all channels						;
;	--------------------------------------------------------	;
;	in	d0:management number					;
;		a6:68000 work base address				;
;	break	none							;
;-----------------------------------------------------------------------;
			.public	all_damper_off
all_damper_off:	movem.l	d0-d7/a0-a6,-(sp)		;
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		move.w	d0,_tmp_kanri(a6)		;management number * 0x200
		moveq	#16-1,d7			;
		moveq	#0,d5				;
?midi_ch_loop:	bclr.b	#damper,midi_flag(a6,d5.w)	;damper flag off!
		addi.w	#$0020,d5			;
		dbra	d7,?midi_ch_loop		;
		moveq	#slot_size-1,d7			;
		movea.l	#bs_PWKTP+slot_work,a4		;
		movea.l	#IO_SCSP,a5			;
?slot_loop:	move.b	PSPN(a4),d6			;check pcm stream
		bmi	?func_false			;
	.if ENGN
		move.b	sl_flag1(a4),d6			;check engine
		bmi	?func_false			;
	.endif		;ENGN
		move.w	_sl_kanri(a4),d6		;
		cmp.w	_tmp_kanri(a6),d6		;check management #
		bne	?func_false			;
		bclr.b	#flg_non,sl_flag2(a4)		;note off
		bclr.b	#flg_KON,sl_flag2(a4)		;key off
		btst.b	#PEON_flg,sl_flag1(a4)		;check pitch eg
		beq.s	?set_to_scsp			;
		bsr	PEG_off				;
?set_to_scsp:	move.b	SCSP_KXKB(a5),d5		;
		andi.b	#%00000111,d5			;
		ori.b	#%00010000,d5			;
		move.b	d5,SCSP_KXKB(a5)		;set key off slot
?func_false:	adda.w	#$0040,a4			;
		adda.w	#$0020,a5			;
		dbra	d7,?slot_loop			;
		movem.l	(sp)+,d0-d7/a0-a6		;
		rts					;

;-----------------------------------------------------------------------;
;	GET WORK ADDRESS						;
;	--------------------------------------------------------	;
; function:								;
;	transfer parmanent register					;
;	out	a4:decoder work top address				;
;		a5:note ON Cue table top address			;
;		a6:sequence control block top address			;
;	break	none							;
;-----------------------------------------------------------------------;
			.public	com_get_workaddress
com_get_workaddress:
		movea.l	#decoder_work,a4		;$9800
		movea.l	#note_on_cue_table,a5		;$9c00
		movea.l	#management_work,a6		;$9900
		rts					;

;-----------------------------------------------------------------------;
;	SEQUENCE ALL NOTE OFF
;	Function  [ All note off output (inside 1 sequence) ]
;	Input	d7.B: 0=note OFF only
;		      1=note OFF and  control OFF
;		a5.L: note on cue table top address
;		a6.L: sequence control block top address
;	Output	nothing
;	Break	a0-a2/d0-d3
;-----------------------------------------------------------------------;
			.public	com_seq_noteoff
com_seq_noteoff:
		movem.l	a5/d6,-(sp)			;
		move.w	#NC_NUM-1,d6			; max 32 cue
?loop:		btst.b	#NC_CTL_BITPOS,(a5)		;
		beq	?skip2				; control bit OFF?  yes
		cmpa.l	NC_PTR(a5),a6			;
		bne	?skip2				; sequence MATCH?  no
		tst.b	d7				; 1: note OFF & control OFF
		beq	?skip1				;
		bclr.b	#NC_CTL_BITPOS,(a5)		; ===== Contrl Bit OFF =====
?skip1:		move.b	NC_CMD(a5),d0			;midi command
		move.b	NC_S1(a5),d1			;midi data 1
		move.b	NC_S2(a5),d2			;midi data 2
		move.b	NC_CTL(a5),d3			;midi port#
		andi.b	#NC_PORT_MASK,d3		;port# mask
		andi.b	#MIDI_COM_CHMASK,d0		;channel mask
		ori.b	#MIDI_COM_NOFF,d0		; SET [NOTE OFF] command
		bsr	com_eventout			; ///// NOTE OFF event output /////
?skip2:		adda.l	#NC_SIZ,a5			; next address
		dbra.w	d6,?loop			; cue end?  no
		movem.l	(sp)+,a5/d6			;
		rts					;

;-----------------------------------------------------------------------;
;	set sequence mode to host interface				;
;	in	a6:Sequence control block top address			;
;	out	none							;
;	break	none							;
;-----------------------------------------------------------------------;
			.public	com_set_seqmode
com_set_seqmode:
		movem.l	a0/d0/d2,-(sp)			;
		movea.w	#host_if_work+OF_HI_STAT1,a0	;
		move.b	CB_MODE(a6),d0			;read mode
		moveq	#0,d2				;
		move.b	CB_SQ_NUM(a6),d2		;management #
		add.w	d2,d2				;
		move.b	d0,(a0,d2.w)			;set mode to host I/F work
		movem.l	(sp)+,a0/d0/d2			;
		rts					;
;-----------------------------------------------------------------------;
;	
;-----------------------------------------------------------------------;
			.public	com_set_status
com_set_status:	movem.l	a0/d0/d2,-(sp)			;
		movea.w	#host_if_work+OF_HI_STAT1+1,a0	;
		move.b	CB_STAT(a6),d0			; D0: status
		moveq	#0,d2				; clear D2
		move.b	CB_SQ_NUM(a6),d2		; Get play number(0-7)
		add.w	d2,d2				; D2 x 2(0-14)
		move.b	d0,(a0,d2.w)			; >>> SET Status to Host I/F work
		movem.l	(sp)+,a0/d0/d2			;
		rts					;
;-----------------------------------------------------------------------;
;	
;-----------------------------------------------------------------------;
			.public	send_user
send_user:	movem.l	d1/a0,-(sp)			;
		moveq	#0,d1				;
		move.b	_tmp_kanri(a6),d1		;
		movea.w	#host_if_work+OF_HI_STAT1+1,a0	;
		move.b	d0,(a0,d1.w)			;
		movem.l	(sp)+,d1/a0			;
		rts					;

;-----------------------------------------------------------------------;
;	NOTE ON CUE SET
;	Function [ set NoteOn event to NoteOn cue ]
;	Input	d0:midi command						;
;		d1:midi data1						;
;		d2:midi data2						;
;		d4: gate time (usec)
;		a5: note on cue table	top address
;		a6: sequence control block top address
;	Output	CY: 0=normal end
;		    1=error (cue full)
;	Break	a0
;-----------------------------------------------------------------------;
			.public	com_notecue_set
com_notecue_set:
		movem.l	a5/d7,-(sp)			;
		moveq.l	#NC_NUM,d7			; max 32 cue
	int_di						;int. disable
	sr_push						
?loop:		btst.b	#NC_CTL_BITPOS,(a5)		;
		beq	ncs30				;empty(Bit=0)?  no
		adda.l	#NC_SIZ,a5			;set next cue address
		subq.b	#1,d7				;
		bne	?loop				;cue end?  no
		stc					;CY=1: error
ncs20:	sr_pop						;int. enable
		movem.l	(sp)+,a5/d7			;
		rts					;
	;-------
ncs30:		move.b	d0,NC_CMD(a5)			;midi command
		move.b	d1,NC_S1(a5)			;midi data 1
		move.b	d2,NC_S2(a5)			;midi data 2
		move.l	d4,NC_CNT(a5)			;gate time
		move.l	a6,NC_PTR(a5)			;control block pointer
		bset.b	#NC_CTL_BITPOS,(a5)		;contrl bit on
		clc					;CY=0: normal end
		bra	ncs20

;-----------------------------------------------------------------------;
;	midi event output to saturn midi receive buffer			;
;	in	d0.b:midi command					;
;		d1.b:midi data1						;
;		d2.b:midi data2						;
;		a4.L:decoder work ($9800)				;
;		a5.L:note on cue table ($9c00)				;
;		a6.L:management work ($9900)				;
;	out	d0.b:midi command (xxxx0000b)				;
;	break	a0,a1,a2,d0						;
;-----------------------------------------------------------------------;
			.public	com_eventout
com_eventout:	movem.l	d5/d6/d7,-(sp)			;
		movea.l	#bs_PWKTP,a0			;68k work top ($7000)
		lea	_MIDI_RCV_WRPT(a0),a1		;saturn midi receive buf ptr($8840)
		lea	_MIDI_RCV_BF(a0),a2		;saturn midi receive buffer($8a00)
		move.b	d0,d5				;
		move.b	d0,d6				;
		andi.b	#MIDI_COM_MASK,d0		;command mask(11110000b)
		cmpi.b	#MIDI_COM_NON,d0		;note on?($90)
		bne	eo20				;
	;------- note on ---------------------------------------;
		move.b	CB_SQ_VOL(a6),d7		;read sequence volume
		btst.b	#SQ_FADE_BITPOS,(a6)		;fade check
		beq	?nofade				;
		move.b	CB_FD_VOL(a6),d7		;read fade volume
?nofade:	sub.b	d7,d2				;velocity
		bcc	eo20				;overflow check
		clr.b	d2				;velocity=0
	;------- make saturn midi format -----------------------;
eo20:		lsr.b	#4,d5				;command
		andi.b	#M4_COM_MASK,d5			;command mask(00000111b)
		andi.b	#MIDI_COM_CHMASK,d6		;midi ch mask(00001111b)
		move.b	CB_SQ_NUM(a6),d7		;read management #
		lsl.b	#5,d7				;bit7-5:management #
		or.b	d7,d6				;bit4-0:midi channel #
		clr.l	d7				;
	;-------------------------------------------------------;
	;	set saturn midi format data to receive buffer	;
	;-------------------------------------------------------;
	sr_push						;
	int_di						;int. disable
		move.b	(a1),d7				;get data pointer
		addq.b	#1,(a1)				;restore data pointer
		add.w	d7,d7				;pointer x4
		add.w	d7,d7				;
		adda.l	d7,a2				;
		move.b	d5,(a2)+			;midi command
		move.b	d6,(a2)+			;midi channel
		move.b	d1,(a2)+			;midi data1
		move.b	d2,(a2)+			;midi data2
	sr_pop						;int. enable
eo40:		movem.l	(sp)+,d5/d6/d7			;
		rts					;

;-----------------------------------------------------------------------;
;	cdaudio volume analyze every 16msec				;
;	in	a4.L:decoder work ($9800)				;
;		a5.L:note on cue table ($9c00)				;
;		a6.L:management work ($9900)				;
;	out	none							;
;	break	a0,a1,a2,a3,d0,d1					;
;-----------------------------------------------------------------------;
			.public	volume_analize
volume_analize:	subq.b	#1,SY_TVOLCNT(a4)		;counter check
		bcc.w	?end				;
		move.b	#SY_VANWAIT_CNT-1,SY_TVOLCNT(a4)	;
		movea.w	#host_if_work,a2		;host i/f work top($700)
	;------- cdaudio l-ch input ----------------------------;
		movea.l	#SCSP_EXTS_L,a0			; EXTS Lch address
		lea	OF_HI_TVOLL(a2),a1		;work l
		bsr	com_volanalize			;
	;------- cdaudio r-ch input ----------------------------;
		movea.l	#SCSP_EXTS_R,a0			; EXTS Rch address
		lea	OF_HI_TVOLR(a2),a1		;work r
		bsr	com_volanalize			;
	;------- effect ch9 input ------------------------------;
		movea.l	#SCSP_EFREG9,a0			;(high)
		lea	OF_HI_HVOLL(a2),a1		;work l
		bsr	com_volanalize			;
		move.w	d0,OF_HI_HVOLR(a2)		;work r
	;------- effect ch10 input -----------------------------;
		movea.l	#SCSP_EFREG10,a0		;(mid)
		lea	OF_HI_MVOLL(a2),a1		;work l
		bsr	com_volanalize			;
		move.w	d0,OF_HI_MVOLR(a2)		;work r
	;------- effect ch11 input -----------------------------;
		movea.l	#SCSP_EFREG11,a0		;(low)
		lea	OF_HI_LVOLL(a2),a1		;work l
		bsr	com_volanalize			;
		move.w	d0,OF_HI_LVOLR(a2)		;work r
?end:		rts					;

;-----------------------------------------------------------------------;
;	read input data and set average data				;
;	in	a0.l:analyze data scsp register address			;
;		a1.l:analyze data set area address			;
;	out	d0.w:volume average data(0000-7FFF)			;
;	break	d0,d1							;
;-----------------------------------------------------------------------;
com_volanalize:	movem.l	d1,-(sp)			;
		move.w	(a0),d0				;read 16bit pcm data
		tst.w	d0				;
		bpl	?skip				; d0=0000-7FFF(new data)
		neg.w	d0				;
?skip:		move.w	(a1),d1				; d1=0000-7FFF(total average)
		add.w	d1,d0				;
		lsr.w	#1,d0				; make new TOTAL average
		move.w	d0,(a1)				;
		movem.l	(sp)+,d1			;
		rts					;

;-----------------------------------------------------------------------;
;	pcm play block check every 16msec				;
;	in	a4.l:decoder work top address				;
;		a5.l:note on cue table top address			;
;		a6.l:sequence control block top address			;
;	out	d1.l:pcm play block renewal bit map			;
;	break	a0,a1,a2,a3,d0,d1,d2,d7					;
;-----------------------------------------------------------------------;
			.public	pcm_address_search
pcm_address_search:
		subq.b	#1,SY_PCMCNT(a4)		;counter check
		bcc	?end				;
		move.b	#SY_PCMWAIT_CNT-1,SY_PCMCNT(a4)	;
		movea.l	#bs_PWKTP,a0			;
		adda.l	#Pcm_Strm,a0			;pcm stream play slot#($8890)
		movea.w	#host_if_work,a1		;
		adda.l	#OF_HI_PCM0,a1			;$7a0
		moveq.l	#0,d0				;
		moveq.l	#0,d1				;
		moveq.l	#PCM_MAX_NUM-1,d7		;management #
?loop:		bsr	com_pcm_control			;play block renewal check
		adda.l	#2,a0				;next management# work
		adda.l	#2,a1				;next play block# work
		addq.b	#1,d0				;next management#
		dbra.w	d7,?loop			;
	;-------------------------------------------------------;
		movea.w	#bs_SIFWK,a0			;$480
		btst.b	#MCI_EN_BITPOS,OF_IPT_CTRL(a0)	;check pcm play interrupt enable
		beq	?end				;disable then end
		tst.b	d1				;check play block renewal
		beq	?end				;
		move.b	d1,OF_PCM_PLAYNUM(a0)		;set play number bit map($482)
		move.b	#MCI_MODE_PCM,OF_IPT_MODE(a0)	;set play block renewal interrupt
		lea	IO_SCSP,a1			;$100000
		move.w	#$20,RG_MCIEB(a1)		;main cpu int. enable
	nop						;wait
		move.w	#$20,RG_MCIPD(a1)		;int. request to main cpu
?end:		rts					;

;-----------------------------------------------------------------------;
;	pcm stream play block check and set renewal bit			;
;	in	d0.l:pcm management(0-7)				;
;		d1.l:pcm play block renewal bit map			;
;		a0.l:pcm play slot number read area address		;
;		a1.l:pcm play address write area address		;
;	out	d1.l:new pcm play block renewal bit map			;
;	break	a2,d1,d2						;
;-----------------------------------------------------------------------;
com_pcm_control:
		btst.b	#PCM_CTL_BITPOS,(a0)		;pcm stream use check
		beq	?skip				;
		movea.l	#SCSP_DBG_REG,a2		;monitor slot,call adrs($100408)
		move.b	(a0),d2				;get slot #
		andi.b	#RG_SLOT_MASK,d2		;
		lsl.w	#8,d2				;
		lsl.w	#3,d2				;bit15-11:MSLC(slot select)
		move.w	d2,(a2)				;slot select
		move.w	#14,d2				;
?loop:	nop						;wait scsp 1cycle(22usec)
		dbra	d2,?loop			;
		move.w	(a2),d2				;read CA/SGC/EG data
		lsr.w	#7,d2				;
		andi.b	#RG_CA_MASK,d2			;pcm play address(0-15)
		cmp.b	(a1),d2				;play block check
		beq	?end				;same block then skip
		move.b	d2,(a1)				;new block
		bset.l	d0,d1				;set bit map
		bra	?end				;
?skip:		move.b	#0,(a1)				;play address clear
?end:		rts					;

;=======================================================================;
;	end of file							;
;=======================================================================;
