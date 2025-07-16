;====================================================================
;	/////  SATURN Sound Driver  /////
;	[ System Interface Table DATA : STSTBL.ASM ]
;	Ver 1.10 1994/6/30
;	Digital Media SOUND K.Fujishima
;====================================================================
;
;	/////  SystemInterface Table  (400H) /////
;
	long	$0440		; 00: System information table pointer
	long	$0700		; 04: Host interface work pointer
	long	$0500		; 08: Sound Area Map CRNT Work Pointer
	long	$0600		; 0C: Sound Tool Interface Work Pointer

	blkb	1,0		; 10: DSP program load fiag(BootROM work)
	blkb	1,0		; 11: Sound driver load flag

	long	$0480		; 12: System interface work pointer

	blkb	14,0		; 16: reserved

	long	$0480		; 24: System interface work pointer

	blkb	24,0		; 28: reserved

;
;	/////  System Information Table  (440H) /////
;
	long	$1000		; 68K Program start address
	long	$5000		;             size
	long	$A000		; Sound Area Map start address
	long	$1000		;                size
	long	$6000		; 68K Work start address
	long	$4000		;          size

	blkb	40,0		; reserved
