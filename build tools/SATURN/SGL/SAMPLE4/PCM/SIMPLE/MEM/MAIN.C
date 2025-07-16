/*****************************************************************************
 *
 *	Copyright (c) 1994,1995,1997 SEGA
 *
 * Library	:PCM・ADPCM再生ライブラリ
 * Module 	:sample program of STREAM PLAY MODE
 * File		:main.c
 * Date		:1995-03-31
 * Version	:1.16
 * Auther	:Y.T
 * Modified     :1997-03-04
 * Modifier     :N.T
 *
 * Comment	:
 *      PCMメモリ再生サンプル
 *        ※smppcm2.cを元に改良。
 *          SGLで再生させるために一部コードを変更。
 *
 *        このサンプルでは、サウンドドライバをプログラムに組み込むタイプの
 *        ものです。
 *        
 ****************************************************************************/

/*------------------------- 《インクルード》 -------------------------*/
#include "sgl.h"
#include "sgl_cd.h"
#include "sega_snd.h"
#include "sega_pcm.h"
/*--------------------------- 《関数宣言》 ---------------------------*/
extern char *sddrvstsk;
extern long sddrvsize;
/*---------------------------- 《定数》 ----------------------------*/

/* ウェーブRAMの転送アドレスとサンプル数 */
/*
   【注意】
   PCM_SIZEのデータサイズを元に、
    8Bit Mono   :×1
   16Bit Mono   :×2
    8Bit Stereo :×2
   16Bit Stereo :×4
   のバッファを必要とします。
   ※PCM_SIZEのデータサイズがそのまま使われる訳ではありません。
   従って、PCM_ADDRに与えるPCMバッファのスタートアドレスは、その事を考慮した
   上で設定しなければなりません。
   例えば、16Bit StereoのAIFFデータを再生するのに
   PCM_SIZE = 4096L * 2
   を設定した場合、( 4096 * 2 ) * 4 = 32768 = 8000H
   のバッファサイズを必要とするので、PCM_ADDRのアドレスは、
   25A78000H以前に設定しないと、音の後半にノイズが入る可能性があります。
   又、PCMのバッファサイズが小さい場合、再生中にCDのリードエラーなどで、
   PCMストリームデータの供給が止まった場合、同じ音が繰り返し再生される
   等の症状が起こる事があります。
   その場合、PCM_SIZEに少し大きめの値を入れる事で解決できます。
*/
#define	PCM_ADDR	( ( void * )0x25a20000 )
#define	PCM_SIZE	( 4096L * 8 )	           /* 8 */

/* リングバッファのサイズ */
/*
   このサイズは、オンメモリ再生時のデータサイズ上限となります。
   デフォルトでは、1024×1024×1=1MBです。
   置かれるメモリによってサイズは変更しましょう。
*/
#define RING_BUF_SIZE   (1024L*1024L*1)
/*----------------------- 《グローバル変数》 -----------------------*/

/* ワークバッファ */
static PcmWork g_movie_work;

/* バッファの確保 */
static Sint8 *g_movie_buf = (Sint8 *)0x00200000;

/*---------------------------- 《関数》 ----------------------------*/
void errPcmFunc( void *obj, Sint32 ec )
{
  slPrint( "Error PCM Func:", slLocate( 5, 2 ) );
  slPrintHex( ec, slLocate( 20, 2 ) );
}

/*====================== サウンドの処理 ===========================*/
static void sndInit(void)
{
  SndIniDt 	snd_init;

  Sint32 bootsnd_map[ 0x400 / 4 ];

  bootsnd_map[ 0 ] = 0xffffffff;
  SND_INI_PRG_ADR( snd_init )       = ( Uint16 * )( &sddrvstsk );
  SND_INI_PRG_SZ( snd_init )        = sddrvsize;
  SND_INI_ARA_ADR( snd_init )       = ( Uint16 * )( &bootsnd_map );
  SND_INI_ARA_SZ( snd_init )        = 1;

  SND_Init( &snd_init );
  SND_ChgMap( 0 );
}

void ss_main( void )
{
  PcmHn        	pcm;
  PcmCreatePara	para;
  Uint32        restart;

  slInitSystem( TV_320x240, NULL, 1 );

  /* Vブランクの設定 */
  slIntFunction( PCM_VblIn );

  /* サウンドの設定 */
  sndInit();

  /* PCMライブラリの初期化 */
  PCM_Init();

  /* PCMライブラリのエラーハンドルの設定 */
  PCM_SetErrFunc( errPcmFunc, NULL );

  restart = 1;

  while( -1 ) {
    if ( restart ) {

      /* ハンドルの作成 */
      PCM_PARA_WORK( &para ) = ( struct PcmWork * )&g_movie_work;
      PCM_PARA_RING_ADDR( &para ) = g_movie_buf;
      PCM_PARA_RING_SIZE( &para ) = RING_BUF_SIZE;
      PCM_PARA_PCM_ADDR( &para ) = PCM_ADDR;
      PCM_PARA_PCM_SIZE( &para ) = PCM_SIZE;

      pcm = PCM_CreateMemHandle( &para );
      PCM_NotifyWriteSize( pcm, RING_BUF_SIZE );

      if ( pcm == NULL ) {
	slPrint( "Error! Open Handle.", slLocate( 5, 2 ) );
	slSynch();
	while( 1 );
      }

      /* 開始 */
      PCM_Start( pcm );
      restart = 0;
    }

    /* 再生タスク */
    PCM_Task( pcm );

    /* ムービの終了判定 */
    if ( PCM_GetPlayStatus( pcm ) == PCM_STAT_PLAY_END ) {

      /* ハンドルの消去 */
      PCM_DestroyMemHandle( pcm );
      restart = 1;
    }
    set_vbar( 3 );
    slSynch();
    reset_vbar( 3 );
  }
}

