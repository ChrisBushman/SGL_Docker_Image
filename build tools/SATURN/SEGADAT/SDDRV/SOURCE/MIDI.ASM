;=======================================================================;
;	SEGASATURN sound driver						;
; file name								;
;	midi.asm							;
; copyright								;
;	SEGA ENTERPRISES						;
; purpose								;
;	MIDI input control						;
; author(s)								;
;	1995-11-14	A.Miyazawa	Ver.2.00			;
; mod history								;
;	1996-02-23	A.Miyazawa	Ver.2.06			;
;	1996-07-02	A.Miyazawa	Ver.2.10L			;
;	1997-06-30	Y.Kashima	Ver.2.20			;
;=======================================================================;

		.include	SCSP.LIB

			.extern	PEG_key_off		;trgt.asm
			.extern	Note_on			;
			.extern	Note_off		;
			.extern	MIXER_wr,PRG_chg	;
			.extern	CTRL_10			;
			.extern	CTRL_20			;
			.extern	PEG_off			;
			.extern	send_user		;
			.extern	xseq_qsound		;
			.extern	xseq_YMH3Dw		;

			.global	MIDI_ctrl
;-----------------------------------------------------------------------;

;  <<<< 検索文字列 >>>>
;
;	  CTRL_01	モジュレーション  ホィール			
;	  MIDI_En	ピッチ  ベンド					
;	  PRG_chg	プログラム （音色）チェンジ			
;	  CTRL_5B	Ｅffect チェンジ（ＤＳＰ Ｍicro Ｐrogram 更新）	
;	  CTRL_40	ダンパー					
;	  CTRL_20	ＢＡＮＫチェンジ	----> CHG.ASM		
;	  CTRL_10	ＭＩＸＥＲチェンジ				
;	  CTRL_07	音量コントロール				
;	  CTRL_0A	ＰＡＮコントロール				
;	  CTRL_11	Ｅffect パン チェンジ				
;	  CTRL_47	Ｅffect Ｖol チェンジ				
;	  CTRL_50	3D


;-----------------------------------------------------------------------;
;【 機 能 】 割込みにてバッファリングされたＭＩＤＩデータを取り出して	;
;	     ノートオン/オフ、プログラムチェンジ、コントロールチェンジ	;
;            等を制御する。						;
;	in	a5	scsp i/o					;
;		a6	work ram top					;
;		d0.w	midi buffer read offset point			;
;	out	d2.b	2nd paramater					;
;		d3.b	3rd paramater					;
;	break	d0,d1,d2,d3,d4						;
;	keep	a5,a6							;
;-----------------------------------------------------------------------;
			.public	MIDI_ctrl
			.public	MIDI_ctrl_pt
MIDI_ctrl:	addq.b	#1,_MIDI_RCV_RDPT(a6)		; read point of MIDI receive buffer.
		lea	slot_work(a6),a4		
		moveq	#0,d1				
		moveq	#0,d2				
		moveq	#0,d3				
		movea.l	#midi_receive_buffer,a1		
		add.w	d0,d0				;bit 31-7 are already cleared before
		add.w	d0,d0				; call this routine.
		adda.w	d0,a1				
MIDI_ctrl_pt:	bsr.w	set_MIDI_OUT_BF			; SCSP-MIDI OUT
		move.b	(a1)+,d0			; Priority(7-3) & コマンド(2-0)
		move.b	(a1)+,d1			; bit 7-5 is management number(0-7),
		move.b	d1,d2				; bit 3-0 is midi channel(0-15).
		andi.w	#$00e0,d2			; already, d2 is management number*0x20.
		add.w	d2,d2				
		add.w	d2,d2				
		add.w	d2,d2				
		add.w	d2,d2				; d2 = management number(0-7)*0x200.
		move.w	d2,_tmp_kanri(a6)		; = 000,200,400,..
		andi.w	#$000f,d1			; = MIDI ch#
		cmpi.w	#$0010,d1			
		bcc.s	?error				;midi channel, 0x10 to 0x1f are
		move.w	d1,d2				; not available on ver-1.41 or later.
		lsl.w	#5,d2				; MIDI channel number*0x20
		add.w	_tmp_kanri(a6),d2		; = 発音管理番号×200H
		move.w	d2,knr_kanri_ofst(a6)		;
		move.w	d1,_tmp_MIDI_ch(a6)		; word	; $0000 ～ $001F
		move.b	(a1)+,d2			; 2nd Message ( Note# )
		move.b	(a1),d3				; 3rd Message ( Velo )
		andi.w	#$0007,d0			;
		add.w	d0,d0				;
		movea.w	?jump_table(pc,d0.w),a1		;
		jmp	(a1)				;
	;-------------------------------------------------------;
?error:		move.b	#TOO_LARGE_CHANNEL,d0		;
		bsr	send_user			;
		rts					;
	;-------------------------------------------------------;
?jump_table:	dc.w	Note_off			; 8n ; note off
		dc.w	Note_on				; 9n : note on
		dc.w	er_1C				; An : polyphonic key pressure
		dc.w	CTRL_CHG			; Bn : control change
		dc.w	PRG_chg				; Cn : program change
		dc.w	er_1D				; Dn : channel pressure
		dc.w	MIDI_En				; En : pitch bend change
		dc.w	er_1E				; Fn :

;-----------------------------------------------------------------------;
;		68K CPU 処理 MIDI の バッファリング			;
; destroy d0/d1/d4							;
;	  a2								;
;-----------------------------------------------------------------------;
			.global	set_MIDI_OUT_BF	
set_MIDI_OUT_BF:
		lea	_MIDI_OUT_BF(a6),a2	;
		move.w	_MIDI_OUT_WRPT(a6),d4	; byte
		andi.w	#$3FF,d4		;
		move.b	(a1),d0			; Priority(7-3) & コマンド(2-0)
		lsl.b	#4,d0			;
		ori.b	#$80,d0			;
		move.b	1(a1),d1		;発音管理番号 ＆ MIDI ch#
		andi.b	#$0F,d1			;d1=MIDI ch#
		or.b	d1,d0			;d0=MIDI cmd
		move.b	d0,(a2,d4.w)		;
		addq.w	#1,d4			;
		andi.w	#$3FF,d4		;
		cmpi.b	#$C0,d0			;
		bcs.w	MIDI_out_st_3		;jump if 3byte out
		cmpi.b	#$E0,d0			;
		bcc.w	MIDI_out_st_3		;jump if 3byte out
		move.b	2(a1),(a2,d4.w)		;MIDI pram#1
		bra.w	MIDI_out_st_2		; jump if 2byte out
MIDI_out_st_3:	move.b	2(a1),(a2,d4.w)		; MIDI 2nd
		addq.w	#1,d4			;
		andi.w	#$3FF,d4		;
		move.b	3(a1),(a2,d4.w)		; MIDI 3rd
MIDI_out_st_2:	addq.w	#1,d4			;
		andi.w	#$3FF,d4		;
		move.w	d4,_MIDI_OUT_WRPT(a6)	; byte
		rts
;-----------------------------------------------------------------------;
er_MIDI2_in:	bset.b	#ERRa_9,Mem_DRVERR_FLG+0	; MIDI ch 10H～1FH input
		rts
er_1C:		bset.b	#ERRa_2,Mem_DRVERR_FLG+1	; MIDI Polyphonic Key pressure
		rts
er_1D:		bset.b	#ERRa_3,Mem_DRVERR_FLG+1	; MIDI channel pressure
		rts
er_1E:		bset.b	#ERRa_4,Mem_DRVERR_FLG+1	; MIDI F0H～FFH input
		rts
;-----------------------------------------------------------------------;
;【 機 能 】 各々のＭＩＤＩコントロールチェンジ[$Bn,$cc,$xx]番号へ分岐	*
;	     させる。未定義のコントロール番号に対してはエラー処理（プロ	*
;	     グラム開発用）または無処理（実機用）とする。		*
;【 入 力 】a6   : work RAM top						*
;	    d1.w : MIDI ch#            	( 00 ～ 1FH )			*
;	    d2.b : MIDI Control Number 	( 00 ～ 7FH )			*
;           d3.b : Parameter 		( 00 ～ 7FH )			*
;【 出 力 】nothing							*
;【destroy】d1.w							*
;【 hold  】a5/a6						94/07/28*
;-----------------------------------------------------------------------;
			.public	CTRL_CHG
CTRL_CHG:	move.w	d2,d4				
		andi.w	#$007f,d2			
		add.w	d2,d2				
		movea.w	MIDI_Bn_TB(pc,d2.w),a0		
		jmp	(a0)				
	;-------------------------------------------------------;
MIDI_Bn_TB:	dc.w	_______		; $00
		dc.w	CTRL_01		; $01 音源ハード Modulation Wheel
		dc.w	CTRL_0B		; $02 Map change
		dc.w	_______		; $03
		dc.w	_______		; $04 不特定（フットタイプ操作子）
		dc.w	_______		; $05 Portament time
		dc.w	_______		; $06 data entry MSB ( RPN/NRPN )
		dc.w	CTRL_07		; $07 Main Volume
		dc.w	_______		; $08 Balance Control
		dc.w	_______		; $09
		dc.w	CTRL_0A		; $0A Direct Pan
		dc.w	CTRL_0B		; $0B Expression
		dc.w	_______		; $0C
		dc.w	_______		; $0D
		dc.w	_______		; $0E
		dc.w	_______		; $0F
		dc.w	CTRL_10		; $10 Mixer change
		dc.w	CTRL_11		; $11 Effect Pan
	.if ENGN
		dc.w	CTRL_12		; $12 不特定（汎用操作子３）
		dc.w	CTRL_13		; $13 不特定（汎用操作子４）
	.else
		dc.w	_______		; $12 不特定（汎用操作子３）
		dc.w	_______		; $13 不特定（汎用操作子４）
	.endif
		dc.w	_______		; $14
		dc.w	_______		; $15
		dc.w	_______		; $16
		dc.w	_______		; $17
		dc.w	_______		; $18
		dc.w	_______		; $19
		dc.w	_______		; $1A
		dc.w	_______		; $1B
		dc.w	_______		; $1C
		dc.w	_______		; $1D
		dc.w	_______		; $1E
		dc.w	_______		; $1F Sequencer loop mode
		dc.w	CTRL_20		; $20 Tone BANK change
		dc.w	_______		; $21
		dc.w	_______		; $22
		dc.w	_______		; $23
		dc.w	_______		; $24
		dc.w	_______		; $25
		dc.w	_______		; $26 data entry LSB ( RPN/NRPN )
		dc.w	_______		; $27
		dc.w	_______		; $28
		dc.w	_______		; $29
		dc.w	_______		; $2A
		dc.w	_______		; $2B
		dc.w	_______		; $2C
		dc.w	_______		; $2D
		dc.w	_______		; $2E
		dc.w	_______		; $2F
		dc.w	_______		; $30
		dc.w	_______		; $31
		dc.w	_______		; $32
		dc.w	_______		; $33
		dc.w	_______		; $34
		dc.w	_______		; $35
		dc.w	_______		; $36
		dc.w	_______		; $37
		dc.w	_______		; $38
		dc.w	_______		; $39
		dc.w	_______		; $3A
		dc.w	_______		; $3B
		dc.w	_______		; $3C
		dc.w	_______		; $3D
		dc.w	_______		; $3E
		dc.w	_______		; $3F
		dc.w	CTRL_40		; $40 Damper
		dc.w	_______		; $41 Portament
		dc.w	_______		; $42 Sosutenute(Code Hold)
		dc.w	_______		; $43 Soft Pedal
		dc.w	_______		; $44
		dc.w	_______		; $45 Hold2(Freeze)
		dc.w	CTRL_11		; $46 Effect Pan
		dc.w	CTRL_47		; $47 Effect return
		dc.w	_______		; $48
		dc.w	_______		; $49
		dc.w	_______		; $4A
		dc.w	_______		; $4B
		dc.w	_______		; $4C
		dc.w	_______		; $4D
		dc.w	_______		; $4E
		dc.w	_______		; $4F
		dc.w	CTRL_50		; $50 Q sound position
		dc.w	CTRL_51		; $51 3D 
		dc.w	CTRL_52		; $52 3D horizontal position
		dc.w	CTRL_53		; $53 3D vertical position
		dc.w	_______		; $54
		dc.w	_______		; $55
		dc.w	_______		; $56
		dc.w	_______		; $57
		dc.w	_______		; $58
		dc.w	_______		; $59
		dc.w	_______		; $5A
		dc.w	CTRL_5B		; $5B Effect change
		dc.w	_______		; $5C Tremono depth
		dc.w	_______		; $5D Chorus depth
		dc.w	_______		; $5E Seleste depth
		dc.w	_______		; $5F phaser depth
		dc.w	_______		; $60 data increment
		dc.w	_______		; $61 data decrement
		dc.w	_______		; $62 NRPN LSB
		dc.w	_______		; $63 NRPN MSB
		dc.w	_______		; $64 RPN  LSB
		dc.w	_______		; $65 PRN  MSB
		dc.w	_______		; $66
		dc.w	_______		; $67
		dc.w	_______		; $68
		dc.w	_______		; $69
		dc.w	_______		; $6A
		dc.w	_______		; $6B
		dc.w	_______		; $6C
		dc.w	_______		; $6D
		dc.w	_______		; $6E
		dc.w	_______		; $6F
		dc.w	_______		; $70
		dc.w	_______		; $71
		dc.w	_______		; $72
		dc.w	_______		; $73
		dc.w	_______		; $74
		dc.w	_______		; $75
		dc.w	_______		; $76
		dc.w	_______		; $77
		dc.w	_______		; $78
		dc.w	_______		; $79 RES ALL CTRLER
		dc.w	_______		; $7A local control
		dc.w	CTRL_7B		; $7B all note off
		dc.w	_______		; $7C OMNI OFF
		dc.w	_______		; $7D omni mode on
		dc.w	_______		; $7E mono mode
		dc.w	_______		; $7F Play mode

_______:	bset.b	#ERRb14,Mem_err_bit+2	; 不正 Control change# input
		rts

;-----------------------------------------------------------------------;
;	MIDI CC17	(control change $11:Effect pan)			;
;	--------------------------------------------------------	;
;	in	a5	scsp register top				;
;		a6	management work top				;
;		d1.w	mixer input # (as midi channel 0 ～ 1F )	;
;		d3.b	pan data ( 00 ～ 7FH )				;
;-----------------------------------------------------------------------;
			.global	CTRL_11
CTRL_11:	andi.b	#$7F,d3				;
		lsr.b	#2,d3				;
		subi.b	#$10,d3				;
		bcc.s	CTRL_11_2			;
		not.b	d3				;
		addi.b	#$10,d3				;
	;-------------------------------------------------------;
CTRL_11_2:	move.b	SND_OUT_ST(a6),d2		;MONO/STEREO status check
		bpl.s	CTRL_11_ST			;
		andi.b	#$E0,d3				;monaural
CTRL_11_ST:	move.w	d1,d2				;
		addi.w	#mixer_wk_SCSP,d2		;
		move.b	(a6,d2.w),d0			;read effect send level from work
		andi.b	#$E0,d0				;
		or.b	d0,d3				;
		move.b	d3,(a6,d2.w)			;write to work
		lsl.w	#5,d1				;
		move.b	d3,SCSP_EFSDLPN(a5,d1.w)	;write to scsp
CTRL_11_ret:	rts					;

	.if	ENGN
;-----------------------------------------------------------------------;
;	midi cc18	(control change $12:engine rpm control)		;
;	--------------------------------------------------------	;
;	in	a5	scsp top					;
;		a6	work ram top					;
;		d1.w	midi channel					;
;		d3.b	paramater					;
;-----------------------------------------------------------------------;
CTRL_12:	move.l	ENGN_wk_cmn+ENGN_wk_TONE(a6),d0	;tone start address
		beq.s	?end				;
		andi.w	#$7F,d3				;
		add.w	d3,d3				;
		move.w	rpm_chg_tb(pc,d3.w),d0		;read rpm data
		lea	racing_interface,a1		;
		move.w	d0,ENGN_P1_rpm(a1)		;set rpm data to work
?end:		rts					;
	;-------------------------------------------------------;
rpm_chg_tb:	dc.w	$45*$00,$45*$01,$45*$02,$45*$03,$45*$04,$45*$05,$45*$06,$45*$07
		dc.w	$45*$08,$45*$09,$45*$0A,$45*$0B,$45*$0C,$45*$0D,$45*$0E,$45*$0F
		dc.w	$45*$10,$45*$11,$45*$12,$45*$13,$45*$14,$45*$15,$45*$16,$45*$17
		dc.w	$45*$18,$45*$19,$45*$1A,$45*$1B,$45*$1C,$45*$1D,$45*$1E,$45*$1F
		dc.w	$45*$20,$45*$21,$45*$22,$45*$23,$45*$24,$45*$25,$45*$26,$45*$27
		dc.w	$45*$28,$45*$29,$45*$2A,$45*$2B,$45*$2C,$45*$2D,$45*$2E,$45*$2F
		dc.w	$45*$30,$45*$31,$45*$32,$45*$33,$45*$34,$45*$35,$45*$36,$45*$37
		dc.w	$45*$38,$45*$39,$45*$3A,$45*$3B,$45*$3C,$45*$3D,$45*$3E,$45*$3F
		dc.w	$45*$40,$45*$41,$45*$42,$45*$43,$45*$45,$45*$45,$45*$46,$45*$47
		dc.w	$45*$48,$45*$49,$45*$4A,$45*$4B,$45*$4C,$45*$4D,$45*$4E,$45*$4F
		dc.w	$45*$50,$45*$51,$45*$52,$45*$53,$45*$54,$45*$55,$45*$56,$45*$57
		dc.w	$45*$58,$45*$59,$45*$5A,$45*$5B,$45*$5C,$45*$5D,$45*$5E,$45*$5F
		dc.w	$45*$60,$45*$61,$45*$62,$45*$63,$45*$64,$45*$65,$45*$66,$45*$67
		dc.w	$45*$68,$45*$69,$45*$6A,$45*$6B,$45*$6C,$45*$6D,$45*$6E,$45*$6F
		dc.w	$45*$70,$45*$71,$45*$72,$45*$73,$45*$74,$45*$75,$45*$76,$45*$77
		dc.w	$45*$78,$45*$79,$45*$7A,$45*$7B,$45*$7C,$45*$7D,$45*$7E,$45*$7F

;-----------------------------------------------------------------------;
;	midi cc19	(control change $13:engine accel control)	;
;	--------------------------------------------------------	;
;	in	a5	scsp top					;
;		a6	work ram top					;
;		d1.w	midi channel					;
;		d3.b	paramater					;
;-----------------------------------------------------------------------;
CTRL_13:	move.l	ENGN_wk_cmn+ENGN_wk_TONE(a6),d0	;tone start address
		beq.s	?end				;
		andi.l	#$78,d3				;
		lsr.l	#3,d3				;
		lea	racing_interface,a1		;
		move.l	d3,ENGN_P1_accl(a1)		;set accel data to work
?end:		rts					;
	.endif			;ENGN

;-----------------------------------------------------------------------;
;	MIDI CC50	(control change $46:Effect return)		;
;	--------------------------------------------------------	;
;	in:	a5	FH1005 top					;
;		a6	management work top				;
;		d1.w	mixer input # (as midi channel 0 ～ 1F )	;
;		d3.b	effect send level data ( 00 ～ 7FH )		;
;-----------------------------------------------------------------------;
			.global	CTRL_47
CTRL_47:
		cmpi.b	#$12,d1				;
		bcc	CTRL_47_ret			;
		andi.b	#$70,d3				;10,20,30,40,50,60,70
		add.b	d3,d3				;bit shift
		move.w	d1,d2				;
		addi.w	#mixer_wk_SCSP,d2		;
		move.b	(a6,d2.w),d0			;read effect panpot from work
		andi.b	#$1F,d0				;
		or.b	d0,d3				;
		move.b	d3,(a6,d2.w)			;write to work
		lsl.w	#5,d1				;
		move.b	d3,SCSP_EFSDLPN(a5,d1.w)	;write to scsp
CTRL_47_ret:	rts					;
;-----------------------------------------------------------------------;
;	MIDI CC1	(control change $01:Modulation wheel)		;
;	--------------------------------------------------------	;
;	in:	a5	FH1005 top					;
;		a6	management work top				;
;		d1.w	MIDI ch# ( 0 ～ 1F )				;
;		d3.b	Modulation data ( 00 ～ 7FH )			;
;-----------------------------------------------------------------------;
			.global	CTRL_01
CTRL_01:	move.w	_tmp_kanri(a6),d2		; = 000,200,400,..,E00
		add.b	d3,d3				; $00～$3F:off / $40～$7F:on
		andi.b	#$E0,d3				; = xxx0 0000B : [PLFOS]
		moveq	#slot_size-1,d7			;
		clr.w	d4				;
		lea	slot_work(a6),a0		;
	;-------------------------------------------------------;
CTRL_01_lp:	cmp.b	sl_MIDI(a0,d4.w),d1		;MIDI ch# equal ?
		bne.s	CTRL_01_1			;
		move.b	PSPN(a4,d4.w),d0		;PCM Stream playing ?
		bmi.w	CTRL_01_1			;
		cmp.w	_sl_kanri(a0,d4.w),d2		;management# equal ?
		bne.s	CTRL_01_1			;
	;-------------------------------------------------------;
		movea.l	sl_layer_adr(a0,d4.w),a1	;
		move.b	LY_SISD(a1),d0			;Mod-Wheel execute ?
		beq.s	CTRL_01_1			;

		move.b	LY_LFOS(a1),d5			;
		move.b	d3,d3				;
		bmi.s	CTRL_01_2			; jump if velo = $40 ～ $7F
		andi.b	#$18,d5				; LFO sens <-- 0
CTRL_01_2:	lsr.w	#1,d4				; = $00,$20,$40,...,$3E0
		move.b	d5,SCSP_PLFOS(a5,d4.w)		; [PLFOS] & [ALFOWS] & [ALFOS]
		add.w	d4,d4				; = $00,$40,$80,...,$7C0
CTRL_01_1:	addi.w	#slot_wk_unit,d4		;next slot
		dbra	d7,CTRL_01_lp			;
		rts					;

;-----------------------------------------------------------------------;
;	MIDI CC123	(control change $7B:All note off)		;
;	--------------------------------------------------------	;
;	in:
;		a6:management work top				;
;-----------------------------------------------------------------------;
			.global	CTRL_7B
			.public		all_note_off
CTRL_7B:	move.w	#ALL_NOTE_OFF_INPUT,d0		;$91
		bsr	send_user			;write status to host i/f
	;-------------------------------------------------------;
	;	
	;-------------------------------------------------------;
all_note_off:	movem.l	d0-d7/a0-a6,-(sp)		;
		lea.l	slot_work(a6),a4		;($8000)
		movea.l	#IO_SCSP,a5			;($100000)
		moveq	#slot_size-1,d7			;
?slot_loop:	clr.w	slot_status(a4)			;slot status clear
		clr.b	sl_flag1(a4)			;engine,eg,lfo flag clear
	;------- key off ---------------------------------------;
		move.b	(a5),d0				;
		andi.b	#%00000111,d0			;
		ori.b	#%00010000,d0			;
		move.b	d0,(a5)				;key off
		adda.w	#$0040,a4			;next slot
		adda.w	#$0020,a5			;
		dbra	d7,?slot_loop			;
	;-------------------------------------------------------;
		movea.l	a6,a0				;
		moveq	#16*8-1,d7			;
?midi_ch_loop:	clr.b	midi_flag(a0)			;
		adda.w	#$0020,a0			;
		dbra	d7,?midi_ch_loop		;
		movem.l	(sp)+,d0-d7/a0-a6		;
		rts					;

;-----------------------------------------------------------------------;
;	MIDI CC11	(control change $0B:Expression)			;
;	--------------------------------------------------------	;
;	in:	a5	FH1005 top					;
;		a6	management work top				;
;		d1.w	MIDI ch# ( 0 ～ 1F )				;
;		d3.b	expression data ( 00 ～ 7FH )			;
;-----------------------------------------------------------------------;
			.public		CTRL_0B
CTRL_0B:
		andi.w	#$007f,d3
		move.w	_tmp_kanri(a6),d2		; d2= management number * 0x200
		move.w	d1,d0				; d1=midi channel number
		lsl.w	#5,d0
		add.w	d2,d0				; d0= d2+midi channel * 0x20
		move.w	d3,midi_expression(a6,d0.w)
		bra.s	scsp_access
;-----------------------------------------------------------------------;
;	MIDI CC7	(control change $07:Volume)			;
;	--------------------------------------------------------	;
;	in:	a5	FH1005 top					;
;		a6	management work top				;
;		d1.w	MIDI ch# ( 0 ～ 1F )				;
;		d3.b	channel volume data ( 00 ～ 7FH )		;
;-----------------------------------------------------------------------;
			.public		CTRL_07
CTRL_07:
		andi.w	#$007f,d3			;
		move.w	_tmp_kanri(a6),d2		;d2= management number * 0x200
		move.w	d1,d0				;d1=midi channel number
		lsl.w	#5,d0				;
		add.w	d2,d0				; d0= d2+midi channel * 0x20
		move.w	d3,midi_volume(a6,d0.w)		;
	;-------------------------------------------------------;
scsp_access:
		move.w	midi_expression(a6,d0.w),d3	;
		move.w	midi_volume(a6,d0.w),d4		;
		mulu	d4,d3				;  d3,d2=(0 to 0x7f)
		lsr.w	#7,d3				;
		move.w	d3,midi_master_volume(a6,d0.w)	;
		moveq	#slot_size-1,d7			;
		moveq	#0,d4				;
		lea	slot_work(a6),a0		;
	;-------------------------------------------------------;
	?slot_loop:
		cmp.w	_sl_kanri(a0,d4.w),d2		; check management number
		bne.s	?function_false
		cmp.b	sl_MIDI(a0,d4.w),d1		; check midi channel
		bne.s	?function_false
		btst.b	#PCM_flg,PSPN(a0,d4.w)		; check PCM stream
		bne.s	?function_false
	.if ENGN		;ENGN
		btst.b	#ENGN_flg,sl_flag1(a0,d4.w)	; Engine ?
		bne.s	?function_false			; jump if yes
	.endif			;ENGN
		btst.b	#flg_FMCR,sl_flag2(a0,d4.w)
		beq.s	?function_false
	;-------------------------------------------------------;
		movem.l	d2/a1,-(sp)			;
		move.w	slot_velocity(a0,d4.w),d2	;
		move.w	midi_master_volume(a6,d0.w),d3	;
		mulu	d2,d3				;0 to 0x7f
		lsr.w	#6,d3				;8bit
		move.w	d3,total_volume(a6,d0.w)	;
		movea.l	management(a6,d0.w),a1		;
		move.w	sequence_volume(a1),d2		;
		eori.b	#$80,d2				;
		ext.w	d2				;
		add.w	d2,d3				;
		bmi.s	?overflow_underflow		;
		cmpi.w	#$0100,d3			;
		bcc.s	?overflow_underflow		;
		bra.s	?result				;
	?overflow_underflow:
		tst.w	d3				;
		spl.b	d3				;
	?result:
		not.b	d3				;
		movem.l	(sp)+,d2/a1			;
	;-------------------------------------------------------;
		lsr.w	#1,d4				; d4 = $000,$020,$040,..,$3E0
		move.b	d3,SCSP_TLVL(a5,d4.w)		;
		add.w	d4,d4				; d4 = $000,$040,$080,..,$7C0
	?function_false:
		addi.w	#slot_wk_unit,d4	; +$40
		dbra	d7,?slot_loop
		rts
;-----------------------------------------------------------------------;
;	MIDI CC10	(control change $0A:Direct Pan)			;
;	--------------------------------------------------------	;
;	in:	a5	FH1005 top					;
;		a6	management work top				;
;		d1.w	MIDI ch# ( 0 ～ 1F )				;
;		d3.b	panpot data ( 00 ～ 7FH )			;
;	--------------------------------------------------------	;
;	midi:	00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f		;
;	scsp:	1f 1e 1d 1c 1b 1a 19 18 17 16 15 14 13 12 11 10		;
;	midi:	10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f		;
;	scsp:	00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f		;
;-----------------------------------------------------------------------;
			.public		CTRL_0A
CTRL_0A:
		andi.b	#$7F,d3				;
		lsr.b	#2,d3				;
		subi.b	#$10,d3				;
		bcc.s	?100				;d3.b=00-0f then skip
		not.b	d3				;
		addi.b	#$10,d3				;d3.b=10-1f
?100:		move.w	knr_kanri_ofst(a6),d0		;
		bset.b	#7,midi_panpot(a6,d0.w)		;set layer pan off
		move.b	midi_panpot(a6,d0.w),d4		;
		btst	#6,d4				;seq pan check
		beq.s	?sequence_pan_off		;
	;------- sequence pan on -------------------------------;
		btst.b	#7,SND_OUT_ST(a6)		;mono/stereo status check
		beq.s	?end_of_control			;seq pan onでステレオのときseq panにしたがう
		;------- monaural
?mono:		clr.w	d3				;d3.b=0 (center)
		bra.s	?start_control			;
	;------- sequence pan off ------------------------------;
?sequence_pan_off:
		btst.b	#7,SND_OUT_ST(a6)		; MONO/STEREO status
		bne.s	?mono				;
		;------- stereo
		ori.b	#$80,d3				;set layer pan off
		move.b	d3,midi_panpot(a6,d0.w)		;
		andi.b	#$1F,d3				;
		move.w	_tmp_kanri(a6),d0		;
	;-------------------------------------------------------;
	;	apply panpot all slot of same midi channel	;
	;-------------------------------------------------------;
?start_control:	moveq	#slot_size-1,d7			;loop size
		moveq	#0,d4				;
		lea	slot_work(a6),a0		;
	;-------------------------------------------------------;
?slot_loop:	cmp.b	sl_MIDI(a0,d4.w),d1		;midi channel# check
		bne	?function_false			;
		cmp.w	_sl_kanri(a0,d4.w),d0		;management# check
		bne	?function_false			;
		btst.b	#PCM_flg,PSPN(a0,d4.w)		;PCM stream playing check
		bne.w	?function_false			;
	.if ENGN
		btst.b	#ENGN_flg,sl_flag1(a0,d4.w)	;engine channel check
		bne.w	?function_false			;
	.endif
	;-------------------------------------------------------;
		lsr.w	#1,d4				;
		move.b	SCSP_DISDLPN(a5,d4.w),d5	;read direct pan data
		andi.b	#$E0,d5				;
		or.b	d3,d5				;
		move.b	d5,SCSP_DISDLPN(a5,d4.w)	;set new direct pan data
		add.w	d4,d4				;d4 recover
?function_false:
		addi.w	#slot_wk_unit,d4		;next slot
		dbra	d7,?slot_loop			;
?end_of_control:
		rts					;

;-----------------------------------------------------------------------;
;	MIDI CC64	(control change $40:damper)			;
;	--------------------------------------------------------	;
;	in:	a5	FH1005 top					;
;		a6	management work top				;
;		d1.w	MIDI ch# ( 0 ～ 1F )				;
;		d3.b	damper data ( 00 ～ 7FH )			;
;-----------------------------------------------------------------------;
			.public		CTRL_40
			.public		func_damper_off
CTRL_40:
		move.w	d1,d7				;
		lsl.w	#5,d7				;management# x $20
		move.w	_tmp_kanri(a6),d6		;management# x $200
		add.w	d6,d7				;
		andi.b	#%01000000,d3			;damper on/off check
		bne.s	func_damper_on			;
	;-------------------------------------------------------;
	;	damper off					;
	;-------------------------------------------------------;
		bclr.b	#damper,midi_flag(a6,d7.w)	;damper mode clear(=off)
func_damper_off:
		movem.l	a0/a1/a4/a5,-(sp)		;
		lea	slot_work(a6),a4		;
		movea.l	#IO_SCSP,a5			;
		moveq	#slot_size-1,d7			;
		moveq	#0,d4				;
	?main_loop:
	;-------------------------------------------------------;
		cmp.w	_sl_kanri(a4),d6		;management# equal check
		bne.s	?function_false

		cmp.b	sl_MIDI(a4),d1			;MIDI ch# equal check
		bne.s	?function_false

		move.b	PSPN(a4),d6			;pcm stream check
		bmi.w	?function_false			;

		btst.b	#noteOn,slot_status+1(a4)		;note on check
		bne.s	?function_false
	;-------------------------------------------------------;
		btst.b	#PEON_flg,sl_flag1(a4)		;pitch env. on flag check
		beq.s	?next_step			;
		bsr	PEG_off				;pitch env. off
	?next_step:
		move.b	(a5),d5
		andi.b	#%00000111,d5			; clear KYONB
		ori.b	#%00010000,d5			; set   KYONEX
		move.b	d5,(a5)

	?function_false:
		adda.w	#$0040,a4
		addi.w	#$0040,d4
		adda.w	#$0020,a5
		dbra	d7,?main_loop

	?end_of_damper_off:
		movem.l	(sp)+,a0/a1/a4/a5
		rts
	;-------------------------------------------------------;
	;	damper on					;
	;-------------------------------------------------------;
func_damper_on:
		bset.b	#damper,midi_flag(a6,d7.w)	;damper mode set(=on)
		rts					;
;-----------------------------------------------------------------------;
;	error process							;
;-----------------------------------------------------------------------;
CTRL_5B_er0:	bset.b	#ERRb_9,Mem_err_bit+2	; cannot find DSP-Prg-ID
		rts
CTRL_5B_er1:	bset.b	#ERRb10,Mem_err_bit+2	; not DSP-Prg down load
		rts
CTRL_5B_er2:	bset.b	#ERRb11,Mem_err_bit+2	; cannot find DSP-RAM-ID
		rts
CTRL_5B_er3:	bset.b	#ERRb12,Mem_err_bit+2	; DSP access area unit is 2000H
		rts
CTRL_5B_er4:	bset.b	#ERRb13,Mem_err_bit+2	; jump if Memory 不足
		rts
CTRL_5B_er5:	bset.b	#ERRb19,Mem_err_bit+1	; jump if R/W size > $20040
		rts
er_03:		bset.b	#ERRb22,Mem_err_bit+1	; Effect change No. > 0FH
		rts

;-----------------------------------------------------------------------;
;	MIDI CC91	(control change $5B:DSP program change)		;
;	--------------------------------------------------------	;
;	in:	a5	FH1005 top					;
;		a6	management work top				;
;		d1.w	MIDI ch# ( 0 ～ 1F )				;
;		d3.b	DSP program # ( 00 ～ 0F )			;
;-----------------------------------------------------------------------;
			.global		CTRL_5B
CTRL_5B:
		cmpi.b	#$10,d3				;effect # check
		bcc.s	er_03				;illegal effect #
		move.b	d3,d1				;
		ori.b	#DSP_PRG_ID*$10,d1		;d1.b=ID+#
		moveq	#DSP_RAM_ID*$10,d3		;d3.b=ID
	;-------------------------------------------------------;
	;	search same effect # module			;
	;-------------------------------------------------------;
		lea	bs_AMAPC,a0			;area map current work top
		moveq	#256/8-1,d7			;max map size
CTRL_5B_lp:	move.b	(a0),d0				;map end(=FF) check
		bmi	CTRL_5B_er0			;not found (dsp program)
		cmp.b	d0,d1				;id(dsp prg)& # check
		beq.s	CTRL_5B_4			;found!
		lea	8(a0),a0			;next id
		dbra	d7,CTRL_5B_lp			;
		bra	CTRL_5B_er0			;not found
	;-------------------------------------------------------;
	;	read program top address & size			;
	;-------------------------------------------------------;
CTRL_5B_4:	bclr.b	#ERRb_9,Mem_err_bit+2		;'not found' error bit clear
		move.l	(a0)+,d0			;read dsp program top address
		andi.l	#$0FFFFF,d0			;
		move.l	(a0),d2				;data load check(=bit7)
		bpl	CTRL_5B_er1			;dsp program unloaded
		bclr.b	#ERRb10,Mem_err_bit+2		;'unloaded' error bit clear
		andi.l	#$0FFFFF,d2			;program size
		movea.l	d0,a0				;a0=dsp program top address
		move.l	d0,DSP_PRG_top(a6)		;store work
	;-------------------------------------------------------;
	;	search same dsp work ram # module		;
	;-------------------------------------------------------;
		lea	bs_AMAPC,a1			;area map current work top
		moveq	#256/8-1,d7			;max map size
CTRL_5B_4lp:	move.b	(a1),d0				;map end(=FF) check
		bmi	CTRL_5B_er2			;not found (dsp work ram)
		bclr.b	#ERRb11,Mem_err_bit+2		;'not found' error bit clear
		andi.b	#$70,d0				;
		cmp.b	d0,d3				;id(dsp work ram) check
		beq.s	CTRL_5B_7			;found!
		lea	8(a1),a1			;next id
		dbra	d7,CTRL_5B_4lp			;
		bra	CTRL_5B_er2			;not found
	;-------------------------------------------------------;
	;	check dsp work ram top address & size		;
	;-------------------------------------------------------;
CTRL_5B_7:	move.l	(a1)+,d5			;read dsp work ram top address
		move.w	d5,d3				;
		andi.w	#$1FFF,d3			;top address check
		bne	CTRL_5B_er3			;illegal address
		bclr.b	#ERRb12,Mem_err_bit+2		;'illegal address' error bit clear
		andi.l	#$0FE000,d5			;
		move.l	(a1),d3				;read dsp work ram size
		andi.l	#$0FFFFF,d3			;
		cmpi.l	#$20040+1,d3			;max size check
		bcc.w	CTRL_5B_er5			;work size too large
		bclr.b	#ERRb19,Mem_err_bit+1		;'too large' error bit clear
	;-------------------------------------------------------;
	;	dsp work ram size check				;
	;		a0:dsp program top address		;
	;	 	d2:ram size(map)			;
	;	 	d5:dsp work ram top address		;
	;	 	d3:need ram size			;
	;	RBLEN:	    0,    1,    2,    3			;
	;	size:	04000,08000,10000,20000			;
	;-------------------------------------------------------;
		move.l	d5,DSP_RW_top(a6)		;($8850)
		move.l	d3,DSP_RW_sz(a6)		;($8854)
		move.b	DL_NEL(a0),DFL_ELMNT_NO(a6)	;(+$22)->($8830)
		move.l	#$4000,d1			; RBLen unit
		move.b	DL_RBL(a0),d0			;
		andi.w	#3,d0				;
		beq.s	CTRL_5B_2			;dsp work unused
		lsl.l	d0,d1				;d1=dsp work size
CTRL_5B_2:	addi.l	#$40,d1				;+$20x2(real time buffer area)
		moveq	#0,d4				;
		move.b	DL_NCT(a0),d4			;read coef table #
		beq.s	CTRL_5B_3			;no coef table
		mulu	#$A00,d4			;
CTRL_5B_3:	add.l	d4,d1				;+coef table size
		cmp.l	d1,d3				;
		bcs	CTRL_5B_er4			;work ram size is not enough
		bclr.b	#ERRb13,Mem_err_bit+2		;'not enough' error bit clear
		move.b	#8,EFCT_CHG_CNT(a6)		;set Effect change exe mode
		rts					;
;-----------------------------------------------------------------------;
;	MIDI Pitch Bend							;
;	--------------------------------------------------------	;
;	in:	a5	FH1005 top					;
;		a6	management work top				;
;		d1.w	MIDI ch# ( 0 ～ 0F )				;
;		d2.b	paramator LSB ( 00 ～ 7F )			;
;		d3.b	paramator MSB ( 00 ～ 7F )			;
;-----------------------------------------------------------------------;
			.global	MIDI_En
MIDI_En:	andi.w	#$7F,d2				; LSB
		andi.w	#$7F,d3				; MSB
		lsl.w	#7,d3				;
		or.w	d3,d2				; d2 = 00xx xxxx xxxx xxxx
		subi.w	#$2000,d2			; d2 = -$2000～0～$1FFF
	;------- read preset bend range ------------------------;
		move.w	d1,d4				;
		lsl.w	#5,d4				;midi channel work offset
		add.w	_tmp_kanri(a6),d4		;management work address
		move.b	program_number(a6,d4.w),d0	;read voice #
		movea.l	bank_address(a6,d4.w),a2	;read tone bank top address
		add.w	d0,d0				;
		move.w	BIN_VOICE(a2,d0.w),d0		;read voice data top address
		move.b	V_PM_BR(a2,d0.w),d0		;read bend range(0 - d)
		andi.w	#$0f,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;x4
		jmp	En_JPTB(pc,d0.w)		;
	;-------------------------------------------------------;
	;						;range	data
En_JPTB:	clr.w	d2				; 0	0
		bra.s	EN_0x
		asr.w	#5,d2				; 1	1/32
		bra.s	EN_0x
		asr.w	#4,d2				; 2	1/16
		bra.s	EN_0x
		asr.w	#4,d2				; 3	3/32
		bra.s	EN_00
		asr.w	#3,d2				; 4	1/8
		bra.s	EN_0x
		asr.w	#3,d2				; 5	5/32
		bra.s	EN_01
		asr.w	#3,d2				; 6	3/16
		bra.s	EN_00
		asr.w	#3,d2				; 7	7/32
		bra.s	EN_04
		asr.w	#2,d2				; 8	1/4
		bra.s	EN_0x
		asr.w	#2,d2				; 9	9/32
		bra.s	EN_05
		asr.w	#2,d2				;10	5/16
		bra.s	EN_01
		asr.w	#2,d2				;11	11/32
		bra.s	EN_06
		asr.w	#2,d2				;12	3/8
		bra.s	EN_00
		asr.w	#1,d2				;24	3/4
;@		bra.s	EN_00
	;-------------------------------------------------------;
EN_00:		move.w	d2,d3				;
		moveq	#1,d4				;(1/2)
		bra.s	EN_07				;
	;-------------------------------------------------------;
EN_06:		move.w	d2,d3				;
		asr.w	#2,d3				;
		bra.s	EN_02				;
	;-------------------------------------------------------;
EN_05:		move.w	d2,d3				;
		moveq	#3,d4				;(1/8)
		bra.s	EN_07				;
	;-------------------------------------------------------;
EN_01:		move.w	d2,d3				;
		moveq	#2,d4				;(1/4)
		bra.s	EN_07				;
	;-------------------------------------------------------;
EN_04:		move.w	d2,d3				;
		asr.w	#1,d3				;
	;-------------------------------------------------------;
EN_02:		add.w	d3,d2				;
		moveq	#1,d4				;(1/2)
	;-------------------------------------------------------;
EN_07:		asr.w	d4,d3				;1/2^n
		add.w	d3,d2				;
	;-------------------------------------------------------;
EN_0x:		move.w	_tmp_kanri(a6),d3		;management work
		lsl.w	#5,d1				;midi channel work offset
		add.w	d1,d3				;
		move.w	d2,knr_PBend_BF(a6,d3.w)	;bend data store
ret_80:		rts					;

;-----------------------------------------------------------------------;
;	MIDI CC80	(control change $50:Q sound position set)	;
;	--------------------------------------------------------	;
;	in:	a5	FH1005 top					;
;		a6	management work top				;
;		d1.w	mixer input # (as midi channel 0 ～ 08 )	;
;		d3.b	position ( 00 ～ 0F )				;
;-----------------------------------------------------------------------;
			.global	CTRL_50
CTRL_50:
		move.b	d1,d0				; channel
		move.b	d3,d1				; position
		bsr	xseq_qsound			;->sdmain.asm
		rts					;
;-----------------------------------------------------------------------;
;	MIDI CC81	(control change $51:Y sound distance set)	;
;	MIDI CC82	(control change $52:Y sound azimuth set)	;
;	MIDI CC83	(control change $53:Y sound elevation set)	;
;	--------------------------------------------------------	;
;	in:	a5	FH1005 top					;
;		a6	management work top				;
;		d1.w	midi channel # ( 0 ～ 0F:unused )		;
;		d3.b	position ( 00 ～ 7F )				;
;-----------------------------------------------------------------------;
			.global	CTRL_51,CTRL_52,CTRL_53
CTRL_51:
		lea.l	Y3D_buf,a0	;
		move.b	d3,d0		; d0=distance
		move.b	d0,(a0)		;
		move.b	1(a0),d1	; d1=azimuth
		move.b	2(a0),d2	; d2=elevation
		bsr	xseq_YMH3Dw	;->sdmain.asm
		rts			;
CTRL_52:
		lea.l	Y3D_buf,a0	;
		move.b	d3,d1		; d1=azimuth
		move.b	d1,1(a0)	;
		move.b	(a0),d0		; d0=distance
		move.b	2(a0),d2	; d2=elevation
		bsr	xseq_YMH3Dw	;->sdmain.asm
		rts			;
CTRL_53:
		lea.l	Y3D_buf,a0	;
		move.b	d3,d2		;
		move.b	d2,2(a0)	; d2=elevation
		move.b	(a0),d0		; d0=distance
		move.b	1(a0),d1	; d1=azimuth
		bsr	xseq_YMH3Dw	;->sdmain.asm
		rts			;
Y3D_buf:
		db	0
		db	0
		db	0

	end
;=======================================================================;
;	end of file							;
;=======================================================================;
