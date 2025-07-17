/*****************************************************************************
 *	インクルードファイル
 *****************************************************************************/

#if 1
/*  for SBL  */
#include "sega_cdc.h"
#include "sega_gfs.h"
#include "sega_stm.h"
#include "sega_scl.h"
#else
/*  for SGL  */
#include "sgl.h"
#include "sl_def.h"
#include "sgl_cd.h"
#endif

#include "machine.h"
#include	"fss.h"

#include	"sega_sys.h"

/*****************************************************************************
 *	関数の宣言
 *****************************************************************************/

/*****************************************************************************
 *	定数マクロ
 *****************************************************************************/


/*****************************************************************************
 *	関数の定義
 *****************************************************************************/
#if 0  /* for SGL */
/* A+B+C+START チェック */
void	chk_pad_reset(void)
{
	if( !(Smpc_Peripheral[0].push & PER_DGT_ST) ){
		if( !(Smpc_Peripheral[0].data & (PER_DGT_TA | PER_DGT_TB | PER_DGT_TC ))){
			SYS_Exit(0);  /* FLASHシステム対応 */
		}
	}
}
#endif

/* トレイオープンチェック */
void	chk_tray_open(void)
{
    Sint32 hirq;

	/* A-Busアクセスが入るので、SCU-DMA中には実行しないこと */
	hirq = CDC_GetHirqReq();
	if ((hirq & CDC_HIRQ_DCHG) != 0) {
		/* CDトレイオープンの場合 */
#if 1
	    set_imask(CPU_INT_MASK);
	    CDC_SetHirqMsk(0);
#endif
		/* マルチプレーヤの起動 */
		SYS_EXECDMP();
	}
    return;
}

