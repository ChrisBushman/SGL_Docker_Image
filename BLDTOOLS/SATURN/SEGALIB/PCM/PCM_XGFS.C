/******************************************************************************
 *	ソフトウェアライブラリ
 *
 *	Copyright (c) 1994,1995,1996,1997 SEGA
 *
 * Library	:ＰＣＭ・ＡＤＰＣＭ再生ライブラリ
 * Module 	:ファイルアクセス（SaurnPCM ファイル）
 * File		:pcm_xgfs.c
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
/*		□ヘッダファイル												*/
/************************************************************************/
#include <sega_xpt.h>
#include "sega_pcm.h"
#include "pcm_gfs.h"
#include "pcm_mem.h"
#include "pcm_lib.h"
#include "pcm_xlib.h"
#include "pcm_xsap.h"
#include "pcm_msub.h"
#include "sega_dma.h"




/************************************************************************/
/*		□外部関数宣言													*/
/************************************************************************/


/************************************************************************/
/*		□関数宣言														*/
/************************************************************************/
Sint32	pcmgfs_loadBuf_sap(PcmHn pcm);
Sint32	pcmgfs_nwCdRead_sap(PcmHn pcm);
void	pcmgfs_task_sap(PcmHn pcm);
Sint32	pcmgfs_preloadFile_sap(PcmHn pcm, Sint32 size);




/************************************************************************/
/*		□関数定義														*/
/************************************************************************/
/*  1996.07.08 Y.H  (start)  */
/*  ファイルシステム関数  */
static	PcmExecFunc	pcmgfs_fntbl_sap = {
	NULL,
	pcmgfs_task_sap,
	pcmgfs_preloadFile_sap,
	NULL,
	NULL
};
/*  1996.07.08 Y.H  ( end )  */




/************************************************************************/
/*		□関数定義														*/
/************************************************************************/
/**************************************************************
 *		SAP 版先読み関数（GFS 用）
 **************************************************************/
Sint32	pcmgfs_nwCdRead_sap(PcmHn pcm)
{
	Sint32	nsct;
	Sint32	ret;

	nsct = PCMGFS_FILE_SECT(pcm) - PCMGFS_LOAD_TOTAL_SECT(pcm);
	if (nsct > 0) {
		ret = GFS_NwCdRead(PCMGFS_HANDLE(pcm), nsct);
		if (ret < 0) {
			PCM_MeSetErrCode(PCM_ERR_GFS_READ);
			return (ret);
		}
    }

	return (0);
}


/**************************************************************
 *		SAP 版読込み関数（GFS 用）
 *------------------------------------------------------------*
 *［戻り値］
 *	0:処理中、1:終了、-1:エラー
 **************************************************************/
Sint32	pcmgfs_loadBuf_sap(PcmHn pcm)
{
	PcmWork		*work 	= *(PcmWork **)pcm;
	PcmPara		*para 	= &work->para;
	PcmStatus	*st 	= &work->status;

	Uint32		write_addr32;
	Sint32		write_size, total_size;
	Sint32		load_sct, load_size;
	Sint32		gfs_ret;
	Sint32		stat, ndata;
	Sint32		delta_size;
	Sint32		usct, vsct;

	if ( (st->play == PCM_STAT_PLAY_TIME) && 
			(PCMXLIB_STAT_XSTAT(st) == PCMXLIB_XSTAT_STOP) ) {
		/*  再生終了判定後にファイルを読み込まない為  */
		return (1);
	}

	/*  ヘッダ読み込み  */
	if ( (PCMXLIB_STAT_XSTAT(st) <= PCMXLIB_XSTAT_BLKHD) &&
			(PCMXLIB_CHK_PRELOAD(st) != TRUE) ) {
		/*  プレロードされている場合は何もしない  */
		if (PCMGFS_EXEC_ONE_STATE(pcm) == FALSE) {
			/*  ヘッダ読み込み命令  */
			gfs_ret = GFS_NwFread(PCMGFS_HANDLE(pcm),
							(PCM_SAP_HEADER_SIZE / PCM_SIZE_2K),
								(void *)PCMXLIB_STAT_XBLK_ADDR(st),
									PCM_SAP_HEADER_SIZE);
			if (gfs_ret < 0) {
				PCM_MeSetErrCode(PCM_ERR_GFS_READ);
				return (-1);
			}
			PCMGFS_EXEC_LOAD_SIZE(pcm) = PCM_SAP_HEADER_SIZE;
			PCMGFS_EXEC_ONE_STATE(pcm) = TRUE;

			/*  ヘッダ読込み指示  */
			PCMXLIB_STAT_XSTAT(st) = PCMXLIB_XSTAT_BLKHD;
		}

		/*  ヘッダ読み込み	*/
		if (PCMGFS_EXEC_ONE_STATE(pcm) == TRUE) {
			/*  読込み処理  */
			gfs_ret = GFS_NwExecOne(PCMGFS_HANDLE(pcm));
			if (gfs_ret < 0) {
				PCM_MeSetErrCode(PCM_ERR_GFS_READ);
				return (-1);
			}

			/*  全部読んだか  */
			GFS_NwGetStat(PCMGFS_HANDLE(pcm), &stat, &ndata);
			if ( (ndata >= PCMGFS_EXEC_LOAD_SIZE(pcm)) ||
					 (gfs_ret == GFS_SVR_COMPLETED) ) {
				/*  ホスト領域への転送が終わった  */
				PCMGFS_EXEC_ONE_STATE(pcm) = FALSE;
				PCMGFS_LOAD_TOTAL_SECT(pcm)	+= 
						(PCM_SAP_HEADER_SIZE / PCM_SIZE_2K);

				/*  ヘッダ解析  */
				PCMXLIB_STAT_XSTAT(st) = PCMXLIB_XSTAT_HDANA;
				PCM_MeHeaderProcess_sap(pcm);
			}
			return (0);
		}
		else {
			/*  まだ処理中  */
			return (0);
		}
	}

	/*  ボディ読込み指示  */
	if (PCMGFS_EXEC_ONE_STATE(pcm) == FALSE) {
		/*  リングバッファの空き容量を得る  */
		PCM_MeGetRingWrite(pcm, 
			(Sint8 **)&write_addr32, &write_size, &total_size);

		/*  空きが１セクタを越える時に読み込む  */
		if (write_size >= PCMLIB_SECT_SIZE) {
			/*  ※ SAP ファイルは１ch 単位で読込む必要があるため、 */
			/*    PCM_SetLoadNum() の設定は意味がなくなる。        */

			/*  読込みセクタ数を求める（バウンダリを合わせる）  */
			load_sct = write_size / PCMLIB_SECT_SIZE;
			usct = PCMXLIB_STAT_XBLK_BCH(st) / PCMLIB_SECT_SIZE;
			vsct = ((PCMXLIB_GET_RING_WOFST(st)) % PCMXLIB_STAT_XBLK_BCH(st))
							/ PCMLIB_SECT_SIZE;
			load_sct = MIN(load_sct, usct - vsct);
			if (load_sct <= 0) {
				return (0);
			}
			load_size = load_sct  * PCMLIB_SECT_SIZE;

			/*  読込み命令  */
			gfs_ret = GFS_NwFread(PCMGFS_HANDLE(pcm), load_sct, 
											(void *)write_addr32, load_size);
			if (gfs_ret < 0) { 
				PCM_MeSetErrCode(PCM_ERR_GFS_READ);
				return (-1);
			}
			PCMGFS_EXEC_LOAD_SIZE(pcm) = load_size;
			PCMGFS_EXEC_ONE_STATE(pcm) = TRUE;
			PCMGFS_NOW_LOAD_SIZE(pcm)  = 0;
		}
	}

	/*  ボディ取り出し操作  */
	if (PCMGFS_EXEC_ONE_STATE(pcm) == TRUE) {
		/*  ＣＤバッファからＰＣＭバッファへの転送処理  */
		gfs_ret = GFS_NwExecOne(PCMGFS_HANDLE(pcm));
		if (gfs_ret < 0) {
			PCM_MeSetErrCode(PCM_ERR_GFS_READ);
			return (-1);
		}

		/*  ホスト領域に転送したバイト数を得る  */
		GFS_NwGetStat(PCMGFS_HANDLE(pcm), &stat, &ndata);
		if (ndata > 0) {
			delta_size = ndata - PCMGFS_NOW_LOAD_SIZE(pcm);
			PCMGFS_NOW_LOAD_SIZE(pcm) = ndata;
			if (delta_size > 0) {
				/*  リングバッファ書き込みポインタ更新  */
				PCM_MeRenewRingWrite(pcm, delta_size);

				/*	ＰＣＭバッファ書き込みポインタ更新  */
				if (PCMXLIB_GET_RING_WOFST(st) <= st->pcm_bsize) {
					/*  バッファフルになるまで貯める  */
					write_size = 
						PCM_XCmnRenewPcmWrite1st(pcm, para, st, delta_size);
				}
			}
		}

		if ( (ndata >= PCMGFS_EXEC_LOAD_SIZE(pcm)) ||
				(gfs_ret == GFS_SVR_COMPLETED) ) {
			/*  ホスト領域への転送が終わった  */
			PCMGFS_EXEC_ONE_STATE(pcm)	= FALSE;
			PCMGFS_LOAD_TOTAL_SECT(pcm)	+= 
				(PCMGFS_EXEC_LOAD_SIZE(pcm) / PCMLIB_SECT_SIZE);

			/*  1st-Read 終了処理  */
			if (PCMXLIB_STAT_XLRDONE(st) == 0) {
				if (PCM_XCmnStartTrg(pcm, st, write_size) == TRUE) {
					/*  再生開始  */
					return (1);
				}
			}

			/*  チャネル変更  */
			PCM_XCmnChangeParity(st);

			return (1);
		}
	}

	return (0);
}


/**************************************************************
 *  	SAP 版 TASK 処理エントリ関数（GFS 用）
 **************************************************************/
void	pcmgfs_task_sap(PcmHn pcm)
{

	if (PCMGFS_CALLED_CDREAD(pcm) == FALSE) {
		/*  ＣＤバッファへの先読み要求  */
		if (pcmgfs_nwCdRead_sap(pcm) != 0) {
			return;
		}
		PCMGFS_CALLED_CDREAD(pcm) = TRUE;
	}

	/*  ＣＤバッファからリングバッファへデータを転送する  */
	pcmgfs_loadBuf_sap(pcm);

	/*  タスク処理  */
	PCM_MeTask(pcm);

	/*  ループ再生制御   */
	if (PCM_GetPlayStatus(pcm) == PCM_STAT_PLAY_END) {
		if (--PCM_HN_CNT_LOOP(pcm) > 0) {
			/*  ハンドルリセット  */
			PCM_GfsReset(pcm);

			/*  先頭から再生開始  */
			PCM_GfsStart(pcm);
		}
	}

	return;
}


/**************************************************************
 *		SAP 版 Preload 関数（GFS 用）
 *------------------------------------------------------------*
 *［入力］
 *	pcm  : pcm ハンドル
 *	size : 転送要求サイズ（サンプル数でない）
 *［戻り値］
 *	読込み量
 **************************************************************/
Sint32	pcmgfs_preloadFile_sap(PcmHn pcm, Sint32 size)
{
	PcmWork		*work 	= *(PcmWork **)pcm;
	PcmStatus	*st 	= &work->status;
	Uint32		write_addr32;
	Sint32		write_size, total_size;
	Sint32		req_sct, hdr_sct, bdy_sct, unit_sct;
	Sint32		start_sct, end_sct;
	Sint32		playbak;

	req_sct = size / PCM_SIZE_2K;
	if (req_sct == 0) {
		/*  １セクタ未満は処理しない  */
		return (0);
	}

	/*  演奏状態に仮設定  */
	playbak   = st->play;
	st->play  = PCM_STAT_PLAY_START;
	start_sct = PCMGFS_LOAD_TOTAL_SECT(pcm);
	pcmlib_exec_pcm = pcm;

	/*  ＣＤバッファへの先読み要求  */
	if (PCMGFS_CALLED_CDREAD(pcm) == FALSE) {
		if (pcmgfs_nwCdRead_sap(pcm) != 0) {
			st->play = playbak;
			pcmlib_exec_pcm = NULL;
			return (0);
		}
		PCMGFS_CALLED_CDREAD(pcm) = TRUE;
	}

	/*  ヘッダサイズ  */
	if (start_sct == 0) {
		/*  ◇ヘッダ読込みは未だ  */
		hdr_sct = PCM_SAP_HEADER_SIZE / PCM_SIZE_2K;

		/*  ヘッダを読み込む  */
		while (1) {
			if (PCMGFS_LOAD_TOTAL_SECT(pcm) == hdr_sct) {
				/*  読込み完了  */
				break;
			}
			if (pcmgfs_loadBuf_sap(pcm) < 0) {
				/*  読込みエラー発生  */
				st->play = playbak;
				pcmlib_exec_pcm = NULL;
				return (0);
			}
		}
	}
	else {
		/*  ◇ヘッダ読込み済み  */
		hdr_sct = 0;
	}

	/*  ボディサイズ  */
	PCM_MeGetRingWrite(pcm, (Sint8 **)&write_addr32, &write_size, &total_size);
	bdy_sct = write_size / PCM_SIZE_2K;
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
		unit_sct = PCMXLIB_STAT_XBLK_B2CH(st) / PCMLIB_SECT_SIZE;
		req_sct = (req_sct / unit_sct) * unit_sct;
		req_sct += hdr_sct;
	}

	/*  ボディを読み込む  */
	end_sct = start_sct + req_sct;
	while (1) {
		if (PCMGFS_LOAD_TOTAL_SECT(pcm) == end_sct) {
			/*  読込み完了  */
			break;
		}
		if (pcmgfs_loadBuf_sap(pcm) < 0) {
			/*  読込みエラー発生  */
			break;
		}
	}

	/*  演奏状態を戻す  */
	st->play = playbak;
	pcmlib_exec_pcm = NULL;

	return ((PCMGFS_LOAD_TOTAL_SECT(pcm) - start_sct) * PCM_SIZE_2K);
}


/********************************************************************
 * ＳＡＰ（Saturn PCM）使用宣言（GFS 用）
 *
 *［機能］
 *  Saturn PCM の使用を宣言する。
 *  以後、ライブラリで使用可能な PCM データは、Saturn PCM のみとなる。
 *	AIFF, ADPCM との併用は不可。
 *
 ********************************************************************/
void	PCM_DeclareUseSapGfs(void)
{
	/*  SAP 使用許可フラグ  */
	pcm_sap_flag = ON;

	/*  ME 実行関数の登録  */
	PCM_MeEtyFnSap();

	/*  ファイルシステム関数の登録  */
	PCM_GfsEtyFn(&pcmgfs_fntbl_sap);

	/*  1996.07.24 Y.H  (start)  */
	/*  DMA 初期化  */
	DMA_ScuInit();
	/*  1996.07.24 Y.H  ( end )  */

	return;
}




/*  end of file  */
