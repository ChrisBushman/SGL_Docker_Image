/******************************************************************************
 *	ソフトウェアライブラリ
 *
 *	Copyright (c) 1994,1995,1996,1997 SEGA
 *
 * Library	:ＰＣＭ・ＡＤＰＣＭ再生ライブラリ
 * Module 	:メモリ再生（SAP 関連追加関数）
 * File		:pcm_xmp.c
 * Date		:1997-06-23
 * Version	:1.24
 * Author	:Y.T, Y.H
 *
 *--------------------------------------------------------------------------*
 * Update	:1996-07-05	1.00	Y.H	新規作成
 *			:1996-10-14	1.21	Y.H	SHC Ver. 3.0F対応
 *
 ****************************************************************************/




/************************************************************************/
/*		□ヘッダファイル												*/
/************************************************************************/
#include "sega_pcm.h"
#include "pcm_mem.h"
#include "pcm_msub.h"
#include "pcm_lib.h"
#include "pcm_xlib.h"
#include "pcm_xsap.h"




/************************************************************************/
/*		□変数宣言														*/
/************************************************************************/




/************************************************************************/
/*		□関数宣言														*/
/************************************************************************/
void	PCM_MeStartTimer(PcmHn hn);

void	pcm_MeGetRingWrite_sap(PcmHn hn, 
	Sint8 **ring_write_addr, Sint32 *write_size, Sint32 *total_write_size);
void	pcm_MeRenewRingWrite_sap(PcmHn hn, Sint32 write_size);
void	pcm_MeTask_sap(PcmHn hn);


/************************************************************************/
/*		□外部関数宣言														*/
/************************************************************************/

/************************************************************************/
/*		□関数定義														*/
/************************************************************************/
/*  1996.07.08 Y.H  (start)  */
/*  MP 実行関数テーブル  */
static	PcmMeExecFunc	pcmme_fntbl_sap = {
	pcm_MeGetRingWrite_sap,
	pcm_MeRenewRingWrite_sap,
	pcm_MeTask_sap
};
/*  1996.07.08 Y.H  ( end )  */




/************************************************************************/
/*		□関数定義														*/
/************************************************************************/
/*******************************************************************
【機　能】
	SaturnPCM file のヘッダ解析処理
【引　数】
	hn　	（入力）　ハンドル
【戻り値】
	なし
*******************************************************************/
Sint32	PCM_MeHeaderProcess_sap(PcmHn hn)
{
	PcmWork		*work 	= *(PcmWork **)hn;
	PcmPara		*para 	= &work->para;
	PcmStatus	*st 	= &work->status;
	PcmInfo		*inf	= &st->info;
	PcmSapHeader	*saphead;
	PcmExtended80	*pext;
	Sint32			filesize, samprate;
	Sint32			ch, bits;
	Sint32			unit;

	if (PCMXLIB_STAT_XSTAT(st) == PCMXLIB_XSTAT_HDANA) {
		/*  ヘッダ解析プロセス  */
		PCMXLIB_STAT_XSTAT(st) = PCMXLIB_XSTAT_BLKBDY;
	}
	else {
		/*  ヘッダ解析済み、もしくはエラー  */
		return (-1);
	}

	/*  リングバッファとヘッダは別領域です  */
	saphead = (PcmSapHeader	*)PCMXLIB_STAT_XBLK_ADDR(st);

	/*  (a) ファイルタイプ			*/
	/*  (b) データタイプ			*/
	/*  それぞれこの関数以前に設定されています  */

	/*  (c) ファイルサイズ			*/
	ch   = PCMSAP_HEADER_CHANNEL(saphead);
	bits = PCMSAP_HEADER_BITS(saphead) >> 3;
	if ( ((ch < 1) || (ch > 2)) || ((bits < 1) || (bits > 2)) ) {
		/*  SAP ファイルでない  */
		return (-1);
	}

	filesize = ch * bits * PCMSAP_HEADER_SAMPLE(saphead);
	PCM_INFO_FILE_SIZE(inf)		= filesize;

	/*  (d) チャネル数 				*/
	PCM_INFO_CHANNEL(inf)		= PCMSAP_HEADER_CHANNEL(saphead);

	/*  (e) サンプリングビット数	*/
	PCM_INFO_SAMPLING_BIT(inf)	= PCMSAP_HEADER_BITS(saphead);

	/*  (f) サンプリングレート		*/
	pext = (PcmExtended80 *)PCMSAP_HEADER_FREQP(saphead);
	samprate = (Sint32)pext->man[0] & 0x0000ffff;
	samprate >>= 0x400E - pext->exp[0];
	PCM_INFO_SAMPLING_RATE(inf)	= samprate;

	/*  (g) ファイルの総サンプル数	*/
	PCM_INFO_SAMPLE_FILE(inf)	= PCMSAP_HEADER_SAMPLE(saphead);

	/*  (h) 圧縮タイプ				*/
	PCM_INFO_COMPRESSION_TYPE(inf)	= PCM_TYPE_NONE;

	/*  ブロックユニットサイズを求めておく  */
	unit = PCM_SAP_BLKUNIT_SIZE;
	if (PCM_IS_16BIT_SAMPLING(st) != 0) {
		unit <<= 1;
	}
	PCMXLIB_STAT_XBLK_BCH(st) = unit;
	if (PCM_IS_STEREO(st) != 0) {
		unit <<= 1;
	}
	PCMXLIB_STAT_XBLK_B2CH(st) = unit;

	/*  リングバッファとヘッダは別領域です  */
	st->media_offset = 0;

	/*  オーディオ処理関数ポインタの設定  */
	st->audio_process_fp = pcm_AudioBlock;

	/*  リングバッファポインタを修正  */
	para->ring_addr = para->pcm_addr;
	para->ring_size = para->pcm_size * bits;
	st->ring_write_addr = para->ring_addr;
	st->ring_read_addr  = para->ring_addr;
	st->ring_end_addr   = para->ring_addr + para->ring_size;
	PCMXLIB_STAT_XRING_WADDR(st)	= st->ring_end_addr;
	PCMXLIB_STAT_XRING_RADDR(st)	= st->ring_end_addr;

	/*  PCMバッファサイズ（1chあたり）をバイト数に変換する  */
	st->pcm_bsize = PCM_SAMPLE2BSIZE(st, para->pcm_size);

	/*  PCM第２チャンネルバッファ開始アドレスを設定する  */
	st->pcm2_addr = para->pcm_addr + st->pcm_bsize;		/*  Rch  */

	/* リングバッファ読み取り位置更新 */
	pcm_RenewRingRead(hn, st->media_offset);	/*  Rch  */
	pcm_RenewRingRead2(hn, st->media_offset);	/*  Lch  */

	/* １回のタスクで処理する量の上限[byte/1ch]	*/
	st->onetask_size = PCM_SAMPLE2BSIZE(st, st->onetask_sample);

	return (0);
}


/*----------------------------------------------*
 *	リングバッファ書き込みポインタ取得（sap）
 *----------------------------------------------*/
void	pcm_MeGetRingWrite_sap(PcmHn hn, 
	Sint8 **ring_write_addr, Sint32 *write_size, Sint32 *total_write_size)
{
	PcmWork		*work = *(PcmWork **)hn;
	PcmPara		*para 	= &work->para;
	PcmStatus	*st = &work->status;

	if ((PCM_IS_STEREO(st) !=0) && (PCMXLIB_STAT_XPARITY(st) == 0)) {
		/*  ステレオＬ  */
		*total_write_size = para->ring_size + PCMXLIB_STAT_XBLK_ROFST(st)
								- PCMXLIB_STAT_XBLK_WOFST(st);

		/*  書き込み可能トータルサイズ  */
		if (*total_write_size <= 0) {
			*write_size = 0;
			*ring_write_addr = NULL;
		} 
		else {
			if (PCMXLIB_STAT_XRING_WADDR(st) >= PCMXLIB_STAT_XRING_RADDR(st)) {
				*write_size = (st->ring_end_addr + para->ring_size) -
								PCMXLIB_STAT_XRING_WADDR(st);
			}
			else {
				*write_size = *total_write_size;
			}
			*ring_write_addr = (Sint8 *)PCMXLIB_STAT_XRING_WADDR(st);
		}
	}
	else {
		/*  モノラル、ステレオＲ  */
		/*  書き込み可能トータルサイズ  */
		*total_write_size = para->ring_size + st->ring_read_offset 
							- st->ring_write_offset;

		/* 連続書き込み可能サイズ */
		if (*total_write_size <= 0) {
			*write_size = 0;
			*ring_write_addr = NULL;
		} 
		else {
			if (st->ring_write_addr >= st->ring_read_addr) {
				*write_size = st->ring_end_addr - st->ring_write_addr;
			}
			else {
				*write_size = *total_write_size;
			}
			*ring_write_addr = st->ring_write_addr;
		}
	}

	return;
}


/*----------------------------------------------*
 *	リングバッファ書き込みポインタ更新（sap）
 *----------------------------------------------*/
void	pcm_MeRenewRingWrite_sap(PcmHn hn, Sint32 write_size)
{
	PcmWork		*work 	= *(PcmWork **)hn;
	PcmPara		*para 	= &work->para;
	PcmStatus	*st 	= &work->status;

	if (write_size <= 0) {
		return;
	}

	if ((PCM_IS_STEREO(st) !=0) && (PCMXLIB_STAT_XPARITY(st) == 0)) {
		/*  ステレオＬ  */
		PCMXLIB_STAT_XBLK_WOFST(st)	 += write_size;
		PCMXLIB_STAT_XRING_WADDR(st) += write_size;

		if (PCMXLIB_STAT_XRING_WADDR(st) >= 
				(st->ring_end_addr + para->ring_size)) {
		 	PCMXLIB_STAT_XRING_WADDR(st) -= para->ring_size;
		}
	}
	else {
		/*  モノラル、ステレオＲ  */
		st->ring_write_offset += write_size;
		st->ring_write_addr   += write_size;

		if (st->ring_write_addr >= st->ring_end_addr) {
		 	st->ring_write_addr -= para->ring_size;
		}
	}

	return;
}


/*----------------------------------------------*
 *	Task（sap）
 *----------------------------------------------*/
void	pcm_MeTask_sap(PcmHn hn)
{
	PcmWork		*work 	= *(PcmWork **)hn;
	PcmStatus	*st 	= &work->status;
	Sint32		sample_now;

	/*  タスクコールカウンタのカウントアップ  */
	st->cnt_task_call++;

	/*  再生ステータスチェック  */
	if ( (st->play <= PCM_STAT_PLAY_PAUSE) ||
				(st->play >= PCM_STAT_PLAY_END) ) {
		return;
	}

	/*  エラー状態チェック  */
	if (pcm_err_code != PCM_ERR_OK) {
		PCM_MeStop(hn);
		st->play = PCM_STAT_PLAY_ERR_STOP;
		return;
	}

	/*  ヘッダ処理  */
	if (st->play == PCM_STAT_PLAY_START) {
		/* 解析が終わらないと始まらない  */
		PCM_MeHeaderProcess_sap(hn);

		/*  ボディ読み込み完了待ち       */
		if (PCMXLIB_STAT_XSTAT(st) == PCMXLIB_XSTAT_PLAYGO) {
			st->play = PCM_STAT_PLAY_HEADER;
		}
		return;
	}

#if		0
	/*  再生トリガ待ち  */
	if (PCMXLIB_STAT_XPLAY_ON(st) == OFF) {
		/*  4k sample/ch 再生を待つ  */
		return;
	}
#endif

	/*  再生と終了  */
	if (st->play == PCM_STAT_PLAY_TIME) {
		/*  オーディオ処理  */
		(*st->audio_process_fp)(hn);

		if (PCM_IsRingEmpty(hn)) {
			/*  再生終了処理  */
			PCMXLIB_STAT_XSTAT(st) = PCMXLIB_XSTAT_STOP;
			PCM_MeGetTimeTotal(hn, &sample_now);
			if (sample_now + PCM_HN_STOP_TRG_SAMPLE(hn) > st->sample_write) {
				/* さあ、再生終了だ！ */
				pcm_EndProcess(hn);
			}
		}

	}

	/* タイマスタート・ＰＣＭ再生スタート処理 */
	if (st->play == PCM_STAT_PLAY_HEADER) {
		/* タイマースタート */
		PCM_MeStartTimer(hn);
	}

	return;
}


/*------------------------------------------------------*
 *	SAP 用 MP 実行関数を登録する
 *------------------------------------------------------*/
void	PCM_MeEtyFnSap(void)
{
	PCM_MeEtyFn(&pcmme_fntbl_sap);

	return;
}




/*  end of file  */
