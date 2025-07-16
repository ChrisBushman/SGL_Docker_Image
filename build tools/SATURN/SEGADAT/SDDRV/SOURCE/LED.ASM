;=======================================================================;
;	SEGASATURN sound driver						;
; file name								;
;	led.asm								;
; copyright								;
;	SEGA ENTERPRISES						;
; purpose								;
;	led & error function control					;
; author(s)								;
;	1995-11-14	A.Miyazawa	Ver.2.00			;
; mod history								;
;	1996-02-23	A.Miyazawa	Ver.2.06			;
;	1996-07-02	A.Miyazawa	Ver.2.10L			;
;	1997-06-30	Y.Kashima	Ver.2.20			;
;=======================================================================;

		.include	scsp.lib

			.extern	program_start

			.global	er_01
			.global	er_09,er_0A,er_0B,er_0C,er_0D,er_0E,er_0F
			.global	er_10,er_11,er_12,er_13,er_14,er_15,er_16
			.global	er_17,er_18,er_19,er_1F
			.global	er_27

	if	sw_MODEL_M
			.global	LED_CTRL
;-----------------------------------------------------------------------;
;	ÇkÇdÇc êßå‰ Åi äJî≠ÉÇÅ[Éh only Åj				;
; LED8(âEí[) -- MIDI-A ( MCS-1 ) éÛêM					;
; LED7       -- MIDI-B ( MCS-2 ) éÛêM					;
; LED6       -- 							;
; LED5       -- 							;
; LED4       -- 							;
; LED3       -- ÇrÇdÇpâìÄÇorgê≥èÌìÆçÏ					;
; LED2       -- CPU over heat						;
; LED1(ç∂í[) -- CPU ê≥èÌìÆçÏ						;
;-----------------------------------------------------------------------;
LED_CTRL:	move.w	CPU_CAPA_cnt(a6),d1		;
		subq.w	#1,d1				;
		bne.s	MIDI_capa_1			;
		ori.b	#%00000010,LED_status(a6)	;
		move.b	#CTO_LED_speed,CTO_LED_cnt(a6)	;
		addi.l	#1,OVER_count(a6)		;
MIDI_capa_1:	move.w	#0,CPU_CAPA_cnt(a6)		;
		move.b	BIG_timer(a6),d0		;
		andi.b	#3,d0				;8msec ?
		beq.s	LED_ctrl_10			;jump if yes
		rts					;
	;-------------------------------------------------------;
LED_ctrl_10:	move.b	LED_status(a6),d2		;
		addi.w	#CPU_LED_speed,LED8_cnt_wk(a6)	;CPU ìÆçÏ
		bcc.s	LED_ctrl_01			;
		bchg	#0,d2				;CPU LED bit0
	;-------------------------------------------------------;
LED_ctrl_01:	move.b	RCVA_LED_cnt(a6),d0		;midi port a receive
		subq.b	#1,d0				;
		bcs.s	LED_ctrl_3			;
		bne.s	LED_ctrl_2			;
		andi.b	#01111111B,d2			;
LED_ctrl_2:	move.b	d0,RCVA_LED_cnt(a6)		;
	;-------------------------------------------------------;
LED_ctrl_3:	move.b	RCVB_LED_cnt(a6),d0		;midi port b receive
		subq.b	#1,d0				;
		bcs.s	LED_ctrl_5			;
		bne.s	LED_ctrl_4			;
		andi.b	#10111111B,d2			;
LED_ctrl_4:	move.b	d0,RCVB_LED_cnt(a6)		;
	;-------------------------------------------------------;
			.global	LED_ctrl_5		;
LED_ctrl_5:	move.b	CTO_LED_cnt(a6),d0		;CPU over heat
		subq.b	#8,d0				;
		bcs.s	LED_ctrl_7			;
		bne.s	LED_ctrl_6			;
		andi.b	#11111101B,d2			;
LED_ctrl_6:	move.b	d0,CTO_LED_cnt(a6)		;
	;-------------------------------------------------------;
LED_ctrl_7:	move.b	SEQ_LED_cnt(a6),d0		;seq data extruct
		bpl.s	LED_ctrl_8			;
		ori.b	#00000100B,d2			;
		bra.s	LED_ctrl_9			;
LED_ctrl_8:	andi.b	#11111011B,d2			;
	;-------------------------------------------------------;
LED_ctrl_9:
LED_ctrl_B:	move.b	d2,LED_status(a6)		;
		move.b	d2,IO_LED+1			;
		rts					;
;-----------------------------------------------------------------------;
;	error flags out to MIDI-OUTPUT 	(model-M function)		;
;	in	none							;
;	out	none							;
;	break	a0,d0,d1,d2,d3						;
;-----------------------------------------------------------------------;
			.global	MIDI_er_out
MIDI_er_out:
		move.w	Mem_DRVERR_FLG,d1		;
		move.b	#$81,d0				;
	sr_push						;
	int_di						;
	;------- 1st data send ---------------------------------;
		move.l	d1,d2				;
		moveq	#$7F,d3				;
		lea	IO_MIDI1+1,a0			;$d00001
		move.b	#5,R01(a0)			;
		move.b	Rx4(a0),d1			;FIFO-Tx status check(R54)
		andi.b	#01000000B,d1			;TxRDY ?
		beq	MIDIA_SEND_er			;
		move.b	d0,Rx6(a0)			;send data to R56
	;------- 2nd data send ---------------------------------;
		move.b	d2,d0				;
		and.b	d3,d0				;7bit
		move.b	#5,R01(a0)			;
		move.b	Rx4(a0),d1			;FIFO-Tx status check(R54)
		andi.b	#01000000B,d1			;TxRDY ?
		beq	MIDIA_SEND_er			;
		move.b	d0,Rx6(a0)			;send data to R56
	;------- 3rd data send ---------------------------------;
		lsr.l	#8,d2				;MSB
		move.b	d2,d0				;
		and.b	d3,d0				;7bit
		move.b	#5,R01(a0)			;
		move.b	Rx4(a0),d1			;FIFO-Tx status check(R54)
		andi.b	#01000000B,d1			;TxRDY ?
		beq	MIDIA_SEND_er			;
		move.b	d0,Rx6(a0)			;send data to R56
MIDIA_SEND_er:
	sr_pop						;
		rts					;
	endif		;sw_MODEL_M			;
;-----------------------------------------------------------------------;
;	error function							;
;-----------------------------------------------------------------------;
	;------- buss error ------------------------------------;
er_1F:		bset.b	#ERRb30,Mem_err_bit+0		; CPU buss error
		jmp	program_start(pc)		;
	;------- address error ---------------------------------;
er_01:		bset.b	#ERRb28,Mem_err_bit+0		; CPU address error
		jmp	program_start(pc)		;
	;------- level#5 ---------------------------------------;
er_09:		bset.b	#ERRa_0,Mem_DRVERR_FLG+1	; SDDRVS : ñ¢íËã` Level#5 î≠ê∂
		rts					; SDDRV  : MIDI-1 Level#5 MIDI error
	;------- level#6 ---------------------------------------;
er_0A:		bset.b	#ERRa_1,Mem_DRVERR_FLG+1	; SDDRVS : ñ¢íËã` Level#6 î≠ê∂
		rts					; SDDRV  : MIDI-2 Level#6 MIDI error
	;------- e.t.c. ----------------------------------------;
er_27:				; Level#1 event on
er_0B:				; Level#3 event on
er_0C:				; Level#4 event on
er_0D:				; Level#7 event on
er_0E:				; CPU vector#04 ïsìññΩóﬂ
er_0F:				; CPU vector#05 ÇOèúéZ
er_10:				; CPU vector#06 CHKñΩóﬂ
er_11:				; CPU vector#07 TRAPVñΩóﬂ
er_12:				; CPU vector#08 ì¡å†à·îΩ
er_13:				; CPU vector#09 TRACEó·äOèàóù
er_14:				; CPU vector#10,11 ñ¢é¿ëïñΩóﬂ
er_15:				; CPU vector#12,13,14,16Å`23,48Å`63 reserved
er_16:				; CPU vector#15 uninitialized
er_17:				; CPU vector#24 ÉXÉvÉäÉAÉX
er_18:				; CPU vector#32Å`47  TRAPñΩóﬂ
er_19:				; CPU vector#64Å`255 user vector
		bset.b	#ERRb29,Mem_err_bit+0		; CPU ó·äOèàóù
		jmp	program_start(pc)		;
;=======================================================================;
;	end of file							;
;=======================================================================;
