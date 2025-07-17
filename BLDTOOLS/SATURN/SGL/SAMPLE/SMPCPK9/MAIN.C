/*****************************************************************************
 *
 *                       Cinepak for SATURN Player 
 *                                by SOJ
 *                         usage sample program
 *                                for SGL
 *
 *
 *                 クロマキー再生のサンプル
 *                 0x00200000h番地にロードしたムービを後ろ
 *                 0x00280000h番地にロードしたムービを手前
 *           	  使用できるムービのファイルサイズは各５１２Ｋまで。
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
#include	"sega_sys.h"
#include	"sgl_cd.h"
#include	"sega_cpk.h"
#include	"sega_snd.h"

/*--------------------------------------------------------------------------*/
/*								 Prototype									*/
/*--------------------------------------------------------------------------*/
void setSprite(Sint32 sizeX, Sint32 sizeY) ;
void setSprite_ckey(Sint32 sizeX, Sint32 sizeY) ;
static CpkHn createMovie() ;
void cpkInit(void) ;
void sndInit(void) ;
void errCpkFunc(void *obj, Sint32 ec) ;
static void smpVblIn(void) ;
static void setKeyOutRange(CpkHn cpk) ;
void create_movie(CpkHn *cpk, Uint32 *g_movie_work, Uint32 *g_movie_buf);
void create_movie_ckey(CpkHn *cpk, Uint32 *g_movie_work, Uint32 *g_movie_buf);

/*--------------------------------------------------------------------------*/
/*								 Define										*/
/*--------------------------------------------------------------------------*/
#define	VDP1_BUF_ONE	(0x08000)
#define	VDP1_BUF_TWO	(0x2B000)

#define	RING_BUF_SIZ	(1024*512L)

/* ウェーブＲＡＭの転送アドレスとサンプル数 */
#define PCM_ADDR			((void*)0x25a0c000)
#define PCM_SIZE			(4096L*14)
#define PCM_ADDR_CKEY		((void*)0x25a44000)
#define PCM_SIZE_CKEY		(4096L*14)

/*--------------------------------------------------------------------------*/
/*								 Extern										*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*								Structure									*/
/*--------------------------------------------------------------------------*/
CpkHn cpk1, cpk_ckey;
SPRITE film_spr, film_spr_ckey ;

Uint32 work_ram_buf[320L * 240L / 2];
Uint32 work_ram_buf_ckey[320L * 240L / 2];
Uint32 cp_size;
Uint32 cp_size_ckey;
/* ワークバッファ */
Uint32 g_movie_work[CPK_15WORK_DSIZE];
Uint32 g_movie_work_ckey[CPK_15WORK_DSIZE];

/* リングバッファ */
static Uint32 *g_movie_buf = (Uint32 *)0x00200000;
static Uint32 *g_movie_buf_ckey = (Uint32 *)0x00280000;

/* パッド情報 */
static	volatile Uint16	PadData1  = 0x0000;

/* キーアウト範囲 */
static Sint32 g_keyout_range;

static volatile Sint32 g_vint_cnt;
static Sint32 g_vint_old;


/*--------------------------------------------------------------------------*/
/*								Main Routin									*/
/*--------------------------------------------------------------------------*/
void ss_main(void)
{
  static Uint32	create_movie_flag;
  static Uint32	create_movie_ckey_flag;

  /* 変数の初期化 */
  g_keyout_range = 21;
  g_vint_cnt = 0;
  g_vint_old = 0;
  create_movie_flag = 1;
  create_movie_ckey_flag = 1;

  slInitSystem(TV_320x224,NULL,1);

  slIntFunction((void*)smpVblIn) ;

  sndInit();

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
    if ((CPK_IsDispTime(cpk1) == TRUE)&&
	(CPK_IsDispTime(cpk_ckey) == TRUE)) {
      /* 画像転送 */
      DMA_ScuMemCopy((void*)(0x25c00000+VDP1_BUF_ONE),
		     (void *) work_ram_buf,
		     cp_size) ;

      film_spr.SRCA = (VDP1_BUF_ONE/8) ;
      slSetSprite(&film_spr, toFIXED(170.0)) ;

      /* 画像転送 */
      DMA_ScuMemCopy((void*)(0x25c00000+VDP1_BUF_TWO),
		     (void *) work_ram_buf_ckey,
		     cp_size_ckey) ;

      film_spr_ckey.SRCA = (VDP1_BUF_TWO/8) ;
      slSetSprite(&film_spr_ckey, toFIXED(170.0)) ;

      slSynch() ;

      CPK_CompleteDisp(cpk1);
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

/*--------------------------------------------------------------------------*/
/*								 VBL Routin									*/
/*--------------------------------------------------------------------------*/
static void smpVblIn(void)
{
#if 0
  /* キーアウト範囲の変更 */
  setKeyOutRange(cpk_ckey);
#endif
  /* Ｃｉｎｅｐａｋの VblIn ルーチンをコール */
  CPK_VblIn();
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
void create_movie(CpkHn *cpk, Uint32 *g_movie_work, Uint32 *g_movie_buf)
{
  CpkHeader		*header;
  Uint32 			movie_x, movie_y;
  CpkCreatePara	para;

  /* ムービ生成 */
  CPK_PARA_WORK_ADDR(&para) = g_movie_work;
  CPK_PARA_WORK_SIZE(&para) = CPK_15WORK_BSIZE;
  CPK_PARA_BUF_ADDR(&para) = g_movie_buf;
  CPK_PARA_BUF_SIZE(&para) = RING_BUF_SIZ;
  CPK_PARA_PCM_ADDR(&para) = PCM_ADDR;
  CPK_PARA_PCM_SIZE(&para) = PCM_SIZE;

  *cpk = CPK_CreateMemMovie(&para);
  if (*cpk == NULL) {
    slPrint( "Error open handle", slLocate( 9, 3 ) );
    while( -1 ) slSynch();
  }

  /* 表示色数の設定 */
  CPK_SetColor(*cpk, CPK_COLOR_15BIT);

  /* メモリ上のムービのファイルサイズを設定 */
  CPK_NotifyWriteSize(*cpk, RING_BUF_SIZ);

  /* ヘッダを読み込む */
  /* ムービのサイズが予めわかっている場合は CPK_Preload と */
  /* CPK_GetHeader は呼ぶ必要ない */
  /* CPK_PreloadHeader() を呼ばない方が再生の開始が若干早くなる */
  CPK_PreloadHeader(*cpk);

  /* ムービのサイズを取得 */
  header = CPK_GetHeader(*cpk);
  movie_x = header->width;
  movie_y = header->height;
  setSprite(movie_x, movie_y);

  /* ムービの表示先アドレスを設定 */
  CPK_SetDecodeAddr(*cpk, (void *)work_ram_buf, 2 * movie_x);
  cp_size = (Uint32)(movie_x * movie_y * 2);
}

void create_movie_ckey(CpkHn *cpk, Uint32 *g_movie_work, Uint32 *g_movie_buf)
{
  CpkHeader		*header;
  Uint32 			movie_x, movie_y;
  CpkCreatePara	para;

  /* ムービ生成 */
  CPK_PARA_WORK_ADDR(&para) = g_movie_work;
  CPK_PARA_WORK_SIZE(&para) = CPK_15WORK_BSIZE;
  CPK_PARA_BUF_ADDR(&para) = g_movie_buf;
  CPK_PARA_BUF_SIZE(&para) = RING_BUF_SIZ;
  CPK_PARA_PCM_ADDR(&para) = PCM_ADDR_CKEY;
  CPK_PARA_PCM_SIZE(&para) = PCM_SIZE_CKEY;

  *cpk = CPK_CreateMemMovie(&para);
  if (*cpk == NULL) {
    slPrint( "Error Open Handle", slLocate( 9, 3 ) );
    while( -1 ) slSynch();
  }

  /* 表示色数の設定 */
  CPK_SetColor(*cpk, CPK_COLOR_15BIT);

  /* メモリ上のムービのファイルサイズを設定 */
  CPK_NotifyWriteSize(*cpk, RING_BUF_SIZ);

  /* ヘッダを読み込む */
  /* ムービのサイズが予めわかっている場合は CPK_Preload と */
  /* CPK_GetHeader は呼ぶ必要ない */
  /* CPK_PreloadHeader() を呼ばない方が再生の開始が若干早くなる */
  CPK_PreloadHeader(*cpk);

  /* ムービのサイズを取得 */
  header = CPK_GetHeader(*cpk);
  movie_x = header->width;
  movie_y = header->height;
  setSprite_ckey(movie_x, movie_y);

  /* ムービの表示先アドレスを設定 */
  CPK_SetDecodeAddr(*cpk, (void *)work_ram_buf_ckey, 2 * movie_x);
  cp_size_ckey = (Uint32)(movie_x * movie_y * 2);

  CPK_SetPcmStreamNo(*cpk, 1);
}

static void setKeyOutRange(CpkHn cpk)
{
  /* キーアウト範囲の変更 */
  if ( (PER_DGT_KU & Smpc_Peripheral[0].push)==0 ) {
    /* 増やす */
    g_keyout_range++;
    if (g_keyout_range > 96) {
      g_keyout_range = 96;
    }
    CPK_SetKeyOutRange(cpk, g_keyout_range);
  } else if ( (PER_DGT_KD & Smpc_Peripheral[0].push)==0 ) {
    /* 減らす */
    g_keyout_range--;
    if (g_keyout_range < 0) {
      g_keyout_range = 0;
    }
    CPK_SetKeyOutRange(cpk, g_keyout_range);
  }
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
  slInitSound((Uint8*)sddrvstsk, (Uint32)sddrvsize, (Uint8*)bootsnd, (Uint32)bootsndsize) ;
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
  film_spr.SRCA = (VDP1_BUF_ONE/8) ;
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

void setSprite_ckey(Sint32 sizeX, Sint32 sizeY)
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

  film_spr_ckey.CTRL = FUNC_Texture ;
  film_spr_ckey.LINK = 0 ;
  film_spr_ckey.PMOD = (ECdis | CL32KRGB) ;
  film_spr_ckey.COLR = 0 ;
  film_spr_ckey.SRCA = (VDP1_BUF_TWO/8) ;
  film_spr_ckey.SIZE = (sizeX/8)<<8 | sizeY;
  film_spr_ckey.XA = luX;
  film_spr_ckey.YA = luY;
  film_spr_ckey.XB = rdX;
  film_spr_ckey.YB = luY;
  film_spr_ckey.XC = rdX;
  film_spr_ckey.YC = rdY;
  film_spr_ckey.XD = luX;
  film_spr_ckey.YD = rdY;
  film_spr_ckey.GRDA = 0 ;
  film_spr_ckey.DMMY = 0 ;
}

