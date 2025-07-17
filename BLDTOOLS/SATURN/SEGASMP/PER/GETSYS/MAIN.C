/*--------------------------------------------------------------------------
 *  FILE: smpper0.c
 *
 *  Copyright(c) 1994 SEGA
 *
 *  PURPOSE:
 *
 *      SMPCのシステムデータ取得サンプルプログラム
 *
 *  DESCRIPTION:
 *
 *      システムデータを取得し、現在のサウンド設定を画面に表示します。
 *
 *      ペリフェラルデータ取得の設定は以下のとおりです。
 *			 NUM : #define def_num		(12)
 *			SIZE : #define def_size		(6)
 *
 *  操作法：1P のＰＡＤにて操作
 *		Ａボタン：システムデータフラグをMONOに変更
 *		Ｂボタン：システムデータフラグをSTEREOに変更
 *		Ｃボタン：システムデータ 再読み込み＋表示
 *
 *
 *  AUTHOR(S)
 *
 *      1994-05-19  N.T Ver.0.90
 *      1994-08-10  N.T Ver.1.00
 *
 *  MOD HISTORY:
 *      1994-11-14  N.T Ver.1.01
 *      1996-09-19  A.H Ver.1.02   CartDev+DevSaturn GNU環境にて作成
 *
 *------------------------------------------------------------------------
 */

/*
 * C STANDARD LIBRARY FUNCTIONS/MACROS DEFINES
 */
#include	"machine.h"
#include	<string.h>
#include	"sega_xpt.h"
#include	"sega_per.h"
#include	"sega_tim.h"
#include	"sega_int.h"
#include	"sega_scl.h"
#include	"sega_dbg.h"

/*
 * C VIRTUAL TYPES DEFINITIONS
 */

/*
 * USER SUPPLIED INCLUDE FILES
 */

/*
 * LOCAL DEFINES/MACROS
 */

#define def_num		(12)             /* PER NUM */
#define def_size	(6)              /* PER SIZE */
#define perWorkSize		((def_num * (def_size + 2) * 2 )+ def_size)

/* SMPC イントバックコマンド発行 */
#define LInit(KIND_TYPE)		PER_LInit(\
									KIND_TYPE,    /* KIND */ \
									def_num,      /* NUM  */ \
									def_size,     /* SIZE */ \
									per_work,     /* WORK */ \
									0)     /* V_BLANK SKIP */

/*
 * TYPEDEF
 */
typedef struct {
	Uint8   type;
	Uint8   size;
	Uint16  data;
#if (def_size > 2)
	Uint8   ex_dat[def_size - 2];
#endif
}SmpPerData;

/*
 * GLOBAL DECLARATIONS
 */
Uint8 per_work[perWorkSize];  /*  SMPC LIB WORK  */
Uint8 Flag_exec_LInit_SYS;	/* PER_LInit コマンド発行要求フラグ  */
								/*    ON :次フレームにてLInit()実行  */
								/*    OFF:                           */
volatile PerKind per_kind_type; /* PER_LInit コマンド種類 */
									/* PER_KD_SYS */
									/* PER_KD_PERTIM */
									/* OFF */
PerGetSys *sys_data;  /* SMPCシステムデータ 取得アドレス */

volatile SmpPerData	*per_data,	 *per_data_now,	per_data_old;
volatile PerMulInfo *mul_info;	/*  マルチタップ情報格納アドレス  */
volatile Uint16 push = 0xffff;  /*  1P PAD 押されたボタン情報 */
Uint16	dummy_pad = 0;

volatile Uint32	flag_Vblank = ON;  /*  ON:VBLANK期間中 */
                                   /* OFF:VBLANK期間外 */

Uint32	vBlankCount = 0;

/*
 * STATIC DECLARATIONS
 */

/*
 * FUNCTION PROTOTYPE DECLARATIONS
 */
static void	_intr_v_blank_in( void );  /* V BLANK IN 処理  */
static void	_intr_v_blank_out( void ); /* V BLANK OUT 処理  */

void	vdpInit(void);					/* 各種割込み初期化  */
Uint16 chkPadInput(void);				/* PAD 入力データ取得処理 */


Uint32 getSystemData(void)
{
	Uint32	smpcMemory;   /* SMPCメモリ格納用変数  */
	if( (sys_data = PER_GET_SYS()) != NULL)
	{	/* systemデータ取得成功 */
		smpcMemory = PER_GS_SM(sys_data);
		DBG_SetCursol( 0, 15 );
		DBG_Printf( "  SYSTEM STATUS READ COMPLETE  \n\n");
		DBG_Printf( "    PER_GS_SM = %8X\n\n", smpcMemory );
		if(smpcMemory & PER_MSK_STEREO){
			DBG_Printf( "     SMPC SOUND STATUS=MONO  \n" );
		/*	set_sound_mode(mono); */ /* サウンド出力設定の切換  */
		}else{
			DBG_Printf( "     SMPC SOUND STATUS=STEREO\n" );
		/*	set_sound_mode(stereo); */ /* サウンド出力設定の切換  */
		}
		return(smpcMemory);
	}else{  /* systemデータ取得失敗 */
		DBG_SetCursol( 0, 15 );
		DBG_Printf( "  SYSTEM STATUS READ INCOMPLETE\n\n");
		DBG_Printf( "    PER_GS_SM = ???????? \n\n" );
		DBG_Printf( "     SMPC SOUND STATUS=??????\n" );
		return(0);
	}
}


/* V_BRANK IN　割り込み処理 */
static void	_intr_v_blank_in( void ){
	flag_Vblank = ON;
	SCL_VblankStart();
}


/* V_BRANK OUT　割り込み処理 */
static void _intr_v_blank_out(void)
{
	SCL_VblankEnd();
	if(Flag_exec_LInit_SYS == OFF){   /* イントバック発行中で無い  */
		/* ペリフェラルデータ取得コマンド発行  */
		PER_LGetPer((PerGetPer **)&per_data, (PerMulInfo **)&mul_info);
	}
	vBlankCount++;
	flag_Vblank = OFF;
}

/*****************************************************************************/

Uint16 chkPadInput(void)
{
	static	Uint16 pad_push = 0xffff; /* 初期データは全ボタン未入力 */
	static	Uint16 pad_old = 0xffff; /* 初期データは全ボタン未入力 */
	if(per_data != NULL){  /*  イントバック正常終了？  */
	/*  本体端子１の接続チェック  */
		if(
			(mul_info[0].con != PER_MCON_NCON_UNKNOWN) &&  /* conect check */
			(
			 (per_data->type == PER_ID_DGT) ||   /* per_type check */
			 (per_data->type == PER_ID_ANL) ||
			 (per_data->type == PER_ID_KBD)
			) &&(per_data->size >= PER_SIZE_DGT)  /* per_size check */
		  )
		{	/*  トリガ情報 チェック  */
			pad_push =	( ~(pad_old) | (per_data->data) );
			pad_old = per_data->data;
		}
	}
	return(pad_push);
}

void	vdpInit(void)
{
	SCL_Vdp2Init();

	/* 割り込み設定 ********************************************************/
	INT_ChgMsk( INT_MSK_NULL, INT_MSK_VBLK_IN|INT_MSK_VBLK_OUT );
	SCL_SetPriority(SCL_SP0|SCL_SP1|SCL_SP2|SCL_SP3|SCL_SP4|SCL_SP5|SCL_SP6|SCL_SP7,7);
	SCL_SetSpriteMode(SCL_TYPE1,SCL_MIX,SCL_SP_WINDOW);
	SCL_SetColRamMode(SCL_CRM15_2048);
	
	INT_SetScuFunc( INT_SCU_VBLK_IN,  _intr_v_blank_in  );
	INT_SetScuFunc( INT_SCU_VBLK_OUT, _intr_v_blank_out );
	INT_ChgMsk( INT_MSK_VBLK_IN|INT_MSK_VBLK_OUT, INT_MSK_NULL );

	SCL_SetFrameInterval( 1 );
	DBG_Initial( &dummy_pad, RGB16_COLOR(31,31,31), 0 );
	DBG_DisplayOn();
	DBG_ClearScreen();
	DBG_SetCursol( 0, 3 );
	DBG_Printf( "   GET SMPC_STATUS(STEREO/MONO) SAMPLE\n\n");
	DBG_Printf( "      BUTTON A:SET SOUND <-MONO\n");
	DBG_Printf( "      BUTTON B:SET SOUND <-STEREO\n\n");
	DBG_Printf( "      BUTTON C:GET SMPC_STATUS\n");
}

Uint32	execLInit(void)
{
	Uint32 smpcMemory;

	if(LInit(per_kind_type) == PER_INT_OK) /* PER_LInit()実行結果チェック */
	{	/* PER_LInit()正常終了 */
		if(per_kind_type == PER_KD_SYS){
			per_kind_type = PER_KD_PERTIM;
					 /* 次フレームでペリフェラルデータ収集の
					     PER_LInit()を発行する */
		}else if(per_kind_type == PER_KD_PERTIM){
		  /* ペリフェラルデータ取得用 PER_LInit()発行後 */
			smpcMemory = getSystemData();
			Flag_exec_LInit_SYS = OFF;
			return(smpcMemory);
		}
	}
	return(0);
}


/*********************
 * NAME:    main()
 *********************/

void main(void)
{
	Uint32 mainCount = 0;
	Uint32 smpcMemory;
	flag_Vblank = ON;
	per_data_now = NULL;
	per_data = NULL;

	per_data_old = *(SmpPerData *)per_work;  /* ダミーアドレス  */
	per_kind_type = PER_KD_SYS; /* システム情報を取得 */
	Flag_exec_LInit_SYS = ON;  /* PER_LInit()実行 */
	sys_data = NULL;

	vdpInit();    /*  VDP,SMPC 割込み登録 */
	execLInit();  /*  ペリフェラルライブラリ初期化  */   
	SCL_DisplayFrame();			/*  V ブランク割込み待ち  */
	smpcMemory = execLInit();	/* 起動時のシステム情報取得 */
	while(1)   /*  main loop  */
	{
		if( per_data != NULL ){  /* ペリフェラルデータ取得済み */
			push = chkPadInput();
			if( (push & PER_DGT_A) == 0)
			{   /*  SMPCメモリ モノラルフラグ設定 */
				smpcMemory |= PER_MSK_STEREO;
				PER_SMPC_SET_SM(smpcMemory);
			/*	set_sound_mode(mono);	*/	/*  サウンド出力設定の切換等  */
			}
			if( (push & PER_DGT_B) == 0)
			{   /*  SMPCメモリ ステレオフラグ設定 */
				smpcMemory &= ~(PER_MSK_STEREO);
				PER_SMPC_SET_SM(smpcMemory);
			/*	set_sound_mode(stereo);	*/	/*  サウンド出力設定の切換等  */
			}
			if( (push & PER_DGT_C) == 0)
			{   /*  SMPCシステムデータ取得コマンド発行 */
				per_kind_type = PER_KD_SYS;
				Flag_exec_LInit_SYS = ON;
			}
		}
		if(Flag_exec_LInit_SYS == ON )	 /* PER_LIint()を実行するか？  */
			smpcMemory = execLInit();
		DBG_SetCursol( 0, 24 );
		DBG_Printf( "  PAD1 DATA = %4X\n", per_data->data);
		DBG_Printf( "  PAD1_PUSH = %4X\n", push);
		DBG_Printf( "  MAIN LOOP COUNTER = %8X\n", mainCount);
		DBG_Printf( "  V-BLANK COUNTER   = %8X", vBlankCount);
		SCL_DisplayFrame();			/*  V ブランク割込み待ち  */
		mainCount++;
	} /*  main loop   */
}
