;=======================================================================;
;	SEGASATURN sound driver						;
; file name								;
;	non.asm								;
; copyright								;
;	SEGA ENTERPRISES						;
; purpose								;
;	MIDI note on control						;
; author(s)								;
;	1995-11-14	A.Miyazawa	Ver.2.00			;
; mod history								;
;	1996-02-23	A.Miyazawa	Ver.2.06			;
;	1996-07-02	A.Miyazawa	Ver.2.10L			;
;	1997-06-30	Y.Kashima	Ver.2.20			;
;=======================================================================;

		.include	scsp.lib

			.globals	on

			.extern	OCT_TB			;
			.extern	FNSTB			;
			.extern	Note_off		;
			.extern	get_LEVEL		;table
			.extern	EXPTB			;table
			.extern	send_user		;
			.extern	old_slot_counter	;

amount_of_layer		.equ		DVA_lyr_cnt
RLEASE_TIME_COUNT	.equ		$1000
;-----------------------------------------------------------------------;
;	in	d1.w:MIDI 1st. byte,track number			;
;		d2.b:MIDI 2nd. byte,note number				;
;		d3.b:MIDI 3rd. byte,velocity				;
;-----------------------------------------------------------------------;
Note_on:	tst.w	d3				; velocity check : 0 ～ 7FH
		beq	Note_off			; jump if Velo = 0
		lea.l	note_on_data(pc),a0		;
		move.b	d1,midi_track_note+0(a0)	;
		move.b	d2,midi_track_note+1(a0)	;
		move.w	d3,midi_velocity(a0)		;
	.if sw_MODEL_M
		bsr	set_monitor			;
	.endif			;sw_MODEL_M
		moveq	#0,d0				;
		move.w	knr_kanri_ofst(a6),d6		;
		move.b	program_number(a6,d6.w),d0	; d0 = MIDI program number.
search_dynamic_filter:
		lea	_DFL_ELMNT_wk(a6),a2		;
		clr.w	d6				;
		move.b	DFL_ELMNT_NO(a6),d6		;element #
		beq.s	layer_pick_up			;
	;-------------------------------------------------------;
		movea.l	DFL_ELMNT_addr(a6),a1		;element data table top
		subq.w	#1,d6				;loop size
		andi.w	#$1F,d6				;
?elementLoop:	cmp.b	(a1),d1				;same midi ch # ?
		bne.s	?200				;
		move.b	1(a1),DFL_wk_MSFC(a2)		;
		move.l	a1,DFL_dst_addr(a2)		;
		sf.b	DFL_EG_seg(a2)			; Segment# clear
		move.b	$11(a1),DFL_wk_AMP(a2)		;
		clr.w	d5				;
		move.b	$10(a1),d5			; = [FRQR]
		lsl.w	#4,d5				;
		move.w	d5,DFL_add_bs(a2)		;
		add.w	d5,d5				;
		add.w	d5,DFL_add_bs(a2)		; [FRQR]×48 --> DFL_add_bs
		move.w	#0,DFL_add_wk(a2)		; LFO reset
?200:		lea	$18(a1),a1			;next element data table top
		lea	$10(a2),a2			;next element work top
		dbra	d6,?elementLoop			;
;-----------------------------------------------------------------------;
;	pick up layer and write to 'DVA_layer'
;-------------------------------------------------------
;	in  d0.l = voice number
;	in  d1.w = MIDI channel
;	in  d2.b = MIDI note note number
;	out d6.w = layer数
;	out a2   = desti.Voice address
;	break    = a1/a2/d0/d6/d7
;-----------------------------------------------------------------------;
layer_pick_up:	move.w	knr_kanri_ofst(a6),d5
		move.l	bank_address(a6,d5.w),d6	; d6 = SCSPBIN top address
		beq	without_bank_change		; error!
		movea.l	d6,a2
		add.w	d0,d0				; = Voice# * 2
		adda.w	BIN_VOICE(a2,d0.w),a2		; BIN_VOICE = 8
		lea	_DVA_layer(a6),a1		; ready
		moveq	#0,d5				; increment counter clear
		moveq	#0,d6				; increment counter clear
		move.b	V_layer_sz(a2),d7		;layer#-1
		bmi	layer_nothing			;<layer not found>
		andi.w	#$7f,d7				;layer max#=127
		moveq	#V_Layer,d0			; ready
?layer_loop:	cmp.b	LY_SNT(a2,d0.w),d2		; start note / MIDI note
		bcs.s	?100				; jp if MIDI note < start note
		cmp.b	LY_ENT(a2,d0.w),d2		; end note / MIDI note
		bhi.s	?100				; jp if end note < MIDI note
		move.l	a2,(a1)
		add.l	d0,(a1)+			; Layer addr --> (DVALY_addr)
		move.l	d5,(a1)+			; Layer# ( DVALY_NO )
		addq.w	#1,d6				; increment counter
		cmpi.w	#slot_size+1,d6			; 同時発音は 32ヶまで
		bcc	er_24				; jump if 32 < d6
?100:		addq.w	#1,d5				; increment counter
		addi.w	#LY_unit,d0			; next Key sprit data addr
		dbra	d7,?layer_loop			; d6.w : key sprit 有効Layer数
		move.w	d6,amount_of_layer(a6)		; 0(error) or 1～32
		move.w	d6,DVA_lyr_cntx(a6)		; 0(error) or 1～32
		beq	selected_layer_not_found	; error! layer not found.

	;-------------------------------------------------------;
	;	Play mode check!				;
	;-------------------------------------------------------;
		move.w	knr_kanri_ofst(a6),d5
		move.b	knr_MONO_NT1(a6,d5.w),knr_MONO_NT2(a6,d5.w)
		move.b	knr_MONO_NT0(a6,d5.w),knr_MONO_NT1(a6,d5.w)
		move.b	d2,knr_MONO_NT0(a6,d5.w)	; d2 = note number
		move.b	d3,knr_MONO_VOL(a6,d5.w)	; d3 = velocity
		movea.l	bank_address(a6,d5.w),a3	;tone bank top address
		move.l	a3,(scspbin_top_address)	;tone bank top address
		move.b	V_vol_bias(a2),volume_bias(a6,d5.w)	; Volume bias(from layer)
		move.b	(a2),d0				; Play mode & Bend Range(from layer)
		andi.b	#%01110000,d0			; Play "MONO" mode ?
		move.b	d0,PM_flag(a6)
		bne	play_mono_mode			; error!

	;-----------------------------------------------------------------------;
	; 発音スロットのアサインとキーオン（高速化のためＦＭと
	; ＰＣＭを別ルーチンにした）
	;-------------------------------------------------------
	;	in  a6 = ワークRAM
	;	in  d5 = 発音管理番号オフセットワークアドレス
	;		(0x40*management number)にMIDIチャネル
	;		オフセットワークアドレス(0x20*MIDI track
	;		 number)を足したもの.
	;-----------------------------------------------------------------------;

		lea.l	old_slot_counter(pc),a2
		addq.w	#1,(a2)

		move.w	amount_of_layer(a6),d7		; レイヤ数をループカウンタとする.
		lea	_DVA_layer(a6),a3
		movea.l	(a3),a2
		btst.b	#7,LY_base_note(a2)		; 今回からここのビット7で行う。*註１
		beq	pcm_control

; *註１
; LY_MDL はＦＭ変調制御レジスタへの出力値が格納されているアドレスを示すアサインで、
; scspbin（Ａ２レジスタ）からのオフセットである。
; 2.10以前はここの値が０になっているときＰＣＭと判別していたが、
; 実際にはモジュレータとして使用するＦＭスロットには値が０になっているものも存在する。
; したがって、ここで判定するのは大変まずい。
; とはいうものの、ほかに判断の基準となるデータをscspbinは持っていないので、
; バンクチェンジ時にスロットの結線をシミュレートし、
; たとえLY_MDLが０でもＦＭ結線される予定があれば、LY_base_noteのビット7を立てる。
; 2.10からはこのビットが立っていればＦＭ、さもなければＰＣＭと判断している。


;-----------------------------------------------------------------------;
; FM音源 note on
;-------------------------------------------------------
;	in  d7 = レイヤ数.ループカウンタ兼用.
;-----------------------------------------------------------------------;
fm_search:

; ＦＭ、ＰＣＭストリーム、エンジンとして発音中のスロットにはアサインされない。
; 使用可能なスロットがレイヤ数分確保できるかをチェックする。
; 確保できたら set_any_data へ。

		lea	slot_work(a6),a4
		move.w	#slot_size-1,d6
		moveq	#0,d4
		moveq	#0,d5
		
?slot_loop________:
		move.w	slot_status(a4),d0
		andi.w	#$ffc8,d0			; ちょっとゆるくした.
		bne.s	?function_false
		
		addq.w	#1,d5				; レイヤ数を数えるカウンタを＋１
		move.w	d4,DVALY_slofst(a3)		; 該当スロット番号をpush！
		addq.w	#8,a3				; 次のレイヤアドレスへ
		cmp.w	d5,d7				;レイヤ数分確保できたら本格的キーオンルーチンへ
		beq	set_any_data
		
?function_false:
		addi.w	#$0040,d4
		adda.w	#$0040,a4
		dbra	d6,?slot_loop________
		
		lea	slot_work(a6),a4		;
		move.b	#TOO_MANY_FM_CHANNELS,d0	;
		bsr	send_user			;
		rts					;no blank slot

	;-------------------------------------------------------;
set_any_data:
		lea	slot_work(a6),a4
		lea	_DVA_layer(a6),a3
?layer_loop:	movem.l	a4/a5,-(sp)
		move.w	DVALY_slofst(a3),d4
		
		adda.w	d4,a4
		
		lsr.w	#1,d4
		adda.w	d4,a5
		add.w	d4,d4
		
		ori.w	#$ff08,slot_status(a4)		; ノートオン,FMビットを立てる.
		move.w	#RLEASE_TIME_COUNT,release_counter(a4)
		
		bsr	PP_sub
		ori.b	#$1f,SCSP_FH_RR(a5)
		
		movem.l	(sp)+,a4/a5
		addq.w	#8,a3
		subq.w	#1,d7
		bne.s	?layer_loop
		
fm_connect:
		move.w	amount_of_layer(a6),d7		; レイヤ数をループカウンタとする.
		lea	_DVA_layer(a6),a3
		
?layer_loop:	movea.l	(a3),a2				; Carria layer addr(DVALY_addr)
		move.w	DVALY_slofst(a3),d4
		move.w	d4,d5
		lsr.w	#1,d5
		
		move.w	LY_MDL(a2),d0			; Carria [MDL] = 0 ?
		move.b	LY_FM+0(a2),d1			; Module GN & FMLY
		
		bsr.s	FM_SUB				; break d1/d2/d3/a0
		
		or.w	d3,d0				; d0 : mmmm xxxx xx00 0000
		move.b	LY_FM+1(a2),d1			; Module GN & FMLY
		
		bsr.s	FM_SUB				; break d1/d2/d3/a0
		
		lsr.w	#6,d3				; d3 : 0000 0000 00yy yyyy
		or.w	d3,d0				; d0 : mmmm xxxx xxyy yyyy
		move.w	d0,SCSP_MDLSL(a5,d5.w)		; write [MDL],[MDXSL],[MDYSL]

		move.b	(a5,d5.w),d0
		andi.b	#%00000111,d0			; clear KYONEX
		ori.b	#%00011000,d0			; set   KYONB ( key-on 準備 )
		move.b	d0,(a5,d5.w)			; write KYONEX & KYONB

		addq.w	#8,a3
		subq.w	#1,d7
		bne.s	?layer_loop
		
		move.b	(a5),d0	
		ori.b	#%00010000,d0			; set KYONEX
		move.b	d0,(a5)				; write KYONEX & KYONB
		
		rts

FM_SUB:
		move.b	d1,d2
		andi.b	#$7F,d1				; Voice内Layer# ( 0,1,2,.. )
		lea	_DVA_layer(a6),a0
?gsny_lp:	cmp.b	DVALY_NO(a0),d1			; Layer# equal ?
		beq.s	?gsny_2				; jump if equal
		addq	#DVALY_unit,a0
		bra.s	?gsny_lp
?gsny_2:	move.w	DVALY_slofst(a0),d3		; $00,$40,...,$7C0
		sub.w	d4,d3				; Module - Carria
		andi.w	#$7c0,d3
		cmpi.w	#$700,d3
		bcs.s	?get_mdy_1
		addi.w	#$800,d3
?get_mdy_1:	move.b	d2,d2				; Generation H or L ?
		bpl.s	?get_mdy_2			; jump if 最新Sample
		addi.w	#$800,d3
?get_mdy_2:	andi.w	#$FC0,d3
		rts

;-----------------------------------------------------------------------;
; PCM NOTE ON
; アサインされたレイヤ数分このルーチンを繰り返してしまうが、
; 実際にはこの方が効率がよかったりする。
;-----------------------------------------------------------------------;
pcm_control:	lea	slot_work(a6),a4
		move.w	#slot_size-1,d6
		moveq	#0,d4
		moveq	#-1,d1
		moveq	#-1,d2
		move.w	(note_on_data+midi_track_note),d5
		move.w	_tmp_kanri(a6),d3
?slot_loop________:

		move.w	slot_status(a4),d0
		beq	slot_open			; 完全にフリーなスロットを発見

		andi.w	#$ffcc,d0			;
		bne.s	?next_step0			;
		move.w	d4,d1				;
?next_step0:	cmp.w	_sl_kanri(a4),d3		;management # check
		bne.s	?function_false			;
		andi.w	#$ffc0,d0			;FM/stream/engine check
		bne.s	?function_false			;
		move.w	d4,d2				; for step 4
		move.w	slot_status(a4),d0		;
		andi.w	#$ffc8,d0			;note off check
		bne.s	?function_false			;
		cmp.w	sl_MIDI(a4),d5			;midi ch # check
		bne.s	?next_step1			;
		swap	d1				;
		move.w	d4,d1				;
		swap	d1				;
?next_step1:	swap	d2				;
		move.w	d4,d2				;
		swap	d2				;
?function_false:
		addi.w	#$0040,d4
		adda.w	#$0040,a4
		dbra	d6,?slot_loop________

		move.w	d1,d4
		bpl.s	slot_open
		swap	d1
		move.w	d1,d4
		bpl.s	slot_open
		swap	d2
		move.w	d2,d4
		bpl.s	slot_open
		swap	d2
		move.w	d2,d4
		bmi.s	slot_search_step5
		bra.s	slot_open

;-----------------------------------------------------------------------;
; もっとも過去にキーオンされたスロットを発音管理番号とは
; 無関係に探し出し,強制的に割り当てる.
; このルーチンを通過することは,最大同時発音数を超えてい
; ることになるため,基本的にはエラーである.
; ここを通過するたびにワークエリア0x000007f2の１ワードが
; インクリメントされる.
;-----------------------------------------------------------------------;
		move.b	#VOICE_FULL,d0
		bsr	send_user
slot_search_step5:
		addq.w	#1,checker_work
		lea.l	slot_work(a6),a4
		moveq	#slot_size-1,d6
		moveq	#0,d0
		moveq	#0,d1
		moveq	#0,d2
		moveq	#0,d4
		move.w	slot_age(a4),d1
		moveq	#0,d3
		cmpi.w	#$1000,d1
		bls.s	?flow_adjust
		cmpi.w	#$f000,d1
		bcc.s	?flow_adjust
		bra.s	?slot_loop________
?flow_adjust:	move.w	#$8000,d3
		add.w	d3,d1
	?slot_loop________:
		move.w	slot_age(a4,d2.w),d0
		add.w	d3,d0
		cmp.w	d1,d0
		bcs.s	?search_next
		move.w	d0,d1				; d1 = 現時点の最古値
		move.w	slot_status(a4,d2.w),d0
		andi.w	#$ffc0,d0
		bne.s	?search_next
		move.w	d2,d4
		bra.s	slot_open
?search_next:	addi.w	#$0040,d2
		dbra	d6,?slot_loop________
		move.b	#KEY_ON_CANCEL,d0
		bsr	send_user
		rts

;-----------------------------------------------------------------------;
; 検出されたスロット番号からキーオンをセットする.
;	--------------------------------------------------------	;
;	in	a4	該当スロットのワークアドレス
;		d7	レイヤ数.ループカウンタ兼用.壊したらだめ.
;-----------------------------------------------------------------------;
slot_open:	lea	slot_work(a6),a4		;
		adda.w	d4,a4				;
		movea.l	#IO_SCSP,a5			;
		lsr.w	#1,d4				;
		adda.w	d4,a5				;
		add.w	d4,d4				;
		ori.b	#$1f,SCSP_FH_RR(a5)		;
		ori.w	#$0008,slot_status(a4)		; ノートオンビットを立てる.
		move.w	#RLEASE_TIME_COUNT,release_counter(a4)
		bsr	PP_sub				;
		move.b	(a5),d0				;
		andi.b	#%00000111,d0			;
		ori.b	#%00011000,d0			;key on execute
		move.b	d0,(a5)				;set key on to scsp
		addq.w	#8,a3				;next layer
		subq.w	#1,d7				;
		beq.s	end_of_slot_search		;
		bra	pcm_control			;
	;-------------------------------------------------------;
end_of_slot_search:
		movea.l	#IO_SCSP,a5			;
		lea	slot_work(a6),a4		;
		move.b	(a5),d0				;
		ori.b	#%00010000,d0			; set KYONEX
		move.b	d0,(a5)				; write KYONEX & KYONB
		rts					;

;-----------------------------------------------------------------------;
;	Write to SCSP
;-----------------------------------------------------------------------;

PP_sub:		move.w	(old_slot_counter),d0
		move.w	d0,slot_age(a4)

;-----------------------------------------------------------------------;
; note-on のための key-off 実行
;-----------------------------------------------------------------------;
		move.b	(a5),d5
		andi.b	#%00000111,d5
		ori.b	#%00010000,d5
non_koff:	move.b	d5,(a5)

;-----------------------------------------------------------------------;
; set slot work data
;-----------------------------------------------------------------------;
		move.w	_tmp_kanri(a6),_sl_kanri(a4)
		move.w	(note_on_data+midi_track_note),d1
		move.w	(note_on_data+midi_velocity),d3
		move.w	d1,sl_MIDI(a4)
		move.b	d3,sl_velo(a4)

;-----------------------------------------------------------------------;
; Key-off の実行
; Key-on の準備
; 波形絶対アドレスの算出＆設定
;	  +--+--+--+--+--+-----+-----+-----+--+-----------+
;	00|--|--|--|KX|KB|SBCTL|SSCTL|LPCTL|8B|    S A    |
;	  +--+--+--+--+--+-----+-----+-----+--+-----------+
;	02|                   S A                         |
;	  +-----------+-----------------+-----------------+
;	0E|    MDL    |      MDXSL      |      MDYSL      |
;	  +-----------+-----------------+-----------------+
;	04|                   LSA                         |
;	  +-----------------------------------------------+
;	06|                   LEA                         |
;	  +--------------+--------------+--+--------------+
;	08|        D2R   |     D1R      |HO|      AR      |
;	  +--------------+--+-----------+--+--------------+
;	0A|  |LS|    KRS    |      DL      |      RR      |
;	  +--+--+--+--+--+--+--+--+--------+--------------+
;	0C|--|--|--|--|--|--|SI|SD|-----------------------|
;	  +--+--+--+--+--+--+--+--+-----------------------+
;	  +--+--------------+-----+--------+-----+--------+
;	12|RE|     LFOF     |PLFOW|  PLFOS |ALFOW| ALFOS  |
;	  +--+--+--+--+--+--+--+--+--+-----+-----+--------+
;	14|--|--|--|--|--|--|--|--|--|   ISEL    |  IMXL  |
;	  +--+--+--+--+--+--+--+--+--+-----------+--------+
;-----------------------------------------------------------------------;

		movea.l	(a3),a2				; desti. Layer addr(DVALY_addr)
		move.w	LY_MDL(a2),d6			; [MDL]
		move.l	a2,sl_layer_adr(a4)		;
		move.l	(scspbin_top_address),d5	;
		add.l	LY_SA(a2),d5			; = [PEON] ～ [SA]
		move.l	d5,(a5)				;
		move.w	d6,SCSP_MDLSL(a5)		;
		move.l	LY_LSA(a2),SCSP_PCM_LSA(a5)	;
		move.l	LY_D2R(a2),SCSP_D2R1R(a5)	;
		move.b	LY_SISD(a2),d6			;
		move.w	LY_RELFO(a2),d5			;
		move.b	d6,SCSP_SISD(a5)		;
		bpl.s	?jump_if_Hardware_modulation_wheel_on
		andi.w	#$ff18,d5			;
?jump_if_Hardware_modulation_wheel_on:
		move.w	d5,SCSP_RELFO(a5)		;
		move.w	LY_ISEL(a2),SCSP_ISEL(a5)	;


;-----------------------------------------------------------------------;
; パンポットの設定
;-----------------------------------------------------------------------;
PP_sub_PAN:	move.b	LY_DSDPN(a2),d6			; d6 = Layer [DISDL] & [DIPAN]
		move.b	SND_OUT_ST(a6),d5		; MONO/STEREO status
		bpl.s	?stereo				; jump if STEREO mode
		andi.b	#$E0,d6				; --> Center
		bra.s	?end_of_function		;
?stereo:	move.w	knr_kanri_ofst(a6),d5		;
		move.b	midi_panpot(a6,d5.w),d5		;
		btst	#6,d5				; SEQ PAN on/off ?
		bne.s	?sequence_pan_on		;
		move.b	d5,d5				;
		bpl.s	?end_of_function		; jump if Layer PAN on
?sequence_pan_on:
		andi.b	#$1F,d5				; = PAN buffer
		andi.b	#$E0,d6				;
		or.b	d5,d6				;
?end_of_function:
		cmp.b	SCSP_DISDLPN(a5),d6		;
		beq.s	?end				;
		move.b	d6,SCSP_DISDLPN(a5)		;
?end:		move.b	d6,sl_DISDLPAN(a4)		;

	;-------------------------------------------------------;
	; ＴＬ算出
	;-------------------------------------------------------;
wr_vol:		clr.w	d6				;
		btst.b	#FMCB,LY_SA(a2)			;
		bne.s	?function_false			; carrier
		move.b	LY_ISLMX(a2),d6			; = [ISEL]&[IMXL]
		andi.b	#7,d6				; = [IMXL] only ( Effect send )
		bne.s	?function_false			; carrier
		move.b	LY_DSDPN(a2),d6			; = [DISDL]&[DIPAN] 
		andi.b	#$E0,d6				; = [DISDL] only ( Direct send )
		bne.s	?function_false			; carrier
		move.b	LY_TL(a2),d6			; = [TL] only
		bclr.b	#flg_FMCR,sl_flag2(a4)		;
		bra	volume_write			;
?function_false:

;-----------------------------------------------------------------------;
; Ｖelocity - Ｌevel 変換
;-----------------------------------------------------------------------;

velocity_level_change:
		bset.b	#flg_FMCR,sl_flag2(a4)		;
		move.b	sl_velo(a4),d6			; $00 ～ $7F(max) : MIDI velo
		movea.l	(scspbin_top_address),a1	;
		adda.w	BIN_VL(a1),a1			;
		moveq	#0,d5				;
		move.b	LY_VLNO(a2),d5			; = VL変換番号
		add.w	d5,d5				;
		adda.l	d5,a1				;
		add.w	d5,d5				;
		add.w	d5,d5				;
		adda.l	d5,a1				; = desti. VL data top
		moveq	#0,d1				;
		moveq	#0,d2				;
		move.b	(a1)+,d3			; = K0 initial
		cmp.b	(a1),d6				; (a1)=V0 / MIDI Velo
		bls.s	?100				; jump if 1st Seg
		move.b	(a1)+,d1			; = V0
		move.b	(a1)+,d2			; = L0
		move.b	(a1)+,d3			; = K1
		cmp.b	(a1),d6				; (a1)=V0
		bls.s	?100				; jump if 2nd Seg
		move.b	(a1)+,d1			; = V1
		move.b	(a1)+,d2			; = L1
		move.b	(a1)+,d3			; = K2
		cmp.b	(a1),d6				; (a1)=V0
		bls.s	?100				; jump if 3rd Seg
		move.b	(a1)+,d1			; = V2
		move.b	(a1)+,d2			; = L2
		move.b	(a1)+,d3			; = K3
?100:		sub.b	d1,d6				; = δＶ
		move.b	d3,d1				;
		andi.w	#7,d1				;
		add.w	d1,d1				;
		lsr.b	#4,d3				;
		jmp	?jump_table(pc,d1.w)		;
	;-------------------------------------------------------;
?jump_table:	bra.s	?velocity_0			;
		bra.s	?velocity_1			;
		bra.s	?velocity_2			;
		bra.s	?velocity_3			;
		bra.s	?velocity_4			;
		bra.s	?velocity_5			;
		bra.s	?velocity_6			;
		bra.s	?velocity_7			;
	;-------------------------------------------------------;
?velocity_4:
?velocity_0:	moveq	#0,d6				;
?velocity_2:	add.b	d2,d6				;
		bra.s	?velocity_exit			;
?velocity_6:	sub.b	d2,d6				;
		neg.b	d6				;
		bra.s	?velocity_exit			;
?velocity_1:	bcs.s	?200				;
		lsl.b	d3,d6				;
		add.b	d2,d6				;
		bra.s	?velocity_exit			;
?200:		andi.w	#$7f,d6				;
		lsl.w	#2,d6				;
		move.w	d6,d1				;
		add.w	d1,d6				;
		add.w	d1,d6				;
		lsl.w	d3,d6				;
		lsr.w	#3,d6				;
		add.b	d2,d6				;
		bra.s	?velocity_exit			;
?velocity_7:	bcs.s	?300				;
		lsl.b	d3,d6				;
		bra.s	?velocity_5+2			;
?300:		andi.w	#$7f,d6				;
		lsl.w	#2,d6				;
		move.w	d6,d1				;
		add.w	d1,d6				;
		add.w	d1,d6				;
		lsl.w	d3,d6				;
		lsr.w	#3,d6				;
		bra.s	?velocity_5+2			;
?velocity_3:	lsr.b	d3,d6				;
		add.b	d2,d6				;
		bra.s	?velocity_exit			;
?velocity_5:	lsr.b	d3,d6				;
		sub.b	d2,d6				;
		neg.b	d6				;
?velocity_exit:	bpl.s	?velocity_8			;
		moveq	#0,d2				;
		add.b	d6,d6				;
		bmi.s	?velocity_9			;
		moveq	#$7f,d2				;
?velocity_9:	move.w	d2,d6				;
?velocity_8:

;-----------------------------------------------------------------------;
; d6 = (MIDI velo)*(V-L) ＶＬ変換データ ( $00～$7F )
;-----------------------------------------------------------------------;
VL_00:		move.w	knr_kanri_ofst(a6),d1		;
		andi.w	#$007f,d6			;
		bne.s	?layer_total_level		;
		addq.w	#1,d6				; adjust d6, because "velocity=0" is error.
?layer_total_level:
		moveq	#0,d5				;
		move.b	LY_TL(a2),d5			; 0x00ff _ 0x0000
		not.b	d5				; 0x0000 _ 0x00ff
		move.b	volume_bias(a6,d1.w),d3		;
		beq.s	?layer_level			;
		ext.w	d3				;
		add.w	d3,d5				;
		bmi	bias_overflow_underflow		;
		cmpi.w	#$0100,d5			;
		bcc	bias_overflow_underflow		;
?layer_level:	mulu	d5,d6				; 0x0000 _ 0x7e81
		lsr.w	#8,d6				; if odd then XF=1
		move.w	d6,slot_velocity(a4)		;
volume_calc_step2:
?midi_level:	move.w	midi_master_volume(a6,d1.w),d5	;
		mulu	d5,d6				; 0x0000 _ 0x3f01
		lsr.w	#6,d6				; 0x0000 _ 0x007e
		move.w	d6,total_volume(a6,d1.w)	;
?sequence_volume_add:
		movea.l	management(a6,d1.w),a1		;
		move.w	sequence_volume(a1),d5		;
		eori.w	#$0080,d5			;
		beq.s	?result				;
		ext.w	d5				;
		add.w	d5,d6				;
		bmi.s	?overflow_underflow		;
		cmpi.w	#$0100,d6			;
		bcc.s	?overflow_underflow		;
		bra.s	?result				;
?overflow_underflow:
		tst.w	d6				;
		spl.b	d6				;
?result:	not.b	d6				;

;-----------------------------------------------------------------------;
; ＴＬ設定
;-----------------------------------------------------------------------;
volume_write:
		cmp.b	SCSP_TLVL(a5),d6
		beq.s	?end

		move.b	d6,SCSP_TLVL(a5)		; [TL]
	?end:

;-----------------------------------------------------------------------;
; 音程書込み
;-----------------------------------------------------------------------;
PLFO_ON:	bclr.b	#PLON_flg,sl_flag1(a4)		;
		btst.b	#PLON,LY_SA(a2)			; PLFO on ?
		beq.s	PEG_ON				; jump if off
		bsr	PLFO_init			; <d0/d1/a1>
PEG_ON:		moveq	#0,d5				;
		bclr.b	#PEON_flg,sl_flag1(a4)		;
		btst.b	#PEON,LY_SA(a2)			; PLFO on ?
		beq.s	PEG_ON_e			; jump if off
		bsr	PEG_init			; return d5.w:音程
PEG_ON_e:	moveq	#0,d0				;
		move.b	sl_MIDI(a4),d0			;
		lsl.w	#5,d0				;
		or.w	_sl_kanri(a4),d0		; + 発音管理番号×200H
		add.w	knr_PBend_BF(a6,d0.w),d5	; = Pitch Bend data
		move.w	d5,sl_tune(a4)			;
		bsr	send_frequency			; ready d5.w:音程
	;-------------------------------------------------------;
	; [LFORES] 解除						;
	;-------------------------------------------------------;
		bclr.b	#7,SCSP_RELFO(a5)		; [LFORES]解除
		rts					

;-----------------------------------------------------------------------;
;	set [OCT/FNS] to scsp						;
;	in	d5
;	out	
;	break	d2,d6
;-----------------------------------------------------------------------;
send_frequency:	moveq	#0,d6				;
		moveq	#0,d2				;
		move.b	LY_fine_tune(a2),d6		;layer fine tune
		ext.w	d6				;
		add.w	d6,d5				;
		move.w	d5,d6				; d5.b = セント量
		lsr.w	#8,d6				; d6.b = 音程
		move.b	sl_note(a4),d2			;
		add.w	d2,d6				; + MIDI note#
		addi.w	#0096,d6			; + 12*8 ( 8octav )
		move.b	LY_base_note(a2),d2		;
		andi.w	#$007f,d2			;
		sub.w	d2,d6				; - base note
		andi.w	#$00ff,d6			; = 音程
	;------- octave separate -------------------------------;
		lea	OCT_TB(pc),a2			;
		move.b	(a2,d6.w),d6			;
		lsl.w	#7,d6				;0ooo osss s000 0000
		move.w	d6,d2				;
		andi.w	#$7800,d6			;[OCT]
		add.w	d2,d2				;
		move.b	d5,d2				;scale & detune
		andi.w	#$0ffe,d2			;0000 ssss dddd ddd0
		lsr.w	#1,d2				;0000 0sss sddd dddd
		lea	FNSTB(pc),a2			;
		moveq	#0,d5				;
		move.b	(a2,d2.w),d5			;[FNS] low byte
	;------- FNS high byte add -----------------------------;
		cmpi.w	#$3de/2,d2			;
		bcs.s	?SD_tune_1			;$000 - $3DC:cent +$000
		cmpi.w	#$706/2,d2			;
		bcs.s	?SD_tune_2			;$3DE - $704:cent +$100
		cmpi.w	#$9b2/2,d2			;
		bcs.s	?SD_tune_3			;$706 - $9B0:cent +$200
		addi.w	#$100,d5			;$9B2 - $BFE:cent +$300
?SD_tune_3:	addi.w	#$100,d5			;
?SD_tune_2:	addi.w	#$100,d5			;
	;------- set frequency data to scsp --------------------;
?SD_tune_1:	or.w	d5,d6				;[OCT]+[FNS]
		cmp.w	SCSP_OCTFNS(a5),d6		;same?
		beq.s	?end				;
		move.w	d6,SCSP_OCTFNS(a5)		;
?end:		rts					;

;-----------------------------------------------------------------------;
;	pitch lfo control (key on)					;
;	in	
;	out	
;	break	a0,a1,d6
;-----------------------------------------------------------------------;
PLFO_init:	lea	EXPTB(pc),a0			;unsigned exponential table
		moveq	#0,d6				;
		move.l	d6,PLFO_cent(a4)		;detune level init
		movea.l	(scspbin_top_address),a1	;tone bank top address
		move.w	BIN_PLFO(a1),d6			;
		adda.l	d6,a1				;pitch lfo data block top
		moveq	#0,d2				;
		move.b	LY_PLFO_NO(a2),d2		;pitch lfo #n
		add.w	d2,d2				;
		add.w	d2,d2				;
		adda.w	d2,a1				;+ (pitch lfo #n) x 4
	;------- 
		moveq	#0,d0				;
		move.b	(a1)+,d0			;[PLFO DLY]
		add.w	d0,d0				;
		move.w	(a0,d0.w),d0			;[PLFO DLY]^2
		lsr.w	#2,d0				;
		move.w	d0,PLFO_Delay(a4)		; = delay counter
	;------- 
		moveq	#0,d0				;
		move.b	(a1)+,d0			;[FRQR]
		add.w	d0,d0				;
		move.w	(a0,d0.w),d6			;[FRQR]^2
		swap	d6				; XX XX ?? ??
		clr.w	d6				; XX XX 00 00
		lsr.l	#8,d6				; 00 XX XX 00 (1msec)
		move.l	d6,PLFO_FRQR_bs(a4)		;
	;------- 
		moveq	#0,d0				;
		move.b	(a1)+,d0			;[HT]
		add.w	d0,d0				;
		move.w	(a0,d0.w),d2			;[HT]^2
		lsr.w	#4,d2				; 0X XX (1msec)
		move.w	d2,PLFO_HTCNT_bs(a4)		;
	;------- 
		moveq	#0,d0				;
		move.b	(a1),d0				;[FDCNT]
		add.w	d0,d0				;
		move.w	(a0,d0.w),d0			;[FDCNT]^2
		lsr.w	#6,d0				; 0,1,2,3,4,....3FF
		bne.w	?plfo_init1			; 	FDCNT=[FDCT]^2/40H
		lsr.w	#1,d2				; HTCNT/2 (1/4周期分)
		bra.w	?plfo_init2			;
?plfo_init1:	add.w	d0,d0				;
		subq.w	#1,d0				; = フェイド期間の符号反転回数
?plfo_init2:	move.w	d2,PLFO_HTCNT_wk(a4)		;
		move.w	d0,PLFO_FDCNT(a4)		;
		move.l	PLFO_FRQR_bs(a4),PLFO_FRQR_wk(a4)
		bset.b	#PLON_flg,sl_flag1(a4)		;
		rts					;

;-----------------------------------------------------------------------;
;	pitch envelope control (key on)					;
;-----------------------------------------------------------------------;
PEG_init:	move.b	#1,PEG_SEG(a4)			;pitch env. segment init.
		moveq	#0,d6				;
		movea.l	(scspbin_top_address),a1	;tone bank top address
		move.w	BIN_PEG(a1),d6			;
		adda.l	d6,a1				;pitch env data block top adrs
		moveq	#0,d2				;
		move.b	LY_PEG_NO(a2),d2		;pitch env #n
		add.w	d2,d2				;
		adda.w	d2,a1				;
		add.w	d2,d2				;
		add.w	d2,d2				;
		adda.w	d2,a1				;+ (pitch env #n) x 10
		move.l	a1,PEG_addr(a4)			;pitch env data #n top adrs
	;------- pitch envelope delay --------------------------;
		moveq	#0,d0				;
		move.b	(a1)+,d0			;[PEG DLY]
		add.w	d0,d0				;
		lea	EXPTB(pc),a0			;unsigned exponential table
		move.w	(a0,d0.w),d0			;
		lsr.w	#2,d0				; = delay counter (1msec)
		move.w	d0,PEG_dly_cnt(a4)		; = delay counter (1msec)
	;-------------------------------------------------------;
		moveq	#0,d0				;
		move.l	d0,PEG_level(a4)		;detune level(cent)
		move.l	d0,PEG_RATE(a4)			;rate
	;------- 
		move.b	(a1)+,d0			;[OL]
		bsr	get_LEVEL			;get pitch env detune level
		move.w	d0,d5				;copy to d5
		swap	d0				;
		clr.w	d0				;
		move.l	d0,PEG_cent(a4)			;detune level work(cent)
		bset.b	#PEON_flg,sl_flag1(a4)		;pitch env flag on
		rts					;

;-----------------------------------------------------------------------;
;	error								;
;-----------------------------------------------------------------------;
	;-------------------------------------------------------;
	;	illegal layer #					;
	;-------------------------------------------------------;
layer_nothing:	bset.b	#ERRa_5,Mem_DRVERR_FLG+1	;
		rts					;

	;-------------------------------------------------------;
	;	voice overflow					;
	;-------------------------------------------------------;
er_24:		bset.b	#ERRa_6,Mem_DRVERR_FLG+1	;
		rts					;

	;-------------------------------------------------------;
	;	volume bias overflow				;
	;-------------------------------------------------------;
bias_overflow_underflow:
		move.b	#OUT_OF_TOTAL_LEVEL,d0		;
		bsr	send_user			;
		rts

	;-------------------------------------------------------;
	;	
	;-------------------------------------------------------;
selected_layer_not_found:
		bset.b	#ERRb16,Mem_err_bit+1		; 該当Ｌayer nothing
		rts					;

	;-------------------------------------------------------;
	;	
	;-------------------------------------------------------;
without_bank_change:
		bset.b	#ERRb17,Mem_err_bit+1		; not ready [k_BANK_adr]
		rts					;

	;-------------------------------------------------------;
	;	
	;-------------------------------------------------------;
play_mono_mode:	move.l	d0,-(sp)			;
		move.b	#FEATURE_NOT_AVAIRABLE,d0	;
		bsr	send_user			;
		move.l	(sp)+,d0			;
		rts					;

	;-------------------------------------------------------;
	;	
	;-------------------------------------------------------;
note_on_data		dc.w	0
			dc.w	0
scspbin_top_address:	dc.l	0

;-----------------------------------------------------------------------;
;	set note & velocity to monitor area				;
;	in	d1.w	midi channel					;
;		d2.b	midi note number				;
;		d3.b	midi velocity					;
;	break	a0,d6							;
;-----------------------------------------------------------------------;
	.if sw_MODEL_M
set_monitor:	move.w	_tmp_kanri(a6),d6		;management # x $200
		cmpi.w	#$E00,d6			;management # = 7 then end
		bne.s	?end				;
		lea	bs_MONTR,a0			;
		move.w	d1,d6				;
		add.w	d6,d6				;
		add.w	d6,d6				;
		move.b	d2,Mem_MNT(a0,d6.w)		;set Tool I/F Monitor(Note#)
		move.b	d3,Mem_MVL(a0,d6.w)		;set Tool I/F Monitor(Velo#)
?end:		rts					;
	.endif			;sw_MODEL_M
;=======================================================================;
;	end of file							;
;=======================================================================;
