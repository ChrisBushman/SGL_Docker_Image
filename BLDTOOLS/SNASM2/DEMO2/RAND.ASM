

	include	"sh2.mac"
	include	"globals.h"

	section	.text

	xdef	_getrand
_getrand
	push	r2,r3

	mov.l	#rndptr,r0
	mov.l	@r0,r1
	mov.l	#rndtable,r2
	add	r1,r2
	mov.l	@r2,r2
	
	add	#4,r1

	mov.l	#1023,r3
	and	r3,r1
	mov.l	r1,@r0

	mov.l	#_randval,r0
	mov.l	@r0,r1
	add.l	r2,r1
	rotl	r1
	mov.l	r1,@r0

	pop	r3,r2

	rts
	nop

	align 4
	
	lits

	align 4

rndptr	dc.l	0

rndtable
	dc.l	$41c6167e,$2781446b,$794b15fb,$59e21cfb
	dc.l	$3f540ff6,$0abd31df,$237c2f1c,$7de14487
	dc.l	$620152bf,$623163de,$69561372,$500f2847
	dc.l	$2c677566,$42873359,$4daa6488,$773c6f59
	dc.l	$6eea5656,$61134a7b,$31d22d85,$19a169d8
	dc.l	$353c3b54,$7d55102f,$1b377aae,$5e65345b
	dc.l	$63da6102,$7a790398,$28cc60e3,$1f1a4b76
	dc.l	$058e7a5f,$28d90799,$388f751f,$493f0f36
	dc.l	$48ee2043,$3f781e4d,$3d0d33fa,$6fbe36a6
	dc.l	$2dda30e4,$0586148e,$42dc5929,$0c6d334e
	dc.l	$62ff1f56,$2be11d70,$262026fb,$718f04b1
	dc.l	$53583305,$56905dc5,$78091ddc,$6b537bcd
	dc.l	$63aa373b,$56486799,$145223d3,$11526e9d
	dc.l	$60061a9f,$11ea25b5,$66c21c06,$48135b98
	dc.l	$19497eb2,$2001371e,$60ac6e32,$72887d31
	dc.l	$329c3752,$10463a95,$1e717e36,$538f4d57
	dc.l	$1ef63a39,$651d0a16,$3cfa1388,$1f7478f5
	dc.l	$4e987e7c,$3d17665c,$5f4130bb,$3d6d4371
	dc.l	$0c8e4c0f,$2c701259,$63c75b01,$391b5a2f
	dc.l	$0c337b3d,$46915ec0,$4a1d51a5,$640d180d
	dc.l	$26ab2433,$578d257e,$485e358f,$273e38e6
	dc.l	$48683e74,$4da6353a,$10b120c3,$04937d11
	dc.l	$0ea83064,$52c72cdb,$46ca2de0,$48603ce1
	dc.l	$14f34ebf,$16094700,$25676fa2,$6ae36c25
	dc.l	$72a04c21,$45311587,$53d55862,$21c50da8
	dc.l	$684f187e,$362e2d09,$6b6b1194,$219f16b0
	dc.l	$3d6d30a9,$409e405a,$2d0b4346,$0f704280
	dc.l	$4db65dcf,$47472c0c,$66a64ba5,$222a55d8
	dc.l	$46ac64fb,$71a071eb,$08b76679,$74245272
	dc.l	$15232692,$15482480,$6bc544a6,$04a71a85
	dc.l	$02b72dd7,$4f8c4290,$45e413ab,$69630444
	dc.l	$03523066,$4ee3029c,$50337525,$2ff95d5e
	dc.l	$32aa7dba,$4c730e60,$1c5d664b,$6f715e7e
	dc.l	$01be5ea9,$358c2e57,$1a191771,$09c30eca
	dc.l	$145e64e5,$052a6533,$4cac3388,$1d511766
	dc.l	$4ea12a7b,$4d757b67,$3064179a,$266906ef
	dc.l	$126f0156,$71427aa0,$501d7a51,$50c50502
	dc.l	$2cf712bb,$0d921945,$0abe036f,$790d75b6
	dc.l	$74386ecc,$131015fd,$08bb5354,$5e510d1c
	dc.l	$147b0c07,$0f940327,$61937a7d,$039252c3
	dc.l	$0dd436c6,$27a50261,$71516101,$41381538
	dc.l	$61a770bf,$3ff17004,$5e0d1e15,$069b4d80
	dc.l	$701f4083,$56d57ea4,$4d693f88,$4b7c729f
	dc.l	$05b67101,$0dda4293,$4a170145,$318b3d12
	dc.l	$18b24102,$63332d5c,$595074d6,$55e15a56
	dc.l	$36a402ad,$1b42794a,$405c18dd,$52861061
	dc.l	$21e92b03,$5a1204e1,$7a0f609b,$6fea5126
	dc.l	$1f2c5161,$6cdc1e62,$5c482b6b,$066d3e14
	dc.l	$72e01f03,$43853f4a,$6d722c46,$0fda1b96
	dc.l	$0ec82e7d,$1a1c37d1,$6a05413e,$68e53492
	dc.l	$6f705b43,$525f4a6c,$7a033b05,$42b32deb
	dc.l	$29b30120,$7d35374d,$167e1466,$44504a01
	dc.l	$28362bc0,$143337e1,$1f0f19c9,$5c381c2e
	dc.l	$18e93629,$63196a4f,$1f5e00b1,$41d12c49
	dc.l	$0a8b5a3b,$21532cfd,$449f6f3f,$26ee0a25
	dc.l	$3a253035,$3b7b6a0d,$031173af,$194c5111
	dc.l	$108c1e32,$4cd452da,$6b7f6bd8,$14161b57
	dc.l	$4fe137a6,$48ce0b7d,$30c15cae,$416265bf
	dc.l	$71130ce4,$5487464c,$5d3a39c1,$6bb3630c
	dc.l	$31596a99,$50474f58,$385a1cbd,$1f783f7c
	dc.l	$4fba0950,$110107ed,$5c1b6cea,$7c8a5449
	dc.l	$7a882cee,$49d65214,$7a8576ab,$37b04b2c
	dc.l	$6cde3435,$25936c11,$532d7301,$4c1c31d7
	dc.l	$3c281843,$10303be7,$58b07f08,$5ced7d79

	end

