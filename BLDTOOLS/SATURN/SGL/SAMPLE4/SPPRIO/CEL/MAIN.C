/*
*/
#include "sgl.h"
#include "../../scroll/sample.cel"
#include "../../scroll/sample.map"
#include "../../scroll/sample.pal"
#include "../../sprite/tails.pl"
#include "../../sprite/tails.tex"

#define NBG1_CEL_ADR  VDP2_VRAM_A0
#define NBG1_MAP_ADR  VDP2_VRAM_A1
#define NBG1_PAL_ADR  VDP2_COLRAM

#define SPR_TXR_ADR  SpriteVRAM + CGADDRESS
#define SPR_TXR_PAL  VDP2_COLRAM + 0x200

#define UNIT toFIXED( 1.0 )
#define FIXED0 toFIXED( 0.0 )

TEXTURE formtbl = TEXTBL( 96, 96, 0x10000 );
SPR_ATTR tails_atr = SPR_ATTRIBUTE( 0, 0x100 | ( 5 << 12 ) | ( 1 << 11 ),
				   No_Gouraud, CL256Bnk, sprNoflip | _ZmCC );
FIXED pos[] = { FIXED0, FIXED0, toFIXED( 150.0 ), toFIXED( ORIGINAL ) };

void ss_main() {
  FIXED  x = 0, y = 0;
  Uint16 pad;

  slInitSystem( TV_320x240, &formtbl, 1 );

  slTVOff();

  slColRAMMode( CRM16_1024 );
  
  slCharNbg1( COL_TYPE_256, CHAR_SIZE_2x2 );
  slPageNbg1( ( void * )NBG1_CEL_ADR, ( void * )NBG1_PAL_ADR, PNB_2WORD );
  slPlaneNbg1( PL_SIZE_1x1 );
  slMapNbg1( ( void * )NBG1_MAP_ADR, ( void * )NBG1_MAP_ADR,
	    ( void * )NBG1_MAP_ADR, ( void * )NBG1_MAP_ADR );

  slDMACopy( cel_sample, ( void * )NBG1_CEL_ADR, sizeof( cel_sample ) );
  slDMACopy( map_sample, ( void * )NBG1_MAP_ADR, sizeof( map_sample ) );
  slDMACopy( pal_sample, ( void * )NBG1_PAL_ADR, sizeof( pal_sample ) );

  slDMACopy( tails_pl, ( void * )SPR_TXR_PAL, sizeof( tails_pl ) );
  slDMACopy( tails_tex, ( void * )SPR_TXR_ADR, sizeof( tails_tex ) );

  if ( slScrAutoDisp( NBG1ON ) == FALSE ) {
    slPrint( "Error!", slLocate( 5, 3 ) );
  }

  slSpriteType( 5 );
  slSpecialPrioModeNbg1( spPRI_Char );
  slSpecialPrioBitNbg1( ON );
  slPriorityNbg1( 6 );
  slPrioritySpr5( 6 );
  slZdspLevel( 7 );

  slTVOn();
  slPrint( "Sample Program", slLocate( 5, 2 ) );

  while( -1 ) {
    pad = Smpc_Peripheral[ 0 ].data;

    if ( pad & PER_DGT_KU ) {
      y += UNIT;
    }
    if ( pad & PER_DGT_KD ) {
      y -= UNIT;
    }
    if ( pad & PER_DGT_KL ) {
      x += UNIT;
    }
    if ( pad & PER_DGT_KR ) {	
      x -= UNIT;
    }
    slScrPosNbg1( x, y );

    slPutSprite( pos, &tails_atr, DEGtoANG( 0.0 ) );
    slSynch();
  }
}


