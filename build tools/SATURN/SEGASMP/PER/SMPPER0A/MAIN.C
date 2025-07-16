/*-----------------------------------------------------------------------------
 *  FILE: smpper0.c
 *
 *  Copyright(c) 1994 SEGA
 *
 *  PURPOSE:
 *
 *      SMPCのペリフェラルデータ・システムデータ取得サンプルプログラム
 *
 *  DESCRIPTION:
 *
 *      ペリフェラルデータ、タイムデータ、システムデータを取得します。
 *      以下に書き込みアドレスを示します。
 *
 *          0x6040000 システムデータ
 *          0x6040020 タイムデータ
 *          0x6040030 マルチタップ情報
 *          0x6040040 ペリフェラルデータ
 *
 *      ペリフェラルデータ取得の設定は以下のとおりです。
 *			 NUM : #define def_num		(12)
 *			SIZE : #define def_size		(6)
 *
 *
 *  INTERFACE: １ＰのＰＡＤにて操作
 *		Ａボタン：システムデータフラグをモノラルに変更
 *		Ｂボタン：システムデータフラグをステレオに変更
 *		Ｃボタン：システムデータ 再読み込み＋表示
 *
 *
 *  CAVEATS:
 *
 *  AUTHOR(S)
 *
 *      1994-05-19  N.T Ver.0.90
 *      1994-08-10  N.T Ver.1.00
 *
 *  MOD HISTORY:
 *      1994-11-14  N.T Ver.1.01
 *      1996-09-19  A.H Ver.1.10  PC+CartDev+DevSaturn GNU環境にて作成
 *
 *-----------------------------------------------------------------------------
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
 * GLOBAL DECLARATIONS
 */

/*
 * LOCAL DEFINES/MACROS
 */

#define ADR_BASE      ((Uint32 *)0x6040000)

#define ADR_OUTPUT  (ADR_BASE)
#define ADR_O_SYS     (ADR_OUTPUT)              /* system                    */

#define ADR_O_PER_MUL   ((Uint8 *)(ADR_OUTPUT + 0x30/4))    /* per           */
#define ADR_O_PER   ((Uint8 *)(ADR_OUTPUT + 0x40/4))     /* per              */
#define ADR_O_TIM   ((Uint8 *)(ADR_OUTPUT + 0x20/4))    /* tim               */
#define def_num		(12)             /* NUM */
#define def_size	(6)             /* SIZE */
#define perWorkSize		((def_num * (def_size + 2) * 2 )+ def_size)
#define	lang	(status_sys & PER_MSK_LANGU)

/*
 * GLOBAL DECLARATIONS
 */
volatile Uint8 per_work[PER_WORK_SIZE(def_num, def_size)];  /*  SMPC LIB WORK 領域  */

/*
 * TYPEDEF
 */
typedef struct {
    Uint8   type;		/* ペリフェラルタイプ */
    Uint8   size;		/* ペリフェラルサイズ */
    Uint8   data[def_size];		/* データ */
}SmpPerData;

typedef struct{
	SmpPerData dummy[def_num];
}PerDataBuffer;


/*
 * STATIC DECLARATIONS
 */
static volatile Uint8 Flag_exec_LInit_SYS;	/* SMPC イントバック発行要求フラグ  */
static volatile Uint32 get_sys_flg;	/* SMPCシステムSTATUS 取得中フラグ  */
static Uint32 msk = OFF;	/* SMPCシステムSTATUS 取得中フラグ  */

static volatile SmpPerData	*per_data, *perdata_now, *perdata_old;
static volatile PerMulInfo *mul_info;	/*  マルチタップ情報格納アドレス  */

static PerGetSys *sys_data;  /* SMPCシステムSTATUS 取得アドレス */
Uint32	status_sys;    /*  SMPCメモリ情報格納用変数  */

volatile Uint32	flag_Vblank = ON;  /*  ON:VBLANK中 OFF:VBLANK外  */
volatile Uint16 diff;  /*  1P PAD 押されたボタン情報 */
static Uint8 *tim_data;


/*
 * STATIC FUNCTION PROTOTYPE DECLARATIONS
 */
static void	_intr_v_blank_in( void );  /* V BLANK IN 処理  */
void vblOutGo(void);					/* V BLANK OUT 処理  */
void	vdpInit(void);					/* 各種割込み初期化  */
static Uint32 LInit(PerKind);			/* SMPC イントバック発行 */
Uint16 chkPadInput(void);				/* PAD 入力データ取得処理 */

/******************************************************************************
 *
 * NAME:    main()      - メイン
 *
 ******************************************************************************
 */

void main(void)
{

    Flag_exec_LInit_SYS = ON;
    get_sys_flg = ON;
    flag_Vblank = ON;
	perdata_now = NULL;
	perdata_old = (SmpPerData *)per_work;

	vdpInit();    /*  VDP,SMPC 割込み登録   VDP初期化  */

	DBG_SetCursol( 0, 16 );
	DBG_Printf( "    BUTTON A:SET MONO\n");
	DBG_Printf( "    BUTTON B:SET STEREO\n");
	DBG_Printf( "    BUTTON C:SET RELOAD SYSTEM DATA\n");

    while(1){
        if(msk == ON){
	        if(NULL == (sys_data = PER_GET_SYS())){   /* 未終了 */
	        }else{
	            ((PerGetSys *)ADR_O_SYS)->cc = PER_GS_CC(sys_data);
	            ((PerGetSys *)ADR_O_SYS)->ac = PER_GS_AC(sys_data);
	            ((PerGetSys *)ADR_O_SYS)->ss = PER_GS_SS(sys_data);
	            ((PerGetSys *)ADR_O_SYS)->sm = status_sys = PER_GS_SM(sys_data);
	            ((PerGetSys *)ADR_O_SYS)->stat = PER_GS_SMPC_STAT(sys_data);
	            msk = OFF;
				DBG_SetCursol( 0, 4 );
				DBG_Printf( "  SYSTEM STATUS READ COMPLETE\n\n");
				DBG_Printf( "    LANG = ");
				switch(lang){
					case(PER_JAPAN):
						DBG_Printf("JAPAN   ");
						break;
					case(PER_ENGLISH):
						DBG_Printf("ENGLISH ");
						break;
					case(PER_FRANCAIS):
						DBG_Printf("FRANCAIS");
						break;
					case(PER_DEUTSCH):
						DBG_Printf("DEUTSCH ");
						break;
					case(PER_ITALIANO):
						DBG_Printf("ITALIANO");
						break;
					case(PER_ESPNOL):
						DBG_Printf("ESPNOL  ");
						break;
					default:   /*  バックアップデータ異状時  */
						DBG_Printf("ERROR   ");
				}  /* END switch(lang)  */
			}
        }

		diff = chkPadInput();
		if( (diff != 0) && (*(Uint8 *)(&diff)) & PER_LDGT_C)
		{   /*  SMPCシステムデータ取得イントバック発行 */
			Flag_exec_LInit_SYS = ON;
			get_sys_flg = ON;
		}

		if( msk == OFF && perdata_now != NULL){
			if( (*(Uint8 *)(&diff)) & PER_LDGT_A)
			{   /*  SMPCメモリ モノラルフラグ設定 */
				status_sys |= PER_MSK_STEREO;
				DBG_SetCursol( 0, 12 );
				DBG_Printf( "     PER_MSK_STEREO = %8X\n", PER_MSK_STEREO );
				PER_SMPC_SET_SM(status_sys);
#if 0
				set_sound_mono();		/*  サウンド出力設定の切換  */
#endif
			}
			if( (*(Uint8 *)(&diff)) & PER_LDGT_B)
			{   /*  SMPCメモリ ステレオフラグ設定 */
				status_sys &= ~(Uint32)(PER_MSK_STEREO);
				DBG_SetCursol( 0, 12 );
				DBG_Printf( "    ~PER_MSK_STEREO = %8X\n", ~(PER_MSK_STEREO) );
				PER_SMPC_SET_SM(status_sys);
#if 0
				set_sound_stereo();		/*  サウンド出力設定の切換  */
#endif
			}
		}

		while(flag_Vblank == ON);
        if(Flag_exec_LInit_SYS == ON ){
        	/* イントバックコマンド発行  */
            if(get_sys_flg == ON){
				/* システムデータ取得要求 */
                if(LInit(PER_KD_SYS) == PER_INT_OK){ /* コマンド発行正常終了 */
                	get_sys_flg = OFF;
                	msk = ON;	/*  システムデータ取得処理中 */
                }
            }else{
				/* ペリフェラルデータ取得要求 */
                if(LInit(PER_KD_PERTIM) == PER_INT_OK){/*コマンド発行正常終了*/
	                Flag_exec_LInit_SYS = OFF;
	            }
            }
		}else{              /*  イントバックコマンド発行済み  */
			if(per_data != NULL){
				/*   収集したペリフェラルデータをバッファに転送  */
				*(PerDataBuffer *)(ADR_O_PER) =*(PerDataBuffer *)per_data;
		        *(ADR_O_PER_MUL) = mul_info[0].id;
		        *(ADR_O_PER_MUL + 1) = mul_info[0].con;
		        *(ADR_O_PER_MUL + 2) = mul_info[1].id;
		        *(ADR_O_PER_MUL + 3) = mul_info[1].con;
				/*   収集した時刻データをバッファに転送  */
		        tim_data = PER_GET_TIM();
		        ADR_O_TIM[0] = tim_data[0];
		        ADR_O_TIM[1] = tim_data[1];
		        ADR_O_TIM[2] = tim_data[2];
		        ADR_O_TIM[3] = tim_data[3];
		        ADR_O_TIM[4] = tim_data[4];
		        ADR_O_TIM[5] = tim_data[5];
		        ADR_O_TIM[6] = tim_data[6];
			}
			DBG_SetCursol( 0, 9 );
			DBG_Printf( "   PER_GS_SM = %08X\n", PER_GS_SM(sys_data) );
        }
        
		DBG_SetCursol( 0, 11 );
		if(status_sys & PER_MSK_STEREO){    /*  1:MONO  */
			DBG_Printf( "   SMPC SOUND STATUS=MONO  " );
		}else{                              /*  0:STEREO  */
			DBG_Printf( "   SMPC SOUND STATUS=STEREO" );
		}
	SCL_DisplayFrame();			/*  V ブランク割込み待ち  */
    } /* --- while(1) の括弧 --- */
}


/******************************************************************************
 *
 * NAME:    vblOutGo()   - V_BRANK OUT　割り込み処理
 *
 ******************************************************************************
*/

/*  V_BRANK IN　割り込み処理 */
static void	_intr_v_blank_in( void ){
	flag_Vblank = ON;
	SCL_VblankStart();
}


/* V_BRANK OUT　割り込み処理 */
static void _intr_v_blank_out(void)
{
	SCL_VblankEnd();
    if(Flag_exec_LInit_SYS != ON){       /* イントバック発行中で無い  */
        /* ペリフェラルデータ取得コマンド発行  */
        PER_LGetPer((PerGetPer **)&per_data, (PerMulInfo **)&mul_info);
    }
/*    v_blank_out_cnt ++;  */
	flag_Vblank = OFF;
}

/*****************************************************************************/
static Uint32  LInit(PerKind set_kind)
{
	return(
	    PER_LInit(set_kind, def_num, def_size, (Uint8 *)per_work, 0)
	);
}

void	vdpInit(void){
	Uint16	dummy_pad = 0;

    /* 割り込み設定 ********************************************************/
	INT_ChgMsk( INT_MSK_NULL, INT_MSK_VBLK_IN|INT_MSK_VBLK_OUT );
	SCL_Vdp2Init();
	SCL_SetPriority(SCL_SP0|SCL_SP1|SCL_SP2|SCL_SP3|SCL_SP4|SCL_SP5|SCL_SP6|SCL_SP7,7);
	SCL_SetSpriteMode(SCL_TYPE1,SCL_MIX,SCL_SP_WINDOW);
	SCL_SetColRamMode(SCL_CRM15_2048);
	
	INT_SetScuFunc( INT_SCU_VBLK_IN,  _intr_v_blank_in  );
	INT_SetScuFunc(INT_SCU_VBLK_OUT, _intr_v_blank_out);
	INT_ChgMsk( INT_MSK_VBLK_IN|INT_MSK_VBLK_OUT, INT_MSK_NULL );

	SCL_SetFrameInterval( 1 );
	DBG_Initial( &dummy_pad, RGB16_COLOR(31,31,31), 0 );
	DBG_DisplayOn();
	DBG_ClearScreen();
}

Uint16 chkPadInput(void){
static	Uint16 pad_diff = (Uint16)NULL;
	if(per_data != NULL){  /*  イントバック正常終了？  */
	/*  本体端子１の接続チェック  */
		perdata_now = (SmpPerData *)per_data;
		if((mul_info[0].con != PER_MCON_NCON_UNKNOWN) &&
			((perdata_now->type == PER_ID_DGT) ||
			 (perdata_now->type == PER_ID_ANL) ||
			 (perdata_now->type == PER_ID_KBD) ) &&
			(perdata_now->size >= PER_SIZE_DGT)	)
		{	/*  トリガ情報 チェック  */
			pad_diff =	*(Uint16 *)(perdata_now->data)
						^ *(Uint16 *)(perdata_old->data);
			pad_diff &= *(Uint16 *)(perdata_old->data);
			perdata_old = perdata_now;
		}
	}
	return(pad_diff);
}