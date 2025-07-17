/********************************************************************
*  FILE:    smperm11.h
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
#ifndef _SMPERM01_H_
#define _SMPERM01_H_
#include "sega_xpt.h"


/* MAIN MODE MODE NO ASSIGN */
enum {
        DUMMY_MODE,              /* NO EXECUTE MODE */
        M0_MODE,
        M1_MODE,
        M2_MODE,
        M3_MODE,
        M4_MODE,
        M5_MODE,
        M6_MODE,
        M7_MODE,
        M8_MODE,
        M9_MODE,
        M10_MODE,
        M11_MODE,
        M12_MODE,
        M13_MODE,
        M14_MODE,
        M15_MODE
     };


/* 関数プロトタイプ宣言 */
extern void main(void);

void    InitGraph(void);

/* グローバル変数　宣言 */
extern Uint8 MainMode;          /* MainProgram Mode No */
extern Uint8 MainLevel;

#endif  /* endif _SMPERM01_H_ */



