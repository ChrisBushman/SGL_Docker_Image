#include "sgl.h"
#include "sprdata.h"

#define REAL_S   1
#define SHADOW_S 2

/* Import symbols. */
extern void VDP1DataSet( void );
extern void VDP2DataSet( void );
extern TEXTURE form_tbl[];

extern Uint8 *VDP2_CCRSA;

FIXED  posbg[ XY ] = {
  toFIXED( 0.0 ), toFIXED( 0.0 )
};

FIXED  posbgold[ XY ] = {
  toFIXED( 0.0 ), toFIXED( 0.0 )
};


SPRITE sonic_atr = {
  FUNC_Sprite,                           
  0,                                     
  ECdis | SPenb | CL16Look | CL_Trans,   
  ( 0x1b000 >> 3 ),                      
  ( 0x10000 >> 3 ),                      
  ( ( 64 / 8 ) << 8 | 96 ),              
  0 - 32,                                
  0 - 48,                                
  64 - 32,                               
  0 - 48,                                
  64 - 32,                               
  96 - 48,                               
  0 - 32,                                
  96 - 48,                               
  0,                                     
  0                                      
};

SPRITE shadow_atr = {
  FUNC_Sprite,                           
  0,                                     
  ECdis | SPenb | CL16Bnk | CL_Trans,    
  0x7ff0,                                
  ( 0x10c00 >> 3 ),                      
  ( ( 64 / 8 ) << 8 | 96 ),              
  0 - 32,                                
  0 - 48,                                
  64 - 32,                               
  0 - 48,                                
  64 - 32,                               
  96 - 48,                               
  0 - 32,                                
  96 - 48,                               
  0,                                     
  0                                      
};

ss_main() {
  Uint16 pad;
  Sint8  rate = 0;

  slInitSystem( TV_320x224, NULL, 2 );
  slSpriteType( 5 );

  slTVOff();

  /* Data Set. */
  VDP1DataSet();
  VDP2DataSet();
  slScrTransparent( NBG2ON );

  /* Priority set. */
  slPrioritySpr0( 6 );
  slPriorityNbg2( 1 );

  /* Set display level. */
  slZdspLevel( 7 );

  slColRAMOffsetNbg2( 1 );
  /* Set cycle pattern and debug display. */
  slScrAutoDisp( NBG2ON );
 
  slTVOn();

  slShadowOn( NBG2ON );
  slScrPosNbg2( toFIXED( 0.0 ), toFIXED( 0.0 ) );
  /* Main loop. */
  while( -1 ) {
    /* Moving Scroll. */
    pad = Smpc_Peripheral[ 0 ].data;

    /* Moving Sprite. */
    if ( pad & PER_DGT_KU ) {
      sonic_atr.YA += REAL_S;
      sonic_atr.YB += REAL_S;
      sonic_atr.YC += REAL_S;
      sonic_atr.YD += REAL_S;
      shadow_atr.YA += SHADOW_S;
      shadow_atr.YB += SHADOW_S;
      shadow_atr.YC += SHADOW_S;
      shadow_atr.YD += SHADOW_S;
    }
    if ( pad & PER_DGT_KD ) {
      sonic_atr.YA -= REAL_S;
      sonic_atr.YB -= REAL_S;
      sonic_atr.YC -= REAL_S;
      sonic_atr.YD -= REAL_S;
      shadow_atr.YA -= SHADOW_S;
      shadow_atr.YB -= SHADOW_S;
      shadow_atr.YC -= SHADOW_S;
      shadow_atr.YD -= SHADOW_S;
    }
    if ( pad & PER_DGT_KL ) {
      sonic_atr.XA += REAL_S;
      sonic_atr.XB += REAL_S;
      sonic_atr.XC += REAL_S;
      sonic_atr.XD += REAL_S;
      shadow_atr.XA += SHADOW_S;
      shadow_atr.XB += SHADOW_S;
      shadow_atr.XC += SHADOW_S;
      shadow_atr.XD += SHADOW_S;
    }
    if ( pad & PER_DGT_KR ) {
      sonic_atr.XA -= REAL_S;
      sonic_atr.XB -= REAL_S;
      sonic_atr.XC -= REAL_S;
      sonic_atr.XD -= REAL_S;
      shadow_atr.XA -= SHADOW_S;
      shadow_atr.XB -= SHADOW_S;
      shadow_atr.XC -= SHADOW_S;
      shadow_atr.XD -= SHADOW_S;
    }

    /* Displaying Sprites/Polygons. */
    slSetSprite( &shadow_atr, toFIXED( 110.0 ) );
    slSetSprite( &sonic_atr, toFIXED( 100.0 ) );

    /* Synching. */
    slSynch();
  }
}






