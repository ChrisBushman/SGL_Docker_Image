/*
   16 Color and Gradation sample
*/
#include "sgl.h"
#include "../scroll/pine.map"
#include "../scroll/pine.pal"
#include "../scroll/pine.cel"

#define NBG1_CEL_ADR ( VDP2_VRAM_A0 )
#define NBG1_MAP_ADR ( VDP2_VRAM_B0 )
#define NBG1_PAL_ADR ( VDP2_COLRAM + 0x7c0 )

/*
   カラー演算比率のテーブル
*/
Uint16 rate[] = {
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

/*
   マップ転送
*/
void map_trans( Uint16 *src, void *dest, Uint16 x, Uint16 y, Uint32 col ) {
  int i, j;
  Uint16 *adr;
  
  adr = ( Uint16 * )dest;
  
  for ( i = 0 ; i < y ; ++i ) {
    for ( j = 0 ; j < x ; ++j ) {
      *adr = ( ( ( *( Uint16 * )src ) ) | ( ( col & 0x000f ) << 12 ) );
      ++adr;
      ++src;
    }
    adr += ( 64 - j );
  }
}

/*
   メイン
*/
void ss_main() {
  Uint16 pad;
  Sint8  grad = 0;

  slInitSystem( TV_320x240, NULL, 1 );

  slTVOff();

  slColRAMMode( CRM16_1024 );

  /* NBG1の設定 */
  slCharNbg1( COL_TYPE_16, CHAR_SIZE_2x2 );
  slPageNbg1( 0, ( void * )NBG1_PAL_ADR, PNB_1WORD | CN_12BIT );
  slPlaneNbg1( PL_SIZE_1x1 );
  slMapNbg1( ( void * )0x40000, ( void * )0x40000, ( void * )0x40000,
	    ( void * )0x40000 );
  slScrTransparent( NBG1ON ); /* この絵はパレット番号0番を使用しています。 */

  /* データの転送 */
  slDMACopy( cel_pine, ( void * )NBG1_CEL_ADR, sizeof( cel_pine ) );
  slDMACopy( pal_pine, ( void * )NBG1_PAL_ADR, sizeof( pal_pine ) );
  map_trans( map_pine, ( void * )NBG1_MAP_ADR, 64, 64, ( 0x7c0 >> 5 ) );

  slScrAutoDisp( NBG0ON | NBG1ON );

  /* ぼかし及びカラー演算の設定 */
  slGradationOn( grdNBG1 );
  slColorCalc( CC_RATE | CC_TOP | NBG1ON );
  slColRateNbg1( CLRate0_32 );

  slPrint( "16 Colour Sample", slLocate( 10, 2 ) );
  slTVOn();

  while( -1 ) {
    pad = Smpc_Peripheral[ 0 ].data;
    if ( !( pad & PER_DGT_TL ) ) {
      slGradationOn( grdNBG1 );
      slColRateNbg1( rate[ grad ] );
    }
    if ( !( pad & PER_DGT_TR ) ) {
      slGradationOff();
      grad = 0;
      slColRateNbg1( rate[ grad ] );
    }
    if ( !( pad & PER_DGT_KL ) ) {
      if ( ++grad > 31 ) grad = 31;
      slColRateNbg1( rate[ grad ] );
    }
    if ( !( pad & PER_DGT_KR ) ) {
      if ( --grad < 0 ) grad = 0;
      slColRateNbg1( rate[ grad ] );
    }
    
    slSynch();
  } 
}
