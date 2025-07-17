/*
*/
#include "sgl.h"
#include "../../scroll/tree.cel"
#include "../../scroll/tree.map"
#include "../../scroll/tree.pal"
#include "../../sprite/tails.pl"
#include "../../sprite/tails.tex"

#define UNIT toFIXED( 1.0 );
TEXTURE formtbl = TEXTBL( 96, 96, 0x10000 );
SPR_ATTR tails_atr = SPR_ATTRIBUTE( 0, 0x100 | ( 5 << 12 ) | ( 1 << 11 ),
				   No_Gouraud, CL256Bnk, sprNoflip | _ZmCC );
FIXED pos[] = { toFIXED( 0.0 ), toFIXED( 0.0 ), toFIXED( 150.0 ) };

void ss_main() {
  FIXED  x = 0, y = 0;
  Uint16 pad;

  slInitSystem( TV_320x240, &formtbl, 1 );

  slTVOff();

  slColRAMMode( CRM16_1024 );
  
  slCharNbg1( COL_TYPE_256, CHAR_SIZE_2x2 );
  slPageNbg1( ( void * )(0x25e00000), 0, PNB_2WORD );
  slPlaneNbg1( PL_SIZE_1x1 );
  slMapNbg1( ( void * )(0x25e20000), ( void * )(0x25e20000), ( void * )(0x25e20000), ( void * )(0x25e20000) );

  slDMACopy( cel_tree, ( void * )( 0x25e00000 ), sizeof( cel_tree ) );
  slDMACopy( map_tree, ( void * )( 0x25e20000 ), sizeof( map_tree ) );
  slDMACopy( pal_tree, ( void * )( 0x25f00000 ), sizeof( pal_tree ) );

  slDMACopy( tails_pl, ( void * )0x25f00200, sizeof( tails_pl ) );
  slDMACopy( tails_tex, ( void * )0x25c10000, sizeof( tails_tex ) );

  if ( slScrAutoDisp( NBG1ON ) == FALSE ) {
    slPrint( "Error!", slLocate( 5, 3 ) );
  }

  slSpriteType( 5 );
  slSpecialPrioModeNbg1( spPRI_Dot );
  slPriorityNbg1( 4 );
  slPrioritySpr5( 4 );
  slSpecialFuncCodeA( sfCOL_cd );
  slSpecialFuncSelectB( 0 );

  slZdspLevel( 7 );
  slTVOn();
  slPrint( "Sample Program", slLocate( 5, 2 ) );

  while( -1 ) {
    pad = Smpc_Peripheral[ 0 ].data;

    if ( pad & PER_DGT_KU ) {
       y -= UNIT;
    }
    if ( pad & PER_DGT_KD ) {
      y += UNIT;
    }
    if ( pad & PER_DGT_KL ) {
      x -= UNIT;
    }
    if ( pad & PER_DGT_KR ) {	
      x += UNIT;
    }
    slScrPosNbg1( x, y );

    slPutSprite( pos, &tails_atr, DEGtoANG( 0.0 ) );
    slSynch();
  }
}
