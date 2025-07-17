/*
   回転BGにラインカラーを掛けるサンプル。
   SGLのサンプル、8.9.2を元に改造。
                            1997.4.7 Tany
*/
#include	"sgl.h"
#include	"ss_scrol.h"

#define		RBG0RB_CEL_ADR			(VDP2_VRAM_A0            )
#define		RBG0RB_MAP_ADR			(VDP2_VRAM_B0            )
#define		RBG0RB_COL_ADR			(VDP2_COLRAM    + 0x00200)
#define		RBG0RA_CEL_ADR			(RBG0RB_CEL_ADR + 0x06e80)
#define		RBG0RA_MAP_ADR			(RBG0RB_MAP_ADR + 0x02000)
#define		RBG0RA_COL_ADR			(RBG0RB_COL_ADR + 0x00200)
#define		RBG0_KTB_ADR			(VDP2_VRAM_A1            )
#define		RBG0_PRA_ADR			(VDP2_VRAM_A1   + 0x1fe00)
#define		RBG0_PRB_ADR			(RBG0_PRA_ADR   + 0x00080)
#define		BACK_COL_ADR			(VDP2_VRAM_A1   + 0x1fffe)

extern Uint8 SynchConst;

/* ラインカラーのためのパレットテーブル
   この値は、
              ( z * 3.46 / 128 )
        k = e                    - 1
	RGB = k << 10 | k << 5 | k
   の式で作成されたものです。
   このあたりは、好みによって分かれる所でしょう。
*/

Uint16 Col[ 127 ] = {
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
  0x0000, 0x0000, 0x0421, 0x0421, 0x0421, 0x0421, 0x0421, 0x0421, 
  0x0421, 0x0421, 0x0421, 0x0421, 0x0421, 0x0421, 0x0421, 0x0421, 
  0x0421, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 0x0842, 
  0x0842, 0x0842, 0x0842, 0x0842, 0x0c63, 0x0c63, 0x0c63, 0x0c63, 
  0x0c63, 0x0c63, 0x0c63, 0x0c63, 0x1084, 0x1084, 0x1084, 0x1084, 
  0x1084, 0x1084, 0x1084, 0x14a5, 0x14a5, 0x14a5, 0x14a5, 0x14a5, 
  0x18c6, 0x18c6, 0x18c6, 0x18c6, 0x18c6, 0x1ce7, 0x1ce7, 0x1ce7, 
  0x1ce7, 0x1ce7, 0x2108, 0x2108, 0x2108, 0x2108, 0x2529, 0x2529, 
  0x2529, 0x294a, 0x294a, 0x294a, 0x2d6b, 0x2d6b, 0x2d6b, 0x318c, 
  0x318c, 0x318c, 0x35ad, 0x35ad, 0x35ad, 0x39ce, 0x39ce, 0x3def, 
  0x3def, 0x4210, 0x4210, 0x4631, 0x4631, 0x4a52, 0x4a52, 0x4e73, 
  0x4e73, 0x5294, 0x5294, 0x56b5, 0x5ad6, 0x5ad6, 0x5ef7, 0x5ef7, 
  0x6318, 0x6739, 0x6b5a, 0x6b5a, 0x6f7b, 0x739c, 0x77bd
};

Uint16 Col2[ 256 ];

void ss_main()
{
  FIXED	posy = toFIXED(0.0);
  ANGLE	angz = DEGtoANG(0.0);
  ANGLE	angz_up = DEGtoANG(0.0);
  Uint16 i, *j;

  /* カラーRAMを初期化 */
  j = ( Uint16 * )0x25f00000;
  for ( i = 0 ; i < 1024 ; ++i ) {
    *j++ = 0x8000;
  }

  slInitSystem(TV_320x224,NULL,1);
  slTVOff();

  /* 回転面にラインカラーを施す場合ラインカラーデータの8～11ビットまでしか
     見ないのでこれでいいのだ。*/
  for( i = 0 ; i < 256 ; ++i ) {
    Col2[ i ] = ( 0x600 / 2 );
  }

  /* ラインカラーテーブルの転送。 */
  slDMACopy( ( void * )Col2, ( void * )0x25e70000, sizeof( Col2 ) ); 
  /* パレットデータの転送。
     127ラインが、係数テーブルAとBの境目になる。Bには、ラインカラーを掛けない
     ので、係数テーブルAの領域にのみパレットを転送すればいい。
  */
  slDMACopy( ( void * )Col, ( void * )0x25f00600, sizeof( Col ) );

  /* カラー演算のための設定。
     TOP画面でレートを指定し、ラインカラー画面との和をとる
     カラーテーブルはVDP2のVRAM-B1領域中の25E70000Hにおく。
  */
  slColorCalc( CC_ADD | CC_TOP | RBG0ON );
  slLineColTable( ( void * )0x25e70000 );
  slLineColDisp( RBG0ON );
  slColRateLNCL( CLRate25_7 );

  slPrint( "Color calculate on Rotate BG", slLocate( 2, 2 ) );

  slColRAMMode( CRM16_1024 );

  slRparaInitSet( ( void * )RBG0_PRA_ADR );
  slMakeKtable( ( void * )RBG0_KTB_ADR );
  slCharRbg0( COL_TYPE_256, CHAR_SIZE_1x1 );
  slPageRbg0( ( void * )RBG0RB_CEL_ADR, 0, PNB_1WORD | CN_12BIT );
  slPlaneRA( PL_SIZE_1x1 );
  sl1MapRA( ( void * )RBG0RA_MAP_ADR );
  slOverRA( 0 );
  slKtableRA( (void *)RBG0_KTB_ADR, K_FIX | K_DOT | K_2WORD | K_ON | K_LINECOL);
  Cel2VRAM( tuti_cel, ( void * )RBG0RA_CEL_ADR, 65536 );
  Map2VRAM( tuti_map, ( void * )RBG0RA_MAP_ADR, 64, 64, 2, 884 );
  Pal2CRAM( tuti_pal, ( void * )RBG0RA_COL_ADR, 160 );

  slPlaneRB( PL_SIZE_1x1 );
  sl1MapRB( ( void * )RBG0RB_MAP_ADR );
  slOverRB( 0 );
  Cel2VRAM( sora_cel, ( void * )RBG0RB_CEL_ADR, 28288 );
  Map2VRAM( sora_map, ( void * )RBG0RB_MAP_ADR, 64, 20, 1, 0 );
  Pal2CRAM( sora_pal, ( void * )RBG0RB_COL_ADR, 256 );
  slKtableRB((void *)RBG0_KTB_ADR , K_FIX | K_DOT | K_2WORD | K_ON| K_LINECOL);


  slRparaMode( K_CHANGE );
  slBack1ColSet( ( void * )BACK_COL_ADR , 0 );

  slScrAutoDisp( NBG0ON | RBG0ON );
  slTVOn();

  while(1)
  {
    slCurRpara( RA );
    slUnitMatrix( CURRENT );
    slTranslate( toFIXED( 0.0 ), toFIXED( 0.0 ) + posy, toFIXED( 100.0 ) );
    posy -= toFIXED( 5.0 );
    slRotX( DEGtoANG( -90.0 ) );
    slRotZ( angz );
    slScrMatSet();

    slCurRpara( RB );
    slUnitMatrix( CURRENT );
    slTranslate( toFIXED( 320.0 ), toFIXED( 155.0 ), toFIXED( 100.0 ) );
    slRotZ( angz );
    slScrMatSet();

    angz_up += DEGtoANG( 0.5 );
    angz = ( slSin( angz_up ) >> 4 );

    SynchConst = 1;
    slSynch();
  } 
}



