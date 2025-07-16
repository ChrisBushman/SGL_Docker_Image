/*******************************************************************
*
*                       Cinepak for SATURN Player 
*                                by SOJ
*                         usage sample program
*
*           メモリ上にあるムービファイルの指定フレーム画像を展開する
*           展開するフレーム画像は先頭から最後まで展開した後,
*           最後から先頭へ展開する。
*           ムービファイルは 0x00200000 番地にロードしておくこと
*           使用できるムービのファイルサイズは１ＭＢまで。
*
*                      Copyright(c) 1994,1995 SEGA
*
*   Comment: main module
*   File   : SMPCPK7\main.c
*   Date   : 1994-10-31
*   Author : Y.T
*   Date   : 1997-09-03  A.H(SOJ) 半角カナコードを削除
*
*******************************************************************/
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
#include "sega_cpk.h"
#include "sega_dma.h"

static void hintProc(void);
static void smpVblIn(void);
static void smpVblOut(void);


/*------------------------- 《マクロ定数》 -------------------------*/

/* スプライト面のＶＲＡＭのアドレス */
#define ADDR_VDP1 			(0x25C00000)

/* ＶＲＡＭの転送先のアドレス */
#define ADDR_VRAM 			((void *)0x25C08000)

/* ウェーブＲＡＭの転送アドレスとサンプル数 */
#define PCM_ADDR			((void*)0x25a20000)
#define PCM_SIZE			(4096L*16)

/* ワークバッファのサイズ */
#define WORK_BUF_SIZ		(1024L*1024L*16)

/* ＴＶ画面サイズ */
#define DISP_XSIZE			(320)
#define DISP_YSIZE			(224)


/*----------------------- 《グローバル変数》 -----------------------*/

/* ワークバッファ */
static Uint32 g_movie_work[CPK_24WORK_DSIZE];

/* リングバッファ */
static Uint32 *g_movie_buf = (Uint32 *)0x00200000;

/* 画像展開バッファ */
static Uint32 g_decode_buf[DISP_XSIZE * DISP_YSIZE];

/* ムービのサイズ */
static Uint32 movie_x, movie_y;

/* 総サンプル数 */
static Sint32 g_sample_total;

/* Ｈブランクカウンタ */
static volatile long	g_hint_cnt;

/* サイクルパターン */
static Uint16	cycle_tbl[]={
	0x4444,0x4444,
	0xffff,0xffff,
	0x4444,0x4444,
	0xffff,0xffff
};

static volatile Sint32 g_vint_cnt;

/*---------------------------- 《関数》 ----------------------------*/

/********************************************************************/
/* エラーが発生した時に呼ばれる関数									*/
/********************************************************************/
void errCpkFunc(void *obj, Sint32 errcode)
{
	/* エラー処理 */
}

/*====================== 画面表示の処理 ===========================*/
static void memsetDword(Uint32 *dst, Uint32 value, Sint32 cnt)
{
	dst	+= cnt;
	while (--cnt >= 0) {
		*--dst = value;
	}
}

static void dispSclInit(void)
{
	SclVramConfig	tp;
	SclConfig		scfg;
	int				i;
	Uint16			BackCol;

	memsetDword((Uint32 *)SCL_VDP2_VRAM_A, 0x80000000, 512L * 512L/2);

   /*******************************************
    *	ＶＤＰ２の初期化                      *
    *******************************************/
	SCL_Vdp2Init();

	/* V-Blank割り込みルーチンの登録 */
	INT_ChgMsk(INT_MSK_NULL,INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);
	INT_SetScuFunc(INT_SCU_VBLK_IN, smpVblIn);
	INT_SetScuFunc(INT_SCU_VBLK_OUT, smpVblOut);
	INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT,INT_MSK_NULL);

	/* Ｈブランクの設定 */
	INT_ChgMsk(INT_MSK_NULL, INT_MSK_HBLK_IN);
	INT_SetScuFunc(INT_SCU_HBLK_IN, hintProc);
	INT_ChgMsk(INT_MSK_HBLK_IN, INT_MSK_NULL);

	SCL_SetFrameInterval(1);
	set_imask(0);

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

static void hintProc(void)
{
	g_hint_cnt++;

}

/* VRAMサイクルパターン（バンクＡ０）レジスタ */
#define		CYCLE_A_REG		0x25f80010

/* VRAMサイクルパターン（バンクＢ０）レジスタ */
#define		CYCLE_B_REG		0x25f80018

/* ＣＰＵリード／ライトモードにする */
#define		CYCLE_CPU_WRITE(reg_addr)	\
	{ \
	*((Uint16 *)(reg_addr)) = 0xeeee; \
	*((Uint16 *)(reg_addr)+1) = 0xeeee; \
	}

/* キャラクタパターンデータリードモードにする */
#define		CYCLE_VDP_READ(reg_addr) \
	{ \
	*((Uint16 *)(reg_addr)) = 0x4444; \
	*((Uint16 *)(reg_addr)+1) = 0x4444; \
	}


void copyDma(Uint32 *vram_addr)
{
	register int		y;
	register Uint32	*src, *dst;

	/* 上半分の転送 */
	while (g_hint_cnt != (DISP_YSIZE/2 + 2)) ;

	/* バンクＡ０をＣＰＵライトモードにする */
	CYCLE_CPU_WRITE(CYCLE_A_REG);

	src = g_decode_buf;
	dst = vram_addr;
	for (y = 0; y < movie_y/2; y++) {
		DMA_ScuMemCopy(dst, src, movie_x * 4);
		while (DMA_ScuResult() == DMA_SCU_BUSY) ;
		src += movie_x;
		dst += SCL_MAXLINE;
	}
	/* バンクＡ０をキャラクタパターンデータリードにする */
	CYCLE_VDP_READ(CYCLE_A_REG);

	/* 下半分の転送 */
	while (g_hint_cnt >= DISP_YSIZE/2) ;

	/* バンクＢ０をＣＰＵライトモードにする */
	CYCLE_CPU_WRITE(CYCLE_B_REG);

	for ( ; y < movie_y; y++) {
		DMA_ScuMemCopy(dst, src, movie_x * 4);
		while (DMA_ScuResult() == DMA_SCU_BUSY) ;
		src += movie_x;
		dst += SCL_MAXLINE;
	}
	/* バンクＢ０をキャラクタパターンデータリードにする */
	CYCLE_VDP_READ(CYCLE_B_REG);
}


/*====================== Ｖブランクの処理 ===========================*/
static void smpVblIn(void)
{
	/* Ｃｉｎｅｐａｋの VblIn ルーチンをコール */
	CPK_VblIn();

	/* グラフィックライブラリを使用する為には実行しなければならない */
	SCL_VblankStart();
	g_hint_cnt = 0;
	g_vint_cnt++;
}

static void smpVblOut(void)
{
	/* グラフィックライブラリを使用する為には実行しなければならない */
	SCL_VblankEnd();
	g_hint_cnt = 0;
}

/* シネパックの初期化 */
void cpkInit(void)
{
	/* シネパックの初期化 */
	CPK_Init();

	/* エラー関数の設定 */
	CPK_SetErrFunc(errCpkFunc, NULL);
}

/* ムービハンドル生成 */
static CpkHn createMovie(void)
{
	CpkCreatePara	para;
	CpkHeader		*header;
	CpkHn			cpk;

	/* ムービハンドル生成 */
	CPK_PARA_WORK_ADDR(&para) = g_movie_work;
	CPK_PARA_WORK_SIZE(&para) = CPK_24WORK_BSIZE;
	CPK_PARA_BUF_ADDR(&para) = g_movie_buf;
	CPK_PARA_BUF_SIZE(&para) = WORK_BUF_SIZ;
	CPK_PARA_PCM_ADDR(&para) = PCM_ADDR;
	CPK_PARA_PCM_SIZE(&para) = PCM_SIZE;
	cpk = CPK_CreateMemMovie(&para);
	if (cpk == NULL) {
		return;
	}
	/* 表示色数を１６００万色に設定 */
	CPK_SetColor(cpk, CPK_COLOR_24BIT);

	/* メモリ上のムービのファイルサイズを設定 */
	CPK_NotifyWriteSize(cpk, WORK_BUF_SIZ);

	/* ムービのサイズを取得 */
	header = CPK_GetHeader(cpk);
	movie_x = header->width;
	movie_y = header->height;

	/* 総サンプル数を取得 */
	g_sample_total = header->sample_total;

	/* ムービの表示先アドレスを設定 */
	CPK_SetDecodeAddr(cpk, g_decode_buf, 4 * movie_x);

	return cpk;
}

/* ウエイト時間 */
/*
#define WAIT_TIME		(60 * 2)
#define WAIT_TIME		(0)
*/
#define WAIT_TIME		(60 / 4)

void waitVbl(Sint32 wait_time)
{
	Sint32		vint_old = g_vint_cnt;

	while (g_vint_cnt - vint_old < wait_time) {
		;
	}
}

void main(void)
{
	CpkHn 			cpk;
	Uint32			movie_lx, movie_ly;
	Sint32 			frame_no;
	int				i;
	Uint32			*vram_addr;

	/* 変数の初期化 */
	g_vint_cnt = 0;

	/* スクロールの設定 */
	dispSclInit();

	/* ＤＭＡの初期化 */
	DMA_ScuInit();

	/* シネパックの初期化 */
	cpkInit();

	/* ムービハンドル生成 */
	cpk = createMovie();

	/* 表示位置の計算 */
	movie_lx = (DISP_XSIZE - movie_x)/2;
	movie_ly = (DISP_YSIZE - movie_y)/2;
	vram_addr = (Uint32 *)(SCL_VDP2_VRAM + 
				4 * (SCL_MAXLINE * movie_ly + movie_lx)
				+ 4 * SCL_MAXLINE * (SCL_MAXLINE/2 - DISP_YSIZE) / 2);

	/* 画面スクロール */
	SCL_Open(SCL_NBG0);
	SCL_MoveTo(FIXED(0), FIXED((SCL_MAXLINE/2 - DISP_YSIZE) / 2),0);
	SCL_Close();
	SCL_DisplayFrame();

	while (1) {
		for (frame_no = 0; frame_no < g_sample_total; frame_no++) {
			/* 指定番号のフレームを展開する */
			CPK_DecodeFrame(cpk, frame_no);

			/* 画像転送 */
			copyDma(vram_addr);

			waitVbl(WAIT_TIME);
		}
		frame_no--;
		for (; frame_no >= 0; frame_no--) {
			/* 指定番号のフレームを展開する */
			CPK_DecodeFrame(cpk, frame_no);

			/* 画像転送 */
			copyDma(vram_addr);

			waitVbl(WAIT_TIME);
		}
	}
}
