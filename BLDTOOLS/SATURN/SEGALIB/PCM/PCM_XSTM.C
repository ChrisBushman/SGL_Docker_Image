/******************************************************************************
 *	ソフトウェアライブラリ
 *
 *	Copyright (c) 1994,1995,1996,1997 SEGA
 *
 * Library	:ＰＣＭ・ＡＤＰＣＭ再生ライブラリ
 * Module 	:ストリームアクセス（SaurnPCM ファイル）
 * File		:pcm_xstm.c
 * Date		:1997-06-23
 * Version	:1.24
 * Author	:Y.H
 *
 *--------------------------------------------------------------------------*
 * Update	:1996-07-05	1.00	Y.H	新規作成
 *			:1996-10-14	1.21	Y.H	SHC Ver. 3.0F対応
 *
 ****************************************************************************/




/************************************************************************/
/*		□条件コンパイル												*/
/************************************************************************/




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
/*		□定数マクロ													*/
/************************************************************************/
/*  SCU DSP-DMA のための定数  */
#define	PCMSTM_TRIMM_MAXELM		(8)		/*  最大数        */
#define	PCMSTM_TRIMM_DEFELM		(4)		/*  デフォルト値  */



/************************************************************************/
/*		□外部関数宣言													*/
/************************************************************************/


/************************************************************************/
/*		□関数宣言														*/
/************************************************************************/
Sint32	pcmstm_loadSap(void *obj, StmHn stm2, Sint32 nsct);
void	pcmstm_task_sap(PcmHn pcm);
Sint32	pcmstm_preloadFile_sap(PcmHn pcm, Sint32 size);
void	pcmstm_setTrMode_sap(PcmHn pcm, PcmTrMode mode);



/************************************************************************/
/*		□変数定義														*/
/************************************************************************/
/*  1996.07.08 Y.H  (start)  */
/*  転送処理関数へのポインタ  */
/*  転送要求関数  */
static	Sint32	(*pcmstm_trfn_load)(PcmStmTransPara *ptrpara, Sint32 n);
/*  転送検査関数  */
static	Bool	(*pcmstm_trfn_check)(void);

/*  STM_StartTrans での転送アドレス変化分  */
static	Sint32	pcmstm_trans_adlt;

/*  ストリーム関数  */
static	PcmExecFunc	pcmstm_fntbl_sap = {
	NULL,
	pcmstm_task_sap,
	pcmstm_preloadFile_sap,
	NULL,
	pcmstm_setTrMode_sap
};
/*  1996.07.08 Y.H  ( end )  */




/************************************************************************/
/*		□関数定義														*/
/************************************************************************/
/**************************************************************
 *		SAP 版 Task 関数（STM 用）
 **************************************************************/
void	pcmstm_task_sap(PcmHn pcm)
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




/**************************************************************
 *		SAP 版 Preload 関数（STM 用）
 **************************************************************/
Sint32	pcmstm_preloadFile_sap(PcmHn pcm, Sint32 size)
{
	PcmWork		*work 	= *(PcmWork **)pcm;
	PcmStatus	*st 	= &work->status;

	Uint32	write_addr32;
	Sint32	write_size, total_size;
	Sint32	req_sct, hdr_sct, bdy_sct, unit_sct;
	Sint32	start_sct, end_sct;
	Sint32	playbak;

	if (PCMSTM_DMA_STATE(pcm) == ON) {
		/*  DMA 転送中  */
		return (0);
	}

	req_sct = size / PCM_SIZE_2K;
	if (req_sct == 0) {
		/*  １セクタ未満は処理しない  */
		return (0);
	}

	/*  演奏状態に仮設定  */
	playbak   = st->play;
	st->play  = PCM_STAT_PLAY_START;
	start_sct = PCMSTM_LOAD_TOTAL_SECT(pcm);
	pcmlib_exec_pcm = pcm;		/*  preload フラグ  */

	/*  ヘッダサイズ  */
	if (start_sct == 0) {
		/*  ◇ヘッダ読込みは未だ  */
		hdr_sct = PCM_SAP_HEADER_SIZE / PCMSTM_SECT_BSIZE(pcm);

		/*  ヘッダを読み込む（含ヘッダ解析）  */
		PCMSTM_LOAD_LIMIT_SECT(pcm) = hdr_sct;
		PCMXLIB_STAT_XSTAT(st)  = PCMXLIB_XSTAT_BLKHD;
		while (STM_ExecServer() != STM_EXEC_COMPLETED) {
			if ( (PCMSTM_LOAD_TOTAL_SECT(pcm) >= hdr_sct) &&
					(PCMSTM_DMA_STATE(pcm) == OFF) ) {
				break;
			}
		}
	}
	else {
		/*  ◇ヘッダ読込み済み  */
		hdr_sct = 0;
	}

	/*  ボディサイズ  */
	PCM_MeGetRingWrite(pcm, (Sint8 **)&write_addr32, &write_size, &total_size);
	bdy_sct = write_size / PCMSTM_SECT_BSIZE(pcm);
	if (PCM_IS_STEREO(st) != 0) {
		bdy_sct <<= 1;
	}

	/*  読込みセクタ数制限  */
	if ( req_sct > (hdr_sct + bdy_sct) ) {
		/*  ヘッダとボディをすべて読み込む  */
		req_sct = hdr_sct + bdy_sct;
	}
	else {
		req_sct -= hdr_sct;
		unit_sct = PCMXLIB_STAT_XBLK_B2CH(st) / PCMSTM_SECT_BSIZE(pcm);
		req_sct = (req_sct / unit_sct) * unit_sct;
		req_sct += hdr_sct;
	}

	/*  ボディを読み込む  */
	end_sct = start_sct + req_sct;
	PCMSTM_LOAD_LIMIT_SECT(pcm) = end_sct;
	while (STM_ExecServer() != STM_EXEC_COMPLETED) {
		if ( (PCMSTM_LOAD_TOTAL_SECT(pcm) >= end_sct) &&
				(PCMSTM_DMA_STATE(pcm) == OFF) ) {
			break;
		}
	}

	/*  演奏状態を戻す  */
	st->play = playbak;
	pcmlib_exec_pcm = NULL;

	return ((PCMSTM_LOAD_TOTAL_SECT(pcm)-start_sct) * PCMSTM_SECT_BSIZE(pcm));
}




/**************************************************************
 *  	ＣＤ→ＰＣＭ転送関数群
 **************************************************************/
/*----------------------------------------------*
 *  ソフトウェア転送　転送終了検査関数
 *----------------------------------------------*/
static	Bool	pcmstm_checkCpu_sap(void)
{
	/*  転送完了  */
	return (FALSE);
}


/*----------------------------------------------*
 *  ソフトウェア転送　転送処理関数
 *----------------------------------------------*/
static Sint32 pcmstm_loadCpu_sap(PcmStmTransPara *para, Sint32 n)
{
	Sint32	cnt;
	Uint32	*src, *dst;
	Sint32	adlt;

	src  = (Uint32 *)PCMSTM_TRPARA_SRC(para);
	dst  = (Uint32 *)PCMSTM_TRPARA_DST(para);
	cnt  = (Sint32)PCMSTM_TRPARA_DSIZ(para);
	adlt = pcmstm_trans_adlt;

	while (--cnt >= 0) {
		*dst++ = *src;
		src += adlt;
	}

	return (0);
}


/*----------------------------------------------*
 *  ヘッダの転送パラメタを得る
 *----------------------------------------------*/
static	Sint32	pcmstm_getTrInf_header(PcmHn pcm, PcmStatus *st,
										Sint32 nsct, Uint32 **ppdst)
{
	Sint32	trans_nsct;

	*ppdst = (Uint32 *)PCMXLIB_STAT_XBLK_ADDR(st);
	trans_nsct = (PCM_SAP_HEADER_SIZE / PCM_SIZE_2K);

	/*  ヘッダ転送を指示  */
	PCMXLIB_STAT_XSTAT(st) = PCMXLIB_XSTAT_BLKHD;

	/*  転送パラメタ設定  */
	trans_nsct = MIN(trans_nsct, nsct);

	return (trans_nsct);
}


/*----------------------------------------------*
 *  ボディの転送パラメタを得る
 *----------------------------------------------*/
static	Sint32	pcmstm_getTrInf_body(PcmHn pcm, PcmStatus *st,
										Sint32 nsct, Uint32 **ppdst)
{
	Sint32		trans_nsct;
	Sint32		usct, vsct;
	Sint32 		write_addr32, write_size, total_size;
	/*  リングバッファの空きバイト数を得る  */
	PCM_MeGetRingWrite(pcm, (Sint8 **)&write_addr32, &write_size, &total_size);

	/*  転送するセクタ数を計算する  */
	*ppdst = (Uint32 *)write_addr32;
	trans_nsct = write_size / PCMSTM_SECT_BSIZE(pcm);

	/*  とりあえず１ｃｈずつ転送する  */
	usct = PCMXLIB_STAT_XBLK_BCH(st) / PCMSTM_SECT_BSIZE(pcm);
	vsct = (PCMXLIB_GET_RING_WOFST(st) % PCMXLIB_STAT_XBLK_BCH(st))
				/ PCMSTM_SECT_BSIZE(pcm);
	trans_nsct = MIN(trans_nsct, usct - vsct);

	/*  転送セクタ制限  */
	trans_nsct = MIN(trans_nsct, nsct);

	return (trans_nsct);
}


/*----------------------------------------------*
 *  DMA 終了検査関数
 *----------------------------------------------*/
static	void	pcmstm_waitDma_sap(PcmHn pcm)
{
#if		0
	if (PCMSTM_DMA_STATE(pcm) == ON) {
		if (PCMSTM_LOAD_FUNC(pcm) == pcmstm_loadDmaCpu_sap) {
			while (PCM_StmDmaCpuResult() == TRUE) ;
			pcmstm_dma_cpu_start = OFF;
		}
		else if (PCMSTM_LOAD_FUNC(pcm) == pcmstm_loadDmaScu_sap) {
			while (PCM_StmDmaScuResult() == TRUE) ;
			pcmstm_dma_scu_start = OFF;
		}
		PCMSTM_DMA_STATE(pcm) = OFF;
	}

#else
	/*  1996.07.08 Y.H  (start)  転送方式設定 I/F 対応  */
	if (PCMSTM_DMA_STATE(pcm) == ON) {
		while ((*pcmstm_trfn_check)() == TRUE);
		pcmstm_dma_cpu_start  = OFF;
		pcmstm_dma_scu_start  = OFF;
		PCMSTM_DMA_STATE(pcm) = OFF;
	}
	/*  1996.07.08 Y.H  ( end )  */

#endif

	return;
}


/*----------------------------------------------*
 *  転送処理本体（完了復帰）
 *----------------------------------------------*/
static	Sint32	pcmstm_loadSap_sub(void *obj, StmHn stm2, Sint32 nsct)
{
	PcmHn		pcm;
	Sint32		trans_nsct;
	Sint32		write_size;
	Sint32		tsz;

	PcmWork		*work;
	PcmPara		*para;
	PcmStatus	*st;
	Uint32		*dst;
	PcmStmTransPara	trpara, *ptrpara;

	pcm		= (PcmHn)obj;
	work 	= *(PcmWork **)pcm;
	para 	= &work->para;
	st 		= &work->status;

	_VTV_PRINTF((VTV_s, "P:_loadDmaScu hn%d\n stm%X sct%d\n", 
		PCM_MeGetHandleNo(pcm), stm2, nsct));

	if (PCM_IsDeath(pcm)) {
		return (0);
	}

	/*  1996.07.26 Y.H  (start)  */
	/*  演奏状態検査  */
	if (st->play < PCM_STAT_PLAY_START) {
		return (0);
	}
	/*  1996.07.26 Y.H  ( end )  */

	/*  終了判定  */
	if (PCMXLIB_STAT_XSTAT(st) == PCMXLIB_XSTAT_STOP) {
		/*  これ以上読み込む必要なし  */
		return (0);
	}

	/*  ○ＤＭＡ終了判定  */
	if (PCMSTM_DMA_STATE(pcm) == ON) {
		if ((*pcmstm_trfn_check)() == TRUE) {
			/*  ◇転送中 */
			return (-1);
		}

		/*  ◇転送終了  */
		PCMSTM_DMA_STATE(pcm) = OFF;
		tsz = PCMSTM_SCT2B(pcm, PCMSTM_DMA_SECT(pcm));
		PCMSTM_WRITE_BSIZE(pcm) += tsz;
		PCMSTM_LOAD_TOTAL_SECT(pcm) += PCMSTM_DMA_SECT(pcm);

		/*  ＠ボディ部	*/
		/*  リングバッファ書き込みポインタ更新  */
		PCM_MeRenewRingWrite(pcm, tsz);

		/*	ＰＣＭバッファ書き込みポインタ更新  */
		if (PCMXLIB_GET_RING_WOFST(st) <= st->pcm_bsize) {
			/*  バッファフルになるまで貯める  */
			write_size = PCM_XCmnRenewPcmWrite1st( pcm, para, st, tsz);
		}

		/*  スタートトリガ処理  */
		if (PCMXLIB_STAT_XLRDONE(st) == 0) {
			if (PCMSTM_WRITE_BSIZE(pcm) >= PCMSTM_BUF_BSIZE(pcm)) {
				if (PCM_XCmnStartTrg(pcm, st, write_size) == TRUE) {
					/*  再生開始  */
					return (PCMSTM_DMA_SECT(pcm));
				}
			}
		}

		if (PCMSTM_WRITE_BSIZE(pcm) >= PCMSTM_BUF_BSIZE(pcm)) {
			/*  チャネル変更  */
			PCM_XCmnChangeParity(st);
		}

		/*  転送セクタ数を通知  */
		return (PCMSTM_DMA_SECT(pcm));
	}

	/*  ○ＤＭＡ転送命令  */
	if (pcmlib_exec_pcm != NULL) {
		if (PCMSTM_LOAD_TOTAL_SECT(pcm) >= PCMSTM_LOAD_LIMIT_SECT(pcm)) {
			/*  プレロード終了  */
			return (0);
		}
	}

	/*  1996.07.08 Y.H  (start)  ヘッダとボディ処理を分離  */
	if (PCMXLIB_STAT_XSTAT(st) <= PCMXLIB_XSTAT_BLKHD) {
		/*  ＠ヘッダ部  */
		/*  ヘッダの転送パラメタを得る  */
		trans_nsct = pcmstm_getTrInf_header(pcm, st, nsct, &dst);
		if (trans_nsct <= 0) {
			return (0);
		}

		/*  コーディング情報(ci)の取得と設定  */
		PCM_GetSetCi(pcm);

		/*  転送  */
		ptrpara = &trpara;
		PCMSTM_TRPARA_DST(ptrpara)	= (void *)dst;
		PCMSTM_TRPARA_SRC(ptrpara)	= (void *)STM_StartTrans(stm2, 
												&pcmstm_trans_adlt);
		PCMSTM_TRPARA_DSIZ(ptrpara) = (Sint32)PCMSTM_SCT2D(pcm, trans_nsct);
		pcmstm_loadCpu_sap(ptrpara, 1);

		/*  転送パラメタ更新  */
		PCMSTM_LOAD_TOTAL_SECT(pcm) = trans_nsct;

		/*  ヘッダ解析  */
		PCMXLIB_STAT_XSTAT(st) = PCMXLIB_XSTAT_HDANA;
		PCM_MeHeaderProcess_sap(pcm);

		/*  転送中  */
		return (trans_nsct);
	}
	else {
		/*  ＠ボディ部  */
		/*  ボディの転送パラメタを得る  */
		trans_nsct = pcmstm_getTrInf_body(pcm, st, nsct, &dst);
		if (trans_nsct <= 0) {
			return (0);
		}

		/*  転送パラメタ設定  */
		PCMSTM_WRITE_ADDR(pcm)  = dst;
		PCMSTM_WRITE_BSIZE(pcm) = 0;
		PCMSTM_BUF_BSIZE(pcm)   = trans_nsct * PCMSTM_SECT_BSIZE(pcm);

		/* コーディング情報(ci)の取得と設定 */
		PCM_GetSetCi(pcm);

		/* 転送  */
		ptrpara = &trpara;
		PCMSTM_TRPARA_DST(ptrpara)	= (void *)dst;
		PCMSTM_TRPARA_SRC(ptrpara)	= (void *)STM_StartTrans(stm2, 
												&pcmstm_trans_adlt);
		PCMSTM_TRPARA_DSIZ(ptrpara) = (Sint32)PCMSTM_SCT2D(pcm, trans_nsct);
		(*pcmstm_trfn_load)(ptrpara, 1);

		PCMSTM_DMA_SECT(pcm)  = trans_nsct;
		PCMSTM_DMA_STATE(pcm) = ON;

		/*  転送中  */
		return (-1);
	}
	/*  1996.07.09 Y.H  ( end )  */
}


/*----------------------------------------------*
 *	転送処理エントリ（完了復帰処理）
 *
 *［備考］
 *	・STM_ExecServer() よりコールされる
 *	・転送は LchRch のバウンダリに合わせる
 *----------------------------------------------*/
Sint32	pcmstm_loadSap(void *obj, StmHn stm2, Sint32 nsct)
{
	PcmHn		pcm;
	PcmWork		*work;
	PcmPara		*para;
	PcmStatus	*st;
	Sint32 		write_addr32, write_size, total_size;
	Sint32		free_sct, free_sct2, free_sct3, onetask_sct;
	Sint32		trans_sct, perch_sct, frac_sct, res_sct, lmt_sct;
	Sint32		wstar_oft, wend_oft;
	Sint32		total_sct, ret;
	Sint32		w_ofst;

	pcm		= (PcmHn)obj;
	work 	= *(PcmWork **)pcm;
	para 	= &work->para;
	st 		= &work->status;

	/*  バッファ残り容量  */
	PCM_MeGetRingWrite(pcm, 
		(Sint8 **)&write_addr32, &write_size, &total_size);

	/*  バッファ空きセクタ数  */
	free_sct = write_size / PCMSTM_SECT_BSIZE(pcm);

	/*  １タスクで処理可能なセクタ数（１ｃｈ分）  */
	onetask_sct = st->onetask_size / PCMSTM_SECT_BSIZE(pcm);

	/*  onetask_sct と ブロック単位を比較して大きいほうをとる  */
	if (PCMXLIB_STAT_XSTAT(st) < PCMXLIB_XSTAT_BLKBDY) {
		perch_sct = 1;
	}
	else {
		perch_sct = PCMXLIB_STAT_XBLK_BCH(st) / PCMSTM_SECT_BSIZE(pcm);
	}
	lmt_sct   = MAX(perch_sct, onetask_sct);

	/*  上限値と比較して小さいほうをとる  */
	free_sct2 = MIN(free_sct, lmt_sct);

	w_ofst    = PCMXLIB_GET_RING_WOFST(st);

	if ((PCM_IS_STEREO(st) != 0) &&
				(PCMXLIB_STAT_XSTAT(st) >= PCMXLIB_XSTAT_BLKBDY)) {
		/*  SAP ファイル	ステレオ 	ボディ			*/

		/*  ＣＤバッファにある容量と比較して小さいほうをとる  */
		/*  SndRam には Lch, Rch　同じサイズだけ入る  */
		free_sct3 = MIN((nsct >> 1), free_sct2);
		if (free_sct3 <= 0) {
			/*  転送できないんだったらすぐに戻る  */
			return (0);
		}

		/*  書き込み終了セクタ位置を整える  */
		wstar_oft = w_ofst / PCMSTM_SECT_BSIZE(pcm);
		wend_oft  = ((wstar_oft + free_sct3) / perch_sct) * perch_sct;

		/*
		 *	<1> L+Rch をまとめて転送する
		 *	<2> 端数が出たら、L+Rch のバウンダリに合わせて転送する
		 */
		if (PCMXLIB_STAT_XPARITY(st) != 0) {
			/*  Rch から転送開始  */
			frac_sct = (w_ofst % PCMXLIB_STAT_XBLK_BCH(st))
							/ PCMSTM_SECT_BSIZE(pcm);
			res_sct  = (perch_sct - frac_sct) % perch_sct;
			if (wend_oft - wstar_oft - res_sct > 0) {
				trans_sct = res_sct + 
					((wend_oft - (wstar_oft + res_sct)) << 1);
			}
			else {
				/*  バウンダリがあわないので次にまとめて送る  */
				return (0);
			}
		}
		else {
			/*  Lch から転送開始		*/
			/*  Lch + Rch 分を転送する	*/
			if (wend_oft - wstar_oft > 0) {
				trans_sct = (wend_oft - wstar_oft) << 1;
			}
			else {
				/*  バウンダリがあわないので次回にまわす  */
				return (0);
			}
		}
	}
	else {
		/*  SAP ファイル	モノラル 	ヘッダ、ボディ	*/
		/*  SAP ファイル	ステレオ	ヘッダ 			*/

		/*  ＣＤバッファにある容量と比較して小さいほうをとる  */
		free_sct3 = MIN(nsct, free_sct2);
		if (free_sct3 <= 0) {
			/*  転送できないんだったらすぐに戻る  */
			return (0);
		}

		/*  1997-06-10  ヘッダ読み込みは別処理とした  */
		if (PCMXLIB_STAT_XSTAT(st) < PCMXLIB_XSTAT_BLKBDY) {
			/*  ヘッダ読み込み  */
			trans_sct = (PCM_SAP_HEADER_SIZE / PCM_SIZE_2K);
		}
		else {
			/*  書き込み終了セクタ位置を整える  */
			wstar_oft = w_ofst / PCMSTM_SECT_BSIZE(pcm);
			wend_oft  = ((wstar_oft + free_sct3) / perch_sct) * perch_sct;
			if (wend_oft > wstar_oft) {
				trans_sct = wend_oft - wstar_oft;
			}
			else {
				/*  端数ではあるが送れる分は送っておく  */
				trans_sct = free_sct3;
			}
		}
	}

	/*  転送する  */
	total_sct = 0;
	while (1) {
		/*  読み込む  */
		while (1) {
			ret = pcmstm_loadSap_sub(obj, stm2, trans_sct);
			if (ret >= 0) {
				/*  転送終了  */
				break;
			}
		}

		if (ret == 0) {
			/*  空き容量がないのでパス  */
			break;
		}

		/*  転送終了   */
		total_sct += ret;
		if (total_sct >= trans_sct) {
			/*  転送終了  */
			break;
		}
	}

	return (trans_sct);
}




/**************************************************************
 *		SAP 版 SetTrMode 関数
 **************************************************************/
void	pcmstm_setTrMode_sap(PcmHn pcm, PcmTrMode mode)
{
 	/*	PCM_SetTrModeCd() よりコールされる  */

	switch(mode) {
		case PCM_TRMODE_CPU:
			pcmstm_trfn_load  = pcmstm_loadCpu_sap;
			pcmstm_trfn_check = pcmstm_checkCpu_sap;
			break;
		case PCM_TRMODE_SDMA:
			pcmstm_trfn_load  = PCM_StmDmaCpuMemCopy4;
			pcmstm_trfn_check = PCM_StmDmaCpuResult;
			break;
		case PCM_TRMODE_SCU:
		default:
 			pcmstm_trfn_load  = PCM_StmDmaScuMemCopy;
 			pcmstm_trfn_check = PCM_StmDmaScuResult;
			break;
	}

	/* 転送関数の設定 */
	PCMSTM_LOAD_FUNC(pcm) = pcmstm_loadSap;
	STM_SetTrFunc(PCMSTM_HANDLE(pcm), PCMSTM_LOAD_FUNC(pcm), pcm);

	return;
}




/********************************************************************
 * ＳＡＰ（Saturn PCM）使用宣言（STM 用）
 *
 *［機能］
 *  Saturn PCM の使用を宣言する。
 *  以後、ライブラリで使用可能な PCM データは、Saturn PCM のみとなる。
 *	AIFF, ADPCM との併用は不可。
 *
 ********************************************************************/
void	PCM_DeclareUseSapStm(void)
{
	/*  SAP 使用許可フラグ  */
	pcm_sap_flag = ON;

	/*  ME 実行関数の登録  */
	PCM_MeEtyFnSap();

	/*  ストリーム関数の登録  */
	PCM_StmEtyFn(&pcmstm_fntbl_sap, pcmstm_waitDma_sap);

	/*  転送先指定  */
	PCM_StmSetDmaDst(PCMSTM_TRDST_SNDR);

	/*  1996.07.24 Y.H  (start)  */
	/*  DMA 初期化  */
	DMA_ScuInit();
	/*  1996.07.24 Y.H  ( end )  */

	return;
}




/********************************************************************
 *	データ転送中のチェック
 ********************************************************************/
/* 戻り値　TRUE:転送中、FALSE:転送中でない */
Bool	PCM_IsTrans(PcmHn pcm)
{
	if (PCMSTM_DMA_STATE(pcm) == ON) {
		return ((*pcmstm_trfn_check)());
	}
	else {
		return (FALSE);
	}
}




/*  end of file  */
