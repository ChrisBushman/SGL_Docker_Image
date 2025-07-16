/*******************************************************************
*
*                       Cinepak for SATURN Player 
*                                by SOJ
*
*                      Copyright(c) 1994,1995 SEGA
*
*   Comment: —˜—pŽÒ—pƒwƒbƒ_
*   File   : SEGA_CPK.H
*   Date   : 1995-10-05
*   Version: 1.20
*   Author : Y.T
*
*******************************************************************/
#ifndef SEGA_CPK_H
#define SEGA_CPK_H


/*-------------------------- sˆ—‘I‘ðt --------------------------*/

/* ƒR[ƒhƒuƒbƒNƒAƒhƒŒƒXƒe[ƒuƒ‹‚ðŽg—p‚·‚é
 * @‚±‚ê‚ð—LŒø‚É‚·‚éê‡‚ÍAcpk_ra15.src cpk_er15.src ‚à“¯Žž‚É‚P‚Æ
 *   éŒ¾‚·‚é‚±‚Æ
 */
/* #define CPK_USE_CBTBL */


/*-------------------- sƒCƒ“ƒNƒ‹[ƒhƒtƒ@ƒCƒ‹t --------------------*/
#include <sega_xpt.h>
#if 0
#include <sega_gfs.h>
#include <sega_stm.h>
#else
#include <sgl_cd.h>
#endif


/*------------------------- sƒ}ƒNƒ’è”t -------------------------*/

/* ƒo[ƒWƒ‡ƒ“ */
#define CPK_VERSION				("CPK Version 1.20 1995-10-05")
							/*	           1         2         3         4	*/
							/*	  1234567890123456789012345678901234567890	*/
							/*	  ++++++++++++++++++++++++++++++++			*/
#define CPK_VERSION_LEN			(32)

/* ƒnƒ“ƒhƒ‹‘” */
#define CPK_HN_MAX				(32)

/* ƒGƒ‰[ƒR[ƒh	*/
typedef enum {
	CPK_ERR_OK					= 0x00000000,/* ƒGƒ‰[‚Í”­¶‚µ‚Ä‚¢‚È‚¢		*/

	/* CPK_Create ‚ÌƒGƒ‰[ */
	CPK_ERR_OUT_OF_HANDLE		= 0x00000101,/* ƒnƒ“ƒhƒ‹”„‚èØ‚ê 			*/
	CPK_ERR_TOO_SMALL_BUF		= 0x00000102,/* œÛŒÜ•Üˆk„Z‰»*/
	CPK_ERR_NO_INIT				= 0x00000103,/* ‰Šú‰»ŠÖ”‚ª®ê‚	*/

	/* CPK_Destroy ‚ÌƒGƒ‰[ */
	CPK_ERR_INVALID_HN			= 0x00000201,/* •s³‚Èƒnƒ“ƒhƒ‹‚ÅÁ‹Ž		*/
	CPK_ERR_ILL_CREATE_MODE		= 0x00000202,/* Create Žž‚Ìƒ‚[ƒh‚Æˆá‚¤		*/

	/* cpk_HeaderProcess ‚ÌƒGƒ‰[ */
	CPK_ERR_TOO_SMALL_FLOP		= 0x00000301,/* FLOP‰Šú’l‚ª¬‚³‚·‚¬‚é		*/
	CPK_ERR_TOO_LARGE_HEADER	= 0x00000302,/* ƒwƒbƒ_‚ª‚«‘å‚·‚¬‚é			*/
											/* (œÛŒÜ•Üˆk„Z‰»	*/
	CPK_ERR_HEADER_DATA			= 0x00000303,/* ƒwƒbƒ_ƒf[ƒ^ƒGƒ‰[ 			*/
	CPK_ERR_TOO_SMALL_PCMBUF	= 0x00000304,/* PCM•Üˆk„Z‰»*/
	CPK_ERR_ILL_SIZE_PCMBUF		= 0x00000305,/* PCM•Üˆk„Z‰»s³ 4096*2`16	*/

	/* cpk_VideoSample ‚ÌƒGƒ‰[ */
	CPK_ERR_SAMPLE_ID			= 0x00000401,/* Ž¯•ÊŽqƒGƒ‰[B00h,01h ˆÈŠO‚¾/
	CPK_ERR_SAMPLE_DATA			= 0x00000402,/* •s³ƒf[ƒ^B				*/

	/*  */
	CPK_ERR_ILLEGAL_PARA		= 0x00000503,/* ˆø‚«”‚ÌŽw’è‚ªŠÔˆá‚Á‚Ä‚¢‚é 	*/
	CPK_ERR_ILLEGAL_HANDLE		= 0x00000504,/* ƒnƒ“ƒhƒ‹‚ªˆÙí 				*/

	/* ˜A‘±Ä¶‚ÉŠÖ‚·‚éƒGƒ‰[ */
	CPK_ERR_NEXT_MOVI_STATUS	= 0x00000601,/* ˜A‘±Ä¶™O–}	*/
	CPK_ERR_NEXT_MOVI_AUDIO		= 0x00000602,/* ƒI[ƒfƒBƒI‚ÌðŒ‚ªˆê’v‚µ‚È‚¢ */
	CPK_ERR_NEXT_MOVI_SCALE		= 0x00000603,/* ƒ^ƒCƒ€ƒXƒP[ƒ‹‚ªˆê’v‚µ‚È‚¢ 	*/
	CPK_ERR_CHANGE_NO_DATA		= 0x00000604,/* ƒf[ƒ^‹Ÿ‹‹•s‘«ó‘Ô‚Åƒ`ƒFƒ“ƒW */
	CPK_ERR_CHANGE_NO_ENTRY		= 0x00000605,/* ƒGƒ“ƒgƒŠ‚ª‚È‚¢ó‘Ô‚Åƒ`ƒFƒ“ƒW */

	/* ƒ|[ƒY‚ÉŠÖ‚·‚éƒGƒ‰[ */
	CPK_ERR_PAUSE_STATUS		= 0x00000701,/* CPK_STAT_PLAY_TIME, ‚Ü‚½‚ÍA
											 * CPK_STAT_PLAY_PAUSE ˆÈŠO‚Ì
											 * ó‘Ô‚Åƒ|[ƒYŠÖ”‚ðƒR[ƒ‹‚µ‚½B
											 */

	/* DMA“]‘—‚ÉŠÖ‚·‚éƒGƒ‰[ */
	CPK_ERR_DMA_MODE			= 0x00000801,/* –¢‘Î‰ž‚Ì“]‘—•ûŽ® 			*/
	CPK_ERR_DMA_CPU_PCM			= 0x00000802,/* CPUDMA‚ªˆÙíI—¹ 			*/

	/* ƒtƒ@ƒCƒ‹“Ç‚Ýž‚ÝƒGƒ‰[ */
	CPK_ERR_GFS_READ			= 0x00000901,/* ‚f‚e‚r‚Ì“Ç‚Ýž‚Ý‚ÅŽ¸”s 		*/
	CPK_ERR_STM_FAD				= 0x00000902,/* STM“]‘—ŠÖ”‚Å,fad‚ª–µ‚ 	*/
	CPK_ERR_STM_HN				= 0x00000903,/* STM“]‘—ŠÖ”‚Å,STM•Û’Ü	*/

	/* CPK_DecodeFrame‚ÉŠÖ‚·‚éƒGƒ‰[ */
	CPK_ERR_DF_NO_DATA			= 0x00000A01,/* ƒwƒbƒ_•ª‚Ìƒf[ƒ^‹Ÿ‹‹‚ª‚È‚¢ 	*/
	CPK_ERR_DF_IS_NOT_VIDEO		= 0x00000A02,/* ƒrƒfƒIƒTƒ“ƒvƒ‹‚Å‚È‚¢ 		*/


	CPK_ERR_END
} CpkErrCode;

/* Ä¶ƒXƒe[ƒ^ƒX */
typedef enum {
	CPK_STAT_PLAY_ERR_STOP		= -1,		/* ˆÙí’âŽ~					*/
	CPK_STAT_PLAY_STOP			= 0,		/* ’âŽ~(ƒNƒŠƒGƒCƒg) 		*/
	CPK_STAT_PLAY_PAUSE			= 1,		/* ˆêŽž’âŽ~		 			*/
	CPK_STAT_PLAY_START			= 2,		/* ŠJŽn						*/
	CPK_STAT_PLAY_HEADER		= 3,		/* ƒwƒbƒ_ˆ—@				*/
	CPK_STAT_PLAY_TIME			= 4,		/* Ä¶’†(ƒ^ƒCƒ}ƒXƒ^[ƒg)	*/
	CPK_STAT_PLAY_END			= 5			/* Ä¶Š®—¹ 				*/
} CpkPlayStatus;

/* ˆêŽž’âŽ~‚Ì§ŒäƒRƒ}ƒ“ƒh */
typedef enum {
	CPK_PAUSE_ON_AT_ONCE,					/* ‘¦ŽžˆêŽž’âŽ~ 			*/
	CPK_PAUSE_ON_KEYFRAME,					/* ƒL[ƒtƒŒ[ƒ€‘Ò‚¿ˆêŽž’âŽ~	*/
	CPK_PAUSE_OFF							/* ˆêŽž’âŽ~‰ðœ 			*/
} CpkPauseCmd;

/* •\Ž¦F”[bit] */
typedef enum {
	CPK_COLOR_24BIT				= 24,		/* 1600–œF 				*/
	CPK_COLOR_15BIT				= 15		/* 32000F 					*/
} CpkColorType;

/* ‹­§Ø‚è‘Ö‚¦‰Â”\«ƒ`ƒFƒbƒN’l */
typedef enum {
	CPK_CHANGE_OK_AT_ONCE		= 0,	/* ‘¦ŽžØ‚è‘Ö‚¦‰Â”\B			*/
	CPK_CHANGE_NO_DATA			= 1,	/* ƒf[ƒ^‹Ÿ‹‹•s‘«B				*/
	CPK_CHANGE_NO_ENTRY			= 2,	/* ƒGƒ“ƒgƒŠ‚ª–³‚¢B				*/
	CPK_CHANGE_TOO_LARGE_HEADER	= 3		/* ƒwƒbƒ_‚ª‚«‘å‚·‚¬‚éB			*/
										/* (ƒoƒbƒtƒ@ƒTƒCƒY‚ª¬‚³‚·‚¬‚é)	*/
} CpkChangeStatus;

/* ‰æ‘œL’£ƒ^ƒCƒ~ƒ“ƒO‚ÌÝ’è’l */
typedef enum {
	CPK_DEC_TIMING_DECODE_IN_HASTE = 0,	/* Žæ‚è‚ ‚¦‚¸L’£‚µ‚Ä‚¨‚­		*/
	CPK_DEC_TIMING_WAIT_AND_DECODE = 1	/* •\Ž¦Žž‚ð‘Ò‚Á‚ÄL’£‚·‚é		*/
} CpkDecTiming;

/* ƒf[ƒ^‚Ì“]‘—•ûŽ® */
typedef enum {
	CPK_TRMODE_CPU = 0,					/* ƒ\ƒtƒgƒEƒFƒA“]‘— 			*/
	CPK_TRMODE_SDMA,					/* ‚c‚l‚`ƒTƒCƒNƒ‹ƒXƒ`[ƒ‹ 		*/
	CPK_TRMODE_SCU,						/* ‚r‚b‚t‚Ì‚c‚l‚`				*/
	CPK_TRMODE_SCUDSPDMA				/* ‚r‚b‚t|‚c‚r‚o‚Ì‚c‚l‚`		*/
} CpkTrMode;

/* [’ˆÓ1] “]‘—•ûŽ®‚ÌÝ’è‚É‚ÍŽŸ‚Ì•\‚ÉŽ¦‚·§ŒÀ‚ª‚ ‚è‚Ü‚·B
 *@@@@@@@@@@@ƒf[ƒ^“]‘—•ûŽ®‚ÌÝ’è‚Ì§ŒÀ
 *  ====================+=================================================
 *     ˆ—@_@•ûŽ®   |  k’±…¯“]‘—  CPU‚ÌDMA  SCU‚ÌDMA  SCU-DSP‚ÌDMA
 *  ====================+=================================================
 *    CPK_SetTrModeCd   |      ›         ›        ›          ~
 *    CPK_SetTrModePcm  |      ›         ›        ~          ›
 *  ====================+=================================================
 * [’ˆÓ2] “]‘—•ûŽ®‚É CPK_TRMODE_SCUDSPDMA ‚ªŽw’è‚³‚ê‚Ä‚¢‚é‚ÆA‰Šúó‘Ô‚Æ‚µ‚ÄA
 *  SCU-DSP ‚ÌŽg—pŒ ‚Í CinepakœQ–Üœw ‚É‚ ‚è‚Ü‚·BSCU-DSP ‚ð CinepakœQ–Üœw ‚Æ 
 *  ‰kàwOŽ¬‚Ì—¼•û‚Å—˜—p‚·‚éê‡‚É‚Í CPK_AllocScuDsp, CPK_FreeScuDsp ‚ðŽg‚¢
 *  ‚Ü‚·B
 */

/* ‚S‚a‚x‚s‚d•¶ŽšƒR[ƒh‚ð Uint32 Œ^‚Å•\Œ»‚·‚éB
 *   ('cvid'‚È‚Ç‚Æ‘‚­‚Æž®“ÛŒÜ‚ªo‚Ä‚µ‚Ü‚¤‚Ì‚ÅA‚±‚ê‚ð‰ñ”ð‚·‚é‚½‚ß‚É)
 */
#define CPK_4BYTE_CHAR(a,b,c,d)					\
	(	((Uint32)((a) & 0x000000FF) << 24) | 	\
		((Uint32)((b) & 0x000000FF) << 16) | 	\
		((Uint32)((c) & 0x000000FF) <<  8) | 	\
		 (Uint32)((d) & 0x000000FF)				\
	)

/* ˆ³k•ûŽ® */
typedef enum {
	CPK_CTYPE_NOVIDEO 	= 0,								/* ‰æ‘œ‚È‚µ 	*/
	CPK_CTYPE_NONE 		= CPK_4BYTE_CHAR('r','a','w',' '),	/* ”ñˆ³k 		*/
	CPK_CTYPE_CVID 		= CPK_4BYTE_CHAR('c','v','i','d'),	/* Cinepakˆ³k 	*/
	CPK_CTYPE_END
} CpkCType;

/* ‚b‚o‚tŽg—pðŒ */
typedef enum {
	CPK_CPU_MAIN 		= 1, 		/* main cpu only (default)			*/
	CPK_CPU_SLAVE 		= 2, 		/* slave cpu only (Not supported) 	*/
	CPK_CPU_DUAL 		= 3, 		/* dual cpu 						*/
	CPK_CPU_END
} CpkCpu;
/* [’ˆÓ] 
 *  ‚b‚o‚tŽg—pðŒ‚É CPK_CPU_DUAL ‚ªŽw’è‚³‚ê‚Ä‚¢‚é‚ÆAslave cpu ‚Í Cinepak
 *  œQ–Üœw ‚ªê—L‚µ‚Ü‚·B‰kàwOŽ¬slave cpu ‚ð—˜—p‚·‚é‚±‚Æ‚Í‚Å‚«‚Ü‚¹‚ñB
 */


/*---------------------- sŠeŽíƒfƒtƒHƒ‹ƒg’lt ----------------------*/

/* ƒNƒƒ}ƒL[ƒJƒbƒgƒIƒt‚ÌƒfƒtƒHƒ‹ƒg’l */
#define CPK_DEFAULT_CKEY_CUTOFF_15		(3 * 7)
#define CPK_DEFAULT_CKEY_CUTOFF_24		(CPK_DEFAULT_CKEY_CUTOFF_15 * 8)

/* –³‰¹—¦‚ÌƒfƒtƒHƒ‹ƒg’l [1/s] */
#define CPK_DEFAULT_SILENCE_RATE	(8)

/* Ä¶‘¬“x‚ÌƒfƒtƒHƒ‹ƒg’l(’Êí‚ÌÄ¶‘¬“x) */
#define CPK_SPEED_ORIGINAL		(0)

/* Ä¶‘¬“x‚ÌÝ’è’PˆÊ */
#define CPK_SPEED_UNIT			(1024)

/* ƒfƒoƒbƒOî•ñ‚Ìo—ÍŠJŽnƒAƒhƒŒƒXiƒfƒtƒHƒ‹ƒg’lj */
#define CPKD_START_ADDR			(0x04000000)

/* ƒfƒoƒbƒOî•ñ‚Ìo—ÍI—¹ƒAƒhƒŒƒXiƒfƒtƒHƒ‹ƒg’lj */
#define CPKD_END_ADDR			(0x047FFFF0)

/* ƒfƒoƒbƒOî•ñ‚Ìo—ÍŠJŽnˆÊ’u‚Ìƒ}[ƒWƒ“[dword] */
#define CPKD_HEAD_MARGIN		(4)

/* CPKDƒVƒXƒeƒ€\‘¢‘Ì‚ÌŽ¯•Ê•¶Žš—ñ’· [dword], [byte] */
#define CPKD_STRING_LEN			(4)
#define CPKD_STRING_LEN_BYTE	(4 * CPKD_STRING_LEN)


/*------------------------- sˆ—ƒ}ƒNƒt -------------------------*/
/* ì¬ƒpƒ‰ƒ[ƒ^‚Ö‚ÌƒAƒNƒZƒXƒ}ƒNƒ */
#define	CPK_PARA_WORK_ADDR(para)	((para)->work_addr)
#define	CPK_PARA_WORK_SIZE(para)	((para)->work_size)
#define	CPK_PARA_BUF_ADDR(para)		((para)->buf_addr)
#define	CPK_PARA_BUF_SIZE(para)		((para)->buf_size)
#define	CPK_PARA_PCM_ADDR(para)		((para)->pcm_addr)
#define	CPK_PARA_PCM_SIZE(para)		((para)->pcm_size)

/* ƒnƒ“ƒhƒ‹ƒAƒNƒZƒX */
#define	CPK_HN_DONT_DEC_VIDEO(hn) ((*(CpkWork **)(hn))->status.dont_dec_video)

/* Code1Book‚ÌƒTƒCƒY [byte],[dbyte] */
#define CPK_BSIZE_CODE1BOOK				(0x800)
#define CPK_BSIZE_CODE1BOOK_FUL			(2 * CPK_BSIZE_CODE1BOOK)
#define CPK_DSIZE_CODE1BOOK				(CPK_BSIZE_CODE1BOOK     / 4)
#define CPK_DSIZE_CODE1BOOK_FUL			(CPK_BSIZE_CODE1BOOK_FUL / 4)

/* ‚Q‚S‚‚‚‰‚”ƒtƒ‹ƒJƒ‰[—p‚Ìƒ[ƒNƒoƒbƒtƒ@ƒTƒCƒY */
#define	CPK_24WORK_BSIZE	sizeof(CpkWork)
#define	CPK_24WORK_DSIZE	((CPK_24WORK_BSIZE + 3) / 4)

/* ‚P‚T‚‚‚‰‚”ƒJƒ‰[—p‚Ìƒ[ƒNƒoƒbƒtƒ@ƒTƒCƒY */
#define	CPK_15WORK_BSIZE	(sizeof(CpkWork) - (4 * CPK_BSIZE_CODE1BOOK))
#define	CPK_15WORK_DSIZE	((CPK_15WORK_BSIZE + 3) / 4)

/* CPKDƒVƒXƒeƒ€\‘¢‘ÌƒAƒNƒZƒX */
#define CPKD_SYS_STRING			(cpkd_sys.string)
#define CPKD_SYS_MODE			(cpkd_sys.mode)
#define CPKD_SYS_START_ADDR		(cpkd_sys.start_addr)
#define CPKD_SYS_END_ADDR		(cpkd_sys.end_addr)
#define CPKD_SYS_PRINT_ADDR		(cpkd_sys.print_addr)
#define CPKD_SYS_RETURN_CNT		(cpkd_sys.return_cnt)


/*-------------------------- sƒf[ƒ^Œ^t --------------------------*/
/* ƒ€[ƒrƒnƒ“ƒhƒ‹ */
typedef void *CpkHn;

/* ì¬ƒpƒ‰ƒ[ƒ^ */
typedef struct {
	Uint32		*work_addr;		/* ƒ[ƒN‚Ìæ“ªƒAƒhƒŒƒX */
								/* –{ƒ‰ƒCƒuƒ‰ƒŠ‚ªƒ€[ƒr‚ðÄ¶‚·‚éŽž‚Ìì‹Æ—Ìˆæ*/
	Uint32		work_size;		/* ƒ[ƒN‚ÌƒoƒCƒg” */
								/* CPK_BSIZE_WORK ‚ðŽw’è‚·‚é */
	Uint32		*buf_addr;		/* ™O–}–¥‰×‚ðŠi”[‚·‚éƒoƒbƒtƒ@‚Ìæ“ªƒAƒhƒŒƒX */
								/* ƒƒ‚ƒŠ‚Ìƒ€[ƒrƒtƒ@ƒCƒ‹‚ÌŽž‚Í‚»‚ÌƒAƒhƒŒƒX */
								/* CD‚Ìƒ€[ƒrƒtƒ@ƒCƒ‹‚ÌŽž‚ÍœÛŒÜ•Üˆk–¯’Ü» */
	Uint32		buf_size;		/* ƒoƒbƒtƒ@ƒTƒCƒY */
								/* ƒƒ‚ƒŠ‚Ìƒ€[ƒrƒtƒ@ƒCƒ‹‚ÌŽž‚Íƒtƒ@ƒCƒ‹ƒTƒCƒY*/
								/* CD‚Ìƒ€[ƒrƒtƒ@ƒCƒ‹‚ÌŽž‚ÍœÛŒÜ•ÜˆkTƒCƒY */
	Uint16		*pcm_addr;		/* ƒTƒEƒ“ƒhƒƒ‚ƒŠ‚ÌPCMƒoƒbƒtƒ@ƒAƒhƒŒƒX 		*/
	Uint32		pcm_size;		/* ƒTƒEƒ“ƒhƒƒ‚ƒŠ‚ÌPCMƒoƒbƒtƒ@ƒTƒCƒY		*/
								/* (1ch•ª‚ÌƒTƒ“ƒvƒ‹”(4096*1..4096*16))		*/
} CpkCreatePara;


/* ƒtƒBƒ‹ƒ€ƒTƒ“ƒvƒ‹ƒe[ƒuƒ‹€–Ú */
typedef struct {
	Uint32		offset;				/* ƒƒfƒBƒAŠJŽnˆÊ’u‚©‚ç‚ÌƒIƒtƒZƒbƒg */
	Sint32		size;				/* ƒTƒ“ƒvƒ‹ƒTƒCƒY 					*/
	Sint32		time;				/* ƒtƒŒ[ƒ€•\Ž¦ŽžA-1:‹O’}„³Ž|–Ý*/
	Sint32		duration;			/* ƒtƒŒ[ƒ€•\Ž¦Ž‘±ŽžŠÔ 			*/
} CpkFilmSample;


/* ƒtƒBƒ‹ƒ€ƒwƒbƒ_ */
typedef struct {
	/* FilmHeader */
	Uint32		film;				/* "FILM" 								*/
	Uint32		size_header;		/* ƒwƒbƒ_ƒTƒCƒY(ƒƒfƒBƒAŠJŽn‚Ö‚Ì‹k­	*/
	Uint32		version;			/* ƒo[ƒWƒ‡ƒ“ 							*/
	Uint32		reserved;			/* —\–ñ 								*/
	/* FrameDescription */
	Uint32		fdsc;				/* "FDSC" 								*/
	Uint32		size_fdsc;			/* FDSCƒTƒCƒY 0x0000001C = 7*4[byte]	*/
	CpkCType	c_type;				/* ˆ³k•ûŽ® "cvid" 						*/
	Uint32		height;				/* ‚½‚Ä•[pixel]						*/
	Uint32		width;				/* ‚æ‚±•[pixel] 						*/
	Uint8		color;				/* •\Ž¦F”[bit] 						*/
	Uint8		sound_channel;		/* ƒ`ƒƒƒlƒ‹” 							*/
	Uint8		sound_smpling_bit;	/* ƒTƒ“ƒvƒŠƒ“ƒOƒrƒbƒg”[bit] 			*/
	Uint8		sound_compress;		/* ˆ³k 00h:–³‚µ 01h:ADPCM 				*/
	Uint32		sound_smpling_rate;	/* Ž|–ÝœÛŒÜ®’ÙHz] ‰ºˆÊ‚Qbyte‚Í¬” 	*/
	Uint32		ckey;				/* ƒNƒƒ}ƒL[î•ñ	 					*/
	/* FilmSampleTable */
	Uint32		stab;				/* "STAB" 								*/
	Uint32		size_stab;			/* STABƒTƒCƒY (‘Ž|–Ý1)*16[byte] 	*/
	Uint32		time_scale_film;	/* ƒ^ƒCƒ€ƒXƒP[ƒ‹ 						*/
	Uint32		sample_total;		/* ‘ƒTƒ“ƒvƒ‹” 						*/
	CpkFilmSample sample[1];		/* ƒTƒ“ƒvƒ‹ƒe[ƒuƒ‹ ”CˆÓƒTƒCƒY 			*/
} CpkHeader;

/* ƒGƒ‰[“o˜^ŠÖ” */
typedef void (*CpkErrFunc)(void *obj, Sint32 err_code);


/* ƒpƒ‰ƒ[ƒ^ */
typedef struct {
	Uint32		AddrBuf;		/* ƒoƒbƒtƒ@ŠJŽnˆÊ’uBƒVƒ€‚Ìê‡ 0x4000000 	*/
	Uint32		SizeBuf;		/* ƒoƒbƒtƒ@ƒTƒCƒYB@ƒVƒ€‚Ìê‡ 0x1000000 	*/
	Uint32		size_trigger;	/* Ä¶ŠJŽnƒgƒŠƒKƒTƒCƒY						*/
	Uint32		AddrVram;		/* ‰æ‘œo—ÍƒAƒhƒŒƒX 						*/
	Uint32		SizeVramLine;	/* ‰æ‘œo—Íƒoƒbƒtƒ@sƒTƒCƒY[byte]			*/
	/* ƒTƒEƒ“ƒhƒhƒ‰ƒCƒoŠÖ˜A */
	Uint32		play_pcm;		/* PCMÄ¶ƒtƒ‰ƒO 0:‰¹‚Ìˆ—‚È‚µ 1:Ä¶		*/
	Uint32		command_blk_no;	/* ƒTƒEƒ“ƒhƒhƒ‰ƒCƒoƒRƒ}ƒ“ƒhƒuƒƒbƒN”Ô† 	*/
	Uint32		pcm_stream_no;	/* PCMƒXƒgƒŠ[ƒ€Ä¶”Ô† 					*/
	Uint16		*addr_pcmbuf;	/* ŽRŸcŸÐšw‚ÌPCMƒoƒbƒtƒ@ƒAƒhƒŒƒX 			*/
	Uint32		size_pcmbuf;	/* ŽRŸcŸÐšw‚ÌPCMƒoƒbƒtƒ@ƒTƒCƒY[sample/1ch] 	*/
	Uint32		pcm_pan;		/* 0..31	*/
	Uint32		pcm_level;		/* 0..7		*/
	Uint32		frequency_vbl;	/* CPK_VblIn()‚ðƒR[ƒ‹‚·‚éŽü”g”[256*Hz] 	*/
} CpkPara;

/* DMA“]‘—î•ñ */
typedef struct {
	Uint32		dst;				/* “]‘—æƒAƒhƒŒƒX 						*/
	Uint32		src;				/* “]‘—Œ³ƒAƒhƒŒƒX 						*/
	Uint32		cnt;				/* “]‘—‰ñ” 							*/
} CpkDmaStock;

/* ŠeŽíó‘Ô */
typedef struct {
	/* ‘S‘Ì§Œäî•ñ */
	Sint32		stat_start;			/* ƒXƒe[ƒ^ƒX‚ÌŠJŽn						*/
	Sint32		play;				/* Ä¶ó‘Ô 							*/
	Uint32		RequestDispFrame;	/* ƒtƒŒ[ƒ€•\Ž¦—v‹ƒtƒ‰ƒO 				*/
	Uint32		CntSample;			/* ƒTƒ“ƒvƒ‹ƒJƒEƒ“ƒ^ 					*/
	Uint32		CntFrame;			/* ƒtƒŒ[ƒ€ƒJƒEƒ“ƒ^ 					*/
	Uint32		offset_media;		/* ƒƒfƒBƒAŠJŽnˆÊ’u‚Ö‚ÌƒIƒtƒZƒbƒg		*/
	Uint32		display_keyframe;	/* •\Ž¦‰æ‘œƒL[ƒtƒŒ[ƒ€ƒtƒ‰ƒO 			*/
	Uint32		standby_keyframe;	/* ‘Ò‹@‰æ‘œƒL[ƒtƒŒ[ƒ€ƒtƒ‰ƒO 			*/
	Uint32		standby_video_flag;	/* •\Ž¦‘Ò‹@‰æ‘œ‚ ‚èƒtƒ‰ƒO 				*/
	Uint32		standby_time_display;/* ‘Ò‹@‰æ‘œ‚Ì•\Ž¦Žž					*/
	Uint32		standby_time_duration;/* ‘Ò‹@‰æ‘œ‚Ì•\Ž¦Ž‘±ŽžŠÔ				*/
	Uint32		only_audio;			/* flag that only audio sample 			*/
	Uint32		only_video;			/* flag that only video sample 			*/
	Uint32		color;				/* o—ÍF”[bit]B15 or 24				*/
	Uint32		wait_keyframe_to_pause;/* ˆêŽž’âŽ~ˆ——pŒO–ØˆÏ‘Ò‚¿ƒtƒ‰ƒO 	*/
	Uint32		wait_and_decode;	/* 1:•\Ž¦Žž‚ð‘Ò‚Á‚Ä‚©‚çL’£‚·‚é		*/
	Sint32		flag_prohibit_komaotoshi;/* 1:ƒRƒ}—Ž‚Æ‚µkinnshiƒtƒ‰ƒO		*/
	Sint32		ckey_cutoff;		/* chroma key cutoff level 				*/
	Sint32		final_audio_sample;	/* ÅŒã‚ÌƒI[ƒfƒBƒIƒTƒ“ƒvƒ‹ 			*/
	Sint32		silence_rate;		/* –³‰¹—¦ [1/s]							*/
	
	/* ƒŠƒ“ƒOƒoƒbƒtƒ@§Œäî•ñ */
	Sint32		ring_start;			/* ‚ÌŠJŽn								*/
	Uint32		OffsetWrite;		/* ƒoƒbƒtƒ@“Ç‚ÝE‘‚«ˆÊ’u[byte]			*/
	Uint32		OffsetRead;			/* (ƒtƒ@ƒCƒ‹æ“ª‚©‚ç‚ÌƒIƒtƒZƒbƒg)		*/
	Uint32		AddrWrite;			/* ƒoƒbƒtƒ@“Ç‚ÝE‘‚«ˆÊ’u(ƒAƒhƒŒƒX)		*/
	Uint32		AddrRead;			/* 										*/
	Uint32		AddrRing;			/* ƒŠƒ“ƒOƒoƒbƒtƒ@ŠJŽnƒAƒhƒŒƒX 			*/
	Uint32		AddrRingEnd;		/* ƒŠƒ“ƒOƒoƒbƒtƒ@I—¹ƒAƒhƒŒƒX 			*/
	Sint32		SizeHead;			/* ƒwƒbƒ_ƒoƒbƒtƒ@ƒTƒCƒY [byte] ¶””{ */
	Uint32		SizeRing;			/* ƒŠƒ“ƒOƒoƒbƒtƒ@ƒTƒCƒY [byte] ¶””{ */
	Uint32		SizeFlop;			/* ‚e‚k‚n‚oƒoƒbƒtƒ@ƒTƒCƒY [byte] 		*/
	
	/* PCMÄ¶î•ñ */
	Sint32		audio_start;		/* ‚ÌŠJŽn								*/
	Uint32		play_pcm;			/* PCMÄ¶ƒtƒ‰ƒO 0:‰¹‚Ìˆ—‚È‚µ 1:Ä¶	*/
	Uint32		bsize_pcmbuf;		/* ŽRŸcŸÐšw‚ÌPCMƒoƒbƒtƒ@ƒTƒCƒY[byte/1ch]*/
	Uint16		*addr_pcmbuf_end;	/* PCMƒoƒbƒtƒ@I—¹ƒAƒhƒŒƒX 				*/
	Uint16		*addr_pcm_write;	/* PCMƒoƒbƒtƒ@‘‚«ž‚ÝƒAƒhƒŒƒX 			*/
	Uint32		smpling_rate;		/* Ž|–ÝœÛŒÜ®’ÙHz] ‰ºˆÊ‚Pbyte‚Í¬” 	*/
	Uint32		cnt_4ksample;		/* PCM play address 4kŽ|–ÝPˆÊXV‹±Ÿ_ */
	Sint32		flag_fill_silence;	/* –³‰¹Ý’èÏ‚Ýƒtƒ‰ƒO					*/
	Sint32		sample_write;		/* ‹Ÿ‹‹ƒTƒ“ƒvƒ‹”[sample]				*/
									/* (æ“ª–¦p‚©‚ç‚ÌŒ»Ý‚Ü‚Å‚Ì‘‹Ÿ‹‹—Ê)	*/
	Sint32		sample_film_start;	/* ƒtƒBƒ‹ƒ€ŠJŽnŽžƒTƒ“ƒvƒ‹”[sample]		*/
									/* (æ“ª–¦p‚©‚çŒ»Ý–¦pŠJŽnŽž“_‚Ü‚Å‚Ì)	*/
	Sint32		sample_pause;		/* ƒ|[ƒYŽžƒTƒ“ƒvƒ‹”[sample]			*/
									/* (æ“ª–¦p‚©‚çÅ‹ß‚Ìƒ|[ƒYŽž“_‚Ü‚Å‚Ì)	*/
	Sint32		count_start;		/* ‚b‚o‚tƒNƒƒbƒNƒ^ƒCƒ}ŠJŽnŽž 		*/
									/* 						[CPUŒÙˆW>>15] 	*/
	Sint32		clock_scale;		/* ‚b‚o‚tƒNƒƒbƒNƒ^ƒCƒ}‚ÌƒXƒP[ƒ‹ 		*/
	Sint32		dont_stop_pcm;		/* I—¹Žž‚ÉPCMƒXƒgƒbƒv‚µ‚È‚¢ƒtƒ‰ƒO 		*/

	/* ŽŸƒ€[ƒr[ŠÇ—î•ñ */
	Sint32		next_start;			/* ‚ÌŠJŽn								*/
/*	CpkHn		hn_next;*/			/* ŽŸƒnƒ“ƒhƒ‹(–¢“o˜^CŠJŽnŒã‚Í NULL) 	*/
	CpkHn		hn_next_start;		/* ŽŸƒnƒ“ƒhƒ‹(ŠJŽnŒã) 					*/
	Sint32		dont_dec_video;		/* I—¹Žž‚ÉPCMƒXƒgƒbƒv‚µ‚È‚¢ƒtƒ‰ƒO 		*/
	
	
	/* PCMƒf[ƒ^‚Ì‚c‚l‚`“]‘—‚ÉŠÖ‚·‚éî•ñ */
	Sint32		cppcm_start;		/* ‚ÌŠJŽn								*/
	CpkTrMode	copy_mode_pcm;		/* ƒf[ƒ^‚Ì“]‘—•ûŽ® 					*/
	Uint32		flag_dma_hold;		/* 1:‚±‚Ìƒ€[ƒr[‚ªDMAŽ‘Œ¹‚ð•ÛŽ‚µ‚Ä‚¢‚é*/
	Uint32		pcm_renew_size;		/* DMAI—¹Žžpcm•Üˆk‘‚ž‚ŸmàQŸ_XVŽQ}*/
	Uint32		buf_renew_size;		/* DMAI—¹Žž•Üˆkæ‚èÎàQŸ_XVŽQ} 	*/
	Uint32		idx_dma_pcm_tbl;	/* dma_pcm_tbl ‚ÌƒCƒ“ƒfƒbƒNƒX 			*/
	CpkDmaStock	dma_pcm_tbl[4];		/* DMA“]‘—î•ñƒGƒ“ƒgƒŠƒe[ƒuƒ‹ 			*/
	
	/* ŽžŠÔŠÇ—î•ñ(ƒrƒfƒI‚Ì‚Ýƒ€[ƒr[—p) */
	Sint32		time_start;			/* ‚ÌŠJŽn								*/
									/* 95-08-17 Y.T : CPK_SetSpeed ‘Î‰ž 	*/
									/*   Vbl‹±Ÿ_‚ð•Û’Üˆ‚‚			*/
	Sint32		cnt_vbl_in;			/* Vlb-In Š„‚èž‚ÝƒJƒEƒ“ƒ^[vbl]			*/
	Sint32		vblcnt_regulator;	/* Vlb-In ƒJƒEƒ“ƒ^Ä¶‘¬“x’²ßŠí		*/
	Sint32		vbl_film_start;		/* ƒtƒBƒ‹ƒ€ŠJŽnŽž[vbl] 				*/
	Sint32		vbl_pause;			/* ƒ|[ƒYŠJŽnŽž[vbl] 					*/
	Sint32		vbl_pcm_start;		/* PCMŠJŽnŽž[vbl]
									 * Å‰‚Ì ‚É cnt_4ksample ‚ðŒë‚Á‚Ä‹±Ÿc
									 * ‰N–Ý‚µ‚È‚¢‚æ‚¤‚ÉB
									 */
	Sint32		time_frame;			/* •\Ž¦‚µ‚½ƒtƒŒ[ƒ€‚Ì•\Ž¦Žž{Ž‘±ŽžŠÔ	*/

	/* Ä¶‘¬“xŠÇ—î•ñ */
	Sint32		speed;				/* Ä¶‘¬“x[‘Î“™‘¬”ä~1024] 			*/
	Bool		audio_sw;			/* Ä¶‘¬“x•ÏXŽž‚Ì‹O’}„³o—ÍQˆ`		*/

	/* ƒGƒ‰[î•ñ */
	Uint32		err_start;			/* ƒGƒ‰[î•ñ‚ÌŠJŽn                     */
	Uint32		CntLossFrame;		/* ƒRƒ}—Ž‚¿ƒJƒEƒ“ƒ^ 					*/
	Uint32		CntPcmLoadMiss;		/* PCMƒ[ƒhƒ~ƒXƒJƒEƒ“ƒ^ 				*/
	Uint32		CntCallTask;		/* ƒ^ƒXƒNƒR[ƒ‹ƒJƒEƒ“ƒ^ 				*/
	Uint32		CntOverCallTask;	/* ‰ßèƒ^ƒXƒNƒR[ƒ‹ƒJƒEƒ“ƒ^ 			*/
	Uint32		cnt_buf_empty;		/* ƒoƒbƒtƒ@‹óƒ^ƒXƒNƒJƒEƒ“ƒ^ 			*/
	Uint32		max_late_time;		/* Å‘å’x‚êŽžŠÔ[ƒtƒBƒ‹ƒ€ŽžŠÔ’PˆÊ] 		*/
	Uint32		max_late_frame;		/* Å‘å’x‚êƒtƒŒ[ƒ€”Ô†					*/
} CpkStatus;


/* ƒtƒ@ƒCƒ‹ƒAƒNƒZƒXŠÖŒW */
typedef struct {
	GfsHn		gfs;			/* ƒtƒ@ƒCƒ‹ƒnƒ“ƒhƒ‹ */
	Sint32		load_sect;		/* “]‘—Å‘åƒZƒNƒ^” */
	Bool		called_cdread;	/* GFS_NwCdRead ‚ðƒR[ƒ‹‚µ‚½‚ç TRUE */
	Bool		exec_one_state;	/* ExecOne ŽÀs’†‚È‚ç TRUE */
	Sint32		exec_load_size;	/* ExecOne ‚Å“Ç‚Ýž‚ÞƒoƒCƒg” */
	Sint32		now_load_size;	/* ExecOne ‚Å“Ç‚Ýž‚ñ‚Å‚¢‚éƒoƒCƒg” */
	Sint32		load_total_sect;/* “Ç‚Ýž‚Ý‘ƒZƒNƒ^” */
	Sint32		file_sect;		/* ƒtƒ@ƒCƒ‹‚ÌƒZƒNƒ^” */
	Sint32		tr_mode;		/* “]‘—ƒ‚[ƒh */
} CpkFileGfsPara;

typedef struct {
	StmHn		stm;			/* ƒXƒgƒŠ[ƒ€ƒnƒ“ƒhƒ‹ */
	Sint32		load_sect;		/* “]‘—Å‘åƒZƒNƒ^” */
	Sint32		old_cd_buf_num;	/* ‘O‰ñ‚Ì‚b‚cƒoƒbƒtƒ@‚Ì” */
	Bool		dma_state;		/* ‚c‚l‚`“]‘—‚µ‚Ä‚¢‚é‚È‚ç TRUE */
	Sint32		dma_sect;		/* ‚c‚l‚`“]‘—‚·‚éƒZƒNƒ^” */
	Uint32		*write_addr;	/* ‘‚«ž‚ÞƒAƒhƒŒƒX */
	Sint32		buf_bsize;		/* ‹ó‚«ƒoƒbƒtƒ@‚ÌƒoƒCƒg” */
	Sint32		write_bsize;	/* “]‘—‚µ‚½ƒoƒCƒg” */
	Sint32		load_total_sect;/* “Ç‚Ýž‚Ý‘ƒZƒNƒ^” */
	StmTrFunc	load_func;		/* “]‘—ŠÖ” */
} CpkFileStmPara;

typedef struct {
	Sint32		faccess_type;		/* ƒtƒ@ƒCƒ‹ƒAƒNƒZƒXƒ^ƒCƒv */
	/* ƒtƒ@ƒCƒ‹ƒ[ƒJƒ‹ƒf[ƒ^ */
	union	{
		CpkFileGfsPara	gfs;
		CpkFileStmPara	stm;
	} data;
} CpkFilePara;

/* ŽÀsŠÖ” */
typedef struct {
	void	(*start)(CpkHn cpk);
	void	(*task)(CpkHn cpk);
	Sint32	(*preload_movie)(CpkHn cpk, Sint32 size);
	void	(*set_load_num)(CpkHn cpk, Sint32 load_sct);
	void	(*set_trmode)(CpkHn cpk, CpkTrMode mode);
	void	(*get_stock_size)(CpkHn cpk, Sint32 *ring_bsize, Sint32 *all_bsiz);
	void	(*get_stock_time)(CpkHn cpk, Sint32 *ring_time, Sint32 *all_time);
} CpkExecFunc;


typedef struct {
	Sint32		work_start;				/* ƒ[ƒN‚ÌŠJŽn					*/
	CpkPara 	para;					/* ‚b‚‰‚Ž‚…‚‚‚‹‹N“®ƒpƒ‰ƒ[ƒ^ */
	CpkStatus 	status;					/* ŠeŽíó‘ÔiŒöŠJ•”•ªj 		*/
	CpkHeader 	*header;				/* ƒtƒBƒ‹ƒ€ƒtƒ@ƒCƒ‹ƒwƒbƒ_ 		*/
	CpkFilePara	filepara;				/* ƒtƒ@ƒCƒ‹ƒAƒNƒZƒXƒpƒ‰ƒ[ƒ^	*/
	CpkExecFunc	execfunc;				/* ŽÀsŠÖ”						*/
#ifdef CPK_USE_CBTBL
	Uint32		*cbtblp[4];				/* ®’Ü–ÜˆW‰cŸØbˆÊŸ×’†‰›—Ý‰Û	*/
	Uint32		cbtbl1a[256];			/* ®’Ü–ÜˆW‰cŸØbˆÊŸ× 			*/
	Uint32		cbtbl2a[256];			/* 								*/
	Uint32		cbtbl1b[256];			/* 								*/
	Uint32		cbtbl2b[256];			/* 								*/
#endif
	Uint32 		code_book[4 * CPK_DSIZE_CODE1BOOK_FUL];
										/* ã’i code1book, code2book	*/
										/* ‰º’i code1book, code2book	*/
} CpkWork;


/* CPKDƒVƒXƒeƒ€\‘¢‘Ì */
typedef struct {
	Sint32 string[CPKD_STRING_LEN];	/* Ž¯•Ê•¶Žš—ñ */
	Sint32 mode;					/* o—Íƒ‚[ƒh ‚OFo—Í‚µ‚È‚¢(ƒfƒtƒHƒ‹ƒg) */
	Sint32 *start_addr;				/* o—ÍƒAƒhƒŒƒX‚Ìæ“ª */
	Sint32 *end_addr;				/* o—ÍƒAƒhƒŒƒX‚Ì––”ö */
	Sint32 *print_addr;				/* Œ»Ý‚Ìo—ÍƒAƒhƒŒƒX */
	Sint32 return_cnt;				/* Ü‚è•Ô‚µƒJƒEƒ“ƒ^ */
} CpkdSys;


/*------------------------ sƒfƒoƒbƒO—péŒ¾t ------------------------*/

/* CPKDƒVƒXƒeƒ€\‘¢‘Ì */
extern CpkdSys cpkd_sys;

/* ƒfƒoƒbƒOo—Íƒ‚[ƒh‚ÌÝ’è@0N:o—Í‚·‚é, OFF:o—Í‚µ‚È‚¢(’}–©c) */
void CPKD_SetMode(Bool mode);

/* ƒfƒoƒbƒOo—ÍƒAƒhƒŒƒX‚ÌÝ’è */
void CPKD_SetPrintAddr(Sint32 *start, Sint32 *end);

/* ƒfƒoƒbƒO•¶Žš—ñ‚Ìo—Í(•¶Žš—ñ’·‚Í‚S‚Ì®””{‚É‹KŠi‰»‚³‚ê‚é) */
void CPKD_PrintStr(char *str);

/* ƒfƒoƒbƒOƒf[ƒ^‚Ìo—Í(‚SƒoƒCƒg) */
void CPKD_PrintData(Sint32 data);


/*-------------------------- sŠÖ”éŒ¾t --------------------------*/

/* ƒ‰ƒCƒuƒ‰ƒŠ‚Ì‰Šú‰» */
Bool CPK_Init(void);

/* ƒ‰ƒCƒuƒ‰ƒŠ‚ÌI—¹ˆ— */
void CPK_Finish(void);

/* ƒnƒ“ƒhƒ‹‚Ìì¬iƒƒ‚ƒŠj */
CpkHn CPK_CreateMemMovie(CpkCreatePara *para);

/* ƒnƒ“ƒhƒ‹‚ÌÁ‹Žiƒƒ‚ƒŠj */
void CPK_DestroyMemMovie(CpkHn cpk);

/* ƒnƒ“ƒhƒ‹‚Ìì¬iƒtƒ@ƒCƒ‹ƒVƒXƒeƒ€j */
CpkHn CPK_CreateGfsMovie(CpkCreatePara *para, GfsHn gfs);

/* ƒnƒ“ƒhƒ‹‚ÌÁ‹Žiƒtƒ@ƒCƒ‹ƒVƒXƒeƒ€j */
void CPK_DestroyGfsMovie(CpkHn cpk);

/* ƒnƒ“ƒhƒ‹‚Ìì¬iƒXƒgƒŠ[ƒ€ƒVƒXƒeƒ€j */
CpkHn CPK_CreateStmMovie(CpkCreatePara *para, StmHn stm);

/* ƒnƒ“ƒhƒ‹‚ÌÁ‹ŽiƒXƒgƒŠ[ƒ€ƒVƒXƒeƒ€j */
void CPK_DestroyStmMovie(CpkHn cpk);

/* Ä¶ƒ^ƒXƒN */
void CPK_Task(CpkHn cpk);

/* •\Ž¦ƒ^ƒCƒ~ƒ“ƒO‚Ì”»’è */
Bool CPK_IsDispTime(CpkHn cpk);

/* •\Ž¦ƒtƒŒ[ƒ€Ží•Ê‚Ì”»’è */
Bool CPK_IsKeyframe(CpkHn cpk);

/* •\Ž¦Š®—¹‚Ì’Ê’m */
void CPK_CompleteDisp(CpkHn cpk);

/* ‚uƒuƒ‰ƒ“ƒN‚h‚mˆ—ŠÖ” */
void CPK_VblIn(void);

/* Ä¶ŠJŽn */
void CPK_Start(CpkHn cpk);

/* Ä¶’âŽ~ */
void CPK_Stop(CpkHn cpk);

/* ˆêŽž’âŽ~ */
Bool CPK_Pause(CpkHn cpk, CpkPauseCmd cmd);

/* ŽŸ‚ÉÄ¶‚·‚éƒ€[ƒr‚Ì“o˜^ */
void CPK_EntryNext(CpkHn cpk);

/* ƒ€[ƒr‚Ì‹­§Ø‚è‘Ö‚¦ */
void CPK_Change(void);

/* ƒ€[ƒr‚ÌØ‚è‘Ö‚¦ó‘Ô‚ÌŽæ“¾ */
CpkChangeStatus CPK_CheckChange(void);

/* Žw’èƒtƒŒ[ƒ€‚Ì“WŠJ */
void CPK_DecodeFrame(CpkHn cpk, Sint32 sample_no);

/* Žw’èƒtƒŒ[ƒ€‚Ì“WŠJ(·•ªƒtƒŒ[ƒ€•ÛØ) */
void CPK_DecodeFrameSafety(CpkHn cpk, Sint32 sample_no);

/* “WŠJæƒAƒhƒŒƒX‚ÌÝ’è */
void CPK_SetDecodeAddr(CpkHn cpk, void *dec_addr, Sint32 dec_line_size);

/* •\Ž¦F”‚ÌÝ’è */
void CPK_SetColor(CpkHn cpk, CpkColorType color);

/* Å‘å“]‘—ƒZƒNƒ^”‚ÌÝ’è */
void CPK_SetLoadNum(CpkHn cpk, Sint32 read_sct);

/* Ä¶‚o‚`‚m‚ÌÝ’è */
void CPK_SetPan(CpkHn cpk, Sint32 pan);

/* Ä¶ƒ{ƒŠƒ…[ƒ€‚ÌÝ’è */
void CPK_SetVolume(CpkHn cpk, Sint32 volume);

/* Ä¶‘¬“x‚ÌÝ’è */
void CPK_SetSpeed(CpkHn cpk, Sint32 speed, Bool audio_sw);

/* ‚o‚b‚lƒRƒ}ƒ“ƒhƒuƒƒbƒN”Ô†‚ÌÝ’è */
/* void CPK_SetPcmCmdBlockNo(CpkHn cpk, Sint32 blk_no); */

/* ‚o‚b‚lƒXƒgƒŠ[ƒ€”Ô†‚ÌÝ’è */
void CPK_SetPcmStreamNo(CpkHn cpk, Sint32 stream_no);

/* ƒL[ƒAƒEƒgi“§–¾‚É‚·‚éFj”ÍˆÍ‚ÌÝ’è */
void CPK_SetKeyOutRange(CpkHn cpk, Sint32 range);

/* –³‰¹—¦‚ÌÝ’è */
void CPK_SetSilenceRate(CpkHn cpk, Sint32 silence_rate);

/* ƒf[ƒ^‚Ì“]‘—•ûŽ®‚ÌÝ’èi‚b‚cƒuƒƒbƒN¨ƒŠƒ“ƒOƒoƒbƒtƒ@j */
void CPK_SetTrModeCd(CpkHn cpk, CpkTrMode mode);

/* ƒf[ƒ^‚Ì“]‘—•ûŽ®‚ÌÝ’èiƒŠƒ“ƒOƒoƒbƒtƒ@¨‚o‚b‚lƒoƒbƒtƒ@j */
void CPK_SetTrModePcm(CpkHn cpk, CpkTrMode mode);

/* Ä¶ŠJŽnƒgƒŠƒKƒTƒCƒY‚ÌÝ’è [byte] */
void CPK_SetStartTrgSize(CpkHn cpk, Sint32 bsize);

/* ƒ^ƒCƒ€ƒXƒP[ƒ‹‚ÌŽæ“¾ */
Sint32 CPK_GetTimeScale(CpkHn cpk);

/* Ä¶Žž‚ÌŽæ“¾ [timescale] */
Sint32 CPK_GetTime(CpkHn cpk);

/* Ä¶ƒXƒe[ƒ^ƒX‚ÌŽæ“¾ */
CpkPlayStatus CPK_GetPlayStatus(CpkHn cpk);

/* ƒtƒBƒ‹ƒ€ƒwƒbƒ_‚ÌƒAƒhƒŒƒX‚ÌŽæ“¾ */
CpkHeader *CPK_GetHeader(CpkHn cpk);

/* ‘‚«ž‚Ýƒoƒbƒtƒ@ƒAƒhƒŒƒX‚ÌŽæ“¾ */
Uint32 *CPK_GetWriteBuf(CpkHn cpk, Sint32 *free_size, Sint32 *free_total);

/* ƒoƒbƒtƒ@‚Ö‚Ì‘‚«ž‚ÝƒTƒCƒY‚Ì’Ê’m */
void CPK_NotifyWriteSize(CpkHn cpk, Sint32 write_size);

/* ƒƒ‚ƒŠ‚Éƒ€[ƒr‚ð“Ç‚Ýž‚Þ */
Sint32 CPK_PreloadMovie(CpkHn cpk, Sint32 size);

/* ƒƒ‚ƒŠ‚ÉƒtƒBƒ‹ƒ€ƒwƒbƒ_‚ð“Ç‚Ýž‚Þ */
Sint32 CPK_PreloadHeader(CpkHn cpk);

/* ƒGƒ‰[ŠÖ”‚Ì“o˜^ */
void CPK_SetErrFunc(CpkErrFunc func, void *obj);

/* ƒGƒ‰[î•ñ‚ÌŽæ“¾ */
CpkErrCode CPK_GetErr(void);

/* SCU-DSPŽ‘Œ¹‚ÌŠm•Û */
Sint32 CPK_AllocScuDsp(void);

/* SCU-DSPŽ‘Œ¹‚Ì‰ð•ú */
void CPK_FreeScuDsp(void);

/* ‚b‚o‚tŽg—pðŒ‚ÌÝ’è */
void CPK_SetCpu(CpkCpu cpu);

/* ƒXƒgƒŠ[ƒ€ƒT[ƒoŠÖ”‚ÌƒR[ƒ‹‚ÌÝ’è */
void CPK_SetStmServerCall(Bool mode);

/* ƒf[ƒ^”õ’~—Ê‚ÌŽæ“¾ [byte] */
/* ‚r‚f‚k‚Ì‚f‚e‚r‚Ìƒo[ƒWƒ‡ƒ“‚ª‚–‚…‚’‚PD‚Q‚PˆÈã‚Ìê‡‚Ì‚ÝŽg—p‰Â”\ */
void CPK_GetStockSize(CpkHn cpk, Sint32 *ring_bsize, Sint32 *all_bsize);

/* ƒf[ƒ^”õ’~—Ê‚ÌŽæ“¾ [timescale] */
/* ‚r‚f‚k‚Ì‚f‚e‚r‚Ìƒo[ƒWƒ‡ƒ“‚ª‚–‚…‚’‚PD‚Q‚PˆÈã‚Ìê‡‚Ì‚ÝŽg—p‰Â”\ */
void CPK_GetStockTime(CpkHn cpk, Sint32 *ring_time, Sint32 *all_time);

#endif /* SEGA_CPK_H */
