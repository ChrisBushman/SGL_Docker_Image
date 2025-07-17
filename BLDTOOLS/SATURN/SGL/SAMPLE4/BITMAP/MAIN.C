#include "sgl.h"
#include "../scroll/flower.pal"

#define UNIT toFIXED( 2.0 );

enum BMP_STATUS {
  BMP_PSET,
  BMP_CIRCLE,
  BMP_LINE,
  BMP_BOX,
  BMP_BOXFILL,
  BMP_PUT,
  BMP_SPRPUT
};

void ss_main()
{
  Sint16 rand[ 5 ];
  Uint16 pad;
  FIXED  x, y;
  
  x = y = toFIXED( 0.0 );

  slInitSystem(TV_320x224, NULL, 1);

  /* Set pallete */
  slDMACopy( pal_flower, ( void * )VDP2_COLRAM, 256 * 2 );

  /* Initialize bitmap functions. */
  if ( slInitBitMap( bmNBG1, BM_512x256, ( void * )0x25e20000 ) == FALSE ) {
    slPrint( "FALSE", slLocate( 10, 3 ) );
    exit( 0 );
  }

  /* Display NBG */
  slScrAutoDisp( NBG0ON | NBG1ON );

  slPrint( "Bitmap Sample", slLocate( 9, 2 ) );

  /* Select random value */
  slPrint( "Press Start button", slLocate( 10, 13 ) );
  while ( ( Smpc_Peripheral[ 0 ].data & PER_DGT_ST ) != 0 ) slSynch();
  slPrint( "                 ", slLocate( 10, 13 ) );

  /* Main looooop! */
  while( -1 ) {
    pad = Smpc_Peripheral[ 0 ].data;

    /* Set random value */
    rand[ 0 ] = ( Sint16 )( slRandom() & 0x000001ff ) - 256;
    rand[ 1 ] = ( Sint16 )( slRandom() & 0x000000ff ) - 128;
    rand[ 2 ] = ( Sint16 )( slRandom() & 0x000001ff ) - 256;
    rand[ 3 ] = ( Sint16 )( slRandom() & 0x000000ff ) - 128;
    rand[ 4 ] = ( Sint16 )( slRandom() & 0x000000ff );

    /* Pad control */
    if ( !( pad & PER_DGT_KU ) ) {
      y += UNIT;
    }
    if ( !( pad & PER_DGT_KD ) ) {
      y -= UNIT;
    }
    if ( !( pad & PER_DGT_KL ) ) {
      x += UNIT;
    }
    if ( !( pad & PER_DGT_KR ) ) {
      x -= UNIT;
    }
    if ( !( pad & PER_DGT_ST ) ) {
      x = y = toFIXED ( 0.0 );
    }

    /* Switch any bitmap functions. */
    switch (  slRandom() & 0x00000007 ) {
    case BMP_PSET:
      slBMPset( rand[ 0 ], rand[ 1 ], rand[ 4 ] );
      break;
    case BMP_CIRCLE:
      slBMCircle( rand[ 0 ], rand[ 1 ],  rand[ 3 ], rand[ 4 ] );
      break;
    case BMP_LINE:
      slBMLine( rand[ 0 ], rand[ 1 ],  rand[ 2 ], rand[ 3 ], rand[ 4 ] );
      break;
    case BMP_BOX:
      slBMBox( rand[ 0 ], rand[ 1 ],  rand[ 2 ], rand[ 3 ], rand[ 4 ] );
      break;
    case BMP_BOXFILL:
      slBMBoxFill( rand[ 0 ], rand[ 1 ],  rand[ 2 ], rand[ 3 ], rand[ 4 ] );
      break;
    case BMP_PUT:
      break;
    case BMP_SPRPUT:
      break;
    }
    slScrPosNbg1( x, y );
    slSynch();
  }
}







