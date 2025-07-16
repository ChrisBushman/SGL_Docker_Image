/*-----------------------------------------------------------------------------
 *	FILE: sega_int.h
 *
 *		Copyright(c) 1994,1995,1996,1997 SEGA
 *
 *	PURPOSE:
 *		INTライブラリヘッダファイル
 *
 *	DESCRIPTION:
 *
 *	AUTHOR(S):
 *		1997-05-20	A.H Ver.2.00
 *
 *	MOD HISTORY:
 *		1994-07-05	N.T Ver.1.00
 *		1995-03-20	K.T Ver.1.00++
 *		1996-11-17	Matusita Misao Ver.1.01
 *			コンパイルオプションとファイル名の変更。
 *		1997-02-25	Kawai Toshikazu Ver.1.02
 *			INT_SetScuFunc, INT_GetScuFunc関数の不具合を修正。
 *			セクション名をSEGA_Pセクションに変更。
 *		1997-04-15	A.H Ver.1.03
 *			SCU割込みベクタ以外のベクタ番号についての処理を削除
 *			割込みステータスレジスタへのライト用マクロ INT_ResStat を削除
 *			（SCU仕様変更(No07)により使用不可の為）
 *		1997-06-10	A.H Ver.2.00
 *			1.旧 SEGA_INT(Ver 1.04 以前)にて、本ライブラリの関数となっていた
 *			  次の関数本体を システムライブラリSEGA_SYS( Ver2.50以降) に移動
 *			  ・void		INT_SetScuFunc( int	n, interrupt_t	handler );
 *			  ・interrupt_t	INT_GetScuFunc( int	n );
 *          2.INT_SetMsk_NR(),INT_ChgMsk_NR() を追加。
 *
 *-----------------------------------------------------------------------------
 */

#ifndef SEGA_INT_H
#define SEGA_INT_H


/* ライブラリバージョン管理用 マクロ */
/* Ver 2.00以降   1997-05-20 A.H(SOJ) */
#define INT_VER_200		1

/*
 * C VIRTUAL TYPES DEFINITIONS
 */
#include "sega_xpt.h"
#include "sega_sys.h"

/*
 * GLOBAL DEFINES/MACROS DEFINES
 */

/* 定数 */
/**** 割り込みアドレスレジスタ ***********************************************/
#define INT_REG_IST		((volatile Uint32 *)0x25fe00a4)	 /* 割り込みステータスレジスタ*/
#if 0	/* K.T 1995/03/20 */
#define INT_REG_IACK	((volatile Uint32 *)0x25fe00bc)	 /* A-Bus割り込みアクノリッジ */
#endif
#define INT_REG_IACK	((volatile Uint32 *)0x25fe00a8)	 /* A-Bus割り込みアクノリッジ */
/***** 割り込みマスクビット値定数 ********************************************/
#define INT_MSK_NULL	  0				/* 指定なし							 */
#define INT_MSK_ALL		 (INT_MSK_ABUS	   |	/* 全指定　					*/\
						  INT_MSK_SPR	   |\
						  INT_MSK_DMAI	   |\
						  INT_MSK_DMA0	   |\
						  INT_MSK_DMA1	   |\
						  INT_MSK_DMA2	   |\
						  INT_MSK_PAD	   |\
						  INT_MSK_SYS	   |\
						  INT_MSK_SND	   |\
						  INT_MSK_DSP	   |\
						  INT_MSK_TIM1	   |\
						  INT_MSK_TIM0	   |\
						  INT_MSK_HBLK_IN  |\
						  INT_MSK_VBLK_OUT |\
						  INT_MSK_VBLK_IN  )
#define INT_MSK_ABUS	 (1 << 15)		/* A-Bus							 */
#define INT_MSK_SPR		 (1 << 13)		/* スプライト描画終了				 */
#define INT_MSK_DMAI	 (1 << 12)		/* DMAイリーガル					 */
#define INT_MSK_DMA0	 (1 << 11)		/* レベル0-DMA						 */
#define INT_MSK_DMA1	 (1 << 10)		/* レベル1-DMA						 */
#define INT_MSK_DMA2	 (1 <<	9)		/* レベル2-DMA						 */
#define INT_MSK_PAD		 (1 <<	8)		/* PAD								 */
#define INT_MSK_SYS		 (1 <<	7)		/* システムマネージャ				 */
#define INT_MSK_SND		 (1 <<	6)		/* サウンドリクエスト				 */
#define INT_MSK_DSP		 (1 <<	5)		/* DSP終了　　						 */
#define INT_MSK_TIM1	 (1 <<	4)		/* タイマ-1							 */
#define INT_MSK_TIM0	 (1 <<	3)		/* タイマ-0							 */
#define INT_MSK_HBLK_IN	 (1 <<	2)		/* H-ブランク-IN					 */
#define INT_MSK_VBLK_OUT (1 <<	1)		/* V-ブランク-OUT					 */
#define INT_MSK_VBLK_IN	 (1 <<	0)		/* V-ブランク-IN					 */

/***** 割り込みステータスビット値定数 ****************************************/
#define INT_ST_NULL		 0				/* 指定なし							 */
#define INT_ST_ALL		(INT_ST_ABUS	  |	   /* 全指定　				   */\
						  INT_ST_SPR	  |\
						  INT_ST_DMAI	  |\
						  INT_ST_DMA0	  |\
						  INT_ST_DMA1	  |\
						  INT_ST_DMA2	  |\
						  INT_ST_PAD	  |\
						  INT_ST_SYS	  |\
						  INT_ST_SND	  |\
						  INT_ST_DSP	  |\
						  INT_ST_TIM1	  |\
						  INT_ST_TIM0	  |\
						  INT_ST_HBLK_IN  |\
						  INT_ST_VBLK_OUT |\
						  INT_ST_VBLK_IN  )
#define INT_ST_ABUS		(INT_ST_ABUS01 |		/* A-Bus				   */\
						 INT_ST_ABUS02 |\
						 INT_ST_ABUS03 |\
						 INT_ST_ABUS04 |\
						 INT_ST_ABUS05 |\
						 INT_ST_ABUS06 |\
						 INT_ST_ABUS07 |\
						 INT_ST_ABUS08 |\
						 INT_ST_ABUS09 |\
						 INT_ST_ABUS10 |\
						 INT_ST_ABUS11 |\
						 INT_ST_ABUS12 |\
						 INT_ST_ABUS13 |\
						 INT_ST_ABUS14 |\
						 INT_ST_ABUS15 |\
						 INT_ST_ABUS16 )
#define INT_ST_ABUS01	(1 << 31)		/* A-Bus割込み00					 */
#define INT_ST_ABUS02	(1 << 30)		/* A-Bus割込み01					 */
#define INT_ST_ABUS03	(1 << 29)		/* A-Bus割込み02					 */
#define INT_ST_ABUS04	(1 << 28)		/* A-Bus割込み03					 */
#define INT_ST_ABUS05	(1 << 27)		/* A-Bus割込み04					 */
#define INT_ST_ABUS06	(1 << 26)		/* A-Bus割込み05					 */
#define INT_ST_ABUS07	(1 << 25)		/* A-Bus割込み06					 */
#define INT_ST_ABUS08	(1 << 24)		/* A-Bus割込み07					 */
#define INT_ST_ABUS09	(1 << 23)		/* A-Bus割込み08					 */
#define INT_ST_ABUS10	(1 << 22)		/* A-Bus割込み09					 */
#define INT_ST_ABUS11	(1 << 21)		/* A-Bus割込み10					 */
#define INT_ST_ABUS12	(1 << 20)		/* A-Bus割込み11					 */
#define INT_ST_ABUS13	(1 << 19)		/* A-Bus割込み12					 */
#define INT_ST_ABUS14	(1 << 18)		/* A-Bus割込み13					 */
#define INT_ST_ABUS15	(1 << 17)		/* A-Bus割込み14					 */
#define INT_ST_ABUS16	(1 << 16)		/* A-Bus割込み15					 */
#define INT_ST_SPR		(1 << 13)		/* スプライト描画終了				 */
#define INT_ST_DMAI		(1 << 12)		/* DMAイリーガル					 */
#define INT_ST_DMA0		(1 << 11)		/* レベル0-DMA						 */
#define INT_ST_DMA1		(1 << 10)		/* レベル1-DMA						 */
#define INT_ST_DMA2		(1 <<  9)		/* レベル2-DMA						 */
#define INT_ST_PAD		(1 <<  8)		/* PAD								 */
#define INT_ST_SYS		(1 <<  7)		/* システムマネージャ				 */
#define INT_ST_SND		(1 <<  6)		/* サウンドリクエスト				 */
#define INT_ST_DSP		(1 <<  5)		/* DSP終了							 */
#define INT_ST_TIM1		(1 <<  4)		/* タイマ-1							 */
#define INT_ST_TIM0		(1 <<  3)		/* タイマ-0							 */
#define INT_ST_HBLK_IN	(1 <<  2)		/* H-ブランク-IN					 */
#define INT_ST_VBLK_OUT (1 <<  1)		/* V-ブランク-OUT					 */
#define INT_ST_VBLK_IN	(1 <<  0)		/* V-ブランク-IN					 */

/***** ベクタ番号定数 ********************************************************/
/********* SCU (CPU と共用)***************************************************/
#define INT_SCU_VBLK_IN		0x40		/* V-ブランク-IN					 */
#define INT_SCU_VBLK_OUT	0x41		/* V-ブランク-OUT					 */
#define INT_SCU_HBLK_IN		0x42		/* H-ブランク-IN					 */
#define INT_SCU_TIM0		0x43		/* タイマ-0							 */
#define INT_SCU_TIM1		0x44		/* タイマ-1							 */
#define INT_SCU_DSP			0x45		/* DSP終了							 */
#define INT_SCU_SND			0x46		/* サウンドリクエスト				 */
#define INT_SCU_SYS			0x47		/* システムマネージャ				 */
#define INT_SCU_PAD			0x48		/* PAD								 */
#define INT_SCU_DMA2		0x49		/* レベル2-DMA						 */
#define INT_SCU_DMA1		0x4a		/* レベル1-DMA						 */
#define INT_SCU_DMA0		0x4b		/* レベル0-DMA						 */
#define INT_SCU_DMAI		0x4c		/* DMAイリーガル					 */
#define INT_SCU_SPR			0x4d		/* スプライト描画終了				 */
#define INT_SCU_ABUS00		0x50		/* A-BUS 外部割込み00				 */
#define INT_SCU_ABUS01		0x51		/* A-BUS 外部割込み01				 */
#define INT_SCU_ABUS02		0x52		/* A-BUS 外部割込み02				 */
#define INT_SCU_ABUS03		0x53		/* A-BUS 外部割込み03				 */
#define INT_SCU_ABUS04		0x54		/* A-BUS 外部割込み04				 */
#define INT_SCU_ABUS05		0x55		/* A-BUS 外部割込み05				 */
#define INT_SCU_ABUS06		0x56		/* A-BUS 外部割込み06				 */
#define INT_SCU_ABUS07		0x57		/* A-BUS 外部割込み07				 */
#define INT_SCU_ABUS08		0x58		/* A-BUS 外部割込み08				 */
#define INT_SCU_ABUS09		0x59		/* A-BUS 外部割込み09				 */
#define INT_SCU_ABUS10		0x5A		/* A-BUS 外部割込み10				 */
#define INT_SCU_ABUS11		0x5B		/* A-BUS 外部割込み11				 */
#define INT_SCU_ABUS12		0x5C		/* A-BUS 外部割込み12				 */
#define INT_SCU_ABUS13		0x5D		/* A-BUS 外部割込み13				 */
#define INT_SCU_ABUS14		0x5E		/* A-BUS 外部割込み14				 */
#define INT_SCU_ABUS15		0x5F		/* A-BUS 外部割込み15				 */

/********* CPU ***************************************************************/
#define INT_CPU_SCI_ERI		0x60		/* SCI受信エラー					 */
#define INT_CPU_SCI_RXI		0x61		/* SCI受信データフル				 */
#define INT_CPU_SCI_TXI		0x62		/* SCI受信データエンプティ			 */
#define INT_CPU_SCI_TEI		0x63		/* SCI送信終了						 */
#define INT_CPU_FRT_ICI		0x64		/* FRTインプットキャプチャ			 */
#define INT_CPU_FRT_OCI		0x65		/* FRTアウトプットコンペア			 */
#define INT_CPU_FRT_OVI		0x66		/* FRTオーバフロー					 */
#define INT_CPU_WDT			0x68		/* WDTインターバル					 */
#define INT_CPU_BSC			0x69		/* BSCコンペアマッチ				 */
#define INT_CPU_DMAC1		0x6c		/* DMACチャネル1					 */
#define INT_CPU_DMAC0		0x6d		/* DMACチャネル0					 */
#define INT_CPU_DIVU		0x6e		/* 除算器							 */

/***** アクノリッジ値 *******************************************************/
#define INT_ACK_ENA			0x1			/* 有効								 */
#define INT_ACK_DIS			0x0			/* 保持								 */

/******************************************************************************
 *
 * NAME:	INT_GetMsk()		-	割り込みマスクレジスタ取得
 *
 * PARAMETERS :
 *		なし。
 *
 * DESCRIPTION:
 *
 * PRECONDITIONS:
 *		なし。
 *
 * POSTCONDITIONS:
 *		(1) Uint32	割り込みマスクビット値
 *
 * CAVEATS:
 *		なし。
 *
 ******************************************************************************
 */
#if	0

■1995-07-05 高橋智延
	a って何なの？

#define INT_GetMsk(a) SYS_GETSCUIM

#endif

#define	INT_GetMsk()	SYS_GETSCUIM

/******************************************************************************
 * 機能：SCU割り込みマスクレジスタの設定
 *
 * NAME:	INT_SetMsk()
 * NAME:	INT_SetMsk_NR()
 *
 * PARAMETERS :
 *		(1) Uint32 msk_bit		<i>	  割り込みマスクビット値
 *
 * DESCRIPTION:
 *
 * PRECONDITIONS:
 *		なし。
 *
 * POSTCONDITIONS:
 *		なし。
 *
 * 注意事項
 * CAVEATS:
 *		INT_SetMsk() は、実行時に A-BUS割込みがイネーブルになるような
 *		設定の場合、SCU割込みステータスレジスタへのライトを行います。
 *		この為、同一クロックで発生した SCU割込みがキャンセルされる恐れ
 *		があります。
 *		通常は、INT_SetMsk_NR() の方を使用して下さい。
 *		なお、_NR は NO_RESET の意味です。
 *
 ******************************************************************************
 */

#define INT_SetMsk(msk_bit) SYS_SETSCUIM(msk_bit)
#define INT_SetMsk_NR(msk_bit) SYS_SETSCUIM_NR(msk_bit)

/******************************************************************************
 * SCU割り込みマスクレジスタの変更
 *
 * NAME:   INT_ChgMsk()
 * NAME:   INT_ChgMsk_NR()
 *
 * PARAMETERS :
 *		(1) Uint32 ena_msk_bit	<i>	  割り込みマスクビット（イネーブル)
 *		(2) Uint32 dis_msk_bit	<i>	  割り込みマスクビット（ディセーブル)
 *
 * DESCRIPTION:
 *
 * PRECONDITIONS:
 *		なし。
 *
 * POSTCONDITIONS:
 *		なし。
 *
 * 注意事項
 * CAVEATS:
 *		INT_ChgMsk() は、実行時に A-BUS割込みがイネーブルになるような
 *		設定の場合、SCU割込みステータスレジスタへのライトを行います。
 *		この為、同一クロックで発生した SCU割込みがキャンセルされる恐れ
 *		があります。
 *		通常は、INT_ChgMsk_NR() の方を使用して下さい。
 *		なお、_NR は NO_RESET の意味です。
 *
 ******************************************************************************
 */

#define INT_ChgMsk(ena_msk_bit, dis_msk_bit)\
		SYS_CHGSCUIM(~(ena_msk_bit), dis_msk_bit)
#ifdef INT_VER_200
	/* SCU割込みマスク変更で割込みクリアされないように処置 */
	#define INT_ChgMsk_NR(ena_msk_bit, dis_msk_bit)\
		SYS_CHGSCUIM_NR(~(ena_msk_bit), dis_msk_bit)
#endif

/******************************************************************************
 *
 * NAME:	INT_GetStat()	-	割り込みステータスレジスタ取得
 *
 * PARAMETERS :
 *		なし。
 *
 * DESCRIPTION:
 *
 * PRECONDITIONS:
 *		なし。
 *
 * POSTCONDITIONS:
 *		(1) Uint32	割り込みステータスレジスタ値
 *
 * CAVEATS:
 *		なし。
 *
 ******************************************************************************
 */

#define INT_GetStat()	(*INT_REG_IST)

/******************************************************************************
 *
 * NAME:	INT_ResStat()		-	割り込みステータスレジスタリセット
 * 1997-04-14 A.H
 *   SCU仕様変更 No.07 「割込みステータスレジスタへの書き込み禁止」
 *   より、'INT_ResStat'は使用禁止。
 *
 * PARAMETERS :
 *		(1) Uint32 status_bit	<i>	  割り込みステータスビット値
 *
 * DESCRIPTION:
 *
 * PRECONDITIONS:
 *		なし。
 *
 * POSTCONDITIONS:
 *		なし。
 *
 * CAVEATS:
 *		なし。
 *
 ******************************************************************************
 */

#if 0
/* 1997-04-14 A.H
 *   SCU仕様変更 No.07 「割込みステータスレジスタへの書き込み禁止」
 *   よって、'INT_ResStat'は通常使用禁止。
 */
#define INT_ResStat(status_bit)\
	do{\
		*INT_REG_IST = status_bit;\
	}while(FALSE)
#endif

/******************************************************************************
 *
 * NAME:	INT_SetAck()			-	A-Bus割り込みアクノリッジ設定
 *
 * PARAMETERS :
 *		(1) Uint32 ack					<i>	  アクノリッジ値
 *
 * DESCRIPTION:
 *
 * PRECONDITIONS:
 *		なし。
 *
 * POSTCONDITIONS:
 *		なし。
 *
 * CAVEATS:
 *		なし。
 *
 ******************************************************************************
 */

#define INT_SetAck(ack)\
	do{\
		*INT_REG_IACK = ack;\
	}while(FALSE)

/******************************************************************************
 *
 * NAME:	INT_GetAck()			-	A-Bus割り込みアクノリッジ取得
 *
 * PARAMETERS :
 *		なし。
 *
 * DESCRIPTION:
 *
 * PRECONDITIONS:
 *		なし。
 *
 * POSTCONDITIONS:
 *		(1) Uint32	アクノリッジ値
 *
 * CAVEATS:
 *		なし。
 *
 ******************************************************************************
 */

#define INT_GetAck()	(*INT_REG_IACK)

/******************************************************************************
 *
 * NAME:	INT_SetFunc()		-	割り込み関数登録
 *
 * PARAMETERS :
 *		(1) Uint32 num		<i>	  割込みベクタ番号
 *		(2) void *hdr		<i>	  割り込み関数アドレス
 *
 * DESCRIPTION:
 *
 * PRECONDITIONS:
 *		なし。
 *
 * POSTCONDITIONS:
 *
 * 注意事項
 * CAVEATS:
 *		指定可能な割込みベクタ番号については、システムライブラリの
 *		マニュアルを参照して下さい。
 *		SCU割込みの登録には、通常は INT_SetScuFunc() を使用して下さい。
 *
 ******************************************************************************
 */

#define INT_SetFunc(num, hdr)	SYS_SETSINT(num, hdr)

#ifndef INT_VER_200
/******************************************************************************
 *
 * NAME:	_INT_SetScuFunc()	 -	 SCU関数登録
 *
 * PARAMETERS :
 *		(1) Uint32 num		<i>	  SCU割込みベクタ番号
 *		(2) void *hdr		<i>	  SCU関数アドレス
 *
 * DESCRIPTION:
 *
 * PRECONDITIONS:
 *		なし。
 *
 * POSTCONDITIONS:
 *
 * 注意事項
 * CAVEATS:
 *		SCU割込みベクタ番号以外を指定してはいけません。
 *		本関数は、マスターSH2のみ使用可能です。
 *
 ******************************************************************************
 */
#endif

#if	0
/*
**■1995-10-30	高橋智延
**	MACL, MACH を呼出側がセーブするようにしたため、割り込みハンドラだけは、
**	自分でセーブしなければいけなくなった。→でも、それは面倒くさいので割り
**	込みハンドラを登録する際に細工をするようにして、ユーザには、そんなこと
**	は意識させないようにする。
*/
#define INT_SetScuFunc(num, hdr)	SYS_SETUINT(num, hdr)
#else
	#ifndef INT_VER_200
		#define _INT_SetScuFunc(num, hdr)	 SYS_SETUINT(num, hdr)
	#else
		/* 関数の本体は、システムライブラリSEGA_SYS に移動 */
	#endif
#endif

/******************************************************************************
 *
 * NAME:	INT_GetFunc()		-	割り込み関数アドレス取得
 *
 * PARAMETERS :
 *		(1) Uint32 num		<i>	  ベクタ番号
 *
 * DESCRIPTION:
 *
 * PRECONDITIONS:
 *		なし。
 *
 * POSTCONDITIONS:
 *		(1) void (*)()	割り込み関数アドレス
 *
 * CAVEATS:
 *		なし。
 *
 ******************************************************************************
 */

#define INT_GetFunc(num) SYS_GETSINT(num)

/******************************************************************************
 *
 * NAME:	_INT_GetScuFunc()		-	SCU関数アドレス取得
 *
 * PARAMETERS :
 *		(1) Uint32 num		<i>	  ベクタ番号(SCUベクタ番号)
 *
 * DESCRIPTION:
 *
 * PRECONDITIONS:
 *		なし。
 *
 * POSTCONDITIONS:
 *		(1) void (*)()	SCU関数アドレス
 *
 * CAVEATS:
 *		なし。
 *
 ******************************************************************************
 */

#ifndef INT_VER_200 /* 関数の本体は、システムライブラリSEGA_SYS に移動 */
	#if	0
/*
**■1995-10-30	高橋智延
**	MACL, MACH を呼出側がセーブするようにしたため、割り込みハンドラだけは、
**	自分でセーブしなければいけなくなった。→でも、それは面倒くさいので割り
**	込みハンドラを登録する際に細工をするようにして、ユーザには、そんなこと
**	は意識させないようにする。
*/
		#define INT_GetScuFunc(num) SYS_GETUINT(num)
	#else
		#define _INT_GetScuFunc(num) SYS_GETUINT(num)
	#endif
#endif  /* INT_VER_200 */

/*
 * STRUCTURE DECLARATIONS
 */

/*
 * TYPEDEFS
 */
#ifndef INT_VER_200
/*
**■1995-10-30	高橋智延
**	MACL, MACH を呼出側がセーブするようにしたため、割り込みハンドラだけは、
**	自分でセーブしなければいけなくなった。→でも、それは面倒くさいので割り
**	込みハンドラを登録する際に細工をするようにして、ユーザには、そんなこと
**	は意識させないようにする。
*/
	typedef	void	( *interrupt_t )( void );
#else
	/* interrupt_t の定義は、SEGA_SYS.H に移動 */
#endif  /* INT_VER_200 */

/*
 * EXTERNAL VARIABLE DECLARATIONS
 */

/*
 * EXTERNAL FUNCTION PROTOTYPE	DECLARATIONS
 */
#if	1
/*
**■1995-10-30	高橋智延
**	MACL, MACH を呼出側がセーブするようにしたため、割り込みハンドラだけは、
**	自分でセーブしなければいけなくなった。→でも、それは面倒くさいので割り
**	込みハンドラを登録する際に細工をするようにして、ユーザには、そんなこと
**	は意識させないようにする。
*/
void		INT_SetScuFunc( int	n, interrupt_t	handler );
interrupt_t	INT_GetScuFunc( int	n );


#endif

#endif	/* ifndef SEGA_INT_H */
