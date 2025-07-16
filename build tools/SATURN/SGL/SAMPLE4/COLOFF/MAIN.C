/*----------------------------------------------------------------------*/
/*	Scroll Color Offset effect					*/
/*----------------------------------------------------------------------*/
#include	"sgl.h"
#include	"ss_scrol.h"

#define		NBG1_CEL_ADR		VDP2_VRAM_B0
#define		NBG1_MAP_ADR		( VDP2_VRAM_B0 + 0x10000 )
#define		NBG1_COL_ADR		( VDP2_COLRAM + 0x00200 )
#define		BACK_COL_ADR		( VDP2_VRAM_A1 + 0x1fffe )

#define ACCELERANDO 0
#define RITARDANDO  1

void ss_main()
{
  FIXED  yama_posx = toFIXED( -40.0 ), yama_posy = toFIXED( -50.0 );
  Sint16 i = 0;
  int    j = ACCELERANDO;
  Uint16 pad;

  slInitSystem( TV_320x224, NULL, 1 );
  slTVOff();
  slPrint( "Color Offset Sample", slLocate( 9, 2 ) );
  
  slColRAMMode( CRM16_1024 );

  slBack1ColSet( ( void * )BACK_COL_ADR, 0 );
  
  slCharNbg1(COL_TYPE_256 , CHAR_SIZE_1x1);
  slPageNbg1( (void *)0x24e40000, (void *)0x600 , PNB_1WORD|CN_10BIT);
  slPlaneNbg1(PL_SIZE_1x1);
  slMapNbg1((void *)0x50000 , (void *)NBG1_MAP_ADR , (void *)NBG1_MAP_ADR , (void *)NBG1_MAP_ADR);
  Cel2VRAM( yama_cel, ( void * )NBG1_CEL_ADR, 31808 );
  Map2VRAM( yama_map, ( void * )NBG1_MAP_ADR, 32, 16, 1, 0 );
  Pal2CRAM( yama_pal, ( void * )NBG1_COL_ADR, 256 );

  slScrPosNbg1( yama_posx, yama_posy );
  slScrAutoDisp( NBG0ON | NBG1ON );

  slColOffsetAUse( NBG1ON );

  slTVOn();  

  while( 1 ){
    pad = Smpc_Peripheral[ 0 ].data;

    if ( j == ACCELERANDO ) {
      if ( ++i > 254 ) j = RITARDANDO;
    } else {
      if ( --i < -254 ) j = ACCELERANDO;
    }
    slColOffsetA( i, - i, i / 2 );
    slSynch();
  } 
}
