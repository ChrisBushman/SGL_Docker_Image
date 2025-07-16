/******************************************************************************
 *	ソフトウェアライブラリ
 *
 *	Copyright (c) 1994,1995,1996,1997 SEGA
 *
 * Library	:ＰＣＭ・ＡＤＰＣＭ再生ライブラリ
 * Module 	:ライブラリインタフェース
 * File		:pcm_xlib.h
 * Date		:1997-06-23
 * Version	:1.24
 * Author	:Y.H
 *
 *--------------------------------------------------------------------------*
 * Update	:1996-07-05	1.00	Y.H	新規作成
 *			:1996-10-14	1.21	Y.H	SHC Ver. 3.0F対応
 *
 ****************************************************************************/


#if !defined(PCM_XLIB_H)
#define PCM_XLIB_H


/************************************************************************/
/*		□ヘッダファイル												*/
/************************************************************************/




/************************************************************************/
/*		□定数マクロ													*/
/************************************************************************/




/************************************************************************/
/*		□列挙定数														*/
/************************************************************************/
/*  拡張ステータス  */
typedef enum {
	PCMXLIB_XSTAT_INIT		= 0,	/*  初期状態					*/
	PCMXLIB_XSTAT_BLKHD		= 1,	/*  ブロックヘッダロード中		*/
	PCMXLIB_XSTAT_HDANA		= 2,	/*  ブロックヘッダ解析中		*/
	PCMXLIB_XSTAT_BLKBDY	= 3,	/*  ブロックボディロード中		*/
	PCMXLIB_XSTAT_PLAYGO	= 4,	/*  演奏ＯＫ					*/
	PCMXLIB_XSTAT_STOP		= 5		/*  演奏終了					*/
} PcmXStat;




/************************************************************************/
/*		□処理マクロ													*/
/************************************************************************/
/*  プレロードされたか検査する  */
#define	PCMXLIB_CHK_PRELOAD(st)	\
	( ((st)->ring_write_offset != 0) ? TRUE : FALSE )

/*  ring_write_offset を取得する  */
#define	PCMXLIB_GET_RING_WOFST(st)	\
	( ((PCM_IS_STEREO(st) !=0) && (PCMXLIB_STAT_XPARITY(st) == 0)) \
		? PCMXLIB_STAT_XBLK_WOFST(st) : st->ring_write_offset )

/*  ring_read_offset を取得する  */
#define	PCMXLIB_GET_RING_ROFST(st)	\
	( ((PCM_IS_STEREO(st) !=0) && (PCMXLIB_STAT_XPARITY(st) == 0)) \
		? PCMXLIB_STAT_XBLK_ROFST(st) : st->ring_read_offset )

/*  ステータス拡張メンバのアクセス  */
#define PCMXLIB_STAT_XBLK_ADDR(st)		((st)->x_blk_addr)
#define PCMXLIB_STAT_XBLK_SIZE(st)		((st)->x_blk_size)
#define PCMXLIB_STAT_XBLK_FLAG(st)		((st)->x_blk_flag)
#define PCMXLIB_STAT_XSTAT(st)			((st)->x_stat)
#define PCMXLIB_STAT_XPARITY(st)		((st)->x_parity)
#define	PCMXLIB_STAT_XLRDONE(st)		((st)->x_lrdone)
#define	PCMXLIB_STAT_XBLK_BCH(st)		((st)->x_blk_bch)
#define	PCMXLIB_STAT_XBLK_B2CH(st)		((st)->x_blk_b2ch)
#define	PCMXLIB_STAT_XBLK_ROFST(st)		((st)->x_blk_rofst)
#define	PCMXLIB_STAT_XBLK_WOFST(st)		((st)->x_blk_wofst)
#define	PCMXLIB_STAT_XPLAY_ON(st)		((st)->x_play)
#define	PCMXLIB_STAT_XLOAD_ON(st)		((st)->x_load)
#define	PCMXLIB_STAT_XRING_WADDR(st)	((st)->x_ring_wraddr)
#define	PCMXLIB_STAT_XRING_RADDR(st)	((st)->x_ring_rdaddr)




/************************************************************************/
/*		□関数宣言														*/
/************************************************************************/
/*	pcm_xmp.c  */
Sint32	PCM_MeHeaderProcess_sap(PcmHn hn);
void	PCM_MeEtyFnSap(void);

/*  pcm_xcmn.c  */
void	PCM_XCmnChangeParity(PcmStatus *st);
Sint32	PCM_XCmnRenewPcmWrite1st(PcmHn pcm, PcmPara *para, PcmStatus *st, 
	Sint32 size);
Bool	PCM_XCmnStartTrg(PcmHn pcm, PcmStatus *st, Sint32 ring_wsz);


#endif		/*  end of PCM_XLIB_H  */




/* end of file  */
