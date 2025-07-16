/*******************************************************************
*
*                       Cinepak for SATURN Player 
*                                by SOJ
*                         usage sample program
*
*	     ストリームシステムを使ってＣＤ上の３つ以上のムービファイルを
*        シームレスに連続再生するサンプルです。
*        ムービファイル名は FILE1, FILE2, FILE3 ・・・
*             char *filename[] = { FILE1, FILE2, FILE3, ....}
*        に定義して下さい。
*
*                        Copyright(c) 1994 SEGA
*
*   Comment: main module
*   File   : SMPCPK12\main.c
*   Date   : 1994-09-08,Ver :1.00,Author : H.G
*   Date   : 1997-03-01,Ver :1.10,Author : A.H
*             サウンドドライバをプログラムに内蔵
*             MAX_DIR を 100 に変更
*             GNU 用の MAKEFILE を追加、動作確認
*   Date   : 1997-09-02,Ver :1.20,Author : A.H
*             サウンドドライバを インクルード形式から、
*             単独の外部オブジェクトをリンクする形式に
*             変更しました。
*               SATURN\SHARE\SDDDRVS.C
*             をコンパイルし、リンクします。
*
*******************************************************************/

/* 再生するムービファイル名 */
#if 1
#define FILE1 "SAMPLE1.CPK"
#define FILE2 "SAMPLE2.CPK"
#define FILE3 "SAMPLE3.CPK"
#else
/* WING WAR のムービーデータ */
#define FILE1 "OPENING.CPK"
#define FILE2 "FUGAKU.CPK"
#define FILE3 "RIKAN.CPK"
#define FILE4 "YAMATO.CPK"
#define FILE5 "GINGA.CPK"
#define FILE6 "ENDING1.CPK"
#define FILE7 "KIKAN.CPK"
#endif

/* 再生するムービファイル名 */
#if 1
static char *filename[] = { FILE1, FILE2, FILE3, };
#else
static char *filename[] = { FILE1, FILE2, FILE3, FILE4, FILE5, FILE6, FILE7, };
#endif

#if 0
#define STATIC static
#else
/* デバッグ効率アップの為 */
#define STATIC
#endif

/* インクルードファイル */
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

#include "sega_snd.h"

static void dispInit(void);
static void setSprite(Uint32 sizeX, Uint32 sizeY);
static void sndInit(void);
static void smpVblIn(void);
static void smpVblOut(void);
static void smpSprEnd(void);
static void fileInit(void);
static StmHn stmOpen(char *fname);
static void stmClose(StmHn fp);
static CpkHn createMovie(StmHn stm, int file_no, Bool start);
static Bool isStmReadEnd(StmHn stm);
static Bool playMovie(int read_no);
static void movieTask(void);
static Bool isReadEnd(int read_no);
static Bool isMovieEnd(int play_no);



/****************************************************************/
/* ロードアドレス 												*/
/****************************************************************/
/* スプライト面のＶＲＡＭのアドレス */
#define ADDR_VDP1 			(0x25C00000)

/* ＶＲＡＭの転送先のアドレス */
#define ADDR_VRAM 				(0x25C08000)

/* ウェーブＲＡＭの転送アドレスとサンプル数 */
#define	PCM_ADDR	((void*)0x25a20000)
#define	PCM_SIZE	(4096L*15)

/* リングバッファのサイズ */
#define	WORK_BUF_SIZ	(1024L*200)

/* ＴＶ画面のサイズ */
#define   DISP_XSIZE       320
#define   DISP_YSIZE       224

/* 同時にオープンするムービの数 */
#define	BUF_NUM		2

/* 配列の大きさ */
#define ARRAY_SIZE(array)	(sizeof(array) / sizeof(array[0]))

/* 再生するムービの数 */
#define	FILE_MAX	(ARRAY_SIZE(filename))

/* ワークバッファ */
static Uint32 g_movie_work[BUF_NUM][CPK_24WORK_DSIZE];

/* リングバッファ */
static Uint32 g_movie_buf[BUF_NUM][WORK_BUF_SIZ / sizeof(Uint32)];

/* スプライト描画終了判定フラグ   TRUE : 描画終了 */
static volatile Bool g_spr_end;


static StmHn			stm[BUF_NUM];
static CpkHn 			cpk[BUF_NUM];
static Bool				g_start_flag; /* 最初のムービを再生開始したら TRUE */

void main(void)
{

	int				play_no, read_no;
	Bool			next_start;

	/* 変数の初期化 */
	g_spr_end = FALSE;
	g_start_flag = FALSE;

	/* サウンドの設定 */
	sndInit();

	/* スプライトの設定 */
	dispInit();

	/* ファイル初期化 */
	fileInit();

	/* シネパックの初期化 */
	CPK_Init();

	play_no = 0;
	read_no = 0;
	next_start = ON;
	for (;;) {
		if (next_start == ON) {
			/* ムービの開始 */
			playMovie(read_no);
			next_start = OFF;
		}

		/* ムービの再生処理 */
		movieTask();

		/* ストリーム読み込み終了判定 */
		if (play_no == read_no && isReadEnd(read_no)) {
			/* 次のムービの読み込み開始 */
			read_no++;
			next_start = ON;
		}

		/* ムービの終了判定 */
		if (isMovieEnd(play_no)) {
			/* 次のムービの再生開始 */
			play_no++;
		}
	}
}


/********************************************************************/
/* ストリームをオープンしムービを生成する							*/
/*   ムービの再生を開始する											*/
/* [引き数]															*/
/*   read_no : 読み込みを開始するムービの通し番号					*/
/* [関数値]                                                         */
/*   TRUE : 生成に成功した											*/
/*   FALSE: 生成に失敗した											*/
/********************************************************************/
static Bool playMovie(int read_no)
{
	int		entry_no;
	int		hd_no;

	/* ストリームオープン */
	entry_no = read_no % FILE_MAX;
	hd_no = read_no % BUF_NUM;
	if ((stm[hd_no] = stmOpen(filename[entry_no])) == NULL) {
		return FALSE;
	}
	/* ムービの生成 */
	if ((cpk[hd_no] = createMovie(stm[hd_no], hd_no, !g_start_flag)) == NULL) {
		return FALSE;
	}

	/* ムービの再生開始 */
	if (g_start_flag == OFF) {
		CPK_Start(cpk[hd_no]);
		g_start_flag = ON;
	} else {
		CPK_EntryNext(cpk[hd_no]);
	}

	return TRUE;
}

/********************************************************************/
/* ストリームの読み込みが終了したか調べる							*/
/* [引き数]															*/
/*   read_no : 読み込みを開始するムービの通し番号					*/
/* [関数値]                                                         */
/*   TRUE : 読み込みが終了した										*/
/*   FALSE: 読み込み中												*/
/********************************************************************/
static Bool isReadEnd(int read_no)
{
	int	hd_no;

	hd_no = read_no % BUF_NUM;
	if (isStmReadEnd(stm[hd_no])) {
		return TRUE;
	}
	return FALSE;
}

/********************************************************************/
/* ムービの再生処理 												*/
/********************************************************************/
static void movieTask(void)
{

	CPK_Task(NULL);
	if (CPK_IsDispTime(NULL) == TRUE) {

		/* フレームバッファの切り替え待ち */
		SCL_DisplayFrame();

		/* ＶＲＡＭからフレームバッファへの描画終了待ち */
		g_spr_end = FALSE;
		while (g_spr_end == FALSE) ;

		CPK_CompleteDisp(NULL);
	}
}

/********************************************************************/
/* ムービの再生が終了したか調べる 									*/
/*  終了していればムービを放棄しストリームをクローズする			*/
/* [引き数]															*/
/*   play_no : 再生しているムービの通し番号							*/
/* [関数値]                                                         */
/*   TRUE : 再生が終了した											*/
/*   FALSE: 再生中である											*/
/********************************************************************/
static Bool isMovieEnd(int play_no)
{
	int hd_no;

	hd_no = play_no % BUF_NUM;
	if (CPK_GetPlayStatus(cpk[hd_no]) == CPK_STAT_PLAY_END) {

		/* ムービの放棄 */
		CPK_DestroyStmMovie(cpk[hd_no]);

		/* ストリームのクローズ*/
		stmClose(stm[hd_no]);

		return TRUE;
	}
	return FALSE;
}

/********************************************************************/
/* ムービを生成する 												*/
/*                                                                  */
/* [引き数]															*/
/*   stm  : ストリームハンドル										*/
/*   buf_no : バッファ番号											*/
/*           0 ～ BUF_NUM-1 の値を取る								*/
/*   start  : 再生する最初のムービの場合 TRUE 						*/
/*            それ以降は FALSE										*/
/* [関数値]                                                         */
/*   ムービハンドル													*/
/*   失敗時は NULL を返す											*/
/********************************************************************/
static CpkHn createMovie(StmHn stm, int buf_no, Bool start)
{
	CpkCreatePara	para;
	CpkHn			cpk;
	CpkHeader		*header;
	static Uint32 movie_x, movie_y;

	/* ムービ生成 */
	CPK_PARA_WORK_ADDR(&para) = g_movie_work[buf_no];
	CPK_PARA_WORK_SIZE(&para) = CPK_24WORK_BSIZE;
	CPK_PARA_BUF_ADDR(&para) = g_movie_buf[buf_no];
	CPK_PARA_BUF_SIZE(&para) = WORK_BUF_SIZ;
	CPK_PARA_PCM_ADDR(&para) = PCM_ADDR;
	CPK_PARA_PCM_SIZE(&para) = PCM_SIZE;
	cpk = CPK_CreateStmMovie(&para, stm);
	if (cpk == NULL) {
		return NULL;
	}

	if (start == TRUE) {
		/* 最初からムービのサイズを取得する */
		/* その以降のムービは同じサイズにすること */
		/* ヘッダを読み込む */
		CPK_PreloadHeader(cpk);

		/* ムービのサイズを取得 */
		header = CPK_GetHeader(cpk);
		movie_x = header->width;
		movie_y = header->height;
		setSprite(movie_x, movie_y);
	}

	/* 表示色数の設定 */
	CPK_SetColor(cpk, CPK_COLOR_15BIT);

	/* 展開先アドレスの設定 */
	CPK_SetDecodeAddr(cpk, (void *)ADDR_VRAM, 2 * movie_x);
	return cpk;
}


/*====================== 画面表示の処理 ===========================*/
#define HENKEI_SPRITE		(3)

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
}


static void setSprite(Uint32 sizeX, Uint32 sizeY)
{
	Uint32 		luX, luY, rdX, rdY;

	SpriteCmd[HENKEI_SPRITE].charSize = (sizeX/8)<<8 | sizeY;
	luX = (320 - sizeX) / 2;
	luY = (224 - sizeY) / 2;
	rdX = luX + sizeX - 1;
	rdY = luY + sizeY - 1;
	SpriteCmd[HENKEI_SPRITE].ax = luX;
	SpriteCmd[HENKEI_SPRITE].ay = luY;
	SpriteCmd[HENKEI_SPRITE].bx = rdX;
	SpriteCmd[HENKEI_SPRITE].by = luY;
	SpriteCmd[HENKEI_SPRITE].cx = rdX;
	SpriteCmd[HENKEI_SPRITE].cy = rdY;
	SpriteCmd[HENKEI_SPRITE].dx = luX;
	SpriteCmd[HENKEI_SPRITE].dy = rdY;
	memcpy((void *)ADDR_VDP1, SpriteCmd, sizeof(SpriteCmd));

}

/*====================== Ｖブランクの処理 ===========================*/
static void smpVblIn(void)
{
	/* Ｃｉｎｅｐａｋの VblIn ルーチンをコール */
	CPK_VblIn();

	/* グラフィックライブラリを使用する為には実行しなければならない */
	SCL_VblankStart();
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

#if 0
	#include "sddrvs.dat"  /* サウンドドライバ(サンプルチェック用Cソース版) */
#else
	extern char sddrvstsk[];
	extern long sddrvsize;
#endif
int sound_map[] = {0x0001,0x0000,0x0001,0x4000,
					0x0102,0x4000,0x0001,0x4000,
					0xffff };
static int errChk;

/*====================== サウンドの処理 ===========================*/
static void sndInit(void)
{
	SndIniDt sys_ini;

    SND_INI_PRG_ADR(sys_ini) = (Uint16 *)&sddrvstsk;
	SND_INI_PRG_SZ(sys_ini) = sddrvsize;
	SND_INI_ARA_ADR(sys_ini) = (Uint16 *)&sound_map;
	SND_INI_ARA_SZ(sys_ini) = sizeof(sound_map);

    SND_Init(&sys_ini);		/* Initialize the sound system */
    
    while(SND_ChgMap(0))
    	if (errChk++ > 512) break;	/* change to the (only) map */
    while(SND_StopPcm(0))
    	if (errChk++ > 512) break;
}


/*====================== ファイルの処理 ===========================*/

/* ルートディレクトリにあるファイルの最大数 */
#define MAX_DIR		100

/* 同時に開くファイルの最大数 */
#define OPEN_MAX	5

/* ストリームグループの最大数 */
#define GRP_MAX		1

/* ストリームグループのＩＤ */
static StmGrpHn grp_hd;

/* ディレクトリ情報管理領域 */
static GfsDirTbl	dir_tbl;

/* ファイル名を含んだ情報 */
static GfsDirName dir_name[MAX_DIR];

/* ＧＦＳの作業領域 */
#if 0
STATIC Uint8 gfs_work[GFS_WORK_SIZE(OPEN_MAX)];
#else
STATIC Uint32 gfs_work[GFS_WORK_SIZE4(OPEN_MAX)];
#endif

/* ストリームの作業領域 */
#if 0
STATIC	Uint8   stm_work[STM_WORK_SIZE(GRP_MAX, OPEN_MAX)];
#else
STATIC	Uint32   stm_work[STM_WORK_SIZE4(GRP_MAX, OPEN_MAX)];
#endif

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
	STM_Init(GRP_MAX, OPEN_MAX, stm_work);

	/* ストリームグループのオープン */
	grp_hd = STM_OpenGrp();
	if (grp_hd == NULL) {
		return;
	}
	STM_SetExecGrp(grp_hd);
}

static StmHn stmOpen(char *fname)
{
    Sint32 fid;
	StmKey key;
	StmHn	stm;

    /* ファイル名からファイル識別子を求める */
    fid = GFS_NameToId(fname);
	STM_KEY_FN(&key) = STM_KEY_CN(&key) = STM_KEY_SMMSK(&key) = 
		STM_KEY_SMVAL(&key) = STM_KEY_CIMSK(&key) = STM_KEY_CIVAL(&key) =
		STM_KEY_NONE;

    stm = STM_OpenFid(grp_hd, fid, &key, STM_LOOP_NOREAD);
	return stm;
}

static void stmClose(StmHn fp)
{
	STM_Close(fp);
}

/********************************************************************/
/* ストリームの終わりまでＣＤバッファに読み込んだか判定する			*/
/* [引き数]                                                         */
/*    stm : ストリームハンドル										*/
/* [関数値]															*/
/*    TRUE : 読み込んだ  FALSE : まだ読み込んでいない				*/
/********************************************************************/
static Bool isStmReadEnd(StmHn stm)
{
	Sint32		fad;
	Sint32		fid;
	StmFrange	frange;
	Sint32		bn;
	StmKey		stmkey;

	/* ストリームの再生範囲を取得する */
	STM_GetInfo(stm, &fid, &frange, &bn, &stmkey);

	/* 再生中のＦＡＤの位置を取得する */
	STM_GetExecStat(grp_hd, &fad);

	if (fad >= (STM_FRANGE_SFAD(&frange) + STM_FRANGE_FASNUM(&frange))) {
		return TRUE;
	} else {
		return FALSE;
	}

}
