/*****************************************************************************
 *
 *	Copyright (c) 1994,1995,1997 SEGA
 *
 * Library	:PCM・ADPCM再生ライブラリ
 * Module 	:sample program of STREAM PLAY MODE
 * File		:main.c
 * Date		:1995-03-31
 * Version	:1.16
 * Auther	:Y.T
 * Modified     :1997-03-04
 * Modifier     :N.T
 *
 * Comment	:
 *      PCMストリーム再生サンプル(AIFF再生)
 *        CDストリーム再生とオンメモリ再生の単一(シームレスではない)再生を
 *        サポートする。
 *        又、PCM・ADPCM再生ライブラリでサポートする全てのサウンドデータ
 *        フォーマットの再生が可能。
 *        ※smppcm2.cを元に改良。
 *
 *        このサンプルでは、サウンドドライバをプログラムに組み込んでいます。
 *        もし、CDからとってくる場合、allディレクトリのサンプルを参考にして
 *        変更してみて下さい。
 *
 *        SGLで再生させるために一部コードを変更。
 ****************************************************************************/

/*------------------------- 《インクルード》 -------------------------*/
#include "sgl.h"
#include "sgl_cd.h"
#include "sega_snd.h"
#include "sega_pcm.h"

static char filename[] = "SAMPLE.AIF";

/* ウェーブRAMの転送アドレスとサンプル数 */
/*
   【注意】
   PCM_SIZEのデータサイズを元に、
    8Bit Mono   :×1
   16Bit Mono   :×2
    8Bit Stereo :×2
   16Bit Stereo :×4
   のバッファを必要とします。
   ※PCM_SIZEのデータサイズがそのまま使われる訳ではありません。
   従って、PCM_ADDRに与えるPCMバッファのスタートアドレスは、その事を考慮した
   上で設定しなければなりません。
   例えば、16Bit StereoのAIFFデータを再生するのに
   PCM_SIZE = 4096L * 2
   を設定した場合、( 4096 * 2 ) * 4 = 32768 = 8000H
   のバッファサイズを必要とするので、PCM_ADDRのアドレスは、
   25A78000H以前に設定しないと、音の後半にノイズが入る可能性があります。
   又、PCMのバッファサイズが小さい場合、再生中にCDのリードエラーなどで、
   PCMストリームデータの供給が止まった場合、同じ音が繰り返し再生される
   等の症状が起こる事があります。
   その場合、PCM_SIZEに少し大きめの値を入れる事で解決できます。
*/
#define	PCM_ADDR	( ( void * )0x25a20000 )
#define	PCM_SIZE	( 4096L * 8 )	           /* 8 */

#define	SECTOR_SIZE	(STM_UNIT_FORM1)

/* データの転送方式 */
#define TR_MODE_CD		PCM_TRMODE_SCU	  /* SCUのDMA            */
#if 0
#define TR_MODE_CD		PCM_TRMODE_CPU	  /* ソフトウェア転送    */
#define TR_MODE_CD		PCM_TRMODE_SDMA	  /* DMAサイクルスチール */
#endif

/* リングバッファのサイズ */
#define	RING_BUF_SIZE	(SECTOR_SIZE*10)	/* 10.. */

/*----------------------- 《グローバル変数》 -----------------------*/
extern char *sddrvstsk;
extern long sddrvsize;

/* ワークバッファ */
static PcmWork g_movie_work;

/* バッファの確保 */
/* リングバッファ */
Uint8 g_movie_buf[RING_BUF_SIZE];

/*--------------------------- 《関数宣言》 ---------------------------*/
/*---------------------------- 《関数》 ----------------------------*/
void errGfsFunc( void *obj, Sint32 ec )
{
  slPrint( "Error GFS Func:", slLocate( 5, 2 ) );
  slPrintHex( ec, slLocate( 20, 2 ) );
}

void errPcmFunc( void *obj, Sint32 ec )
{
  slPrint( "Error PCM Func:", slLocate( 5, 2 ) );
  slPrintHex( ec, slLocate( 20, 2 ) );
}

/*====================== サウンドの処理 ===========================*/

static void sndInit(void)
{
  SndIniDt 	snd_init;

  Sint32 bootsnd_map[ 0x400 / 4 ];
  bootsnd_map[ 0 ] = 0xffffffff;
  SND_INI_PRG_ADR( snd_init )       = ( Uint16 * )( &sddrvstsk );
  SND_INI_PRG_SZ( snd_init )        = sddrvsize;
  SND_INI_ARA_ADR( snd_init )       = ( Uint16 * )( &bootsnd_map );
  SND_INI_ARA_SZ( snd_init )        = 1;

  SND_Init( &snd_init );
  SND_ChgMap( 0 );
}

/*====================== ファイルの処理 ===========================*/

/*
  ルートディレクトリにあるファイルの最大数
  この数を越えたCDの場合、ファイルの検索に失敗する恐れがあります。
*/
#define MAX_DIR		100

/* 同時に開くファイルの最大数 */
#define OPEN_MAX	5

/* ディレクトリ情報管理領域 */
static GfsDirTbl	dir_tbl;

/* ファイル名を含んだ情報 */
static GfsDirName dir_name[ MAX_DIR ];

/* GFSの作業領域 */
Uint32 g_gfs_work[ ( GFS_WORK_SIZE( OPEN_MAX ) + 3 ) / 4 ];

static void fileInit( void )
{
  Sint32 file_num;

  /* GFSの初期化 */
  GFS_DIRTBL_TYPE( &dir_tbl )    = GFS_DIR_NAME;
  GFS_DIRTBL_DIRNAME( &dir_tbl ) = dir_name;
  GFS_DIRTBL_NDIR( &dir_tbl )    = MAX_DIR;

  /* ファイルシステムの初期化 */
  file_num = GFS_Init( OPEN_MAX, g_gfs_work, &dir_tbl );
  if ( file_num < 0 ) {
    slPrint( "Error! GFS Init.", slLocate( 5, 2 ) );
    slSynch();
    while( 1 );
  }

  /* エラー関数の設定 */
  GFS_SetErrFunc( errGfsFunc, NULL );
}

void ss_main( void )
{
  PcmHn        	pcm;
  PcmCreatePara	para;
  Uint32        restart;
  GfsHn         gfs;

  slInitSystem( TV_320x240, NULL, 1 );

  /* Vブランクの設定 */
  slIntFunction( PCM_VblIn );

  /* ファイル初期化 */
  fileInit();

  /* サウンドの設定 */
  sndInit();

  /* PCMライブラリの初期化 */
  PCM_Init();

  /* PCMライブラリのエラーハンドルの設定 */
  PCM_SetErrFunc( errPcmFunc, NULL );

  restart = 1;

  while( -1 ) {
    if ( restart ) {
      if ( ( gfs = GFS_Open( GFS_NameToId( filename ) ) ) == NULL ) {
	slPrint( "Error! File System open.", slLocate( 5, 2 ) );
	slSynch();
	while( 1 );
      }	

      /* ハンドルの作成 */
      PCM_PARA_WORK( &para ) = ( struct PcmWork * )&g_movie_work;
      PCM_PARA_RING_ADDR( &para ) = g_movie_buf;
      PCM_PARA_RING_SIZE( &para ) = RING_BUF_SIZE;
      PCM_PARA_PCM_ADDR( &para ) = PCM_ADDR;
      PCM_PARA_PCM_SIZE( &para ) = PCM_SIZE;

      pcm = PCM_CreateGfsHandle( &para, gfs );

      if ( pcm == NULL ) {
	slPrint( "Error! Open Handle.", slLocate( 5, 2 ) );
	slSynch();
	while( 1 );
      }
      PCM_SetStartTrgSample( pcm, 2560 );

      /* 開始 */
      PCM_Start( pcm );
      restart = 0;
    }

    /* 再生タスク */
    PCM_Task( pcm );
    
    /* ムービの終了判定 */
    if ( PCM_GetPlayStatus( pcm ) == PCM_STAT_PLAY_END ) {
      
      /* ハンドルの消去 */
      PCM_DestroyGfsHandle( pcm );
      
      /* ファイルシステムのクローズ */
      GFS_Close( gfs );
      
      restart = 1;
    }
    /* タスクバーの表示 */
    set_vbar( 3 );
    slSynch();
    reset_vbar( 3 );
  }
}

