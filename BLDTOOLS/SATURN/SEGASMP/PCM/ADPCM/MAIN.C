/******************************************************************************
 *	ソフトウェアライブラリ
 *
 *	Copyright (c) 1994,1995,1997 SEGA
 *
 * Library	:ＰＣＭ・ＡＤＰＣＭ再生ライブラリ
 * Module 	:sample program of STREAM PLAY MODE
 * File		:smppcm2.c
 * Date		:1995-03-31
 * Version	:1.16
 * Auther	:Y.T
 * Modified     :1997-03-04
 * Modifier     :N.T
 *
 * Comment	:
 *	ストリームシステムを使ってＣＤ上のファイルを再生するサンプル
 *	ファイル名は "SAMPLE1.XA"
 *
 *      ADPCMの再生をサポート、サウンドドライバをCD上のファイル又はメモリ上
 *      のどちらかを選択可能に。
 *      ADPCMのファイルを再生する際には、ADPCMの#defineをします。
 *      CDROM上のサウンドドライバとサウンドエリアマップを使用する場合には、
 *      USE_CDを#defineします。
 *
 ****************************************************************************/

/*------------------------- 《インクルード》 -------------------------*/
#include <machine.h>
#include <string.h>
#define _SH
#include "sega_xpt.h"
#include "sega_sys.h"
#include "sega_int.h"
#include "sega_per.h"
#include "sega_cdc.h"
#include "sega_gfs.h"
#include "sega_stm.h"
#include "sega_snd.h"
#include "sega_pcm.h"

/* XA再生する場合に定義する。 */
#define XA_AUDIO
/* ADPCMデータを使用する際定義。*/
/*#define ADPCM*/
/* CD ROMからサウンドドライバ、サウンドエリアマップを転送する際定義 */
/*#define USE_CD*/

#ifdef XA_AUDIO
#ifndef ADPCM
#define ADPCM
#endif
#endif

#include	"sega_scl.h"

#define SMPPCMD_VblIn()				
#define SMPPCMD_Init(a)				
#define SMPPCMD_MON_Reset()			
#define SMPPCMD_PCM_Task			PCM_Task
#define VTV_Printf(a)				
#define VTV_PRINTF(a)				
#define _VTV_Printf(a)				
#define _VTV_PRINTF(a)				

/*--------------------------- 《関数宣言》 ---------------------------*/

static void vblInit(void);
static void smpVblIn(void);
static void smpVblOut(void);

#ifndef USE_CD
extern char *sddrvstsk;
extern long sddrvsize;
#else
Sint32 SDDRVS_TSK_SIZE;
Sint32 BOOTSND_MAP_SIZE;
#endif
/*---------------------------- 《定数》 ----------------------------*/

/* ウェーブＲＡＭの転送アドレスとサンプル数 */
#define	PCM_ADDR	((void*)0x25a20000)
#define	PCM_SIZE	(4096L*2)				/* 2.. */

/* セクタのサイズ */
#ifdef XA_AUDIO
#define	SECTOR_SIZE     (STM_UNIT_FORM2)	/* 10.. */
#else
#define	SECTOR_SIZE	(STM_UNIT_FORM1)	/* 10.. */
#endif

/* リングバッファのサイズ */
#define	RING_BUF_SIZE	(SECTOR_SIZE*10)	/* 10.. */

/* データの転送方式（ＣＤブロック→リングバッファ） */
#define TR_MODE_CD		PCM_TRMODE_SCU	  /* ＳＣＵのＤＭＡ         */
#if 0
#define TR_MODE_CD		PCM_TRMODE_CPU	  /* ソフトウェア転送       */
#define TR_MODE_CD		PCM_TRMODE_SDMA	  /* ＤＭＡサイクルスチール */
#endif

/*----------------------- 《グローバル変数》 -----------------------*/

/* ワークバッファ */
static PcmWork g_movie_work;

/* リングバッファ */
Uint32 g_movie_buf[RING_BUF_SIZE / sizeof(Uint32)];

/* 再生するファイル名 */
#if defined(ADPCM) & !defined(XA_AUDIO)
static char filename[] = "SAMPLE.ADP";
#elif defined(XA_AUDIO)
static char filename[] = "SAMPLE2.XA";
#else
static char filename[] = "SAMPLE.AIF";
#endif

/*---------------------------- 《関数》 ----------------------------*/

void errGfsFunc(void *obj, Sint32 ec)
{
  VTV_PRINTF((VTV_s, "S:ErrGfs %X %X\n", obj, ec));
}

void errStmFunc(void *obj, Sint32 ec)
{
  VTV_PRINTF((VTV_s, "S:ErrStm %X %X\n", obj, ec));
}

void errPcmFunc(void *obj, Sint32 ec)
{
  VTV_PRINTF((VTV_s, "S:ErrPcm %X %X\n", obj, ec));
}

/*====================== Ｖブランクの処理 ===========================*/

static void vblInit(void)
{
  set_imask(0);

  /* Ｖブランクの設定 */
  INT_ChgMsk(INT_MSK_NULL,INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);
  INT_SetScuFunc(INT_SCU_VBLK_IN, smpVblIn);
  INT_SetScuFunc(INT_SCU_VBLK_OUT, smpVblOut);
  INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT,INT_MSK_NULL);
}

static void smpVblIn(void)
{
  /* ＰＣＭライブラリの VblIn ルーチンをコール */
  PCM_VblIn();

  /* グラフィックライブラリを使用する為には実行しなければならない */
  SCL_VblankStart();
}

static void smpVblOut(void)
{
  /* グラフィックライブラリを使用する為には実行しなければならない */
  SCL_VblankEnd();
}

/*====================== サウンドの処理 ===========================*/
#ifdef USE_CD
static Sint32 fileLoad( Sint8 *name, void *addr, Sint32 *bsize )
{
  Sint32 fid, i;
  GfsHn  gfshn;

  for ( i = 0; i < 10; i++ ) {
    fid = GFS_NameToId( name );
    if ( fid >= 0 ) {
      /*
	 ファイルサイズを求め、メモリに読み込む。
	 はっきりいって高速化という観点から見れば非常にダサイく重いやり方だが、
	 サウンドドライバやマップのサイズが確定していないサンプルプログラム
	 という立場上こうします。
      */
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
  if ( fileLoad( "SDDRVS.TSK", (void *)sddrvs_tsk, &SDDRVS_TSK_SIZE ) ) {
    while( 1 );
  }
  if ( fileLoad( "BOOTSND.MAP", (void *)bootsnd_map, &BOOTSND_MAP_SIZE ) ) {
    while( 1 );
  }
  SND_INI_PRG_ADR(snd_init) 	= (Uint16 *)(&sddrvs_tsk);
  SND_INI_PRG_SZ(snd_init) 	= (Uint16 )SDDRVS_TSK_SIZE;
  SND_INI_ARA_ADR(snd_init) 	= (Uint16 *)bootsnd_map;
  SND_INI_ARA_SZ(snd_init) 	= (Uint16)BOOTSND_MAP_SIZE;
#else
  bootsnd_map[ 0 ] = 0xffffffff;
  SND_INI_PRG_ADR(snd_init)       = (Uint16 *)(&sddrvstsk);
  SND_INI_PRG_SZ(snd_init)        = sddrvsize;
  SND_INI_ARA_ADR(snd_init)       = (Uint16 *)(&bootsnd_map);
  SND_INI_ARA_SZ(snd_init)        = 1;
#endif
  SND_Init(&snd_init);
  SND_ChgMap(0);
}


/*====================== ファイルの処理 ===========================*/

/* ルートディレクトリにあるファイルの最大数 */
#define MAX_DIR		100

/* 同時に開くファイルの最大数 */
#define OPEN_MAX	5

/* ストリームグループのＩＤ */
static StmGrpHn grp_hd;

/* ディレクトリ情報管理領域 */
static GfsDirTbl	dir_tbl;

/* ファイル名を含んだ情報 */
static GfsDirName dir_name[MAX_DIR];

/* ＧＦＳの作業領域 */
GfsMng g_gfs_work;
Uint32 g_gfs_work2[(GFS_WORK_SIZE(OPEN_MAX)-sizeof(GfsMng))/4];

static Uint8   stm_work[STM_WORK_SIZE(12, 24)];

static void fileInit(void)
{
  Sint32 file_num;

  /* GFSの初期化 */
  GFS_DIRTBL_TYPE(&dir_tbl) = GFS_DIR_NAME;
  GFS_DIRTBL_DIRNAME(&dir_tbl) = dir_name;
  GFS_DIRTBL_NDIR(&dir_tbl) = MAX_DIR;

  /* ファイルシステムの初期化 */
  file_num = GFS_Init(OPEN_MAX, &g_gfs_work, &dir_tbl);
  if (file_num < 0) {
    while( 1 );
  }

  /* エラー関数の設定 */
  GFS_SetErrFunc(errGfsFunc, NULL);
}

static void stmInit(void)
{
  /* ストリームシステムの初期化 */
  STM_Init(12, 24, stm_work);
  
  /* エラー関数の設定 */
  STM_SetErrFunc(errStmFunc, NULL);
  
  /* ストリームグループのオープン */
  grp_hd = STM_OpenGrp();
  if (grp_hd == NULL) {
    while( 1 );
  }
  STM_SetLoop(grp_hd, STM_LOOP_DFL, STM_LOOP_ENDLESS);
  STM_SetExecGrp(grp_hd);
}

static StmHn stmOpen(char *fname)
{
  Sint32 fid;
  StmKey key;
  /* ファイル名からファイル識別子を求める */
  fid = GFS_NameToId((Sint8 *)fname);
  
  /* ストリームキーの設定 */
  STM_KEY_FN( &key ) = STM_KEY_NONE;
  STM_KEY_CIMSK( &key ) = STM_KEY_CIVAL( &key ) = STM_KEY_NONE;
#ifdef XA_AUDIO
  STM_KEY_CN( &key ) = 0;	/* channel No. */
  /* Audio Sector ( Necessary to play in XA-Audio ) */
  STM_KEY_SMMSK( &key ) = STM_KEY_SMVAL( &key ) = STM_SM_AUDIO;
#else
  STM_KEY_CN( &key ) = STM_KEY_NONE;
  STM_KEY_SMMSK( &key ) = STM_KEY_SMVAL( &key ) = STM_KEY_NONE;
#endif

  return STM_OpenFid(grp_hd, fid, &key, STM_LOOP_NOREAD);
}

static void stmClose(StmHn fp)
{
  STM_Close(fp);
}

void main(void)
{
  PcmHn 			pcm;
  StmHn			stm;
  PcmCreatePara	para;
  Uint32			restart;

#ifdef XA_AUDIO
  PcmInfo 		info;
#endif

  /* Ｖブランクの設定 */
  vblInit();

  /* ファイル初期化 */
  fileInit();

  /* サウンドの設定 */
  sndInit();

  /* ＰＣＭライブラリの初期化 */
  PCM_Init();

#ifdef ADPCM
  /* ＡＤＰＣＭ使用宣言 (ADPCM伸張ライブラリがリンクされます。 */
  PCM_DeclareUseAdpcm();
#endif

  /* ＰＣＭライブラリのエラーハンドルの設定 */
  PCM_SetErrFunc(errPcmFunc, NULL);

  /* ストリームシステムの初期化 */
  stmInit();

  restart = 1;

  for (;;) {
    if (restart) {
      /* ストリームオープン */
      if ( ( stm = stmOpen( filename ) ) == NULL ) {
	while( 1 );
      }

      /* 取り出し領域のリセット
       *	ストリームシステムの仕様が...
       *	最初の転送を STM_ExecServer で行うなら、これは不要。
       */
      STM_ResetTrBuf(stm);
      
      /* ハンドルの作成 */
      PCM_PARA_WORK(&para) = (struct PcmWork *)&g_movie_work;
      PCM_PARA_RING_ADDR(&para) = (Sint8 *)g_movie_buf;
      PCM_PARA_RING_SIZE(&para) = RING_BUF_SIZE;
      PCM_PARA_PCM_ADDR(&para) = PCM_ADDR;
      PCM_PARA_PCM_SIZE(&para) = PCM_SIZE;
      pcm = PCM_CreateStmHandle(&para, stm);
      if (pcm == NULL) {
	while( 1 );
      }

#ifdef XA_AUDIO
      /*
	 CDROM-XA再生の場合、ファイルには再生情報は一切無い。
	 そのため、PCM_FILE_TYPE_NO_HEADERを指定。
	 再生情報は、セクタの中のサブセクタにあるのでそれを明示するため、
	 PCM_DATA_TYPE_ADPCM_SCTを指定。
      */
      PCM_INFO_FILE_TYPE(&info) = PCM_FILE_TYPE_NO_HEADER;
      PCM_INFO_DATA_TYPE(&info) = PCM_DATA_TYPE_ADPCM_SCT;
      PCM_SetInfo(pcm, &info);
#endif
      
      /* データの転送方式の設定（ＣＤブロック→リングバッファ） */
      /* PCM_SetTrModeCd(pcm, TR_MODE_CD); */
      
      /* 開始 */
      PCM_Start(pcm);
      restart = 0;
    }

    /* サーバの実行 */
    STM_ExecServer();

    /* 再生タスク */
    PCM_Task(pcm);

    /* ムービの終了判定 */
    if (PCM_GetPlayStatus(pcm) == PCM_STAT_PLAY_END) {

      /* ハンドルの消去 */
      PCM_DestroyStmHandle(pcm);

      /* ストリームのクローズ*/
      stmClose(stm);

      restart = 1;
    }
  }
}

