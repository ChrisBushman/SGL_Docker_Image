/*----------------------------------------------------------------------------
 * routines to run the DSP math processor on the saturn
 *  MTH_PolyDataTransInit     -  座標変換処理の初期化
 *  MTH_PolyDataTransExec     -  座標変換処理実行
 *  MTH_PolyDataTransCheck    -  座標変換処理完了チェック
 *
 *----------------------------------------------------------------------------
 */

/*
 * USER SUPPLIED INCLUDE FILES
 */
#include "includes\portab.h"
#include "includes\dsp.h"


static Uint32 dspProgram[] = {
#include "mth_dspp.cod"
};


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

	((**(void(**)(Uint32, Uint32))0x6000344)((0xFFFFFFFF), (32)));
	 
	 
/*	 INT_ChgMsk(INT_MSK_NULL, INT_MSK_DSP); */  /* DSP の割り込みをマスクする  */

	 DSP_WRITE_REG(DSP_RW_CTRL, 0);    /* DSP Stop                           */
    
	 ctrl = 0x8000 | dst;
    
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
 *****************************************************************************
 */
void DSP_Start(Uint8 pc)
{
    Uint32  ctrl;

    /** BEGIN ***************************************************************/
    DSP_WRITE_REG(DSP_RW_CTRL, 0);    /* DSP Stop                           */
    ctrl = 0x18000 | pc;
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
 * POSTCONDITIONS:
 *
 *     Uint8   result              - <o> 終了フラグ
 *                                       DSP_END     = 実行終了
 *                                       DSP_NOT_END = 実行中
 *
 *****************************************************************************
 */
Uint8 DSP_CheckEnd(void)
{
    Uint32  vbr, *irqVector;
    int     iMask;
    Uint32  *l, i;

    /** BEGIN ***************************************************************/
    if((*(Uint32*)0x25fe00a4) & 0x20) {
        for(; DSP_READ_REG(DSP_RW_CTRL) & 0x800000; ) {}
        *(Uint32*)0x25fe00a4  = ~0x20;
        *(Uint16*)0xfffffe92 |= 0x10;   /* cash parge */ 
        return DSP_END;
    }  else
        return DSP_NOT_END;
#if 0
    if(INT_GetStat() & INT_ST_DSP) {
        for(; DSP_READ_REG(DSP_RW_CTRL) & 0x800000; ) {}
        INT_ResStat(INT_ST_DSP);
        *((Uint16*)0xfffffe92) |= 0x10;   /* cash parge */ 
        return DSP_END;
    }  else
        return DSP_NOT_END;
#endif
}




static MthPolyTransParm iPolyTransParm;


/*************************************************************************
 *
 * NAME : MTH_PolyDataTransInit  -  Initialize Coord Transfer by DSP
 *
 * PARAMETERS
 *
 *     No exist.
 *
 *************************************************************************
 */
void    MTH_PolyDataTransInit(void)
{
    /** BEGIN ************************************************************/
    DSP_LoadProgram(0, dspProgram, 256);
}


/*************************************************************************
 *
 * NAME : MTH_PolyDataTransExec  -  Execute Coord Transfer by DSP
 *
 * PARAMETERS
 *
 *     (1) MthPolyTransParm *polyTransParm  - <i/o>  座標変換パラメータテーブル
 *
 *************************************************************************
 */
void    MTH_PolyDataTransExec(MthPolyTransParm *polyTransParm)
{
    MthPolyTransParm wPolyTransParm;
    Uint32 w;

    memcpy(&wPolyTransParm,polyTransParm,sizeof(MthPolyTransParm));

    w = ((Uint32)wPolyTransParm.viewLight        ) >> 2;
    wPolyTransParm.viewLight         = (MthViewLight*)w;

	 w = ((Uint32)wPolyTransParm.surfPoint        ) >> 2;
    wPolyTransParm.surfPoint	     = (MthXyz*)w;
    
	 w = ((Uint32)wPolyTransParm.surfNormal       ) >> 2;
    wPolyTransParm.surfNormal        = (MthXyz*)w;
    
	 w = ((Uint32)wPolyTransParm.surfBright       ) >> 2;
    wPolyTransParm.surfBright        = (Sint32*)w;
    
	 w = ((Uint32)wPolyTransParm.transViewVertSrc ) >> 2;
    wPolyTransParm.transViewVertSrc  = (MthXyz*)w;
    
	 w = ((Uint32)wPolyTransParm.transViewVertAns ) >> 2;
    wPolyTransParm.transViewVertAns  = (MthXyz*)w;
    
	 w = ((Uint32)wPolyTransParm.vertNormal       ) >> 2;
    wPolyTransParm.vertNormal        = (MthXyz*)w;
    
	 w = ((Uint32)wPolyTransParm.vertBright       ) >> 2;
    wPolyTransParm.vertBright        = (Sint32*)w;
    
	 w = ((Uint32)wPolyTransParm.transWorldVertSrc) >> 2;
    wPolyTransParm.transWorldVertSrc = (MthXyz*)w;
    
	 w = ((Uint32)wPolyTransParm.transWorldVertAns) >> 2;
    
	 wPolyTransParm.transWorldVertAns = (MthXyz*)w;
    DSP_WriteData(DSP_RAM_0 | 0, (Uint32*)&wPolyTransParm, sizeof(MthPolyTransParm)/4);
    DSP_Start(0);
}


/*************************************************************************
 *
 * NAME : MTH_PolyDataTransCheck  -  Check Coord Transfer Complete
 *
 * PARAMETERS
 *
 *     No exist.
 * 
 *************************************************************************
 */
void    MTH_PolyDataTransCheck(void)
{
    /** BEGIN ************************************************************/
    while(DSP_CheckEnd() == DSP_NOT_END); 
}

