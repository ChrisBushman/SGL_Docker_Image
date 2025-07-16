{
	if( $0 ~ /[ \t]+.+:[ \t]+\.equ[ \t]+.+/ ){
		sub( /[ \t]+/, "" )
	}
	
	gsub( /\$/, "." )
	
	if( $0 ~ /[^A-Za-z0-9"]+(SP)([^A-Za-z0-9"]+|$)/ )
		gsub( "SP", "sp" )
	if( $0 ~ /[^A-Za-z0-9"]+(SR)([^A-Za-z0-9"]+|$)/ )
		gsub( "SR", "sr" )
	if( $0 ~ /[^A-Za-z0-9"]+(PR)([^A-Za-z0-9"]+|$)/ )
		gsub( "PR", "pr" )
	if( $0 ~ /[^A-Za-z0-9"]+(GBR)([^A-Za-z0-9"]+|$)/ )
		gsub( "GBR", "gbr" )
	if( $0 ~ /[^A-Za-z0-9"]+(VBR)([^A-Za-z0-9"]+|$)/ )
		gsub( "VBR", "vbr" )
	if( $0 ~ /[^A-Za-z0-9"]+(PC)([^A-Za-z0-9"]+|$)/ )
		gsub( "PC", "pc" )
	if( $0 ~ /[^A-Za-z0-9"]+(MACH)([^A-Za-z0-9"]+|$)/ )
		gsub( "MACH", "mach" )
	if( $0 ~ /[^A-Za-z0-9"]+(MACL)([^A-Za-z0-9"]+|$)/ )
		gsub( "MACL", "macl" )
	if( $0 ~ /[^A-Za-z0-9]+R[0-9]([^A-Za-z]+|$)/ ){
		gsub( "R0", "r0" )
		gsub( "R1", "r1" )
		gsub( "R2", "r2" )
		gsub( "R3", "r3" )
		gsub( "R4", "r4" )
		gsub( "R5", "r5" )
		gsub( "R6", "r6" )
		gsub( "R7", "r7" )
		gsub( "R8", "r8" )
		gsub( "R9", "r9" )
	}
	print
}
