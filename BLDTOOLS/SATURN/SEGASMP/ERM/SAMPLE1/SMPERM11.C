/********************************************************************
*  FILE:    smperm11.c
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
#include <stdio.h>
#include <string.h>
#include "sega_xpt.h"
#include "sega_dma.h"
#include "sega_scl.h"
#define _SPR2_
#include "sega_spr.h"
#include "sega_per.h"
#include "sega_cdc.h"
#include "sega_sys.h"
#include "sega_erm.h"
#include "../font\smp_font.h"
#include "../..\v_blank\v_blank.h"

#include "smperm11.h"
#include "smperm10.h"
#include "smperm12.h"

/* 外部関数 */
extern void Vdp2Clear(void);
extern void BtmpClr(Uint8 *vram, Uint16 x, Uint16 y,Uint16 xs,Uint16 ys);

/* 内部関数 */
void MainFramDisp(void);
void MainAdm(void);
void MainConfig(void);
void ChkMain(void);
void Success(void);
void Error(void);
void PadDataMake(void);
void CDOpenCheck(void);

Uint8 ExitFlag;
Uint8 MainMode;
Uint8 MainLevel;
Uint8 SubMain;

Sint16 Num = 0;     /* 選択番号 */
Sint16 Id = 3;      /* ID */
Sint16 Mem = 0;     /* メモリー*/
Sint16 Test = 0;    /* テスト */

Sint16 SMPA_pad_data1;       /* 1P PAD DATA          */
Sint16 SMPA_pad_edge1;       /* 1P PAD EDGE          */

Uint8 buff[30];

/* 各変数の文字列 */
#if 0
Uint8 NUMCH[8][5] = {"1","2","3","4","5","6","7","AUTO"};
Uint8 IDCH[3][6] ={"0x5A","0x5C","ERROR"};
Uint8 MEMCH[3][6] = {"YES","NO","ERROR"};
Uint8 TESTCH[4][6] = {"START","CHECK","OK","ERROR"};
#else
Uint8 *NUMCH[8] = {"1","2","3","4","5","6","7","AUTO"};
Uint8 *IDCH[4] ={"0x5A","0x5C","ERROR","?????"};
Uint8 *MEMCH[3] = {"YES","NO","ERROR"};
Uint8 *TESTCH[4] = {"START","CHECK","OK","ERROR"};
#endif

/* 各変数の文字カラー */
Uint16 NUMCHCLR[4] = {2,2,2,2};
Uint16 IDCHCLR[4] = {2,2,2,2};
Uint16 MEMCHCLR[4] = {2,2,2,2};
Uint16 TESTCHCLR[4] = {2,2,2,2};

/* 各変数の文字のバックカラー */
Uint16 NUMBKCLR[4] = {3,0,0,0};
Uint16 IDBKCLR[4] = {0,3,0,0};
Uint16 MEMBKCLR[4] = {0,0,3,0};
Uint16 TESTBKCLR[4] = {0,0,0,3};

#define RESET_PAD (PER_DGT_A | PER_DGT_B | PER_DGT_C | PER_DGT_S)

/*--------------------------------------------------*/
/*  メインループ関数                                */
/*--------------------------------------------------*/
void MainLoop(void)
{

    XyInt xy;

    ExitFlag = 1;
    MainMode = M0_MODE;
    MainLevel = 0;

    while(ExitFlag){
        PadDataMake();

        /*------ A & B & C & START -- Go MP-------*/
        if((SMPA_pad_data1&RESET_PAD)==RESET_PAD){
            SYS_EXECDMP();
        }
        /*----------------*/

        /* CDのオープンチェック */
        CDOpenCheck();

        SPR_2OpenCommand(SPR_2DRAW_PRTY_OFF);
        xy.x = 320 -1;
        xy.y = 240 -1;
        SPR_2SysClip(0,&xy);
        xy.x = 0;
        xy.y = 0;
        SPR_2LocalCoord(0,&xy);

        MainAdm();

        SPR_2CloseCommand();

        /* Ｖブランク待ち   */
        SCL_DisplayFrame();


    }
}

/*--------------------------------------------------*/
/*  描画のイニシャル関数                            */
/*--------------------------------------------------*/
void InitGraph(void)
{
    Vdp2Clear();
    Num = 0;
    Id = 3;
    Mem = 0;
    Test = 0;
}
/*--------------------------------------------------*/
/*  メインの管理関数                                */
/*--------------------------------------------------*/
void MainAdm(void)
{
    /* PAD DATA MAKE */
    PadDataMake();
    /* Branch Mode */
    switch (MainMode) {
    case M0_MODE:/* 基本画面の表示 */
        InitGraph();
        MainFramDisp();
        MainMode = M1_MODE;
        break;
    case M1_MODE:/* 設定画面 */
        MainConfig();
        break;
    case M2_MODE:/* 実行 */
        ChkMain();
        break;
    case M3_MODE:/* 成功終了 */
        Success();
        break;
    case M4_MODE:/* エラー処理 */
        Error();
        break;
    }
}

/*--------------------------------------------------*/
/*  メイン画面の描画                                */
/*--------------------------------------------------*/
void MainFramDisp(void)
{
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,(Uint8*)"ERM TEST CONFIG",0x20,0x20,2,0);

    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,(Uint8*)"NUMBER      ",0x20,0x48,2,0);
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,(Uint8*)"ID CHECK    ",0x20,0x68,2,0);
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,(Uint8*)"MEMORY CHECK",0x20,0x88,2,0);
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,(Uint8*)"TEST",0x20,0xa8,2,0);
}

/*--------------------------------------------------*/
/*  チェック項目の設定                              */
/*--------------------------------------------------*/
void MainConfig(void)
{
    static Sint16 cusud = 0;
    static Sint16 sp = 0;

    /* カーソルの移動 */
    if(SMPA_pad_edge1&PER_DGT_U){
        cusud--;
        sp = 0;
        if(cusud < 0) cusud = 3;
    }

    if(SMPA_pad_edge1&PER_DGT_D){
        cusud++;
        sp = 0;
        if(cusud > 3) cusud = 0;
    }

    if(SMPA_pad_data1&PER_DGT_U){
        sp++;
        if(sp > 20){
            sp = 18;
            cusud--;
            if(cusud < 0) cusud = 3;
        }
    }

    if(SMPA_pad_data1&PER_DGT_D){
        sp++;
        if(sp > 20){
            sp = 18;
            cusud++;
            if(cusud > 3) cusud = 0;
        }
    }


    switch(cusud){
    case 0:/* NUMBER */
        Num = 0;
        break;
    case 1:/* ID CHECK */
        if(SMPA_pad_edge1&PER_DGT_L){
            Id--;
            if(Id < 0) Id = 1;
        }

        if(SMPA_pad_edge1&PER_DGT_R){
            Id++;
            if(Id > 1) Id = 0;
        }
        break;
    case 2:/* MEMORY CHECK */
        if(SMPA_pad_edge1&PER_DGT_L){
            Mem--;
            if(Mem < 0) Mem = 1;
        }

        if(SMPA_pad_edge1&PER_DGT_R){
            Mem++;
            if(Mem > 1) Mem = 0;
        }
        break;
    case 3:/* TEST の開始 */
        if(SMPA_pad_edge1&(PER_DGT_A|PER_DGT_C)){
            /*MainMode = M2_MODE;*/
            MainMode = M2_MODE;
            cusud = 0;
            Test++;
            SubMain = 0;
            BtmpClr((Uint8 *)SCL_VDP2_VRAM_A0,0xa0,0xa8,0x40,0x10);
        }
        break;
    }

    BtmpClr((Uint8 *)SCL_VDP2_VRAM_A0,0xa0,0x48 + cusud*0x20,0x40,0x10);

    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,NUMCH[Num],0xa0,0x48,
                                                NUMCHCLR[cusud],NUMBKCLR[cusud]);
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,IDCH[Id],0xa0,0x68,
                                                IDCHCLR[cusud],IDBKCLR[cusud]);
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,MEMCH[Mem],0xa0,0x88,
                                                MEMCHCLR[cusud],MEMBKCLR[cusud]);
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,TESTCH[Test],0xa0,0xa8,
                                            TESTCHCLR[cusud],TESTBKCLR[cusud]);

}

/*--------------------------------------------------*/
/*  各値の表示                                      */
/*--------------------------------------------------*/
void MainValueDisp(void)
{
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,NUMCH[Num],0xa0,0x48,2,0);
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,IDCH[Id],0xa0,0x68,2,0);
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,MEMCH[Mem],0xa0,0x88,2,0);
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,TESTCH[Test],0xa0,0xa8,2,0);

}

/*--------------------------------------------------*/
/*  PAD DATA MAKE                                   */
/*--------------------------------------------------*/
void ChkMain(void)
{
    Sint16 id;
    Sint16 res;

    switch(SubMain){
    case 0:/* ID チェック */
        id = ERM_IdChk();
        switch(id){
        case ERM_ID8:
            Id = 0;
            break;
        case ERM_ID32:
            Id = 1;
            break;
        default:
            Id = 2;
            break;
        }

        if(Id == 2){
            Test = 3;
            SubMain = 0;
            MainMode = M4_MODE;
            return;
        }
        else{
            BtmpClr((Uint8 *)SCL_VDP2_VRAM_A0,0xa0,0x68,0x40,0x10);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,IDCH[Id],0xa0,0x68,2,0);
        }
        if(Mem == 1){
            Test = 2;
            SubMain = 0;
            MainMode = M3_MODE;
            return;
        }
        SubMain++;
        break;
    case 1:/* メモリーチェック */
        res = ERM_Chk();
        switch(res){
        case 0:
            Test = 2;
            SubMain = 0;
            MainMode = M3_MODE;/* 成功終了 */
            break;
        case ERM_ERR_ID:/* ID エラーのとき */
            Test = 3;
            Id = 2;
            SubMain = 0;
            MainMode = M4_MODE;
            break;
        case ERM_ERR_MEM:/* メモリーエラーのとき */
            Mem = 2;
            Test = 3;
            SubMain = 0;
            MainMode = M4_MODE;
            break;
        }
    break;
    }
}

void Success(void)
{
    switch(SubMain){
    case 0:/* OK と表示する */
        BtmpClr((Uint8 *)SCL_VDP2_VRAM_A0,0xa0,0xa8,0x40,0x10);
        FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,TESTCH[Test],0xa0,0xa8,3,0);
        SubMain++;
        break;
    case 1:/* A,B,Cボタンのどれかで終了*/
        if(SMPA_pad_edge1&(PER_DGT_A|PER_DGT_B|PER_DGT_C)){
            MainMode = M0_MODE;
            SubMain = 0;
        }
        break;
    }
}

void Error(void)
{
    switch(SubMain){
    case 0:
        if(Id == 2){/* IDエラーの出力 */
            BtmpClr((Uint8 *)SCL_VDP2_VRAM_A0,0xa0,0x68,0x40,0x10);
            BtmpClr((Uint8 *)SCL_VDP2_VRAM_A0,0xa0,0xa8,0x40,0x10);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,IDCH[Id],0xa0,0x68,5,0);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,TESTCH[Test],0xa0,0xa8,5,0);
        }
        else{/* メモリーエラーの出力 */
            BtmpClr((Uint8 *)SCL_VDP2_VRAM_A0,0xa0,0x88,0x40,0x10);
            BtmpClr((Uint8 *)SCL_VDP2_VRAM_A0,0xa0,0xa8,0x40,0x10);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,MEMCH[Mem],0xa0,0x88,5,0);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,TESTCH[Test],0xa0,0xa8,5,0);
        }
        SubMain++;
        break;
    case 1:/* A,B,C ボタンのどれかで終了 */
        if(SMPA_pad_edge1&(PER_DGT_A|PER_DGT_B|PER_DGT_C)){
            MainMode = M0_MODE;
            SubMain=0;
        }
        break;
    }
}
/*--------------------------------------------------*/
/*  PAD DATA MAKE                                   */
/*--------------------------------------------------*/
void    PadDataMake(void)
{

    SMPA_pad_data1 = (Sint16)PadData1;
    SMPA_pad_edge1 = (Sint16)PadData1E;

}


/*--------------------------------------------------*/
/*  割り込み要因のレジスターのビットが1か？         */
/*--------------------------------------------------*/
Bool isHirqOn(Sint32 flag)
{
    return((CDC_GetHirqReq() & flag) != 0);
}

/*--------------------------------------------------*/
/*  CDのオープンチェック　                          */
/*--------------------------------------------------*/
void CDOpenCheck(void)
{
    CdcStat stat;

    if(isHirqOn(CDC_HIRQ_DCHG)){
        SYS_EXECDMP();
    }

    CDC_GetPeriStat(&stat);

}

