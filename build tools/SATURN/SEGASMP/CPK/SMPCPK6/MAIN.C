/*******************************************************************
*
*                       Cinepak for SATURN Player 
*                                by SOJ
*                         usage sample program
*
*		        ファイルシステムを使ってＣＤ上のムービファイルを
*                   ＶＤＰ２に１６００万色で再生するサンプル
*         ムービファイル名を、char *filename に 登録して下さい。
*
*         ストリームシステムを使う場合は #define USE_STM を定義してください。
*
*               注意：
*                   Hint割り込みは使わないこと。
*                   VブランクIn, Out 割り込みは極力短くすること。
*                   データ転送方式は、ソフトウェア転送を選ぶこと。
*
*                      Copyright(c) 1994,1995 SEGA
*
*   Comment: main module
*   File   : SMPCPK6.c
*   Date   : 1994-10-31
*   Author : H.G
*
*******************************************************************/

/* 再生するムービファイル名 */
#if 0
char *filename = "OPENING.CPK";
char *filename = "SAMPLE0.CPK";
char *filename = "SAMPLE01.CPK";
char *filename = "SAMPLE02.CPK";
char *filename = "SAMPLE1.CPK";
char *filename = "SAMPLE2.CPK";
char *filename = "SAMPLE3.CPK";
#else
char *filename = "SAMPLE.CPK";
#endif

#if 0
#define USE_DIR
char *dirname = "MOVE";
#endif

/* ストリームシステムを使う場合に定義する */
/* #define USE_STM */

/* シネパックライブラリにスレーブＣＰＵを使わせる時に定義する。 */
#if 0
#define DUAL_CPU
#endif

/* ディレクトリ移動する際に定義する。 */
/* #define USE_DIR */

/* 再生開始トリガサイズの設定を行う */
/* #define START_TRG_SIZE		 (RING_BUF_SIZ - 24*2048) */
/* #define START_TRG_SIZE		 (30 * 2048) */
#define START_TRG_SIZE		 (100 * 2048)

#include <machine.h>
#include <string.h>
#define _SH
#include "sega_xpt.h"
#include "sega_sys.h"
#include "sega_def.h"
#include "sega_mth.h"
#include "sega_scl.h" 
#include "sega_int.h"
#define  _SPR2_
#include "sega_spr.h"
#include "sega_dma.h"
#include "sega_tim.h"
#include "sega_cdc.h"
#include "sega_gfs.h"
#include "sega_stm.h"
#include "sega_per.h"
#include "sega_snd.h"
#include "sega_cpk.h"



void smpVblIn(void);
void smpVblOut(void);
void timeFunc(void);
void changeDir(char *dir);
void dispSclInit(void);

/*------------------------- 《マクロ定数》 -------------------------*/

/* ウェーブＲＡＭの転送アドレスとサンプル数 */
#define	PCM_ADDR	((void*)0x25a20000)
#define	PCM_SIZE	(4096L*16)

/* リングバッファのサイズ */
#if 1
#define	RING_BUF_SIZ	(1024L*400)
#else
#define	RING_BUF_SIZ	(1024L*1000)
#endif

/* ＴＶ画面のサイズ */
#define   DISP_XSIZE       320
#define   DISP_YSIZE       224

/* ボリュームの最小値，最大値 */
#define LEVEL_MIN	(0)
#define LEVEL_MAX	(7)

/* パンの最小値，最大値，中央値 */
#define PAN_MIN		(0)					/* 左端：左は最大、右はゼロ */
#define PAN_MAX		(31)				/* 右端：右は最大、左はゼロ */
#define PAN_CENTER	((PAN_MAX + 1) / 2)	/* 中央：左も、右も最大 	*/

/* ２分の１ */
#define SMP_DIV2(a)		((a) >> 1)

/* Ｖブランク中に一括転送できる画像サイズの最大 */
#ifndef __GNU__	
	/* SHC */
	#define SMPCPK_VBL_COPY_MAX			(288 * 144)
#else
	/* GCC */
	#define SMPCPK_VBL_COPY_MAX			(288 * 100)
#endif

/*----------------------- 《グローバル変数》 -----------------------*/

/* ワークバッファ */
Uint32 g_movie_work[CPK_24WORK_DSIZE];

/* リングバッファ */
Uint32 g_movie_buf[ ( RING_BUF_SIZ + 3 ) / sizeof(Uint32)];

/* 画像展開バッファ */
Uint32 g_decode_buf[320L*240L*4/4];


/* ムービのサイズ */
Uint32			movie_x, movie_y;

/* タイマ割り込みが入ったら TRUE */
volatile Sint32 g_time_on;

/* ＶブランクＩＮ割り込みが入ったら TRUE */
volatile Sint32 g_vbl_in;

/* サイクルパターン */
Uint16	cycle_tbl[]={
#if 1
	0x4444,0x4444,
	0xffff,0xffff,
	0x4444,0x4444,
	0xffff,0xffff
#else
	0x4444,0x4444,
	0x4444,0x4444,
	0x4444,0x4444,
	0x4444,0x4444,
#endif
};

/* ボリュームの値 */
Sint32 g_level = LEVEL_MAX;

/* パンの値：左右を 0..31 で表現する値 */
Sint32 g_pan = PAN_CENTER;

/* パンの値：引数に指定する値 */
Sint32 g_pan_tbl[PAN_MAX + 1] = {
	0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 
	0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

/*---------------------------- 《関数》 ----------------------------*/

/********************************************************************/
/* エラーが発生した時に呼ばれる関数									*/
/********************************************************************/
void errGfsFunc(void *obj, Sint32 ec)
{
	/* エラー処理 */
}

void errStmFunc(void *obj, Sint32 ec)
{
	/* エラー処理 */
}

void errCpkFunc(void *obj, Sint32 ec)
{
	/* エラー処理 */
}


/*====================== 画面表示の処理 ===========================*/
 void memsetDword(Uint32 *dst, Uint32 value, Sint32 cnt)
{
	Uint32 *dst_stop = dst;

	dst	+= cnt;
	while (dst > dst_stop) {
		*--dst = value;
	}
}

 void dispSclInit(void)
{
	SclVramConfig	tp;
	SclConfig		scfg;
	int				i;
	Uint16			BackCol;

	/* erase VDP2 */
	memsetDword((Uint32 *)SCL_VDP2_VRAM_A, 0x80000000, 512L * 512L/2);

   /*******************************************
    *	ＶＤＰ２の初期化                      *
    *******************************************/
	SCL_Vdp2Init();

	set_imask(15);
	/* V-Blank割り込みルーチンの登録 */
	INT_ChgMsk(INT_MSK_NULL,INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);

	/* V_Blank Out 割り込みを待つ */
	(*((volatile Uint32 *)0x25fe00a4)) &= 0xfffffffc;	/* まずクリア */
	while( !((*((volatile Uint32 *)0x25fe00a4)) & 2) );

	INT_SetScuFunc(INT_SCU_VBLK_IN, smpVblIn);
	INT_SetScuFunc(INT_SCU_VBLK_OUT, smpVblOut);
	INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT,INT_MSK_NULL);

	/* Ｈブランクの設定 */
	SCL_SetFrameInterval(1);
	set_imask(0);

	/* タイマ０割り込みの設定 */
	INT_SetScuFunc(INT_SCU_TIM0, timeFunc);

   /*******************************************
    *	スクロールコンフィグレーションの設定                *
    *******************************************/
	SCL_InitConfigTb(&scfg);
	scfg.dispenbl  = ON;
	scfg.platesize = SCL_PL_SIZE_1X1;
	scfg.bmpsize   = SCL_BMP_SIZE_512X256;
	scfg.coltype   = SCL_COL_TYPE_1M;
	scfg.datatype  = SCL_BITMAP;
	scfg.mapover   = SCL_OVER_2;
	for(i=0;i<16;i++)
		scfg.plate_addr[i] = SCL_VDP2_VRAM_A;
	SCL_SetConfig(SCL_NBG0, &scfg);

   /*******************************************
    *	スクロールコンフィグレーションの設定                *
    *******************************************/
	SCL_InitVramConfigTb(&tp);
	tp.vramModeA = OFF;
	tp.vramModeB = OFF;
	SCL_SetVramConfig(&tp);

   /*******************************************
    *	プライオリティを設定(0～7)                  *
    *******************************************/
	SCL_SetPriority(SCL_SPR,0);
	SCL_SetPriority(SCL_NBG0,7);

	/* サイクルパターンの設定 */
    SCL_SetCycleTable(cycle_tbl);

   /*******************************************
    *	バック画面の色を黒に設定              *
    *******************************************/
	BackCol = 0x0000;
	SCL_SetBack(SCL_VDP2_VRAM+0x80000-2,1,&BackCol);

	SCL_Open(SCL_NBG0);
		SCL_MoveTo(FIXED(0), FIXED(0),0);/* Home Position */
		SCL_Scale(FIXED(1.0), FIXED(1.0));
		SCL_SetRotateViewPoint(160,112,500);
		SCL_SetRotateCenter(160,112,0);
	SCL_Close();
	SCL_DisplayFrame();
}

/*====================== Ｖブランクの処理 ===========================*/
 void smpVblIn(void)
{

	/* Ｃｉｎｅｐａｋの VblIn ルーチンをコール */
	CPK_VblIn();

	/* グラフィックライブラリを使用する為には実行しなければならない */
	SCL_VblankStart();
	g_vbl_in = TRUE;

}


 void smpVblOut(void)
{
	/* グラフィックライブラリを使用する為には実行しなければならない */
	SCL_VblankEnd();
}

/* タイマ０割り込み */
 void timeFunc(void)
{
	g_time_on = TRUE;
}


/* VRAMサイクルパターン（バンクＡ０）レジスタ */
#define		CYCLE_A_REG		0x25f80010

/* VRAMサイクルパターン（バンクＢ０）レジスタ */
#define		CYCLE_B_REG		0x25f80018

/* ＣＰＵリード／ライトモードにする */
#define		CYCLE_CPU_WRITE(reg_addr)	\
	{ \
	*((volatile Uint32 *)(reg_addr)) = 0xeeeeeeee; \
	}

/* キャラクタパターンデータリードモードにする */
#define		CYCLE_VDP_READ(reg_addr) \
	{ \
	*((volatile Uint32 *)(reg_addr)) = 0x44444444; \
	}


#define	DMANOWAIT	0 /* 0 no wait */

/* ２分割転送（ウエイトが多いが、画像サイズが大きい場合は仕方ない） */
 void copyDma2(volatile Uint32 *src, volatile Uint32 *dst, int movie_x, int movie_y)
{
	volatile Sint32		copy_size = 4 * movie_x;
	volatile Uint32 	*src_stop1 = src + movie_x * SMP_DIV2(movie_y);
	volatile Uint32 	*src_stop2 = src + movie_x * movie_y;

	/* 上半分の転送 */
	g_time_on = FALSE;
	while (g_time_on == FALSE) ;  /* 走査線が画面中央にくるまでウェイト */

	g_vbl_in = FALSE;

	/* バンクＡ０をＣＰＵライトモードにする */
	CYCLE_CPU_WRITE(CYCLE_A_REG);

	while (src < src_stop1) {
		DMA_ScuMemCopy( (Uint32 *)dst, (Uint32 *)src, copy_size);
		src += movie_x;
		dst += SCL_MAXLINE;
#if DMANOWAIT
		while (DMA_ScuResult() == DMA_SCU_BUSY) ;
#endif
	}
	/* バンクＡ０をキャラクタパターンデータリードにする */
	CYCLE_VDP_READ(CYCLE_A_REG);

	/* 下半分の転送 */
	while (g_vbl_in == FALSE);

	/* バンクＢ０をＣＰＵライトモードにする */
	CYCLE_CPU_WRITE(CYCLE_B_REG);

	while (src < src_stop2) {
		DMA_ScuMemCopy((Uint32 *)dst, (Uint32 *)src, copy_size);
		src += movie_x;
		dst += SCL_MAXLINE;
#if DMANOWAIT
		while (DMA_ScuResult() == DMA_SCU_BUSY) ;
#endif
	}
	/* バンクＢ０をキャラクタパターンデータリードにする */
	CYCLE_VDP_READ(CYCLE_B_REG);
}


/* 一括転送（Ｖｂｌ中に転送できるサイズの場合） */
 void copyDma1(Uint32 *src, Uint32 *dst, int movie_x, int movie_y)
{
	Sint32		copy_size = 4 * movie_x;
	Uint32 		*src_stop1 = src + movie_x * SMP_DIV2(movie_y);
	Uint32 		*src_stop2 = src + movie_x * movie_y;

	/* 上半分の転送 */
	g_time_on = FALSE;
	while (g_time_on == FALSE);
	g_vbl_in = FALSE;

	/* バンクＡ０，Ｂ０をＣＰＵライトモードにする */
	/* バンクＡ０をＣＰＵライトモードにする */
	CYCLE_CPU_WRITE(CYCLE_A_REG);

	while (src < src_stop1) {
		DMA_ScuMemCopy(dst, src, copy_size);
		src += movie_x;
		dst += SCL_MAXLINE;
#if DMANOWAIT
		while (DMA_ScuResult() == DMA_SCU_BUSY) ;
#endif
	}

	/* バンクＡ０をキャラクタパターンデータリードにする */
	CYCLE_VDP_READ(CYCLE_A_REG);

	/* 下半分の転送 */
	/* ウエイトしない。Ｖｂｌ中に一気に転送する。 */
	/* while (g_vbl_in == FALSE); */

	/* バンクＢ０をＣＰＵライトモードにする */
	CYCLE_CPU_WRITE(CYCLE_B_REG);

	while (src < src_stop2) {
		DMA_ScuMemCopy(dst, src, copy_size);
		src += movie_x;
		dst += SCL_MAXLINE;
#if DMANOWAIT
		while (DMA_ScuResult() == DMA_SCU_BUSY) ;
#endif
	}
	/* バンクＢ０をキャラクタパターンデータリードにする */
	CYCLE_VDP_READ(CYCLE_B_REG);
}

 void copyDma(Uint32 *src, Uint32 *dst, int movie_x, int movie_y)
{
	if ( (movie_x * movie_y) <= SMPCPK_VBL_COPY_MAX) {

		/* 一括転送（Ｖｂｌ中に転送できるサイズの場合） */
		copyDma1(src, dst, movie_x, movie_y);
	} else {

		/* ２分割転送（ウエイトが多いが、画像サイズが大きい場合は仕方ない） */
		copyDma2(src, dst, movie_x, movie_y); 
	}
}


/*====================== サウンドの処理 ===========================*/
#define SDDRVS_TSK_SIZE			(0x6000)
#define BOOTSND_MAP_SIZE		(0x0100)
Sint32 sddrvs_tsk[SDDRVS_TSK_SIZE / 4];
Sint32 bootsnd_map[BOOTSND_MAP_SIZE / 4];

 Sint32 fileLoad(Sint8 *name, void *addr, Sint32 bsize)
{
	Sint32 		fid, i;

	for (i = 0; i < 10; i++) {
		fid = GFS_NameToId(name);
		if (fid >= 0) {
			GFS_Load(fid, 0, addr, bsize);
			return 0;
		}
	}
	return -1;
}

 void sndInit(void)
{
	SndIniDt 	snd_init;

	if (fileLoad("SDDRVS.TSK", (void *)sddrvs_tsk, SDDRVS_TSK_SIZE)) {
		return;
	}
	if (fileLoad("BOOTSND.MAP", (void *)bootsnd_map, BOOTSND_MAP_SIZE)) {
		return;
	}
	SND_INI_PRG_ADR(snd_init) 	= (Uint16 *)sddrvs_tsk;
	SND_INI_PRG_SZ(snd_init) 	= (Uint16 )SDDRVS_TSK_SIZE;
	SND_INI_ARA_ADR(snd_init) 	= (Uint16 *)bootsnd_map;
	SND_INI_ARA_SZ(snd_init) 	= (Uint16)BOOTSND_MAP_SIZE;
	SND_Init(&snd_init);
	SND_ChgMap(0);
}


/*====================== ファイルの処理 ===========================*/

/* ルートディレクトリにあるファイルの最大数 */
#define MAX_DIR		100

/* 同時に開くファイルの最大数 */
#define OPEN_MAX	5

/* ストリームグループのＩＤ */
 StmGrpHn grp_hd;

/* ディレクトリ情報管理領域 */
 GfsDirTbl	dir_tbl;

/* ファイル名を含んだ情報 */
 GfsDirName dir_name[MAX_DIR];

/* ＧＦＳの作業領域 */
Uint8 gfs_work[GFS_WORK_SIZE(OPEN_MAX)];

Uint8   stm_work[STM_WORK_SIZE(OPEN_MAX, OPEN_MAX)];

 void fileInit(void)
{
	Sint32 file_num;

    /* ファイルシステムの初期化 */
	GFS_DIRTBL_TYPE(&dir_tbl) = GFS_DIR_NAME;
	GFS_DIRTBL_DIRNAME(&dir_tbl) = dir_name;
	GFS_DIRTBL_NDIR(&dir_tbl) = MAX_DIR;
	do{
		file_num = GFS_Init(OPEN_MAX, gfs_work, &dir_tbl);
	}while(file_num < 0);

	/* エラー関数の設定 */
	GFS_SetErrFunc(errGfsFunc, NULL);

#ifdef USE_STM
	/* ストリームシステムの初期化 */
	STM_Init(OPEN_MAX, OPEN_MAX, stm_work);

	/* エラー関数の設定 */
	STM_SetErrFunc(errStmFunc, NULL);
#endif

}

 void fileInit2(void)
{
#ifdef USE_DIR
	Sint32 file_num;
	changeDir(dirname);
#endif

#ifdef USE_STM
	/* ストリームグループのオープン */
	grp_hd = STM_OpenGrp();
	if (grp_hd == NULL) {
		return;
	}
	STM_SetExecGrp(grp_hd);
#endif
}

#ifdef USE_STM
 StmHn stmOpen(char *fname)
{
    Sint32 fid;
	StmKey key;

    /* ファイル名からファイル識別子を求める */
    fid = GFS_NameToId(fname);
	STM_KEY_FN(&key) = STM_KEY_CN(&key) = STM_KEY_SMMSK(&key) = 
		STM_KEY_SMVAL(&key) = STM_KEY_CIMSK(&key) = STM_KEY_CIVAL(&key) =
		STM_KEY_NONE;
	return STM_OpenFid(grp_hd, fid, &key, STM_LOOP_NOREAD);
}

 void stmClose(StmHn fp)
{
	STM_Close(fp);
}
#endif



#ifdef USE_DIR

GfsDirTbl	dir_tbl2;
GfsDirName dir_name2[MAX_DIR];

void changeDir(char *dir_name)
{
	Sint32	fid;

	fid = GFS_NameToId(dir_name);

	GFS_DIRTBL_TYPE(&dir_tbl2) = GFS_DIR_NAME;
	GFS_DIRTBL_DIRNAME(&dir_tbl2) = dir_name2;
	GFS_DIRTBL_NDIR(&dir_tbl2) = MAX_DIR;
	GFS_LoadDir(fid, &dir_tbl2);
	GFS_SetDir(&dir_tbl2);
}
#endif

 GfsHn fileOpen(char *fname)
{
    Sint32 fid;

    /* ファイル名からファイル識別子を求める */
    fid = GFS_NameToId(fname);
    return GFS_Open(fid);
}

 void fileClose(GfsHn fp)
{
	GFS_Close(fp);
}

/**********************/
/* シネパックの初期化 */
/**********************/
void cpkInit(void)
{
	/* シネパックの初期化 */
	CPK_Init();

	/* エラー関数の設定 */
	CPK_SetErrFunc(errCpkFunc, NULL);

#ifdef DUAL_CPU
	/* スレーブＣＰＵをシネパックライブラリに使わせる */
	CPK_SetCpu(CPK_CPU_DUAL);
#endif
}

/**********************/
/* ムービハンドル生成 */
/**********************/
#ifdef USE_STM
CpkHn createMovie( StmHn stm )
#else
CpkHn createMovie( GfsHn gfs )
#endif
{
	CpkCreatePara	para;
	CpkHeader		*header;
	CpkHn			cpk;

	/* ムービハンドル生成 */
	CPK_PARA_WORK_ADDR(&para) = g_movie_work;
	CPK_PARA_WORK_SIZE(&para) = CPK_24WORK_BSIZE;
	CPK_PARA_BUF_ADDR(&para) = g_movie_buf;
	CPK_PARA_BUF_SIZE(&para) = RING_BUF_SIZ;
	CPK_PARA_PCM_ADDR(&para) = PCM_ADDR;
	CPK_PARA_PCM_SIZE(&para) = PCM_SIZE;

	do{
#ifdef USE_STM
		cpk = CPK_CreateStmMovie(&para, stm);
#else
		cpk = CPK_CreateGfsMovie(&para, gfs);
#endif
	}while(cpk == NULL);

	/* 転送方式をソフトウェア転送に設定する */
#if 0
	CPK_SetTrModePcm(cpk, CPK_TRMODE_CPU); 
#else
/*	CPK_SetTrModePcm(cpk, CPK_TRMODE_SDMA); */
	CPK_SetTrModeCd(cpk, CPK_TRMODE_SDMA);
/*	CPK_SetTrModeCd(cpk, CPK_TRMODE_CPU); */
#endif

	CPK_SetTrModeCd(cpk, CPK_TRMODE_SCU); 
	CPK_SetLoadNum(cpk, 10);

	/* 表示色数を１６００万色に設定 */
	CPK_SetColor(cpk, CPK_COLOR_24BIT);

	/* ヘッダを読み込む */
	/* ムービのサイズが予めわかっている場合は CPK_Preload と */
	/* CPK_GetHeader は呼ぶ必要ない */
	/* ファイルシステムの場合 CPK_PreloadHeader() を呼ばない方が */
	/* 再生の開始が若干早くなる */
#ifdef USE_STM
	CPK_PreloadHeader(cpk);
#else
	CPK_PreloadHeader(cpk);   /* GFS でも実行しないと 不定値が使用されてしまう */
#endif

	/* ムービのサイズを取得 */
	header = CPK_GetHeader(cpk);
	movie_x = header->width;
	movie_y = header->height;

	/* ムービの展開アドレスを設定 */
	CPK_SetDecodeAddr(cpk, (void *)g_decode_buf, movie_x * 4);

#ifdef START_TRG_SIZE
	/* 再生開始トリガサイズの設定 [byte] */
	CPK_SetStartTrgSize(cpk, START_TRG_SIZE);
#endif

	CPK_SetVolume(cpk, g_level);
	CPK_SetPan(cpk, g_pan_tbl[g_pan]);

	return cpk;
}


void main(void)
{
#ifdef USE_STM
	StmHn			stm;
#else
	GfsHn			gfs;
#endif
	CpkHn 			cpk;
	Uint32			restart;
	Uint32			movie_lx, movie_ly;
	Uint32			*vram_addr;
	char 			*fname;

	/* 変数の初期化 */
	g_time_on = FALSE;
	g_vbl_in = FALSE;

	/* スクロールの設定 */
	dispSclInit();

	/* ファイル初期化 */
	fileInit();

	/* サウンドの設定 */
	sndInit();

	/* ＤＭＡの初期化 */
	DMA_ScuInit();

	/* シネパックの初期化 */
	cpkInit();

	/* ファイル初期化２ */
	fileInit2();

	/* タイマ０の設定 */
	TIM_T0_DISABLE();
	TIM_T0_SET_CMP(DISP_YSIZE/2 + 2);
#if 1
	TIM_T1_SET_MODE(0x101);  /* タイマ１は未使用？ */
#endif
	TIM_T0_ENABLE();

	restart = 1;

	/* erase VDP2 */
	memsetDword((Uint32 *)SCL_VDP2_VRAM_A, 0x80000000, 512L * 512L/2);

	while (1) {
		if (restart) {

#if 1  /*  毎ループ毎に実行すべきか疑問なので移動 A.H(SOJ) */
			/* erase VDP2 */
			memsetDword((Uint32 *)SCL_VDP2_VRAM_A, 0x80000000, 512L * 512L/2);
#endif

			fname = filename;

#ifdef USE_STM
			/* ストリームオープン */
			while ((stm = stmOpen(fname)) == NULL);
			/* ムービハンドル生成 */
			cpk = createMovie(stm);
#else
			/* ファイルオープン */
			while ((gfs = fileOpen(fname)) == NULL);
			/* ムービハンドル生成 */
			cpk = createMovie(gfs);
#endif

			/* 表示位置の計算 */
			movie_lx = (DISP_XSIZE - movie_x)/2;
			movie_ly = (DISP_YSIZE - movie_y)/2;
			vram_addr = (Uint32 *)(SCL_VDP2_VRAM + 
						4 * (SCL_MAXLINE * movie_ly + movie_lx)
						+ 4 * SCL_MAXLINE * (SCL_MAXLINE/2 - DISP_YSIZE) / 2);

			/* 画面スクロール */
			SCL_Open(SCL_NBG0);
			SCL_MoveTo(FIXED(0), FIXED((SCL_MAXLINE/2 - DISP_YSIZE) / 2), 0);
			SCL_Close();
			SCL_DisplayFrame();

			/* ムービ開始 */
			CPK_Start(cpk);

			restart = 0;
		} /*	if (restart) */

		/* ムービの再生処理 */
		CPK_Task(cpk);
		CPK_Task(cpk);

		/* 画面表示要求のチェック */
		if (CPK_IsDispTime(cpk) == TRUE) {

			/* 画像転送 */
			copyDma(g_decode_buf, vram_addr, movie_x, movie_y);

			CPK_CompleteDisp(cpk);

		}

		/* ムービの終了判定 */
		if (CPK_GetPlayStatus(cpk) == CPK_STAT_PLAY_END) {
#ifdef USE_STM
			/* ムービの放棄 */
			CPK_DestroyStmMovie(cpk);
			stmClose(stm);
#else
			/* ムービの放棄 */
			CPK_DestroyGfsMovie(cpk);
			fileClose(gfs);
#endif
			restart = 1;
		}
	} /*	while (1) */
}
