#include "sgl.h"
#include "sprdata.h"
#include "../../sprite/tails.tex"
#include "../../sprite/tails.pl"

TEXTURE form_tbl[] = {
  TEXTBL( Tails_Width, Tails_Height, CGTop )
};

void VDP1DataSet( void )
{
  /* Pallete transfer. */
  slDMACopy( tails_pl, ( void * )( TailsPalOff ), 0x200 );  /* Tails */

  /* Texture transfer. */
  slDMACopy( tails_tex, ( void * )( TailsTxrOff ), sizeof( tails_tex ) );
  slDMAWait();
}

