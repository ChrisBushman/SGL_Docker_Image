;=======================================================================;
;	SEGASATURN sound driver						;
; file name								;
;	sdmain.asm							;
; copyright								;
;	SEGA ENTERPRISES						;
; purpose								;
;									;
; author(s)								;
;	1995-11-14	A.Miyazawa	Ver.2.00			;
; mod history								;
;	1996-02-23	A.Miyazawa	Ver.2.06			;
;	1996-07-02	A.Miyazawa	Ver.2.10L			;
;	1997-06-30	Y.Kashima	Ver.2.20			;
;=======================================================================;

		.include	scsp.lib

			.extern	com_get_workaddress	;sdcom
			.extern	stop_to_start		;sddrv
			.extern	stop_and_start		;sddrv
			.extern	play_to_pause		;sddrv
			.extern	fade_to_pause		;sddrv
			.extern	to_stop			;sddrv
			.extern	playpause_to_play	;sddrv
			.extern	fadepause_to_fade	;sddrv
			.extern	dsp3d_init		;dsp3d
			.extern	dsp3d_op		;dsp3d

;-----------------------------------------------------------------------;
;	command analyze & jump process					;
;	--------------------------------------------------------	;
;	set address register & control mode/command matrix		;
;	in	d0:management number(0-7)				;
;		d1:input parameter					;
;		d2:input parameter					;
;		d3:input parameter					;
;		d7:command number					;
;		a0:sequence data top address				;
;		a2:jump table BASE address				;
;	out	d0:play number(0-7)					;
;		d1:input parameter					;
;		d2:input parameter					;
;		d3:input parameter					;
;		a0:sequence data top address				;
;		a4:Decoder work	top address				;
;		a5:Note ON Cue table	top address			;
;		a6:Sequence control block top address			;
;	break	a2,d7							;
;-----------------------------------------------------------------------;
			.public		command_interpriter
command_interpriter:
		bsr	com_get_workaddress		;set a4,a5,a6
		move.l	d0,-(sp)			;
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		add.w	d0,d0				;
		adda.w	d0,a6				;
		adda.w	d0,a6				;
		adda.w	d0,a6				;
		move.l	(sp)+,d0			;

		add.b	CB_MODE(a6),d7			; d7=mode(0-7)
		add.w	d7,d7				;
		movea.w	command_jump_table(pc,d7.w),a2	;
		jmp	(a2)				;
	;-------------------------------------------------------;
			.public		ret_normal
ret_normal:	clc					; CY=0: Normal end
		rts
	;-------------------------------------------------------;
			.public		ret_error
ret_error:	stc					; CY=1: Error
		rts
	;-------------------------------------------------------;
command_jump_table:
	;-------------------------------------------------------;
	;	start command					;
	;-------------------------------------------------------;
		dc.w	stop_to_start			;0:stop
		dc.w	stop_and_start			;1:play
		dc.w	stop_to_start			;2:stop and pause
		dc.w	stop_and_start			;3:play and pause
		dc.w	stop_to_start			;4:stop
		dc.w	stop_and_start			;5:fade
		dc.w	stop_to_start			;6:stop
		dc.w	stop_and_start			;7:fade and pause
	;-------------------------------------------------------;
	;	stop command					;
	;-------------------------------------------------------;
		dc.w	ret_normal			;0:stop
		dc.w	to_stop				;1:play
		dc.w	ret_normal			;2:stop and pause
		dc.w	to_stop				;3:play and pause
		dc.w	ret_normal			;4:stop
		dc.w	to_stop				;5:fade
		dc.w	ret_normal			;6:stop
		dc.w	to_stop				;7:fade and pause
	;-------------------------------------------------------;
	;	pause command					;
	;-------------------------------------------------------;
		dc.w	ret_normal			;0:stop
		dc.w	play_to_pause			;1:play
		dc.w	ret_normal			;2:stop and pause
		dc.w	ret_normal			;3:play and pause
		dc.w	ret_normal			;4:stop
		dc.w	fade_to_pause			;5:fade
		dc.w	ret_normal			;6:stop
		dc.w	ret_normal			;7:fade and pause
	;-------------------------------------------------------;
	;	resume command					;
	;-------------------------------------------------------;
		dc.w	ret_normal			;0:stop
		dc.w	ret_normal			;1:play
		dc.w	ret_normal			;2:stop and pause
		dc.w	playpause_to_play		;3:play and pause
		dc.w	ret_normal			;4:stop
		dc.w	ret_normal			;5:fade
		dc.w	ret_normal			;6:stop
		dc.w	fadepause_to_fade		;7:fade and pause

;-----------------------------------------------------------------------;
;	q sound control							;
;	input	d0:channel(0-3:4ch or 0-7:8ch)				;
;		d1:pan data(0-30)					;
;	output	none							;
;	break	d0,d1,d2,d3,d7,a0,a1,a2,a6,flag				;
;-----------------------------------------------------------------------;
			.public	xseq_qsound
xseq_qsound:	movea.w	#bs_SIFWK,a0		;system interface work($0480)
		move.b	OF_3D_FLAG(a0),d2	;
		andi.b	#$30,d2			;q sound use check
		beq.w	ret_normal		;
		movea.l	#SCSP_DSP_COEF,a1	;scsp coef address($100700)
		lea	qsound_pan_table,a2	;q sound filter data table
		moveq	#$03,d3			;mask(0-3)
		btst	#4,d2			;8ch?
		beq.s	?skip			;
		move.b	#$07,d3			;mask(0-7)
?skip:		and.l	d3,d0			;mask channel
		lsl.l	#3,d0			;ch x 8 (get CH offset)
		andi.l	#$1f,d1			;mask PAN data(0-30)
		lsl.l	#3,d1			;pan x 8 (get PAN offset)
		clr.l	d7			;
		move.b	OF_QCOEF_PTR(a0),d7	;
		adda.l	d7,a1			;100700 + COEF offset
		adda.l	d0,a1			;	+ CH   offset
		adda.l	d1,a2			;get PAN data  offset
		move.w	(a2)+,(a1)+		;set L filter data
		move.w	(a2)+,(a1)+		;set L direct data
		move.w	(a2)+,(a1)+		;set R direct data
		move.w	(a2)+,(a1)+		;set R filter data
		bra.w	ret_normal		;
;-----------------------------------------------------------------------;
;	yamaha 3d status write						;
;	in	d0:distance(00-7F)					;
;		d1:azimuth position(00-7F)				;
;		d2:elevation position(00-7F)				;
;	out	none							;
;	break	d0,d1,d2,d4,a0,flag					;
;-----------------------------------------------------------------------;
			.public	xseq_YMH3Dw
xseq_YMH3Dw:	movea.w	#bs_SIFWK,a0			;system interface work($0480)
		move.b	OF_3D_FLAG(a0),d4		;
		andi.b	#$c0,d4				;YAMAHA 3D use check
		beq.w	ret_normal			;
		adda.l	#OF_3DCH1_WORK,a0		;
		move.b	d0,(a0)+			;distance
		move.b	d1,(a0)+			;azimuth
		move.b	d2,(a0)+			;elevation
		move.b	#$80,(a0)+			;flag
		move.b	#$7f,(a0)+			;dist.
		move.b	#$7f,(a0)+			;azim.
		move.b	#$7f,(a0)			;elev.
		bsr	dsp3d_op			;
		bra.w	ret_normal			;

	if	0		;yamaha 3d status read
;-----------------------------------------------------------------------;
;	yamaha 3d status read						;
;	in	none							;
;	out	d0:distance(00-7F)					;
;		d1:azimuth position(00-7F)				;
;		d2:elevation position(00-7F)				;
;	break	a0							;
;-----------------------------------------------------------------------;
			.public	xseq_YMH3Dr
xseq_YMH3Dr:	movea.l	Mem_IFWK_PTR,a0			; a0: system IFwork top(480h)
		move.b	OF_3D_FLAG(a0),d1		;
		andi.b	#$c0,d1				;YAMAHA 3D use check
		beq.w	ret_normal			;
		adda.l	#OF_3DCH1_WORK,a0		;
		move.b	(a0)+,d0			;distance
		move.b	(a0)+,d1			;azimuth
		move.b	(a0),d2				;elevation
		bra.w	ret_normal			;
	endif			;yamaha 3d status read

;-----------------------------------------------------------------------;
;	yamaha 3d initialize						;
;	in	none							;
;	out	none							;
;	break	a0,d0							;
;-----------------------------------------------------------------------;
			.public	xseq_YMH3Di
xseq_YMH3Di:	movea.w	#bs_SIFWK,a0			;system interface work($0480)
		move.b	OF_3D_FLAG(a0),d0		;
		andi.b	#$c0,d0				;YAMAHA 3D use check
		beq.w	ret_normal			;
		adda.l	#OF_3DCH1_WORK,a0		;
		moveq	#0,d0				;
		move.b	d0,(a0)+			;distance
		move.b	d0,(a0)+			;azimuth
		move.b	#$20,(a0)			;elevation
		bsr	dsp3d_init			;
		bra.w	ret_normal			;

;-----------------------------------------------------------------------;
;	q sound control filter data table				;
;-----------------------------------------------------------------------;
			.public	qsound_pan_table
qsound_pan_table:
		dc.w	$0000*8,$0FFF*8,$0000*8,$0FFF*8	; 00: L15
		dc.w	$0000*8,$0FFF*8,$0000*8,$0E5E*8	; 01: L14
		dc.w	$0000*8,$0FFF*8,$0000*8,$0BF2*8	; 02: L13
		dc.w	$0000*8,$0FFF*8,$0000*8,$09F9*8	; 03: L12
		dc.w	$0000*8,$0FFF*8,$0000*8,$0770*8	; 04: L11
		dc.w	$0000*8,$0FFF*8,$0000*8,$042E*8	; 05: L10
		dc.w	$0000*8,$0FFF*8,$0000*8,$0244*8	; 06: L9
		dc.w	$0000*8,$0FFF*8,$0199*8,$007F*8	; 07: L8
		dc.w	$0000*8,$0F33*8,$0333*8,$002C*8	; 08: L7
		dc.w	$0000*8,$0F33*8,$0333*8,$0015*8	; 09: L6
		dc.w	$0000*8,$0E66*8,$0666*8,$000A*8	; 10: L5
		dc.w	$0000*8,$0E66*8,$0800*8,$0005*8	; 11: L4
		dc.w	$0000*8,$0D99*8,$08CC*8,$0002*8	; 12: L3
		dc.w	$0000*8,$0D99*8,$0999*8,$0001*8	; 13: L2
		dc.w	$0000*8,$0CCC*8,$0A66*8,$0000*8	; 14: L1
		dc.w	$0000*8,$0B33*8,$0B33*8,$0000*8	; 15: center
		dc.w	$0000*8,$0A66*8,$0CCC*8,$0000*8	; 16: R1
		dc.w	$0001*8,$0999*8,$0D99*8,$0000*8	; 17: R2
		dc.w	$0002*8,$08CC*8,$0D99*8,$0000*8	; 18: R3
		dc.w	$0005*8,$0800*8,$0E66*8,$0000*8	; 19: R4
		dc.w	$000B*8,$0666*8,$0E66*8,$0000*8	; 20: R5
		dc.w	$0016*8,$04CC*8,$0F33*8,$0000*8	; 21: R6
		dc.w	$002C*8,$0333*8,$0F33*8,$0000*8	; 22: R7
		dc.w	$007F*8,$0199*8,$0FFF*8,$0000*8	; 23: R8
		dc.w	$0251*8,$0000*8,$0FFF*8,$0000*8	; 24: R9
		dc.w	$047B*8,$0000*8,$0FFF*8,$0000*8	; 25: R10
		dc.w	$0770*8,$0000*8,$0FFF*8,$0000*8	; 26: R11
		dc.w	$09F9*8,$0000*8,$0FFF*8,$0000*8	; 27: R12
		dc.w	$0BF2*8,$0000*8,$0FFF*8,$0000*8	; 28: R13
		dc.w	$0DE6*8,$0000*8,$0FFF*8,$0000*8	; 29: R14
		dc.w	$0FFF*8,$0000*8,$0FFF*8,$0000*8	; 30: R15

;=======================================================================;
;	end of file							;
;=======================================================================;
