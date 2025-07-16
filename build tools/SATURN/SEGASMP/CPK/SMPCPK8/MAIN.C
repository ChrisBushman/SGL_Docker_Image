/*******************************************************************
*
*                       Cinepak for SATURN Player 
*                                by SOJ
*                         usage sample program
*
*               マルチムービ再生のサンプル
*               SAMPLE0.CPK をまずメモリに読み込み,
*               SAMPLE1.CPK と SAMPLE2.CPK と SAMPLE3.CPK はＣＤからデータを
*               読み込みならが４つのムービを同時に再生する。
*               [注意]
*               ４つのムービは同じコマ数にすること。
*               ４つのムービは同じ画像サイズ(120, 80)にすること。
*               SAMPLE1.CPK と SAMPLE2.CPK と SAMPLE3.CPK はボリュームを０に
*               している。
*               SAMPLE0.CPK は １ＭＢ以下にすること。
*               SAMPLE1.CPK と SAMPLE2.CPK と SAMPLE3.CPK はデータレートを
*               100KB/S 以下にすること。
*               [表示座標]
*               SAMPLE1.CPK (20,  16) - (139,  95)
*               SAMPLE2.CPK (180, 16) - (299,  95)
*               SAMPLE3.CPK (20, 128) - (139, 207)
*               SAMPLE0.CPK (180,128) - (299, 207)
*
*                      Copyright(c) 1994,1995 SEGA
*
*   Comment: main module
*   File   : SMPCPK8.C
*   Date   : 1994-10-04
*   Author : H.G
*
*******************************************************************/
#include <machine.h>
#include <string.h>
#include "sega_stm.h"
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
#include "sega_snd.h"
#include "sega_cpk.h"

static void smpVblIn(void);
static void smpVblOut(void);
static void smpSprEnd(void);


/*------------------------- 《マクロ定数》 -------------------------*/

/* スプライト面のＶＲＡＭのアドレス */
#define ADDR_VDP1 			(0x25C00000)

/* ウェーブＲＡＭの転送アドレスとバイト数 */
#define	PCM_ADDR	(0x25a0c000)
#define	PCM_SIZE	(4096L*14)

/* リングバッファのサイズ */
#define	WORK_BUF_SIZ	(1024L*180)

/* ＴＶ画面のサイズ */
#define   DISP_XSIZE       320
#define   DISP_YSIZE       224

/* メモリにロードするアドレス */
#define	 LOAD_MEM_ADDR 0x200000

/* ロードサイズ */
#define	LOAD_MEM_SIZE  0x100000

/* 再生するムービの数 */
#define	FILE_NUM		4

#define	VRAM_START_SIZE	0x8000
#define	VRAM_START_ADDR	(ADDR_VDP1+VRAM_START_SIZE)
#define	VRAM_CHAR_SIZE	0x18000

/* ボリューム切り替え周期 */
#define VOL_CHANGE_VBL	(3*60)

/*----------------------- 《グローバル変数》 -----------------------*/

/* ウェーブＲＡＭの転送アドレスとバイト数 */
static void *g_pcm_addr[] = {(void *)PCM_ADDR, 
							 (void *)(PCM_ADDR+PCM_SIZE*2),
							 (void *)(PCM_ADDR+PCM_SIZE*4),
							 (void *)(PCM_ADDR+PCM_SIZE*6)};


/* ＶＲＡＭの転送先のアドレス */
static Uint32 *g_vram_addr[] = {(Uint32 *)VRAM_START_ADDR, 
								(Uint32 *)(VRAM_START_ADDR+VRAM_CHAR_SIZE), 
								(Uint32 *)(VRAM_START_ADDR+VRAM_CHAR_SIZE*2),
								(Uint32 *)(VRAM_START_ADDR+VRAM_CHAR_SIZE*3)};

/* ワークバッファ */
static Uint32 g_movie_work[FILE_NUM][CPK_15WORK_DSIZE];

/* リングバッファ */
static Uint8 g_movie_buf[FILE_NUM][WORK_BUF_SIZ];

/* 再生するムービファイル名 */
static char *filename[] = 
	{"SAMPLE1.CPK", "SAMPLE2.CPK", "SAMPLE3.CPK", "SAMPLE0.CPK"};

/* スプライト描画終了判定フラグ   TRUE : 描画終了 */
static volatile Bool g_spr_end;

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

void errCpkFunc(void *obj, Sint32 errcode)
{
	/* エラー処理 */
}



/*====================== 画面表示の処理 ===========================*/
#define HENKEI_SPRITE		(3)
#define	SPRITE_NUM			(4)
#define HENKEI_SPRITE_END	(HENKEI_SPRITE + SPRITE_NUM - 1)

#define	MOVIE_XLEN			120
#define	MOVIE_YLEN			80
#define	MOVIE1_X1			20
#define	MOVIE1_Y1			16
#define MOVIE1_X2			(MOVIE1_X1+MOVIE_XLEN-1)
#define	MOVIE1_Y2			(MOVIE1_Y1+MOVIE_YLEN-1)
#define	MOVIE2_X1			180
#define	MOVIE2_Y1			16
#define MOVIE2_X2			(MOVIE2_X1+MOVIE_XLEN-1)
#define	MOVIE2_Y2			(MOVIE2_Y1+MOVIE_YLEN-1)
#define	MOVIE3_X1			20
#define	MOVIE3_Y1			128
#define MOVIE3_X2			(MOVIE3_X1+MOVIE_XLEN-1)
#define	MOVIE3_Y2			(MOVIE3_Y1+MOVIE_YLEN-1)
#define	MOVIE4_X1			180
#define	MOVIE4_Y1			128
#define MOVIE4_X2			(MOVIE4_X1+MOVIE_XLEN-1)
#define	MOVIE4_Y2			(MOVIE4_Y1+MOVIE_YLEN-1)

#define	MOVIE1_VRAM_ADDR	(VRAM_START_SIZE)/8
#define	MOVIE2_VRAM_ADDR	(VRAM_START_SIZE+VRAM_CHAR_SIZE*1)/8
#define	MOVIE3_VRAM_ADDR	(VRAM_START_SIZE+VRAM_CHAR_SIZE*2)/8
#define	MOVIE4_VRAM_ADDR	(VRAM_START_SIZE+VRAM_CHAR_SIZE*3)/8


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
		/* control  */  (JUMP_NEXT | FUNC_DISTORSP),
		/* link     */  0,
		/* drawMode */  0x04A8,
		/* color    */  0x0000,		/* 黒 */
		/* charAddr */  MOVIE1_VRAM_ADDR,
		/* charSize */  0x2098,
		/* ax, ay   */  MOVIE1_X1,  MOVIE1_Y1,
		/* bx, by   */  MOVIE1_X2,  MOVIE1_Y1,
		/* cx, cy   */  MOVIE1_X2,	MOVIE1_Y2,
		/* dx, dy   */  MOVIE1_X1,	MOVIE1_Y2,
		/* grshAddr */  0,
		/* dummy    */  0
	}, {
		/* control  */  (JUMP_NEXT | FUNC_DISTORSP),
		/* link     */  0,
		/* drawMode */  0x04A8,
		/* color    */  0x0000,		/* 黒 */
		/* charAddr */  MOVIE2_VRAM_ADDR,
		/* charSize */  0x2098,
		/* ax, ay   */  MOVIE2_X1,  MOVIE2_Y1,
		/* bx, by   */  MOVIE2_X2,  MOVIE2_Y1,
		/* cx, cy   */  MOVIE2_X2,	MOVIE2_Y2,
		/* dx, dy   */  MOVIE2_X1,	MOVIE2_Y2,
		/* grshAddr */  0,
		/* dummy    */  0
	}, {
		/* control  */  (JUMP_NEXT | FUNC_DISTORSP),
		/* link     */  0,
		/* drawMode */  0x04A8,
		/* color    */  0x0000,		/* 黒 */
		/* charAddr */  MOVIE3_VRAM_ADDR,
		/* charSize */  0x2098,
		/* ax, ay   */  MOVIE3_X1,  MOVIE3_Y1,
		/* bx, by   */  MOVIE3_X2,  MOVIE3_Y1,
		/* cx, cy   */  MOVIE3_X2,	MOVIE3_Y2,
		/* dx, dy   */  MOVIE3_X1,	MOVIE3_Y2,
		/* grshAddr */  0,
		/* dummy    */  0
	}, {
		/* control  */  (JUMP_NEXT | FUNC_DISTORSP),
		/* link     */  0,
		/* drawMode */  0x04A8,
		/* color    */  0x0000,		/* 黒 */
		/* charAddr */  MOVIE4_VRAM_ADDR,
		/* charSize */  0x2098,
		/* ax, ay   */  MOVIE4_X1,  MOVIE4_Y1,
		/* bx, by   */  MOVIE4_X2,  MOVIE4_Y1,
		/* cx, cy   */  MOVIE4_X2,	MOVIE4_Y2,
		/* dx, dy   */  MOVIE4_X1,	MOVIE4_Y2,
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
	INT_ChgMsk(INT_MSK_NULL,INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);
	INT_SetScuFunc(INT_SCU_VBLK_IN, smpVblIn);
	INT_SetScuFunc(INT_SCU_VBLK_OUT, smpVblOut);
	INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT,INT_MSK_NULL);

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


static void setSprite(Uint32 sizeX, Uint32 sizeY)
{
	Uint32 		luX, luY, rdX, rdY;
	int			sp_no;

	for (sp_no = HENKEI_SPRITE; sp_no <= HENKEI_SPRITE_END; sp_no++) {
		SpriteCmd[sp_no].charSize = (sizeX/8)<<8 | sizeY;
		luX = (320 - sizeX) / 2;
		luY = (224 - sizeY) / 2;
		rdX = luX + sizeX - 1;
		rdY = luY + sizeY - 1;
/*		SpriteCmd[sp_no].ax = luX;
		SpriteCmd[sp_no].ay = luY;
		SpriteCmd[sp_no].bx = rdX;
		SpriteCmd[sp_no].by = luY;
		SpriteCmd[sp_no].cx = rdX;
		SpriteCmd[sp_no].cy = rdY;
		SpriteCmd[sp_no].dx = luX;
		SpriteCmd[sp_no].dy = rdY;
*/
	}
	memcpy((void *)ADDR_VDP1, SpriteCmd, sizeof(SpriteCmd));

}

static void smpSprEnd(void)
{
	g_spr_end = TRUE;
}

/*====================== Ｖブランクの処理 ===========================*/
static Sint32 g_cnt_vblin = 0;
static void smpVblIn(void)
{
	/* Ｃｉｎｅｐａｋの VblIn ルーチンをコール */
	CPK_VblIn();

	/* グラフィックライブラリを使用する為には実行しなければならない */
	SCL_VblankStart();

	g_cnt_vblin++;
}

static void smpVblOut(void)
{
	/* グラフィックライブラリを使用する為には実行しなければならない */
	SCL_VblankEnd();
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


/*====================== ファイルの処理 ===========================*/

/* ルートディレクトリにあるファイルの最大数 */
#define MAX_DIR		100

/* 同時に開くファイルの最大数 */
#define OPEN_MAX	FILE_NUM

/* ストリームグループのＩＤ */
static StmGrpHn grp_hd;

/* ディレクトリ情報管理領域 */
static GfsDirTbl	dir_tbl;

/* ファイル名を含んだ情報 */
static GfsDirName dir_name[MAX_DIR];

/* ＧＦＳの作業領域 */
static Uint8 gfs_work[GFS_WORK_SIZE(OPEN_MAX)];

static	Uint8   stm_work[STM_WORK_SIZE(12, 24)];

static void fileInit(void)
{
	Sint32 file_num;

	/* GFSの初期化 */
	GFS_DIRTBL_TYPE(&dir_tbl) = GFS_DIR_NAME;
	GFS_DIRTBL_DIRNAME(&dir_tbl) = dir_name;
	GFS_DIRTBL_NDIR(&dir_tbl) = MAX_DIR;

    /* ファイルシステムの初期化 */
    file_num = GFS_Init(OPEN_MAX, gfs_work, &dir_tbl);
	if (file_num < 0) {
		return;
	}

	/* ストリームシステムの初期化 */
	STM_Init(12, 24, stm_work);

	/* エラー関数の設定 */
	GFS_SetErrFunc(errGfsFunc, NULL);
	STM_SetErrFunc(errStmFunc, NULL);
}


static StmHn stmOpen(char *fname)
{
    Sint32 fid;
	StmKey key;
	StmHn  stm;

    /* ファイル名からファイル識別子を求める */
    fid = GFS_NameToId(fname);
	STM_KEY_FN(&key) = STM_KEY_CN(&key) = STM_KEY_SMMSK(&key) = 
		STM_KEY_SMVAL(&key) = STM_KEY_CIMSK(&key) = STM_KEY_CIVAL(&key) =
		STM_KEY_NONE;
	return STM_OpenFid(grp_hd, fid, &key, STM_LOOP_NOREAD);
	return stm;
}

static void stmClose(StmHn fp)
{
	STM_Close(fp);
}

/* ファイルをメモリに読み込む */
static Sint32 fileRead(char *filename, void *addr)
{
	Sint32 		fid;
	GfsHn  		gfs;
	Sint32		sctsize, nsct, lastsize;
	Sint32		load_size;

	STM_SetExecGrp(NULL);
	fid = GFS_NameToId(filename);
	gfs = GFS_Open(fid);
	load_size = 0;
	if (gfs != NULL) {
		GFS_GetFileSize(gfs, &sctsize, &nsct, &lastsize);
		load_size = GFS_Fread(gfs, nsct, addr, sctsize * (nsct - 1) +lastsize);
		GFS_Close(gfs);
	}
	STM_SetExecGrp(grp_hd);
	return load_size;
}


/* シネパックの初期化 */
static void cpkInit(void)
{
	/* シネパックの初期化 */
	CPK_Init();

	/* エラー関数の設定 */
	CPK_SetErrFunc(errCpkFunc, NULL);
}

static CpkHn createMemMovie(void)
{
	CpkHn cpk;
	static Uint32 movie_x, movie_y;
	CpkCreatePara	para;
	CpkHeader		*header;
	int				file_no = FILE_NUM-1;

	/* ムービの生成 */
	CPK_PARA_WORK_ADDR(&para) = g_movie_work[file_no];
	CPK_PARA_WORK_SIZE(&para) = CPK_15WORK_BSIZE;
	CPK_PARA_BUF_ADDR(&para) = (Uint32 *)LOAD_MEM_ADDR;
	CPK_PARA_BUF_SIZE(&para) = LOAD_MEM_SIZE;
	CPK_PARA_PCM_ADDR(&para) = g_pcm_addr[file_no];
	CPK_PARA_PCM_SIZE(&para) = PCM_SIZE;
	cpk = CPK_CreateMemMovie(&para);
	if (cpk == NULL) {
		return NULL;
	}

	/* 表示色数の設定 */
	CPK_SetColor(cpk, CPK_COLOR_15BIT);

	/* メモリ上のムービのファイルサイズを設定 */
	CPK_NotifyWriteSize(cpk, LOAD_MEM_SIZE);

	/* ムービのサイズを取得 */
	header = CPK_GetHeader(cpk);
	movie_x = header->width;
	movie_y = header->height;
	setSprite(movie_x, movie_y);

	CPK_SetDecodeAddr(cpk, g_vram_addr[file_no], 2 * movie_x);
	CPK_SetPcmStreamNo(cpk, file_no);
	return cpk;
}

static CpkHn createMovie(StmHn stm, int file_no)
{
	static Uint32 movie_x, movie_y;
	CpkCreatePara	para;
	CpkHeader		*header;
	CpkHn			cpk;

	/* ムービ生成 */
	CPK_PARA_WORK_ADDR(&para) = g_movie_work[file_no];
	CPK_PARA_WORK_SIZE(&para) = CPK_15WORK_BSIZE;
	CPK_PARA_BUF_ADDR(&para) = (Uint32 *)g_movie_buf[file_no];
	CPK_PARA_BUF_SIZE(&para) = WORK_BUF_SIZ;
	CPK_PARA_PCM_ADDR(&para) = g_pcm_addr[file_no];
	CPK_PARA_PCM_SIZE(&para) = PCM_SIZE;
	cpk = CPK_CreateStmMovie(&para, stm);
	if (cpk == NULL) {
		return NULL;
	}

	/* 表示色数の設定 */
	CPK_SetColor(cpk, CPK_COLOR_15BIT);
	CPK_SetLoadNum(cpk, 7);

	if (file_no == 0) {
		STM_MovePickup(stm, 0);

		/* ヘッダを読み込む */
		CPK_PreloadHeader(cpk);

		/* ムービのサイズを取得 */
		header = CPK_GetHeader(cpk);
		movie_x = header->width;
		movie_y = header->height;
		setSprite(movie_x, movie_y);

	}
	CPK_SetDecodeAddr(cpk, g_vram_addr[file_no], 2 * movie_x);
	CPK_SetPcmStreamNo(cpk, file_no);
	CPK_SetVolume(cpk, 0);
	return cpk;
}

/* ボリュームの切り替え */
void changeVolume(CpkHn cpk[])
{
	if ((g_cnt_vblin % VOL_CHANGE_VBL) == 0) {

		switch ((g_cnt_vblin / VOL_CHANGE_VBL) % 8) {
		case 0:
			CPK_SetVolume(cpk[0], 7);
			CPK_SetVolume(cpk[1], 0);
			CPK_SetVolume(cpk[2], 0);
			CPK_SetVolume(cpk[3], 0);
			break;
		case 1:
			CPK_SetVolume(cpk[0], 5);
			CPK_SetVolume(cpk[1], 5);
			CPK_SetVolume(cpk[2], 0);
			CPK_SetVolume(cpk[3], 0);
			break;
		case 2:
			CPK_SetVolume(cpk[0], 0);
			CPK_SetVolume(cpk[1], 7);
			CPK_SetVolume(cpk[2], 0);
			CPK_SetVolume(cpk[3], 0);
			break;
		case 3:
			CPK_SetVolume(cpk[0], 0);
			CPK_SetVolume(cpk[1], 5);
			CPK_SetVolume(cpk[2], 5);
			CPK_SetVolume(cpk[3], 0);
			break;
		case 4:
			CPK_SetVolume(cpk[0], 0);
			CPK_SetVolume(cpk[1], 0);
			CPK_SetVolume(cpk[2], 7);
			CPK_SetVolume(cpk[3], 0);
			break;
		case 5:
			CPK_SetVolume(cpk[0], 0);
			CPK_SetVolume(cpk[1], 0);
			CPK_SetVolume(cpk[2], 5);
			CPK_SetVolume(cpk[3], 5);
			break;
		case 6:
			CPK_SetVolume(cpk[0], 0);
			CPK_SetVolume(cpk[1], 0);
			CPK_SetVolume(cpk[2], 0);
			CPK_SetVolume(cpk[3], 7);
			break;
		case 7:
			CPK_SetVolume(cpk[0], 5);
			CPK_SetVolume(cpk[1], 0);
			CPK_SetVolume(cpk[2], 0);
			CPK_SetVolume(cpk[3], 5);
			break;
		}
	}
}

void main(void)
{
	CpkHn 			cpk[FILE_NUM];
	StmHn			stm[FILE_NUM];
	volatile	Uint32			restart;
	volatile	int				i;
	volatile	Bool			frame_change_flag;
	volatile	Bool			start_flag;
	volatile	Bool			all_stop;

	/* 変数の初期化 */
	g_spr_end = FALSE;
	start_flag = FALSE;

	/* スプライトの設定 */
	dispInit();

	/* ファイル初期化 */
	fileInit();

	/* サウンドの設定 */
	sndInit();

	/* シネパックの初期化 */
	cpkInit();

	/* ストリームグループのオープン */
	grp_hd = STM_OpenGrp();
	if (grp_hd == NULL) {
		return;
	}
	STM_SetLoop(grp_hd, STM_LOOP_DFL, STM_LOOP_ENDLESS);
	STM_SetExecGrp(grp_hd);

	/* メモリ読み込み */
	if (fileRead(filename[FILE_NUM-1], (void *)LOAD_MEM_ADDR) <= 0) {
		return;
	}

	restart = 1;

	while (1) {
		if (restart) {
			/* メモリ再生ムービの生成 */
			if ((cpk[FILE_NUM-1] = createMemMovie()) == NULL) {
				while(1) ;
			}
			for (i = 0; i < FILE_NUM-1; i++) {
				/* ストリームオープン */
				if ((stm[i] = stmOpen(filename[i])) == NULL) {
					while(1) ;
				}
			}
			for (i = 0; i < FILE_NUM-1; i++) {
				/* ムービの生成 */
				if ((cpk[i] = createMovie(stm[i], i)) == NULL) {
					while(1) ;
				}
			}

			/* ムービ開始 */
			for (i = 0; i <FILE_NUM-1; i++) {
				CPK_Start(cpk[i]);
			}

			/* ムービの再生処理 */
			while(start_flag == FALSE) {
				for (i = 0; i <FILE_NUM-1; i++) {
					CPK_Task(cpk[i]);
					if (CPK_GetPlayStatus(cpk[0]) == CPK_STAT_PLAY_TIME &&
						CPK_GetPlayStatus(cpk[1]) == CPK_STAT_PLAY_TIME &&
						CPK_GetPlayStatus(cpk[2]) == CPK_STAT_PLAY_TIME ) {
						start_flag = TRUE;
					}
				}
			}
			start_flag = FALSE;
			CPK_Start(cpk[FILE_NUM-1]);
			while(1) {
				CPK_Task(cpk[FILE_NUM-1]);
				if (CPK_GetPlayStatus(cpk[FILE_NUM-1]) == CPK_STAT_PLAY_TIME) {
					break;
				}
			}

			restart = 0;
		}

		/* ムービの再生処理 */
		for (i = 0; i <FILE_NUM; i++) {
			CPK_Task(cpk[i]);
		}

		/* 画面表示要求のチェック */
		frame_change_flag = TRUE;
		for (i = 0; i < FILE_NUM; i++) {
			if (CPK_GetPlayStatus(cpk[i]) == CPK_STAT_PLAY_TIME) {
				if (CPK_IsDispTime(cpk[i]) == FALSE) {
					frame_change_flag = FALSE;
				}
			}
		}

		if (frame_change_flag == TRUE) {
			/* フレームバッファの切り替え */
			SCL_DisplayFrame();

			/* ＶＲＡＭからフレームバッファへの描画終了待ち */
			g_spr_end = FALSE;
			while (g_spr_end == FALSE) ;
			for (i = 0; i <FILE_NUM; i++) {
				CPK_CompleteDisp(cpk[i]);
			}
		}

		/* ボリュームの切り替え */
		changeVolume(cpk);

		/* ムービの終了判定 */
		all_stop = TRUE;
		for (i = 0; i < FILE_NUM; i++) {
			if (CPK_GetPlayStatus(cpk[i]) != CPK_STAT_PLAY_END) {
				all_stop = FALSE;
			}
		}

		if (all_stop) {
			restart = 1;

			for (i = 0; i <FILE_NUM-1; i++) {
				/* ムービの放棄 */
				CPK_DestroyStmMovie(cpk[i]);

				/* ストリームのクローズ*/
				stmClose(stm[i]);
			}

			if (CPK_GetPlayStatus(cpk[FILE_NUM-1]) == CPK_STAT_PLAY_END) {
				CPK_DestroyMemMovie(cpk[FILE_NUM-1]);
			}
		}
	}
}
