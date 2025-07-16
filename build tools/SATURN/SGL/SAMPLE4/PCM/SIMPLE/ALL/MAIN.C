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
 *      PCMストリーム再生サンプル
 *        CDストリーム再生とオンメモリ再生の単一(シームレスではない)再生を
 *        サポートする。
 *        又、PCM・ADPCM再生ライブラリでサポートする全てのサウンドデータ
 *        フォーマットの再生が可能。
 *        ※smppcm2.cを元に改良。
 *        オンメモリ再生をしたい場合、USE_MEMを#defineして下さい。
 *
 *        再生可能なサウンドのフォーマットは、
 *        PCM(AIFF)、ADPCM(XA)、ADPCM(AIFF)、Saturn PCM
 *        ・XAフォーマットでのADPCM(SEGA ADPCMなどで生成したデータ)を再生する
 *          場合、XA_AUDIOを#defineして下さい。
 *        ・AIFF形式(Mode2 Form2トラックを使用しないAIFFヘッダを持つ)ADPCMの
 *          再生をする場合、ADPCMを#defineして下さい。
 *        ・Saturn PCM形式(aif2sap.exeで生成したデータ)を再生する場合、には
 *          USE_SAPを#defineして下さい。
 *
 *        PCM・ADPCM再生ライブラリの持つ3つの再生方法のいずれも使用できます。
 *        ・メモリ再生の場合
 *          USE_MEMを#defineして下さい。
 *        ・ファイルシステム再生の場合
 *          USE_GFSを#defineして下さい。(default)
 *        ・ストリームシステム再生の場合
 *          USE_STMを#defineして下さい。
 *        ※CDROM-XAの再生など再生方法に依存する場合がありますので注意が
 *          必要です。
 *
 *        このサンプルでは、サウンドドライバをCD-ROMから取ってくる他に、
 *        プログラムにドライバを組み込むタイプのものにも対応しています。
 *        手元の音データ(CD-ROM)にサウンドドライバが無い場合にはそちらを使用
 *        して下さい。CDからサウンドドライバとマップファイルを取ってくる場合、
 *        USE_CDを#defineして下さい。(default)
 *
 *        SGLで再生させるために一部コードを変更。
 ****************************************************************************/

/*------------------------- 《インクルード》 -------------------------*/
#include "sgl.h"
#include "sgl_cd.h"
#include "sega_snd.h"
#include "sega_pcm.h"

/*---------------------------- 《定数》 ----------------------------*/
/*
   この下の部分は、Makeのオプションで指定しても良い。
   例えば、
     make "DFLAGS=-DXA_AUDIO -DUSE_STM -DUSE_CD"
   のようにしてmakeをかければ、
     #define XA_AUDIO
     #define USE_CD
   と同じ効果が得られます。
*/

/* 再生するファイルの種類 */
/* XA再生する場合に定義する。 */
/*#define XA_AUDIO*/
/* ADPCMデータを使用する際定義。*/
/*#define ADPCM*/
/* Saturn PCMを使用する場合 */
/*#define SAP*/

/* 再生手段 */
/* メモリ再生をする場合。 */
/*#define USE_MEM*/
/* ファイルシステムを用いて再生をする場合。(デフォルト) */
/*#define USE_GFS*/
/* ストリームシステムを用いて再生をする場合。 */
/*#define USE_STM*/

/* サウンドドライバを何処から取ってくるのか */
/* CD ROMからサウンドドライバ、サウンドエリアマップを転送する際定義 */
/*#define USE_CD*/

/*
   XAのオーディオは常にADPCMなので、もしADPCMの使用を宣言していなかったら、
   宣言しておく。又、XAの場合ストリームシステムは必須。
*/
#if defined(XA_AUDIO)
#ifndef ADPCM
#define ADPCM
#endif
#ifndef USE_STM
#define USE_STM
#endif
#endif

/*
   Saturn PCMは、CDブロック→サウンドRAM直接転送型のアルゴリズムなので、
   SIMMにデータをおいたような再生スタイルは出来ません。
   従って、ここでは強制的にCDからストリームをとって来るように変更しています。
*/
#if defined(SAP) & defined(USE_MEM)
#undef USE_MEM
#endif


/* 再生するファイル名 */
#ifndef USE_MEM
#if defined(ADPCM) & !defined(XA_AUDIO)
static char filename[] = "SAMPLE.ADP";
#elif defined(XA_AUDIO)
static char filename[] = "SAMPLE.XA";
#elif defined(SAP)
static char filename[] = "SAMPLE.SAP";
#else
static char filename[] = "SAMPLE.AIF";
#endif
#endif

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

#ifndef USE_MEM
/* セクタのサイズ */
#ifdef XA_AUDIO
#define	SECTOR_SIZE     (STM_UNIT_FORM2)
#else
#define	SECTOR_SIZE	(STM_UNIT_FORM1)
#endif
#endif

/* データの転送方式 */
#define TR_MODE_CD		PCM_TRMODE_SCU	  /* SCUのDMA            */
#if 0
#define TR_MODE_CD		PCM_TRMODE_CPU	  /* ソフトウェア転送    */
#define TR_MODE_CD		PCM_TRMODE_SDMA	  /* DMAサイクルスチール */
#endif

/* リングバッファのサイズ */
#ifdef USE_MEM
/*
   このサイズは、オンメモリ再生時のデータサイズ上限となります。
   デフォルトでは、1024×1024×1=1MBです。
   置かれるメモリによってサイズは変更しましょう。
*/
#define RING_BUF_SIZE   (1024L*1024L*1)
#else
#define	RING_BUF_SIZE	(SECTOR_SIZE*10)	/* 10.. */
#endif

/*----------------------- 《グローバル変数》 -----------------------*/
#ifndef USE_CD
extern char *sddrvstsk;
extern long sddrvsize;
#else
Sint32 SDDRVS_TSK_SIZE;
Sint32 BOOTSND_MAP_SIZE;
#endif

/* ワークバッファ */
static PcmWork g_movie_work;

/* バッファの確保 */
#ifdef SAP
/*  Saturn PCM ファイル再生用拡張ワークエリア  */
/*
   Saturn PCMは、メインメモリを経由しないので、リングバッファ入りませんが、
   内部処理用にある程度のバッファを必要とします。
*/
static Sint8 g_movie_buf[PCM_SAP_XWORK_SIZE];
#elif defined(USE_MEM)
/* リングバッファ */
/* データのおいてあるアドレス
   これは、SIMMアドレスでもSATURN実機上のアドレスでも構いません。
   SATURNのアドレスの場合、オンメモリ再生という事になります。
*/
#if 1
/*
   SIMM-RAMの場合
*/
static Sint8 *g_movie_buf = (Sint8 *)0x04000000;
#else
/*
     Low-RAMにデータを置いた場合(PCMのオンメモリ再生)
*/
static Sint8 *g_movie_buf = (Sint8 *)0x00200000;
#endif

#else
/* リングバッファ */
Uint8 g_movie_buf[RING_BUF_SIZE];
#endif


/*--------------------------- 《関数宣言》 ---------------------------*/
/*---------------------------- 《関数》 ----------------------------*/
#if !defined(USE_MEM)
#ifdef USE_STM
void errStmFunc( void *obj, Sint32 ec )
{
  slPrint( "Error STM Func:", slLocate( 5, 2 ) );
  slPrintHex( ec, slLocate( 20, 2 ) );
}
#endif
void errGfsFunc( void *obj, Sint32 ec )
{
  slPrint( "Error GFS Func:", slLocate( 5, 2 ) );
  slPrintHex( ec, slLocate( 20, 2 ) );
}
#endif

void errPcmFunc( void *obj, Sint32 ec )
{
  slPrint( "Error PCM Func:", slLocate( 5, 2 ) );
  slPrintHex( ec, slLocate( 20, 2 ) );
}

/*====================== サウンドの処理 ===========================*/
#ifdef USE_CD
/*
   ファイルID、ファイルサイズを求め、メモリに読み込む。
   はっきりいって高速化という観点から見れば非常にダサイく重い
   やり方だが、サウンドドライバやマップのサイズが確定していない
   サンプルプログラムという立場上こうします。
   本来ならばアプリケーション中では、file idとfile sizeが予め分かって
   いる訳だから、
     fileLoad関数は、
     GFS_Load( fid, 0, addr, *bsize );
   だけでOKなはず！
*/
static Sint32 fileLoad( Sint8 *name, void *addr, Sint32 *bsize )
{
  Sint32 fid, i;
  GfsHn  gfshn;

  for ( i = 0; i < 10; i++ ) {
    fid = GFS_NameToId( name );
    if ( fid >= 0 ) {
      gfshn = GFS_Open( fid );
      GFS_GetFileInfo( gfshn, &fid, NULL, bsize, NULL );
      GFS_Load( fid, 0, addr, *bsize );
      GFS_Close( gfshn );
      return 0;
    }
  }
  return -1;
}
#endif

static void sndInit(void)
{
  SndIniDt 	snd_init;
  /* あまりスマートな方法とはいえないが... */

  Sint32 bootsnd_map[ 0x400 / 4 ];
#ifdef USE_CD
  Sint32 sddrvs_tsk[ 0x10000 / 4 ];
  if ( fileLoad( "SDDRVS.TSK", ( void * )sddrvs_tsk, &SDDRVS_TSK_SIZE ) ) {
    while( 1 );
  }
  if ( fileLoad( "BOOTSND.MAP", ( void * )bootsnd_map, &BOOTSND_MAP_SIZE ) ) {
    while( 1 );
  }
  SND_INI_PRG_ADR( snd_init ) 	= ( Uint16 * )( &sddrvs_tsk );
  SND_INI_PRG_SZ( snd_init ) 	= ( Uint16 )SDDRVS_TSK_SIZE;
  SND_INI_ARA_ADR( snd_init ) 	= ( Uint16 * )bootsnd_map;
  SND_INI_ARA_SZ( snd_init ) 	= ( Uint16 )BOOTSND_MAP_SIZE;
#else
  bootsnd_map[ 0 ] = 0xffffffff;
  SND_INI_PRG_ADR( snd_init )       = ( Uint16 * )( &sddrvstsk );
  SND_INI_PRG_SZ( snd_init )        = sddrvsize;
  SND_INI_ARA_ADR( snd_init )       = ( Uint16 * )( &bootsnd_map );
  SND_INI_ARA_SZ( snd_init )        = 1;
#endif

  SND_Init( &snd_init );
  SND_ChgMap( 0 );
}

#if !defined(USE_MEM) || defined(USE_CD)
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

#ifdef USE_STM
/*
   ストリームシステムに関する設定。
*/
#define MAX_GRP 1
/* ストリームグループのID */
static StmGrpHn grp_hd;
static Uint8    stm_work[ STM_WORK_SIZE( MAX_GRP, OPEN_MAX ) ];

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
#ifdef XA_AUDIO
  STM_KEY_CN( &key ) = 0;	/* チャネル番号 */
  /* オーディオセクタが存在。(XA再生の場合必ず必要) */
  STM_KEY_SMMSK( &key ) = STM_KEY_SMVAL( &key ) = STM_SM_AUDIO;
#else
  STM_KEY_CN( &key ) = STM_KEY_NONE;
  STM_KEY_SMMSK( &key ) = STM_KEY_SMVAL( &key ) = STM_KEY_NONE;
#endif

  return STM_OpenFid( grp_hd, fid, &key, STM_LOOP_NOREAD );
}

static void stmClose( StmHn fp )
{
  STM_Close( fp );
}

#endif
#endif

void ss_main( void )
{
  PcmHn        	pcm;
  PcmCreatePara	para;
  Uint32        restart;
#ifdef USE_STM
  StmHn	       	stm;
#elif !defined(USE_MEM)
  GfsHn         gfs;
#endif
#ifdef XA_AUDIO
  PcmInfo 	info;
#endif

  slInitSystem( TV_320x240, NULL, 1 );

  /* Vブランクの設定 */
  slIntFunction( PCM_VblIn );

#if !defined(USE_MEM) || defined(USE_CD)
  /* ファイル初期化 */
  fileInit();
#endif

  /* サウンドの設定 */
  sndInit();

  /* PCMライブラリの初期化 */
  PCM_Init();

#ifdef ADPCM
  /* ADPCM使用宣言 (ADPCM伸張ライブラリがリンクされます。) */
  PCM_DeclareUseAdpcm();
#elif defined(SAP)
  /* Saturn PCM 使用宣言 */
#ifdef USE_STM
  PCM_DeclareUseSapStm();
#else
  PCM_DeclareUseSapGfs();
#endif
#endif

  /* PCMライブラリのエラーハンドルの設定 */
  PCM_SetErrFunc( errPcmFunc, NULL );

#ifdef USE_STM
  /* ストリームシステムの初期化 */
  stmInit();
#endif

  restart = 1;

  while( -1 ) {
    if ( restart ) {
#ifdef USE_STM
      /* ストリームオープン */
      if ( ( stm = stmOpen( filename ) ) == NULL ) {
	slPrint( "Error! Stream open.", slLocate( 5, 2 ) );
	slSynch();
	while( 1 );
      }
#elif !defined(USE_MEM)
      if ( ( gfs = GFS_Open( GFS_NameToId( filename ) ) ) == NULL ) {
	slPrint( "Error! File System open.", slLocate( 5, 2 ) );
	slSynch();
	while( 1 );
      }	
#endif

#ifdef USE_STM
      /* 取り出し領域のリセット
       *	ストリームシステムの仕様が...
       *	最初の転送を STM_ExecServer で行うなら、これは不要。
       */
      STM_ResetTrBuf( stm );
#endif

      /* ハンドルの作成 */
      PCM_PARA_WORK( &para ) = ( struct PcmWork * )&g_movie_work;
#ifdef SAP
      PCM_PARA_XWORK_ADDR( &para ) = g_movie_buf;
      PCM_PARA_XWORK_SIZE( &para ) = PCM_SAP_XWORK_SIZE;
#else
      PCM_PARA_RING_ADDR( &para ) = g_movie_buf;
      PCM_PARA_RING_SIZE( &para ) = RING_BUF_SIZE;
#endif
      PCM_PARA_PCM_ADDR( &para ) = PCM_ADDR;
      PCM_PARA_PCM_SIZE( &para ) = PCM_SIZE;

#ifdef USE_MEM
      pcm = PCM_CreateMemHandle( &para );
      /* データサイズの告知 */
      PCM_NotifyWriteSize( pcm, RING_BUF_SIZE );
#elif defined(USE_STM)
      pcm = PCM_CreateStmHandle( &para, stm );
#else
      pcm = PCM_CreateGfsHandle( &para, gfs );
#endif

      if ( pcm == NULL ) {
	slPrint( "Error! Open Handle.", slLocate( 5, 2 ) );
	slSynch();
	while( 1 );
      }
      
      /*
	 状況に応じて設定すると不具合が改善される場合があります。
	 但し、この値を変えたことによってある種のデータについては改善されても
	 ある種のデータについては逆に不具合となる事もありますので、慎重に
	 使用して下さい。
      */
      /* シームレス再生時のブランクタイムを減らしたい時など */
      /* PCM_Set1TaskSample( pcm, 2048 ); */
      /* 曲(音)の末尾が切れる場合など */
      /* PCM_SetStopTrigSample( pcm, 2048 ); */

#ifndef USE_MEM
      /* データの転送方式の設定(CDブロック→) */
      PCM_SetTrModeCd( pcm, TR_MODE_CD );
#endif

#ifdef XA_AUDIO
      /*
	 CDROM-XA再生の場合、ファイルには再生情報は一切無い。
	 そのため、PCM_FILE_TYPE_NO_HEADERを指定。
	 再生情報は、セクタの中のサブセクタにあるのでそれを明示するため、
	 PCM_DATA_TYPE_ADPCM_SCTを指定。
      */
      PCM_INFO_FILE_TYPE( &info ) = PCM_FILE_TYPE_NO_HEADER;
      PCM_INFO_DATA_TYPE( &info ) = PCM_DATA_TYPE_ADPCM_SCT;
      PCM_SetInfo( pcm, &info );
#endif

      /* 開始 */
      PCM_Start( pcm );
      restart = 0;
    }

#ifdef USE_STM
    /* サーバの実行 */
    STM_ExecServer();
#endif
    
    /* 再生タスク */
    PCM_Task( pcm );
    
    /* ムービの終了判定 */
    if ( PCM_GetPlayStatus( pcm ) == PCM_STAT_PLAY_END ) {
      
      /* ハンドルの消去 */
#ifdef USE_MEM
      PCM_DestroyMemHandle( pcm );
#elif defined(USE_STM)
      PCM_DestroyStmHandle( pcm );
#else
      PCM_DestroyGfsHandle( pcm );
#endif
      
#ifdef USE_STM
      /* ストリームのクローズ */
      stmClose( stm );
#elif !defined( USE_MEM )
      /* ファイルシステムのクローズ */
      GFS_Close( gfs );
#endif
      restart = 1;
    }

    /* タスクゲージの表示 */
    set_vbar( 3 );
    slSynch();
    reset_vbar( 3 );
  }
}

