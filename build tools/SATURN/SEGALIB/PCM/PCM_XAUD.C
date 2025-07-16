/******************************************************************************
 *	ソフトウェアライブラリ
 *
 *	Copyright (c) 1994,1995,1996,1997 SEGA
 *
 * Library	:ＰＣＭ・ＡＤＰＣＭ再生ライブラリ
 * Module 	:PCM buffer management, PCM data copy（SAP file）
 * File		:pcm_xaud.c
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
#include "sega_pcm.h"
#include "pcm_msub.h"
#include "pcm_lib.h"
#include "pcm_xlib.h"




/************************************************************************/
/*		□条件コンパイル												*/
/************************************************************************/




/************************************************************************/
/*		□処理マクロ													*/
/************************************************************************/
/*  ＤＭＡパラメタの設定   */
#define ENTRY_DMA_PCM(st, idx, dest, srce, sz)								\
		(																	\
			(st)->copy_tbl[(idx)].dst1 = (Sint8 *)(dest), 					\
			(st)->copy_tbl[(idx)].src  = (Sint8 *)(srce), 					\
			(st)->copy_tbl[(idx)].size = (Sint32)(sz)		 				\
		)




/************************************************************************/
/*		□関数定義														*/
/************************************************************************/
/*--------------------------------------------------------------*
 *		ブロックファイルの処理
 *--------------------------------------------------------------*/
/*  メモリ再生／(ブロック)コピー情報テーブルの設定  */
static	void	pcm_SetBlockDMAtbl(PcmHn hn, Sint32 size_copy)
{
	Sint32	size_write, size_write_total, size_2nd;
	Sint8 	*addr_write1, *addr_write2;

	/*  ＰＣＭ書き込み先アドレス取得  */
	pcm_GetPcmWrite(hn, &addr_write1, &addr_write2, 
						&size_write, &size_write_total);

	if (size_copy <= size_write) {
		/*------------------------------*
		 *	折り返し転送はない
		 *------------------------------*/
		pcm_RenewPcmWrite(hn, size_copy);
	}
	else {
		/*------------------------------*
		 *	折り返し転送あり
		 *------------------------------*/
		/*  ○１回目のロード  */
		pcm_RenewPcmWrite(hn, size_write);

		/*　○２回目のロード  */
		size_2nd = size_copy - size_write;
		pcm_RenewPcmWrite(hn, size_2nd);

	}

	return;
}


/*  転送量を求める  */
static	void	pcm_AudioBlock_size(PcmHn hn, PcmStatus *st, Sint32 *psz)
{
	Sint32		size_write, size_write_total;
	Sint8 		*addr_write1, *addr_write2;
	Sint32		size_mono, 		/* １チャンネル分の在庫 [byte/1ch] 	*/
				size_copy; 		/* コピーするサイズ [byte/1ch] 		*/
	Sint32		w_ofst, r_ofst, flsz, smpl_now;

#if		1
	PCM_MeGetTimeTotal(hn, &smpl_now);
	r_ofst = smpl_now;
#else
	r_ofst = PCMXLIB_GET_RING_ROFST(st);
#endif

	w_ofst = PCMXLIB_GET_RING_WOFST(st);
	flsz   = st->info.file_size;
	if (PCM_IS_STEREO(st) != 0) {
		flsz >>= 1;
	}
	if (w_ofst > flsz) {
		/*  リングバッファには EOF 以降のゴミも供給されている  */
		size_mono = flsz - r_ofst;
	}
	else {
		size_mono = w_ofst - r_ofst;
	}

	/*  再生していないサンプル数と比較する  */
	if (PCM_IS_8BIT_SAMPLING(st) != 0) {
		/*  8 bit  */
		size_mono = MIN( size_mono, 
						st->info.sample_file - st->sample_write_file );
	}
	else {
		/*  16 bit  */
		size_mono = MIN( size_mono, 
					((st->info.sample_file - st->sample_write_file) << 1) );
	}

	/*  全部ロードできるか検査する  */
	/*	  size_write		: 連続書き込み可能サイズ[byte]					*/
	/*	  size_write_total	: 折り返しを含めた書き込み可能サイズ[byte]		*/
	pcm_GetPcmWrite(hn, &addr_write1, &addr_write2, 
						&size_write, &size_write_total);

	/*  読込み保証値と比較する  */
	size_copy = MIN(size_mono, size_write_total);

	/* １回のタスクで処理する量の上限をみる		*/
	size_copy = MIN(size_copy, st->onetask_size);

	/*  戻り値  */
	*psz	= size_copy;

	return;
}


/*--------------------------------------------------------------*
 *	□ブロックファイル処理エントリ
 *
 *［備考］
 *	本関数はデータ読込関数が次の条件でデータを読込むものと仮定
 *	(1) L0 R0 L1 R1 ...             (gfs, imm mode)
 *	(2) L0R0 L1R1 L2 R2 L3R3 ...    (stm)
 *--------------------------------------------------------------*/
void	pcm_AudioBlock(PcmHn hn)
{
	PcmWork		*work 	= *(PcmWork **)hn;
	PcmStatus	*st		= &work->status;
	Sint32		size_copy; 		/*  コピーするサイズ(byte/1ch)  */

	/*
	 *	ＰＣＭバッファがいっぱいになるまで
	 *	GFS/STM 転送関数 内でＰＣＭ書き込みポインタを更新する
	 */
	if (PCMXLIB_GET_RING_WOFST(st) < st->pcm_bsize) {
		return;
	}

	/*  転送完了したか??  */
	if (PCMXLIB_STAT_XLOAD_ON(st) == OFF) {
		return;
	}

	/* １回のタスクでの転送量を求める  */
	pcm_AudioBlock_size(hn, st, &size_copy);
	if (size_copy <= 0) {
		/*  転送するものはない  */
		return;
	}

	if (PCM_IS_STEREO(st) != 0) {
		/*  ◇ステレオ  */
		if (PCMXLIB_STAT_XPARITY(st) == 0) {
			/*  ＰＣＭバッファ書き込み位置更新  */
			pcm_SetBlockDMAtbl(hn, size_copy);

			/*  LRch まとめて読み込む  */
			pcm_RenewRingRead2(hn, size_copy);
			pcm_RenewRingRead(hn, size_copy);
		}
	}
	else {
		/*  ◇モノラル  */
		/*  ＰＣＭバッファ書き込み位置更新  */
		pcm_SetBlockDMAtbl(hn, size_copy);

		/*  リングバッファ読込みアドレス更新  */
		pcm_RenewRingRead(hn, size_copy);
	}

	PCMXLIB_STAT_XLOAD_ON(st) = OFF;	/*  更新終了  */

	return;
}




/*  end of file  */
