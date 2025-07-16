;****************************************************************
;	Saturn Sample Program
;	Master sh2 irq handlers
;
;	Copyright SEGA of America inc. 1994
;---------------------------------------------------------------*
;****************************************************************

	include	"sh2.mac"
	include	"equ.h"
	include	"globals.h"
	include "regs.h"

	section	.text

	.align 4

	xref	readpad

; wait for the next vertical blank interupt
	xdef 	waitvbl
waitvbl
	push	r1
	push	r2

	mov.l	#vcounter,r0
	mov.l	@r0,r1
	add	#1,r1
@waitloop:
	mov.l	@r0,r2
	cmp/gt	r2,r1
	bt	@waitloop

	pop	r2
	pop	r1

	rts
	nop

	.align 4
	lits


	xdef 	init_irqs
init_irqs
	mov.l	#$f0,r2
	ldc	r2,sr

	mov.l	#VBlank_in,r1
	stc.l	vbr,r0
	mov.l	#64*4,r2
	add	r2,r0
	mov.l	r1,@r0
	mov.l	@r0,r1

	mov.l	#$00,r2
	ldc	r2,sr

	mov.l	#intmask,r1
	mov.l	@r1,r0

	mov.l	#$25fe00a0,r1
	mov.l	r0,@r1

	rts
	nop


;-------------------------------------------------------;
;INVALID IRQ
;-------------------------------------------------------;
invalid_irq
	rte		;return back to mainline
	nop

;---------------------------------------------------------------*
;	VRES Interrupt
;---------------------------------------------------------------*
vres_irq:

	rte
	nop


	lits

	cnop	0,4

;---------------------------------------------------------------*
;	V Interrupt
;---------------------------------------------------------------*

VBlank_in:

	push	r0
	push	r1
	push	pr

	mov	#0,r1			; read pad 0
	call	readpad
	mov.l	#pad0,r1
	mov.l	r0,@r1

	mov	#1,r1			; read pad 1
	call	readpad
	mov.l	#pad1,r1
	mov.l	r0,@r1

	mov.l	#vcounter,r0		; increment the virtical blank counter
	mov.l	@r0,r1
	add	#1,r1
	mov.l	r1,@r0

	mov.l	#intmask,r1		; clear the interupt so it can happen again
	mov.l	@r1,r0

	mov.l	#$25fe00a0,r1
	mov.l	r0,@r1


	pop	pr
	pop	r1
	pop	r0

	rte
	nop

	.align	4


	lits

	.align	4

;---------------------------------------------------------------*
;	H Interrupt
;---------------------------------------------------------------*

h_irq:

	mov.l	r2,@-r15
	stc.l	gbr,@-r15	;save regs

	ldc.l	@r15+,gbr
	mov.l	@r15+,r2	;restore regs

	rte		;return to unified IRQ handler
	nop		;RTS!!


	.align	4
hcounter	dc.l	$00000000

	lits

;---------------------------------------------------------------*
;	CMD Interrupt
;---------------------------------------------------------------*

cmd_irq:
	mov.l	r2,@-r15
	stc.l	gbr,@-r15	;save regs

	ldc.l	@r15+,gbr
	mov.l	@r15+,r2	;restore regs

	rte		;return to unified IRQ handler
	nop		;RTS!!


	.align	4
cmdcounter	dc.l	$00000000

	lits


;---------------------------------------------------------------*
;	ERROR
;---------------------------------------------------------------*

error0:
	bra	error0	;bad IRQ HALT
	nop

	end

