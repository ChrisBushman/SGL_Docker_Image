/*-----------------------------------------------------------------------------
 *  FILE: smpsnd0.c
 *
 *  Copyright(c) 1994 SEGA
 *
 *  PURPOSE:
 *
 *      サウンドサンプルプログラム
 *
 *  DESCRIPTION:
 *
 *      シーケンスデータを鳴らします。
 *
 *  INTERFACE:
 *
 *      < FUNCTIONS LIST >
 *
 *  CAVEATS:
 *
 *  AUTHOR(S)
 *
 *      1994-05-19  N.T Ver.0.90
 *
 *  MOD HISTORY:
 *
 *      1994-08-30  N.T Ver.1.01
 *      1995-11から サウンドドライバのサイズ変更に伴い随時調整を行う
 *
 *-----------------------------------------------------------------------------
 */

/*
 * C VIRTUAL TYPES DEFINITIONS
 */
#include "sega_xpt.h"

/*
 * USER SUPPLIED INCLUDE FILES
 */
#include "sega_snd.h"

/*
 * GLOBAL DECLARATIONS
 */

/*
 * LOCAL DEFINES/MACROS
 */

/*
 * STATIC DECLARATIONS
 */

/*
 * STATIC FUNCTION PROTOTYPE DECLARATIONS
 */

/******************************************************************************
 *
 * NAME:    main()      - メイン
 *
 * PARAMETERS :
 *      なし
 *
 * DESCRIPTION:
 *      メイン
 *
 * PRECONDITIONS:
 *      なし。
 *
 * POSTCONDITIONS:
 *      なし
 *
 * CAVEATS:
 *      なし。
 *
 ******************************************************************************
 */
void tubo(void);
void main(void)
{

    SndIniDt snd_init;                                  /* システム起動データ*/
#if	0
	/*
	**■1995-07-28	高橋智延
	**	使ってないので削除
	*/
    SndSeqStat status;                              /* シーケンスステータス  */
    Uint32 i;
#endif

    /** BEGIN ****************************************************************/
    /*
     *  process 1   （各PAD情報初期化）
     */

    SND_INI_PRG_ADR(snd_init) = (Uint16 *)0x6080000;
	/*
	 * 1996-02-15 H.O
	 * 以下の値は、使用するサウンドドライバ(sddrvs.tsk)の大きさにあわせて下さい。
	 */
    SND_INI_PRG_SZ(snd_init) = (Uint16 )25174;	/* サウンドドライバ Ver2.20 */

    SND_INI_ARA_ADR(snd_init) = (Uint16 *)0x6029800; /* サウンドマップ */
    SND_INI_ARA_SZ(snd_init) = (Uint16)74;
    SND_Init(&snd_init);						/* サウンドシステム起動		 */
    SND_ChgMap(0);	/* サウンドエリアマップ変更	 */

  	SND_MoveData((Uint16 *)0x6020000,			/* 音色データ転送			 */
				 (Uint32)0x5000,				/* 32bitアライメント		 */
				 SND_KD_TONE,
				 0);
	SND_MoveData((Uint16 *)0x6025000,			/* シーケンスデータ転送		 */
				 (Uint32)0x400, 				/* 32bitアライメント		 */
				 SND_KD_SEQ,
				 0);
	SND_MoveData((Uint16 *)0x6025400,			/* シーケンスデータ転送		 */
				 (Uint32)0x2700, 				/* 32bitアライメント		 */
				 SND_KD_SEQ,
				 1);
	SND_MoveData((Uint16 *)0x6028000,			/* シーケンスデータ転送		 */
				 (Uint32)0x600, 				/* 32bitアライメント		 */
				 SND_KD_DSP_PRG,
				 0);
	SND_MoveData((Uint16 *)0x6028600,			/* シーケンスデータ転送		 */
				 (Uint32)0x600, 				/* 32bitアライメント		 */
				 SND_KD_DSP_PRG,
				 1);
	SND_MoveData((Uint16 *)0x6028c00,			/* シーケンスデータ転送		 */
				 (Uint32)0x600, 				/* 32bitアライメント		 */
				 SND_KD_DSP_PRG,
				 2);
	SND_MoveData((Uint16 *)0x6029200,			/* シーケンスデータ転送		 */
				 (Uint32)0x600, 				/* 32bitアライメント		 */
				 SND_KD_DSP_PRG,
				 3);
	SND_StartSeq(1, 0, 2, 1);					/* シーケンス開始			 */
    while(1);
}
