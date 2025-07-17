/*
 * 名称:        システム プログラム
 *
 * ファイル名:  sega_sys.h
 *
 * 日付:     1994/05/06  for BOOT 0.90,0.901,0.91  by T. S.
 *           1994/07/16  for BOOT 0.92x,0.95       by T. S.
 *           1994/07/29  for BOOT 0.96x,1.00       by T. S.
 *           1994/08/31  how to exec CD multi-player
 *           1994/09/21  for Ver.1.00 BOOT ROM
 *           1995/03/31  MPEG check service
 *           1995/09/26  Ver.2.01 : sega_sys.lib   by M. M.
 *           1995/12/06  Ver.2.04 : SYS_Exit修正   by M. M.
 *           1996/01/31  Ver.2.10 : SYS_Exit修正   by M. M.
 *           1996/02/26  Ver.2.11 : SYS_Exit修正   by M. M.
 *           1996/08/06  Ver.2.20 : 新SYS追加      by H. K.
 *           1996/10/01  Ver.2.21: コンパイルオプション変更 by H. K.
 *           1997/05/23  Ver.2.50 : A.H(SOJ)
 *				割込みルーチンに関する macレジスタ待避を
 *				INTライブラリから SYSライブラリへと変更する為、
 *				下記 4個の関数およびマクロを追加。
 *					void SYS_SetUintMacSave( _Num_ ,_Hdr_ )
 *					void SYS_SETUINT_NO_MACSAVE(_Num_, _Hdr_)
 *					interrupt_t SYS_GetUintMacSave( _Num_ )
 *					interrupt_t SYS_GETUINT_NO_MACSAVE(_Num_)
 *
 *
 * 目的:        BOOT ROM のサービスルーチンを定義します。
 *              また、割込みルーチンの管理を行います。
 *
 *              (1) SCU 割り込みサービスルーチンの登録参照
 *              (2) SH2 割り込みベクタの登録参照
 *              (3) システムセマフォの操作  (BOOT ROM V0.90以降)
 *              (4) SCU 割り込みマスクの操作(BOOT ROM V0.90以降)
 *              (5) システムクロックの切替え(BOOT ROM V0.92以降)
 *              (6) SCU 割り込みルーチン優先度の変更(SCU2.1以降)
 *              (7) CD  マルチプレーヤの起動実行(CD付きモデル用)
 *              (8) パワーオンクリアメモリの操作 (ROM V1.00以降)
 *              (9) MPEG チェック(完了復帰型)    (ROM V1.00以降)
 *
 *              sega_sys.lib内の関数を宣言します。（Ver.2.01以降）
 *
 * 使用方法:
 *
 *      void  Vfunc(void)
 *      {
 *           // ... 実行文 ... //
 *      }
 *
 *      extern void  Hihdr(void);
 *
 *      void  test()
 *      {
 *           void (*tmp)();
 *           Sint32 rep;
 *           Uint32 msk, clk, lval1;
 *           Uint8  p0;
 *           const Uint32 PriTab[32] = {
 *              0x00f0ffff, 0x00e0fffe, 0x00d0fffc, 0x00c0fff8,
 *              0x00b0fff0, 0x00a0ffe0, 0x0090ffc0, 0x0080ff80,
 *              0x0080ff80, 0x0070fe00, 0x0070fe00, 0x0070fe00,
 *              0x0070fe00, 0x0070fe00, 0x0070fe00, 0x0070fe00,
 *              0x0070fe00, 0x0070fe00, 0x0070fe00, 0x0070fe00,
 *              0x0070fe00, 0x0070fe00, 0x0070fe00, 0x0070fe00,
 *              0x0070fe00, 0x0070fe00, 0x0070fe00, 0x0070fe00,
 *              0x0070fe00, 0x0070fe00, 0x0070fe00, 0x0070fe00,
 *           };
 *
 *           SYS_SETUINT(0x40, Vfunc);  // Vfunc() を VB-In 割込み用に登録   //
 *           tmp = SYS_GETUINT(0x41);   // 現在の VB-Out ルーチン登録を参照  //
 *           SYS_SETUINT(0x41, 0);      // VB-Out ルーチンの登録を消去       //
 *
 *           SYS_SETSINT(0x42, Hihdr);  // Hihdr() を HB-In ベクタに登録     //
 *           tmp = SYS_GETSINT(0x6e);   // 現在の DIVU ベクタ登録を参照      //
 *           SYS_SETSINT(0x6e, 0);      // DIVU ベクタを再初期化             //
 *
 *           rep = SYS_TASSEM(0x5);     // セマフォ 5 をテスト・アンド・セット //
 *           SYS_CLRSEM(0x5);           // セマフォ 5 をクリア               //
 *
 *           SYS_SETSCUIM(0xfffffffe);  // VB-In 割込みのみを許可に設定      //
 *           SYS_CHGSCUIM(0xfffffffc, 0); // VB-In, VB-Out を許可に変更      //
 *           SYS_CHGSCUIM(0xffffffff, 2); // VB-Out  を不許可に変更          //
 *           msk = SYS_GETSCUIM;        // 現在の SCU 割込みマスク値を参照   //
 *
 *           SYS_CHGSYSCK(1);           // システムクロックを２８Ｍに切替え  //
 *           clk = SYS_GETSYSCK;        // 現在の システムクロック値を参照   //
 *                                      //   0: 26 MHz       1: 28 MHz       //
 *                                      //     320/640 pix/H   352/704 pix/H //
 *
 *           SYS_CHGUIPR(PriTab);       // ＳＣＵ割り込みルーチン優先度変更  //
 *
 *           SYS_EXECDMP();             // ＣＤマルチプレーヤ起動実行        //
 *
 *           p0 = SYS_PCLRMEM;          // パワーオンクリアメモリ先頭バイト  //
 *           lval1 = *((Uint32*)&SYS_PCLRMEM+1); // と後半ロングワードを参照 //
 *
 *           rep = SYS_CHKMPEG(0);      // 引数は必ず 0. mansys.txt の使用例 //
 *                                      // に従ってください                  //
 *      }
 *
 *      // ... 別のファイル ... //
 *      #pragma interrupt(Hihdr)
 *      void  Hihdr(void)
 *      {
 *           // ... 実行文 ... //
 *      }
 *
 */

#ifndef SEGA_SYS_H
#define SEGA_SYS_H

/***********************************************************************
 *      インクルードファイル
 ***********************************************************************/
#include "sega_xpt.h"

/*****************************************************************************
 *      定数マクロ
 *****************************************************************************/

/* バージョン */
                    /*             1         2         3         4    */
                    /*    1234567890123456789012345678901234567890    */
                    /*    ++++++++++++++++++++++++++++++++++          */
#define SYS_VERSION_STR ("SYS Version 2.50 1997-06-10")
#define SYS_VERSION_LEN         (32)
/* ライブラリバージョン管理用 マクロ */
/* Ver 2.50以降   1997-05-20 A.H(SOJ) */
#define SYS_VER_250


/***********************************************************************
 *      処理マクロ
 ***********************************************************************/
#ifndef SYS_VER_250
	#define SYS_SETUINT(_Num_, _Hdr_) \
	        ((**(volatile void(**)(Uint32, void*))0x6000300)((_Num_), (_Hdr_)))
	#define SYS_GETUINT(_Num_) \
        ((void*)(**(void(*(**)(Uint32))(Uint32))0x6000304)(_Num_))
#else
  /* macレジスタ対応を、SYS_SetUintMacSaveで行うように変更 */
	#define SYS_SETUINT(_Num_, _Hdr_) \
			SYS_SetUintMacSave( _Num_ ,_Hdr_ );
	#define SYS_SETUINT_NO_MACSAVE(_Num_, _Hdr_) \
	        ((**(volatile void(**)(Uint32, void*))0x6000300)((_Num_), (_Hdr_)))
	#define SYS_GETUINT(_Num_) \
			SYS_GetUintMacSave(_Num_);
	#define SYS_GETUINT_NO_MACSAVE(_Num_) \
        ((void*)(**(void(*(**)(Uint32))(Uint32))0x6000304)(_Num_))
#endif  /* SYS_VER_250 */

#define SYS_SETSINT(_Num_, _Hdr_) \
        ((**(volatile void(**)(Uint32, void*))0x6000310)((_Num_), (_Hdr_)))
#define SYS_GETSINT(_Num_) \
        ((void*)(**(void(*(**)(Uint32))(Uint32))0x6000314)(_Num_))

#define SYS_TASSEM(_Num_) \
        ((**(volatile Uint32(**)(Uint32))0x6000330)(_Num_))
#define SYS_CLRSEM(_Num_) \
        ((**(volatile void(**)(Uint32))0x6000334)(_Num_))

#define SYS_SETSCUIM(_MaskPat_) \
        ((**(volatile void(**)(Uint32))0x6000340)(_MaskPat_))
#define SYS_CHGSCUIM(_AndMask_, _OrMask_) \
        ((**(volatile void(**)(Uint32, Uint32))0x6000344)((_AndMask_), (_OrMask_)))

#define SYS_GETSCUIM \
        (*(volatile Uint32*)0x6000348)

#define SYS_CHGSYSCK(_CkMode_) \
        ((**(volatile void(**)(Uint32))0x6000320)(_CkMode_))
#define SYS_GETSYSCK \
        (*(volatile Uint32*)0x6000324)

#define SYS_CHGUIPR(_IprTab_) \
        ((**(volatile void(**)(Uint32*))0x6000280)(_IprTab_))

#define SYS_EXECDMP \
        (**(volatile void(**)(void))0x600026C)

#define SYS_PCLRMEM \
        (*(volatile Uint8*)0x6000210)

#define SYS_CHKMPEG(_dummy_) \
        ((**(volatile Sint32(**)(Sint32))0x6000274)(_dummy_))

/***********************************************************************
 *      関数の宣言
 ***********************************************************************/
void SYS_CheckTrack(Sint32 tno);
void SYS_Exit(Sint32 code);


/* Ver 2.20より追加された新しい関数 */
/*-------------------------------------------------------------------
 *	書式	：	void SYS_SETSCUIM_NR(Uint32 MaskPat);
 *	入力	：	MaskPat	:SCU割り込みマスク値
 *	出力	：	なし
 *	関数値	：	なし
 *	機能	：	マスク保存メモリ、SCU割り込みマスクレジスタに
 *				MaskPatの値を書き込み、次に同じ値をSCU割り込み
 *				レジスタに書き込みます。ただし、A-Bus割り込み
 *				マスクビットが許可になる場合、A-Busアクノリッジ
 *				レジスタをクリアします。
 *				SYS_SETSCUIM関数との違いは、SCUの割込みステータスレジスタ
 *				の上位ワードをクリアしないことです。
 *	備考	：	SYS_SETSCUIM() を実行すると、待機中の SCU割込みがキャンセ
 *				ルされます。
 *				SCU割込みの発生を確実にする為、通常は SYS_SETSCUIM_NR() の
 *				方を使用して下さい。
 *------------------------------------------------------------------*/
 void SYS_SETSCUIM_NR(Uint32 MaskPat);

/*-------------------------------------------------------------------
 *	書式	：	void SYS_CHGSCUIM_NR(Uint32 AndMask,Uint32 OrMask);
 *	入力	：	AndMask	:許可用マスク値
 *			：	OrMask	:拒否用マスク値
 *	出力	：	なし
 *	関数値	：	なし
 *	機能	：	マスク保存メモリ内容を、AndMaskと論理積をとり、
 *				それにOrMAskと論理和をとった結果を、マスク保存用
 *				メモリとSCU割り込みマスクレジスタに書き込みます。
 *				次に、AndMaskとOrMaskの論理和を、SCUステータスレ
 *				ジスタに書き込みます。
 *				ただし、A-Bus割り込みマスクビットが許可になる場合、
 *				A-Busアクノリッジレジスタをクリアします。これらの
 *				操作を不可分に実行します。
 *				SYS_CHGSCUIM関数との違いは、SCU割込みステータスレジスタ
 *				の上位ワードをクリアしないことです。
 *	備考	：	SYS_CHGSCUIM() を実行すると、待機中の SCU割込みがキャンセ
 *				ルされます。
 *				SCU割込みの発生を確実にする為、通常は SYS_CHGSCUIM_NR() の
 *				方を使用して下さい。
 *------------------------------------------------------------------*/
 void SYS_CHGSCUIM_NR(Uint32 AndMask,Uint32 OrMask);


#ifndef SYS_VER_250
#else
/* Ver 2.50より追加された新しい型宣言 */
typedef	void	( *interrupt_t )( void );


/* Ver 2.50より追加された新しい関数 */
void	SYS_SetUintMacSave( Uint32 Num ,interrupt_t Hdr );
interrupt_t  SYS_GetUintMacSave( Uint32 Num );

void	volatile SYS_GetMac( volatile Uint32 *addr_mac );
void	volatile SYS_SetMac( volatile Uint32 *addr_mac );

Uint32	volatile SYS_GetMach(void);
Uint32	volatile SYS_GetMacl(void);
void	volatile SYS_SetMach( volatile Uint32 mach );
void	volatile SYS_SetMacl( volatile Uint32 macl );
void	volatile SYS_SetMac_hl( volatile Uint32 mach, volatile Uint32 macl );
#endif

#endif  /*   #ifndef SEGA_SYS_H   */
