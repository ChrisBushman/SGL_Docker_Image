/*-----------------------------------------------------------------------------
 *	FILE: int_main.c
 *
 *		Copyright(c) 1994,1997 SEGA
 *
 *	PURPOSE:
 *		INTライブラリメインソースファイル
 *
 *	DESCRIPTION:
 *
 *	AUTHOR(S):
 *		1997-04-15	A.H Ver.1.03
 *
 *	MOD HISTORY:
 *		1997-02-25	Kawai Toshikazu Ver.1.02
 *			セクション名をSEGA_Pセクションに変更。
 *			INT_SetScuFunc, INT_GetScuFunc関数の不具合を修正。
 *                （関数アドレスがNULLの場合の処理追加。）
 *		1997-04-15	A.H Ver.1.03
 *			SCU割込みベクタ以外のベクタ番号についての処理を削除
 *			割込みステータスレジスタへのライト用マクロ INT_ResStat を削除
 *			（SCU仕様変更(No07)により使用不可の為）
 *		1997-05-20	A.H Ver.2.00
 *			旧 SEGA_INT(Ver 1.04 以前)にて、ライブラリ関数となっていた
 *			次の関数について、関数本体を SEGA_SYS( Ver2.50以降) に移動
 *			・void		INT_SetScuFunc( int	n, interrupt_t	handler );
 *			・interrupt_t	INT_GetScuFunc( int	n );
 *
 *-----------------------------------------------------------------------------
 */

/****************************************
*			include files				*
****************************************/
#include	"sega_int.h"

#ifndef INT_VER_200  /*   ifndef INT_VER_200    */

/****************************************
*			defines				*
****************************************/
#define		INT_SCU_TOTAL			(0x5F - 0x3F)
#define		INT_SCU_OFFSET		(0x40)

/****************************************
*			declare functions			*
****************************************/
void	__interrupt_handler0( void );   /* ベクタ40用エントリ */
void	__interrupt_handler1( void );   /* ベクタ41用エントリ */
void	__interrupt_handler2( void );   /* ベクタ42用エントリ */
void	__interrupt_handler3( void );   /* ベクタ43用エントリ */
void	__interrupt_handler4( void );   /* ベクタ44用エントリ */
void	__interrupt_handler5( void );   /* ベクタ45用エントリ */
void	__interrupt_handler6( void );   /* ベクタ46用エントリ */
void	__interrupt_handler7( void );   /* ベクタ47用エントリ */
void	__interrupt_handler8( void );   /* ベクタ48用エントリ */
void	__interrupt_handler9( void );   /* ベクタ49用エントリ */
void	__interrupt_handler10( void );  /* ベクタ4A用エントリ */
void	__interrupt_handler11( void );  /* ベクタ4B用エントリ */
void	__interrupt_handler12( void );  /* ベクタ4C用エントリ */
void	__interrupt_handler13( void );  /* ベクタ4D用エントリ */
#if 0
/* 1997-04-14 A.H */
/* SCU割込みには ベクタ4E,4F は存在しないので削除 */
void	__interrupt_handler14( void );
void	__interrupt_handler15( void );
#endif
void	__interrupt_handler16( void );  /* ベクタ50用エントリ */
void	__interrupt_handler17( void );  /* ベクタ51用エントリ */
void	__interrupt_handler18( void );  /* ベクタ52用エントリ */
void	__interrupt_handler19( void );  /* ベクタ53用エントリ */
void	__interrupt_handler20( void );  /* ベクタ54用エントリ */
void	__interrupt_handler21( void );  /* ベクタ55用エントリ */
void	__interrupt_handler22( void );  /* ベクタ56用エントリ */
void	__interrupt_handler23( void );  /* ベクタ57用エントリ */
void	__interrupt_handler24( void );  /* ベクタ58用エントリ */
void	__interrupt_handler25( void );  /* ベクタ59用エントリ */
void	__interrupt_handler26( void );  /* ベクタ5A用エントリ */
void	__interrupt_handler27( void );  /* ベクタ5B用エントリ */
void	__interrupt_handler28( void );  /* ベクタ5C用エントリ */
void	__interrupt_handler29( void );  /* ベクタ5D用エントリ */
void	__interrupt_handler30( void );  /* ベクタ5E用エントリ */
void	__interrupt_handler31( void );  /* ベクタ5F用エントリ */
#if 0
/* 1997-04-14  A.H
 *      SCU 割込み以外を削除
 */
void	__interrupt_handler32( void );
void	__interrupt_handler33( void );
void	__interrupt_handler34( void );
void	__interrupt_handler35( void );
void	__interrupt_handler36( void );
void	__interrupt_handler37( void );
void	__interrupt_handler38( void );
void	__interrupt_handler39( void );
void	__interrupt_handler40( void );
void	__interrupt_handler41( void );
void	__interrupt_handler42( void );
void	__interrupt_handler43( void );
void	__interrupt_handler44( void );
void	__interrupt_handler45( void );
void	__interrupt_handler46( void );
void	__interrupt_handler47( void );
void	__interrupt_handler48( void );
void	__interrupt_handler49( void );
void	__interrupt_handler50( void );
void	__interrupt_handler51( void );
void	__interrupt_handler52( void );
void	__interrupt_handler53( void );
void	__interrupt_handler54( void );
void	__interrupt_handler55( void );
void	__interrupt_handler56( void );
void	__interrupt_handler57( void );
void	__interrupt_handler58( void );
void	__interrupt_handler59( void );
void	__interrupt_handler60( void );
void	__interrupt_handler61( void );
void	__interrupt_handler62( void );
void	__interrupt_handler63( void );
void	__interrupt_handler64( void );
void	__interrupt_handler65( void );
void	__interrupt_handler66( void );
void	__interrupt_handler67( void );
void	__interrupt_handler68( void );
void	__interrupt_handler69( void );
void	__interrupt_handler70( void );
void	__interrupt_handler71( void );
void	__interrupt_handler72( void );
void	__interrupt_handler73( void );
void	__interrupt_handler74( void );
void	__interrupt_handler75( void );
void	__interrupt_handler76( void );
void	__interrupt_handler77( void );
void	__interrupt_handler78( void );
void	__interrupt_handler79( void );
void	__interrupt_handler80( void );
void	__interrupt_handler81( void );
void	__interrupt_handler82( void );
void	__interrupt_handler83( void );
void	__interrupt_handler84( void );
void	__interrupt_handler85( void );
void	__interrupt_handler86( void );
void	__interrupt_handler87( void );
void	__interrupt_handler88( void );
void	__interrupt_handler89( void );
void	__interrupt_handler90( void );
void	__interrupt_handler91( void );
void	__interrupt_handler92( void );
void	__interrupt_handler93( void );
void	__interrupt_handler94( void );
void	__interrupt_handler95( void );
void	__interrupt_handler96( void );
void	__interrupt_handler97( void );
void	__interrupt_handler98( void );
void	__interrupt_handler99( void );
void	__interrupt_handler100( void );
void	__interrupt_handler101( void );
void	__interrupt_handler102( void );
void	__interrupt_handler103( void );
void	__interrupt_handler104( void );
void	__interrupt_handler105( void );
void	__interrupt_handler106( void );
void	__interrupt_handler107( void );
void	__interrupt_handler108( void );
void	__interrupt_handler109( void );
void	__interrupt_handler110( void );
void	__interrupt_handler111( void );
void	__interrupt_handler112( void );
void	__interrupt_handler113( void );
void	__interrupt_handler114( void );
void	__interrupt_handler115( void );
void	__interrupt_handler116( void );
void	__interrupt_handler117( void );
void	__interrupt_handler118( void );
void	__interrupt_handler119( void );
void	__interrupt_handler120( void );
void	__interrupt_handler121( void );
void	__interrupt_handler122( void );
void	__interrupt_handler123( void );
void	__interrupt_handler124( void );
void	__interrupt_handler125( void );
void	__interrupt_handler126( void );
void	__interrupt_handler127( void );
void	__interrupt_handler128( void );
void	__interrupt_handler129( void );
void	__interrupt_handler130( void );
void	__interrupt_handler131( void );
void	__interrupt_handler132( void );
void	__interrupt_handler133( void );
void	__interrupt_handler134( void );
void	__interrupt_handler135( void );
void	__interrupt_handler136( void );
void	__interrupt_handler137( void );
void	__interrupt_handler138( void );
void	__interrupt_handler139( void );
void	__interrupt_handler140( void );
void	__interrupt_handler141( void );
void	__interrupt_handler142( void );
void	__interrupt_handler143( void );
void	__interrupt_handler144( void );
void	__interrupt_handler145( void );
void	__interrupt_handler146( void );
void	__interrupt_handler147( void );
void	__interrupt_handler148( void );
void	__interrupt_handler149( void );
void	__interrupt_handler150( void );
void	__interrupt_handler151( void );
void	__interrupt_handler152( void );
void	__interrupt_handler153( void );
void	__interrupt_handler154( void );
void	__interrupt_handler155( void );
void	__interrupt_handler156( void );
void	__interrupt_handler157( void );
void	__interrupt_handler158( void );
void	__interrupt_handler159( void );
void	__interrupt_handler160( void );
void	__interrupt_handler161( void );
void	__interrupt_handler162( void );
void	__interrupt_handler163( void );
void	__interrupt_handler164( void );
void	__interrupt_handler165( void );
void	__interrupt_handler166( void );
void	__interrupt_handler167( void );
void	__interrupt_handler168( void );
void	__interrupt_handler169( void );
void	__interrupt_handler170( void );
void	__interrupt_handler171( void );
void	__interrupt_handler172( void );
void	__interrupt_handler173( void );
void	__interrupt_handler174( void );
void	__interrupt_handler175( void );
void	__interrupt_handler176( void );
void	__interrupt_handler177( void );
void	__interrupt_handler178( void );
void	__interrupt_handler179( void );
void	__interrupt_handler180( void );
void	__interrupt_handler181( void );
void	__interrupt_handler182( void );
void	__interrupt_handler183( void );
void	__interrupt_handler184( void );
void	__interrupt_handler185( void );
void	__interrupt_handler186( void );
void	__interrupt_handler187( void );
void	__interrupt_handler188( void );
void	__interrupt_handler189( void );
void	__interrupt_handler190( void );
void	__interrupt_handler191( void );
void	__interrupt_handler192( void );
void	__interrupt_handler193( void );
void	__interrupt_handler194( void );
void	__interrupt_handler195( void );
void	__interrupt_handler196( void );
void	__interrupt_handler197( void );
void	__interrupt_handler198( void );
void	__interrupt_handler199( void );
void	__interrupt_handler200( void );
void	__interrupt_handler201( void );
void	__interrupt_handler202( void );
void	__interrupt_handler203( void );
void	__interrupt_handler204( void );
void	__interrupt_handler205( void );
void	__interrupt_handler206( void );
void	__interrupt_handler207( void );
void	__interrupt_handler208( void );
void	__interrupt_handler209( void );
void	__interrupt_handler210( void );
void	__interrupt_handler211( void );
void	__interrupt_handler212( void );
void	__interrupt_handler213( void );
void	__interrupt_handler214( void );
void	__interrupt_handler215( void );
void	__interrupt_handler216( void );
void	__interrupt_handler217( void );
void	__interrupt_handler218( void );
void	__interrupt_handler219( void );
void	__interrupt_handler220( void );
void	__interrupt_handler221( void );
void	__interrupt_handler222( void );
void	__interrupt_handler223( void );
void	__interrupt_handler224( void );
void	__interrupt_handler225( void );
void	__interrupt_handler226( void );
void	__interrupt_handler227( void );
void	__interrupt_handler228( void );
void	__interrupt_handler229( void );
void	__interrupt_handler230( void );
void	__interrupt_handler231( void );
void	__interrupt_handler232( void );
void	__interrupt_handler233( void );
void	__interrupt_handler234( void );
void	__interrupt_handler235( void );
void	__interrupt_handler236( void );
void	__interrupt_handler237( void );
void	__interrupt_handler238( void );
void	__interrupt_handler239( void );
void	__interrupt_handler240( void );
void	__interrupt_handler241( void );
void	__interrupt_handler242( void );
void	__interrupt_handler243( void );
void	__interrupt_handler244( void );
void	__interrupt_handler245( void );
void	__interrupt_handler246( void );
void	__interrupt_handler247( void );
void	__interrupt_handler248( void );
void	__interrupt_handler249( void );
void	__interrupt_handler250( void );
void	__interrupt_handler251( void );
void	__interrupt_handler252( void );
void	__interrupt_handler253( void );
void	__interrupt_handler254( void );
void	__interrupt_handler255( void );
#endif

/****************************************
*		declare private objects			*
****************************************/
#if 0
/* 1997-04-14  A.H
 *      SCU 割込み以外を削除
 */
static const interrupt_t	__interrupt_handler[256] = {
#else
static const interrupt_t	__interrupt_handler[INT_SCU_TOTAL] = {
#endif
	__interrupt_handler0,
	__interrupt_handler1,
	__interrupt_handler2,
	__interrupt_handler3,
	__interrupt_handler4,
	__interrupt_handler5,
	__interrupt_handler6,
	__interrupt_handler7,
	__interrupt_handler8,
	__interrupt_handler9,
	__interrupt_handler10,
	__interrupt_handler11,
	__interrupt_handler12,
	__interrupt_handler13,
#if 0
/* 1997-04-14 A.H */
/* SCU割込みには ベクタ4E,4F は存在しないので削除 */
	__interrupt_handler14,
	__interrupt_handler15,
#else
	__interrupt_handler13,  /* dummy */
	__interrupt_handler13,  /* dummy */
#endif
	__interrupt_handler16,
	__interrupt_handler17,
	__interrupt_handler18,
	__interrupt_handler19,
	__interrupt_handler20,
	__interrupt_handler21,
	__interrupt_handler22,
	__interrupt_handler23,
	__interrupt_handler24,
	__interrupt_handler25,
	__interrupt_handler26,
	__interrupt_handler27,
	__interrupt_handler28,
	__interrupt_handler29,
	__interrupt_handler30,
	__interrupt_handler31,
#if 0
/* 1997-04-14  A.H
 *      SCU 割込み以外を削除
 */
	__interrupt_handler32,
	__interrupt_handler33,
	__interrupt_handler34,
	__interrupt_handler35,
	__interrupt_handler36,
	__interrupt_handler37,
	__interrupt_handler38,
	__interrupt_handler39,
	__interrupt_handler40,
	__interrupt_handler41,
	__interrupt_handler42,
	__interrupt_handler43,
	__interrupt_handler44,
	__interrupt_handler45,
	__interrupt_handler46,
	__interrupt_handler47,
	__interrupt_handler48,
	__interrupt_handler49,
	__interrupt_handler50,
	__interrupt_handler51,
	__interrupt_handler52,
	__interrupt_handler53,
	__interrupt_handler54,
	__interrupt_handler55,
	__interrupt_handler56,
	__interrupt_handler57,
	__interrupt_handler58,
	__interrupt_handler59,
	__interrupt_handler60,
	__interrupt_handler61,
	__interrupt_handler62,
	__interrupt_handler63,
	__interrupt_handler64,
	__interrupt_handler65,
	__interrupt_handler66,
	__interrupt_handler67,
	__interrupt_handler68,
	__interrupt_handler69,
	__interrupt_handler70,
	__interrupt_handler71,
	__interrupt_handler72,
	__interrupt_handler73,
	__interrupt_handler74,
	__interrupt_handler75,
	__interrupt_handler76,
	__interrupt_handler77,
	__interrupt_handler78,
	__interrupt_handler79,
	__interrupt_handler80,
	__interrupt_handler81,
	__interrupt_handler82,
	__interrupt_handler83,
	__interrupt_handler84,
	__interrupt_handler85,
	__interrupt_handler86,
	__interrupt_handler87,
	__interrupt_handler88,
	__interrupt_handler89,
	__interrupt_handler90,
	__interrupt_handler91,
	__interrupt_handler92,
	__interrupt_handler93,
	__interrupt_handler94,
	__interrupt_handler95,
	__interrupt_handler96,
	__interrupt_handler97,
	__interrupt_handler98,
	__interrupt_handler99,
	__interrupt_handler100,
	__interrupt_handler101,
	__interrupt_handler102,
	__interrupt_handler103,
	__interrupt_handler104,
	__interrupt_handler105,
	__interrupt_handler106,
	__interrupt_handler107,
	__interrupt_handler108,
	__interrupt_handler109,
	__interrupt_handler110,
	__interrupt_handler111,
	__interrupt_handler112,
	__interrupt_handler113,
	__interrupt_handler114,
	__interrupt_handler115,
	__interrupt_handler116,
	__interrupt_handler117,
	__interrupt_handler118,
	__interrupt_handler119,
	__interrupt_handler120,
	__interrupt_handler121,
	__interrupt_handler122,
	__interrupt_handler123,
	__interrupt_handler124,
	__interrupt_handler125,
	__interrupt_handler126,
	__interrupt_handler127,
	__interrupt_handler128,
	__interrupt_handler129,
	__interrupt_handler130,
	__interrupt_handler131,
	__interrupt_handler132,
	__interrupt_handler133,
	__interrupt_handler134,
	__interrupt_handler135,
	__interrupt_handler136,
	__interrupt_handler137,
	__interrupt_handler138,
	__interrupt_handler139,
	__interrupt_handler140,
	__interrupt_handler141,
	__interrupt_handler142,
	__interrupt_handler143,
	__interrupt_handler144,
	__interrupt_handler145,
	__interrupt_handler146,
	__interrupt_handler147,
	__interrupt_handler148,
	__interrupt_handler149,
	__interrupt_handler150,
	__interrupt_handler151,
	__interrupt_handler152,
	__interrupt_handler153,
	__interrupt_handler154,
	__interrupt_handler155,
	__interrupt_handler156,
	__interrupt_handler157,
	__interrupt_handler158,
	__interrupt_handler159,
	__interrupt_handler160,
	__interrupt_handler161,
	__interrupt_handler162,
	__interrupt_handler163,
	__interrupt_handler164,
	__interrupt_handler165,
	__interrupt_handler166,
	__interrupt_handler167,
	__interrupt_handler168,
	__interrupt_handler169,
	__interrupt_handler170,
	__interrupt_handler171,
	__interrupt_handler172,
	__interrupt_handler173,
	__interrupt_handler174,
	__interrupt_handler175,
	__interrupt_handler176,
	__interrupt_handler177,
	__interrupt_handler178,
	__interrupt_handler179,
	__interrupt_handler180,
	__interrupt_handler181,
	__interrupt_handler182,
	__interrupt_handler183,
	__interrupt_handler184,
	__interrupt_handler185,
	__interrupt_handler186,
	__interrupt_handler187,
	__interrupt_handler188,
	__interrupt_handler189,
	__interrupt_handler190,
	__interrupt_handler191,
	__interrupt_handler192,
	__interrupt_handler193,
	__interrupt_handler194,
	__interrupt_handler195,
	__interrupt_handler196,
	__interrupt_handler197,
	__interrupt_handler198,
	__interrupt_handler199,
	__interrupt_handler200,
	__interrupt_handler201,
	__interrupt_handler202,
	__interrupt_handler203,
	__interrupt_handler204,
	__interrupt_handler205,
	__interrupt_handler206,
	__interrupt_handler207,
	__interrupt_handler208,
	__interrupt_handler209,
	__interrupt_handler210,
	__interrupt_handler211,
	__interrupt_handler212,
	__interrupt_handler213,
	__interrupt_handler214,
	__interrupt_handler215,
	__interrupt_handler216,
	__interrupt_handler217,
	__interrupt_handler218,
	__interrupt_handler219,
	__interrupt_handler220,
	__interrupt_handler221,
	__interrupt_handler222,
	__interrupt_handler223,
	__interrupt_handler224,
	__interrupt_handler225,
	__interrupt_handler226,
	__interrupt_handler227,
	__interrupt_handler228,
	__interrupt_handler229,
	__interrupt_handler230,
	__interrupt_handler231,
	__interrupt_handler232,
	__interrupt_handler233,
	__interrupt_handler234,
	__interrupt_handler235,
	__interrupt_handler236,
	__interrupt_handler237,
	__interrupt_handler238,
	__interrupt_handler239,
	__interrupt_handler240,
	__interrupt_handler241,
	__interrupt_handler242,
	__interrupt_handler243,
	__interrupt_handler244,
	__interrupt_handler245,
	__interrupt_handler246,
	__interrupt_handler247,
	__interrupt_handler248,
	__interrupt_handler249,
	__interrupt_handler250,
	__interrupt_handler251,
	__interrupt_handler252,
	__interrupt_handler253,
	__interrupt_handler254,
	__interrupt_handler255,
#endif
};

/****************************************
*		declare public objects			*
****************************************/
#if 0
/* 1997-04-14  A.H
 *      SCU 割込み以外を削除
 */
interrupt_t	__interrupt_vector[256];
#else
interrupt_t	__interrupt_vector[INT_SCU_TOTAL];
#endif

/* SCU関数の登録 */
void	INT_SetScuFunc( int	n, interrupt_t	handler )
{
	interrupt_t	hndl;

	/* ライブラリ内ベクタテーブルへの登録 */
#if 0
/* 1997-04-14  A.H
 *      SCU 割込み以外を削除
 */
	__interrupt_vector[n] = handler;
#else
	__interrupt_vector[(n - INT_SCU_OFFSET)] = handler;
#endif

	/* 登録用ハンドラの設定 */
	if (handler == NULL) {
		/* 指定アドレスがNULLならばBOOT ROM内テーブルを初期化 */
		/* （SCUダミー関数を登録） */
		hndl = NULL;
	} else {
		/* それ以外ならばライブラリ内ハンドラを登録 */
#if 0
/* 1997-04-14  A.H
 *      SCU 割込み以外を削除
 */
		hndl = __interrupt_handler[n];
#else
		hndl = __interrupt_handler[(n - INT_SCU_OFFSET)];
#endif
	}

	/* SCU割り込みルーチンの登録 */
	_INT_SetScuFunc( n, hndl );

	return;
}


/* SCU関数アドレスの取得 */
interrupt_t	INT_GetScuFunc( int	n )
{
	interrupt_t handler;

	/* 登録されているSCU関数アドレスを取得 */
#if 0
/* 1997-04-14  A.H
 *      SCU 割込み以外を削除
 */
	handler = __interrupt_vector[n];
#else
	handler = __interrupt_vector[(n - INT_SCU_OFFSET)];
#endif

	/* 指定アドレスがNULLならばBOOT ROM内テーブルの値を返す */
	/* （SCUダミー関数のアドレス） */
	if (handler == NULL) {
		/* SCU割り込みルーチンの取得 */
		handler = _INT_GetScuFunc(n);
	}
	return	handler;
}


#else  /*   ifndef INT_VER_200    */

/* SCU関数の登録 */
void	INT_SetScuFunc( int	n, interrupt_t	handler )
{
	SYS_SetUintMacSave( (Uint32)n, (void *)handler );
}

/* SCU関数アドレスの取得 */
interrupt_t	INT_GetScuFunc( int	n )
{
	return( (interrupt_t)SYS_GetUintMacSave( (Uint32)n ) );
}

#endif  /*   ifndef INT_VER_200    */