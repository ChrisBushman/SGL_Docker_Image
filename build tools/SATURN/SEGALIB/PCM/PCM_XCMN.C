/******************************************************************************
 *	ソフトウェアライブラリ
 *
 *	Copyright (c) 1994,1995,1996,1997 SEGA
 *
 * Library	:ＰＣＭ・ＡＤＰＣＭ再生ライブラリ
 * Module 	:GFS/STM 共通関数
 * File		:pcm_xcmn.c
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
#include <machine.h>
#include <sega_xpt.h>

#include "sega_pcm.h"
#include "pcm_stm.h"
#include "pcm_mem.h"
#include "pcm_msub.h"
#include "pcm_lib.h"
#include "pcm_xlib.h"
#include "pcm_xsap.h"




/************************************************************************/
/*		□関数定義														*/
/************************************************************************/
/*----------------------------------------------*
 *  スタートトリガ処理
 *----------------------------------------------*/
Bool	PCM_XCmnStartTrg(PcmHn pcm, PcmStatus *st, Sint32 ring_wsz)
{
	Sint32	w_ofst;

	w_ofst = PCMXLIB_GET_RING_WOFST(st);

	if ( ((pcmlib_exec_pcm != NULL) && (ring_wsz == 0)) ||
		 	((pcmlib_exec_pcm == NULL) &&
				(st->sample_write >= PCM_HN_START_TRG_SAMPLE(pcm)) &&
					(w_ofst >= PCM_HN_START_TRG_SIZE(pcm)) &&
						((w_ofst % PCMXLIB_STAT_XBLK_BCH(st)) == 0)) ) {

		if ( (PCM_IS_STEREO(st) == 0) ||
			((PCM_IS_STEREO(st)!=0) && (PCMXLIB_STAT_XPARITY(st)!=0)) ) {
			/*  転送完了フラグを立てる  */
			PCMXLIB_STAT_XSTAT(st)    = PCMXLIB_XSTAT_PLAYGO;
			PCMXLIB_STAT_XLRDONE(st)  = 1;

			/*  パリティ変換  */
			PCM_XCmnChangeParity(st);

			return (TRUE);
		}
	}

	return (FALSE);
}


/*----------------------------------------------*
 *	ＰＣＭバッファがいっぱいになるまで
 *	STM 転送関数 内でＰＣＭ書き込みポインタを更新する
 *----------------------------------------------*/
Sint32	PCM_XCmnRenewPcmWrite1st(PcmHn pcm, PcmPara *para, PcmStatus *st, 
	Sint32 size)
{
	Sint32	write_addr32, write_size, total_size;

	/*  ＰＣＭバッファ書き込みポインタ更新  */
	if ( (PCM_IS_STEREO(st) == 0) || (PCMXLIB_STAT_XPARITY(st) != 0) ) {
		pcm_RenewPcmWrite(pcm, size);
	}

	PCM_MeGetRingWrite(pcm, (Sint8 **)&write_addr32, &write_size, &total_size);

	return (write_size);
}


/*----------------------------------------------*
 *  パリティ変換（ＬＲ）
 *----------------------------------------------*/
void	PCM_XCmnChangeParity(PcmStatus *st)
{
	PCMXLIB_STAT_XLOAD_ON(st) = ON;		/*  load PCM data  */

	if ( (PCMXLIB_GET_RING_WOFST(st) % PCMXLIB_STAT_XBLK_BCH(st)) == 0 ) {
		/*  片チャネルの転送は終わった  */
		PCMXLIB_STAT_XPARITY(st) = 1 - PCMXLIB_STAT_XPARITY(st);
	}

	return;
}




/*  end of file  */
