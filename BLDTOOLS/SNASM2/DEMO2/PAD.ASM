;****************************************************************
;	Saturn Sample Program
;	Master sh2 pad read routine
;
;	Copyright SEGA of America inc. 1994
;---------------------------------------------------------------*
;****************************************************************

	include	"sh2.mac"
	include	"equ.h"
	include	"globals.h"
	include "regs.h"

	section	.text

	align 4

	xdef readpad


; read a joystick
; inputs
;	R1 = port number to read 0 or 1
; output
;	R0 = joystick value or FFFF if no joytick connected
;   8button : 1101 0001 1111 1111 111L RXYZ SACB RLDU

readpad
	push	pr

	push	r1
	push	r2
	push	r3
	push	r4
	push	r5

	push	gbr

	mov.l	#PADADR,r2		; set scmp into sh2 direct mode
	ldc	r2,gbr
	mov	#3,r0
	mov.b	r0,@(SMPADMD,gbr)
	call	smpdelay

	shll	r1			; factor in the port number
	add	r1,r2
	ldc	r2,gbr

	mov	#$40,r0			; read in the ID & make sure it's valid
	mov.b	r0,@(PAD1CTR,gbr)
	call	smpdelay

	mov	#$40,r0
	mov.b	r0,@(PAD1DAT,gbr)
	call	smpdelay

	mov.b	@(PAD1DAT,gbr),r0
	extu.b	r0,r1
	call	smpdelay

	mov	#$0,r0
	mov.b	r0,@(PAD1DAT,gbr)
	call	smpdelay

	mov.b	@(PAD1DAT,gbr),r0
	extu.b	r0,r2
	call	smpdelay

	mov	#0,r0

	mov	#3,r3
	tst	r3,r2
	bt	@l1
	mov	#1,r0
@l1:

	mov	#$0C,r4
	tst	r4,r2
	bt	@l2
	or	#2,r0
@l2:

	tst	r3,r1
	bt	@l3
	or	#4,r0
@l3:

	mov	#$0C,r3
	tst	r3,r1
	bt	@l4
	or	#8,r0
@l4:

	cmp/eq	#$0B,r0
	bt	@correct

	bra	@skip8but
	mov	#-1,r0
@correct:

	mov	#$60,r0
	mov.b	r0,@(PAD1CTR,gbr)
	call	smpdelay
	
	mov	#$60,r0
	mov.b	r0,@(PAD1DAT,gbr)
	call	smpdelay

	mov.b	@(PAD1DAT,gbr),r0
	extu.b	r0,r1
	call	smpdelay

	mov	#$20,r0
	mov.b	r0,@(PAD1DAT,gbr)
	call	smpdelay

	mov.b	@(PAD1DAT,gbr),r0
	extu.b	r0,r2
	call	smpdelay
	
	mov	#$0,r0
	mov.b	r0,@(PAD1DAT,gbr)
	call	smpdelay

	mov.b	@(PAD1DAT,gbr),r0
	extu.b	r0,r3
	call	smpdelay
	
	mov	#$40,r0
	mov.b	r0,@(PAD1DAT,gbr)
	call	smpdelay

	mov.b	@(PAD1DAT,gbr),r0
	extu.b	r0,r4
	call	smpdelay
	
	mov	#$60,r0
	mov.b	r0,@(PAD1DAT,gbr)
	call	smpdelay

	mov	#$60,r0
	mov.b	r0,@(PAD1CTR,gbr)
	call	smpdelay
	
	mov	#$08,r5
	and	r5,r1	
	
	mov	#$0f,r5
	and	r5,r2
	and	r5,r3		
	and	r5,r4		

	shll2	r4
	shll2	r4

	shll8	r3

	shll8	r1
	shll	r1

	mov	r2,r0
	or	r4,r0
	or	r3,r0
	or	r1,r0

	not	r0,r0

@skip8but:

	pop	gbr


	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1

	pop	pr

	rts
	nop

	lits

	align 4


; delay for 2 microseconds
smpdelay:
	mov	#12,r0
@delayloop:
	nop
	dt	r0
	bf	@delayloop

	rts
	nop

	cnop	0,4
	lits


	end


