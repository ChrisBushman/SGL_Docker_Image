/********************************************************************
*  FILE:    smperm10.c
*
*   Copyright(c) 1996 SEGA
*
*  PURPOSE:
*   「拡張RAMライブラリ」テストプログラム
*
*  AUTHOR(S):
*   H.K
*
*  MOD HISTORY:
*   Written by H.K on 1997-06-03 Ver.1.11
********************************************************************/
#include <machine.h>
#include "sega_xpt.h"
#include "sega_scl.h"
#include "sega_erm.h"
#include "smperm10.h"
#include "smperm12.h"

/* メインループ関数　*/
extern void MainLoop();


typedef void ( *Sequence)( void );

Uint8 Level = INIT;
/*--------------------------------------------------------------*/
/*      各設定の初期化                                          */
/*--------------------------------------------------------------*/
void Init(void){
	Sint32	err;

    DispInit();
    /* Ｖブランク待ち   */
    SCL_DisplayFrame();
    Level=MAIN;
    err = ERM_Init();

}

/*--------------------------------------------------------------*/
/*      メイン関数                                              */
/*--------------------------------------------------------------*/
void    main( void ){

    /* シーケンス関数の登録 */
    static Sequence sequence[] = {
        Init,
        MainLoop,
    };

    /* シーケンスのメインループ */
    for(;;){
        sequence[Level]();
        /* Ｖブランク待ち   */
        SCL_DisplayFrame();
    }
}

