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

	align 4

	xref	flip_fb, start_draw
	xref	readpad

; set an SCU interupt handler function
; input
;	R4 = interupt number
;	R5 = pointer to handler
_set_SCU_int
	push	pr
	
	mov.l	#SYS_SETUINT,r0
	mov.l	@r0,r0
	jsr	@r0
	nop

	pop	pr
	
	rts
	nop


; set the SCU interupt mask
; input
;	R4 = AND mask for enabling interupts
;	R5 = OR mask for disabling interupts
_set_SCU_int_mask
	push	pr
	
	mov.l	#SYS_SETSCUIM,r0
	mov.l	@r0,r0
	jsr	@r0
	nop

	pop	pr
	
	rts
	nop


; set an SCU interupt handler function
; input
;	R4 = interupt number
;	R5 = pointer to handler
_set_SYS_int
	
	push	pr

	mov.l	#SYS_SETSINT,r0
	mov.l	@r0,r0
	jsr	@r0
	nop

	pop	pr
	
	rts
	nop



	xdef 	_init_irqs
_init_irqs

	push	pr

	mov.l	#$40,r4
	mov.l	#VBlank_in,r5
	call	_set_SCU_int
	
	mov.l	#$41,r4
	mov.l	#VBlank_out,r5
	call	_set_SCU_int
	
	mov.l	#$4D,r4
	mov.l	#Draw_Done,r5
	call	_set_SCU_int
	
	
	mov.l	#_intmask,r4
	mov.l	@r4,r4
	call	_set_SCU_int_mask
	
	
	pop	pr

	rts
	nop

	align 4
	lits
	align 4


; wait for VDP1 to finish drawing sprites 
	xdef 	_waitvdp1
_waitvdp1
	push	r1

	mov.l	#_VDP1busy,r1
	mov.l	#_drawflag,r2
@waitloop:
	mov.l	@r1,r0
	mov.l	@r2,r3
	or	r3,r0
	cmp/eq	#0,r0
	bf	@waitloop

	pop	r1

	rts
	nop

	align 4
	lits


	
; wait for the next vertical blank interupt
	xdef 	_waitvbl
_waitvbl
	push	r1
	push	r2

	mov.l	#_vcounter,r0
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

	align 4
	lits

	
;---------------------------------------------------------------*
;	VDP1 drawing complete
;---------------------------------------------------------------*

Draw_Done:
       
	push	r0,r1,r2

	mov.l	#_VDP1busy,r0		; clear vdp1 busy flag once drawing is completed
	mov	#0,r1
	mov.l	r1,@r0

	mov.l	#_vcounter,r0
	mov.l	@r0,r1
	mov.l	#_drawcounter,r0
	mov.l	@r0,r2
	sub	r2,r1
	mov.l	r1,@r0

	pop	r2,r1,r0

	rts
	nop		

	align	4


	lits

	align	4

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

	mov.l	#_drawflag,r1
	mov.l	@r1,r0
	cmp/eq	#0,r0
	bt	@skpclr

	mov.l	#_clearflag,r1
	mov	#1,r0
	mov.l	r0,@r1

	mov.l	#TVMR,r0
	mov	#VDP1MODE|8,r1
	mov.w	r1,@r0

	call	flip_fb
	call	start_draw

@skpclr:


	pop	pr
	pop	r1
	pop	r0

	rts
	nop		

	align	4


	lits

	align	4

;---------------------------------------------------------------*
;	V Interrupt end
;---------------------------------------------------------------*

VBlank_out:
       
	push	r0
	push	r1
	push	pr


	mov.l	#_clearflag,r1
	mov.l	@r1,r0
	cmp/eq	#0,r0
	bt	@skpdraw

	mov	#0,r0
	mov.l	r0,@r1

	mov.l	#_drawflag,r1
	mov.l	r0,@r1

	mov	#VDP1MODE,r0
	mov.l	#TVMR,r1
	mov.w	r0,@r1

	mov.l	#_VDP1busy,r0
	mov	#1,r1
	mov.l	r1,@r0

;	call	start_draw
	mov.l	#_vcounter,r0
	mov.l	@r0,r1
	mov.l	#_drawcounter,r0
	mov.l	r1,@r0
@skpdraw:

	mov	#0,r1			; read pad 0
	call	readpad
	mov.l	#_pad0,r1
	mov.l	r0,@r1

	mov	#1,r1			; read pad 1
	call	readpad
	mov.l	#_pad1,r1
	mov.l	r0,@r1

	mov.l	#_vcounter,r0		; increment the virtical blank counter
	mov.l	@r0,r1
	add	#1,r1
	mov.l	r1,@r0



	pop	pr
	pop	r1
	pop	r0

	rts
	nop		

	align	4


	lits

	align	4



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


	align	4
hcounter	dc.l	$00000000

	lits

;---------------------------------------------------------------*
;	ERROR
;---------------------------------------------------------------*

error0:
	bra	error0	;bad IRQ HALT
	nop

	end

