/*******************************************************************
 *
 *                       Cinepak for SATURN Player 
 *                                by SOJ
 *                         usage sample program
 *
 *　　　　　ストリームシステムを使ってＣＤ上のムービファイルを
 *　　　　　スムーズに連続再生するサンプル
 *          ムービファイル名は CPKFILE1と、CPKFILE2に 定義．
 *          ＣＤ上に CPKFILE1、CPKFILE2 の順に配置すること。
 *
 *                      Copyright(c) 1994,1995 SEGA
 *
 *   Comment: main module
 *   File   : SMPCPK5.C
 *   Date   : 1994-10-31
 *   Author : H.G
 *
 *******************************************************************/

/* ムービーファイル名 */
#if 0
#define CPKFILE1	"SAMPLE01.CPK"
#define CPKFILE2	"SAMPLE02.CPK"
#else
#define CPKFILE1	"GINGA.CPK"
#define CPKFILE2	"KIKAN.CPK"
#endif

#if 1
/* 高水準PerLibを使用する場合 (add by a.h) */
	#define UseHighPerLib
	#define UsePerLib
#else
/* 低水準PerLibを使用する場合 (add by a.h) */
	#define UseLowPerLib
	#define UsePerLib
#endif

/* いったん WORK RAM H に展開し、VRAMへDMA転送する */
#define OUTPUT_WORK_RAM

/* シネパックライブラリにスレーブＣＰＵを使わせる。 */
#define DUAL_CPU

/* 再生開始トリガサイズの設定を行う */
/* #define START_TRG_SIZE		 (RING_BUF_SIZ - 24*2048) */
#define START_TRG_SIZE		 (30 * 2048)

#include "machine.h"
#include <string.h>
#include "sega_xpt.h"
#include "sega_sys.h"
#include "sega_def.h"
#include "sega_mth.h"
#include "sega_scl.h"
#include "sega_int.h"
#define  _SPR2_
#include "sega_spr.h"
#include "sega_dma.h"
#include "sega_cdc.h"
#include "sega_gfs.h"
#include "sega_stm.h"
#include "sega_snd.h"
#include "sega_cpk.h"
#include "sega_dbg.h"  /* add by A.H */

#ifdef UseHighPerLib
	#include "per_x.h"  /* add by A.H */
	#include "sega_per.h"
#endif
#ifdef UseLowPerLib
	#include "sega_per.h"
#endif

static void smpVblIn(void);
static void smpVblOut(void);
static void smpSprEnd(void);

/*------------------------- 《マクロ定数》 -------------------------*/

/* スプライト面のＶＲＡＭのアドレス */
#define ADDR_VDP1		(0x25C00000)

/* ＶＲＡＭの転送先のアドレス */
#define ADDR_VRAM_CPK	(0x25C08000)

/* ウェーブＲＡＭの転送アドレスとサンプル数 */
#define	PCM_ADDR		((void*)0x25a20000)
#define	PCM_SIZE		(4096L*16)

/* リングバッファのサイズ */
#ifdef NONE_COMPRESS_VIDEO
	#define	RING_BUF_SIZ	(1024L*300)
#else
	#define	RING_BUF_SIZ	(1024L*200)
#endif

/* ＴＶ画面のサイズ */
#define DISP_XSIZE		(320)
#define DISP_YSIZE		(224)

/* 再生するムービの数 */
#define	FILE_NUM		(2)

/* ボリュームの最小値，最大値 */
#define LEVEL_MIN		(0)
#define LEVEL_MAX		(7)

/* パンの最小値，最大値，中央値 */
#define PAN_MIN			(0)					/* 左端：左は最大、右はゼロ */
#define PAN_MAX			(31)				/* 右端：右は最大、左はゼロ */
#define PAN_CENTER		((PAN_MAX + 1) / 2)	/* 中央：左も、右も最大 	*/

/* ピックアップ移動の判定マージン [セクタ] */
#define MOVE_PICKUP_MARGIN		(20)

/*----------------------- 《グローバル変数》 -----------------------*/

#ifdef UseHighPerLib  /* PER高水準lib */
	static SysPort	*__port;
	/* デバイス情報 */
	const static SysDevice	*device;
	/*  PAD トリガ情報  */
	trigger_t  trigger, trigger_old=!(0);
#endif
#ifdef UseLowPerLib 			/* PER基本lib */
	#define		PERDATANUM		(6)
	#define		PERDATASIZE		(2)
	#define		PERWORKSIZE		((PERDATANUM * (PERDATASIZE + 2) * 2) + PERDATASIZE)
	volatile Uint8     perdatawork[PERWORKSIZE] ;
	static PerGetPer *output_dt;
	static PerMulInfo *mul_info;
	typedef struct  {
		Uint8	type;
		Uint8	size;
		Uint8	data[PERDATASIZE];
	}OutputPerDataDGT;
	OutputPerDataDGT	*perdataP1;
	OutputPerDataDGT	*perdata_old = (OutputPerDataDGT *)perdatawork;
#endif

/* ワークバッファ */
static Uint32 g_movie_work[FILE_NUM][CPK_15WORK_DSIZE];

/* リングバッファ */
static Uint32 g_movie_buf[FILE_NUM][RING_BUF_SIZ / sizeof(Uint32)];

/* ムービの強制切り替えフラグ */
volatile Bool g_change_flag;

/* 再生するムービファイル名 */
char *filename[] = { CPKFILE1, CPKFILE2  };

/* スプライト描画終了判定フラグ   TRUE : 描画終了 */
static volatile Bool g_spr_end;

/* いったん出力するワークラムのバッファ */
#ifdef OUTPUT_WORK_RAM
	Uint32 work_ram_buf[320L * 256L / 2];
	Uint32 cp_size;
#endif

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

/* フレームバッファの切り替えと、描画終了待ち */
void smp_WaitDisplayFrame(void)
{
	/* ＶＲＡＭからフレームバッファへの(前回の)描画終了待ち */
	while (g_spr_end == FALSE) ;

	/* フレームバッファの切り替え */
	SCL_DisplayFrame();

	g_spr_end = FALSE;
}

void smp_DisplayFrameWait(void)
{
#if 0
	/* フレームバッファの切り替え */
	SCL_DisplayFrame();

	g_spr_end = FALSE;

	/* ＶＲＡＭからフレームバッファへの(前回の)描画終了待ち */
	while (g_spr_end == FALSE) ;
#else
	/* ＶＲＡＭからフレームバッファへの(前回の)描画終了待ち */
	while (g_spr_end == FALSE) ;

	/* フレームバッファの切り替え */
	SCL_DisplayFrame();

	g_spr_end = FALSE;
#endif
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
		/* drawMode */  0x04A8,
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
	Uint32	imask;

	imask = get_imask();
    set_imask(15);
    SCL_Vdp2Init();
    SCL_SetPriority(SCL_SP0|SCL_SP1|SCL_SP2|SCL_SP3|
                    SCL_SP4|SCL_SP5|SCL_SP6|SCL_SP7,7);
    SCL_SetSpriteMode(SCL_TYPE1,SCL_MIX,SCL_SP_WINDOW);
	SCL_SetColRamMode(SCL_CRM15_2048);  /* Add by A.H */

/* Ｖブランクの設定 */
	/* V_BLANK割込みマスク */
	INT_ChgMsk(INT_MSK_NULL,INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);

	/* V_Blank Out 割り込みstatus flag を監視 */
	(*((volatile Uint32 *)0x25fe00a4)) &= 0xfffffffc;	/* まずクリア */
	while( !((*((volatile Uint32 *)0x25fe00a4)) & 2) );

	INT_SetScuFunc(INT_SCU_VBLK_IN, smpVblIn);
	INT_SetScuFunc(INT_SCU_VBLK_OUT, smpVblOut);
	INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT,INT_MSK_NULL);
/* Ｖブランクの設定終了 */


	/* スプライト描画終了割り込みの設定 */
	INT_ChgMsk(INT_MSK_NULL,INT_MSK_SPR);
	INT_SetScuFunc(INT_SCU_SPR, smpSprEnd);
	INT_ChgMsk(INT_MSK_SPR, INT_MSK_NULL);

	set_imask(imask);  /* Add by A.H */

	SCL_SetFrameInterval( 1 );   /* Add by A.H */
	
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
#ifdef UseHighPerLib
	PER_GetPort( __port );
#else
	PER_LGetPer( &output_dt, &mul_info);
#endif
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


/*====================== ファイルの処理 ===========================*/

/* ルートディレクトリにあるファイルの最大数 */
#define MAX_DIR		500

/* 同時に開くファイルの最大数 */
#define OPEN_MAX	(FILE_NUM + 3)

/* ストリームグループのＩＤ */
static StmGrpHn grp_hd;

/* ディレクトリ情報管理領域 */
static GfsDirTbl	dir_tbl;

/* ファイル名を含んだ情報 */
static GfsDirName dir_name[MAX_DIR];

/* ＧＦＳの作業領域 */
static Uint8 gfs_work[GFS_WORK_SIZE(OPEN_MAX)];

/* ＳＴＭの作業領域 */
static Uint8 stm_work[STM_WORK_SIZE(12, 24)];

static void fileInit(void)
{
	Sint32 file_num;

    /* ファイルシステムの初期化 */
	GFS_DIRTBL_TYPE(&dir_tbl) = GFS_DIR_NAME;
	GFS_DIRTBL_DIRNAME(&dir_tbl) = dir_name;
	GFS_DIRTBL_NDIR(&dir_tbl) = MAX_DIR;
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
	stm = STM_OpenFid(grp_hd, fid, &key, STM_LOOP_NOREAD);
	return stm;
}

static void stmClose(StmHn fp)
{
	STM_Close(fp);
}


static void goNextStm(StmHn stm)
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
	
	if (fad + MOVE_PICKUP_MARGIN < STM_FRANGE_SFAD(&frange)) {
		/* ピックアップの移動 */
		STM_MovePickup(stm, 0);
	}
}

/* シネパックの初期化 */
void cpkInit(void)
{
	/* シネパックの初期化 */
	CPK_Init();

	/* エラー関数の設定 */
	CPK_SetErrFunc(errCpkFunc, NULL);

#ifdef DUAL_CPU
	/* シネパックライブラリにスレーブＣＰＵを使わせる */
	CPK_SetCpu(CPK_CPU_DUAL);
#endif
}

/* ムービハンドル生成 */
static CpkHn createMovie(StmHn stm, int file_no)
{
	static Uint32 	movie_x, movie_y;
	CpkCreatePara	para;
	CpkHeader		*header;
	CpkHn			cpk;

	/* ムービ生成 */
	CPK_PARA_WORK_ADDR(&para) = g_movie_work[file_no];
	CPK_PARA_WORK_SIZE(&para) = CPK_15WORK_BSIZE;
	CPK_PARA_BUF_ADDR(&para) = g_movie_buf[file_no];
	CPK_PARA_BUF_SIZE(&para) = RING_BUF_SIZ;
	CPK_PARA_PCM_ADDR(&para) = PCM_ADDR;
	CPK_PARA_PCM_SIZE(&para) = PCM_SIZE;
	cpk = CPK_CreateStmMovie(&para, stm);
	if (cpk == NULL) {
		return NULL;
	}

	/* 表示色数を３２０００色に設定 */
	CPK_SetColor(cpk, CPK_COLOR_15BIT);

	if (file_no == 0) {
		/* ピックアップの移動 */
		STM_MovePickup(stm, 0);

		/* ヘッダを読み込む */
		CPK_PreloadHeader(cpk);

		/* ムービのサイズを取得 */
		header = CPK_GetHeader(cpk);
		movie_x = header->width;
		movie_y = header->height;
		setSprite(movie_x, movie_y);
	}

	/* ムービの展開アドレスを設定 */
#ifdef OUTPUT_WORK_RAM
	CPK_SetDecodeAddr(cpk, (void *)work_ram_buf, 2 * movie_x);
	cp_size = (Uint32)(movie_x * movie_y * 2);
#else
	CPK_SetDecodeAddr(cpk, (void *)ADDR_VRAM_CPK, 2 * movie_x);
#endif

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
	StmHn			stm[FILE_NUM];
	CpkHn			cpk[FILE_NUM];
	Uint32			restart;
	Sint32			is_keyframe;
	int				i;
	Uint16  dummy_pad = 0;

#ifdef OUTPUT_WORK_RAM
	DMA_ScuInit();
#endif

	/* 変数の初期化 */
	g_spr_end = FALSE;

	/* PAD 初期化 */
#ifdef UseHighPerLib	/*  PER高水準  */
		__port = PER_OpenPort();
#endif
#ifdef UseLowPerLib		/*  PER低水準  */
		PER_LInit( PER_KD_PER, PERDATANUM, PERDATASIZE, (Uint8 *)perdatawork, 1 );
#endif

	/* スプライトとスクロールの設定 */
	dispInit();
	DBG_Initial( &dummy_pad, RGB16_COLOR(31,31,31), 0 );

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

	restart = 1;

	while (1) {
		if (restart) {

			for (i = 0; i < FILE_NUM; i++) {
				/* ストリームオープン */
				while ((stm[i] = stmOpen(filename[i])) == NULL);
				/* ムービハンドル生成 */
				while ((cpk[i] = createMovie(stm[i], i)) == NULL);
			}

			/* ムービ開始 */
			CPK_Start(cpk[0]);

			/* 次に再生するムービの登録 */
			CPK_EntryNext(cpk[1]);
			restart = 0;
		} /* if (restart)  */
		
#ifndef UsePerLib
		/* ムービの再生処理 */
		CPK_Task(cpk[0]);
		CPK_Task(cpk[1]);
#endif

		is_keyframe = 0;

		for (i = 0; i < FILE_NUM; i++) {
#ifdef UsePerLib
			CPK_Task(cpk[i]) ;	/* ムービの再生処理 */
#endif
			/* 画面表示要求のチェック */
			if (CPK_IsDispTime(cpk[i]) == TRUE) {

#ifdef OUTPUT_WORK_RAM
				DMA_ScuMemCopy((void *)ADDR_VRAM_CPK, 
					(void *)work_ram_buf, cp_size);
#endif
				/* 表示完了の通知 */
				CPK_CompleteDisp(cpk[i]);
			}
#ifdef UseHighPerLib
			/*  1P デバイス情報取得  */
			device = PER_GetDeviceA( &__port[0], 0 );
			/*  デバイス接続チェック */
			if( device != NULL && PER_GetType( device ) != 0x20 ){
				trigger = PER_GetTrigger( device );
				/*  トリガ情報 チェック  */
				if( PER_GetPressEdge( trigger_old, trigger ) & TRG_A ){
					g_change_flag = TRUE ;
				}
				trigger_old = trigger;
			}
#endif
#ifdef UseLowPerLib
			/*  本体端子１の接続チェック  */
			perdataP1 = (OutputPerDataDGT *)output_dt;
			if((mul_info[0].con != PER_MCON_NCON_UNKNOWN) &&
				((perdataP1->type == PER_ID_DGT) ||
				 (perdataP1->type == PER_ID_ANL) ||
				 (perdataP1->type == PER_ID_KBD) ) &&
				(perdataP1->size >= PER_SIZE_DGT)	)
			{
				/*  トリガ情報 チェック  */
				if( (perdataP1->data[0] & PER_LDGT_A)==0 &&
					(perdata_old->data[0] & PER_LDGT_A) )	{
					perdata_old = perdataP1;
					g_change_flag = TRUE ;
				}
			}
#endif
			if (g_change_flag == TRUE) {
				if ( i == (FILE_NUM - 1) ){
					goNextStm(stm[0]);
				}else{
					goNextStm(stm[(i + 1)]);
				}
				if (CPK_CheckChange() == CPK_CHANGE_OK_AT_ONCE) {
					CPK_Change();   /* ムービ強制切換 */
					g_change_flag = FALSE ;
				}
			}
/*		for (i = 0; i < FILE_NUM; i++) のloop end を 移動 */
			/*  ↑＿  SGL版と同じ場所に変更 by a.h */

			/* フレームバッファの切り替えと、描画終了待ち */
			smp_DisplayFrameWait();

			/* ムービの終了判定 */
			if (CPK_GetPlayStatus(cpk[FILE_NUM-1]) == CPK_STAT_PLAY_END) {

				/* 最終フレームを表示するための待ち */
				/* フレームバッファの切り替え */
				SCL_DisplayFrame();

				for (i = 0; i < FILE_NUM; i++) {
				/* ムービの放棄 */
					CPK_DestroyStmMovie(cpk[i]);
					/* ストリームのクローズ*/
					stmClose(stm[i]);
				}
				g_change_flag = FALSE ;
				restart = 1;
			}   /* if (CPK_GetPlayStatus(cpk[FILE_NUM-1]) == CPK_STAT_PLAY_END) */
		} /* for (i = 0; i < FILE_NUM; i++)  */
	} /* while(1) */
} /* main() end */
