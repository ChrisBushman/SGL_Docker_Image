/********************************************************************
*  FILE:    smperm12.c
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
#include <machine.h>
#include "sega_xpt.h"
#include "sega_dma.h"
#include "sega_scl.h"
#define _SPR2_
#include "sega_spr.h"
#include "smperm12.h"

#include "../font\smp_font.h"
#include "../..\v_blank\v_blank.h"

#define COMMAND_MAX     512
#define GOUR_TBL_MAX    512
#define LOOKUP_TBL_MAX  512
#define CHAR_MAX        100
#define DRAW_PRTY_MAX   256

SPR_2DefineWork(work2d,COMMAND_MAX,GOUR_TBL_MAX,
        LOOKUP_TBL_MAX,CHAR_MAX,DRAW_PRTY_MAX)

/* CopyRight */
static Uint8    CpRt[] = {"(C) SEGA ENTERPRISES,LTD.1996,1997"};

/*--------------------------------------------------------------*/
/*      ビットマップのフィル                                    */
/*--------------------------------------------------------------*/
void BtmpFill(Uint8 *vram,Uint16 x,Uint16 y,Uint16 xs,Uint16 ys,Uint8 fill)
{
    Uint32  i,j;

    vram += (x + y*512);
    for(i=0;i<ys;i++){
        for(j=0;j<xs;j++){
            *vram++ = fill;
        }
        vram += (512 - xs);
    }
}

/*--------------------------------------------------------------*/
/*      ビットマップの消去                                      */
/*--------------------------------------------------------------*/
void BtmpClr(Uint8 *vram, Uint16 x, Uint16 y,Uint16 xs,Uint16 ys)
{
    BtmpFill((Uint8 *)vram, x, y, xs, ys, 0 );
}

/*--------------------------------------------------------------*/
/*      Vdp2 Clear                                              */
/*--------------------------------------------------------------*/
void    Vdp2Clear(void)
{
    Uint16  i,*ptr;

    ptr = (Uint16 *)SCL_VDP2_VRAM_A0;
    for(i=0;i<0x20000/8;i++){
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
    }
    ptr = (Uint16 *)SCL_VDP2_VRAM_A1;
    for(i=0;i<0x20000/8;i++){
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
        *ptr++ = 0;
    }
}

/*--------------------------------------------------------------*/
/*      ビットマップの転送                                      */
/*--------------------------------------------------------------*/
void    BtmpLoad(Uint8 *vram, Uint8 *work,Uint16 size)
{
    Uint16 i;

    for(i = 0;i < size;i++){
        *(vram++) = *(work++);
    }
}

static  Uint32  color[12] = {
    0x00000000,/* 透明色になるところだから何を入れても良い */
    0x00000000,/* 黒 */
    0x00FFFFFF,/* 白 */
    0x00FF0000,/* 青 */
    0x0000FF00,/* 緑 */
    0x000000FF,/* 赤 */
    0x00008000, /* 深緑 */
};

/*--------------------------------------------------------------*/
/*      VDP1,VDP2の設定                                         */
/*--------------------------------------------------------------*/
void    DispInit(void)
{
    Uint32      i;
    SclConfig   Nbg0Scfg;
    Uint16      BackCol;


    *(Uint16 *)0x25F80000 &= 0x7fff;/* 画面表示をＯＦＦにする */

    set_imask(0);
    SetVblank();
    SCL_Vdp2Init();

    SCL_SetDisplayMode(SCL_NON_INTER,SCL_240LINE,SCL_NORMAL_A);
    PER_SMPC_RES_ENA();/* リセットボタン有効 */

    SCL_SetPriority(SCL_NBG0, 6);
    SCL_SetPriority(SCL_SP0|SCL_SP1|SCL_SP2|SCL_SP3|
                    SCL_SP4|SCL_SP5|SCL_SP6|SCL_SP7,7);

    SCL_SetSpriteMode(SCL_TYPE1,SCL_MIX,SCL_SP_WINDOW);
    SPR_2Initial(&work2d);
    SPR_SetEraseData(0,0,0,320-1,240-1);

    SCL_SetColRamMode(SCL_CRM24_1024);

    SCL_AllocColRam(SCL_SPR,256,ON);
    SCL_SetColRam(SCL_SPR,0,7,color);


    BackCol = RGB16_COLOR(0,0,0) & 0x7fff;

    SCL_SetBack(SCL_VDP2_VRAM+0x80000-2,1,&BackCol);

    FNT_SetBuffSize(512,256,FNT_JAPAN);

    SCL_SetFrameInterval(1);
    SCL_DisplayFrame();

    SCL_AllocColRam(SCL_NBG0,256,OFF);
    SCL_SetColRam(SCL_NBG0,0,7,color);


    /* サイクルパターン設定 */
    Scl_s_reg.vramcyc[0] = 0x44FF;  /* NBG0 256 Bitmap */

    /* NBG0初期化 */
    SCL_InitConfigTb(&Nbg0Scfg);
        Nbg0Scfg.dispenbl = ON;
        Nbg0Scfg.coltype  = SCL_COL_TYPE_256;
        Nbg0Scfg.datatype = SCL_BITMAP;
        for(i=0;i<4;i++)
            Nbg0Scfg.plate_addr[i] = SCL_VDP2_VRAM_A0;

    SCL_SetConfig(SCL_NBG0, &Nbg0Scfg);

    /**************************************
    *   スクロール画面の初期描画      *
    **************************************/
    SCL_Open(SCL_NBG0);
        SCL_MoveTo(FIXED(0), FIXED(0), FIXED(0));
        SCL_Scale(FIXED(1.0), FIXED(1.0));
    SCL_Close();

    SCL_DisplayFrame();
}

