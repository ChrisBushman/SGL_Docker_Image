/************************************************************************/
/*	file name															*/
/*		snd_main.c														*/
/*	copyright															*/
/*		SEGA ENTERPRISES												*/
/*  purpose																*/
/*      サウンドI/Fライブラリソースファイル								*/
/*  description															*/
/*      サウンドドライバとのインタフェースを実現する					*/
/*  interface(as follows)												*/
/*		< functions list >												*/
/*		SET_COMMAND()       	- コマンドセット						*/
/*		SET_PRM()           	- パラメータセット						*/
/*		SND_Init()				- サウンドシステム起動					*/
/*		SND_MoveData()  		- サウンドデータ転送					*/
/*		SND_ChgMap()			- サウンドエリアマップ変更				*/
/*		SND_SetTlVl()			- 全体音量設定							*/
/*		SND_ChgEfct()			- エフェクト変更						*/
/*		SND_ChgMix()			- ミキサ変更							*/
/*		SND_ChgMixPrm()			- ミキサパラメータ変更					*/
/*		SND_ChkHard()			- ハードウェアチェック					*/
/*		SND_StopDsp()			- DSP中止								*/
/*		SND_OffAllSound()		- 全サウンド発音スロット停止			*/
/*		SND_StartSeq()			- シーケンス開始						*/
/*		SND_StopSeq()			- シーケンス中止						*/
/*		SND_PauseSeq()			- シーケンス一時中断					*/
/*		SND_ContSeq()			- シーケンス一時中断解除				*/
/*		SND_SetSeqVl()			- シーケンス音量設定					*/
/*		SND_ChgTempo()			- テンポ変更							*/
/*		SND_CtrlDirMidi()		- MIDIダイレクトコントロール			*/
/*		SND_SetSeqPan()			- シーケンスPAN設定						*/
/*		SND_StartPcm()			- PCM開始								*/
/*		SND_StopPcm()			- PCM中止								*/
/*		SND_ChgPcm()			- PCM変更								*/
/*		SND_StartVlAnl()		- 音量解析開始							*/
/*		SND_StopVlAnl()			- 音量解析中止							*/
/*		SND_SetCdDaLev()		- CD-DA Level設定						*/
/*		SND_SetCdDaPan()		- CD-DA Pan設定							*/
/*		SND_GetSeqStat()		- シーケンスステータス取得				*/
/*		SND_GetSeqPlayPos()		- 発音管理番号再生位置取得				*/
/*		SND_GetPcmPlayAdr()		- PCM実行アドレス取得					*/
/*		SND_GetAnlTlVl()		- 解析全体ステレオ音量取得				*/
/*		SND_GetAnlHzVl()		- 解析周波数帯域別ステレオ音量取得		*/
/*		SND_SetQSound()			- Ｑサウンド定位設定					*/
/*		SND_Set3D_Stat()		- Ｙサウンド定位設定					*/
/*		SND_Set3D_Init()		- Ｙサウンド初期化						*/
/*		SND_Init_Sound()		- サウンド初期化						*/
/*		SND_AllocPcm()			- PCM用スロットの確保					*/
/*		SND_ReleasePcm()		- PCM用スロットの開放					*/
/*  caveats																*/
/*		none															*/
/*  author(s)															*/
/*		1994-05-18  N.T			Ver.0.90								*/
/*  mod history															*/
/*		1994-11-08  N.T			Ver.1.09								*/
/*		1994-12-30  N.T			Ver.1.10								*/
/*		1994-12-30  K.T			Ver.1.10(+)								*/
/*		1995-03-23	Y.K(DM)		Ver.1.11								*/
/*		1996-10-18	Y.K(DM)		Ver.1.20								*/
/*		1997-07-11	Y.K(DM)		Ver.1.30								*/
/*		1997-08-21	Tany, A.H	Ver.1.31								*/
/************************************************************************/

/************************************************************************/
/*		C VIRTUAL TYPES DEFINITIONS										*/
/************************************************************************/
#include <string.h>

#ifdef USE_SGL
#include "sgl.h"
#include "sl_def.h"
#else
#include <machine.h>
#include "sega_xpt.h"
#include "sega_int.h"
#include "sega_per.h"
#include "sega_dma.h"
#endif

#include "sega_snd.h"

/************************************************************************/
/*		USER SUPPLIED INCLUDE FILES										*/
/************************************************************************/
#if 0    /* Ver.1.31 sega_snd.h に統合 */
#include "snd_main.h"
#endif
/************************************************************************/
/*		GLOBAL DECLARATIONS												*/
/************************************************************************/
Uint8 *snd_adr_sys_int_work;				/* system i/f work top adrs格納	*/
Uint32 snd_msk_work_work;					/* sound priority mask			*/

/********************************************************************/
/*		LOCAL DEFINES/MACROS										*/
/********************************************************************/
#if 0
#define _DMA_SCU								/* SCU DMA を使用する		*/
#endif
/*		定数 */
/**** アドレス **************************************************************/
#define ADR_SCSP_REG	((Uint8 *)0x25b00400)	/* SCSP共通制御レジスタ		*/
#define ADR_SND_MEM		((Uint8 *)0x25a00000)	/* sound mem. top address	*/
#define ADR_SND_VECTOR	((Uint8 *)0x25a00000)	/* sound vector address		*/
#define ADR_SYS_TBL		(ADR_SND_MEM + 0x400)	/* system i/f area			*/

/**** システムインタフェーステーブルオフセット ******************************/
#define ADR_SYS_INFO		(0x00)	/* system info. table address			*/
#define ADR_HOST_INT		(0x04)	/* host i/f work address				*/
#define ADR_ARA_CRNT		(0x08)	/* sound area map CRNT work top adrs	*/
#define ADR_SYS_INT_WORK	(0x12)	/* system i/f work top address			*/
#define ADR_HARD_CHK_STAT	(0x18)	/* hard check return status 格納 work	*/
#define	ADR_TIMING_FLAG		(0xe0)	/* handshake timing flag				*/
#define	ADR_CMD_MODE		(0xe1)	/* command sequence control flag		*/

/**** システム情報テーブルオフセット ****************************************/
#define ADR_PRG_ADR		(0x00)			/* 68K program area top address		*/
#define ADR_PRG_SIZE	(0x04)			/* 68K program area size        	*/
#define ADR_ARA_ADR		(0x08)			/* sound area map area top address	*/

/**** ホストインタフェースワークオフセット **********************************/
#define ADR_COM_DATA	(0x00)					/* コマンド					*/
#define ADR_PRM_DATA	(0x02)					/* パラメータ				*/
#define ADR_SONG_STAT	(0x80)					/* song status				*/
#define ADR_TL_VL		(0x90)					/* Total volume				*/
#define ADR_TL_HZ_VL	(0x94)					/* 周波数帯域別Volume		*/
#define ADR_PCM			(0xa0)					/* PCM						*/
#define ADR_SEQ			(0xb0)					/* Sequence					*/
/**** ホストインタフェースワーク定数 ****************************************/
#define SIZE_COM_BLOCK		(0x10)				/* command block size		*/
#define MAX_NUM_COM_BLOCK	8					/* command block max #		*/

/**** サウンド起動 **********************************************************/
#define SCSP_REG_SET    0x0200                  /* SCSPレジスタ設定値		*/
#define MEM_CLR_SIZE    0xb000                  /* sound mem. clear size	*/

/**** サウンドエリアマップ情報(long word単位) *******************************/
#define ARA_MAP_SIZE		0x2					/* sound area map size		*/
/******** オフセット値(long word単位) ***************************************/
#define ARA_MAP_0			0x0			/* data種別,data #,area start adrs	*/
#define ARA_MAP_4			0x1			/* 転送済みbit,area size			*/
/******** ビット位置 ********************************************************/
#define B_END_MARK			31					/* data end mark bit		*/
#define B_DATA_ID			28					/* data kind id				*/
#define B_ID_NUM			24					/* data #					*/
#define B_START_ADR			0					/* start address			*/
#define B_LOAD_MARK			31					/* 転送済みbit				*/
#define B_AREA_SIZE			0					/* area size				*/
/******** マスクビット ******************************************************/
#define M_DATA_ID			(0x7  << B_DATA_ID)			/* data kind id		*/
#define M_ID_NUM			(0xf  << B_ID_NUM)			/* data #			*/
#define M_START_ADR			(0xfffff << B_START_ADR)	/* start adrs.		*/
#define M_LOAD_MARK			(0x1  << B_LOAD_MARK)		/* 転送済みbit		*/
#define M_AREA_SIZE			(0xfffff << B_AREA_SIZE)	/* area size		*/
/******* check data *********************************************************/
#define	END_MARK_DATA		0xff000000			/* end mark data			*/
/**** コマンド **************************************************************/
#define COM_START_SEQ		0x01				/* Sequence Start			*/
#define COM_STOP_SEQ		0x02				/* Sequence Stop			*/
#define COM_PAUSE_SEQ		0x03				/* Sequence Pause			*/
#define COM_CONT_SEQ		0x04				/* Sequence Continue		*/
#define COM_SET_SEQ_VL		0x05				/* Sequence Volume			*/
#define COM_CHG_TEMPO		0x07				/* Tempo Change				*/
#define COM_CHG_MAP			0x08				/* map Change				*/
#define COM_CTRL_DIR_MIDI	0x09				/* MIDI direct control		*/
#define COM_START_VL_ANL	0x0a				/* Volume analize start		*/
#define COM_STOP_VL_ANL		0x0b				/* Volume analize stop		*/
#define COM_STOP_DSP		0x0c				/* DSP stop					*/
#define COM_OFF_ALL_SOUND	0x0d				/* Sound all OFF			*/
#define COM_SET_SEQ_PAN		0x0e				/* Sequence PAN				*/
#define	COM_INIT_SOUND		0x10				/* Sound Initialize			*/
#define COM_SET_3D_SOUND	0x11				/* 3D sound parameter set	*/
#define COM_SET_QSOUND		0x12				/* Qsound parameter set		*/
#define COM_SET_3D_INIT		0x13				/* 3D sound position init	*/
#define COM_SET_TEMPO_MODE	0x14				/* tempo mode set			*/
#define COM_SET_TEMPO_RATIO	0x15				/* tempo ratio set			*/
#define COM_SET_CD_DA_LEV	0x80				/* CD-DA Level				*/
#define COM_SET_CD_DA_PAN	0x81				/* CD-DA pan				*/
#define COM_SET_TL_VL		0x82				/* Total Volume				*/
#define COM_CHG_EFCT		0x83				/* Effect Change			*/
#define COM_START_PCM		0x85				/* PCM start				*/
#define COM_STOP_PCM		0x86				/* PCM stop					*/
#define COM_CHG_MIX			0x87				/* Mixer change				*/
#define COM_CHG_MIX_PRM		0x88				/* Mixer parameter change	*/
#define COM_CHK_HARD		0x89				/* Hard check				*/
#define COM_CHG_PCM_PRM		0x8a				/* PCM parameter change		*/
#define COM_ALLOC_PCM		0x8b				/* PCM slot allocation		*/
#define COM_REL_PCM			0x8c				/* PCM slot release			*/

/* 処理マクロ */
/**** メモリライト **********************************************************/
#define POKE_B(adr, data)	(*((volatile Uint8 *)(adr)) = ((Uint8)(data)))		/* byte	*/
#define POKE_W(adr, data)	(*((volatile Uint16 *)(adr)) = ((Uint16)(data)))	/* word	*/
#define POKE_L(adr, data)	(*((volatile Uint32 *)(adr)) = ((Uint32)(data)))	/* long	*/
/**** メモリリード **********************************************************/
#define PEEK_B(adr)			(*((volatile Uint8 *)(adr)))			/* byte	*/
#define PEEK_W(adr)			(*((volatile Uint16 *)(adr)))			/* word	*/
#define PEEK_L(adr)			(*((volatile Uint32 *)(adr)))			/* long	*/
/**** 桍五椣爪舗掛折Middle SCUの制限回避 ******************************/
#define CHG_LONG(x)    (((x) * 2) + (0x4 - ( ((x) * 2) % 4) ))
/**** ホストインタフェースワークマクロ **************************************/
/**** command data max address			*/
#define MAX_ADR_COM_DATA	\
    (adr_host_int_work + ADR_COM_DATA + (SIZE_COM_BLOCK * MAX_NUM_COM_BLOCK))
/**** command data now address			*/
#define NOW_ADR_COM_DATA	(adr_com_block + ADR_COM_DATA)
/**** サウンドドライバコマンドブロック設定関数初期処理マクロ ****************/
#define HOST_SET_INIT()\
	Uint32 msk;\
	do{\
		msk = get_imask();\
		set_imask(0xf);\
	}while(FALSE)

/**** サウンドドライバコマンドブロック設定関数リターンマクロ *****************/
/* 1994/02/24 Start */
static Uint32 intrflag;
#if 0
#define HOST_SET_RETURN(ret)\
	do{\
		set_imask(msk);\
		intrflag = 0;
		return (ret);\
	}while(FALSE)
#else

/* 割り込み使用時の2重呼び出し防止フラグ */
#define HOST_SET_RETURN(ret)\
	do{\
		intrflag=0;\
		return(ret);\
	}while(FALSE)

#endif

/*** サウンドドライバへのアクセスウェートマクロ */
/*#define _WAIT_()\	*/
/*	do{\	*/
/*		int i,j;for(i=0;i<32;i++) j=*(volatile int *)0;\	*/
/*	}while(0)	*/
/* 1994/02/24 End */

/********************************************************************/
/*	function name													*/
/*		SET_COMMAND()       	- コマンドセット					*/
/*	parameters														*/
/*		(1) Uint16 set_com      - <i> コマンド						*/
/*	description														*/
/*		コマンドをインタフェース領域にセットします					*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		なし														*/
/*	caveats:														*/
/*		commandへのWRITEは16bitで行ないます							*/
/*		16bit中の上位8bitへcommandをセットします					*/
/********************************************************************/

#define SET_COMMAND(set_com)\
(POKE_W((adr_com_block + ADR_COM_DATA), (Uint16)(set_com) << 8)) /* command set   */

/********************************************************************/
/*	function name													*/
/*		SET_PRM()           	- パラメータセット					*/
/*	parameters														*/
/*		(1) Uint32 no           - <i> パラメータ番号				*/
/*		(2) Uint8 set_prm       - <i> パラメータ					*/
/*	description														*/
/*		パラメータをインタフェース領域にセットします				*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		なし														*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

#define SET_PRM(no, set_prm)\
(POKE_B(adr_com_block + ADR_PRM_DATA + (no), (set_prm))) /* parameter set	*/

/*
 * STATIC DECLARATIONS
 */ 
static volatile Uint8 *adr_sys_info_tbl;                 /* system 情報table address 格納    */
static volatile Uint8 *adr_host_int_work;                /* host i/f work 先頭address 格納*/
static volatile Uint32 *adr_snd_area_crnt;             /* sound area map CRNT work 先頭address */
static volatile Uint16 *adr_song_stat;                   /* song status               */
static volatile Uint16 *adr_tl_vl;                       /* Total volume              */
static volatile Uint16 *adr_tl_hz_vl;                    /* 周波数帯域別Volume        */
static volatile Uint16 *adr_pcm;                         /* PCM                       */
static volatile Uint16 *adr_seq;                         /* Sequence                  */
static volatile Uint8  *adr_com_block;                   /* 現在書き込みcommand block      */

/*
 * STATIC FUNCTION PROTOTYPE DECLARATIONS
 */
static void DmaClrZero(void *, Uint32);
static void GetSndMapInfo(void **, Uint32 **, Uint16, Uint16);
static Uint16 ChgPan(SndPan);
static void CopyMem(void *,void *,Uint32);
static Uint8 GetComBlockAdr(void);

/********************************************************************/
/*	function name													*/
/*		SET_TIMING()			- タイミングフラグ書き込み			*/
/*	parameters														*/
/*		なし														*/
/*	description														*/
/*		タイミングフラグハンドシェークモードであれば，				*/
/*		タイミングフラグを書き込む									*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		なし														*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/
void SND_TIMING()
{
	if (PEEK_B(ADR_SYS_TBL + ADR_CMD_MODE) & 0x80){
		POKE_B((ADR_SYS_TBL + ADR_TIMING_FLAG), (Uint8)0x80 );
	}else{
		while(PEEK_W(adr_com_block + ADR_COM_DATA));
	}
}

/********************************************************************/
/*	function name													*/
/*		SND_Init()				- サウンドシステム起動				*/
/*	parameters														*/
/*		(1) SndIniDt *sys_ini	- <i> システム起動用データ			*/
/*	description														*/
/*		システム起動用データに従って，サウンドシステムを起動する	*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		なし														*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

void SND_Init(SndIniDt *sys_ini)
{
    /** BEGIN ****************************************************************/
#ifdef _DMA_SCU
    DMA_ScuInit();                              /* DMA SCU初期化処理         */
#endif	/* _DMA_SCU */

    /* サウンドOFF               */
#ifdef USE_SGL      /* Ver.1.31  SGL対応 */
    slSoundOffWait();
#else 
    PER_SMPC_SND_OFF();
#endif
    /* SCSP共通レジスタ設定        */
    POKE_W(ADR_SCSP_REG, SCSP_REG_SET); 
    /* DMAメモリゼロクリア       */
    DmaClrZero(ADR_SND_MEM, MEM_CLR_SIZE);
	/* 68K program 転送							->($25a00000)	*/
    CopyMem(ADR_SND_VECTOR,(void *)(SND_INI_PRG_ADR(*sys_ini)),SND_INI_PRG_SZ(*sys_ini));
	/* system 情報table address 取得			= $25a00440		*/
    adr_sys_info_tbl = (Uint8 *)(ADR_SND_MEM + PEEK_L(ADR_SYS_TBL + ADR_SYS_INFO));
	/* host i/f work address 取得				= $25a00700		*/
	adr_host_int_work = (Uint8 *)(ADR_SND_MEM + PEEK_L(ADR_SYS_TBL + ADR_HOST_INT));
	/* system i/f work address 取得				= $25a00480		*/
	snd_adr_sys_int_work = (Uint8 *)(ADR_SND_MEM + 
				((Uint32)PEEK_W(ADR_SYS_TBL + ADR_SYS_INT_WORK) << 16
				| (Uint32)PEEK_W(ADR_SYS_TBL + ADR_SYS_INT_WORK + 2)));
	/* 現在書き込みcommand blockaddress 初期化	= $25a00700		*/
	adr_com_block = adr_host_int_work;
	/* sound area map CRNTwork 取得				= $25a00500		*/
	adr_snd_area_crnt = (Uint32 *)(ADR_SND_MEM + PEEK_L(ADR_SYS_TBL + ADR_ARA_CRNT));
	/* sequence mode status						= $25a00780		*/
	adr_song_stat = (Uint16 *)(adr_host_int_work + ADR_SONG_STAT);
	/* pcm stream play#0 address				= $25a007a0		*/
	adr_pcm = (Uint16 *)(adr_host_int_work + ADR_PCM);
	/* sequence play#0 address					= $25a007b0		*/
	adr_seq = (Uint16 *)(adr_host_int_work + ADR_SEQ);
	/* digital audio input level left			= $25a00790		*/
	adr_tl_vl = (Uint16 *)(adr_host_int_work + ADR_TL_VL);
	/* treble input level left					= $25a00794		*/
	adr_tl_hz_vl = (Uint16 *)(adr_host_int_work + ADR_TL_HZ_VL);
	/* sound area map 転送						->($25a0a000)	*/
	CopyMem((void *)(PEEK_L(adr_sys_info_tbl + ADR_ARA_ADR) + ADR_SND_MEM),
			(void *)(SND_INI_ARA_ADR(*sys_ini)),CHG_LONG(SND_INI_ARA_SZ(*sys_ini)));
#ifdef USE_SGL      /* Ver.1.31  SGL対応 */
    slSoundOnWait();      /* サウンドON                */
    {       /* サウンドCPU起動後のウェイト */
    	volatile int tmp, count;
    	count = 1000L;
    	while(count--){
    		tmp = *(volatile int*)0x20200000;
    	}
    }
#else 
	PER_SMPC_SND_ON();    /* サウンドON                */
#endif
    intrflag = 0;         /* 割り込みフラグの初期化 */
}

/********************************************************************/
/*	function name													*/
/*		SND_MoveData()  		- サウンドデータ転送				*/
/*	parameters														*/
/*		(1) Uint16 *source      - <i> サウンドデータ転送元アドレス	*/
/*		(2) Uint32 size         - <i> サウンドデータ転送サイズ		*/
/*		(3) Uint16 data_kind    - <i> データ種別					*/
/*		(4) Uint16 data_no      - <i> データ番号					*/
/*	description														*/
/*		サウンドデータをサウンドメモリへ転送します					*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		なし														*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

void SND_MoveData(Uint16 *source, Uint32 size,Uint16 data_kind, Uint16 data_no)
{
	void *adr;                                  /* 転送先アドレス            */
	Uint32 *load_mark_adr;                      /* 転送済みビット設定address   */

	GetSndMapInfo(&adr, &load_mark_adr, data_kind, data_no);
                                                /* sound area map 情報取得      */

	CopyMem(adr, (void *)source, size);         /* sound area map 転送         */
	POKE_L(load_mark_adr, (*load_mark_adr | M_LOAD_MARK));
                                                /* 転送済みbit on            */
}

/********************************************************************/
/*	function name													*/
/*		SND_ChgMap()			- サウンドエリアマップ変更			*/
/*	parameters														*/
/*		(1) SndAreaMap area_no	- <i> サウンドエリアマップ番号		*/
/*	description														*/
/*		サウンドエリアマップを変更します							*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

SndRet SND_ChgMap(SndAreaMap area_no)
{
/* 1994/02/24 Start */
#if 0
	HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
	if(intrflag) return(SND_RET_NSET);
	intrflag = 1;
/* 1994/02/24 End */
	if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
	SET_PRM(0, area_no);					/* parameter set	*/
	SET_COMMAND(COM_CHG_MAP);				/* command set		*/
	SND_TIMING();

	if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
	SET_PRM(0, area_no);					/* parameter set	*/
	SET_COMMAND(COM_CHG_MAP);				/* command set		*/
	SND_TIMING();

	HOST_SET_RETURN(SND_RET_SET);
}
/********************************************************************/
/*	function name													*/
/*		SND_SetTlVl()			- 全体音量設定						*/
/*	parameters														*/
/*		(1) SndTlVl vol			- <i> 全体音量						*/
/*	description														*/
/*		全体音量を設定します										*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

SndRet SND_SetTlVl(SndTlVl vol)
{
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, vol);						/* parameter set	*/
    SET_COMMAND(COM_SET_TL_VL);				/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_ChgEfct()			- エフェクト変更					*/
/*	parameters														*/
/*		(1) SndEfctBnkNum efct_no									*/
/*								- <i>   Effect bank number			*/
/*	description														*/
/*		エフェクトを変更します										*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

SndRet SND_ChgEfct(SndEfctBnkNum efct_no)
{
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, efct_no);					/* parameter set	*/
    SET_COMMAND(COM_CHG_EFCT);				/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_ChgMix()			- ミキサ変更						*/
/*	PARAMETERS														*/
/*		(1) SndToneBnkNum tone_no									*/
/*								- <i> 音色 bank number				*/
/*		(2) SndMixBnkNum mix_no - <i> MIxer bank number				*/
/*	DESCRIPTION														*/
/*		ミキサを変更します											*/
/*	PRECONDITIONS													*/
/*		なし														*/
/*	POSTCONDITIONS													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	CAVEATS															*/
/*		なし														*/
/********************************************************************/

SndRet SND_ChgMix(SndToneBnkNum tone_no, SndMixBnkNum mix_no)
{
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, tone_no);					/* parameter set	*/
    SET_PRM(1, mix_no);						/* parameter set	*/
    SET_COMMAND(COM_CHG_MIX);				/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_ChgMixPrm()			- ミキサパラメータ変更				*/
/*	parameters														*/
/*		(1) SndEfctOut efct_out	- <i> Effect out select				*/
/*		(2) SndLev level		- <i> Effect return Level			*/
/*		(3) SndPan pan			- <i> Effect pan					*/
/*	description														*/
/*		ミキサパラメータを変更します								*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

SndRet SND_ChgMixPrm(SndEfctOut efct_out, SndLev level, SndPan pan)
{
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, efct_out);					/* parameter set	*/
    SET_PRM(1, ChgPan(pan) | (level << 5));	/* parameter set	*/
    SET_COMMAND(COM_CHG_MIX_PRM);			/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_ChkHard()			- ハードウェアチェック				*/
/*	parameters														*/
/*		(1) SndHardStat *stat	- <o> ハードチェックステータス		*/
/*		(2) SndHardPrm prm		- <i> ハードチェックパラメータ		*/
/*	description														*/
/*		ハードウェアをチェックします								*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

SndRet SND_ChkHard(SndHardStat *stat, SndHardPrm prm)
{
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    *(Uint16 *)(ADR_SYS_TBL + ADR_HARD_CHK_STAT) = 0;
    SET_PRM(0, prm);						/* parameter set	*/
    SET_COMMAND(COM_CHK_HARD);				/* command set		*/
    while((*stat = *(volatile Uint16 *)(ADR_SYS_TBL + ADR_HARD_CHK_STAT)) == 0);	/* _WAIT_();	*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_StopDsp()			- DSP中止							*/
/*	parameters														*/
/*		なし														*/
/*	description														*/
/*		DSPを中止します												*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

SndRet SND_StopDsp(void)
{
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
	SET_PRM(0, 0);					/* sequence don't stop	*/
	SET_PRM(1, 0);					/* pcm don't stop		*/
	SET_PRM(2, 0);					/* cd don't stop		*/
	SET_PRM(3, 1);					/* dsp initiarize		*/
	SET_PRM(4, 0);					/* mixer not initialize	*/
	SET_COMMAND(COM_INIT_SOUND);			/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_OffAllSound()		- 全サウンド発音スロット停止		*/
/*	parameters														*/
/*		なし														*/
/*	description														*/
/*		全サウンド発音スロットを停止します							*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*      (1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

SndRet SND_OffAllSound(void)
{
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
	SET_PRM(0, 1);					/* all sequence stop	*/
	SET_PRM(1, 0);					/* pcm don't stop		*/
	SET_PRM(2, 0);					/* cd don't stop		*/
	SET_PRM(3, 0);					/* dsp not initiarize	*/
	SET_PRM(4, 0);					/* mixer not initialize	*/
	SET_COMMAND(COM_INIT_SOUND);			/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_StartSeq()			- シーケンス開始					*/
/* PARAMETERS														*/
/*      (1) SndSeqNum seq_no	- <i> 発音管理番号					*/
/*      (2) SndSeqBnkNum seq_bk_no									*/
/*								- <i> Sequence bank number			*/
/*      (3) SndSeqSongNum song_no									*/
/*								- <i> Sequence song number			*/
/*      (4) SndSeqPri pri_lev	- <i> Priorty level					*/
/* DESCRIPTION														*/
/*      シーケンスを開始します										*/
/* PRECONDITIONS													*/
/*      なし														*/
/* POSTCONDITIONS													*/
/*      (1) SndRet				- <o> コマンド実行状態				*/
/* CAVEATS															*/
/*      なし														*/
/********************************************************************/

SndRet SND_StartSeq(SndSeqNum seq_no, SndSeqBnkNum seq_bk_no,
                    SndSeqSongNum song_no, SndSeqPri pri_lev)
{
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, seq_no);						/* parameter set	*/
    SET_PRM(1, seq_bk_no);					/* parameter set	*/
    SET_PRM(2, song_no);					/* parameter set	*/
    SET_PRM(3, pri_lev);					/* parameter set	*/
    SET_COMMAND(COM_START_SEQ);				/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_StopSeq()			- シーケンス中止					*/
/*	parameters														*/
/*		(1) SndSeqNum seq_no	- <i> 発音管理番号					*/
/*	description														*/
/*		シーケンスを中止します										*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

SndRet SND_StopSeq(SndSeqNum seq_no)
{
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, seq_no);						/* parameter set	*/
    SET_COMMAND(COM_STOP_SEQ);				/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_PauseSeq()			- シーケンス一時中断				*/
/*	parameters														*/
/*		(1) SndSeqNum seq_no	- <i> 発音管理番号					*/
/*	description														*/
/*		シーケンスを一時中断します									*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/
 
 SndRet SND_PauseSeq(SndSeqNum seq_no)
 {
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, seq_no);						/* parameter set	*/
    SET_COMMAND(COM_PAUSE_SEQ);				/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_ContSeq()			- シーケンス一時中断解除			*/
/*	parameters														*/
/*		(1) SndSeqNum seq_no	- <i> 発音管理番号					*/
/*	description														*/
/*		シーケンスの一時中断を解除します							*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/
 
 SndRet SND_ContSeq(SndSeqNum seq_no)
 {
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, seq_no);						/* parameter set	*/
    SET_COMMAND(COM_CONT_SEQ);				/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_SetSeqVl()			- シーケンス音量設定				*/
/*	parameters														*/
/*		(1) SndSeqNum seq_no	- <i> 発音管理番号					*/
/*		(2) SndSeqVl seq_vl		- <i> Sequence Volume				*/
/*		(3) SndFade fade		- <i> fade Rate						*/
/*	description														*/
/*      シーケンスの音量を設定します								*/
/*	preconditions													*/
/*      なし														*/
/*	postconditions													*/
/*      (1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*      なし														*/
/********************************************************************/

SndRet SND_SetSeqVl(SndSeqNum seq_no, SndSeqVl seq_vl, SndFade fade)
{
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, seq_no);						/* parameter set	*/
    SET_PRM(1, seq_vl);						/* parameter set	*/
    SET_PRM(2, fade);						/* parameter set	*/
    SET_COMMAND(COM_SET_SEQ_VL);			/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_ChgTempo()			- テンポ変更						*/
/*	parameters														*/
/*      (1) SndSeqNum seq_no	- <i> 発音管理番号					*/
/*      (2) SndTempo tempo		- <i> Tempo							*/
/*	description														*/
/*		シーケンスのテンポを変更します								*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/
 
 SndRet SND_ChgTempo(SndSeqNum seq_no, SndTempo tempo)
 {
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, seq_no);						/* parameter set	*/
    SET_PRM(2, tempo >> 8);					/* parameter set	*/
    SET_PRM(3, tempo);						/* parameter set	*/
    SET_COMMAND(COM_CHG_TEMPO);				/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_CtrlDirMidi()		- MIDIダイレクトコントロール		*/
/*	parameters														*/
/*		(1) SndSeqNum seq_no	- <i> 発音管理番号					*/
/*		(2) SndSeqPri seq_pri	- <i> Priorty Level					*/
/*		(3) Uint8 md_com		- <i> MIDI command					*/
/*		(4) Uint8 ch			- <i> MIDI channel					*/
/*		(5) Uint8 dt1			- <i> MIDI data1					*/
/*		(6) Uint8 dt2			- <i> MIDI data2					*/
/*	description														*/
/*		MIDIをダイレクトにコントロールします						*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

SndRet SND_CtrlDirMidi(SndSeqNum seq_no, SndSeqPri seq_pri, Uint8 md_com,
                        Uint8 ch, Uint8 dt1, Uint8 dt2)
{
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, (seq_pri << 3) | md_com);	/* parameter set	*/
    SET_PRM(1, (seq_no << 5) | ch);			/* parameter set	*/
    SET_PRM(2, dt1);						/* parameter set	*/
    SET_PRM(3, dt2);						/* parameter set	*/
    SET_COMMAND(COM_CTRL_DIR_MIDI);			/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_SetSeqPan()			- シーケンスPAN設定					*/
/*	parameters														*/
/*		(1) SndSeqNum seq_no	- <i> 発音管理番号					*/
/*		(2) Uint8 ctrl_sw		- <i> Control ON/OFF				*/
/*		(3) Uint8 md_pan		- <i> MIDI PAN data					*/
/*	description														*/
/*		シーケンスPANを設定します									*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/
 
 SndRet SND_SetSeqPan(SndSeqNum seq_no, Uint8 ctrl_sw, Uint8 md_pan)
 {
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();			/* host i/f area 設定初期処理	*/
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, seq_no);						/* parametor set	*/
    SET_PRM(1, (ctrl_sw | md_pan));			/* parametor set	*/
    SET_COMMAND(COM_SET_SEQ_PAN);			/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}
/********************************************************************/
/*	function name													*/
/*		SND_StartPcm()			- PCM開始						*/
/*	parameters														*/
/*      (1) SndPcmStartPrm *sprm									*/
/*								- <i> PCM開始パラメータポインタ		*/
/*      (2) SndPcmChgPrm *cprm	- <i> PCM変更パラメータポインタ		*/
/*	description														*/
/*		PCMデータを再生します										*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/
SndRet SND_StartPcm(SndPcmStartPrm *sprm, SndPcmChgPrm *cprm)
{
	SND_PRM_TL(*cprm) = 0;

	return SND_StartPcmTL( sprm, cprm );
}

/********************************************************************/
/*	function name													*/
/*		SND_StartPcmTL()			- PCM開始						*/
/*	parameters														*/
/*      (1) SndPcmStartPrm *sprm									*/
/*								- <i> PCM開始パラメータポインタ		*/
/*      (2) SndPcmChgPrm *cprm	- <i> PCM変更パラメータポインタ		*/
/*	description														*/
/*		PCMデータを再生します										*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		Ver.1.31  より追加											*/
/********************************************************************/

SndRet SND_StartPcmTL(SndPcmStartPrm *sprm, SndPcmChgPrm *cprm)
{
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();			/* host i/f area 設定初期処理	*/
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, SND_PRM_MODE(*sprm) | SND_PRM_NUM(*cprm));
    SET_PRM(1, (SND_PRM_LEV(*cprm) << 5) | ChgPan(SND_PRM_PAN(*cprm)));
    SET_PRM(2, SND_PRM_SADR(*sprm) >> 8);
    SET_PRM(3, SND_PRM_SADR(*sprm));
    SET_PRM(4, SND_PRM_SIZE(*sprm) >> 8);
    SET_PRM(5, SND_PRM_SIZE(*sprm));
    SET_PRM(6, SND_PRM_PICH(*cprm) >> 8);
    SET_PRM(7, SND_PRM_PICH(*cprm));
    SET_PRM(8, (SND_R_EFCT_IN(*cprm) << 3) | SND_R_EFCT_LEV(*cprm));
    SET_PRM(9, (SND_L_EFCT_IN(*cprm) << 3) | SND_L_EFCT_LEV(*cprm));
    SET_PRM(10, SND_PRM_TL(*cprm));
    SET_PRM(11, 0);
    SET_COMMAND(COM_START_PCM);				/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_StopPcm()			- PCM中止							*/
/*	parameters														*/
/*		(1) SndPcmNum pcm_num	- <i> 再生停止PCMストリーム再生番号	*/
/*	description														*/
/*		PCMを中止します												*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/
 
 SndRet SND_StopPcm(SndPcmNum pcm_num)
 {
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();			/* host i/f area 設定初期処理	*/
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, pcm_num);					/* parameter set	*/
    SET_COMMAND(COM_STOP_PCM);				/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_ChgPcm()			- PCM変更							*/
/*	parameters														*/
/*		(1) SndPcmChgPrm *cprm	- <i> PCM変更パラメータポインタ		*/
/*	description														*/
/*		PCMデータを再生します										*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/
SndRet SND_ChgPcm(SndPcmChgPrm *cprm)
{
	SND_PRM_TL(*cprm) = 0;   /* トータルレベルを 0に設定(減衰 0) */
	return SND_ChgPcmTL( cprm );
}

/********************************************************************/
/*	function name													*/
/*		SND_ChgPcmTL()			- PCM変更							*/
/*	parameters														*/
/*		(1) SndPcmChgPrm *cprm	- <i> PCM変更パラメータポインタ		*/
/*	description														*/
/*		PCMデータを再生します										*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		Ver.1.31  より追加											*/
/********************************************************************/
 
 SndRet SND_ChgPcmTL(SndPcmChgPrm *cprm)
 {
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();			/* host i/f area 設定初期処理	*/
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, SND_PRM_NUM(*cprm));
    SET_PRM(1, (SND_PRM_LEV(*cprm) << 5) | ChgPan(SND_PRM_PAN(*cprm)));
    SET_PRM(2, SND_PRM_PICH(*cprm) >> 8);
    SET_PRM(3, SND_PRM_PICH(*cprm));
    SET_PRM(4, (SND_R_EFCT_IN(*cprm) << 3) | SND_R_EFCT_LEV(*cprm));
    SET_PRM(5, (SND_L_EFCT_IN(*cprm) << 3) | SND_L_EFCT_LEV(*cprm));
    SET_PRM(6, SND_PRM_TL(*cprm));
    SET_COMMAND(COM_CHG_PCM_PRM);			/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_AllocPcm()			- PCMスロットの確保					*/
/*	parameters														*/
/*      (1) SndPcmChgPrm *cprm	- <i> PCMパラメータポインタ			*/
/*	description														*/
/*		ストリーム用スロットを確保します							*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

SndRet SND_AllocPcm(SndPcmChgPrm *cprm)
{
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, SND_PRM_NUM(*cprm));
    SET_COMMAND(COM_ALLOC_PCM);				/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_ReleasePcm()		- PCMスロットの開放					*/
/*	parameters														*/
/*      (1) SndPcmChgPrm *cprm	- <i> PCMパラメータポインタ			*/
/*	description														*/
/*		ストリーム用スロットを開放します							*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

SndRet SND_ReleasePcm(SndPcmChgPrm *cprm)
{
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, SND_PRM_NUM(*cprm));
    SET_COMMAND(COM_REL_PCM);				/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_StartVlAnl()		- 音量解析開始						*/
/*	parameters														*/
/*		なし														*/
/*	description														*/
/*		音量解析を行います											*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		周波数帯域別音量解析をする場合は，エフェクト変更にて		*/
/*		専用のDSPプログラムを実行してください						*/
/********************************************************************/
 
 SndRet SND_StartVlAnl(void)
 {
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();			/* host i/f area 設定初期処理	*/
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_COMMAND(COM_START_VL_ANL);			/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_StopVlAnl()			- 音量解析中止						*/
/*	parameters														*/
/*		なし														*/
/*	description														*/
/*		音量解析を中止します										*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

 SndRet SND_StopVlAnl(void)
 {
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_COMMAND(COM_STOP_VL_ANL);               /* command set            */
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_SetCdDaLev()		- CD-DA Level設定					*/
/*	parameters														*/
/*		(1) SndLev left			- <i> 左出力の音量					*/
/*		(2) SndLev right		- <i> 右出力の音量					*/
/*	description														*/
/*		現在のステレオ音量を変更します								*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/
 
 SndRet SND_SetCdDaLev(SndLev left, SndLev right)
 {
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, (left) * 2 << 4);			/* parameter set	*/
    SET_PRM(1, (right) * 2 << 4);			/* parameter set	*/
    SET_COMMAND(COM_SET_CD_DA_LEV);			/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_SetCdDaPan()		- CD-DA Pan設定						*/
/*	parameters														*/
/*		(1) SndPan	left		- <i> 左出力のPAN					*/
/*		(2) SndPan	right		- <i> 右出力のPAN					*/
/*	description														*/
/*		現在のステレオPANを変更します								*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

SndRet SND_SetCdDaPan(SndPan left, SndPan right)
{
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* host i/f area 設定初期処理 */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, ChgPan(left));				/* parameter set	*/
    SET_PRM(1, ChgPan(right));				/* parameter set	*/
    SET_COMMAND(COM_SET_CD_DA_PAN);			/* command set		*/
    HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_GetSeqStat()		- シーケンスステータス取得			*/
/*	parameters														*/
/*		(1) SndSeqStat *status	- <o> シーケンスステータスポインタ	*/
/*		(2) SndSeqNum seq_no	- <i> 発音管理番号					*/
/*	description														*/
/*		シーケンスステータスを取得します							*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		なし														*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/
 
 void SND_GetSeqStat(SndSeqStat *status, SndSeqNum seq_no)
 {
    SND_SEQ_STAT_MODE(*status) = (PEEK_W(adr_song_stat + seq_no)) & 0xff00;
    SND_SEQ_STAT_STAT(*status) = (Uint8)(PEEK_W(adr_song_stat + seq_no));
}

/********************************************************************/
/*	function name													*/
/*		SND_GetSeqPlayPos()		- 発音管理番号再生位置取得			*/
/*	parameters														*/
/*		(1) SndSeqPlayPos *pos	- <o> 発音管理番号再生位置			*/
/*		(2) SndSeqNum seq_no	- <i> 発音管理番号					*/
/*	description														*/
/*		発音管理番号再生位置を取得します							*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		なし														*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/
 
 void SND_GetSeqPlayPos(SndSeqPlayPos *pos, SndSeqNum seq_no)
 {
    *pos = PEEK_W(adr_seq + seq_no);
}

/********************************************************************/
/*	function name													*/
/*		SND_GetPcmPlayAdr()		- PCM実行アドレス取得				*/
/*	parameters														*/
/*		(1) SndPcmPlayAdr *pcm_adr									*/
/*								- <o> PCM実行アドレス				*/
/*		(2) SndPcmNum num		- <i> PCM再生番号					*/
/*	description														*/
/*		PCM実行アドレスを取得します									*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		なし														*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/
 
 void SND_GetPcmPlayAdr(SndPcmPlayAdr *pcm_adr, SndPcmNum num)
 {
    SND_PCM_RADR(*pcm_adr) = (Uint8)(PEEK_W(adr_pcm + num) >> 8);
    SND_PCM_LADR(*pcm_adr) = (Uint8)PEEK_W(adr_pcm + num);
 }

/********************************************************************/
/*	function name													*/
/*		SND_GetAnlTlVl()		- 解析全体ステレオ音量取得			*/
/*	parameters														*/
/*		(1) SndCdVlAnl *left	- <o> 左出力の全体解析音量			*/
/*		(2) SndCdVlAnl *right	- <o> 右出力の全体解析音量			*/
/*	description														*/
/*		全体ステレオ解析音量を取得します							*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		なし														*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

void SND_GetAnlTlVl(SndCdVlAnl *left, SndCdVlAnl *right)
{
    *left = (SndCdVlAnl)PEEK_W(adr_tl_vl);
    *right = (SndCdVlAnl)PEEK_W(adr_tl_vl + 1);
}

/********************************************************************/
/*	function name													*/
/*		SND_GetAnlHzVl()		- 解析周波数帯域別ステレオ音量取得	*/
/*	parameters														*/
/*		(1) SndCdHzSrVl *hz_vl	- <o> 周波数帯域別ステレオ解析音量	*/
/*	description														*/
/*		周波数帯域別ステレオ解析音量を取得します					*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		なし														*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/
 
void SND_GetAnlHzVl(SndCdHzSrVl *hz_vl)
{
	SND_CD_LHIGH(*hz_vl) = (SndCdVlAnl)PEEK_W(adr_tl_hz_vl + 0);
	SND_CD_RHIGH(*hz_vl) = (SndCdVlAnl)PEEK_W(adr_tl_hz_vl + 1);
	SND_CD_LMID(*hz_vl) = (SndCdVlAnl)PEEK_W(adr_tl_hz_vl + 2);
	SND_CD_RMID(*hz_vl) = (SndCdVlAnl)PEEK_W(adr_tl_hz_vl + 3);
	SND_CD_LLOW(*hz_vl) = (SndCdVlAnl)PEEK_W(adr_tl_hz_vl + 4);
	SND_CD_RLOW(*hz_vl) = (SndCdVlAnl)PEEK_W(adr_tl_hz_vl + 5);
}

/********************************************************************/
/*	function name													*/
/*		SND_SetQSound()			-	QSOUND							*/
/*	parameters														*/
/*		(1) Uint8 QCh			- <i> QSOUND Channel				*/
/*		(2) Uint8 QSt			- <i> QSOUND Status					*/
/*	description														*/
/*		QSOUND status set											*/
/*	preconditions													*/
/*		none														*/
/*	postconditions													*/
/*		(1) SndRet              - <o> コマンド実行状態				*/
/*	caveats															*/
/*		none														*/
/********************************************************************/

 SndRet SND_SetQSound(Uint8 QCh, Uint8 QSt)
 {
#if 0
	HOST_SET_INIT();				/* host i/f area set init. process */
#endif
	if(intrflag) return(SND_RET_NSET);
	intrflag = 1;
	if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
	SET_PRM(0, QCh);						/* parameter set	*/
	SET_PRM(1, QSt);						/* parameter set	*/
	SET_COMMAND(COM_SET_QSOUND);			/* command set		*/
	HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_Set3D_Stat()		- Ｙサウンド定位設定				*/
/*	parameters														*/
/*		(1) Uint8 Y3DDst		- <i> Distance						*/
/*		(2) Uint8 Y3DAzm		- <i> Azimuth						*/
/*		(3) Uint8 Y3DElv		- <i> Elevation						*/
/*	description														*/
/*		yamaha 3d status set										*/
/*	preconditions													*/
/*		none														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		none														*/
/********************************************************************/
 SndRet SND_Set3D_Stat(Uint8 Y3DDst, Uint8 Y3DAzm, Uint8 Y3DElv)
 {
#if 0
	HOST_SET_INIT();				/* host i/f area set init. process */
#endif
	if(intrflag) return(SND_RET_NSET);
	intrflag = 1;
	if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
	SET_PRM(0, Y3DDst);						/* parameter set	*/
	SET_PRM(1, Y3DAzm);						/* parameter set	*/
	SET_PRM(2, Y3DElv);						/* parameter set	*/
	SET_COMMAND(COM_SET_3D_SOUND);			/* command set		*/
	HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_Set3D_Init()		- Ｙサウンド初期化					*/
/*	parameters														*/
/*		none														*/
/*	description														*/
/*		yamaha 3d status initialize									*/
/*	preconditions													*/
/*		none														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		none														*/
/********************************************************************/
 SndRet SND_Set3D_Init()
 {
#if 0
	HOST_SET_INIT();				/* host i/f area set init. process */
#endif
	if(intrflag) return(SND_RET_NSET);
	intrflag = 1;
	if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
	SET_COMMAND(COM_SET_3D_INIT);	/* command set            */
	HOST_SET_RETURN(SND_RET_SET);
}

/********************************************************************/
/*	function name													*/
/*		SND_Init_Sound()		- サウンド初期化					*/
/*	parameters														*/
/*		(1) Uint8 SeqStop		- <i> All sequence stop				*/
/*		(2) Uint8 PcmStop		- <i> All pcm stream stop			*/
/*		(3) Uint8 CdStop		- <i> Cd-da stop					*/
/*		(4) Uint8 DspInit		- <i> Dsp initialize				*/
/*		(5) Uint8 MixInit		- <i> Mixer initialize				*/
/*	description														*/
/*		Sound initialize											*/
/*	preconditions													*/
/*		none														*/
/*	postconditions													*/
/*		(1) SndRet				- <o> コマンド実行状態				*/
/*	caveats															*/
/*		none														*/
/********************************************************************/
SndRet SND_Init_Sound(Uint8 SeqStop,Uint8 PcmStop,Uint8 CdStop,Uint8 DspInit,Uint8 MixInit)
{
	SET_PRM(0, SeqStop);					/* parameter set	*/
	SET_PRM(1, PcmStop);					/* parameter set	*/
	SET_PRM(2, CdStop);						/* parameter set	*/
	SET_PRM(3, DspInit);					/* parameter set	*/
	SET_PRM(4, MixInit);					/* parameter set	*/
	SET_COMMAND(COM_INIT_SOUND);			/* command set		*/
	HOST_SET_RETURN(SND_RET_SET);
}
/********************************************************************/
/*	function name													*/
/*		SND_SetTempoMode()		-	tempo mode set					*/
/*	parameters														*/
/*		(1) SndSeqNum seq_no	- <i> 発音管理番号					*/
/*		(2) Uint8 tmpMode		- <i> tempo mode					*/
/*	description														*/
/*		テンポ処理のモードを変更します								*/
/*	preconditions													*/
/*		none														*/
/*	postconditions													*/
/*		(1) SndRet              - <o> コマンド実行状態				*/
/*	caveats															*/
/*		none														*/
/********************************************************************/

SndRet SND_SetTempoMode(SndSeqNum seq_no, Uint8 tmpMode)
 {
	if(intrflag) return(SND_RET_NSET);
	intrflag = 1;
	if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
	SET_PRM(0, seq_no);						/* parameter set	*/
    SET_PRM(1, tmpMode);					/* parameter set	*/
	SET_COMMAND(COM_SET_TEMPO_MODE);		/* command set		*/
	HOST_SET_RETURN(SND_RET_SET);
}
/********************************************************************/
/*	function name													*/
/*		SND_SetTempoRatio()		-	tempo ratio set					*/
/*	parameters														*/
/*		(1) SndSeqNum seq_no	- <i> 発音管理番号					*/
/*		(2) Uint16 tmpRatio		- <i> tempo ratio					*/
/*	description														*/
/*		相対テンポ変更												*/
/*	preconditions													*/
/*		none														*/
/*	postconditions													*/
/*		(1) SndRet              - <o> コマンド実行状態				*/
/*	caveats															*/
/*		none														*/
/********************************************************************/

SndRet SND_SetTempoRatio(SndSeqNum seq_no, Uint8 tmpRatio)
 {
	if(intrflag) return(SND_RET_NSET);
	intrflag = 1;
	if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
	SET_PRM(0, seq_no);						/* parameter set	*/
    SET_PRM(2, tmpRatio >> 8);				/* parameter set	*/
    SET_PRM(3, tmpRatio);					/* parameter set	*/
	SET_COMMAND(COM_SET_TEMPO_RATIO);		/* command set		*/
	HOST_SET_RETURN(SND_RET_SET);
}
/************************************************************************/
/*		内部提供関数													*/
/*		< functions list >												*/
/*		DmaClrZero()			- DMAによるメモリのゼロクリア			*/
/*		GetSndMapInfo()			- サウンドエリアマップ情報取得			*/
/*		ChgPan()				- PANデータ変換							*/
/*		CopyMem()				- メモリコピ－							*/
/*		GetComBlockAdr()		- コマンドブロック						*/
/************************************************************************/

/********************************************************************/
/*	function name													*/
/*		DmaClrZero()			- DMAによるメモリのゼロクリア		*/
/*	parameters														*/
/*		(1) void *dst			- <i> ディスティネーションアドレス	*/
/*		(2) Uint32 cnt			- <i> クリアバイト数				*/
/*	description														*/
/*		DMAを使用してメモリをゼロクリアします						*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		なし														*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/

static void DmaClrZero(void *dst, Uint32 cnt)
{
    memset(dst, 0x00, cnt);
}

/********************************************************************/
/*	function name													*/
/*		GetSndMapInfo()			- サウンドエリアマップ情報取得		*/
/*	parameters														*/
/*		(1) void **adr			- <o> 転送先アドレス				*/
/*		(2) Uint32 **ladr		- <o> 転送済みビットアドレス		*/
/*		(3) Uint16 data_kind	- <i> データ種別					*/
/*		(4) Uint16 data_no		- <i> データ番号					*/
/*	description														*/
/*		サウンドエリアマップの情報を取得します						*/
/*	preconditions													*/
/*		なし														*/
/*	postconditions													*/
/*		なし														*/
/*	caveats															*/
/*		マップ末の識別はエンドビットではなくエンドデータに変更		*/
/********************************************************************/

static void GetSndMapInfo(void **adr, Uint32 **ladr, Uint16 data_kind, Uint16 data_no)
{
    Uint32 i = 0;
    Uint32 map0;

	map0 = PEEK_L(adr_snd_area_crnt + ARA_MAP_0);
	for(i = 1; (map0 & END_MARK_DATA) != END_MARK_DATA; i++){
		if((((map0 & M_DATA_ID) >> B_DATA_ID) == (Uint32)data_kind) &&
			(((map0 & M_ID_NUM) >> B_ID_NUM) == (Uint32)data_no)){
			/* start address read from map file						*/
			*adr = (void *)(ADR_SND_MEM + ((map0 & M_START_ADR) >> B_START_ADR));
			/* data trans. flag read from map file					*/
			*ladr = (Uint32 *)(adr_snd_area_crnt + ARA_MAP_SIZE * (i - 1) + ARA_MAP_4);
			break;
		}
		map0 = PEEK_L(adr_snd_area_crnt + ARA_MAP_SIZE * i + ARA_MAP_0);
	}
}
/********************************************************************/
/*	function name													*/
/*		ChgPan()				- PANデータ変換						*/
/*	parameters														*/
/*		(1) SndPan pan			- <i> PAN							*/
/*	description														*/
/*		登録用のPANに変換します										*/
/*	preconditions													*/
/*		(1) Uint16				- <o> 変換後PAN						*/
/*	postconditions													*/
/*		なし														*/
/*	caveats															*/
/*		なし														*/
/********************************************************************/
static Uint16 ChgPan(SndPan pan)
{
    return(((pan) < 0) ? (~(pan) + 0x10 + 1) : (pan));
}

/********************************************************************/
/*	function name													*/
/*		CopyMem()				- メモリコピ－						*/
/*	parameters														*/
/*		(1)void *dst			- <i> 転送先アドレス(long pointer)	*/
/*		(2)void *src			- <i> 転送元アドレス(long pointer)	*/
/*		(3)Uint32 cnt			- <i> 転送サイズ（ロング単位）		*/
/*	description														*/
/*		none														*/
/*	preconditions													*/
/*		none														*/
/*	postconditions													*/
/*		none														*/
/*	caveats															*/
/*		none														*/
/********************************************************************/

static void CopyMem(void *dst, void *src, Uint32 cnt)
{
#ifndef _DMA_SCU
	memcpy(dst, src, cnt);
#else
                                                /*****************************/
    DMA_ScuMemCopy(dst, src, cnt);
    while(DMA_SCU_END != DMA_ScuResult());
#endif /* _DMA_SCU */
}

/********************************************************************/
/*	function name													*/
/*		GetComBlockAdr()		- コマンドブロック					*/
/*	parameters														*/
/*		none														*/
/*	description														*/
/*		none														*/
/*	preconditions													*/
/*		none														*/
/*	postconditions													*/
/*		(1) Uint8				- <ret> コマンドブロックの空き状態	*/
/*	caveats															*/
/*		none														*/
/********************************************************************/

static Uint8 GetComBlockAdr(void)
{
    if(*NOW_ADR_COM_DATA){              /* 以前のblockが引き取り済みでないか?*/
        /* 次コマンドブロックアドレス設定処理 ********************************/
        if(NOW_ADR_COM_DATA >= (MAX_ADR_COM_DATA - SIZE_COM_BLOCK)){
                                                    /* 最大値か?            */
            return OFF;                             /* block空き無し      */
        }else{
            adr_com_block += SIZE_COM_BLOCK;        /* 現在command block恭歡丑餅*/
            while(NOW_ADR_COM_DATA < (MAX_ADR_COM_DATA - SIZE_COM_BLOCK)){
                if(*NOW_ADR_COM_DATA){
                    adr_com_block += SIZE_COM_BLOCK;
                }else{
                    return ON;                      /* block空き有り         */
                }
            }
            return OFF;                             /* block空き無し         */
        }
    }else{
        adr_com_block = adr_host_int_work;  /* blockの先頭へ              */
        while(NOW_ADR_COM_DATA < (MAX_ADR_COM_DATA - SIZE_COM_BLOCK)){
            if(*NOW_ADR_COM_DATA){
                adr_com_block += SIZE_COM_BLOCK;
            }else{
                return ON;                          /* block空き有り         */
            }
        }
        return OFF;                                 /* block空き無し         */
    }
}

