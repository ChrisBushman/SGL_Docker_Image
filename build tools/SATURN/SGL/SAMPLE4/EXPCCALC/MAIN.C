/*
   Expanded Color Calc Sample Program
                                    1997 Tany
*/
#include "sgl.h"

/* Import external symbols. */
extern Uint8 cel_sample[];
extern Uint32 map_sample[];
extern Uint16 pal_sample[];
extern Uint8 am2_cel[];
extern Uint16 am2_map[];
extern Uint16 am2_pal[];
extern Uint8 yama_cel[];
extern Uint16 yama_map[];
extern Uint16 yama_pal[];

extern void Cel2VRAM( Uint8 *, void *, Uint32 );
extern void Map2VRAM( Uint16 *, void *, Uint16, Uint16, Uint16, Uint32 );
extern void Map2VRAM2( Uint32 *, void *, Uint16, Uint16, Uint16, Uint32 );
extern void Pal2CRAM( Uint16 *, void *, Uint32 );

/* Defines */
#define NBG1_CEL_ADR ( void * )( VDP2_VRAM_A1 )
#define NBG1_MAP_ADR ( void * )( VDP2_VRAM_A0 )
#define NBG1_PAL_ADR ( VDP2_COLRAM )

#define NBG2_CEL_ADR ( void * )( VDP2_VRAM_B0 )
#define NBG2_MAP_ADR ( void * )( VDP2_VRAM_A0 + 0x4000 )
#define NBG2_PAL_ADR ( VDP2_COLRAM + 0x400 )

#define NBG3_CEL_ADR ( void * )( VDP2_VRAM_B0 + 0x8000 )
#define NBG3_MAP_ADR ( void * )( VDP2_VRAM_A0 + 0x8000 )
#define NBG3_PAL_ADR ( VDP2_COLRAM + 0x200 )

#define UNIT toFIXED( 1.0 );

/* Color rate table */
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
ss_main() {
  FIXED  x[ 3 ], y[ 3 ]; /* Scroll positions.      */
  Uint16 pad;            /* Pad Data               */
  Sint8  rate_a = 12;    /* Color rate about NBG 1 */
  Sint8  rate_b = 10;    /* Color rate about NBG 3 */
  Uint32 i = 0;

  slInitSystem( TV_320x240, NULL, 1 );

  slTVOff();
  {
    slColRAMMode( CRM16_1024 );
    
    /* Clear VDP2 VRAM */
    slDMAXCopy( &i, ( void * )VDP2_VRAM_A0, 0x20000, Sfix_Dinc_Long ); 

    /* Set NBG1 Parameter */
    slCharNbg1( COL_TYPE_256, CHAR_SIZE_1x1 );
    slPageNbg1( NBG1_CEL_ADR, ( void * )( NBG1_PAL_ADR ),
	       PNB_1WORD | CN_12BIT );
    slPlaneNbg1( PL_SIZE_1x1 );
    slMapNbg1( NBG1_MAP_ADR, NBG1_MAP_ADR, NBG1_MAP_ADR, NBG1_MAP_ADR );

    /* Set NBG2 Parameter */
    slCharNbg2( COL_TYPE_256, CHAR_SIZE_2x2 );
    slPageNbg2( NBG2_CEL_ADR, ( void * )( NBG2_PAL_ADR ), PNB_2WORD );
    slPlaneNbg2( PL_SIZE_2x2 );
    slMapNbg2( NBG2_MAP_ADR, NBG2_MAP_ADR, NBG2_MAP_ADR, NBG2_MAP_ADR );

    /* Set NBG3 Parameter */
    slCharNbg3( COL_TYPE_256, CHAR_SIZE_1x1 );
    slPageNbg3( NBG3_CEL_ADR, ( void * )( NBG3_PAL_ADR ),
	       PNB_1WORD | CN_12BIT );
    slPlaneNbg3( PL_SIZE_1x1 );
    slMapNbg3( NBG3_MAP_ADR, NBG3_MAP_ADR, NBG3_MAP_ADR, NBG3_MAP_ADR );
    slScrTransparent( NBG3ON );

    /* Data tansfer of NBG1 */
    Cel2VRAM( am2_cel, NBG1_CEL_ADR, 16000 );
    Map2VRAM( am2_map, NBG1_MAP_ADR, 32, 32, 0, 0 );
    Pal2CRAM( am2_pal, ( void * )NBG1_PAL_ADR, 256 );

    /* Data tansfer of NBG2 */
    Cel2VRAM( cel_sample, NBG2_CEL_ADR, 7168 );
    Map2VRAM2( map_sample, NBG2_MAP_ADR, 64, 64, 2, 0x40000 >> 5 );
    Pal2CRAM( pal_sample, ( void * )NBG2_PAL_ADR, 256 );

    /* Data tansfer of NBG3 */
    Cel2VRAM( yama_cel, NBG3_CEL_ADR, 31808 );
    Map2VRAM( yama_map, NBG3_MAP_ADR, 32, 16, 1, 0x8000 >> 5 );
    Pal2CRAM( yama_pal, ( void * )NBG3_PAL_ADR, 256 );

    /* Set Cycle pattern and enable display scroll */
    if ( slScrAutoDisp( NBG0ON | NBG1ON | NBG2ON | NBG3ON ) == NG ) {
      slScrAutoDisp( NBG0ON );
      slPrint( "Error!", slLocate( 5, 3 ) );
      while( -1 ) slSynch();
    }

    /* Set priority */
    slPriorityNbg0( 7 );
    slPriorityNbg1( 6 );
    slPriorityNbg3( 5 );
    slPriorityNbg2( 4 );

    /* Set parameters about color colculation. */
    slColorCalc( CC_RATE | CC_TOP | NBG1ON | NBG3ON | CC_EXT );
    slColRateNbg1( rate_tbl[ rate_a ] );
    slColRateNbg3( rate_tbl[ rate_b ] ); 

    slPrint( "Expanded color calc Sample Program", slLocate( 2, 2 ) );
  }
  slTVOn();

  /* Initialize scroll position */
  x[ 0 ] = x[ 1 ] = x[ 2 ] = toFIXED( 0.0 );
  y[ 0 ] = y[ 1 ] = y[ 2 ] = toFIXED( 0.0 );
  while( -1 ) {
    pad = Smpc_Peripheral[ 0 ].data;

    /* Normal scroll 1 (AM#2) */
    if ( !( pad & PER_DGT_TA ) ) {
      if ( pad & PER_DGT_KU ) {
	y[ 0 ] -= UNIT;
      }
      if ( pad & PER_DGT_KD ) {
	y[ 0 ] += UNIT;
      }
      if ( pad & PER_DGT_KL ) {
	x[ 0 ] -= UNIT;
      }
      if ( pad & PER_DGT_KR ) {	
	x[ 0 ] += UNIT;
      }
      if ( pad & PER_DGT_TL ) {	
	if ( ++rate_a > 31 ) rate_a = 31;
      }
      if ( pad & PER_DGT_TR ) {	
	if ( --rate_a < 0 ) rate_a = 0;
      }      
    }
    /* Normal scroll 2 (2D Map) */
    if ( !( pad & PER_DGT_TB ) ) {
      if ( pad & PER_DGT_KU ) {
	y[ 1 ] -= UNIT;
      }
      if ( pad & PER_DGT_KD ) {
	y[ 1 ] += UNIT;
      }
      if ( pad & PER_DGT_KL ) {
	x[ 1 ] -= UNIT;
      }
      if ( pad & PER_DGT_KR ) {	
	x[ 1 ] += UNIT;
      }
    }
    /* Normal scroll 3 (Mountain) */
    if ( !( pad & PER_DGT_TC ) ) {
      if ( pad & PER_DGT_KU ) {
	y[ 2 ] -= UNIT;
      }
      if ( pad & PER_DGT_KD ) {
	y[ 2 ] += UNIT;
      }
      if ( pad & PER_DGT_KL ) {
	x[ 2 ] -= UNIT;
      }
      if ( pad & PER_DGT_KR ) {	
	x[ 2 ] += UNIT;
      }
      if ( pad & PER_DGT_TL ) {	
	if ( ++rate_b > 31 ) rate_b = 31;
      }
      if ( pad & PER_DGT_TR ) {	
	if ( --rate_b < 0 ) rate_b = 0;
      }
    }
    if ( !( pad & PER_DGT_ST ) ) {
      x[ 0 ] = x[ 1 ] = x[ 2 ] = toFIXED( 0.0 );
      y[ 0 ] = y[ 1 ] = y[ 2 ] = toFIXED( 0.0 );
      rate_a = 12;
      rate_b = 10;
    }

    slPrintHex( rate_a, slLocate( 18, 10 ) );
    slPrintHex( rate_b, slLocate( 18, 11 ) );
    slPrint( "Color Rate About NBG0 :", slLocate( 1, 10 ) );
    slPrint( "Color Rate About NBG3 :", slLocate( 1, 11 ) );
    if ( !( pad & PER_DGT_TL ) ) {
      slColorCalcMode( CC_RATE | CC_TOP | CC_EXT );
    }
    if ( !( pad & PER_DGT_TR ) ) {
      slColorCalcMode( CC_RATE | CC_TOP );
    }
    /* Set Color Rate */
    slColRateNbg1( rate_tbl[ rate_a ] );
    slColRateNbg3( rate_tbl[ rate_b ] ); 

    /* Set scroll position */
    slScrPosNbg1( x[ 0 ], y[ 0 ] );
    slScrPosNbg2( x[ 1 ], y[ 1 ] );
    slScrPosNbg3( x[ 2 ], y[ 2 ] );

    slSynch();
  }
}



