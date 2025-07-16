/*
   Colour Calculation Sprite and Scroll
*/
#include "sgl.h"
#include "../../scroll/sample.cel"
#include "../../scroll/sample.map"
#include "../../scroll/sample.pal"
#include "../../sprite/tails.pl"
#include "../../sprite/tails.tex"

/* Defines */
#define NBG1_CEL_ADR   VDP2_VRAM_A0
#define NBG1_MAP_ADR   VDP2_VRAM_A0 + 0x10000
#define NBG1_PAL_ADR   VDP2_COLRAM

#define UNIT toFIXED( 1.0 );
#define FIXED0 toFIXED( 0.0 )

/* Global variables */
/* Data table about sprite */
TEXTURE formtbl = TEXTBL( 96, 96, 0x10000 );
SPR_ATTR tails_atr = SPR_ATTRIBUTE( 0, 0x100 | ( 5 << 12 ) | ( 1 << 11 ),
				   No_Gouraud, CL256Bnk, sprNoflip | _ZmCC );
FIXED pos[] = { FIXED0, FIXED0, toFIXED( 150.0 ), toFIXED( ORIGINAL) };

/* Color calc rate */
Uint16 rate_tbl[] = {
  CLRate31_1,
  CLRate30_2,
  CLRate29_3,
  CLRate28_4,
  CLRate27_5,
  CLRate26_6,
  CLRate25_7,
  CLRate24_8,
  CLRate23_9,
  CLRate22_10,
  CLRate21_11,
  CLRate20_12,
  CLRate19_13,
  CLRate18_14,
  CLRate17_15,
  CLRate16_16,
  CLRate15_17,
  CLRate14_18,
  CLRate13_19,
  CLRate12_20,
  CLRate11_21,
  CLRate10_22,
  CLRate9_23,
  CLRate8_24,
  CLRate7_25,
  CLRate6_26,
  CLRate5_27,
  CLRate4_28,
  CLRate3_29,
  CLRate2_30,
  CLRate1_31,
  CLRate0_32,
};

/* Main function */
void ss_main() {
  FIXED  x = 0, y = 0;
  Uint16 pad;
  Sint8  rate = 10;
  
  slInitSystem( TV_320x240, &formtbl, 1 );

  slTVOff();
  {
    slColRAMMode( CRM16_1024 );

    /* Set data and parameter about NBG1 */
    slCharNbg1( COL_TYPE_256, CHAR_SIZE_2x2 );
    slPageNbg1( ( void * )NBG1_CEL_ADR, ( void * )NBG1_PAL_ADR, PNB_2WORD );
    slPlaneNbg1( PL_SIZE_1x1 );
    slMapNbg1( ( void * )NBG1_MAP_ADR, ( void * )NBG1_MAP_ADR,
	      ( void * )NBG1_MAP_ADR, ( void * )NBG1_MAP_ADR );
    
    slDMACopy( cel_sample, ( void * )NBG1_CEL_ADR, sizeof( cel_sample ) );
    slDMACopy( map_sample, ( void * )NBG1_MAP_ADR, sizeof( map_sample ) );
    slDMACopy( pal_sample, ( void * )NBG1_PAL_ADR, sizeof( pal_sample ) );

    /* Set sprite data */
    slDMACopy( tails_pl, ( void * )0x25f00200, sizeof( tails_pl ) );
    slDMACopy( tails_tex, ( void * )0x25c10000, sizeof( tails_tex ) );

    slScrAutoDisp( NBG0ON | NBG1ON );

    /* Set Sprite color color calc parameter */
    slSpriteType( 5 );
    slSpriteCCalcCond( CC_PR_CN );
    slSpriteCCalcNum( 6 );
    slColorCalc( CC_RATE | CC_TOP | NBG1ON | SPRON );
    slColRateSpr1( rate_tbl[ rate ] );
    slPriorityNbg1( 5 );
    slPrioritySpr5( 6 );

    slZdspLevel( 7 );
    slPrint( "Sprite color calc ", slLocate( 5, 2 ) );
  }
  slTVOn();

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
    if ( !( pad & PER_DGT_TL ) ) {
      if ( ++rate > 31 ) rate = 31;
    }
    if ( !( pad & PER_DGT_TR ) ) {
      if ( --rate < 0 ) rate = 0;
    }

    slColRateSpr1( rate_tbl[ rate ] );
    slScrPosNbg1( x, y );

    slPutSprite( pos, &tails_atr, DEGtoANG( 0.0 ) );
    slSynch();
  }
}
