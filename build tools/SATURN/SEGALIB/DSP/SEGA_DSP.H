/*----------------------------------------------------------------------------
 *  sega_dsp.h -- DSP ライブラリ インクルードファイル
 *     Ver.1.10
 *
 *  Copyright(c) 1994 SEGA
 *  Written by H.E on 1994-04-04 Ver.0.80
 *  Updated by H.E on 1994-04-04 Ver.0.80
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
 *  ＤＳＰ制御処理モジュール用インクルードファイル
 *
 *----------------------------------------------------------------------------
 */

#ifndef SEGA_DSP_H
#define SEGA_DSP_H

#include "sega_xpt.h"
#include "sega_def.h"

/*
 * GLOBAL DEFINES/MACROS DEFINES
 */

#define  DSP_REG_BASE        0x25FE0080 /* DSP Register base address        */

#define  DSP_RW_CTRL         0x00000000 /* DSP Control Port                 */
#define  DSP_W_PDAT          0x00000004 /* DSP Program RAM Data Port        */
#define  DSP_W_DADR          0x00000008 /* DSP Data RAM Address Port        */
#define  DSP_RW_DDAT         0x0000000c /* DSP Data RAM Data Port           */

#define  DSP_RAM_0           0x00       /* DSP RAM 0 Address                */
#define  DSP_RAM_1           0x40       /* DSP RAM 1 Address                */
#define  DSP_RAM_2           0x80       /* DSP RAM 2 Address                */
#define  DSP_RAM_3           0xc0       /* DSP RAM 3 Address                */

#define  DSP_END             1          /* DSP Process End Code             */
#define  DSP_NOT_END         0          /* DSP Process Not End Code         */

/*  DSPプログラム制御ポート用 ビット名 */
#define  DSP_CTRL_LOAD_ENA   0x00008000 /* bit15：プログラムカウンタ転送許可 */
#define  DSP_CTRL_EXEC       0x00010000 /* bit16：プログラム実行制御フラグ   */
#define  DSP_CTRL_EXEC_STEP  0x00020000 /* bit17：ステップ実行制御フラグ     */
#define  DSP_CTRL_ENDI       0x00040000 /* bit18：プログラム終了割込みフラグ */
#define  DSP_CTRL_OVER       0x00080000 /* bit19：DSPオーバーフローフラグ    */
#define  DSP_CTRL_CURRY      0x00100000 /* bit20：DSPキャリーフラグ          */
#define  DSP_CTRL_ZERO       0x00200000 /* bit21：DSPゼロフラグ              */
#define  DSP_CTRL_SIGN       0x00400000 /* bit22：DSPサインフラグ            */
#define  DSP_CTRL_DMA        0x00800000 /* bit23：DSP-DMA実行中フラグ        */
#define  DSP_CTRL_PAUSE      0x02000000 /* bit25：DSP一時停止制御フラグ      */
#define  DSP_CTRL_PAUSE_RES  0x04000000 /* bit26：DSP一時停止解除フラグ      */


#define DSP_WRITE_REG(reg, val) \
      *(volatile Uint32*)(DSP_REG_BASE+(reg)) = (val)
#define DSP_READ_REG(reg) \
      (*(volatile Uint32*)(DSP_REG_BASE+(reg)))

/*
 * EXTERNAL VARIABLE DECLARATIONS
 */

extern volatile Uint32 DSP_Copy_Ctrl_Port;

/*
 * EXTERNAL FUNCTION PROTOTYPE DECLARATIONS
 */
void  DSP_LoadProgram(Uint8 dst, Uint32 *src, Uint16 count);
void  DSP_WriteData(Uint8 dst, Uint32 *src, Uint16 count);
void  DSP_ReadData(Uint32 *dst, Uint8 src, Uint16 count);
void  DSP_Start(Uint8 pc);
void  DSP_Stop(void);
Uint8 DSP_CheckEnd(void);

#endif /* ifndef SEGA_DSP_H */

/*  end of file */
