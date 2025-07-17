/*******************************************************************
*
*                       Cinepak for SATURN Player 
*                                by SOJ
*                         usage sample program
*
*                 クロマキー再生のサンプル
*                 4000000h番地にロードしたムービを後ろ
*                 4400000h番地にロードしたムービを手前
*
*                      Copyright(c) 1994,1995 SEGA
*
*   Comment: main module
*   File   : SMPCPK9.c
*   Date   : 1994-10-04
*   Author : Y.T
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
#include "sega_per.h"
#include "sega_snd.h"
#include "sega_cpk.h"


static void smpVblIn(void);
static void smpVblOut(void);
static void smpSprEnd(void);


/*------------------------- 《マクロ定数》 -------------------------*/

/* スプライト面のＶＲＡＭのアドレス */
#define ADDR_VDP1 			(0x25C00000)

/* ＶＲＡＭの転送先のアドレス */
#define ADDR_VRAM 			((void *)0x25C08000)
#define ADDR_VRAM_CKEY 		((void *)0x25C2B000)

/* ウェーブＲＡＭの転送アドレスとサンプル数 */
#define PCM_ADDR			((void*)0x25a0c000)
#define PCM_SIZE			(4096L*14)
#define PCM_ADDR_CKEY		((void*)0x25a44000)
#define PCM_SIZE_CKEY		(4096L*14)

/* ワークバッファのサイズ */
#define WORK_BUF_SIZ		(1024L*1024L*16) 

/* ＴＶ画面サイズ */
#define DISP_XSIZE			(320)
#define DISP_YSIZE			(224)


/*----------------------- 《グローバル変数》 -----------------------*/

/* ワークバッファ */
static Uint32 g_movie_work[CPK_24WORK_DSIZE];
static Uint32 g_movie_work_ckey[CPK_24WORK_DSIZE];

/* リングバッファ */
static Uint32 *g_movie_buf = (Uint32 *)0x04000000;
static Uint32 *g_movie_buf_ckey = (Uint32 *)0x04400000;

/* スプライト描画終了判定フラグ   TRUE : 描画終了 */
static volatile Bool g_spr_end;

/* キーアウト範囲 */
Sint32 g_keyout_range;

static Uint32 g_vint_cnt;
static Uint32 g_vint_old;

/********************************************************************/
/* エラーが発生した時に呼ばれる関数									*/
/********************************************************************/
void errGfsFunc(void *obj, Sint32 ec)
{
	/* エラー処理 */
}

void errCpkFunc(void *obj, Sint32 errcode)
{
	/* エラー処理 */
}


/*====================== ファイルの処理 ===========================*/

/* ルートディレクトリにあるファイルの最大数 */
#define MAX_DIR		100

/* 同時に開くファイルの最大数 */
#define OPEN_MAX	5

/* ディレクトリ情報管理領域 */
static GfsDirTbl	dir_tbl;

/* ファイル名を含んだ情報 */
static GfsDirName dir_name[MAX_DIR];

/* ＧＦＳの作業領域 */
GfsMng g_gfs_work;
Uint8 g_gfs_work2[GFS_WORK_SIZE(OPEN_MAX)-sizeof(GfsMng)];

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
		return;
	}

	/* エラー関数の設定 */
	GFS_SetErrFunc(errGfsFunc, NULL);
}


/*====================== 画面表示の処理 ===========================*/
#define HENKEI_SPRITE		(4)

static SprSpCmd SpriteCmd[] = {
	{
		/* control  */ (JUMP_NEXT | FUNC_SCLIP),
		/* link     */  0,
		/* drawMode */  0,
		/* color    */  0,
		/* charAddr */  0,
		/* charSize */  0,
		/* ax, ay   */  0, 0,
		/* bx, by   */  0, 0,
		/* cx, cy   */  351, 223,
		/* dx, dy   */  0, 0,
		/* grshAddr */  0,
		/* dummy    */  0
	}, {
		/* control  */ (JUMP_NEXT | FUNC_LCOORD),
		/* link     */  0,
		/* drawMode */  0,
		/* color    */  0,
		/* charAddr */  0,
		/* charSize */  0,
		/* ax, ay   */  0, 0,
		/* bx, by   */  0, 0,
		/* cx, cy   */  0, 0,
		/* dx, dy   */  0, 0,
		/* grshAddr */  0,
		/* dummy    */  0
	}, {
		/* control  */ (JUMP_NEXT | FUNC_UCLIP),
		/* link     */  0,
		/* drawMode */  0,
		/* color    */  0,
		/* charAddr */  0,
		/* charSize */  0,
		/* ax, ay   */  0, 0,
		/* bx, by   */  0, 0,
		/* cx, cy   */  351, 223,
		/* dx, dy   */  0, 0,
		/* grshAddr */  0,
		/* dummy    */  0
   }, {
		/* control  */ 	(JUMP_NEXT | ZOOM_NOPOINT | DIR_NOREV | FUNC_POLYGON),
	    /* link     */  0,
	    /* drawMode */ (ECDSPD_DISABLE | COLOR_0 | COMPO_REP),
	    /* color    */  0,
	    /* charAddr */  0,
	    /* charSize */  0,
	    /* ax, ay   */  0, 0,
	    /* bx, by   */  DISP_XSIZE - 1, 0,
	    /* cx, cy   */  DISP_XSIZE - 1, DISP_YSIZE - 1,
	    /* dx, dy   */  0, DISP_YSIZE - 1,
	    /* grshAddr */  0,
	    /* dummy    */  0
	}, {
		/* control  */  (JUMP_NEXT | FUNC_DISTORSP),
		/* link     */  0,
		/* drawMode */  0x04E8,
		/* color    */  0x0000,		/* 黒 */
		/* charAddr */  0x1000,
		/* charSize */  0x2098,
		/* ax, ay   */  0x0020,         0x0020,
		/* bx, by   */  0x0020 + 255,   0x0020,
		/* cx, cy   */  0x0020 + 255,   0x0020 + 151,
		/* dx, dy   */  0x0020,         0x0020 + 151,
		/* grshAddr */  0,
		/* dummy    */  0
	}, {
		/* control  */  (JUMP_NEXT | FUNC_DISTORSP),
		/* link     */  0,
		/* drawMode */  0x04A8,
		/* color    */  0x0000,		/* 黒 */
		/* charAddr */  0x5600,
		/* charSize */  0x2098,
		/* ax, ay   */  0x0020,         0x0020,
		/* bx, by   */  0x0020 + 255,   0x0020,
		/* cx, cy   */  0x0020 + 255,   0x0020 + 151,
		/* dx, dy   */  0x0020,         0x0020 + 151,
		/* grshAddr */  0,
		/* dummy    */  0
	}, {
		/* [ 0]     */
		/* control  */ 	(CTRL_END),
		/* link     */ 	0,
		/* drawMode */ 	0,
		/* color    */ 	0,
		/* charAddr */ 	0,
		/* charSize */ 	0,
		/* ax, ay   */  0,   0,
		/* bx, by   */  0,   0,
		/* cx, cy   */  0,   0,
		/* dx, dy   */  0,   0,
		/* rshAddr */   0,
		/* dummy    */  0
	}, {
		/* [ 0]     */
		/* control  */ 	(CTRL_END),
		/* link     */ 	0,
		/* drawMode */ 	0,
		/* color    */ 	0,
		/* charAddr */ 	0,
		/* charSize */ 	0,
		/* ax, ay   */  0,   0,
		/* bx, by   */  0,   0,
		/* cx, cy   */  0,   0,
		/* dx, dy   */  0,   0,
		/* rshAddr */   0,
		/* dummy    */  0
	}
};

void eraseVram(void)
{
	Sint32 *addr = (Sint32 *)(0x25C80000);	/* end of VDP1 VRAM */

	do {
		*--addr = 0x80008000;
	} while ((Sint32)addr > 0x25C00000);	/* start of VDP1 VRAM */
}

static void dispInit(void)
{
    Uint8  *VRAM;
	Uint16			BackCol;

    set_imask(0);
    SCL_Vdp2Init();
    SCL_SetPriority(SCL_SP0|SCL_SP1|SCL_SP2|SCL_SP3|
                    SCL_SP4|SCL_SP5|SCL_SP6|SCL_SP7,7);
    SCL_SetSpriteMode(SCL_TYPE1,SCL_MIX,SCL_SP_WINDOW);

	/* Ｖブランクの設定 */
	INT_ChgMsk(INT_MSK_NULL, INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);

	/* V_Blank Out 割り込みを待つ */
	(*((volatile Uint32 *)0x25fe00a4)) &= 0xfffffffc;	/* まずクリア */
	while( !((*((volatile Uint32 *)0x25fe00a4)) & 2) );

	INT_SetScuFunc(INT_SCU_VBLK_IN, smpVblIn);
	INT_SetScuFunc(INT_SCU_VBLK_OUT, smpVblOut);
	INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT, INT_MSK_NULL);

	/* スプライト描画終了割り込みの設定 */
	INT_ChgMsk(INT_MSK_NULL,INT_MSK_SPR);
	INT_SetScuFunc(INT_SCU_SPR, smpSprEnd);
	INT_ChgMsk(INT_MSK_SPR, INT_MSK_NULL);

	BackCol = 0x0000;
	SCL_SetBack(SCL_VDP2_VRAM+0x80000-2,1,&BackCol);

    SPR_Initial(&VRAM);
    SPR_SetEraseData(RGB16_COLOR(0,0,0),0,0,DISP_XSIZE-1,DISP_YSIZE-1);
	SCL_SetFrameInterval(1);
	SCL_DisplayFrame();
	SCL_DisplayFrame();
	SCL_SetFrameInterval(-1);

	eraseVram();
}

static void setSprite(Uint32 command, Uint32 sizeX, Uint32 sizeY)
{
	Uint32 		luX, luY, rdX, rdY;

	SpriteCmd[command].charSize = (sizeX/8)<<8 | (sizeY - 1);
	luX = (320 - sizeX) / 2;
	luY = (224 - sizeY) / 2;
	rdX = luX + sizeX - 1;
	rdY = luY + sizeY - 1;
	SpriteCmd[command].ax = luX;
	SpriteCmd[command].ay = luY;
	SpriteCmd[command].bx = rdX;
	SpriteCmd[command].by = luY;
	SpriteCmd[command].cx = rdX;
	SpriteCmd[command].cy = rdY;
	SpriteCmd[command].dx = luX;
	SpriteCmd[command].dy = rdY;
	memcpy((void *)ADDR_VDP1, SpriteCmd, sizeof(SpriteCmd));

}


/*====================== Ｖブランクの処理 ===========================*/
static void smpVblIn(void)
{
	/* Ｃｉｎｅｐａｋの VblIn ルーチンをコール */
	CPK_VblIn();

	/* グラフィックライブラリを使用する為には実行しなければならない */
	SCL_VblankStart();

	g_vint_cnt++;
}

static void smpVblOut(void)
{

	/* グラフィックライブラリを使用する為には実行しなければならない */
	SCL_VblankEnd();
}

static void smpSprEnd(void)
{
	g_spr_end = TRUE;
}

/*====================== サウンドの処理 ===========================*/
#define SDDRVS_TSK_SIZE			(0x6000)
#define BOOTSND_MAP_SIZE		(0x0100)
Sint32 sddrvs_tsk[SDDRVS_TSK_SIZE / 4];
Sint32 bootsnd_map[BOOTSND_MAP_SIZE / 4];

static Sint32 fileLoad(Sint8 *name, void *addr, Sint32 bsize)
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

static void sndInit(void)
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


/* シネパックの初期化 */
void cpkInit(void)
{
	/* シネパックの初期化 */
	CPK_Init();

	/* エラー関数の設定 */
	CPK_SetErrFunc(errCpkFunc, NULL);
}

/* ムービハンドル生成 */
void create_movie(CpkHn *cpk, Uint32 *g_movie_work, Uint32 *g_movie_buf)
{
	CpkHeader		*header;
	Uint32 			movie_x, movie_y;
	CpkCreatePara	para;

	/* ムービ生成 */
	CPK_PARA_WORK_ADDR(&para) = g_movie_work;
	CPK_PARA_WORK_SIZE(&para) = CPK_24WORK_BSIZE;
	CPK_PARA_BUF_ADDR(&para) = g_movie_buf;
	CPK_PARA_BUF_SIZE(&para) = WORK_BUF_SIZ;
	CPK_PARA_PCM_ADDR(&para) = PCM_ADDR;
	CPK_PARA_PCM_SIZE(&para) = PCM_SIZE;

	*cpk = CPK_CreateMemMovie(&para);
	if (*cpk == NULL) {
		return;
	}

	/* 表示色数の設定 */
	CPK_SetColor(*cpk, CPK_COLOR_15BIT);

	/* メモリ上のムービのファイルサイズを設定 */
	CPK_NotifyWriteSize(*cpk, WORK_BUF_SIZ);

	/* ムービのサイズを取得 */
	header = CPK_GetHeader(*cpk);
	movie_x = header->width;
	movie_y = header->height;
	setSprite(HENKEI_SPRITE, movie_x, movie_y);

	/* ムービの表示先アドレスを設定 */
	CPK_SetDecodeAddr(*cpk, ADDR_VRAM, 2 * movie_x);
	CPK_SetVolume(*cpk, 0);

}

void create_movie_ckey(CpkHn *cpk, Uint32 *g_movie_work, Uint32 *g_movie_buf)
{
	CpkHeader		*header;
	Uint32 			movie_x, movie_y;
	CpkCreatePara	para;

	/* ムービ生成 */
	CPK_PARA_WORK_ADDR(&para) = g_movie_work;
	CPK_PARA_WORK_SIZE(&para) = CPK_24WORK_BSIZE;
	CPK_PARA_BUF_ADDR(&para) = g_movie_buf;
	CPK_PARA_BUF_SIZE(&para) = WORK_BUF_SIZ;
	CPK_PARA_PCM_ADDR(&para) = PCM_ADDR_CKEY;
	CPK_PARA_PCM_SIZE(&para) = PCM_SIZE_CKEY;

	*cpk = CPK_CreateMemMovie(&para);
	if (*cpk == NULL) {
		return;
	}

	/* 表示色数の設定 */
	CPK_SetColor(*cpk, CPK_COLOR_15BIT);

	/* メモリ上のムービのファイルサイズを設定 */
	CPK_NotifyWriteSize(*cpk, WORK_BUF_SIZ);

	/* ムービのサイズを取得 */
	header = CPK_GetHeader(*cpk);
	movie_x = header->width;
	movie_y = header->height;
	setSprite(HENKEI_SPRITE + 1, movie_x, movie_y);

	/* ムービの表示先アドレスを設定 */
	CPK_SetDecodeAddr(*cpk, ADDR_VRAM_CKEY, 2 * movie_x);

	CPK_SetPcmStreamNo(*cpk, 1);
/*	CPK_SetPcmCmdBlockNo(*cpk, 1); */
/*	CPK_SetVolume(*cpk, 0); */
}


void main(void)
{
	CpkHn 			cpk1, cpk_ckey;
	Uint32			create_movie_flag;
	Uint32			create_movie_ckey_flag;

	/* 変数の初期化 */
	g_spr_end = FALSE;
	g_keyout_range = 21;
	g_vint_cnt = 0;
	g_vint_old = 0;
	create_movie_flag = 1;
	create_movie_ckey_flag = 1;

	/* スプライトの設定 */
	dispInit();

	/* ファイル初期化 */
	fileInit();

	/* サウンドの設定 */
	sndInit();

	/* シネパックの初期化 */
	cpkInit();

	while (1) {
		if (create_movie_flag) {
			create_movie(&cpk1, g_movie_work, g_movie_buf);
			CPK_Start(cpk1);
			create_movie_flag = 0;
		}
		if (create_movie_ckey_flag) {
			create_movie_ckey(&cpk_ckey, g_movie_work_ckey, g_movie_buf_ckey);
			/* キーアウト範囲の設定 */
			CPK_SetKeyOutRange(cpk_ckey, g_keyout_range);
			CPK_Start(cpk_ckey);
			create_movie_ckey_flag = 0;
		}

		/* ムービの再生処理 */
		CPK_Task(cpk1);
		CPK_Task(cpk_ckey);

		/* 画面表示要求のチェック */
		if (CPK_IsDispTime(cpk1) == TRUE) {
			/* フレーム切り替え */
			SCL_DisplayFrame();
			/* ＶＲＡＭからフレームバッファへの描画終了待ち */
			g_spr_end = FALSE;
			while (g_spr_end == FALSE) ;
			CPK_CompleteDisp(cpk1);
		}
		if (CPK_IsDispTime(cpk_ckey) == TRUE) {
			CPK_CompleteDisp(cpk_ckey);
		}

		/* ムービの終了判定 */
		if (CPK_GetPlayStatus(cpk1) == CPK_STAT_PLAY_END) {
			CPK_DestroyMemMovie(cpk1);
			create_movie_flag = 1;
		}
		if (CPK_GetPlayStatus(cpk_ckey) == CPK_STAT_PLAY_END) {
			CPK_DestroyMemMovie(cpk_ckey);
			create_movie_ckey_flag = 1;
		}
	}
}
