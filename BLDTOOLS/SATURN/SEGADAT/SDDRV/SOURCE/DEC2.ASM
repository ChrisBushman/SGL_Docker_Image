;=======================================================================;
;	SEGASATURN sound driver						;
; file name								;
;	dec2.asm							;
; copyright								;
;	SEGA ENTERPRISES						;
; purpose								;
;	sequence data decoder						;
; author(s)								;
;	1994-10-26	K.Nitta		Ver.1.00			;
; mod history								;
;	1995-11-14	A.Miyazawa	Ver.2.00			;
;	1996-02-23	A.Miyazawa	Ver.2.06			;
;	1996-07-02	A.Miyazawa	Ver.2.10L			;
;	1997-06-30	Y.Kashima	Ver.2.20			;
;=======================================================================;
		.include	scsp.lib

			.globals on

;	area1 内での各変数の Offset

ofstNextAdr			equ	0		;read pointer
ofstLoopStart			equ	4		;loop start point
ofstRetOfRef			equ	8		;
ofstTopOfTrack			equ	12		;
ofstNextAdrInTmp		equ	16		;
ofst1stTmpInLoop		equ	20		;
ofstRestOfReferenced		equ	24		;
ofst_dummy_1			equ	25		;
ofst_dummy_2			equ	26		;
ofst_dummy_3			equ	27		;
ofst_dummy_4			equ	28		;
ofst_dummy_5			equ	29		;
ofst_dummy_6			equ	30		;
ofst_dummy_7			equ	31		;

mcAddToDeltaTime:	macro
			moveq.l	#0,d7
			move.b	(a0)+,d7	
			add.l	d7,d1
			endm

;-----------------------------------------------------------------------;
;	
;	
;	out:	d0.w	midi channel#,status				;
;		d1.l	delta time
;		d2.l	gate time
;		d3.b	midi command
;		d4.l	
;		d5
;		d6
;-----------------------------------------------------------------------;
sq_decoder:	lsl.w	#5,d1				;x32
		lea.l	area1(pc,d1),a1		;culc work adr of song in 
		add.w	d0,d0				;			area1
		bne	jumpCont			;jump - 10 clk , through = 8 clk
;-----------------------------------------------------------------------;
tp0:		move.l	(a1),a0				;
outputEvent:	moveq.l	#0,d0				;processing_status
		moveq.l	#0,d1				;delta time
		moveq.l	#0,d2				;channel number
		moveq.l	#0,d3				;MIDI CMD
		moveq.l	#0,d4				;MIDI Data 1
		moveq.l	#0,d5				;MIDI Data 2
		moveq.l	#0,d6				;gate time
get1Event:	move.b	(a0)+,d2			;get event
		cmpi.b	#7fh,d2				;
		bhi	notNote				;command
	;-------------------------------------------------------;
	;	note event					;
	;-------------------------------------------------------;
note:		move.b	(a0)+,d4			;note
		move.b	(a0)+,d5			;velo
		move.l	d2,d7				;gate & delta bit8
		add.w	d7,d7				;
		add.w	d7,d7				;
		move.w	d7,d3				;prepare gate
		andi.w	#FFh,d7				;
		add.w	d7,d7				;
		move.b	(a0)+,d3			;p3:gate time
		add.l	d3,d6				;9bit gate time
		move.b	(a0)+,d7			;p4:delta time
		add.l	d7,d1				;9bit delta time
		andi.b	#00001111b,d2			;midi channel #(0-15)
		move.w	#90h,d3				;MIDI CMD
referenceCheck:	cmp	ofstRestOfReferenced(a1),d0	;d0 = proseccing status , = 0
		bne.w	referenceCheckCont		;jump when in referencing
		move.l	a0,(a1)				;
		move.l	d0,d7				;d7:status
		move.b	d2,d0				;
		lsl.w	#8,d0				;
		add.l	d7,d0				;d0:midi channel # with status
		move.l	d6,d2				;d2:9bit gate time
		swap	d4				;
		add.b	d5,d4				;d4:note # with velocity
		rts					;
	;-------------------------------------------------------;
	;	work area					;
	;-------------------------------------------------------;
area1:		ds.b	32*8
	;-------------------------------------------------------;
	;	
	;-------------------------------------------------------;
referenceCheckCont:
		subq.b	#1,ofstRestOfReferenced(a1)	;
		bne.b	storeNextReadPoint		;
		move.l	ofstRetOfRef(a1),(a1)		;
		move.l	d0,d7				;
		move.b	d2,d0				;d0 決定
		lsl.w	#8,d0				;1 byte to left
		add.l	d7,d0				;
		move.l	d6,d2				;d2 決定
		swap	d4				;
		add.b	d5,d4				;
		rts					;
	;-------------------------------------------------------;
	;	save read pointer				;
	;-------------------------------------------------------;
storeNextReadPoint:
		move.l	a0,(a1)				;
	;-------------------------------------------------------;
	;	
	;-------------------------------------------------------;
endOfDecode:	move.l	d0,d7				;
		move.b	d2,d0				;d0 決定
		lsl.w	#8,d0				;1 byte to left
		add.l	d7,d0				;
		move.l	d6,d2				;d2 決定
		swap	d4				;
		add.b	d5,d4				;
		rts					;
	;-------------------------------------------------------;
	;	command event					;
	;-------------------------------------------------------;
notNote:	move.l	d2,d7				;
		subi.w	#80h,d7				;
		add.w	d7,d7				;
		move.w	eventJumpTable(pc,d7),d3	;
		jmp	eventJumpTable(pc,d3)		;
	;-------------------------------------------------------;
eventJumpTable:	dc.w	otherEvent-eventJumpTable	;#80h(その他)
		dc.w	reference-eventJumpTable	;#81h
		dc.w	loop-eventJumpTable		;#82h
		dc.w	endOfTrack-eventJumpTable	;#83h
		dc.w	otherEvent-eventJumpTable	;#84h(その他)
		dc.w	otherEvent-eventJumpTable	;#85h(その他)
		dc.w	otherEvent-eventJumpTable	;#86h(その他)
		dc.w	otherEvent-eventJumpTable	;#87h(その他)
		dc.w	extGate200h-eventJumpTable	;#88h
		dc.w	extGate800h-eventJumpTable	;#89h
		dc.w	extGate1000h-eventJumpTable	;#8ah
		dc.w	extGate2000h-eventJumpTable	;#8bh
		dc.w	extDelta100h-eventJumpTable	;#8ch
		dc.w	extDelta200h-eventJumpTable	;#8dh
		dc.w	extDelta800h-eventJumpTable	;#8eh
		dc.w	extDelta1000h-eventJumpTable	;#8fh
		dc.w	otherEvent-eventJumpTable	;#90h(その他)
		dc.w	otherEvent-eventJumpTable	;#91h(その他)
		dc.w	otherEvent-eventJumpTable	;#92h(その他)
		dc.w	otherEvent-eventJumpTable	;#93h(その他)
		dc.w	otherEvent-eventJumpTable	;#94h(その他)
		dc.w	otherEvent-eventJumpTable	;#95h(その他)
		dc.w	otherEvent-eventJumpTable	;#96h(その他)
		dc.w	otherEvent-eventJumpTable	;#97h(その他)
		dc.w	otherEvent-eventJumpTable	;#98h(その他)
		dc.w	otherEvent-eventJumpTable	;#99h(その他)
		dc.w	otherEvent-eventJumpTable	;#9ah(その他)
		dc.w	otherEvent-eventJumpTable	;#9bh(その他)
		dc.w	otherEvent-eventJumpTable	;#9ch(その他)
		dc.w	otherEvent-eventJumpTable	;#9dh(その他)
		dc.w	otherEvent-eventJumpTable	;#9eh(その他)
		dc.w	otherEvent-eventJumpTable	;#9fh(その他)
		dc.w	polyKeyPressure-eventJumpTable	;#a0h
		dc.w	polyKeyPressure-eventJumpTable	;#a1h(その他)
		dc.w	polyKeyPressure-eventJumpTable	;#a2h(その他)
		dc.w	polyKeyPressure-eventJumpTable	;#a3h(その他)
		dc.w	polyKeyPressure-eventJumpTable	;#a4h(その他)
		dc.w	polyKeyPressure-eventJumpTable	;#a5h(その他)
		dc.w	polyKeyPressure-eventJumpTable	;#a6h(その他)
		dc.w	polyKeyPressure-eventJumpTable	;#a7h(その他)
		dc.w	polyKeyPressure-eventJumpTable	;#a8h(その他)
		dc.w	polyKeyPressure-eventJumpTable	;#a9h(その他)
		dc.w	polyKeyPressure-eventJumpTable	;#aah(その他)
		dc.w	polyKeyPressure-eventJumpTable	;#abh(その他)
		dc.w	polyKeyPressure-eventJumpTable	;#ach(その他)
		dc.w	polyKeyPressure-eventJumpTable	;#adh(その他)
		dc.w	polyKeyPressure-eventJumpTable	;#aeh(その他)
		dc.w	polyKeyPressure-eventJumpTable	;#afh(その他)
		dc.w	controlChange-eventJumpTable	;#b0h(その他)
		dc.w	controlChange-eventJumpTable	;#b1h(その他)
		dc.w	controlChange-eventJumpTable	;#b2h(その他)
		dc.w	controlChange-eventJumpTable	;#b3h(その他)
		dc.w	controlChange-eventJumpTable	;#b4h(その他)
		dc.w	controlChange-eventJumpTable	;#b5h(その他)
		dc.w	controlChange-eventJumpTable	;#b6h(その他)
		dc.w	controlChange-eventJumpTable	;#b7h(その他)
		dc.w	controlChange-eventJumpTable	;#b8h(その他)
		dc.w	controlChange-eventJumpTable	;#b9h(その他)
		dc.w	controlChange-eventJumpTable	;#bah(その他)
		dc.w	controlChange-eventJumpTable	;#bbh(その他)
		dc.w	controlChange-eventJumpTable	;#bch(その他)
		dc.w	controlChange-eventJumpTable	;#bdh(その他)
		dc.w	controlChange-eventJumpTable	;#beh(その他)
		dc.w	controlChange-eventJumpTable	;#bfh(その他)
		dc.w	programChange-eventJumpTable	;#c0h(その他)
		dc.w	programChange-eventJumpTable	;#c1h(その他)
		dc.w	programChange-eventJumpTable	;#c2h(その他)
		dc.w	programChange-eventJumpTable	;#c3h(その他)
		dc.w	programChange-eventJumpTable	;#c4h(その他)
		dc.w	programChange-eventJumpTable	;#c5h(その他)
		dc.w	programChange-eventJumpTable	;#c6h(その他)
		dc.w	programChange-eventJumpTable	;#c7h(その他)
		dc.w	programChange-eventJumpTable	;#c8h(その他)
		dc.w	programChange-eventJumpTable	;#c9h(その他)
		dc.w	programChange-eventJumpTable	;#cah(その他)
		dc.w	programChange-eventJumpTable	;#cbh(その他)
		dc.w	programChange-eventJumpTable	;#cch(その他)
		dc.w	programChange-eventJumpTable	;#cdh(その他)
		dc.w	programChange-eventJumpTable	;#ceh(その他)
		dc.w	programChange-eventJumpTable	;#cfh(その他)
		dc.w	channnelPressure-eventJumpTable	;#d0h(その他)
		dc.w	channnelPressure-eventJumpTable	;#d1h(その他)
		dc.w	channnelPressure-eventJumpTable	;#d2h(その他)
		dc.w	channnelPressure-eventJumpTable	;#d3h(その他)
		dc.w	channnelPressure-eventJumpTable	;#d4h(その他)
		dc.w	channnelPressure-eventJumpTable	;#d5h(その他)
		dc.w	channnelPressure-eventJumpTable	;#d6h(その他)
		dc.w	channnelPressure-eventJumpTable	;#d7h(その他)
		dc.w	channnelPressure-eventJumpTable	;#d8h(その他)
		dc.w	channnelPressure-eventJumpTable	;#d9h(その他)
		dc.w	channnelPressure-eventJumpTable	;#dah(その他)
		dc.w	channnelPressure-eventJumpTable	;#dbh(その他)
		dc.w	channnelPressure-eventJumpTable	;#dch(その他)
		dc.w	channnelPressure-eventJumpTable	;#ddh(その他)
		dc.w	channnelPressure-eventJumpTable	;#deh(その他)
		dc.w	channnelPressure-eventJumpTable	;#dfh(その他)
		dc.w	pitchBend-eventJumpTable	;#e0h(その他)
		dc.w	pitchBend-eventJumpTable	;#e1h(その他)
		dc.w	pitchBend-eventJumpTable	;#e2h(その他)
		dc.w	pitchBend-eventJumpTable	;#e3h(その他)
		dc.w	pitchBend-eventJumpTable	;#e4h(その他)
		dc.w	pitchBend-eventJumpTable	;#e5h(その他)
		dc.w	pitchBend-eventJumpTable	;#e6h(その他)
		dc.w	pitchBend-eventJumpTable	;#e7h(その他)
		dc.w	pitchBend-eventJumpTable	;#e8h(その他)
		dc.w	pitchBend-eventJumpTable	;#e9h(その他)
		dc.w	pitchBend-eventJumpTable	;#eah(その他)
		dc.w	pitchBend-eventJumpTable	;#ebh(その他)
		dc.w	pitchBend-eventJumpTable	;#ech(その他)
		dc.w	pitchBend-eventJumpTable	;#edh(その他)
		dc.w	pitchBend-eventJumpTable	;#eeh(その他)
		dc.w	pitchBend-eventJumpTable	;#efh(その他)
		dc.w	otherEvent-eventJumpTable	;#f0h(その他)
		dc.w	otherEvent-eventJumpTable	;#f1h(その他)
		dc.w	otherEvent-eventJumpTable	;#f2h(その他)
		dc.w	otherEvent-eventJumpTable	;#f3h(その他)
		dc.w	otherEvent-eventJumpTable	;#f4h(その他)
		dc.w	otherEvent-eventJumpTable	;#f5h(その他)
		dc.w	otherEvent-eventJumpTable	;#f6h(その他)
		dc.w	otherEvent-eventJumpTable	;#f7h(その他)
		dc.w	otherEvent-eventJumpTable	;#f8h(その他)
		dc.w	otherEvent-eventJumpTable	;#f9h(その他)
		dc.w	otherEvent-eventJumpTable	;#fah(その他)
		dc.w	otherEvent-eventJumpTable	;#fbh(その他)
		dc.w	otherEvent-eventJumpTable	;#fch(その他)
		dc.w	otherEvent-eventJumpTable	;#fdh(その他)
		dc.w	otherEvent-eventJumpTable	;#feh(その他)
		dc.w	otherEvent-eventJumpTable	;#ffh(その他)

;-----------------------------------------------------------------------;
;	#$81	referance						;
;-----------------------------------------------------------------------;
reference:	moveq.l	#0,d7
		move.b	(a0)+,d7			;p1:event address MSB
		lsl.w	#8,d7				;
		move.b	(a0)+,d7			;p2:event address LSB
		move.b	(a0)+,ofstRestOfReferenced(a1)	;p3:event #
		move.l	a0,ofstRetOfRef(a1)		;next pointer(for end of reference)
		move.l	ofstTopOfTrack(a1),a6		;top address
		lea.l	(a6,d7.l),a0			;next pointer
							;次回読み出し位置はこのルーチン
							;(= ファイル)の最後の方で書く
							;ので、area1 にはここでは
							;書かなくて良い
		bra	get1Event			;
;-----------------------------------------------------------------------;
;	#$82	loop command						;
;-----------------------------------------------------------------------;
loop:	mcAddToDeltaTime				;add delta time
		cmpi.l	#0,ofstLoopStart(a1)		;
		beq	?1stLoopCmd			;
	;------- loop end --------------------------------------;
		move.l	ofstLoopStart(a1),a0		;get loop point
		bra	get1Event			;
	;------- loop start ------------------------------------;
?1stLoopCmd:	move.l	a0,ofstLoopStart(a1)		;loop start point set
		bra	get1Event			;

;-----------------------------------------------------------------------;
;	#$83	end of track						;
;-----------------------------------------------------------------------;
endOfTrack:	moveq.l	#DEC_RET_NODATA,d0		;
		bra	endOfDecode			;
;-----------------------------------------------------------------------;
;	#$c?	program change						;
;-----------------------------------------------------------------------;
programChange:
;-----------------------------------------------------------------------;
;	#$d?	channel pressure					;
;-----------------------------------------------------------------------;
channnelPressure:
		move.b	(a0)+,d4			;p1:value
		bclr.l	#7,d4				;
		bra	continuousCtrlCont		;
;-----------------------------------------------------------------------;
;	#$e?	pitch bend						;
;-----------------------------------------------------------------------;
pitchBend:	moveq.l	#0,d4				;
		move.b	(a0)+,d5			;p1:bend value
		bclr.l	#7,d5				;
		bra	continuousCtrlCont		;
;-----------------------------------------------------------------------;
;	#$a?	polyphonic key pressure					;
;-----------------------------------------------------------------------;
polyKeyPressure:
;-----------------------------------------------------------------------;
;	#$b?	control change						;
;-----------------------------------------------------------------------;
controlChange:	move.b	(a0)+,d4			;p1:control type
		move.b	(a0)+,d5			;p2:value
		bclr.l	#7,d4				;
	;-------------------------------------------------------;
continuousCtrlCont:
		bne	?skip				;channel <= 15 ( count from 0 )
		move.l	d2,d3				;
		andi.b	#00001111b,d2			;
		andi.b	#11110000b,d3			;clear lower 4 bit of midi cmd
	mcAddToDeltaTime				;p3:delta time(add to counter)
		bra	referenceCheck			;
?skip:		move.l	d2,d3				;
		andi.b	#00001111b,d2			;
		bset	#4,d2				;
		andi.b	#11110000b,d3			;clear lower 4 bit of midi cmd
	mcAddToDeltaTime				;p2:delta time(add to counter)
		bra	referenceCheck			;

;-----------------------------------------------------------------------;
;-----------------------------------------------------------------------;
extGate200h:					;#88h
extGate800h:					;#89h
extGate1000h:					;#8ah
extGate2000h:					;#8bh
		move.w	extTable(pc,d7),d3		;get gate time
		add.l	d3,d6				;add to counter
		bra	get1Event			;
;-----------------------------------------------------------------------;
;-----------------------------------------------------------------------;
extDelta100h:					;#8ch
extDelta200h:					;#8dh
extDelta800h:					;#8eh
extDelta1000h:					;#8fh
		move.w	extTable(pc,d7),d3		;get delta time
		add.l	d3,d1				;add to counter
		bra	get1Event			;
	;-------------------------------------------------------;
extTable:	dc.w	0				;
		dc.w	0				;
		dc.w	0				;
		dc.w	0				;
		dc.w	0				;
		dc.w	0				;
		dc.w	0				;
		dc.w	0				;
		dc.w	0200h				;gate
		dc.w	0800h				;
		dc.w	1000h				;
		dc.w	2000h				;
		dc.w	0100h				;delta
		dc.w	0200h				;
		dc.w	0800h				;
		dc.w	1000h				;
;-----------------------------------------------------------------------;
;	event error							;
;-----------------------------------------------------------------------;
otherEvent:	move.l	#DEC_RET_ERROR,d0		;status failed
		bra	endOfDecode			;

;-----------------------------------------------------------------------;
;	
;-----------------------------------------------------------------------;
jumpCont:	move.w	topOfJumpTable(pc,d0),d0	;
		jmp	topOfJumpTable(pc,d0)		;
topOfJumpTable:	dc.w	tp0-topOfJumpTable		;
		dc.w	tp1-topOfJumpTable		;
		dc.w	tp2-topOfJumpTable		;
		dc.w	tp3-topOfJumpTable		;

;-----------------------------------------------------------------------;
;	
;-----------------------------------------------------------------------;
tp1:		adda.w	4(a0),a0			;演奏データ ADR -> area1
		move.l	a0,(a1)				;
		move.l	a0,ofstTopOfTrack(a1)		;
		moveq.l	#0,d7				;
		move.l	d7,ofstLoopStart(a1)		;area1 内ループ開始位置クリア
		move.b	d7,ofstRestOfReferenced(a1)	;
		bra	outputEvent			;

;-----------------------------------------------------------------------;
;	
;-----------------------------------------------------------------------;
tp2:		moveq.l	#DEC_RET_NORMAL,d0		;status
		movea.l	a0,a2				;
		adda.w	4(a0),a2			;演奏データ ADR -> area1
		movea.l	a2,ofstTopOfTrack(a1)		;
		moveq.l	#0,d7				;
		move.l	d7,ofst1stTmpInLoop(a1)		;
		move.w	6(a0),d7			;
		beq	prepareToOut1stTmp		;when tempo loop exists
		add.l	a0,d7				;
		move.l	d7,ofst1stTmpInLoop(a1)		;
prepareToOut1stTmp:
		lea	8(a0),a2			;
		bra	output1TmpEvent			;

;-----------------------------------------------------------------------;
;	
;-----------------------------------------------------------------------;
tp3:		moveq.l	#DEC_RET_NORMAL,d0		;
		move.l	ofstNextAdrInTmp(a1),a2		;今回読み出し位置決定
		cmpa.l	ofstTopOfTrack(a1),a2		;
		bne	output1TmpEvent			;
		; when rest of tempo not exists		;
		cmpi.l	#0,ofst1stTmpInLoop(a1)		;tempo loop is exists?
		bne	tempoLoop			;when tempo loop not exists or old ver.
		moveq.l	#DEC_RET_NODATA,d0		;
		bra	endOfDecode			;
tempoLoop:	move.l	ofst1stTmpInLoop(a1),ofstNextAdrInTmp(a1)
		move.l	ofstNextAdrInTmp(a1),a2		;
	;-------------------------------------------------------;
	;△ time 追い出し、テンポイベント出力
	;	in	a1:今回読み出し位置
	;		a1:area1 内 その曲の頭
	;	out	none
	;	break	none					;
	;-------------------------------------------------------;
output1TmpEvent:
		move.l	(a2)+,d1			;step
		move.l	(a2)+,d6			;tempo value
		move.l	a2,ofstNextAdrInTmp(a1)		;次回に備えて読み位置保存
		bra	endOfDecode			;


;=======================================================================;
;	end of file							;
;=======================================================================;
