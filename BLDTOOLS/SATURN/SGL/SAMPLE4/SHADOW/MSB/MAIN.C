/*
   MSB Shadow sample
                            1997. Tany
*/
#include "sgl.h"
#include "sprdata.h"

/* User defines */
#define TAILS    1
#define SHADOW_S 1.3

/* Import symbols. */ 
extern void    VDP1DataSet( void );
extern void    VDP2DataSet( void );
extern TEXTURE form_tbl[];

/* Initilize */
/* Position */
FIXED  possdw[ XYZS ] = {
  toFIXED( 0.0 ), toFIXED( 0.0 ), toFIXED( 120.0 ), toFIXED( 1.3 )
};
FIXED  postxr[ XYZS ] = {
  toFIXED( 0.0 ), toFIXED( 0.0 ), toFIXED( 120.0 ), toFIXED( ORIGINAL )
};

/* Sprite(Tails) attribute */
SPR_ATTR tails_atr = SPR_ATTRIBUTE( 0, 0 | ( 5 << 12 ) | ( 1 << 11 ),
				   No_Gouraud, CL256Bnk, sprNoflip | _ZmCC );
/* Shadow (of Tails) attribute */
SPR_ATTR shadow_atr = SPR_ATTRIBUTE( 0, 0 | ( 5 << 12 ) | ( 1 << 11 ),
				    No_Gouraud, CL256Bnk | MSBon,
				    sprNoflip | _ZmCC );

/* Main function */
void ss_main() {
  Uint16 pad;     /* Pad push data */
  Uint16 edg;     /* Pad edge data */
  Uint8  seq = 0; /* Sequence of display sprite */
  Uint8  tp = ON; /* Transparent shadow */
  
  slInitSystem( TV_320x224, form_tbl, 1 );
  slSpriteType( 5 );

  slTVOff();
  {
    /* Data Set */
    VDP1DataSet();
    VDP2DataSet();
    
    /* Priority set */
    slPriorityNbg0( 6 );
    slPrioritySpr5( 5 );
    slPriorityNbg1( 4 );
    
    slCurColor( 4 );
    /* Set sprite/polygon display level */
    slZdspLevel( 7 );
    
    /* Setting about VDP2 Scroll display */
    slScrTransparent( NBG1ON ); /* This picture uses pallete number 0 */
    slColRAMOffsetNbg1( 1 );
    
    /* Set cycle pattern */
    slScrAutoDisp( NBG0ON | NBG1ON );
    
    /* If you use sprite shadow, must set sprite mode to pallete only mode. */
    slSpriteColMode( SPR_PAL );
    
    /* Set Sprite shadow */
    slShadowOn( NBG1ON );
    
    slScrPosNbg1( toFIXED( 0.0 ), toFIXED( 0.0 ) );
    
    slPrint( "MSB Shadow Sample", slLocate( 5, 17 ) );
  }
  slTVOn();

  /* Main loop */
  while( -1 ) {
    slTpShadowMode( tp );
    pad = Smpc_Peripheral[ 0 ].data;
    edg = Smpc_Peripheral[ 0 ].push;

    /* Move sprite */
    if ( pad & PER_DGT_KU ) {
      postxr[ Y ] += toFIXED( TAILS );
      possdw[ Y ] += toFIXED( SHADOW_S );
    }
    if ( pad & PER_DGT_KD ) {
      postxr[ Y ] -= toFIXED( TAILS );
      possdw[ Y ] -= toFIXED( SHADOW_S );
    }
    if ( pad & PER_DGT_KL ) {
      postxr[ X ] += toFIXED( TAILS );
      possdw[ X ] += toFIXED( SHADOW_S );
    }
    if ( pad & PER_DGT_KR ) {
      postxr[ X ] -= toFIXED( TAILS );
      possdw[ X ] -= toFIXED( SHADOW_S );
    }
    /* Change mode */
    /* Change sequence */
    if ( !( edg & PER_DGT_TL ) ) {
      seq = 0;
    }
    if ( !( edg & PER_DGT_TR ) ) {
      seq = 1;
    }
    /* Change MSB ON (MSBon) bit */
    if ( !( edg & PER_DGT_TA ) ) {
      shadow_atr.atrb |= 0x8000;
    }
    if ( !( edg & PER_DGT_TB ) ) {
      shadow_atr.atrb &= 0x7fff;
    }
    if ( !( edg & PER_DGT_TC ) ) {
      if ( shadow_atr.atrb & 0x8000 ) {
	shadow_atr.atrb &= 0x7fff;
      } else {
	shadow_atr.atrb |= 0x8000;
      }
    }
    /* Change transparency mode */
    if ( !( edg & PER_DGT_TX ) ) {
      tp = ON;
    }
    if ( !( edg & PER_DGT_TY ) ) {
      tp = OFF;
    }
    if ( !( edg & PER_DGT_TZ ) ) {
      if ( tp == ON ) {
	tp = OFF;
      } else {
	tp = ON;
      }
    }
    /* Reset */
    if ( !( edg & PER_DGT_ST ) ) {
      seq = 0;
      tp  = ON;
      shadow_atr.atrb |= 0x8000;
      postxr[ X ] = toFIXED( 0.0 );
      possdw[ X ] = toFIXED( 0.0 );
      postxr[ Y ] = toFIXED( 0.0 );
      possdw[ Y ] = toFIXED( 0.0 );
    }
    
    /* Display Sprite/Shadow. */
    if ( seq == 0 ) {
      slDispSprite( possdw, &shadow_atr, DEGtoANG( 0.0 ) );      
      slDispSprite( postxr, &tails_atr, DEGtoANG( 0.0 ) );
    } else {
      slDispSprite( postxr, &tails_atr, DEGtoANG( 0.0 ) );
      slDispSprite( possdw, &shadow_atr, DEGtoANG( 0.0 ) );
    }

    slSynch();
    }
}
