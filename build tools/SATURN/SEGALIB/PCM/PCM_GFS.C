/******************************************************************************
 *	ソフトウェアライブラリ
 *
 *	Copyright (c) 1994,1995,1996,1997 SEGA
 *
 * Library	:ＰＣＭ・ＡＤＰＣＭ再生ライブラリ
 * Module 	:ファイルアクセス
 * File		:pcm_gfs.c
 * Date		:1997-08-25
 * Version	:1.26
 * Author	:Y.T, Y.H
 *
 *--------------------------------------------------------------------------*
 * Update	:1994-10-04	1.00	Y.T	新規作成
 *			:1996-07-05	1.20	Y.H	SaturnPCM file 対応
 *			:1996-10-14	1.21	Y.H	SHC Ver. 3.0F対応
 *			:1997-08-25 1.26	N.T Total Level対応
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

/* #define	PCM_DEBUG 1 */

#ifdef PCM_DEBUG
#include "play.h"
#endif




/************************************************************************/
/*		□関数宣言														*/
/************************************************************************/
static	Sint32	pcmgfs_nwCdRead_aiff(PcmHn pcm);
static	Sint32	pcmgfs_loadBuf_aiff(PcmHn pcm);
static	void	pcmgfs_task_aiff(PcmHn pcm);
static	Sint32	pcmgfs_preloadFile_aiff(PcmHn pcm, Sint32 size);




/************************************************************************/
/*		□変数定義														*/
/************************************************************************/
/*  1996.06.11 Y.H  (start)  */
/*  ファイルシステム関数テーブル  */
static	PcmExecFunc	pcmgfs_exec_fntbl_org = {
	NULL,
	pcmgfs_task_aiff,
	pcmgfs_preloadFile_aiff,
	NULL,
	NULL,
};

static	PcmExecFunc	pcmgfs_exec_fntbl;
/*  1996.06.11 Y.H  ( end )  */




/************************************************************************/
/*		□関数定義														*/
/************************************************************************/
/********************************************************************/
/*	ファイルシステム初期化											*/
/********************************************************************/
static	void	gfsInit(void)
{
	/* 変数の初期化 */

	if (pcm_sap_flag == OFF) {
		/*  ◇ AIFF  */
		/*  GFS function for AIFF  */
		memcpy( (void *)&pcmgfs_exec_fntbl,
				(void *)&pcmgfs_exec_fntbl_org,
					sizeof(PcmExecFunc) );
	}

	return;
}


/********************************************************************/
/*  SAP 使用宣言の取り消し  										*/
/********************************************************************/
/*  1996.07.08 Y.H  (start)  */
void	PCM_UnDeclareUseSapGfs(void)
{
	pcm_sap_flag = OFF;

	gfsInit();

	/*  リングバッファ管理関数の登録  */
	PCM_MeEtyFnAiff();

	return;
}
/*  1996.07.08 Y.H  ( end )  */


/********************************************************************/
/* ハンドルの作成（ファイルシステム）								*/
/* [入力]															*/
/*    para : 作成パラメータ											*/
/*    gfs  : ファイルハンドル										*/
/* [関数値]															*/
/*    ハンドル（作成できない場合は NULL)							*/
/********************************************************************/
PcmHn PCM_CreateGfsHandle(PcmCreatePara *para, GfsHn gfs)
{
	PcmHn   	pcm;
	Sint32		sctsize, nsct, lastsize;

	if (PCM_lib_gfs_init == OFF) {
		gfsInit();
		PCM_lib_gfs_init = ON;
	}

	if (gfs == NULL) {
		PCM_MeSetErrCode(PCM_ERR_ILLEGAL_PARA);
		return NULL;
	}

	/* 作成パラメータのチェック */
	PCMLIB_CHK_CREATE_PARA(para);

	if (pcm_sap_flag != OFF) {
		/*  ◇ SAP  */
		/*  ハンドル作成  */
		pcm =  PCM_GfsCreate(gfs, 
						 (PcmWork *)PCM_PARA_WORK(para), 
						 PCM_PARA_XWORK_ADDR(para), PCM_PARA_XWORK_SIZE(para),
						 PCM_PARA_PCM_ADDR(para), PCM_PARA_PCM_SIZE(para));
	}
	else {
		/*  ◇ AIFF  */
		/*  ハンドル作成  */
		pcm =  PCM_GfsCreate(gfs, 
						 (PcmWork *)PCM_PARA_WORK(para), 
						 PCM_PARA_RING_ADDR(para), PCM_PARA_RING_SIZE(para),
						 PCM_PARA_PCM_ADDR(para), PCM_PARA_PCM_SIZE(para));
	}

	if (pcm != NULL) {
		PCMLIB_FACCESS_TYPE(pcm) = PCMLIB_FACCESS_GFS;

		/* 実行関数の設定 */
		PCMLIB_SET_START_FUNC(pcm, PCM_GfsStart);
		PCMLIB_SET_TASK_FUNC(pcm, PCM_GfsTask);
		PCMLIB_SET_PRELOAD_FILE_FUNC(pcm, PCM_GfsPreloadFile);
		PCMLIB_SET_SET_LOAD_NUM_FUNC(pcm, PCM_GfsSetLoadNum);
		PCMLIB_SET_SET_TRMODE_FUNC(pcm, PCM_GfsSetTrMode);

		/* ローカルデータの初期化 */
		/* ファイルサイズの取得 */
		GFS_GetFileSize(gfs, &sctsize, &nsct, &lastsize);
		PCMGFS_FILE_SECT(pcm) = nsct;		/*  端数入り  */
		PCMGFS_CALLED_CDREAD(pcm) = FALSE;
		PCMGFS_EXEC_ONE_STATE(pcm) = FALSE;
		PCMGFS_LOAD_TOTAL_SECT(pcm) = 0;

		/* ファイルハンドルの設定 */
		PCMGFS_HANDLE(pcm) = gfs;

		/* 最大転送セクタ数の設定 */
		PCMGFS_LOAD_SECT(pcm) = LOAD_SECT_NUM;
		GFS_SetTransPara(PCMGFS_HANDLE(pcm), PCMGFS_LOAD_SECT(pcm));

		/* 転送モードの設定 */
		if (pcm_sap_flag != OFF) {
			/*  SAP  */
			PCMGFS_TR_MODE(pcm) = GFS_TMODE_SCU;
		}
		else {
			/*  AIFF  */
			PCMGFS_TR_MODE(pcm) = GFS_TMODE_SDMA1;
		}
		GFS_SetTmode(PCMGFS_HANDLE(pcm), PCMGFS_TR_MODE(pcm));

	}

	return pcm;
}


/********************************************************************/
/* ハンドルの消去（ファイルシステム）								*/
/* [入力]															*/
/*    pcm  : ハンドル												*/
/********************************************************************/
void PCM_DestroyGfsHandle(PcmHn pcm)
{
	/* ハンドルチェック */
	PCMLIB_CHK_HANDLE(pcm);

	if (PCMLIB_FACCESS_TYPE(pcm) != PCMLIB_FACCESS_GFS) {
		PCM_MeSetErrCode(PCM_ERR_ILL_CREATE_MODE);
	}

	PCM_MeDestroy(pcm);
}


/********************************************************************/
/* ハンドルのリセット												*/
/*																	*/
/* [引き数]															*/
/*    pcm : ハンドル												*/
/* [関数値]															*/
/* 　なし															*/
/********************************************************************/
void PCM_GfsReset(PcmHn pcm)
{
	/*  1996.06.26 Y.H  (start)  */
	/*  
		SAP ファイルの場合、GFS_NwCdRead() でエラー（-16）が
		発生するため。正しく終了させておく。
	*/
	GFS_NwStop(PCMGFS_HANDLE(pcm));
	/*  1996.06.26 Y.H  (start)  */

	GFS_Seek(PCMGFS_HANDLE(pcm), 0, GFS_SEEK_SET);
	PCMGFS_CALLED_CDREAD(pcm) = FALSE;
	PCMGFS_LOAD_TOTAL_SECT(pcm) = 0;
	PCM_MeReset(pcm);

	return;
}


/********************************************************************/
/* ハンドルを生成する												*/
/*   ファイルハンドルは、ファイルシステムにより予め取得すること		*/
/*																	*/
/* [引き数]															*/
/*    gfs 		: ファイルハンドル									*/
/*    work 		: ワークアドレス									*/
/*    buf 		: バッファアドレス（ SAP 作業領域アドレス）			*/
/*    bufsize 	: バッファのバイト数（ SAP 作業領域サイズ）			*/
/*    pcmbuf	:ＰＣＭのウェーブＲＡＭの先頭アドレス				*/
/*    pcmsize	:ＰＣＭのウェーブＲＡＭのバイト数					*/
/* [関数値]															*/
/*    ハンドル														*/
/*    NULL の場合はエラー											*/
/********************************************************************/
PcmHn PCM_GfsCreate(GfsHn gfs, PcmWork *work, 
		void *buf, Sint32 bufsize, void *pcmbuf, Sint32 pcmsize)
{
	PcmPara		para;

	/* リングバッファ */
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
	para.pcm_pan = PCMLIB_PCM_PAN;
	para.pcm_level = PCMLIB_PCM_LEVEL;
	para.pcm_total_level = 0;    /* tany 97/8/17 */

	/* ハンドルの作成 */
	return PCM_MeCreate(work, &para);
}


/********************************************************************/
/* ＣＤバッファからの最大転送セクタ数を設定する						*/
/* [入力]															*/
/*    pcm  : ハンドル												*/
/*    load_sct : 最大転送セクタ数									*/
/********************************************************************/
void PCM_GfsSetLoadNum(PcmHn pcm, Sint32 load_sct)
{
	PCMGFS_LOAD_SECT(pcm) = load_sct;

	/* 転送セクタ数を設定する */
	GFS_SetTransPara(PCMGFS_HANDLE(pcm), PCMGFS_LOAD_SECT(pcm));
}


/********************************************************************/
/* ＣＤバッファへの先読み要求										*/
/*																	*/
/* [引き数]															*/
/*    pcm : ハンドル												*/
/********************************************************************/
static	Sint32	pcmgfs_nwCdRead_aiff(PcmHn pcm)
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


/********************************************************************/
/* 再生を先頭から開始する											*/
/*																	*/
/* [引き数]															*/
/*    pcm : ハンドル												*/
/********************************************************************/
void PCM_GfsStart(PcmHn pcm)
{
	/* ＣＤバッファへの先読み要求 */
	/* pcmgfs_NwCdRead(pcm); */
	/* ここで先読み要求しても、実際には先読みできず、エラーが発生する。 */

	PCM_MeStart(pcm);
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
void PCM_GfsTask(PcmHn pcm)
{
	(PCM_TASK_FUNC(&pcmgfs_exec_fntbl))(pcm);

	return;
}

/*  AIFF 版 Task 関数  */
static	void	pcmgfs_task_aiff(PcmHn pcm)
{
	if (PCMGFS_CALLED_CDREAD(pcm) == FALSE) {
		/*  ＣＤバッファへの先読み要求  */
		if (pcmgfs_nwCdRead_aiff(pcm) != 0) {
			return;
		}
		PCMGFS_CALLED_CDREAD(pcm) = TRUE;
	}

	/*  ＣＤバッファからリングバッファへデータを転送する  */
	pcmgfs_loadBuf_aiff(pcm);

	/* タスク処理 */
	PCM_MeTask(pcm);

	/* ループ再生制御 */
	if (PCM_GetPlayStatus(pcm) == PCM_STAT_PLAY_END) {

		VTV_PRINTF((VTV_s, "P:Play %X Loop %X\n", 
			PCM_STAT_PLAY_END, PCM_HN_CNT_LOOP(pcm)));

		VTV_PRINTF((VTV_s, "P:exec_one_state %X\n", 
			PCMGFS_EXEC_ONE_STATE(pcm)));


		if (--PCM_HN_CNT_LOOP(pcm) > 0) {

			PCM_GfsReset(pcm);

			VTV_PRINTF((VTV_s, "P:GfsReset %X\n", pcm));

			PCM_GfsStart(pcm);

			VTV_PRINTF((VTV_s, "P:GfsStart %X\n", pcm));
		}
	}

	return;
}


/********************************************************************/
/* ＣＤバッファからのリングバッファにデータを転送する				*/
/*																	*/
/* [引き数]															*/
/*    pcm : ハンドル												*/
/* [関数値]															*/
/*    0:転送中、正数:終了、負数：エラー								*/
/********************************************************************/
static	Sint32	pcmgfs_loadBuf_aiff(PcmHn pcm)
{

	Uint32		write_addr32;
	Sint32		write_size, total_size;
	Sint32		load_sct, load_size;
	Sint32		gfs_ret;
	Sint32		stat, ndata;
	Sint32		delta_size;
#ifdef _PCMD
	Sint32 		fread_this_call = 0;
#endif

	if (PCMGFS_LOAD_TOTAL_SECT(pcm) >= PCMGFS_FILE_SECT(pcm)) {
		/* ファイルをすべて読み込んだ */
		return (0);
	}

	if (PCMGFS_EXEC_ONE_STATE(pcm) == FALSE) {
		/* シネパックのバッファの空きバイト数を得る */
		PCM_MeGetRingWrite(pcm, 
			(Sint8 **)&write_addr32, &write_size, &total_size);

		/*  書き込みサイズが１セクタを越える時  */
		if (write_size >= PCMLIB_SECT_SIZE) {
			/* ＣＤバッファからシネパックのバッファへの転送を開始する */
			if (write_size >= PCMGFS_LOAD_SECT(pcm) * PCMLIB_SECT_SIZE) {
				/*  ２０セクタまとめてロードする  */
				load_size = PCMGFS_LOAD_SECT(pcm) * PCMLIB_SECT_SIZE;
			}
			else {
				/*  連続している分ロードする  */
				load_size = write_size;
			}
			load_sct = load_size / PCMLIB_SECT_SIZE;
			load_size = load_sct * PCMLIB_SECT_SIZE;

			_VTV_PRINTF((VTV_s, "P:NwFread %X\n", load_sct));

			gfs_ret = GFS_NwFread(PCMGFS_HANDLE(pcm), load_sct, 
											(void *)write_addr32, load_size);
			if (gfs_ret < 0) { 
				PCM_MeSetErrCode(PCM_ERR_GFS_READ);
				return (-1);
			}

			PCMGFS_EXEC_LOAD_SIZE(pcm) = load_size;
			PCMGFS_EXEC_ONE_STATE(pcm) = TRUE;
			PCMGFS_NOW_LOAD_SIZE(pcm)  = 0;

#ifdef _PCMD
			fread_this_call = 1;
#endif
		}
	}

	if (PCMGFS_EXEC_ONE_STATE(pcm) == TRUE) {

		_VTV_PRINTF((VTV_s, "P:Call NwExecOne\n"));

		/* ＣＤバッファからシネパックへのバッファ転送処理 */
		gfs_ret = GFS_NwExecOne(PCMGFS_HANDLE(pcm));
		if (gfs_ret < 0) {
			PCM_MeSetErrCode(PCM_ERR_GFS_READ);
			return (-1);
		}

		_VTV_PRINTF((VTV_s, "P:Retn NwExecOne %d\n", gfs_ret));

		/* ホスト領域に転送したバイト数を得る */
		GFS_NwGetStat(PCMGFS_HANDLE(pcm), &stat, &ndata);
		if (ndata > 0) {
			delta_size = ndata - PCMGFS_NOW_LOAD_SIZE(pcm);
			PCMGFS_NOW_LOAD_SIZE(pcm) = ndata;
			if (delta_size > 0) {
				PCM_MeRenewRingWrite(pcm, delta_size);
			}
		}

		if (ndata >= PCMGFS_EXEC_LOAD_SIZE(pcm) ||
							 (gfs_ret == GFS_SVR_COMPLETED)) {
			/* ホスト領域への転送が終わった */
			PCMGFS_EXEC_ONE_STATE(pcm)	= FALSE;

			/*
			 *  リングバッファがセクタバウンダリでないと
			 *	バグる（この関数の第１行の終了条件が無視される）
			 */
			PCMGFS_LOAD_TOTAL_SECT(pcm)	+= 
				(PCMGFS_EXEC_LOAD_SIZE(pcm) / PCMLIB_SECT_SIZE);

			return (1);
		}
#ifdef _PCMD
		else {
			Sint32 snum;
			CDC_GetSctNum(0, &snum);/* バッファ区画０のセクタ数を得る */
			if (fread_this_call == 0) {	/* 今回でなく以前のコールで fread した */
				if (PCMGFS_EXEC_LOAD_SIZE(pcm) / 2048 < snum) {
					/* ＣＤバッファにデータがあるのに転送できなかった */
					VTV_PRINTF((VTV_s, "C:Yet? %2X<%2X %X\n",
						ndata / 2048, PCMGFS_EXEC_LOAD_SIZE(pcm) / 2048, 
						snum));
				}
			}
		}
#endif

	}

	return (0);
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
Sint32 PCM_GfsPreloadFile(PcmHn pcm, Sint32 size)
{
	return ( (PCM_PRELOAD_FILE_FUNC(&pcmgfs_exec_fntbl))(pcm, size) );
}

/*  AIFF 版 Preload 関数  */
static	Sint32	pcmgfs_preloadFile_aiff(PcmHn pcm, Sint32 size)
{

	GfsHn		gfs;
	Uint32		write_addr32;
	Sint32		write_size, total_size;
	Sint32		load_sct, load_size;

	/*  リングバッファサイズと比較する  */
	PCM_MeGetRingWrite(pcm, (Sint8 **)&write_addr32, &write_size, &total_size);
	if (size > write_size) {
		/*  リングバッファサイズを越える  */
		load_size  = write_size;
	}
	else {
		load_size  = size;
	}
	load_sct  = load_size / PCMLIB_SECT_SIZE;
	load_size = load_sct * PCMLIB_SECT_SIZE;

	/*  ＣＤバッファからの転送量を決める  */
	gfs = PCMGFS_HANDLE(pcm);
	GFS_SetTransPara(gfs, load_sct);

	/*  本体を読み込む  */
	PCMGFS_LOAD_TOTAL_SECT(pcm) = 0;
	if ( (load_size >= PCMLIB_SECT_SIZE) &&
			(load_sct > 0) ) {
		/* ＣＤバッファからリングバッファへの転送する */
		load_size = GFS_Fread(gfs, load_sct, 
									(void *)write_addr32, load_size);
		if (load_size < 0) { 
			PCM_MeSetErrCode(PCM_ERR_GFS_READ);
			return (load_size);
		}

		/*  リングバッフ書き込みポインタを更新する  */
		PCM_MeRenewRingWrite(pcm, load_size);
		PCMGFS_LOAD_TOTAL_SECT(pcm) += load_sct;
	}
	else {
		load_size = 0;
	}

	return (load_size);
}


/********************************************************************/
/* データの転送方式の設定											*/
/* [入力]															*/
/*    pcm   : ムービハンドル										*/
/*    mode  : 転送方式												*/
/*      PCM_TRMODE_CPU : ソフトウェア転送							*/
/*      PCM_TRMODE_SDMA : ＤＭＡサイクルスチール					*/
/*      PCM_TRMODE_SCU  : ＳＣＵのＤＭＡ							*/
/********************************************************************/
void PCM_GfsSetTrMode(PcmHn pcm, PcmTrMode mode)
{
	Sint32	tmode;

	switch(mode) {
	case PCM_TRMODE_CPU:
	default:
		tmode = GFS_TMODE_CPU;
		break;
	case PCM_TRMODE_SDMA:
		tmode = GFS_TMODE_SDMA1;
		break;
	case PCM_TRMODE_SCU:
		tmode = GFS_TMODE_SCU;
		break;
	}

	/* 転送モードの設定 */
	PCMGFS_TR_MODE(pcm) = tmode;
	GFS_SetTmode(PCMGFS_HANDLE(pcm), PCMGFS_TR_MODE(pcm));

	return;
}


/*------------------------------------------------------*
 *	AIFF / SAP 処理関数のポインタを登録する
 *------------------------------------------------------*/
/*  ファイルシステム関数の登録  */
void PCM_GfsEtyFn(PcmExecFunc *fnlst)
{
	pcmgfs_exec_fntbl = *fnlst;

	return;
}




/*  end of file  */
