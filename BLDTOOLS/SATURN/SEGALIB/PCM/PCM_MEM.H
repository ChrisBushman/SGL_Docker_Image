/******************************************************************************
 *	ソフトウェアライブラリ
 *
 *	Copyright (c) 1994,1995,1996,1997 SEGA
 *
 * Library	:ＰＣＭ・ＡＤＰＣＭ再生ライブラリ
 * Module 	:メモリ再生
 * File		:pcm_mem.h
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


#if !defined(PCM_MEM_H)
#define PCM_MEM_H


/************************************************************************/
/*		□ヘッダファイル												*/
/************************************************************************/
#ifdef _PCMD
	/* Debug of sega_pcm */
	#include "sega_vtv.h"
#else
	#define VTV_PRINTF(a)
	#define VTV_Printf(a)
#endif
	#define _VTV_PRINTF(a)
	#define _VTV_Printf(a)




/************************************************************************/
/*		□定数マクロ													*/
/************************************************************************/
/* 本当は static 宣言する関数であるがデバッグを容易にするために。 */
#define STATIC		static




/************************************************************************/
/*		□処理マクロ													*/
/************************************************************************/
/*  リングバッファ管理関数  */
#define	PCMME_GET_RINGWR_FN(fn)		((fn)->pcm_get_ringwrite)
#define	PCMME_RENEW_RINGWR_FN(fn)	((fn)->pcm_renew_ringwrite)
#define	PCMME_TASK_FN(fn)			((fn)->pcm_task)




/************************************************************************/
/*		□データ型宣言													*/
/************************************************************************/
/*  MP 実行関数  */
typedef struct {
	void	(*pcm_get_ringwrite)(PcmHn hn, 
				Sint8 **r_wad, Sint32 *wr_size, Sint32 *total_wr_size);
	void	(*pcm_renew_ringwrite)(PcmHn hn, Sint32 wr_size);
	void	(*pcm_task)(PcmHn hn);
} PcmMeExecFunc;




/************************************************************************/
/*		□変数宣言														*/
/************************************************************************/

/* pcm_mp.c */
extern	PcmErrCode pcm_err_code;	/* エラーコード 		*/
extern	Bool	pcm_sap_flag;		/*  SAP 使用許可フラグ  */




/************************************************************************/
/*		□関数宣言														*/
/************************************************************************/

/* pcm_mp.c */
void PCM_MeSetErrCode(PcmErrCode err);
PcmErrCode PCM_MeGetErrCode(void);
void PCM_MeSetErrFunc(PcmErrFunc func, void *obj);
void PCM_MeSet1TaskSample(PcmHn hn, Sint32 sample);
void PCM_MeSetStartTrgSize(PcmHn pcm, Sint32 size);
void PCM_MeSetStartTrgSample(PcmHn hn, Sint32 sample);
void PCM_MeSetStopTrgSample(PcmHn hn, Sint32 sample);
void PCM_MeSetPauseWork(Sint32 *addr, Sint32 size);
void PCM_MeSetTrModePcm(PcmHn hn, PcmTrMode mode);
void PCM_MeInit(void);
void PCM_MeFinish(void);
void PCM_MeReset(PcmHn hn);
PcmHn PCM_MeCreate(PcmWork *work, PcmPara *para);
void PCM_MeDestroy(PcmHn hn);
void PCM_MeSetInfo(PcmHn hn, PcmInfo *info);
Sint32 PCM_MeGetRingSize(PcmHn hn);
void PCM_MeGetRingWrite(PcmHn hn, Sint8 **ring_write_addr, Sint32 *write_size, Sint32 *total_write_size);
void PCM_MeRenewRingWrite(PcmHn hn, Sint32 SizeWrite);
PcmPara *PCM_MeGetPara(PcmHn hn);
PcmStatus *PCM_MeGetStatus(PcmHn hn);
void PCM_MeStart(PcmHn hn);
void PCM_MeStop(PcmHn hn);
void PCM_MePause(PcmHn hn, PcmPauseCmd cmd);
void PCM_MeGetTime(PcmHn hn, Sint32 *sample_now);
void PCM_MeGetTimeTotal(PcmHn hn, Sint32 *sample_now);
void PCM_MeSetVolume(PcmHn hn, Uint32 level);
void PCM_MeSetPan(PcmHn hn, Uint32 pan);
void PCM_MeEntryNext(PcmHn hn);
PcmHn PCM_MeGetEntryNext(void);
PcmChangeStatus PCM_MeCheckChange(void);
void PCM_MeChange(PcmHn hn_e);
PcmHn PCM_MeGetHandle(Uint32 i);
Sint32 PCM_MeGetHandleNo(PcmHn hn);
Sint32 PCM_IsRingEmpty(PcmHn hn);
void PCM_MeTask(PcmHn hn);
void PCM_MeVblIn(void);
Bool PCM_MeIsCreate(PcmHn pcm);

void PCM_MeEtyFn(PcmMeExecFunc *pfntbl);
void PCM_MeEtyFnAiff(void);


#endif	/*  end of PCM_MEM_H  */


/*  end of file  */
