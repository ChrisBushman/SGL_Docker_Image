/*======================================================================
 *		strt2_g.c -- サンプルスタートモジュール２
 *  	Copyright(c) 1994 SEGA
 *  	Written by H.E on 1994-02-15 Ver.0.80
 *  	Updated by M.K on 1994-11-11 Ver.1.10
 *	Note:
 *  	Ｃモジュール実行環境初期化処理モジュールで以下のルーチンを含む。
 *  		_INIT		-- Ｃモジュール実行開始エントリルーチン
 *  		_INITSCT    -- データセクション初期化処理
 *======================================================================*/
#include <stdlib.h>
#include <machine.h>
#include <sega_xpt.h>

extern Uint8 *_MD_BGN;
extern Uint8 *_D_BGN, *_D_END;
extern Uint8 *_B_BGN, *_B_END;

extern void main(void);

void _INITSCT();

void _INIT()
{
	_INITSCT();
    main();
    for (;;)
		;
}

void _INITSCT()
{
	Uint8 *src,*dst;

	/*  Clear B Section */
	for (dst = _B_BGN; dst < _B_END; dst++)
		*dst = 0;
	/*  Copy  D Section */
	src = _MD_BGN;
	dst = _D_BGN;
	if (src != dst) {
	    for (; dst < _D_END; dst++, src++)
		*dst = *src;
	}
}

/*===== End of file ====================================================*/
