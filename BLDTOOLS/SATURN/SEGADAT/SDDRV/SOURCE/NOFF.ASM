;=======================================================================;
;	SEGASATURN sound driver						;
; file name								;
;	noff.asm							;
; copyright								;
;	SEGA ENTERPRISES						;
; purpose								;
;	Note off control						;
; author(s)								;
;	1995-11-14	A.Miyazawa	Ver.2.00			;
; mod history								;
;	1996-02-23	A.Miyazawa	Ver.2.06			;
;	1996-07-02	A.Miyazawa	Ver.2.10L			;
;	1997-06-30	Y.Kashima	Ver.2.20			;
;=======================================================================;

		.include	SCSP.LIB

			.globals	on

			.extern	get_EXPTBS
			.extern	get_EXPTB2
			.extern	get_LEVEL
			.extern	Note_on
			.extern	Play_mono
			.extern	note_on_data

;-----------------------------------------------------------------------;
;	Note off ( polyphonic mode )					;
;	--------------------------------------------------------	;
;	in	d0:
;		d1:midi channel #					;
;		d2:midi note #						;
;		a4:slot work top	0x008000			;
;		a5:scsp register top	0x100000			;
;		a6:common work top	0x008800			;
;	out	
;	break	d0,d3,d4,d5,d6,d7,a0,a1,a2,a3,a4			;
;	keep	d1,d2,a5,a6						;
;	free	a3							;
;-----------------------------------------------------------------------;
Note_off:	move.b	d1,(note_on_data+midi_track_note+0)
		move.b	d2,(note_on_data+midi_track_note+1)
		lea	slot_work(a6),a4		;
		movem.l	a4/a5,-(sp)			;
		moveq	#0,d0				;
		move.w	d0,-(sp)			;
		moveq	#slot_size-1,d7			;
OF_0:		moveq	#0,d4				;
	;-------------------------------------------------------;
?slot_loop:	btst.b	#noteOn,slot_status+1(a4)		;note on check
		beq.s	?function_false			;
	.if ENGN
		move.b	sl_flag1(a4),d6			;engine ctrl use check
		bmi.w	?function_false			;
	.endif			; ENGN
		move.w	_sl_kanri(a4),d6		;
		cmp.w	_tmp_kanri(a6),d6		;management # check
		bne	?function_false			;
		move.b	(note_on_data+midi_track_note+0),d0
		cmp.b	sl_MIDI(a4),d0			;midi ch# check
		bne	?function_false			;
		move.b	(note_on_data+midi_track_note+1),d0
		cmp.b	sl_note(a4),d0			;midi note# check
		bne	?function_false			;
		move.b	PSPN(a4),d6			;pcm stream check
		bmi.w	?function_false			;
	;------------------------------------------------------;
		move.w	knr_kanri_ofst(a6),d6		;
		btst.b	#damper,midi_flag(a6,d6.w)	;damper check
		bne	?next_step1			;
		btst.b	#PEON_flg,sl_flag1(a4)		;pitch eg check
		beq.s	?next_step			;
		bsr	PEG_off				;pitch eg work init.
	;------- scsp key off write ----------------------------;
?next_step:	move.b	(a5),d5				;
		andi.b	#%00000111,d5			; clear KYONB
		ori.b	#%00010000,d5			; set   KYONEX
		move.b	d5,(a5)
	;------- goto next slot check --------------------------;
?next_step1:	bclr.b	#noteOn,slot_status+1(a4)		;note on bit clear
	.ifz rereaseCheck
		move.b	#0,slot_status(a4)		;FM clear		when no rerease check
	.endif			; rereaseCheck

		move.w	d4,$02(sp)			;
?function_false:
		addi.w	#$0040,d4			;
		adda.w	#$0040,a4			;
		adda.w	#$0020,a5			;
		dbra	d7,?slot_loop			;
	;-------------------------------------------------------;
		move.w	(sp)+,d4			
		beq	end_of_note_off
		move.b	(note_on_data+midi_track_note+0),d1
	;-------------------------------------------------------;
	;	ÇcÇrÇo ÇbÇoÇtãÏìÆÇc-ÇeilterÇÃåüçı		;
	;	in	d1:MIDI channel				;
	;-------------------------------------------------------;
DFLTR_off:	lea	_DFL_ELMNT_wk(a6),a2		;0x1e00
		clr.w	d6				;
		move.b	DFL_ELMNT_NO(a6),d6		;element #
		beq.s	?dfltr_off_1			;
		movea.l	DFL_ELMNT_addr(a6),a1		; = Element data top
		subq.w	#1,d6				;
		andi.w	#$001f,d6			;
	;-------------------------------------------------------;
?dfltr_off_3:	cmp.b	(a1),d1				; Element MIDI ch = MIDI# ?
		bne.s	?dfltr_off_2			;
		move.b	1(a1),d0			;
		cmpi.b	#8,d0				; 1000 ?
		beq.s	?dfltr_off_lfo			; jump if LFO
		bcs.s	?dfltr_off_2			; jump if LFO/EG off
?dfltr_off_eg:	move.b	#6,DFL_EG_seg(a2)		; ã≠êß set seg#6
		move.w	DFL_add_wk(a2),d4		; = $0000Å`$FF00
		move.b	15(a1),d0			; = [RL]
		lsl.w	#8,d0				; = -$8000Å`$7F00
		addi.w	#$8000,d0			; = $0000Å`$FF00
		move.w	d0,DFL_level(a2)		; set ìûíBÉåÉxÉã
		cmp.w	d0,d4				;
		bcc.s	?dfl_off_eg_1			; jump if [RL] < add_wk
		move.b	14(a1),d0			; = [RR]
		bpl.s	?dfl_off_eg_2			;
		bra.s	?dfl_off_eg_3			;
?dfl_off_eg_1:	move.b	14(a1),d0			; = [RR]
		bmi.s	?dfl_off_eg_2			;
?dfl_off_eg_3:	neg.b	d0				;
?dfl_off_eg_2:	bsr	get_EXPTBS			; return d0.w : -$3F01Å`+$3F01
		move.w	d0,DFL_add_bs(a2)		; set rate
		bra.s	?dfltr_off_2			;
?dfltr_off_lfo:	move.w	#0,DFL_add_bs(a2)		; LFO only ( don't destroy EG )
?dfltr_off_2:	lea	$18(a1),a1			;
		lea	$10(a2),a2			;
		dbra	d6,?dfltr_off_3			;
?dfltr_off_1:
end_of_note_off:
		movem.l	(sp)+,a4/a5			;
		rts					;

;-----------------------------------------------------------------------;
;	Pitch EG ÇÃoff							;
;	--------------------------------------------------------	;
;	in	a4:slot work address 		0x008000+0x40*slot #	;
;		a5:scsp register address	0x100000+0x20*slot #	;
;	break	d0,d3,d5,d6,a1						;
;-----------------------------------------------------------------------;
PEG_off:	move.b	#6,PEG_SEG(a4)			;
		movea.l	PEG_addr(a4),a1			;
		addq	#8,a1				;
		move.b	(a1)+,d0			;read RR
		beq.s	?peg_off_3			;
		bsr	get_EXPTB2			;d0.w = -$3F01 to +$3F01
		moveq	#0,d6				;
		move.w	d0,d6				;
		swap	d6				;
		asr.l	#4,d6				;-$03f01000 to +$03f01000
		move.b	(a1),d0				;read RL
		bsr	get_LEVEL			;d0.w = -$5FFF to +$5FFF
		moveq	#0,d5				;
		move.w	d0,d5				;
		swap	d5				;-$5fff0000 to +$5fff0000
		move.l	#$80000000,d0			;
		move.l	d5,PEG_level(a4)		; set ìûíBÉåÉxÉã
		add.l	d0,d5				;$20010000 to $dfff0000
		move.l	PEG_cent(a4),d3			;
		add.l	d0,d3				;
		cmp.l	d3,d5				;
		beq.s	?peg_off_3			; jump if _cent = _level
		bcc.s	?peg_off_2			; jump if _cent < _level
		move.l	d6,d6				;
		bmi.s	?peg_off_exit			;
		bra.s	?peg_off_4			;
?peg_off_2:	move.l	d6,d6				;rate check
		bpl.s	?peg_off_exit			;
?peg_off_4:	neg.l	d6				;
?peg_off_exit:	move.l	d6,PEG_RATE(a4)			; set åXÇ´Å^4msec
		rts					;
?peg_off_3:	addq.b	#1,PEG_SEG(a4)			;
		rts					;

;=======================================================================;
;	end of file							;
;=======================================================================;
