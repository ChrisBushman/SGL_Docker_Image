/*****************************************************************************
 *
 *                       Cinepak for SATURN Player 
 *                                by SOJ
 *                         usage sample program
 *                                for SGL
 *
 *
 *		ファイルシステムを使ってＣＤ上のムービファイルを再生するサンプル
 *                       ムービファイル名は "SAMPLE.CPK"
 *
 *                      Copyright(c) 1994,1995 SEGA
 *
 *                gcc version cygnus-2.7-95q3-SOA-951018
 *                ld version 2.5-95q3 (with BFD 2.5-95q3)
 *                  SGL2.0A for 3rd Party Version
 *        GNU Make version 3.71, by Richard Stallman and Roland McGrath.
 *
 *****************************************************************************/

/*--------------------------------------------------------------------------*/
/*								 Include									*/
/*--------------------------------------------------------------------------*/
#include	"sgl.h"
#include	"sgl_cd.h"
#include	"sega_cpk.h"
#include	"sega_snd.h"
#include	"sega_int.h"
#include	"sega_tim.h"
/*--------------------------------------------------------------------------*/
/*								 Prototype									*/
/*--------------------------------------------------------------------------*/

void timer0func(void) ;
CpkHn createMovie(GfsHn gfs) ;
void cpkInit(void) ;
void fileInit(void) ;
GfsHn fileOpen(char *fname) ;
void fileClose(GfsHn fp) ;
void sndInit(void) ;
void errGfsFunc(void *obj, Sint32 ec) ;
void errCpkFunc(void *obj, Sint32 ec) ;
void smpVblIn(void) ;

/*--------------------------------------------------------------------------*/
/*								 Define										*/
/*--------------------------------------------------------------------------*/
#define	WIDTH_H					320
#define	WIDTH_V					224

/* VRAMサイクルパターン（バンクＡ０）レジスタ */
#define		CYCLE_A_REG			0x25f80010

/* VRAMサイクルパターン（バンクＢ０）レジスタ */
#define		CYCLE_B_REG			0x25f80018

/* ＣＰＵリード／ライトモードにする */
#define		CYCLE_CPU_WRITE(reg_addr)	\
{ \
    *((Uint32 *)(reg_addr)) = 0xeeeeeeee; \
  }

/* キャラクタパターンデータリードモードにする */
#define		CYCLE_VDP_READ(reg_addr) \
{ \
    *((Uint32 *)(reg_addr)) = 0x44444444; \
  }

#define	CYCPAT_BM_READ			0x44444444
#define	CYCPAT_CPU_RW			0xffffffff

#define	RING_BUF_SIZ			(1024L*200)

/* ウェーブＲＡＭの転送アドレスとサンプル数 */
#define	PCM_ADDR				((void*)0x25a20000)
#define	PCM_SIZE				(4096L*16)

/*--------------------------------------------------------------------------*/
/*								 Extern										*/
/*--------------------------------------------------------------------------*/
extern Uint8 PauseFlag ;
/*--------------------------------------------------------------------------*/
/*								Structure									*/
/*--------------------------------------------------------------------------*/
volatile Sint32 Switch_VBL_IN = TRUE ;
volatile Sint32 Switch_timer0 = TRUE ;
Uint32 movie_x ;
Uint32 movie_x_size ;
Uint32 movie_y_half ;
Uint32 movie_y ;
Uint8 * src ;
Uint8 * dst ;
void * cpk_vdp2_dst ;

Uint32 work_ram_buf[320L * 224L * 4 / 4];
Uint32 g_movie_work[CPK_24WORK_DSIZE]; /* Work Buffer */
Uint32 g_movie_buf[RING_BUF_SIZ / sizeof(Uint32)]; /* Ring Buffer */
     char *g_filenaem = "SAMPLE.CPK";
     
     /*--------------------------------------------------------------------------*/
     /*								Main Routin									*/
     /*--------------------------------------------------------------------------*/
void ss_main(void)
{
  volatile CpkHn cpk;
  volatile GfsHn gfs;
  volatile Uint32	restart;
  register Sint32 iu ;

  slInitSystem(TV_320x224,NULL,1);

  slTVOff();
  slBitMapNbg0(COL_TYPE_1M,BM_512x256,(void *)VDP2_VRAM_A0);
  slColRAMMode(CRM16_1024);
  slScrAutoDisp(NBG0ON);
  slTVOn();

  slScrCycleSet(	CYCPAT_BM_READ,	CYCPAT_BM_READ,
		CYCPAT_BM_READ,	CYCPAT_BM_READ);

  slScrPosNbg0(toFIXED(0),toFIXED((512/2 - WIDTH_V)/2)); /* x , y */

  slVRAMMode((Uint16)NULL) ;

  slSynch();

  /* ブランキング開始割り込みスクロールのデータ転送の転送を禁止 */
  PauseFlag = -1 ;

  slIntFunction((void*)smpVblIn) ;

  fileInit() ;

  sndInit();

  cpkInit();

  restart = 1;

  /* タイマ０割り込みの設定 */
  INT_SetScuFunc(INT_SCU_TIM0, timer0func);

  /* タイマ０の設定 */
  TIM_T0_SET_CMP(WIDTH_V/2 + 2);
  TIM_T1_SET_MODE(0x101);
  TIM_T0_ENABLE();


  while (1) {
    if (restart) {

      /* ファイルオープン */
      if ((gfs = fileOpen(g_filenaem)) == NULL) {
	slPrint("Error File Open" , slLocate(9,3));
	while( -1 ) slSynch();
      }

      /* ムービハンドル生成 */
      cpk = createMovie(gfs);

      /* ムービ開始 */
      CPK_Start(cpk);

      restart = 0;
    }

    CPK_Task(cpk) ;

    /* 画面表示要求のチェック */
    if (CPK_IsDispTime(cpk) == TRUE) {

	#if 1
      Switch_timer0 = TRUE ;
      while(Switch_timer0) ;
      Switch_VBL_IN = TRUE ;

      src = (Uint8*)work_ram_buf ;
      dst = (Uint8*)cpk_vdp2_dst ;

	#if 0
      /* バンクＡ０をＣＰＵライトモードにする */
      slScrCycleSet(	CYCPAT_CPU_RW,	CYCPAT_CPU_RW,
		    CYCPAT_BM_READ,	CYCPAT_BM_READ);
	#endif
      /* バンクＡ０をＣＰＵライトモードにする */
      CYCLE_CPU_WRITE(CYCLE_A_REG);

      for (iu=0; iu<movie_y_half; iu++) {
	DMA_ScuMemCopy(dst, src, movie_x_size) ;
	src += movie_x_size ;
	dst += 2048 ;
      }
	#if 0
      /* バンクＡ０をキャラクタパターンデータリードにする */
      slScrCycleSet(	CYCPAT_BM_READ,	CYCPAT_BM_READ,
		    CYCPAT_BM_READ,	CYCPAT_BM_READ);
	#endif
      /* バンクＡ０をキャラクタパターンデータリードにする */
      CYCLE_VDP_READ(CYCLE_A_REG);

      while(Switch_VBL_IN) ;

	#if 0
      /* バンクＢ０をＣＰＵライトモードにする */
      slScrCycleSet(	CYCPAT_BM_READ,	CYCPAT_BM_READ,
		    CYCPAT_CPU_RW,	CYCPAT_CPU_RW);
	#endif
      /* バンクＢ０をＣＰＵライトモードにする */
      CYCLE_CPU_WRITE(CYCLE_B_REG);

      for ( ; iu<movie_y; iu++) {
	DMA_ScuMemCopy(dst, src, movie_x_size) ;
	src += movie_x_size ;
	dst += 2048 ;
      }
	#if 0
      /* バンクＢ０をキャラクタパターンデータリードにする */
      slScrCycleSet(	CYCPAT_BM_READ,	CYCPAT_BM_READ,
		    CYCPAT_BM_READ,	CYCPAT_BM_READ);
	#endif
      /* バンクＢ０をキャラクタパターンデータリードにする */
      CYCLE_VDP_READ(CYCLE_B_REG);
	#endif

      /* 表示完了の通知 */
      CPK_CompleteDisp(cpk);
    }
    /* ムービの終了判定 */
    if (CPK_GetPlayStatus(cpk) == CPK_STAT_PLAY_END) {

      /* 最終フレームを表示するための待ち */
      /* フレームバッファの切り替え待ち */

      /* ムービの放棄 */
      CPK_DestroyGfsMovie(cpk);

      /* ファイルのクローズ*/
      fileClose(gfs);

      restart = 1 ;
    }
  }
}

/*--------------------------------------------------------------------------*/
/*								GFS Routin									*/
/*--------------------------------------------------------------------------*/

/* ルートディレクトリにあるファイルの最大数 */
#define MAX_DIR		100

/* 同時に開くファイルの最大数 */
#define OPEN_MAX	5

/* ディレクトリ情報管理領域 */
GfsDirTbl	dir_tbl;

/* ファイル名を含んだ情報 */
GfsDirName dir_name[MAX_DIR];

/* ＧＦＳの作業領域 */
Uint8 gfs_work[GFS_WORK_SIZE(OPEN_MAX)];
     
void errGfsFunc(void *obj, Sint32 ec)
{
  slPrint("Error Gfs Func" , slLocate(9,3));
  slPrintHex( ec, slLocate( 26, 3 ) );
  while( -1 ) slSynch();
  /* エラー処理 */
}

void fileInit(void)
{
  Sint32 file_num;

  /* GFSの初期化 */
  GFS_DIRTBL_TYPE(&dir_tbl) = GFS_DIR_NAME;
  GFS_DIRTBL_DIRNAME(&dir_tbl) = dir_name;
  GFS_DIRTBL_NDIR(&dir_tbl) = MAX_DIR;

  /* ファイルシステムの初期化 */
  file_num = GFS_Init(OPEN_MAX, gfs_work, &dir_tbl);
  if (file_num < 0) {
    slPrint( "Error GFS Init", slLocate( 26, 3 ) );
    while( -1 ) slSynch();
  }

  /* エラー関数の設定 */
  GFS_SetErrFunc(errGfsFunc, NULL);
}

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

/*--------------------------------------------------------------------------*/
/*								 VBL Routin									*/
/*--------------------------------------------------------------------------*/
void smpVblIn(void)
{
  /* Ｃｉｎｅｐａｋの VblIn ルーチンをコール */
  CPK_VblIn();
  Switch_VBL_IN = FALSE ;
}

/*--------------------------------------------------------------------------*/
/*								 Cinepak Routin								*/
/*--------------------------------------------------------------------------*/
void errCpkFunc(void *obj, Sint32 ec)
{
  slPrint("Error Cpk Func" , slLocate(9,3));
  slPrintHex( ec, slLocate( 26, 3 ) );
  while( -1 ) slSynch();
  /* エラー処理 */
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
CpkHn createMovie(GfsHn gfs)
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
  cpk = CPK_CreateGfsMovie(&para, gfs);
  if (cpk == NULL) {
    return;
  }

  /* 表示色数を16m色に設定 */
  CPK_SetColor(cpk, CPK_COLOR_24BIT);

  /* ヘッダを読み込む */
  /* ムービのサイズが予めわかっている場合は CPK_Preload と */
  /* CPK_GetHeader は呼ぶ必要ない */
  /* CPK_PreloadHeader() を呼ばない方が再生の開始が若干早くなる */
  CPK_PreloadHeader(cpk);

  /* ムービのサイズを取得 */
  header = CPK_GetHeader(cpk);
  movie_x = header->width;
  movie_y = header->height;
  movie_x_size = movie_x *4 ;
  movie_y_half = movie_y/2 ;

  cpk_vdp2_dst = 	(Uint8*)(0x25e00000 + 
				 4 * (512 * ((WIDTH_V - movie_y)/2) + 
				      ((WIDTH_H - movie_x)/2)) + 
				 4 * 512 * (512/2 - WIDTH_V) / 2);

  /* ムービの展開アドレスを設定 */
  CPK_SetDecodeAddr(cpk, (void *)work_ram_buf, 4 * movie_x);

  return cpk;
}

/*--------------------------------------------------------------------------*/
/*								 Sound Routin								*/
/*--------------------------------------------------------------------------*/
extern Uint32 sddrvsize ;
extern Uint32 bootsndsize ;
extern char sddrvstsk[];
extern char bootsnd[];

void sndInit(void)
{
#if 0
  slInitSound((void*)sddrvstsk, sddrvsize, (void*)bootsnd, bootsndsize) ;
  *(volatile Uint8 *)(0x25a004e1) = 0x00 ; /* Hand Shake Off */
#else
  SndIniDt 	snd_init;

  SND_INI_PRG_ADR(snd_init) 	= (Uint16 *)sddrvstsk;
  SND_INI_PRG_SZ(snd_init) 	= (Uint16 )sddrvsize;
  SND_INI_ARA_ADR(snd_init) 	= (Uint16 *)bootsnd;
  SND_INI_ARA_SZ(snd_init) 	= (Uint16 )bootsndsize;
  SND_Init(&snd_init);
  SND_ChgMap(0);
#endif
}

void timer0func(void)
{
  Switch_timer0 = FALSE ;
}

