/*----------------------------------------------------------------------------
 *  dsp_ctrl.c -- DSP ライブラリ CTRL モジュール
 *  Copyright(c) 1994 SEGA
 *  Written by H.E on 1994-04-04 Ver.0.80
 *  Updated by H.E on 1994-07-25 Ver.1.00
 *  Updated by A.H on 1997-01-07 Ver.1.10
 *       CPU内部除算器使用時の割り込みマスク処理に関する不具合対応のために
 *       ライブラリのコンパイルオプションを-div=cpuとしました。
 *  Updated by A.H on 1997-05-19 Ver.1.20
 *     1.DSP_CheckEnd()内部にて、SCUの割込みステータスレジスタへのライトを
 *       行っていた処理を削除。
 *     2.DSPプログラム制御ポートのリードを複数の場所で行わなくてすむ様に
 *       該当レジスタのコピーを保持する様に変更
 *       DSPプログラム制御ポートを保持するグローバル変数を追加
 *       追加変数名  volatile Uint32 DSP_Copy_Ctrl_Port
 *     3.SH2のキャッシュパージ方法が不正であったバグを修正
 *     4.DSPプログラム制御ポートへのアクセス用 ビット名マクロを追加
 *        追加マクロ名称は、次の通り。
 *        DSP_CTRL_LOAD_ENA   0x00008000  bit15：プログラムカウンタ転送許可
 *        DSP_CTRL_EXEC       0x00010000  bit16：プログラム実行制御フラグ
 *        DSP_CTRL_EXEC_STEP  0x00020000  bit17：ステップ実行制御フラグ
 *        DSP_CTRL_ENDI       0x00040000  bit18：プログラム終了割込みフラグ
 *        DSP_CTRL_OVER       0x00080000  bit19：DSPオーバーフローフラグ
 *        DSP_CTRL_CURRY      0x00100000  bit20：DSPキャリーフラグ
 *        DSP_CTRL_ZERO       0x00200000  bit21：DSPゼロフラグ
 *        DSP_CTRL_SIGN       0x00400000  bit22：DSPサインフラグ
 *        DSP_CTRL_DMA        0x00800000  bit23：DSP-DMA実行中フラグ
 *        DSP_CTRL_PAUSE      0x02000000  bit25：DSP一時停止制御フラグ
 *        DSP_CTRL_PAUSE_RES  0x04000000  bit26：DSP一時停止解除フラグ
 *
 *  このライブラリはＤＳＰ制御処理モジュールで、以下のルーチンを含む。
 *
 *  DSP_LoadProgram         -  プログラムロード
 *  DSP_WriteData           -  データライト
 *  DSP_ReadData            -  データリード
 *  DSP_Start               -  実行開始
 *  DSP_Stop                -  実行停止
 *  DSP_CheckEnd            -  実行終了チェック
 *
 *  このライブラリを使用するには次のインクルードファイルを定義する必要がある。
 *
 *  #include "sega_dsp.h"
 *
 *----------------------------------------------------------------------------
 */

/*
 * USER SUPPLIED INCLUDE FILES
 */
#include "sega_int.h"
#include "sega_dsp.h"
#ifdef _SH
#include <machine.h>
#endif

/*
 * GLOBAL DECLARATIONS
 */

/* A.H(SOJ) 1997-05-19 Ver.1.10 */
    /* DSPプログラム制御ポートを保持用グローバル変数を追加 */
volatile Uint32 DSP_Copy_Ctrl_Port;


/*****************************************************************************
 *
 * NAME:  DSP_LoadProgram()    - Load DSP Program
 *
 * PARAMETERS :
 *
 *     (1) Uint8   dst         - <i> ＤＳＰプログラムＲＡＭ内のアドレス
 *     (2) Uint32  *src        - <i> ワークＲＡＭ内プログラムエリアアドレス
 *     (3) Uint16  count       - <i> プログラムサイズ（ロングワード単位）
 *
 * DESCRIPTION:
 *
 *     ＤＳＰを停止し、ＤＳＰへ指定プログラムをロードする。
 *
 * POSTCONDITIONS:
 *
 *     No exist.
 *
 * CAVEATS:
 *
 *
 *****************************************************************************
 */
void DSP_LoadProgram(Uint8 dst, Uint32 *src, Uint16 count)
{
    Uint32  ctrl;
    int     i;

    /** BEGIN ***************************************************************/
    INT_ChgMsk(INT_MSK_NULL, INT_MSK_DSP);   /* DSP の割り込みをマスクする  */
    DSP_WRITE_REG(DSP_RW_CTRL, 0);    /* DSP Stop                           */
#if 0  /* 1997-05-19 A.H(SOJ)  マクロの追加に伴い変更 */
    ctrl = 0x8000 | dst;
#else
    ctrl = DSP_CTRL_LOAD_ENA | dst;
#endif
    DSP_WRITE_REG(DSP_RW_CTRL, ctrl); /* Set Program Pos                    */
    for(i=0; i<count; i++)
        DSP_WRITE_REG(DSP_W_PDAT, *src++); /* Write DSP Program             */
}


/*****************************************************************************
 *
 * NAME:  DSP_WriteData()      - Write Data in the DSP Data RAM
 *
 * PARAMETERS :
 *
 *     (1) Uint8   dst         - <i> ＤＳＰデータＲＡＭ内のアドレス
 *     (2) Uint32  *src        - <i> ワークＲＡＭ内データエリアアドレス
 *     (3) Uint16  count       - <i> データサイズ（ロングワード単位）
 *
 * DESCRIPTION:
 *
 *     ＤＳＰを停止し、ＤＳＰのデータＲＡＭへ指定データを書き込む。
 *
 * POSTCONDITIONS:
 *
 *     No exist.
 *
 * CAVEATS:
 *
 *
 *****************************************************************************
 */
void DSP_WriteData(Uint8 dst, Uint32 *src, Uint16 count)
{
    Uint32  ramAddr;
    int     i;

    /** BEGIN ***************************************************************/
    DSP_WRITE_REG(DSP_RW_CTRL, 0);      /* DSP Stop                         */
    ramAddr = dst;
    DSP_WRITE_REG(DSP_W_DADR, ramAddr); /* Write Data Address               */
    for(i=0; i<count; i++)
        DSP_WRITE_REG(DSP_RW_DDAT, *src++);   /* Write Data                 */
}


/*****************************************************************************
 *
 * NAME:  DSP_ReadData()       - Read Data from the DSP Data RAM
 *
 * PARAMETERS :
 *
 *     (1) Uint32  *dst        - <o> ワークＲＡＭ内データエリアアドレス
 *     (2) Uint8   src         - <i> ＤＳＰデータＲＡＭ内のアドレス
 *     (3) Uint16  count       - <i> データサイズ（ロングワード単位）
 *
 * DESCRIPTION:
 *
 *     ＤＳＰを停止し、ＤＳＰのデータＲＡＭから指定データを読み出す。
 *
 * POSTCONDITIONS:
 *
 *     No exist.
 *
 * CAVEATS:
 *
 *
 *****************************************************************************
 */
void DSP_ReadData(Uint32 *dst, Uint8 src, Uint16 count)
{
    Uint32  ramAddr;
    int     i;

    /** BEGIN ***************************************************************/
    DSP_WRITE_REG(DSP_RW_CTRL, 0);        /* DSP Stop                       */
    ramAddr = src;
    DSP_WRITE_REG(DSP_W_DADR, ramAddr++); /* Write Data Address             */
    for(i=0; i<count; i++)
        *dst++ = DSP_READ_REG(DSP_RW_DDAT);   /* Read  Data                 */
}


/*****************************************************************************
 *
 * NAME:  DSP_Start()          - Execute DSP Program
 *
 * PARAMETERS :
 *
 *     (1) Uint8   pc          - <i> ＤＳＰプログラムの開始位置
 *
 * DESCRIPTION:
 *
 *     指定されたＤＳＰプログラムの実行開始位置から実行する。
 *
 * POSTCONDITIONS:
 *
 *     No exist.
 *
 * CAVEATS:
 *
 *
 *****************************************************************************
 */
void DSP_Start(Uint8 pc)
{
    Uint32  ctrl;

    /** BEGIN ***************************************************************/
    DSP_WRITE_REG(DSP_RW_CTRL, 0);    /* DSP Stop                           */
#if 0  /* 1997-05-19 A.H(SOJ)  マクロの追加に伴い変更 */
    ctrl = 0x18000 | pc;
#else
    ctrl = (DSP_CTRL_LOAD_ENA | DSP_CTRL_EXEC | pc);
#endif
    DSP_WRITE_REG(DSP_RW_CTRL, ctrl); /* Set Program pc & start DSP         */
}


/*****************************************************************************
 *
 * NAME:  DSP_Stop()           - Stop DSP Program
 *
 * PARAMETERS :
 *
 *     No exist.
 *
 * DESCRIPTION:
 *
 *     実行中のＤＳＰプログラムを停止する。
 *
 * POSTCONDITIONS:
 *
 *     No exist.
 *
 * CAVEATS:
 *
 *
 *****************************************************************************
 */
void DSP_Stop(void)
{
    /** BEGIN ***************************************************************/
    DSP_WRITE_REG(DSP_RW_CTRL, 0);    /* DSP Stop                           */
}


/*****************************************************************************
 *
 * NAME:  DSP_CheckEnd()           - Check DSP Process End
 *
 * PARAMETERS :
 *
 *     No exist.
 *
 * DESCRIPTION:
 *
 *     ＤＳＰプログラムの処理終了をチェックする。
 *
 * POSTCONDITIONS:
 *
 *     Uint8   result              - <o> 終了フラグ
 *                                       DSP_END     = 実行終了
 *                                       DSP_NOT_END = 実行中
 *
 * CAVEATS:
 *    本関数内部では、DSPプログラム制御ポートレジスタをリードします。
 *    DSPプログラム制御ポートレジスタのリードは、プログラムコード全体で
 *    1ヶ所でのみ行う事。
 *    プログラム中で本レジスタの参照が必要な場合は、本関数を実行した後に、
 *    グローバル変数 DSP_Copy_Ctrl_Port を参照して下さい。
 *
 *****************************************************************************
 */
Uint8 DSP_CheckEnd(void)
{
#if	0
	/*
	**■1995-07-26	高橋智延
	**	使ってないので削除
	*/
    Uint32  vbr, *irqVector;
    int     iMask;
    Uint32  *l, i;
#endif
    /** BEGIN ***************************************************************/
#if 0
    if((*(volatile Uint32*)0x25fe00a4) & 0x20) {
        for(; DSP_READ_REG(DSP_RW_CTRL) & 0x800000; ) {}
        *(volatile Uint32*)0x25fe00a4  = ~0x20;  /* DSP終了割込みのみ保持 */
        *(volatile Uint16*)0xfffffe92 |= 0x10;   /* cash parge */ 
        return DSP_END;
    }  else
        return DSP_NOT_END;
#else  /* 1997-05-19 A.H(SOJ) Ver 1.20 */
#if 0
       1.DSP_CheckEnd()内部にて、SCUの割込みステータスレジスタへのライトを
         行っていた処理を削除。
       2.DSPプログラム制御ポートのリードを複数の場所で行わなくてすむ様に
         該当レジスタのコピーを保持する様に変更
         DSPプログラム制御ポートを保持するグローバル変数を追加
         追加変数名  volatile Uint32 DSP_Copy_Ctrl_Port
       3.SH2のキャッシュパージ方法が不正であったバグを修正
#endif
    DSP_Copy_Ctrl_Port = DSP_READ_REG(DSP_RW_CTRL); 
    if( DSP_Copy_Ctrl_Port & (DSP_CTRL_EXEC | DSP_CTRL_DMA) ){
	    return DSP_NOT_END;
	}else{
	    *(volatile Uint8*)0xfffffe92 |= 0xFE;   /* cache DISABLE */ 
    	*(volatile Uint8*)0xfffffe92 |= 0x11;   /* cache parge & ENABLE */ 
	    return DSP_END;
	}
#endif
#if 0
    if(INT_GetStat() & INT_ST_DSP) {
        for(; DSP_READ_REG(DSP_RW_CTRL) & 0x800000; ) {}
        INT_ResStat(INT_ST_DSP);
        *((volatile Uint16*)0xfffffe92) |= 0x10;   /* cash parge */ 
        return DSP_END;
    }  else
        return DSP_NOT_END;
#endif
}

/*  end of file */
