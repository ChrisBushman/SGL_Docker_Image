#include "sgl.h"

extern Uint32 water[];

void  ss_main() {
  Uint16 i;
  Uint32 vceltbl[ 40 ];
  Uint32 linetbl[ 224 * 2 ];
  Uint32 a, b, c, d = 0;
  slInitSystem( TV_320x224, NULL, 1 );

  slTVOff();
  {
    /* Make vertical cell scroll table */
    for ( i = 0 ; i < 40 ; ++i ) {
      vceltbl[ i ] = 0x400 * i;
    }
    /* Make line scroll table */
    for ( i = 0 ; i < 224 ; ++i ) {
      linetbl[ i * 2 ] = ( 0x10000 * 320 * i ) & 0x1ff0000; 
      a = ( 0xa000 * i ) & 0x1ff0000;
      b = 64 * ( a / 0x10000 + 1 );
      c = ( 320 / 8 ) * ( i + 1 );
      linetbl[ i * 2 + 1 ] = a + ( 0x400 * ( 64 - ( ( b >= c ) ? 64 : b - d ) ) );
      d = c;
    }
    /* Transfer datas */
    slDMACopy( water, ( void * )VDP2_VRAM_A0, 320 * 224 * 4 );
    slDMACopy( vceltbl, ( void * )( VDP2_VRAM_B1 + 0x10000 ), 40 * 4 );
    slDMACopy( linetbl, ( void * )( VDP2_VRAM_B1 + 0x18000 ), 224 * 8 );

    slVRAMMode( Div_B );

    slBitMapNbg0( COL_TYPE_1M, BM_512x256, ( void * )VDP2_VRAM_A0 );

    /* Set Cycle pattern and display enable bit */
    slScrAutoDisp( NBG0ON );
    slScrCycleSet( 0x44444444, 0x44444444, 0x44444444, 0xfceeeeee );

    /* Set Line/Vertical_Cell scroll mode */
    slLineScrollModeNbg0( lineSZ1 | lineHScroll | lineVScroll |VCellScroll );
    slVCellTable( ( void * )VDP2_VRAM_B1 + 0x10000 );
    slLineScrollTable0( ( void * )( VDP2_VRAM_B1 + 0x18000 ) );
  }
  slTVOn();

  while( -1 ) slSynch();
}

