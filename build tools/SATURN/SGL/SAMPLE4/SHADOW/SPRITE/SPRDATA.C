#include "sgl.h"
#include "sprdata.h"
#include "../../sprite/sonic.tex"
#include "../../sprite/shadow.tex"
#include "../../sprite/sonic.pl"

TEXTURE form_tbl[] = {
  TEXTBL( 64, 96, CGTop ),
  TEXTBL( 64, 96, CGTop + 0xc00 )
};

void VDP1DataSet( void )
{
  // Pallete transfer.
  slDMACopy( sonic_pl, ( void * )( SonicPalOff ), 0x20 );  // Sonic.
  slDMACopy( sonic_lu, ( void * )( SonicLUTOff ), 0x20 );  // Color Lookup Tbl.

  // Texture transfer.
  slDMACopy( sonic_tex, ( void * )( SonicTxrOff ), sizeof( sonic_tex ) );
  slDMACopy( shadow_tex, ( void * )( ShadowTxrOff ), sizeof( shadow_tex ) );
  slDMAWait();
}

