/******************************************************************************
 *	ソフトウェアライブラリ
 *
 *	Copyright (c) 1994,1995,1996,1997 SEGA
 *
 * Library	:ＰＣＭ・ＡＤＰＣＭ再生ライブラリ
 * Module 	:ストリームアクセス
 * File		:pcm_stm.c
 * Date		:1997-08-25
 * Version	:1.26
 * Author	:Y.T, Y.H
 *
 *--------------------------------------------------------------------------*
 * Update	:1994-10-04	1.00	Y.T	新規作成
 *			 1996-07-05	1.20	Y.H	SaturnPCM file 対応
 *			:1996-10-14	1.21	Y.H	SHC Ver. 3.0F対応
 *          :1997-08-25 1.26    N.T Total Level対応
 *
 ****************************************************************************/




/************************************************************************/
/*		□ヘッダファイル												*/
/************************************************************************/
#include <machine.h>
#include <sega_xpt.h>
#include <sega_dma.h>
#include <sega_csh.h>

#include "sega_pcm.h"
#include "pcm_stm.h"
#include "pcm_mem.h"
#include "pcm_msub.h"
#include "pcm_lib.h"
#include "pcm_xlib.h"
#include "pcm_xsap.h"

/* #define	PCM_DEBUG 1 */
#ifdef PCM_DEBUG
	#include "play.h"
#endif




/************************************************************************/
/*		□関数宣言														*/
/************************************************************************/
#if		0
void	pcmstm_loadBuf(PcmHn pcm);
#endif


Sint32	pcmstm_loadCpu(void *obj, StmHn stm, Sint32 nsct);
Sint32	pcmstm_loadDmaCpu(void *obj, StmHn stm, Sint32 nsct);
Sint32	pcmstm_loadDmaScu(void *obj, StmHn stm2, Sint32 nsct);
STATIC Sint32	pcmstm_loadDummy(void *obj, StmHn stm, Sint32 nsct);
void	pcmstm_task_aiff(PcmHn pcm);
Sint32	pcmstm_preloadFile_aiff(PcmHn pcm, Sint32 size);
void	pcmstm_setTrMode_aiff(PcmHn pcm, PcmTrMode mode);
void	pcmstm_waitDma_aiff(PcmHn pcm);




/************************************************************************/
/*		□変数定義														*/
/************************************************************************/
/*  DMA 転送を開始したかを表すフラグ 開始したら TRUE  */
Bool	pcmstm_dma_cpu_start;
Bool	pcmstm_dma_scu_start;

/*  1996.07.08 Y.H  (start)  */
/*  DMA 転送先  */
static	Sint32	pcmstm_dma_dst;
/*  1996.07.08 Y.H  ( end )  */

/*  DMA 転送管理  */
static	void	*dma_cpu_dis_adr;	/* パージ対象ディスティネーションアドレス */
static	Uint32	dma_cpu_cnt;		/* パージ対象ディスティネーションカウント  */
static	void	*dma_scu_dis_adr;
static	Uint32	dma_scu_cnt;
static	Uint8	dma_start_flg;		/*  DMA の開始有無フラグ  */


/*  ストリーム関数テーブル  */
static	PcmExecFunc	pcmstm_exec_fntbl_org = {
	NULL,
	pcmstm_task_aiff,
	pcmstm_preloadFile_aiff,
	NULL,
	pcmstm_setTrMode_aiff
};

static	PcmExecFunc	pcmstm_exec_fntbl;

/*  DMA 終了検査関数  */
static	void	(*pcmstm_wait_dma)(PcmHn pcm) = pcmstm_waitDma_aiff;




/************************************************************************/
/*		□関数定義														*/
/************************************************************************/
/********************************************************************/
/*	ストリーム初期化												*/
/********************************************************************/
static	void	stmInit(void)
{
	/*  変数の初期化  */
	pcmstm_dma_cpu_start = OFF;
	pcmstm_dma_scu_start = OFF;

	if (pcm_sap_flag == OFF) {
		/*  ◇ AIFF  */
		/*  STM function for AIFF  */
		memcpy( (void *)&pcmstm_exec_fntbl,
				(void *)&pcmstm_exec_fntbl_org,
					sizeof(PcmExecFunc) );

		/*  DMA 転送先の設定  */
		PCM_StmSetDmaDst(PCMSTM_TRDST_WRH);
	}

	return;
}

/*  1996.07.08 Y.H  (start)  */
/*  DMA 転送先の設定  */
void	PCM_StmSetDmaDst(Sint32 dst_flag)
{
	pcmstm_dma_dst = dst_flag;

	return;
}
/*  1996.07.08 Y.H  ( end )  */


/********************************************************************/
/*  SAP 使用宣言の取り消し  										*/
/********************************************************************/
/*  1996.07.08 Y.H  (start)  */
void	PCM_UnDeclareUseSapStm(void)
{
	pcm_sap_flag = OFF;

	stmInit();

	/*  リングバッファ管理関数の登録  */
	PCM_MeEtyFnAiff();

	return;
}
/*  1996.07.08 Y.H  ( end )  */


/********************************************************************/
/* ハンドルの作成（ストリームシステム）								*/
/* [入力]															*/
/*    para : 作成パラメータ											*/
/*    stm  : ストリームハンドル										*/
/* [関数値]															*/
/*    ハンドル（作成できない場合は NULL)							*/
/********************************************************************/
PcmHn	PCM_CreateStmHandle(PcmCreatePara *para, StmHn stm)
{
	PcmHn   	pcm;

	if (PCM_lib_stm_init == OFF) {
		stmInit();
		PCM_lib_stm_init = ON;
	}

	if (stm == NULL) {
		PCM_MeSetErrCode(PCM_ERR_ILLEGAL_PARA);
		return NULL;
	}

	/*  作成パラメータのチェック  */
	PCMLIB_CHK_CREATE_PARA(para);

	if (pcm_sap_flag != OFF) {
		/*  ◇ SAP  */
		/*  ハンドル作成  */
		pcm =  PCM_StmCreate(stm, 
						 (PcmWork *)PCM_PARA_WORK(para), 
						 PCM_PARA_XWORK_ADDR(para), PCM_PARA_XWORK_SIZE(para),
						 PCM_PARA_PCM_ADDR(para), PCM_PARA_PCM_SIZE(para));
	}
	else {
		/*  ◇ AIFF  */
		/*  ハンドル作成  */
		pcm =  PCM_StmCreate(stm, 
						 (PcmWork *)PCM_PARA_WORK(para), 
						 PCM_PARA_RING_ADDR(para), PCM_PARA_RING_SIZE(para),
						 PCM_PARA_PCM_ADDR(para), PCM_PARA_PCM_SIZE(para));
	}

	if (pcm != NULL) {
		PCMLIB_FACCESS_TYPE(pcm) = PCMLIB_FACCESS_STM;

		/* 実行関数の設定 */
		PCMLIB_SET_START_FUNC(pcm, PCM_StmStart);
		PCMLIB_SET_TASK_FUNC(pcm, PCM_StmTask);
		PCMLIB_SET_PRELOAD_FILE_FUNC(pcm, PCM_StmPreloadFile);
		PCMLIB_SET_SET_LOAD_NUM_FUNC(pcm, PCM_StmSetLoadNum);
		PCMLIB_SET_SET_TRMODE_FUNC(pcm, PCM_StmSetTrMode);

		/* ローカルデータを初期化 */
		PCMSTM_OLD_CD_BUFNUM(pcm) = -1;
		PCMSTM_DMA_STATE(pcm) = OFF;
		PCMSTM_DMA_SECT(pcm) = 0;
		PCMSTM_WRITE_ADDR(pcm) = NULL;
		PCMSTM_BUF_BSIZE(pcm) = 0;
		PCMSTM_WRITE_BSIZE(pcm) = 0;
		PCMSTM_SECT_BSIZE(pcm) = SECT_SIZE_2048;
		PCMSTM_AUDIO_1ST_SECT(pcm) = 0;
		PCMSTM_LOAD_FUNC(pcm) = pcmstm_loadDmaCpu;
		PCMSTM_LOAD_TOTAL_SECT(pcm) = 0;

		/* ストリームハンドルの設定 */
		PCMSTM_HANDLE(pcm) = stm;

		/* 転送関数の設定 */
		if (pcm_sap_flag != OFF) {
			/*  SAP  */
			PCM_StmSetTrMode(pcm, PCM_TRMODE_SCU);
		}
		else {
			/*  AIFF */
			STM_SetTrFunc(PCMSTM_HANDLE(pcm), PCMSTM_LOAD_FUNC(pcm), pcm);
		}

		/* 最大転送セクタ数の設定 */
		PCMSTM_LOAD_SECT(pcm) = LOAD_SECT_NUM;
		STM_SetTrPara(PCMSTM_HANDLE(pcm), PCMSTM_LOAD_SECT(pcm));

	}

	return pcm;
}


/********************************************************************/
/* ハンドルの消去（ストリームシステム）								*/
/*   ハンドルを破棄した後は、ハンドルを引き数にもつ関数は利用でき	*/
/*   ない															*/
/* [入力]															*/
/*    pcm  : ハンドル												*/
/********************************************************************/
void	PCM_DestroyStmHandle(PcmHn pcm)
{
	/* ハンドルチェック */
	PCMLIB_CHK_HANDLE(pcm);

	if (PCMLIB_FACCESS_TYPE(pcm) != PCMLIB_FACCESS_STM) {
		PCM_MeSetErrCode(PCM_ERR_ILL_CREATE_MODE);
	}

	/* ＤＭＡ転送の終了を待つ */
	(*pcmstm_wait_dma)(pcm);	/*  1996.06.11 Y.H : 関数ポインタにした  */

	/* 転送関数の解除 */
	STM_SetTrFunc(PCMSTM_HANDLE(pcm), pcmstm_loadDummy, NULL);

	PCM_MeDestroy(pcm);

	return;
}


/********************************************************************/
/* ハンドルを生成する												*/
/*   ストリームハンドルは、ストリームシステムにより予め取得すること	*/
/*																	*/
/* [引き数]															*/
/*	stm 		: ストリームハンドル								*/
/*	work 		: ワークアドレス									*/
/*	buf 		: バッファアドレス（ SAP 作業領域アドレス）			*/
/*	bufsize 	: バッファのバイト数（ SAP 作業領域サイズ）			*/
/*	pcmbuf		:ＰＣＭのウェーブＲＡＭの先頭アドレス				*/
/*	pcmsize		:ＰＣＭのウェーブＲＡＭのバイト数					*/
/*																	*/
/* [関数値]															*/
/*    ハンドル														*/
/*    NULL の場合はエラー											*/
/*																	*/
/********************************************************************/
PcmHn	PCM_StmCreate(StmHn stm, PcmWork *work , 
			void *buf, Sint32 bufsize, void *pcmbuf, Sint32 pcmsize)
{
	PcmPara		para;

	/* リングバッファ（SAP 作業領域アドレス） */
	para.ring_addr = buf;
	para.ring_size = bufsize;

	/* トリガサイズ */
	para.start_trg_size 	= PCM_DEFAULT_SIZE_START_TRG;
	para.start_trg_sample 	= PCM_DEFAULT_SAMPLE_START_TRG;
	para.stop_trg_sample 	= PCM_DEFAULT_SAMPLE_STOP_TRG;

	/* サウンドドライバコマンドブロック番号 */
	para.command_blk_no = PCMLIB_COMMAND_BLK_NO;

	/* ＰＣＭストリーム再生番号 */
	para.pcm_stream_no = PCMLIB_PCM_STREAM_NO;

	/* ＰＣＭバッファ */
	para.pcm_addr = pcmbuf;
	para.pcm_size = pcmsize;

	/* ＰＡＮとボリューム */
	para.pcm_pan   = PCMLIB_PCM_PAN;
	para.pcm_level = PCMLIB_PCM_LEVEL;
	para.pcm_total_level = 0;  /* tany 97/8/17 */

	/* ハンドルの作成 */
	return PCM_MeCreate(work, &para);
}


/********************************************************************/
/* ＣＤバッファからの最大転送セクタ数を設定する						*/
/* [入力]															*/
/*    pcm  : ハンドル												*/
/*    load_sct : 最大転送セクタ数									*/
/********************************************************************/
void PCM_StmSetLoadNum(PcmHn pcm, Sint32 load_sct)
{
	PCMSTM_LOAD_SECT(pcm) = load_sct;

	/* ＣＤバッファからの最大転送セクタ数を設定する */
	STM_SetTrPara(PCMSTM_HANDLE(pcm), PCMSTM_LOAD_SECT(pcm));

	return;
}


/********************************************************************/
/* 再生を先頭から開始する											*/
/*																	*/
/* [引き数]															*/
/*    pcm : ハンドル												*/
/********************************************************************/
void PCM_StmStart(PcmHn pcm)
{
	/* 再生開始 */
	PCM_MeStart(pcm);

	return;
}


/********************************************************************/
/* コーディング情報(ci)の取得と設定 								*/
/********************************************************************/
void	PCM_GetSetCi(PcmHn pcm)
{
	StmSct 			sinfo;

	if (PCM_MeIsNotSetCi(pcm)) {	/* コーディング情報(ci)の取得が必要 */
		for (;;) {
			/* セクタ情報を得る */
			if (STM_GetSctInfo(PCMSTM_HANDLE(pcm), PCMSTM_AUDIO_1ST_SECT(pcm), 
								&sinfo) == TRUE) {
				if (STM_SCT_SM(&sinfo) & 0x40) {
					/* コーディング情報(ci)から得た情報を設定する */
					PCM_MeSetCi(pcm, STM_SCT_CI(&sinfo));
				} else {
					PCMSTM_AUDIO_1ST_SECT(pcm)++;
					continue;
				}
			}
			break;
		}
	}

	return;
}


/********************************************************************/
/* 再生タスク														*/
/*   再生中は定期的にこの関数を呼ぶ									*/
/*   呼ぶ間隔はＰＣＭバッファの半分の再生時間以下で定期的に呼ぶ		*/
/*   必要頻度より少ないと再生が乱れる								*/
/*																	*/
/* [引き数]															*/
/*    pcm : ハンドル												*/
/*     NULL が指定された時は現在オープンされているハンドル   		*/
/*     をすべての再生中処理をする									*/
/********************************************************************/
void	PCM_StmTask(PcmHn pcm)
{
	(PCM_TASK_FUNC(&pcmstm_exec_fntbl))(pcm);

	return;
}

/*  AIFF 版 Task 関数  */
void	pcmstm_task_aiff(PcmHn pcm)
{

	if (PCM_IsDeath(pcm)) {
		return;
	}

#if 0
	/* コーディング情報(ci)の取得と設定 */
	PCM_GetSetCi(pcm);

	/* ＣＤバッファからリングバッファへデータを転送する */
	pcmstm_loadBuf(pcm);
#endif

	/* タスク処理 */
	PCM_MeTask(pcm);

	return;
}


#if		0
/********************************************************************/
/* ＣＤバッファからのリングバッファにデータを転送する				*/
/*																	*/
/* [引き数]															*/
/*    pcm : ハンドル												*/
/********************************************************************/
STATIC void pcmstm_loadBuf(PcmHn pcm)
{
	Uint32 write_addr32;
	Sint32 write_size;
	Sint32 total_size;
	Sint32	cd_buf_num;


	/* リングバッファの空きバイト数を得る */
	PCM_MeGetRingWrite(pcm, (Sint8 **)&write_addr32, &write_size, &total_size);
	PCMSTM_WRITE_ADDR(pcm) = (Uint32 *)write_addr32;
	PCMSTM_BUF_BSIZE(pcm) = write_size;
	PCMSTM_WRITE_BSIZE(pcm) = 0;

	if (write_size >= PCMSTM_SECT_BSIZE(pcm)) {

		/* ＣＤからの読み込んだデータをリングバッファに転送する */
		cd_buf_num = STM_GetNumCdbuf(PCMSTM_HANDLE(pcm));
		DEBUG_PRINT_CDBUF(cd_buf_num);

		/* ＣＤバッファにたまっているセクタ数が転送セクタ数になったら */
		/* 実際の転送処理をする */
		/* ただし、最初と最後はたまっていなくても転送する */
#if 0
		if (PCMSTM_OLD_CD_BUFNUM(pcm) == -1 || 
							PCMSTM_OLD_CD_BUFNUM(pcm) == cd_buf_num ||
							cd_buf_num >= PCMSTM_LOAD_SECT(pcm)) {}
#else
		if (cd_buf_num == 0 || 
			cd_buf_num >= PCMSTM_LOAD_SECT(pcm)) {
#endif
			pcmlib_exec_pcm = pcm;
			STM_ExecServer();
			pcmlib_exec_pcm = NULL;
		}
		PCMSTM_OLD_CD_BUFNUM(pcm) = cd_buf_num;
	}

}
#endif


/********************************************************************/
/* ＤＭＡ転送の終了を待ち（ AIFF ）									*/
/*																	*/
/* [引き数]															*/
/*    pcm : ハンドル												*/
/********************************************************************/
void	pcmstm_waitDma_aiff(PcmHn pcm)
{
	if (PCMSTM_DMA_STATE(pcm) == ON) {
		if (PCMSTM_LOAD_FUNC(pcm) == pcmstm_loadDmaCpu) {
			while (PCM_StmDmaCpuResult() == TRUE) ;
			pcmstm_dma_cpu_start = OFF;
		}
		else if (PCMSTM_LOAD_FUNC(pcm) == pcmstm_loadDmaScu) {
			while (PCM_StmDmaScuResult() == TRUE) ;
			pcmstm_dma_scu_start = OFF;
		}
		PCMSTM_DMA_STATE(pcm) = OFF;
	}

	return;
}


/**************************************************************/
/*  ダミー転送 												  */
/* 何も転送せず０を返す										  */
/**************************************************************/
STATIC Sint32 pcmstm_loadDummy(void *obj, StmHn stm, Sint32 nsct)
{
	return (0);
}


/**************************************************************/
/*  ＣＰＵのＤＭＡの転送関数 								  */
/**************************************************************/
/*  CPU-DMA 転送エントリ  */
Sint32 pcmstm_loadDmaCpu(void *obj, StmHn stm2, Sint32 nsct)
{
	PcmHn 		pcm;
	Sint32 		adlt;
	Sint32		trans_nsct;
	Sint32 		write_addr32, write_size, total_size;
	Sint32		tsz;
	PcmStmTransPara	trpara, *ptrpara;

	pcm = (PcmHn)obj;

	_VTV_PRINTF((VTV_s, "P:_loadDmaCpu hn%d\n stm%X sct%d\n", 
		PCM_MeGetHandleNo(pcm), stm2, nsct));

	if (PCM_IsDeath(pcm)) {
		return 0;
	}

	if (PCMSTM_DMA_STATE(pcm) == ON) {
		if (PCM_StmDmaCpuResult() == TRUE) {
			return -1;	/* まだ、転送中 */
		}
		else {
			PCMSTM_DMA_STATE(pcm) = OFF;
			tsz = PCMSTM_SCT2B(pcm, PCMSTM_DMA_SECT(pcm));
			PCMSTM_WRITE_BSIZE(pcm) += tsz;
			PCM_MeRenewRingWrite(pcm, tsz);
			return PCMSTM_DMA_SECT(pcm);
		}
	}

	/* リングバッファの空きバイト数を得る */
	PCM_MeGetRingWrite(pcm, (Sint8 **)&write_addr32, &write_size, &total_size);

	/* 転送するセクタ数を計算する */
	trans_nsct = write_size / PCMSTM_SECT_BSIZE(pcm);
	trans_nsct = MIN(trans_nsct, nsct);

	if (trans_nsct <= 0) {
		return 0;
	}

	/* コーディング情報(ci)の取得と設定 */
	PCM_GetSetCi(pcm);

	/* 転送 */
	/*  1996.07.09 Y.H  (start)  */
	ptrpara = &trpara;
	PCMSTM_TRPARA_DST(ptrpara)	 = (void *)write_addr32;
	PCMSTM_TRPARA_SRC(ptrpara)	 = (void *)STM_StartTrans(stm2, &adlt);
	PCMSTM_TRPARA_DSIZ(ptrpara)  = (Sint32)PCMSTM_SCT2D(pcm, trans_nsct);
	PCM_StmDmaCpuMemCopy4(ptrpara, 1);
	/*  1996.07.09 Y.H  ( end )  */

	PCMSTM_DMA_SECT(pcm) = trans_nsct;
	PCMSTM_DMA_STATE(pcm) = ON;

	return -1;	/* 転送中 */
}

/*  1996.07.08 Y.H  (start)  転送関数設定 I/F 追加  */
/*  CPU-DMA 転送関数  */
Sint32	PCM_StmDmaCpuMemCopy4(PcmStmTransPara *ptrpara, Sint32 n)
{
    DmaCpuComPrm com_prm;                       /* 共通転送パラメータ        */
    DmaCpuPrm prm;                              /* 転送パラメータ            */
	void	*src, *dst;
	Uint32	cnt;

	dst = PCMSTM_TRPARA_DST(ptrpara);
	src = PCMSTM_TRPARA_SRC(ptrpara);
	cnt = (Uint32)PCMSTM_TRPARA_DSIZ(ptrpara);
                                                /*****************************/
    DMA_CpuStop(DMA_CPU_CH1);                   /* DMA転送中止               */

    com_prm.pr = DMA_CPU_FIX;                   /* 優先順位設定(ラウンドロビン)   */
    com_prm.dme = DMA_CPU_ENA;                  /* DMAマスタイネーブル設定(許可)    */
    com_prm.msk = DMA_CPU_M_PR |                /* マスク設定(プライオリティモード)  */
                  DMA_CPU_M_AE |                /* (アドレスエラーフラグ)    */
                  DMA_CPU_M_NMIF |              /* (NMIフラグ)               */
                  DMA_CPU_M_DME;                /* (DMAマスタイネーブル)     */

    DMA_CpuSetComPrm(&com_prm);                 /* DMA共通転送パラメータ設定 */

    prm.sar = (Uint32)src;                      /* ソースアドレス設定        */
    prm.dar = (Uint32)dst;                      /* ディスティネーションアドレス設定      */
    prm.tcr = cnt;                              /* トランスファカウント設定  */
    prm.dm = DMA_CPU_AM_ADD;                    /* ディスティネーションアドレスモード設定  */
    prm.sm = DMA_CPU_AM_NOM;                    /* ソースアドレスモード設定  moke    */
    prm.ts = DMA_CPU_4;                         /* トランスファサイズ設定            */
    prm.ar = DMA_CPU_AUTO;                      /* オートリクエストモード設定          */
    prm.ie = DMA_CPU_INT_DIS;                   /* インタラプトイネーブル設定         */

    prm.msk = DMA_CPU_M_SAR |                   /* マスク設定                */
              DMA_CPU_M_DAR |
              DMA_CPU_M_TCR |
              DMA_CPU_M_DM  |
              DMA_CPU_M_SM  |
              DMA_CPU_M_TS  |
              DMA_CPU_M_AR  |
              DMA_CPU_M_IE  |
              DMA_CPU_M_TE;                     /* トランスファエンドビットのクリア指定*/

    DMA_CpuSetPrm(&prm, DMA_CPU_CH1);           /* DMA転送パラメータ設定     */

    DMA_CpuStart(DMA_CPU_CH1);                  /* DMA転送開始               */
    dma_cpu_dis_adr = dst;
    dma_cpu_cnt = cnt * 4;

	return (0);
}
/*  1996.07.08 Y.H  ( end )  */

/*  1996.07.08 Y.H  (start)  関数型を Bool に変更  */
/*  CPU-DMA 終了検査  */
/*	TRUE:転送中		FALSE:転送終了  */
Bool	PCM_StmDmaCpuResult(void)
{
	DmaCpuComStatus	com_status;				/* 共通ステータス                */
	DmaCpuStatus	status;					/* ステータス                    */
											/*********************************/
	DMA_CpuGetComStatus(&com_status);		/* 共通ステータス取得            */
	if (com_status.ae == DMA_CPU_ADR_ERR) {	/* アドレスエラーが発生した場合        */
		return (FALSE);						/* 異常終了 = DMA_CPU_FAIL       */
	}
	status = DMA_CpuGetStatus(DMA_CPU_CH1);	/* ステータス取得                */
	if (status == DMA_CPU_TE_MV) {			/* 動作中である場合              */
		return (TRUE);						/* 実行中   = DMA_CPU_BUSY       */
	}
	CSH_Purge(dma_cpu_dis_adr, dma_cpu_cnt);

	return (FALSE);							/* 正常終了 = DMA_CPU_END        */
}
/*  1996.07.08 Y.H  ( end )  */


/**************************************************************/
/*  ＳＣＵのＤＭＡの転送関数 								  */
/**************************************************************/
/*  SCU-DMA 転送エントリ  */
Sint32	pcmstm_loadDmaScu(void *obj, StmHn stm2, Sint32 nsct)
{
	PcmHn		pcm;
	Sint32 		adlt;
	Sint32		trans_nsct;
	Sint32 		write_addr32, write_size, total_size;
	Sint32		tsz;
	PcmStmTransPara	trpara, *ptrpara;

	pcm = (PcmHn)obj;

	_VTV_PRINTF((VTV_s, "P:_loadDmaScu hn%d\n stm%X sct%d\n", 
		PCM_MeGetHandleNo(pcm), stm2, nsct));

	if (PCM_IsDeath(pcm)) {
		return 0;
	}

	if (PCMSTM_DMA_STATE(pcm) == ON) {
		if (PCM_StmDmaScuResult() == TRUE) {
			return -1;/* まだ、転送中 */
		}
		else {
			PCMSTM_DMA_STATE(pcm) = OFF;
			tsz = PCMSTM_SCT2B(pcm, PCMSTM_DMA_SECT(pcm));
			PCMSTM_WRITE_BSIZE(pcm) += tsz;
			PCM_MeRenewRingWrite(pcm, tsz);
			return PCMSTM_DMA_SECT(pcm);
		}
	}

	/* リングバッファの空きバイト数を得る */
	PCM_MeGetRingWrite(pcm, (Sint8 **)&write_addr32, &write_size, &total_size);

	/* 転送するセクタ数を計算する */
	trans_nsct = write_size / PCMSTM_SECT_BSIZE(pcm);
	trans_nsct = MIN(trans_nsct, nsct);

	if (trans_nsct <= 0) {
		return 0;
	}

	/* コーディング情報(ci)の取得と設定 */
	PCM_GetSetCi(pcm);

	/* 転送 */
	/*  1996.07.09 Y.H  (start)  */
	ptrpara = &trpara;
	PCMSTM_TRPARA_DST(ptrpara)	 = (void *)write_addr32;
	PCMSTM_TRPARA_SRC(ptrpara)	 = (void *)STM_StartTrans(stm2, &adlt);
	PCMSTM_TRPARA_DSIZ(ptrpara)  = (Sint32)PCMSTM_SCT2D(pcm, trans_nsct);
	PCM_StmDmaScuMemCopy(ptrpara, 1);
	/*  1996.07.09 Y.H  ( end )  */

	PCMSTM_DMA_SECT(pcm) = trans_nsct;
	PCMSTM_DMA_STATE(pcm) = ON;

	return -1;	/* 転送中 */
}

/*  1996.07.08 Y.H  (start)  転送関数設定 I/F 追加  */
/*  SCU-DMA 転送関数  */
Sint32	PCM_StmDmaScuMemCopy(PcmStmTransPara *ptrpara, Sint32 n)
{
    DmaScuPrm	prm;
    Uint32		msk;

    msk = get_imask();
    set_imask(15);

    prm.dxr = (Uint32)PCMSTM_TRPARA_SRC(ptrpara);
    prm.dxw = (Uint32)PCMSTM_TRPARA_DST(ptrpara);
    prm.dxc = (Uint32)(PCMSTM_TRPARA_DSIZ(ptrpara) << 2);

    dma_scu_dis_adr = (void *)prm.dxw;
    dma_scu_cnt     = (Uint32)prm.dxc;

    prm.dxad_r = DMA_SCU_R0;
	if (pcmstm_dma_dst == PCMSTM_TRDST_WRH) {
		/*  WORK-RAM High  */
	    prm.dxad_w = DMA_SCU_W4;
	}
	else {
		/*  SCSP(B-BUS)  */
	    prm.dxad_w = DMA_SCU_W2;
	}
    prm.dxmod = DMA_SCU_DIR;
    prm.dxrup = DMA_SCU_KEEP;
    prm.dxwup = DMA_SCU_KEEP;
    prm.dxft = DMA_SCU_F_DMA;
    prm.msk = DMA_SCU_M_DXR    |
              DMA_SCU_M_DXW    ;

    DMA_ScuSetPrm(&prm, DMA_SCU_CH0);
    DMA_ScuStart(DMA_SCU_CH0);
    dma_start_flg = ON;                         /* DMAはスタートしている     */
    set_imask(msk);                                         /* 割り込みPOP   */

	return (0);
}
/*  1996.07.08 Y.H  ( end )  */

/*  1996.07.08 Y.H  (start)  関数型を Bool に変更  */
/*  SCU-DMA 終了検査  */
/*	TRUE:転送中		FALSE:転送終了  */
Bool	PCM_StmDmaScuResult(void)
{
	DmaScuStatus	status;
	Uint32	msk;

	if (dma_start_flg == ON) {
		/*  DMAを開始している時  */
		msk = get_imask();
		set_imask(15);
		DMA_ScuGetStatus(&status, DMA_SCU_CH0);
		if (status.dxmv == DMA_SCU_MV) {
			set_imask(msk);			/* 割り込みPOP   */
			return (TRUE);			/*  DMA_SCU_BUSY  */
		}
		CSH_Purge(dma_scu_dis_adr, dma_scu_cnt);
		set_imask(msk);				/* 割り込みPOP   */
		return (FALSE);				/*  DMA_SCU_END  */
	}
	else {
		/*  DMAを開始していない時  */
		return (FALSE);				/*  DMA_SCU_END  */
	}
}
/*  1996.07.08 Y.H  ( end )  */


/**************************************************************/
/*  ソフトウェア転送の転送関数								  */
/**************************************************************/
Sint32	pcmstm_loadCpu(void *obj, StmHn stm, Sint32 nsct)
{
	PcmHn		pcm;
	Uint32 		*src, *dst;
	Sint32 		adlt;
	long		i;
	Sint32		trans_nsct;
	Sint32		trans_size;
	Sint32 		write_addr32, write_size, total_size;
	Sint32		tsz;

	pcm = (PcmHn)obj;

	_VTV_PRINTF((VTV_s, "P:_loadCpu hn%d\n stm%X sct%d\n", 
		PCM_MeGetHandleNo(pcm), stm2, nsct));

	if (PCM_IsDeath(pcm)) {
		return 0;
	}

	/* リングバッファの空きバイト数を得る */
	PCM_MeGetRingWrite(pcm, (Sint8 **)&write_addr32, &write_size, &total_size);

	/* 転送するセクタ数を計算する */
	trans_nsct = write_size / PCMSTM_SECT_BSIZE(pcm);
	trans_nsct = MIN(trans_nsct, nsct);

	if (trans_nsct <= 0) {
		return 0;
	}

	/* コーディング情報(ci)の取得と設定 */
	PCM_GetSetCi(pcm);

	/* 転送 */
	src = STM_StartTrans(stm, &adlt);
	dst = (Uint32 *)write_addr32;
	i = trans_size = PCMSTM_SCT2D(pcm, trans_nsct);
	while (--i >= 0) {
		*dst++ = *src;
		src += adlt;
	}

	/* リングバッファへの書き込みサイズの報告 */
	tsz = trans_size << 2;
	PCMSTM_WRITE_BSIZE(pcm) += tsz;
	PCM_MeRenewRingWrite(pcm, tsz);

	return trans_nsct;
}


/********************************************************************/
/* ＣＤからリングバッファにファイルを読み込む						*/
/*																	*/
/* [引き数]															*/
/*    pcm : ハンドル												*/
/*    size : 読み込むバイト数										*/
/*           リングバッファが読み込みバイト数より小さい時は			*/
/*           バッファサイズになる。									*/
/*           読み込むバイト数はセクタ単位（２０４８の倍数）になる。	*/
/* [関数値]															*/
/*    実際に読み込んだバイト数										*/
/********************************************************************/
Sint32	PCM_StmPreloadFile(PcmHn pcm, Sint32 size)
{
	return ( (PCM_PRELOAD_FILE_FUNC(&pcmstm_exec_fntbl))(pcm, size) );
}

/*  AIFF 版 Preload 関数  */
Sint32	pcmstm_preloadFile_aiff(PcmHn pcm, Sint32 size)
{
	Uint32		write_addr32;
	Sint32		write_size, total_size;
	Sint32		load_sct, load_size;

	load_size = 0;

	/*  リングバッファの空きバイト数を得る  */
	PCM_MeGetRingWrite(pcm, (Sint8 **)&write_addr32, &write_size, &total_size);

	if (write_size >= PCMSTM_SECT_BSIZE(pcm)) {
		/* ＣＤからの読み込んだデータをリングバッファに転送する */
		if (size > write_size) {
			load_size = write_size;
		}
		else {
			load_size = size;
		}
		load_sct  = load_size / PCMSTM_SECT_BSIZE(pcm);
		load_size = load_sct * PCMSTM_SECT_BSIZE(pcm);

		if (load_sct > 0) {
			PCMSTM_WRITE_ADDR(pcm)  = (Uint32 *)write_addr32;
			PCMSTM_BUF_BSIZE(pcm)   = load_size;
			PCMSTM_WRITE_BSIZE(pcm) = 0;

			pcmlib_exec_pcm = pcm;
			while (STM_ExecServer() != STM_EXEC_COMPLETED) {
				if ( (PCMSTM_WRITE_BSIZE(pcm) >= PCMSTM_BUF_BSIZE(pcm)) &&
						(PCMSTM_DMA_STATE(pcm) == OFF) ) {
					break;
				}
			}
			pcmlib_exec_pcm = NULL;

			load_size = PCMSTM_WRITE_BSIZE(pcm);
		}
	}

	return load_size;
}


/********************************************************************/
/* 転送方式の設定													*/
/* [入力]															*/
/*    pcm   : ムービハンドル										*/
/*    mode  : 転送方式												*/
/*      PCM_TRMODE_CPU : ソフトウェア転送							*/
/*      PCM_TRMODE_SDMA : ＤＭＡサイクルスチール					*/
/*      PCM_TRMODE_SCU  : ＳＣＵのＤＭＡ							*/
/********************************************************************/
void	PCM_StmSetTrMode(PcmHn pcm, PcmTrMode mode)
{
	(PCM_TRMODE_FUNC(&pcmstm_exec_fntbl))(pcm, mode);

	return;
}

/*  AIFF 版 SetTrMode 関数  */
void	pcmstm_setTrMode_aiff(PcmHn pcm, PcmTrMode mode)
{

	switch(mode) {
	case PCM_TRMODE_CPU:
	default:
		PCMSTM_LOAD_FUNC(pcm) = pcmstm_loadCpu;
		break;
	case PCM_TRMODE_SDMA:
		PCMSTM_LOAD_FUNC(pcm) = pcmstm_loadDmaCpu;
		break;
	case PCM_TRMODE_SCU:
		PCMSTM_LOAD_FUNC(pcm) = pcmstm_loadDmaScu;
		break;
	}

	/* 転送関数の設定 */
	STM_SetTrFunc(PCMSTM_HANDLE(pcm), PCMSTM_LOAD_FUNC(pcm), pcm);

	return;
}


/**************************************************************
 *  	ストリーム関数の登録（ AIFF ）
 **************************************************************/
/*  ストリーム関数の登録  */
void	PCM_StmEtyFn( PcmExecFunc *fnlst, void (*dmafn)(PcmHn pcm) )
{
	/*  ストリーム関数  */
	pcmstm_exec_fntbl = *fnlst;

	/*  DMA 終了検査関数  */
	pcmstm_wait_dma = dmafn;

	return;
}




/*  end of file  */
