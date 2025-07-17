{
	if( $0 ~ /[^A-Za-z0-9"]+(CMP)([^A-Za-z0-9"]+|$)/ )
		gsub( "CMP", "cmp" )
	gsub( ",[ \t]*/[Ss]$", ",.s" )
	gsub( "/[Ss][ \t]+", ".s\t" )
	gsub( "/EQ[ \t]+", "/eq\t" )
	gsub( "/GE[ \t]+", "/ge\t" )
	gsub( "/GT[ \t]+", "/gt\t" )
	gsub( "/HI[ \t]+", "/hi\t" )
	gsub( "/HS[ \t]+", "/hs\t" )
	gsub( "/PL[ \t]+", "/pl\t" )
	gsub( "/PZ[ \t]+", "/pz\t" )
	gsub( "/STR[ \t]+", "/str\t" )
	print
}