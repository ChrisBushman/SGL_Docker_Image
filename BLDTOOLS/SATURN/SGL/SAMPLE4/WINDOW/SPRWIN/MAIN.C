#include "sgl.h"
#include "sprdata.h"

#define UNIT 2

/* Import symbols. */
extern void VDP1DataSet( void );
extern void VDP2DataSet( void );
extern TEXTURE form_tbl[];

extern Uint8 *VDP2_CCRSA;

/* Global variables */
FIXED  posbg[ XY ] = {
  toFIXED( 0.0 ), toFIXED( 0.0 )
};

FIXED  posbgold[ XY ] = {
  toFIXED( 0.0 ), toFIXED( 0.0 )
};

/* Sprite(Tails) attribute */
SPR_ATTR tails_atr = SPR_ATTRIBUTE( 0, 0 | ( 5 << 12 ) | ( 1 << 11 ),
				   No_Gouraud, CL256Bnk | MSBon,
				   sprNoflip | _ZmCC );

FIXED  postxr[ XYZS ] = {
  toFIXED( 0.0 ), toFIXED( 0.0 ), toFIXED( 120.0 ), toFIXED( ORIGINAL )
};

/* Main loop */
ss_main() {
  Uint16 pad;
  Sint8  rate = 0;
  Uint32 dumy = 0;

  slInitSystem( TV_320x224, form_tbl, 2 );
  slTVOff();
  {
    /* Set sprite type */
    slSpriteType( 5 );

    /* Data Set. */
    VDP1DataSet();
    VDP2DataSet();

    /* Priority set. */
    slPrioritySpr5( 6 );
    slPriorityNbg2( 5 );
    slPriorityNbg1( 4 );

    slScrTransparent( NBG1ON );
    slScrTransparent( NBG2ON );

    /* Set display level. */
    slZdspLevel( 7 );

    slColRAMOffsetNbg2( 1 );

    /* Set cycle pattern and debug display. */
    slScrAutoDisp( NBG1ON | NBG2ON );

    /* Set parameters about sprite window */
    slSpriteColMode( SPR_PAL );
    slSpriteWinMode( ON );
    slScrWindowModeNbg1( spw_OUT );
    slScrWindowModeNbg2( spw_IN );
  }
  slTVOn();

  /* Main loop. */
  while( -1 ) {

    pad = Smpc_Peripheral[ 0 ].data;

    /* Moving Sprite. */
    if ( pad & PER_DGT_KU ) {
      postxr[ Y ] += toFIXED( UNIT );
    }
    if ( pad & PER_DGT_KD ) {
      postxr[ Y ] -= toFIXED( UNIT );
    }
    if ( pad & PER_DGT_KL ) {
      postxr[ X ] += toFIXED( UNIT );
    }
    if ( pad & PER_DGT_KR ) {
      postxr[ X ] -= toFIXED( UNIT );
    }
    if ( !( pad & PER_DGT_TL ) ) {
      slScrWindowModeNbg1( spw_OUT );
      slScrWindowModeNbg2( spw_IN );
    }
    if ( !( pad & PER_DGT_TR ) ) {
      slScrWindowModeNbg1( spw_IN );
      slScrWindowModeNbg2( spw_OUT );
    }

    /* Displaying Sprites/Polygons. */
    slDispSprite( postxr, &tails_atr, DEGtoANG( 0.0 ) );

    /* Synching. */
    slSynch();
  }
}
