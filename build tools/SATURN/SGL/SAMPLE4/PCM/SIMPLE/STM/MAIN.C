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
 *      PCMストリーム再生サンプル(AIFF)
 *        ※smppcm2.cを元に改良。
 *          SGLで再生させるために一部コードを変更。
 *
 *        このサンプルでは、サウンドドライバをプログラムに組み込むタイプの
 *        ものです。
 ****************************************************************************/

/*------------------------- 《インクルード》 -------------------------*/
#include "sgl.h"
#include "sgl_cd.h"
#include "sega_snd.h"
#include "sega_pcm.h"
/*--------------------------- 《関数宣言》 ---------------------------*/
extern char *sddrvstsk;
extern long sddrvsize;
/*---------------------------- 《定数》 ----------------------------*/
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

#define TR_MODE_CD      PCM_TRMODE_SCU	  /* SCU DMAによるデータ転送 */

/* リングバッファのサイズ */
#define	RING_BUF_SIZE	(SECTOR_SIZE*10)	/* 10.. */
/*----------------------- 《グローバル変数》 -----------------------*/

/* ワークバッファ */
static PcmWork g_movie_work;

/* バッファの確保 */
/* リングバッファ */
Uint8 g_movie_buf[RING_BUF_SIZE];

/*---------------------------- 《関数》 ----------------------------*/

void errGfsFunc( void *obj, Sint32 ec )
{
  slPrint( "Error GFS Func:", slLocate( 5, 2 ) );
  slPrintHex( ec, slLocate( 20, 2 ) );
}

void errStmFunc( void *obj, Sint32 ec )
{
  slPrint( "Error STM Func:", slLocate( 5, 2 ) );
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

/*
   ストリームシステムに関する設定。
*/
#define MAX_GRP   1
/* ストリームグループのID */
static StmGrpHn grp_hd;
static Uint8    stm_work[ STM_WORK_SIZE( MAX_GRP, OPEN_MAX ) ];

static void stmInit( void )
{
  /* ストリームシステムの初期化 */
  STM_Init( 12, 24, stm_work );
  
  /* エラー関数の設定 */
  STM_SetErrFunc( errStmFunc, NULL );
  
  /* ストリームグループのオープン */
  grp_hd = STM_OpenGrp();
  if ( grp_hd == NULL ) {
    while( 1 );
  }
  STM_SetLoop( grp_hd, STM_LOOP_DFL, STM_LOOP_ENDLESS );
  STM_SetExecGrp( grp_hd );
}

static StmHn stmOpen( char *fname )
{
  Sint32 fid;
  StmKey key;
  /* ファイル名からファイル識別子を求める */
  fid = GFS_NameToId( ( Sint8 * )fname );
  
  /* ストリームキーの設定 */
  STM_KEY_FN( &key ) = STM_KEY_NONE;
  STM_KEY_CIMSK( &key ) = STM_KEY_CIVAL( &key ) = STM_KEY_NONE;
  STM_KEY_CN( &key ) = STM_KEY_NONE;
  STM_KEY_SMMSK( &key ) = STM_KEY_SMVAL( &key ) = STM_KEY_NONE;

  return STM_OpenFid( grp_hd, fid, &key, STM_LOOP_NOREAD );
}

static void stmClose( StmHn fp )
{
  STM_Close( fp );
}

void ss_main( void )
{
  PcmHn        	pcm;
  PcmCreatePara	para;
  Uint32        restart;
  StmHn	       	stm;

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

  /* ストリームシステムの初期化 */
  stmInit();

  restart = 1;

  while( -1 ) {
    if ( restart ) {

      /* ストリームオープン */
      if ( ( stm = stmOpen( filename ) ) == NULL ) {
	slPrint( "Error! Stream open.", slLocate( 5, 2 ) );
	slSynch();
	while( 1 );
      }

      /* 取り出し領域のリセット
       *	ストリームシステムの仕様が...
       *	最初の転送を STM_ExecServer で行うなら、これは不要。
       */
      STM_ResetTrBuf(stm);

      /* ハンドルの作成 */
      PCM_PARA_WORK( &para ) = ( struct PcmWork * )&g_movie_work;
      PCM_PARA_RING_ADDR( &para ) = g_movie_buf;
      PCM_PARA_RING_SIZE( &para ) = RING_BUF_SIZE;
      PCM_PARA_PCM_ADDR( &para ) = PCM_ADDR;
      PCM_PARA_PCM_SIZE( &para ) = PCM_SIZE;

      pcm = PCM_CreateStmHandle( &para, stm );

      if ( pcm == NULL ) {
	slPrint( "Error! Open Handle.", slLocate( 5, 2 ) );
	slSynch();
	while( 1 );
      }

      /* データの転送方式の設定(CDブロック→) */
      PCM_SetTrModeCd( pcm, TR_MODE_CD );

      /* 開始 */
      PCM_Start( pcm );
      restart = 0;
    }

    /* サーバの実行 */
    STM_ExecServer();

    /* 再生タスク */
    PCM_Task( pcm );

    /* ムービの終了判定 */
    if ( PCM_GetPlayStatus( pcm ) == PCM_STAT_PLAY_END ) {

      /* ハンドルの消去 */
      PCM_DestroyStmHandle( pcm );

      /* ストリームのクローズ*/
      stmClose( stm );

      restart = 1;
    }
    set_vbar( 3 );
    slSynch();
    reset_vbar( 3 );
  }
}

