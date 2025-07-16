/******************************************************************************
 *	ソフトウェアライブラリ
 *
 *	Copyright (c) 1994,1995,1996,1997 SEGA
 *
 * Library	:ＰＣＭ・ＡＤＰＣＭ再生ライブラリ
 * Module 	:ファイルアクセス
 * File		:pcm_gfs.h
 * Date		:1997-06-23
 * Version	:1.24
 * Author	:Y.T, Y.H
 *
 *--------------------------------------------------------------------------*
 * Update	:1994-10-04	1.00	Y.T	新規作成
 *			:1996-07-05	1.20	Y.H	SaturnPCM file 対応
 *			:1996-10-14	1.21	Y.H	SHC Ver. 3.0F対応
 *
 ****************************************************************************/


#if !defined(PCM_GFS_H)
#define PCM_GFS_H


/************************************************************************/
/*		□ヘッダファイル												*/
/************************************************************************/
#include <sega_gfs.h>
#include <sega_pcm.h>




/************************************************************************/
/*		□定数マクロ													*/
/************************************************************************/
/* １回のタスクで転送する最大セクタ数 */
#define		LOAD_SECT_NUM		20

/* ExecOne のリトライ回数 */
#define		RETRY_CNT		10




/************************************************************************/
/*		□処理マクロ													*/
/************************************************************************/
#ifdef PCM_DEBUG
#else
#define 	PL_DEBUG_PRINTNUM(a, b, c)
#define		PL_DEBUG_PRINT(a, b, c)
#define		PL_DEBUG_START_TIME()
#define		PL_DEBUG_PRINT_TIME(str)
#endif

/* ＧＦＳのローカルデータ */
#define	PCMGFS_FILE_PARA(pcm) \
			(PCMLIB_FILEPARA(pcm).data.gfs)

/* ストリームハンドル */
#define	PCMGFS_HANDLE(pcm) \
			(PCMGFS_FILE_PARA(pcm).gfs)

/* 最大転送セクタ数 */
#define	PCMGFS_LOAD_SECT(pcm) \
			(PCMGFS_FILE_PARA(pcm).load_sect)

/* GFS_NwCdRead をコールしたら TREU */
#define PCMGFS_CALLED_CDREAD(pcm) \
			(PCMGFS_FILE_PARA(pcm).called_cdread)

/* ExecOne 実行中なら TREU */
#define PCMGFS_EXEC_ONE_STATE(pcm) \
			(PCMGFS_FILE_PARA(pcm).exec_one_state)

/* ExecOne で読み込むバイト数 */
#define	PCMGFS_EXEC_LOAD_SIZE(pcm)	\
			(PCMGFS_FILE_PARA(pcm).exec_load_size)

/* ExecOne で読み込んでいるバイト数 */
#define	PCMGFS_NOW_LOAD_SIZE(pcm)	\
			(PCMGFS_FILE_PARA(pcm).now_load_size)

/* 読み込み総セクタ数 */
#define	PCMGFS_LOAD_TOTAL_SECT(pcm) \
			(PCMGFS_FILE_PARA(pcm).load_total_sect)

/* ファイルセクタ数 */
#define	PCMGFS_FILE_SECT(pcm) \
			(PCMGFS_FILE_PARA(pcm).file_sect)

/* 転送モード */
#define	PCMGFS_TR_MODE(pcm) \
			(PCMGFS_FILE_PARA(pcm).tr_mode)




/************************************************************************/
/*		□関数宣言														*/
/************************************************************************/

/*** pcm_gfs.c ***/

/* ハンドルのリセット */
void PCM_GfsReset(PcmHn pcm);

/* ムービの生成 */
PcmHn PCM_GfsCreate(GfsHn gfs, PcmWork *work, 
	void *buf, Sint32 bufsize, void *pcmbuf, Sint32 pcmsize);

/* ムービの再生開始 */
void PCM_GfsStart(PcmHn pcm);

/* ムービの再生タスク */
void PCM_GfsTask(PcmHn pcm);

/* ムービをメモリに読み込む */
Sint32 PCM_GfsPreloadFile(PcmHn pcm, Sint32 size);

/* 転送セクタ数の設定 */
void PCM_GfsSetLoadNum(PcmHn pcm, Sint32 load_sct);

/* データの転送方式の設定 */
void PCM_GfsSetTrMode(PcmHn pcm, PcmTrMode mode);

/*  ファイルシステム関数の登録  */
void PCM_GfsEtyFn(PcmExecFunc *fnlst);
void PCM_GfsEtySapFn(void);

#endif		/*  end of  PCM_GFS_H  */


/*  end of file  */
