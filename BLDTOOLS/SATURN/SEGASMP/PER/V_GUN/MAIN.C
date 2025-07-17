/****************************************
*			include files				*
****************************************/
#define	_SPR2_
#include	"sega_spr.h"

#include	"machine.h"
#include	"sega_int.h"
#include	"sega_scl.h"
#include	"sega_cdc.h"
#include	"sega_sys.h"

#include	"image.h"

#include	"per_x.h"
#include	"smpc.h"
#include	"vdp2.h"

/****************************************
*		declare public objects			*
****************************************/
Uint16	tvstat;
volatile trigger_t	PadData1  = 0x0000;
volatile trigger_t	PadData1E = 0x0000;
volatile trigger_t	PadData2  = 0x0000;
volatile trigger_t	PadData2E = 0x0000;
SysPort	*__port = NULL;
volatile int g_exle = 0;	/* 外部ラッチ入力イネーブルフラグ  0:OFF 1:ON */
volatile Uint16	back_color = 0;

/****************************************
*		declare private objects			*
****************************************/
#if 0
static SysPort	*__port;
#endif

/****************************************
*			declare functions			*
****************************************/
void	_spr2_transfercommand( void );
void	_spr2_transferimage( void	*image, Uint32	size );
void	_spr2_initialize( void );
void	_spr2_setspritecoord( int	x, int	y );
void	_spr2_getspritecoord( int	*x, int	*y );
void	_spr2_setscreencolor( int	color );
void	move_cursor( const SysPort	*port, int	*x, int	*y );
trigger_t	get_trigger( const SysPort	*port );

extern	void scl_main(void);

/* トレイオープンチェック */
void	chk_tray_open(void)
{
    Sint32 hirq;

	/* A-Busアクセスが入るので、SCU-DMA中には実行しないこと */
	hirq = CDC_GetHirqReq();
	if ((hirq & CDC_HIRQ_DCHG) != 0) {
		/* CDトレイオープンの場合 */
	    set_imask(15);
	    CDC_SetHirqMsk(0);
		/* マルチプレーヤの起動 */
		SYS_EXECDMP();
	}
    return;
}


static void	smpper12( SysPort	*port ){
#if 0   /* 1997-09-02  A.H(SOJ)  使用しなくなった為 */
	Uint16	color = 0;
#endif
	int		x,y;


	_spr2_getspritecoord( &x, &y );
	for(;;){
		trigger_t	trigger;


/*  V-BLANK-OUT 同期処理 */
/*  バーチャガンのトリガ検出が、画面表示期間開始から遅れてはならない */
#if 0
		SCL_DisplayFrame();
#else
		chk_tray_open();

		if( !(~PadData1E & PER_DGT_S ) ){
			if( !(~PadData1 & ( PER_DGT_A | PER_DGT_B | PER_DGT_C ) ) ){
				SYS_Exit(0);  /* FLASH SEGASATURN 対応 */
			}
		}

		SCL_DisplayFrame();
#endif

		trigger = get_trigger( port );
#if	1
		
		if( back_color != 0 ){
			move_cursor( port, &x, &y );
			_spr2_setspritecoord( x, y );
		}
		
	#if 0  /* 1997-09-02  A.H(SOJ)  白フラッシュが 1フレーム遅れるバグを修正 */
		_spr2_setscreencolor( color );
		_spr2_transfercommand();

		if( trigger & (TRG_C|TRG_START))   /* V_GUN トリガーチェック */
			color = 0xffff;
		else
			color = 0x0000;
	#else   /* スプライトでは 画面フラッシュの同期が間に合わない為 */
			/* VDP2 のバックカラーを使用する様に 変更 */
		_spr2_transfercommand();

		if( trigger & (TRG_C|TRG_START)) {  /* V_GUN トリガーチェック */
			VDP2_EXTEN = VDP2_EXTEN_EXLTEN;   /* ラッチ入力 有効 */
			VDP2_BGON = 0;  /*  BG面を消す */
			back_color = 0xffff;      /* バックカラーを白 */
			g_exle = 1;   /* 画面フラッシュ */
		}else{
			back_color = 0;  /* バックカラーを黒 */
			g_exle = 0;
		}
		/* VRAM上の バックカラーデータを直接書き換え */
		*(volatile Uint16*)(SCL_VDP2_VRAM+0x80000-2) = back_color;
	#endif

#else
		move_cursor( port, &x, &y );
		_spr2_setscreencolor( 0xffff );
		_spr2_setspritecoord( x, y );
		_spr2_transfercommand();
#endif

	}
}

static void	intr_v_blank_in( void ){
	VDP2_BGON = 1;  /* NBG0 表示をイネーブル */
	SCL_VblankStart();
}

static void	intr_v_blank_out( void ){
	SCL_VblankEnd();
	
	SMPC_IOSEL = 0;   /* port1,port2 を SMPCコントロールモード */
#if 0  /* 1997-09-02  A.H(SOJ) */
	SMPC_EXLE  = 0;   /*       VDP2外部ラッチ入力 ディセーブル */
#endif
	SMPC_DDR(0) = 0;  /* PORT1 全ビットを入力 */
	SMPC_DDR(1) = 0;  /* PORT2 全ビットを入力 */
	
	VDP2_EXTEN = VDP2_EXTEN_EXLTEN;   /* ラッチ入力 有効 */

#if 0
	if( g_exle ){
		VDP2_EXTEN = VDP2_EXTEN_EXLTEN;   /* ラッチ入力 有効 */
		*(volatile Uint16*)(SCL_VDP2_VRAM+0x80000-2) = back_color;
	}else{
		*(volatile Uint16*)(SCL_VDP2_VRAM+0x80000-2) = back_color;
	}
#endif

#if 0
	PER_GetPort( __port );
#else   /* FSS 対応  1997-09-02 */
	if( __port != NULL ){
		const SysDevice	*device;
		
		PER_GetPort( __port );
		
		if(( device = PER_GetDeviceR( &__port[0], 0 )) != NULL ){
			trigger_t	prev = PadData1;
			trigger_t	current = PER_GetTrigger( device );
			
			PadData1  = current;
			PadData1E = PER_GetPressEdge( prev, current );
		}
		else{
			PadData1 = PadData1E = 0;
		}
		
		if(( device = PER_GetDeviceR( &__port[1], 0 )) != NULL ){
			trigger_t	prev = PadData2;
			trigger_t	current = PER_GetTrigger( device );
			
			PadData2  = current;
			PadData2E = PER_GetPressEdge( prev, current );
		}
		else{
			PadData2 = PadData2E = 0;
		}
	}
#endif
	tvstat = VDP2_TVSTAT;
}

int	main( void ){
	int imask;


	chk_tray_open();      /* 初期化の先頭で トレイオープンチェック */

	imask = get_imask();
	set_imask(15);   /* sh2 全割込み禁止 */
	SCL_Vdp2Init();
	SCL_SetPriority(SCL_SP0|SCL_SP1|SCL_SP2|SCL_SP3|
					SCL_SP4|SCL_SP5|SCL_SP6|SCL_SP7,7);
	SCL_SetSpriteMode(SCL_TYPE1,SCL_MIX,SCL_SP_WINDOW);
	SCL_SetColRamMode(SCL_CRM24_1024);
	
	__port = PER_OpenPort();
	
	INT_ChgMsk( INT_MSK_NULL, INT_MSK_VBLK_IN|INT_MSK_VBLK_OUT );
	INT_SetScuFunc( INT_SCU_VBLK_IN,  intr_v_blank_in  );
	INT_SetScuFunc( INT_SCU_VBLK_OUT, intr_v_blank_out );
	INT_ChgMsk( INT_MSK_VBLK_IN|INT_MSK_VBLK_OUT, INT_MSK_NULL );

	set_imask(imask);
	
	_spr2_initialize();
	_spr2_transfercommand();
	_spr2_transferimage( cursor_image, sizeof( cursor_image ));
	
	scl_main();
	SCL_SetFrameInterval( 1 );
	
	smpper12( __port );
	
	while(1);
}
