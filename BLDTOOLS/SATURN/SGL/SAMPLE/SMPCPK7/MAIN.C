/*****************************************************************************
 *
 *                       Cinepak for SATURN Player 
 *                                by SOJ
 *                         usage sample program
 *                                for SGL
 *
 *
 *           メモリ上にあるムービファイルの指定フレーム画像を展開する
 *           展開するフレーム画像は先頭から最後まで展開した後,
 *           最後から先頭へ展開する。
 *           ムービファイルは 0x00200000 番地にロードしておくこと
 *           使用できるムービはキーフレーム１００％のもの。
 *           使用できるムービのファイルサイズは１０２４ＫＢまで。
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
#include	<sega_int.h>
#include	<sega_tim.h>
/*--------------------------------------------------------------------------*/
/*								 Prototype									*/
/*--------------------------------------------------------------------------*/

void timer0func(void) ;
CpkHn createMovie(void) ;
void cpkInit(void) ;
void cpkSendData(void) ;
void sndInit(void) ;
void errGfsFunc(void *obj, Sint32 ec) ;
void errCpkFunc(void *obj, Sint32 ec) ;
void smpVblIn(void) ;

/*--------------------------------------------------------------------------*/
/*								 Define										*/
/*--------------------------------------------------------------------------*/
#define	WIDTH_H					320
#define	WIDTH_V					224

#define	CYCPAT_BM_READ			0x44444444
#define	CYCPAT_CPU_RW			0xffffffff

#define	RING_BUF_SIZ	(1024L*1024)

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

Uint32 work_ram_buf[320L * 240L * 4 / 4];
Uint32 g_movie_work[CPK_24WORK_DSIZE];	/* Work Buffer */
Uint32 *g_movie_buf = (Uint32 *)0x00200000;

/* 総サンプル数 */
static Sint32 g_sample_total;

static volatile Uint32 g_vint_cnt;

/* ウエイト時間 */
#define WAIT_TIME		(60 / 15)

void waitVbl(Sint32 wait_time)
{
  Uint32		vint_old;

  vint_old = g_vint_cnt;
  while (g_vint_cnt - vint_old < wait_time) {
    ;
  }
}

/*--------------------------------------------------------------------------*/
/*								Main Routin									*/
/*--------------------------------------------------------------------------*/
void ss_main(void)
{
  volatile CpkHn cpk;
  volatile Uint32	restart;
  Sint32	 frame_no;

  /* 変数の初期化 */
  g_vint_cnt = 0;

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
  PauseFlag = (_ScrPause | _SprPause) ;

  slIntFunction((void*)smpVblIn) ;

  sndInit();

  cpkInit();

  /* タイマ０割り込みの設定 */
  INT_SetScuFunc(INT_SCU_TIM0, timer0func);

  /* タイマ０の設定 */
  TIM_T0_SET_CMP(WIDTH_V/2 + 2);
  TIM_T1_SET_MODE(0x101);
  TIM_T0_ENABLE();

  /* ムービハンドル生成 */
  cpk = createMovie();

  while (1) {
    for (frame_no = 0; frame_no < g_sample_total; frame_no++) {
      /* 指定番号のフレームを展開する */
      CPK_DecodeFrame(cpk, frame_no);
      cpkSendData();
      waitVbl(WAIT_TIME);
    }
    frame_no--;
    for (; frame_no >= 0; frame_no--) {
      /* 指定番号のフレームを展開する */
      CPK_DecodeFrame(cpk, frame_no);
      cpkSendData();
      waitVbl(WAIT_TIME);
    }
  }
}

/*--------------------------------------------------------------------------*/
/*								 VBL Routin									*/
/*--------------------------------------------------------------------------*/
void smpVblIn(void)
{
  /* Ｃｉｎｅｐａｋの VblIn ルーチンをコール */
  CPK_VblIn();
  Switch_VBL_IN = FALSE ;
  g_vint_cnt++;
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
CpkHn createMovie(void)
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
  cpk = CPK_CreateMemMovie(&para);
  if (cpk == NULL) {
    slPrint( "Error Create handle", slLocate( 9, 3 ) );
    while( -1 ) slSynch();
  }

  /* 表示色数を16m色に設定 */
  CPK_SetColor(cpk, CPK_COLOR_24BIT);

  /* メモリ上のムービのファイルサイズを通知する */
  CPK_NotifyWriteSize(cpk, RING_BUF_SIZ);

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

  /* 総サンプル数を取得 */
  g_sample_total = header->sample_total;

  cpk_vdp2_dst = 	(Uint8*)(0x25e00000 + 
				 4 * (512 * ((WIDTH_V - movie_y)/2) + 
				      ((WIDTH_H - movie_x)/2)) + 
				 4 * 512 * (512/2 - WIDTH_V) / 2);

  /* ムービの展開アドレスを設定 */
  CPK_SetDecodeAddr(cpk, (void *)work_ram_buf, 4 * movie_x);

  return cpk;
}

void cpkSendData(void){
  register Sint32 iu ;

  Switch_timer0 = TRUE ;
  while(Switch_timer0) ;
  Switch_VBL_IN = TRUE ;

  src = (Uint8*)work_ram_buf ;
  dst = (Uint8*)cpk_vdp2_dst ;

  /* バンクＡ０をＣＰＵライトモードにする */
  slScrCycleSet(	CYCPAT_CPU_RW,	CYCPAT_CPU_RW,
		CYCPAT_BM_READ,	CYCPAT_BM_READ);

  for (iu=0; iu<movie_y_half; iu++) {
    DMA_ScuMemCopy(dst, src, movie_x_size) ;
    src += movie_x_size ;
    dst += 2048 ;
  }
  /* バンクＡ０をキャラクタパターンデータリードにする */
  slScrCycleSet(	CYCPAT_BM_READ,	CYCPAT_BM_READ,
		CYCPAT_BM_READ,	CYCPAT_BM_READ);

  while(Switch_VBL_IN) ;

  /* バンクＢ０をＣＰＵライトモードにする */
  slScrCycleSet(	CYCPAT_BM_READ,	CYCPAT_BM_READ,
		CYCPAT_CPU_RW,	CYCPAT_CPU_RW);

  for ( ; iu<movie_y; iu++) {
    DMA_ScuMemCopy(dst, src, movie_x_size) ;
    src += movie_x_size ;
    dst += 2048 ;
  }
  /* バンクＢ０をキャラクタパターンデータリードにする */
  slScrCycleSet(	CYCPAT_BM_READ,	CYCPAT_BM_READ,
		CYCPAT_BM_READ,	CYCPAT_BM_READ);

}
/*--------------------------------------------------------------------------*/
/*				Sound Routin						    					*/
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

