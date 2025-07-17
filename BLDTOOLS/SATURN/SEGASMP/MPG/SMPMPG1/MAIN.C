/********************************************************************
 *      ソフトウェアライブラリ
 *
 *      Copyright (c) 1995 CSK Research Institute Corp.
 *      Copyright (c) 1995 SEGA
 *
 *  Library: MPEGライブラリ
 *  Module : MPEG再生テスト
 *  File   : smpmpg1.c
 *  Date   : 1997-03-26
 *  Version: 1.1
 *  Auther : H.T
 *  Modifier: (CRI),K.K
 *******************************************************************/

/*******************************************************************
 *      インクルードファイル
 *******************************************************************/
#include        <ctype.h>
#include        <stdarg.h>
#include        <machine.h>
#include        "sega_cdc.h"
#include        "sega_def.h"
#include        "sega_mth.h"
#include        "sega_scl.h"
#include        "sega_int.h"
#include        "sega_gfs.h"
#include        "sega_stm.h"
#include        "sega_sys.h"
#include        "sega_mpg.h"
#include        "sega_snd.h"

/*******************************************************************
 *      デバッグ用定数マクロ
 *******************************************************************/

#if 1
#define USE_SDDRVS_DAT  /* サウンドドライバの実行バイナリを、CD-ROMからリードしない */
#endif

/*******************************************************************
 *      定数マクロ
 *******************************************************************/

/* 再生するトラック番号 */
#define S_TNO           2                       /* 開始トラック */
#define E_TNO           2                       /* 終了トラック */

/* TOC情報のインデックス */
#define TOC_ETNO_INF    100                     /* 最終トラック情報 */
#define TOC_LOUT_INF    101                     /* リードアウト情報 */

/* CDブロックタイムアウト時間 */
#define IPL_TMOUT_COUNT 400000

/*==================================================================
  =     MPEGストリームのオープン
  ==================================================================*/

/* 同時に開くファイルの最大数 */
#define FILE_OPEN_MAX           10

/* ディレクトリ情報の最大数 */
#define MAX_DIR_ID              10

/* 同時に開くストリームグループの最大数 */
#define SGRP_OPEN_MAX           2

/* 同時に開くストリームの最大数 */
#define STRM_OPEN_MAX           4

/*==================================================================
  =     その他
  ==================================================================*/

/*-------------------------------*
 *    VDP2(スクロールVDP関連)    *
 *-------------------------------*/
#define SCRL_REG        0x25f80000        /* SCROLL REGISTER */

/*******************************************************************
 *      処理マクロ
 *******************************************************************/

/* レジスタアドレスからポインタへの変換 */
#define PTR_WD(adr)     ((volatile Uint16 *)(adr))

/* 画面ステータスレジスタ */
#define SCRL_TVSTS      PTR_WD(SCRL_REG+0x04)

/* V-BLANK期間のチェック */
#define IS_VBLANK()     (((*SCRL_TVSTS) & 0x08) != 0)

/*******************************************************************
 *      データ型の定義
 ******************************************************************/

typedef void (*VbFunc)(void);

/*******************************************************************
 *      変数定義
 *******************************************************************/

/* VSYNCカウンタ */
static volatile Sint32  vsync_cnt;

/* gfsライブラリのための作業領域 */
static Uint32   file_work[GFS_WORK_SIZE(FILE_OPEN_MAX) / 4];

/* stmライブラリのための作業領域 */
static Uint8    strm_work[STM_WORK_SIZE(SGRP_OPEN_MAX, STRM_OPEN_MAX)];

/* TOC情報 */
static Uint32   toc[102];

/* Vblank-in割り込み登録関数 */
static VbFunc   vbi_func;

/*******************************************************************
 *      関数宣言
 *******************************************************************/

void    main(void);
static void     initHw(void);
static void     getSysClk(Sint32 *dmode2);
static void     setVdp2(Sint32 dmode2);
static void     scrollInit(Sint32 mode2);
static void     setVblank(void);
static void     usrVblankStart(void);
static void     usrVblankEnd(void);
static void     waitVblank(void);
static void     setVbiFunc(VbFunc vbf);
static void     vsync_proc(void);
static void     waitVsync(Sint32 vinc);
static void     setScsp(void);
static void     initSw(void);
static void     initCdb(void);
static Bool     isHirqOn(Sint32 flag);
static Bool     initGfsStm(void);
static StmGrpHn stmOpen(StmHn *stm_v, StmHn *stm_a);
static void     tnoToFad(Sint32 *sfad, Sint32 *efas);
static void     createMpHandle(MpgMv *mpgmv, MpgWn *mpgwn,
                               StmHn stm_a, StmHn stm_v);
static void     resVblank();    
static void     errFunc(Sint32 errlvl);

/*******************************************************************
 *      関数定義
 *******************************************************************/

/* メイン */
void    main(void)
{
    MpgMv       mpgmv;
    MpgWn       mpgwn;
    StmHn       stm_a, stm_v;
    StmGrpHn    grp;
    
    vsync_cnt = 0;
    
    /* ハードウェアの初期化 */
    initHw();
    
    /* ソフトウェアの初期化 */
    initSw();
    
    /* MPEGストリームのオープン */
    grp = stmOpen(&stm_v, &stm_a);
    
    /* サーバ実行グループの指定 */
    STM_SetExecGrp(grp);
    
    /* MPEGハンドルの作成 */
    createMpHandle(&mpgmv, &mpgwn, stm_a, stm_v);

    while (TRUE) {
        /* MPEG 再生の開始 */
        if (MPG_MvStart(mpgmv, ON) != MPG_ERR_OK) {
            errFunc(-11);
        }
        while (TRUE) {
            /* VSYNC との同期 */
            waitVsync(1);
            
            /* MPEGシステムの状態の取り込み */
            MPG_CaptStat();
            
            if (MPG_MvGetVideoStat(mpgmv) == MPG_VSTAT_STOP &&
                MPG_MvGetAudioStat(mpgmv) == MPG_ASTAT_STOP) {
                break;
            }
            STM_ExecServer();
        }
    }
    
    /* MPEGハンドルの消去 */
    MPG_WnDestroy(mpgwn);
    MPG_MvDestroy(mpgmv);
    
    /* ストリームグループのクローズ */
    STM_CloseGrp(grp);

    /* V-BLANK割り込みルーチンの登録解除 */
    resVblank();    

    while (TRUE) {
        ;
    }
    return;
}

/* ハードウェアの初期化 */
static void     initHw(void)
{
    Sint32      dmode2;
    
    /* システムクロックの取得 */
    getSysClk(&dmode2);
    
    /* ＶＤＰ２の設定 */
    setVdp2(dmode2);
    /* サウンドブロックの設定 */
    setScsp();
}

/* システムクロックの取得 */
static void     getSysClk(Sint32 *dmode2)
{
    Sint32      clock;
    
    /* システムクロック値の参照 */
    clock = SYS_GETSYSCK;
    
    if (clock == 0) {
        *dmode2 = SCL_NORMAL_A;
    } else if (clock == 1){
        *dmode2 = SCL_NORMAL_B;
    } else {
        errFunc(-5);
    }
    return;
}

/* ＶＤＰ２の設定 */
static void     setVdp2(Sint32 dmode2)
{
    /* ＶＤＰ２の初期化 */
    scrollInit(dmode2);
    
    /* 割り込み処理関数登録 */
    setVbiFunc(vsync_proc);
    waitVsync(5);
    
    /* 外部信号の設定 */
    Scl_s_reg.extenbl = 0x0003;

    /* キャラクタコントロールの設定 */
    Scl_d_reg.charcontrl0 = 0x3000;

    /* レジスタに反映させる */
    SclProcess = 1;

    waitVsync(5);
    
    return;
}

/* ＶＤＰ２の初期化 */
static void     scrollInit(Sint32 mode2)
{
    Sint32      intmask;
    
    /* 割り込み禁止処理 */
    intmask = get_imask();
    set_imask(15);
    
    /* SCLライブラリ初期化 */
    SCL_Vdp2Init();

    SCL_SetDisplayMode(SCL_SINGLE_INTER, SCL_240LINE, mode2);

    SCL_SetPriority(SCL_NBG1,7);

    vbi_func = NULL;

    /* V-BLANK割り込みルーチンの登録 */
    setVblank();
    
    /* 割り込み禁止解除処理 */
    set_imask(intmask);
    
    /* Vblank処理が終るのを待つ */
    waitVblank();
    
    return;
}

/* V-BLANK割り込みルーチンの登録 */
static void     setVblank(void)
{
    /* V_Blank 割り込みルーチンの登録 */
    INT_ChgMsk(INT_MSK_NULL, INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);
    INT_SetScuFunc(INT_SCU_VBLK_IN, usrVblankStart);
    INT_SetScuFunc(INT_SCU_VBLK_OUT, usrVblankEnd);
    INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT, INT_MSK_NULL);

    return;
}

static void     usrVblankStart(void)
{
    /* グラフィックライブラリを使用する為には実行しなければならない */
    SCL_VblankStart();
    
    if (vbi_func != NULL) {
        vbi_func();
    }
}

static void     usrVblankEnd(void)
{
    /* グラフィックライブラリを使用する為には実行しなければならない */
    SCL_VblankEnd();
}

/* Vblank処理が終るのを待つ */
static void     waitVblank(void)
{
    /* Vblank期間になるのを待つ */
    while (IS_VBLANK() == FALSE) {
        ;
    }
    /* Vblank期間でなくなるのを待つ */
    while (IS_VBLANK()) {
        ;
    }
    return;
}

/* 割り込み処理関数登録 */
static void     setVbiFunc(VbFunc vbf)
{
    vbi_func = vbf;
}

/* VSYNC 割り込み関数 */
static void     vsync_proc(void)
{
    vsync_cnt++;
    
    return;
}

/* 数VSYNC待つ(setVbiFunc()実行後によぶこと) */
static void     waitVsync(Sint32 vinc)
{
    Sint32      oldvcnt;
    
    oldvcnt = vsync_cnt;
    while (vsync_cnt < oldvcnt+vinc) {
        ;
    }
    return;
}

#ifndef USE_SDDRVS_DAT
/* ファイルのロード */
static Sint32 fileLoad(Sint8 *name, void *addr, Sint32 bsize)
{
	Sint32 		fid, i;

	for (i = 0; i < 10; i++) {
		fid = GFS_NameToId(name);
		if (fid >= 0) {
			GFS_Load(fid, 0, addr, bsize);
			return 0;
		}
	}
	return -1;
}

#else
	extern char sddrvstsk[];   /* サウンドドライバのバイナリデータ */
	extern long sddrvsize;
	Uint16 sound_map[] = {0xFFFF,0x0000,0x0001,0x4000,
					0x0102,0x4000,0x0001,0x4000,
					0xffff };
/*	static int errChk;  */
#endif

/* サウンドブロックの設定 */
 /* 1997.3.26 直接操作からサウンドドライバ環境へ変更 */
static void     setScsp(void)
{
#if 0
    /* SCSPの設定 */
    *((Uint16 *)0x25b00216) = 0x00ff;
    *((Uint16 *)0x25b00236) = 0x00ef;
    *((Uint16 *)0x25b00400) = 0x000f;
#else
	SndIniDt 	snd_init;
#ifndef USE_SDDRVS_DAT
	if (fileLoad("SDDRVS.TSK", (void *)sddrvs_tsk, SDDRVS_TSK_SIZE)) {
		return;
	}
	if (fileLoad("BOOTSND.MAP", (void *)bootsnd_map, BOOTSND_MAP_SIZE)) {
		return;
	}
	SND_INI_PRG_ADR(snd_init) 	= (Uint16 *)sddrvs_tsk;
	SND_INI_PRG_SZ(snd_init) 	= (Uint16 )SDDRVS_TSK_SIZE;
	SND_INI_ARA_ADR(snd_init) 	= (Uint16 *)bootsnd_map;
	SND_INI_ARA_SZ(snd_init) 	= (Uint16)BOOTSND_MAP_SIZE;
#else
	SND_INI_PRG_ADR(snd_init) 	= (Uint16 *)&sddrvstsk;
	SND_INI_PRG_SZ(snd_init) 	= sddrvsize;
	SND_INI_ARA_ADR(snd_init) 	= (Uint16 *)&sound_map;
	SND_INI_ARA_SZ(snd_init) 	= sizeof(sound_map);
#endif
	SND_Init(&snd_init);
	SND_ChgMap(0);
    /*  SND I/F 関数による設定  */
    /*  注意：実際には、ループ回数の上限を決めること */
    while (SND_SetCdDaLev(6, 6) == SND_RET_NSET);
    while (SND_SetCdDaPan(-15, 15) == SND_RET_NSET);
    while (SND_SetTlVl(15) == SND_RET_NSET);
#endif
    return;
}

/* ソフトウェアの初期化 */
static void     initSw(void)
{
    /* ＣＤブロックの初期化（ソフトリセット） */
    initCdb();

    /* MPEG システムの初期化 */
    if (MPG_Init(MPG_DSCN_ITL) != MPG_ERR_OK) {
        errFunc(-10);
    }
    
    /* gfsとstmの初期化 */
    if (initGfsStm() == FALSE) {
        errFunc(-1);
    }
    return;
}

/* ＣＤブロックの初期化（ソフトリセット） */
static void     initCdb(void)
{
    Sint32      ret;
    Sint32      timer;
    
    do {
        ret = CDC_CdInit(0x01, 0xff, 0xff, 0xff);
    } while (ret != CDC_ERR_OK);
    
    timer = 0;
    while (isHirqOn(CDC_HIRQ_ESEL) == FALSE) {
        if (++timer > IPL_TMOUT_COUNT) {
            return;
        }
    }
}

/* 終了フラグをチェックする */
static Bool     isHirqOn(Sint32 flag)
{
    return ((CDC_GetHirqReq() & flag) != 0);
}

/* gfsとstmの初期化 */
static Bool     initGfsStm(void)
{
    Sint32      gret;
    Bool        ret;
    Uint32      *fworkp = file_work;
    Uint8       *sworkp = strm_work;
    GfsDirTbl   dirtbl;
    GfsDirId    dirid[MAX_DIR_ID];
    
    /* ファイルシステムの初期化、マウント */
    GFS_DIRTBL_TYPE(&dirtbl) = GFS_DIR_ID;
    GFS_DIRTBL_NDIR(&dirtbl) = MAX_DIR_ID;
    GFS_DIRTBL_DIRID(&dirtbl) = dirid;
    gret = GFS_Init(FILE_OPEN_MAX, fworkp, &dirtbl);
    
    /* ストリームシステム初期化 */
    ret = STM_Init(SGRP_OPEN_MAX, STRM_OPEN_MAX, sworkp);
    
    return ret;
}

/* MPEGストリームのオープン */
static StmGrpHn stmOpen(StmHn *stm_v, StmHn *stm_a)
{
    StmGrpHn    grp;
    StmKey      key;
    Sint32      sfad, efas;
    StmFrange   frange;
#if 0
    Sint32      i;
#endif
    /* ストリームグループのオープン */
    if ((grp = STM_OpenGrp()) == NULL) {
        errFunc(-6);
    }
    
    /* ストリームキーの設定 */
    STM_KEY_FN(&key) = STM_KEY_NONE;
    STM_KEY_CN(&key) = STM_KEY_NONE;
    STM_KEY_CIMSK(&key) = STM_KEY_CIVAL(&key) = STM_KEY_NONE;
    
    /* TNOからFADへの変換 */
    tnoToFad(&sfad, &efas);
    
    /* ストリーム再生範囲の設定 */
    STM_FRANGE_SFAD(&frange) = sfad;
    if (E_TNO == 0) {
        STM_FRANGE_FASNUM(&frange) = STM_FAD_CDEND;
    } else {
        STM_FRANGE_FASNUM(&frange) = efas;
    }
    
    /* 再生範囲によるストリームのオープン */
    STM_KEY_SMMSK(&key) = STM_KEY_SMVAL(&key) = STM_SM_VIDEO;
    if ((*stm_v=STM_OpenFrange(grp, &frange, &key, STM_LOOP_READ)) == NULL) {
        errFunc(-7);
    }
    STM_KEY_SMMSK(&key) = STM_KEY_SMVAL(&key) = STM_SM_AUDIO;
    if ((*stm_a=STM_OpenFrange(grp, &frange, &key, STM_LOOP_READ)) == NULL) {
        errFunc(-8);
    }
    return grp;
}

/* TNOからFADへの変換 */
static void     tnoToFad(Sint32 *sfad, Sint32 *efas)
{
    Sint32      tnum;
    Sint32      etoc;
    Sint32      ret;
    CdcStat     stat;
    
    /* 定期ＣＤステータス情報の取得（<BUSY>以外になったらぬける） */
    while (TRUE) {
        ret = CDC_GetPeriStat(&stat);
        if (ret == CDC_ERR_PERI) {
            continue;
        }
        if (ret != CDC_ERR_OK) {
            errFunc(-12);
        }
        if (CDC_GET_STC(&stat) != CDC_ST_BUSY) {
            break;
        }
    }

    /* ＴＯＣ情報の取得 */
    ret = CDC_TgetToc(toc);
    
    /* トラック数を求める */
    tnum = (toc[TOC_ETNO_INF] >> 16) & 0x000000ff;
    
    /* 再生終了FADを参照するTOC情報番号を求める */
    if (E_TNO == tnum) {
        etoc = TOC_LOUT_INF;
    } else {
        etoc = E_TNO;
    }
    
    /* FADの計算 */
    *sfad = toc[S_TNO - 1] & 0x00ffffff;
    *efas = (toc[etoc] & 0x00ffffff) - (*sfad);
    
    return;
}

/* MPEGハンドルの作成 */
static void     createMpHandle(MpgMv *mpgmv, MpgWn *mpgwn,
                               StmHn stm_a, StmHn stm_v)
{
    /* MPEGハンドルの生成 */
    if ((*mpgmv = MPG_MvCreate(stm_v, stm_a)) == NULL) {
        errFunc(-2);
    }
    
    /* MPEGウィンドウの生成 */
    if ((*mpgwn = MPG_WnCreate(0, 0, 352, 240)) == NULL) {
        errFunc(-3);
    }
    
    /* MPEGウィンドウの接続 */
    MPG_MvConnectWin(*mpgmv, *mpgwn);
    
    /* 補間の設定 */
    MPG_WnSetIntpol(*mpgwn, ON, ON, ON, ON);
    
    /* 表示ON */
    MPG_WnDisp(*mpgwn, ON);
    
    return;
}

/* V-BLANK割り込みルーチンの登録解除 */
static void     resVblank()
{
    /* V_Blank 割り込みルーチンの登録解除 */
    INT_ChgMsk(INT_MSK_NULL, INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);
    INT_SetScuFunc(INT_SCU_VBLK_IN, NULL);
    INT_SetScuFunc(INT_SCU_VBLK_OUT, NULL);

    return;
}

/* エラー処理関数 */
static void     errFunc(Sint32 errlvl)
{
    /* エラー通知 */
    while (TRUE) {
        ;
    }
}




