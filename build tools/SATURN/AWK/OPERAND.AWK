{
	if(( $0 ~ /[Mm][Oo][Vv]\.[LlWwBb][ \t]+r[0-9]+,r[0-9]+/ )){
		gsub( /[Mm][Oo][Vv]\.[LlWwBb]/, "mov\t" )
	}
	if( $0 ~ /mov\.b[ \t]+#/ ){
		gsub( "mov.b", "mov\t" )
	}
	
	gsub( /tas[ \t]+/, "tas.b\t" )
	gsub( /dmuls[ \t]+/, "dmuls.l\t" )
	gsub( /swap[ \t]+/, "swap.w\t" )
	
	gsub( /[ \t]+RTS/, "\trts" )
	gsub( /[ \t]+BRA[ \t]+/, "\tbra\t" )
	gsub( /MOV\.B/, "mov.b" )
	gsub( /MOV\.W/, "mov.w" )
	gsub( /MOV\.L/, "mov.l" )
	gsub( /STS\.L/, "sts.l" )
	gsub( /LDS\.L/, "lds.l" )
	gsub( /STC\.L/, "stc.l" )
	gsub( /LDC\.L/, "ldc.l" )
	
	gsub( /\.endm/, ".ENDM" )
	gsub( /\.macro/, ".MACRO" )
	gsub( /\.aif/, ".AIF" )
	gsub( /\.aelse/, ".AELSE" )
	gsub( /\.aendi/, ".AENDI" )
	gsub( /[ \t]+eq[ \t]+/, "\tEQ\t" )
	gsub( /[ \t]+ne[ \t]+/, "\tNE\t" )
	gsub( /[ \t]+gt[ \t]+/, "\tGT\t" )
	gsub( /[ \t]+ge[ \t]+/, "\tGE\t" )
	gsub( /[ \t]+lt[ \t]+/, "\tLT\t" )
	gsub( /[ \t]+le[ \t]+/, "\tLE\t" )
	gsub( /\.assignc/, ".ASSIGNC" )
	gsub( /\.assigna/, ".ASSIGNA" )
	gsub( /\.assign/, ".ASSIGN" )
	
	print
}
