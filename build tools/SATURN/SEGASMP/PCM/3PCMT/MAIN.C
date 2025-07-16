/******************************************************************************
 *	ソフトウェアライブラリ
 *
 *	Copyright (c) 1994,1995 SEGA
 *
 * Library	:ＰＣＭ・ＡＤＰＣＭ再生ライブラリ
 * Module 	:sample program of STREAM PLAY MODE
 * File		:smppcm12.c
 * Date		:1997-03-18
 * Version	:.
 * Auther	:
 *
 * Comment	:
 *   ストリームシステムを使ってＣＤ上のAIFF(ADPCM)ファイルを
 *   シームレスに連続再生するサンプル
 *   オーディオファイル名は
 *      "SAMPLE1.AIF", "SAMPLE2.AIF", "SAMPLE3.AIF", "SAMPLE4.AIF"
 * Warning:
 *   サンプルプログラムは、STM(ストリーム)ライブラリとPCM_EntryNext関数
 * 使用して作成しています。これはシームレスに再生するには必要な手順で
 * そのため下記のような制限がありますので、
 *  1.再生するグループのすべてのAIFFまたはADPCM系のデータファイルは同じ
 *   データレートにする必要があります。つまり上記のファイル名のデータは
 *   すべてSAMPLE1.AIFと同じ種類(AIFF)でかつ同一周波数である必要があり
 *   ます。(オーディオデータ作成時に注意が必要です。)
 *  2.連続して同じファイル名を宣言することはできません。
 *    ×　SAMPLE1.AIF→SAMPLE2.AIF→SAMPLE2.AIF→SAMPLE2.AIF
 *　　○　SAMPLE1.AIF→SAMPLE2.AIF→SAMPLE3.AIF→SAMPLE2.AIF
 *　　　(×と同様のことを行うにはSAMPLE3.AIFの中身をSAMPLE2.AIFと同じにする。)
 *  3.今回4つのファイルのシームレス再生のサンプルですが、基本的にファイル数を
 *   変更するだけの場合、 2の倍数のファイル数を宣言してください。
 *   　このサンプルプログラムでは、PCMのリングバッファを2つだけで行う形にし
 *   ており、バッファの振り分け方法を単純化しているためです。奇数ファイルの
 *   再生を行う場合は、バッファの振り分け方法を改良してください。
 *   
 ****************************************************************************/

#define SUB_DIR_NAME "SONIC12"

#include "machine.h"
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
#include "sega_scl.h"

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
static void playAudio( int read_no );
static Bool isStmReadEnd(StmHn stm);
static Bool isReadEnd(int read_no);
static Bool isAudioEnd(int play_no);


extern char *sddrvstsk;
extern long sddrvsize;

/*---------------------------- 《定数》 ----------------------------*/

/* ウェーブＲＡＭの転送アドレスとサンプル数 */
#define	PCM_ADDR	((void*)0x25a20000)
#define	PCM_SIZE	(4096L*10)				/* 2.. */

/* リングバッファのサイズ */
#define	RING_BUF_SIZE	(2048L*10)			/* 10.. */

/* データの転送方式（ＣＤブロック→リングバッファ） */
#define TR_MODE_CD		PCM_TRMODE_CPU		/* ソフトウェア転送 			*/

/* 同時にオープンするオーディオの数 */
#define	BUF_NUM		2

/* 配列の大きさ */
#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

/* 再生するオーディオファイルの数 */
#define	FILE_MAX	(ARRAY_SIZE(filename))

/*----------------------- 《グローバル変数》 -----------------------*/

/* ワークバッファ */
static PcmWork g_movie_work[BUF_NUM];

/* リングバッファ */
Uint32 g_movie_buf[BUF_NUM][RING_BUF_SIZE / sizeof(Uint32)];

/* 再生するファイル名 */
static char *filename[] = 
	{"S1_87_1.ADP","S1_87_2.ADP"};

static	PcmHn	pcm[BUF_NUM]; /* PCMのハンドル */
static	StmHn	stm[BUF_NUM]; /* STMのハンドル */
#if 0
static	GfsHn	gfs[BUF_NUM]; /* STMのハンドル */
#endif
static	Bool	g_start_flag; /* 最初のオーディオを再生開始したら TRUE */

/*---------------------------- 《関数》 ----------------------------*/

GfsErrStat errGfsStat;
StmErrStat errStmStat;
PcmErrCode errPcmStat;

void errGfsFunc(void *obj, Sint32 ec)
{
	VTV_PRINTF((VTV_s, "S:ErrGfs %X %X\n", obj, ec));
	GFS_GetErrStat(&errGfsStat);
	while(1);
}

void errStmFunc(void *obj, Sint32 ec)
{
	VTV_PRINTF((VTV_s, "S:ErrStm %X %X\n", obj, ec));
	STM_GetErrStat(&errStmStat);
	while(1);
}

void errPcmFunc(void *obj, Sint32 ec)
{
	VTV_PRINTF((VTV_s, "S:ErrPcm %X %X\n", obj, ec));
	errPcmStat = PCM_GetErr();
/*	while(1); */
}


/*====================== Ｖブランクの処理 ===========================*/
/* Vブランクの初期化 */
static void vblInit(void)
{
	int tmp_imask;

  tmp_imask = get_imask();
  set_imask(15);

  /* Ｖブランクの設定 */
  INT_ChgMsk(INT_MSK_NULL,INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);
  INT_SetScuFunc(INT_SCU_VBLK_IN, smpVblIn);
  INT_SetScuFunc(INT_SCU_VBLK_OUT, smpVblOut);
  INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT,INT_MSK_NULL);

  set_imask(tmp_imask);
}

/* VブランクINの登録 */
static void smpVblIn(void)
{
  /* ＰＣＭライブラリの VblIn ルーチンをコール */
  PCM_VblIn();

  /* グラフィックライブラリを使用する為には実行しなければならない */
  SCL_VblankStart();
}

/* VブランクOUTの登録 */
static void smpVblOut(void)
{
  /* グラフィックライブラリを使用する為には実行しなければならない */
  SCL_VblankEnd();
}


/*====================== サウンドの処理 ===========================*/
#define SDDRVS_TSK_SIZE			(0x8000)
#define BOOTSND_MAP_SIZE		(0x0100)
Sint32 sddrvs_tsk[SDDRVS_TSK_SIZE / 4];
Sint32 bootsnd_map[BOOTSND_MAP_SIZE / 4];

/* サウンドの初期化 */
static void sndInit(void)
{
  SndIniDt 	snd_init;
  Uint16 map[] = { 0xffff };
  
  SND_INI_PRG_ADR(snd_init) 	= (Uint16 *)( &sddrvstsk);
  SND_INI_PRG_SZ(snd_init) 	= (Uint16 )sddrvsize;
  SND_INI_ARA_ADR(snd_init) 	= (Uint16 *)map;
  SND_INI_ARA_SZ(snd_init) 	= 2;
  
  SND_Init(&snd_init);
  SND_ChgMap(0);
}


/*====================== ファイルの処理 ===========================*/

/* ルートディレクトリにあるファイルの最大数 */
#define MAX_DIR_ROOT		200
/* SONIC12ディレクトリにあるファイルの最大数 */
#define MAX_DIR_SUB		140


/* 同時に開くファイルの最大数 */
#define OPEN_MAX	5

/* ストリームグループのＩＤ */
static StmGrpHn grp_hd;

/* サブディレクトリ情報 fid */
Sint32 dir_sub_fid;

/* ディレクトリ情報管理領域 */
static GfsDirTbl	dir_tbl_root;
static GfsDirTbl	dir_tbl_sub;

/* ファイル名を含んだ情報 */
static GfsDirName dir_name_root[MAX_DIR_ROOT];
static GfsDirName dir_name_sub[MAX_DIR_SUB];

/* ＧＦＳの作業領域 */
Uint32 g_gfs_work2[(GFS_WORK_SIZE(OPEN_MAX) + 3 ) / 4];

static Uint8   stm_work[STM_WORK_SIZE(12, 24)];

/* ファイルシステムの初期化 */
static void fileInit(void)
{
  Sint32 file_num;
  
  /* GFSの初期化 */
  GFS_DIRTBL_TYPE(&dir_tbl_root) = GFS_DIR_NAME;
  GFS_DIRTBL_DIRNAME(&dir_tbl_root) = dir_name_root;
  GFS_DIRTBL_NDIR(&dir_tbl_root) = MAX_DIR_ROOT;
  
  GFS_DIRTBL_TYPE(&dir_tbl_sub) = GFS_DIR_NAME;
  GFS_DIRTBL_DIRNAME(&dir_tbl_sub) = dir_name_sub;
  GFS_DIRTBL_NDIR(&dir_tbl_sub) = MAX_DIR_SUB;
  
  /* ファイルシステムの初期化 */
  file_num = GFS_Init(OPEN_MAX, g_gfs_work2, &dir_tbl_root);
  if (file_num < 0) {
    while( -1 );
    return;
  }
  
  dir_sub_fid = GFS_NameToId( SUB_DIR_NAME );
  GFS_LoadDir( dir_sub_fid, &dir_tbl_sub);
  GFS_SetDir(&dir_tbl_sub);

  /* エラー関数の設定 */
  GFS_SetErrFunc(errGfsFunc, NULL);
}



/* ストリームシステムの初期化 */
static void stmInit(void)
{
  /* ストリームシステムの初期化 */
  STM_Init(12, 24, stm_work);
  
  /* エラー関数の設定 */
  STM_SetErrFunc(errStmFunc, NULL);
  
  /* ストリームグループのオープン */
  grp_hd = STM_OpenGrp();
  if (grp_hd == NULL) {
    while( -1 );
    return;
  }
  STM_SetLoop(grp_hd, STM_LOOP_DFL, STM_LOOP_ENDLESS);
  STM_SetExecGrp(grp_hd);
}

/* ストリームのオープン */
static StmHn stmOpen(char *fname)
{
  Sint32 fid;
  StmKey key;
  
  /* ファイル名からファイル識別子を求める */
  fid = GFS_NameToId((Sint8 *)fname);
  STM_KEY_FN(&key) = STM_KEY_CN(&key) = STM_KEY_SMMSK(&key) = 
    STM_KEY_SMVAL(&key) = STM_KEY_CIMSK(&key) = STM_KEY_CIVAL(&key) =
      STM_KEY_NONE;
	return STM_OpenFid(grp_hd, fid, &key, STM_LOOP_READ);
}

/* ストリームのクローズ */
static void stmClose(StmHn fp)
{
  STM_Close(fp);
}

/* Audio再生の開始 */
static void playAudio( int read_no )
{
  PcmCreatePara	para;
  int hd_no;
  
  hd_no = read_no % BUF_NUM;
  if ((stm[hd_no] = stmOpen(filename[read_no])) == NULL) {
    while( -1 );
    return;
  }
  STM_ResetTrBuf(stm[hd_no]);
  /* ハンドルの作成 */
  PCM_PARA_WORK(&para) = (struct PcmWork *)&g_movie_work[hd_no];
  PCM_PARA_RING_ADDR(&para) = (Sint8 *)g_movie_buf[hd_no];
  PCM_PARA_RING_SIZE(&para) = RING_BUF_SIZE;
  PCM_PARA_PCM_ADDR(&para) = PCM_ADDR;
  PCM_PARA_PCM_SIZE(&para) = PCM_SIZE;
  pcm[hd_no] = PCM_CreateStmHandle(&para, stm[hd_no]);
  if (pcm[hd_no] == NULL) {
    while ( -1 );
    return;
  }
#if 0
  if (hd_no == 0) {
    /* ピックアップの移動 */
    STM_MovePickup(stm[hd_no], 0);
  }
#endif
  /* オーディオの再生開始 */
  if (g_start_flag == FALSE) {
    PCM_Start(pcm[hd_no]);
    g_start_flag = TRUE;
  } else {
    PCM_EntryNext(pcm[hd_no]);
  }
  return;
}

/* ストリームの終了判定 */
static Bool isStmReadEnd(StmHn pstm)
{
  Sint32		fad;
  Sint32		fid;
  StmFrange	frange;
  Sint32		bn;
  StmKey		stmkey;
  
  /* ストリームの再生範囲を取得する */
  STM_GetInfo(pstm, &fid, &frange, &bn, &stmkey);
  
  /* 再生中のＦＡＤの位置を取得する */
  STM_GetExecStat(grp_hd, &fad);
  
  if (fad >= (STM_FRANGE_SFAD(&frange) + STM_FRANGE_FASNUM(&frange))) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/* ファイルの読み込み終了判定 */
static Bool isReadEnd(int read_no)
{
  int no;
  
  no = read_no % BUF_NUM;
  
  if (isStmReadEnd(stm[no])) {
    return TRUE;
  }
  return FALSE;
}

/* Audio再生の終了判定 */
static Bool isAudioEnd(int play_no)
{
  int no;
  
  no = play_no % BUF_NUM;
  if (PCM_GetPlayStatus(pcm[no]) == PCM_STAT_PLAY_END) {
    
    /* 再生オーディオの放棄 */
    PCM_DestroyStmHandle(pcm[no]);
    /* ストリームのクローズ*/
    stmClose(stm[no]);
    
    return TRUE;
  }
  return FALSE;
}


/* メインルーチン */
void main(void)
{
  int		play_no;	/* 再生ファイル番号 */
  int		read_no;	/* ファイル読み込み番号 */
  Bool	next_start;	/* 次のファイルの読み込みフラグ */

	/* Ｖブランクの設定 */
  vblInit();
  
  /* ファイル初期化 */
  fileInit();
  
  /* サウンドの設定 */
  sndInit();
  
  /* ＰＣＭライブラリの初期化 */
  PCM_Init();
  
  /* ＡＤＰＣＭ使用宣言 (ADPCM伸張ライブラリのリンク) */
  PCM_DeclareUseAdpcm();
  
  /* ＰＣＭライブラリのエラーハンドルの設定 */
  PCM_SetErrFunc(errPcmFunc, NULL);
  
  /* ストリームシステムの初期化 */
  stmInit();
  
  g_start_flag = FALSE;
  
  play_no = 0;
  read_no = 0;
  next_start = ON;
  
  
  for (;;) {
    /* ストリームオープン */
    if (next_start == ON) {
      read_no = read_no % FILE_MAX;
      /* Audio開始 */
      playAudio(read_no);
      next_start = OFF;
    }
    /* サーバの実行 */
    STM_ExecServer();
    
    /* 再生タスク */
    PCM_Task(NULL);
    
    /* ストリーム読み込み終了判定 */
    if (play_no == read_no && isReadEnd(read_no)) {
      /* 次のオーディオの読み込み開始 */
      read_no++;
      next_start = ON;
    }
    
    /* 再生オーディオの終了判定 */
    if (isAudioEnd(play_no)) {
      /* 次のムービの再生開始 */
      play_no++;
      play_no = play_no % FILE_MAX;
    }
  }
}
