/*****************************************************************************
 *
 *                       Cinepak for SATURN Player 
 *                                by SOJ
 *                         usage sample program
 *                                for SGL
 *
 *
 *		ファイルシステムを使ってＣＤ上のムービファイルを再生するサンプル
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

#define		USED_DMA_SCU_RESULT			0
#define		NOT_USED_DMA_SCU_RESULT		1
#define		MD_DMA_SCU_RESULT			NOT_USED_DMA_SCU_RESULT

#if MD_DMA_SCU_RESULT == USED_DMA_SCU_RESULT
#define DMA_SCU_BUSY    2
extern Uint32 DMA_ScuResult(void); /* データ転送終了チェック    */
#endif
/*--------------------------------------------------------------------------*/
/*								 Prototype									*/
/*--------------------------------------------------------------------------*/
void setSprite(Sint32 sizeX, Sint32 sizeY) ;
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
#define	VDP1_CG_ADR				(0x10000)

#define	RING_BUF_SIZ			(1024L*400)

/* ウェーブＲＡＭの転送アドレスとサンプル数 */
#define	PCM_ADDR				((void*)0x25a20000)
#define	PCM_SIZE				(4096L*16)

/*--------------------------------------------------------------------------*/
/*								 Extern										*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*								Structure									*/
/*--------------------------------------------------------------------------*/
SPRITE film_spr ;

Uint32 work_ram_buf[320L * 224L / 2];
Uint32 cp_size;
Uint32 g_movie_work[CPK_15WORK_DSIZE];	/* Work Buffer */
Uint32 g_movie_buf[RING_BUF_SIZ / sizeof(Uint32)];	/* Ring Buffer */
     char *g_filenaem = "SAMPLE.CPK";		/* Play Movie File Name */
     
     /*--------------------------------------------------------------------------*/
     /*								Main Routin									*/
     /*--------------------------------------------------------------------------*/
     void ss_main(void)
{
  CpkHn 			cpk;
  GfsHn			gfs;
  Uint32			restart;

  slInitSystem(TV_320x224,NULL,1);

  slIntFunction((void*)smpVblIn) ;

  fileInit() ;

  sndInit();

  cpkInit();

  restart = 1;

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

      DMA_ScuMemCopy((void*)(0x25c00000+VDP1_CG_ADR),
		     (void *) work_ram_buf,
		     cp_size) ;
#if MD_DMA_SCU_RESULT == USED_DMA_SCU_RESULT
      while (DMA_ScuResult() == DMA_SCU_BUSY) ;
#endif
      slSetSprite(&film_spr, toFIXED(170.0)) ;
      slSynch() ;
      /* 表示完了の通知 */
      CPK_CompleteDisp(cpk);
    }
    /* ムービの終了判定 */
    if (CPK_GetPlayStatus(cpk) == CPK_STAT_PLAY_END) {

      /* ムービの放棄 */
      CPK_DestroyGfsMovie(cpk);

      /* ファイルのクローズ*/
      fileClose(gfs);

      restart = 1;
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
    slPrint("Error File Open" , slLocate(9,3));
    while( 1 ) slSynch();
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
}

/*--------------------------------------------------------------------------*/
/*								 Cinepak Routin								*/
/*--------------------------------------------------------------------------*/
void errCpkFunc(void *obj, Sint32 ec)
{
  slPrint("Error Cpk Func:" , slLocate(9,3));
  slPrintHex( ec, slLocate( 26, 3 ) );
  while ( -1 ) slSynch();
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
  Uint32 			movie_x, movie_y;
  CpkCreatePara	para;
  CpkHeader		*header;
  CpkHn			cpk;

  /* ムービハンドル生成 */
  CPK_PARA_WORK_ADDR(&para) = g_movie_work;
  CPK_PARA_WORK_SIZE(&para) = CPK_15WORK_BSIZE;
  CPK_PARA_BUF_ADDR(&para) = g_movie_buf;
  CPK_PARA_BUF_SIZE(&para) = RING_BUF_SIZ;
  CPK_PARA_PCM_ADDR(&para) = PCM_ADDR;
  CPK_PARA_PCM_SIZE(&para) = PCM_SIZE;
  cpk = CPK_CreateGfsMovie(&para, gfs);
  if (cpk == NULL) {
    slPrint( "Error Create handle", slLocate( 9, 3 ) );
    while( -1 ) slSynch();
  }

  /* 表示色数を３２０００色に設定 */
  CPK_SetColor(cpk, CPK_COLOR_15BIT);

  /* ヘッダを読み込む */
  /* ムービのサイズが予めわかっている場合は CPK_Preload と */
  /* CPK_GetHeader は呼ぶ必要ない */
  /* CPK_PreloadHeader() を呼ばない方が再生の開始が若干早くなる */
  CPK_PreloadHeader(cpk);

  /* ムービのサイズを取得 */
  header = CPK_GetHeader(cpk);
  movie_x = header->width;
  movie_y = header->height;
  setSprite(movie_x, movie_y);

  /* ムービの展開アドレスを設定 */
  CPK_SetDecodeAddr(cpk, (void *)work_ram_buf, 2 * movie_x);
  cp_size = (Uint32)(movie_x * movie_y * 2);

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

void setSprite(Sint32 sizeX, Sint32 sizeY)
{
  Uint32 		luX, luY, rdX, rdY ;

  luX = (320 - sizeX) / 2 ;
  luY = (224 - sizeY) / 2 ;
  rdX = luX + sizeX - 1 ;
  rdY = luY + sizeY - 1 ;

  luX -= 160 ;
  rdX -= 160 ;
  luY -= 112 ;
  rdY -= 112 ;

  film_spr.CTRL = FUNC_Texture ;
  film_spr.LINK = 0 ;
  film_spr.PMOD = (ECdis | CL32KRGB) ;
  film_spr.COLR = 0 ;
  film_spr.SRCA = (VDP1_CG_ADR/8) ;
  film_spr.SIZE = (sizeX/8)<<8 | sizeY;
  film_spr.XA = luX;
  film_spr.YA = luY;
  film_spr.XB = rdX;
  film_spr.YB = luY;
  film_spr.XC = rdX;
  film_spr.YC = rdY;
  film_spr.XD = luX;
  film_spr.YD = rdY;
  film_spr.GRDA = 0 ;
  film_spr.DMMY = 0 ;

}
