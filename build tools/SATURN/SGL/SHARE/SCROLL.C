#include "scroll.h"

void PalleteSet( void *, Uint32, Uint16 ) __attribute__ ((section(".text")));
void PalleteSet( srcs, size, off )
     void   *srcs; /* Source data address.                */
     Uint32 size;  /* Data size.                          */
     Uint16 off;   /* Offset from Colour RAM (0x25f00000) */
{
    slDMACopy( srcs, ( void * )( VDP2_COLRAM  + off ), size ); 
}

void CellSet( void *, Uint32, Uint32 ) __attribute__ ((section(".text")));
void CellSet( srcs, size, off )
     void   *srcs;
     Uint32 size;
     Uint32 off;
{
    slDMACopy( srcs, ( void * )( VDP2_VRAM_A0 + off ), size ); 
}

void Map1WordSet( SCROLL, Uint32, Uint32 ) __attribute__ ((section(".text")));
void Map1WordSet( srcs, palet, off )
     SCROLL srcs;  /* Rosource Database. */
     Uint32 palet; /* Pallete offset.    */
     Uint32 off;   /* Cel offset.        */
{
  Uint16 i, j = 0, k = 0;
  Uint16 chrno;
  Uint16 src;
  Uint16 *dest;
  register Uint16 mask;

  dest = ( Uint16 * )( VDP2_VRAM_A0 + srcs.MapOffset );
  /* First Step. */
  if ( ( srcs.CHAR_SIZE & 0x0001 ) == CHAR_SIZE_1x1 ) off >>= 5;
  else off >>= 7;
  
  if ( srcs.DATA_TYPE & CN_12BIT ) mask = 0x0fff;
  else mask = 0x03ff;
  
    for ( i = 0, j = 0, k = 0; i < srcs.MapSize >> 1 ; ++i, ++j, ++k ) {
    src = *( Uint16 * )( srcs.MapSrcs + i * 2 );

    /* Second Step. */
    chrno = ( src + off ) & mask;

    /* Third Step. */
    /* Joint pallete section and Direction section. */
    chrno |= ( ( ( src & 0xf000 ) >> 12 ) + palet ) << 12;
    if ( !( srcs.DATA_TYPE & CN_12BIT ) ) {
      chrno |= src & 0xc000;
    }
    if ( j >= srcs.MapHSize ) {
      j = 64;
      if ( ( srcs.CHAR_SIZE & 0x0001 ) == CHAR_SIZE_2x2 ) j = 32;
      k += j - srcs.MapHSize;
      j = 0;
    }
    *( dest + k ) = chrno;
  } 
}

void Map2WordSet( SCROLL, Uint32, Uint32 ) __attribute__ ((section(".text")));
void Map2WordSet( srcs, palet, off )
     SCROLL srcs;
     Uint32 palet;
     Uint32 off;
{
  Uint16 i, j = 0, k = 0;
  Uint32 dir;
  Uint32 chrno;
  register Uint32 *dest;

  dest = ( Uint32 * )( srcs.MapOffset + VDP2_VRAM_A0 );

  /* If Pattern name bank is 2 Words. */
  off >>= 5;
  for ( i = 0, j = 0, k = 0 ; i < 2048 ; ++i, ++j, ++k ) {
    chrno = *( Uint32 * )( srcs.MapSrcs + i * 4 );
    palet = ( chrno & 0x007f0000 ) + ( palet << 16 );
    dir   = chrno & 0xf0000000;
    chrno = ( chrno & 0x00007fff ) + off;

    chrno |= palet | dir;
    if ( j >= srcs.MapHSize ) {
      j = 64;
      if ( ( srcs.CHAR_SIZE & 0x0001 ) == CHAR_SIZE_2x2 ) j = 32;
      k += j - srcs.MapHSize;
      j = 0;
    }
    *( dest + k ) = chrno;
  }
}

void MapSet( SCROLL ) __attribute__ ((section(".text")));
void MapSet( srcs )
     SCROLL srcs;
{
  Uint16 paloff;
  Uint32 offset;
  
  paloff = srcs.PalleteOffset >> 1;
  if ( srcs.CMode & 0x02 ) paloff >>= 1;
  switch ( srcs.CHAR_SIZE & 0x0070 ) {
  case COL_TYPE_16:
    paloff >>= 4;
    break;
  case COL_TYPE_256:
    paloff >>= 8;
    break;
  case COL_TYPE_2048:
    paloff >>= 11;
  }
  offset = srcs.CellOffset;// >> 1;
  
  if ( srcs.DATA_TYPE & PNB_1WORD ) Map1WordSet( srcs, paloff, offset );
  else Map2WordSet( srcs, paloff, offset );
}

void ScrollDataSet( SCROLL ) __attribute__ ((section(".text")));
void ScrollDataSet( srcs )
     SCROLL srcs; /* Scroll data transfer information table. */
{
#ifdef INITIALIZE
  static Uint8 colmode = 0;
#endif
  Uint32 i;
#ifdef INITIALIZE
  Uint32 *j;
#endif
  /* Set FUNCTION Table. */
  void ( *slCharNbg )( Uint16, Uint16 );
  void ( *slPageNbg )( void *, void *, Uint16 );
  void ( *slPlaneNbg )( Uint16 );
  void ( *slMapNbg )( void *, void *, void *, void * );
  
  void *slCharNBG[] = {
    slCharNbg0,
    slCharNbg1,
    slCharNbg2,
    slCharNbg3
  };

  void *slPageNBG[] = {
    slPageNbg0,
    slPageNbg1,
    slPageNbg2,
    slPageNbg3
  };
  
  void *slPlaneNBG[] = {
    slPlaneNbg0,
    slPlaneNbg1,
    slPlaneNbg2,
    slPlaneNbg3
  };
  
  void *slMapNBG[] = {
    slMapNbg0,
    slMapNbg1,
    slMapNbg2,
    slMapNbg3
  };

#ifdef INITIALIZE
  /* Set First time. */
  if ( colmode == 0 ) {
    // INITIALIZE
    j = ( Uint32 * )( VDP2_VRAM_A0 );
    for ( i = 0 ; i < 0x20000 ; ++i ) *( j + i ) = 0;
    slColRAMMode( srcs.CMode );
    /* Clear VRAM */
    slDMAXCopy( &colmode, ( void * )VDP2_VRAM_A0, 0x80000, Sfix_Dinc_Byte );
    colmode = 1;
  }
#endif

  /* Transfer routine call and transfer section. */
  /* Color data transfer. */
  if ( ( srcs.FLAGS & COL_TRNS ) )
    PalleteSet( srcs.PalleteSrcs, srcs.PalleteSize, srcs.PalleteOffset );

  /* Cel data transfer. */
  if ( ( srcs.FLAGS & CEL_TRNS ) )
    CellSet( srcs.CellSrcs, srcs.CellSize, srcs.CellOffset );

  /* Pattern name data transfer. */
  if ( ( srcs.FLAGS & MAP_TRNS ) )
    MapSet( srcs );

  /* Register Set */

  i = srcs.FLAGS >> 3;
  /* slCharNbg */
  slCharNbg = slCharNBG[ i ];
  ( *slCharNbg )( srcs.CHAR_SIZE & 0x0070, srcs.CHAR_SIZE & 0x0001 );
  /* sl PageNbg */
  slPageNbg = slPageNBG[ i ];
  ( *slPageNbg )( ( void * )( VDP2_VRAM_A0 + srcs.CellOffset ), 
		 ( void * )( srcs.PalleteOffset >> 1 ), srcs.DATA_TYPE );
  /* slPlaneNbg */
  slPlaneNbg = slPlaneNBG[ i ];
  ( *slPlaneNbg )( srcs.PLANE_SIZE );
  /* slMapNbg */
  slMapNbg = slMapNBG[ i ];
  ( *slMapNbg )( ( void * )( VDP2_VRAM_A0 + srcs.MapOffset ),
		( void * )( VDP2_VRAM_A0 + srcs.MapOffset ),
		( void * )( VDP2_VRAM_A0 + srcs.MapOffset ),
		( void * )( VDP2_VRAM_A0 + srcs.MapOffset ) );
}


