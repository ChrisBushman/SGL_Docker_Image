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
 *      PCMストリームシームレス再生サンプル
 *      このサンプルはシームレスなPCMストリーム再生のサンプルです。
 *      構造を見易くするため余計な機能は一切つけていません。もし、他の
 *      データ形式の再生や強制的に次の曲に移るなどの操作をしたい時には、
 *      他のサンプルを御覧下さい。
 *      このサンプルでは、ストリームシステムを用いたAIFF再生のシームレス再生
 *      を行ないます。サウンドドライバはプログラムに組み込まれます。
 *
 ****************************************************************************/

/*------------------------- 《インクルード》 -------------------------*/
#include "sgl.h"
#include "sgl_cd.h"
#include "sega_snd.h"
#include "sega_pcm.h"

/*---------------------------- 《定数》 ----------------------------*/
static char *filename[] = { "SAMPLE01.AIF", "SAMPLE02.AIF" };
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

/* 用意するリングバッファの数 */
#define BUF_NUM 2

/* 配列の大きさ */
#define ARRAY_SIZE(array)       (sizeof(array) / sizeof(array[0]))

/* 再生するオーディオファイルの数 */
#define FILE_MAX        (ARRAY_SIZE(filename))

/* データの転送方式 */
#define TR_MODE_CD		PCM_TRMODE_SCU	  /* SCUのDMA            */
#if 0
#define TR_MODE_CD		PCM_TRMODE_CPU	  /* ソフトウェア転送    */
#define TR_MODE_CD		PCM_TRMODE_SDMA	  /* DMAサイクルスチール */
#endif

/* リングバッファのサイズ */
#define	RING_BUF_SIZE	(STM_UNIT_FORM1*10)	/* 10.. */

/*----------------------- 《グローバル変数》 -----------------------*/
extern char *sddrvstsk;
extern long sddrvsize;

/* バッファの確保 */
/* ワークバッファ */
static PcmWork g_movie_work[ BUF_NUM ];

/* リングバッファ */
Uint8 g_movie_buf[ BUF_NUM ][RING_BUF_SIZE];

/*--------------------------- 《関数宣言》 ---------------------------*/
/*---------------------------- 《関数》 ----------------------------*/
void errStmFunc( void *obj, Sint32 ec )
{
  slPrint( "Error STM Func:", slLocate( 5, 2 ) );
  slPrintHex( ec, slLocate( 20, 2 ) );
}

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

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=*/
/*                                                                 */
/*====================== サウンドの初期化 =========================*/
/*                                                                 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=*/
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

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=*/
/*                                                                 */
/*=-=-=-=-=-=-=-=-=-=ファイルシステムに関する処理=-=-=-=-=-=-=-=-=-*/
/*                                                                 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=*/
/* GFSで使うグローバルな情報 */

/* 同時に開くファイルの最大数 */
#define OPEN_MAX	5

/* GFSの作業領域 */
Uint32 g_gfs_work[ ( GFS_WORK_SIZE( OPEN_MAX ) + 3 ) / 4 ];

/*
  ルートディレクトリにあるファイルの最大数
  この数を越えたCDの場合、ファイルの検索に失敗する恐れがあります。
*/
#define MAX_DIR		1000

/* ディレクトリ情報管理領域 */
static GfsDirTbl	dir_tbl;

/* ファイル名を含んだ情報 */
static GfsDirName dir_name[ MAX_DIR ];


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

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=*/
/*                                                                 */
/*=-=-=-=-=-=-=-=-ストリーム システムに関する処理=-=-=-=-=-=-=-=-=-*/
/*                                                                 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=*/
/* ストリームグループのID */
static StmGrpHn grp_hd;

/* ストリームシステムの作業領域 */
static Uint8    stm_work[ STM_WORK_SIZE( 1, 5 ) ];

/* 最大ストリームグループ数 */
#define MAX_GRP 1

static void stmInit( void )
{
  /* ストリームシステムの初期化 */
  STM_Init( MAX_GRP, OPEN_MAX, stm_work );
  
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
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=*/
/*                                                                 */
/*-=-=-=-=--=-=-=-=-=-=-=PCMに関する処理-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*                                                                 */
/*-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
PcmHn         pcm[ BUF_NUM ];
StmHn         stm[ BUF_NUM ];

void StartSequence( Uint8 port, Uint16 file ) {
  PcmCreatePara	para;

  /* ストリームオープン */
  if ( ( stm[ port ] = stmOpen( filename[ file ] ) ) == NULL ) {
    slPrint( "Error! Stream System open.", slLocate( 3, 2 ) );
    while( 1 ) slSynch();
  }

  /* 取り出し領域のリセット
   *	ストリームシステムの仕様が...
    *	最初の転送を STM_ExecServer で行うなら、これは不要。
   */
  STM_ResetTrBuf( stm[ port ] );
  
  /* ハンドルの作成 */
  PCM_PARA_WORK( &para ) = ( struct PcmWork * )&g_movie_work[ port ];
  PCM_PARA_RING_ADDR( &para ) = g_movie_buf[ port ];
  PCM_PARA_RING_SIZE( &para ) = RING_BUF_SIZE;
  PCM_PARA_PCM_ADDR( &para ) = PCM_ADDR;
  PCM_PARA_PCM_SIZE( &para ) = PCM_SIZE;
  
  pcm[ port ] = PCM_CreateStmHandle( &para, stm[ port ] );
  
  if ( pcm[ port ] == NULL ) {
    slPrint( "Error! Open Handle.", slLocate( 5, 2 ) );
    while( 1 ) slSynch();
  }  
}

void ss_main( void )
{
  Uint32        restart = 1;
  Uint8         port = 0;
  Uint8         next;
  Uint16        file = 0;

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

  /* 開始 */
  StartSequence( port, file );

  /* サーバの実行 */
  STM_ExecServer();

  PCM_Start( pcm[ port ] );

  while( -1 ) {
    if ( restart ) {
      file = ( file + 1 + FILE_MAX ) % FILE_MAX;
      next = ( port + 1 + BUF_NUM ) % BUF_NUM;
      StartSequence( next, file );
      PCM_EntryNext( pcm[ next ] );
      restart = 0;
    }

    /* サーバの実行 */
    STM_ExecServer();
    /* 再生タスク */
    PCM_Task( pcm[ port ] ); 

    /* ムービの終了判定 */
    if ( PCM_GetPlayStatus( pcm[ port ] ) == PCM_STAT_PLAY_END ) {
      /* ハンドルの消去 */
      PCM_DestroyStmHandle( pcm[ port ] );
      /* ストリームのクローズ */
      STM_Close( stm[ port ] );

      port = ( port + 1 + BUF_NUM ) % BUF_NUM;
      restart = 1;
    }
    slSynch();
  }
}

