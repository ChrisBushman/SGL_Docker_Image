/******************************************************************************
 *	ソフトウェアライブラリ
 *
 *	Copyright (c) 1994,1995,1996,1997 SEGA
 *
 * Library	:ＰＣＭ・ＡＤＰＣＭ再生ライブラリ
 * Module 	:ストリームアクセス
 * File		:pcm_stm.h
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


#if !defined(PCM_STM_H)
#define PCM_STM_H


/************************************************************************/
/*		□ヘッダファイル												*/
/************************************************************************/
#include <sega_stm.h>
#include "sega_pcm.h"




/************************************************************************/
/*		□定数マクロ													*/
/************************************************************************/
/* １回のタスクで転送する最大セクタ数 */
#define		LOAD_SECT_NUM		20

/* １セクタのバイト数 */
#define		SECT_SIZE_2048	2048
#define		SECT_SIZE_2324	2324




/************************************************************************/
/*		□列挙定数														*/
/************************************************************************/
/* データ転送モード */
typedef enum {
		PCM_STM_LOAD_CPU,
		PCM_STM_LOAD_DMACPU,
		PCM_STM_LOAD_DMASCU
} PcmStmLoadMode;

/*  STM 転送先  */
typedef enum {
	PCMSTM_TRDST_WRH = 0,	/*  WORKRAM High  */
	PCMSTM_TRDST_SNDR		/*  SOUND RAM     */
} PcmStmTrDst;

/*  STM 転送関数タイプ  */
typedef enum {
	PCMSTM_TRFN_CMPL = 0,	/*  完了復帰  */
	PCMSTM_TRFN_IMM			/*  即時復帰  */
} PcmStmTrFuncType;




/************************************************************************/
/*		□処理マクロ													*/
/************************************************************************/
#ifdef PCM_DEBUG
#define		DEBUG_PRINT_CDBUF(num) \
	{ \
	char buf[128];\
	sprintf(buf, "cdbuf %03d", num);\
	PLDSP_String(buf, 100, 160);\
	}
#else

#define		DEBUG_PRINT_CDBUF(num)
#define 	PL_DEBUG_PRINTNUM(a, b, c)
#define		PL_DEBUG_PRINT(a, b, c)
#define		PL_DEBUG_START_TIME()
#define		PL_DEBUG_PRINT_TIME(str)
#endif

/* セクタ数(及びセクタサイズ)から、Byte数に換算する */
#define PCMSTM_SCT2B(pcm, nsct)	\
			(PCMSTM_SECT_BSIZE(pcm) * (nsct))

/* セクタ数(及びセクタサイズ)から、Dword数に換算する */
#define PCMSTM_SCT2D(pcm, nsct)	\
			((PCMSTM_SECT_BSIZE(pcm) >> 2) * (nsct))

/* ＳＴＭのローカルデータ */
#define	PCMSTM_FILE_PARA(pcm) \
			(PCMLIB_FILEPARA(pcm).data.stm)

/* ストリームハンドル */
#define	PCMSTM_HANDLE(pcm) \
			(PCMSTM_FILE_PARA(pcm).stm)

/* 転送セクタ数 */
#define	PCMSTM_LOAD_SECT(pcm) \
			(PCMSTM_FILE_PARA(pcm).load_sect)

/* 前回のＣＤバッファの数 */
#define PCMSTM_OLD_CD_BUFNUM(pcm) \
			(PCMSTM_FILE_PARA(pcm).old_cd_buf_num)

/* ＤＭＡ転送しているなら TRUE */
#define PCMSTM_DMA_STATE(pcm) \
			(PCMSTM_FILE_PARA(pcm).dma_state)

/* ＤＭＡ転送するセクタ数 */
#define PCMSTM_DMA_SECT(pcm) \
			(PCMSTM_FILE_PARA(pcm).dma_sect)

/* 書き込むアドレス */
#define	PCMSTM_WRITE_ADDR(pcm) \
			(PCMSTM_FILE_PARA(pcm).write_addr)

/* 空きバッファのバイト数 */
#define	PCMSTM_BUF_BSIZE(pcm) \
			(PCMSTM_FILE_PARA(pcm).buf_bsize)

/* 転送したバイト数 */
#define	PCMSTM_WRITE_BSIZE(pcm) \
			(PCMSTM_FILE_PARA(pcm).write_bsize)

/* １セクタのバイト数 */
#define	PCMSTM_SECT_BSIZE(pcm) \
			(PCMSTM_FILE_PARA(pcm).sect_bsize)

/* 先頭オーディオセクタ */
#define	PCMSTM_AUDIO_1ST_SECT(pcm) \
			(PCMSTM_FILE_PARA(pcm).audio_1st_sect)

/* 転送関数 */
#define	PCMSTM_LOAD_FUNC(pcm) \
			(PCMSTM_FILE_PARA(pcm).load_func)

/* 読み込み総セクタ数 */
#define	PCMSTM_LOAD_TOTAL_SECT(pcm) \
			(PCMSTM_FILE_PARA(pcm).load_total_sect)

/* 読込み上限 */
#define	PCMSTM_LOAD_LIMIT_SECT(pcm) \
			(PCMSTM_FILE_PARA(pcm).load_limit_sect)


/*  転送パラメータアクセスマクロ  */
#define	PCMSTM_TRPARA_DST(para)		((para)->dst)
#define	PCMSTM_TRPARA_SRC(para)		((para)->src)
#define	PCMSTM_TRPARA_DSIZ(para)	((para)->dsiz)




/************************************************************************/
/*		□データ型宣言													*/
/************************************************************************/
typedef Sint32 (*LoadFunc)(void *obj, StmHn stm, Sint32 nsct);


/*  転送パラメータ構造体  */
typedef struct {
	void	*dst;		/*  転送先アドレス         */
	void	*src;		/*  転送元アドレス         */
	Sint32	dsiz;		/*  転送サイズ(dword単位)  */
} PcmStmTransPara;




/************************************************************************/
/*		□変数宣言														*/
/************************************************************************/
extern	Bool	pcmstm_dma_cpu_start;
extern	Bool	pcmstm_dma_scu_start;




/************************************************************************/
/*		□関数宣言														*/
/************************************************************************/
/* ムービの生成 */
PcmHn PCM_StmCreate(StmHn stm, PcmWork *work , 
		void *buf, Sint32 bufsize, void *pcmbuf, Sint32 pcmsize);

/* ムービの再生開始 */
void PCM_StmStart(PcmHn pcm);

/* ムービの再生タスク */
void PCM_StmTask(PcmHn pcm);

/* ムービをメモリに読み込む */
Sint32 PCM_StmPreloadFile(PcmHn pcm, Sint32 size);

/* 転送セクタ数の設定 */
void PCM_StmSetLoadNum(PcmHn pcm, Sint32 load_sct);

/* 転送方式の設定 */
void PCM_StmSetTrMode(PcmHn pcm, PcmTrMode mode);

/* プレロード本体 */
Sint32 PCM_StmPreloadFile_main(PcmHn pcm, Sint32 size);


/* pcm_stm.c, pcm_xstm.c 共用関数  */
void	PCM_GetSetCi(PcmHn pcm);
Sint32	PCM_StmDmaCpuMemCopy4(PcmStmTransPara *ptrpara, Sint32 elm);
Bool	PCM_StmDmaCpuResult(void);
Sint32	PCM_StmDmaScuMemCopy(PcmStmTransPara *ptrpara, Sint32 elm);
Bool	PCM_StmDmaScuResult(void);
void	PCM_StmSetDmaDst(Sint32 dst_flag);


/*  ストリーム関数の登録  */
void PCM_StmEtyFn(PcmExecFunc *fnlst, void (*fn)(PcmHn pcm));
void PCM_StmEtySapFn(void);

/*  ストリーム転送関数設定  */
Sint32	PCM_SetStmTrFuncSap( PcmHn 	pcm, 
			Sint32	(*loadfn)(PcmStmTransPara *para, Sint32 cnt), 
			Bool	(*chkfn)(void), Sint32 tracss );

/*  アクセス方式設定  */
Sint32	PCM_SetStmTrAcssSap(PcmHn pcm, Sint32 tracss);

#endif		/*  end of PCM_STM_H  */


/*  end of file  */
