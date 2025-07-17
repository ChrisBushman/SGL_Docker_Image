/************************************************************************/
/*  file name                                                           */
/*      snd_main.h                                                      */
/*  copyright                                                           */
/*      SEGA ENTERPRISES                                                */
/*  purpose                                                             */
/*      サウンドI/Fライブラリヘッダファイル                             */
/*  description                                                         */
/*      サウンドドライバとのインタフェースを実現する                    */
/*  author(s)                                                           */
/*      1994-05-18  N.T         Ver.0.90                                */
/*  mod history                                                         */
/*      1994-09-23  N.T         Ver.1.04                                */
/*      1994-12-01  N.T         Ver.1.05                                */
/*      1996-10-18  Y.Kashima   Ver.1.20                                */
/*      1996-10-18  Y.Kashima   Ver.1.20                                */
/*      1997-07-11  Y.Kashima   Ver.1.30                                */
/************************************************************************/

#ifndef SND_MAIN_H
#define SND_MAIN_H

/************************************************************************/
/*      C VIRTUAL TYPES DEFINITIONS                                     */
/************************************************************************/
#include <machine.h>
#include "sega_xpt.h"
#if 0
#include "sega_dma.h"
#include "sega_per.h"
#endif

/************************************************************************/
/* USER SUPPLIED INCLUDE FILES                                          */
/************************************************************************/

/************************************************************************/
/* GLOBAL DEFINES/MACROS DEFINES                                        */
/************************************************************************/

/**** 定数 **************************************************************/
/******** コマンド実行状態 *************************************************/
#define SND_RET_SET     0                   /* 正常終了         */
#define SND_RET_NSET    1                   /* 異常終了         */
/******** サウンドコントロール設定 ***************************************************/
#define SND_CTRL_OUTPUT_STEREO  (0 << 7)    /* sound output mode : STEREO   */
#define SND_CTRL_OUTPUT_MONO    (1 << 7)    /* sound output mode : MONO     */

/******** Sequence PAN コントロールスイッチ *******************************************/
#define SND_SEQ_PAN_OFF (0 << 7)            /* Control OFF      */
#define SND_SEQ_PAN_ON  (1 << 7)            /* Control ON       */

/******** ハードウェアチェックパラメータ *************************************************/
#define SND_DRAM4           0                   /* DRAM 4Mbit r/w   */
#define SND_DRAM8           1                   /* DRAM 8Mbit r/w   */
#define SND_SCSP_MIDI       2                   /* SCSP MIDI        */
#define SND_SOUND_SRC_LR    3                   /* 音源出力(L/R)    */
#define SND_SOUND_SRC_L     4                   /* 音源出力(L)      */
#define SND_SOUND_SRC_R     5                   /* 音源出力(R)      */
/******** ハードウェアチェックステータス **************************************************/
#define SND_HARD_OK     0x8000              /* 正常終了         */
#define SND_HARD_ERR    0x7fff              /* 異常終了         */
/******** 割り込み許可ビット ************************************************/
#define SND_INT_PCM_ADR (1 <<  7)           /* PCM play address 更新    */
/******** 対Host割り込み種別 ************************************************/
#define SND_FCT_PCM_ADR     0x01                /* PCM play address 更新    */
/******** PCMアドレス更新割り込みステータス ******************************************/
#define SND_PCM_PLAY0   (1 <<  0)           /* PCM再生番号0              */
#define SND_PCM_PLAY1   (1 <<  1)               /* PCM再生番号1              */
#define SND_PCM_PLAY2   (1 <<  2)               /* PCM再生番号2              */
#define SND_PCM_PLAY3   (1 <<  3)               /* PCM再生番号3              */
#define SND_PCM_PLAY4   (1 <<  4)               /* PCM再生番号4              */
#define SND_PCM_PLAY5   (1 <<  5)               /* PCM再生番号5              */
#define SND_PCM_PLAY6   (1 <<  6)               /* PCM再生番号6              */
#define SND_PCM_PLAY7   (1 <<  7)               /* PCM再生番号7              */

/******** SndSeqStat(シーケンスステータス) ***********************************/
/************* SND_SEQ_STAT_MODE(song mode) **********************************/
#define SND_MD_STOP     0x000                           /* stop             */
#define SND_MD_PLAY     0x100                           /* play             */
#define SND_MD_PLAY_PS  0x300                           /* pause            */
#define SND_MD_FADE     0x500                           /* fade proc        */
/******** SndPcmStartPrm(PCM開始パラメータ) **********************************/
/************* SND_PRM_MODE(ステレオ・monaural+サンプリングレート) ***************************/
#define SND_MD_MONO     (0 <<  7)               /* monaural                 */
#define SND_MD_STEREO   (1 <<  7)               /* stereophonic             */
#define SND_MD_16       (0 <<  4)               /* 16bitPCM                 */
#define SND_MD_8        (1 <<  4)               /* 8bitPCM                  */
/******** SND_MoveData(サウンドデータ転送) ***********************************/
#define SND_KD_TONE     0x0                 /* 音色              */
#define SND_KD_SEQ      0x1                 /* シーケンス        */
#define SND_KD_DSP_PRG  0x2                 /* DSPプログラム     */
#define SND_KD_DSP_RAM  0x3                 /* DSP work RAM         */
/*      #define SND_KD_PCM_BUF  0x4             pcm stream buffer           */
/**** アクセスマクロ *********************************************************/
/******** SndIniDt(システム起動) *********************************************/
#define SND_INI_PRG_ADR(sys_ini)    ((sys_ini).prg_adr) /* 68Kprogram 格納... */
#define SND_INI_PRG_SZ(sys_ini)     ((sys_ini).prg_sz)  /* 68Kprogram サイズ... */
#define SND_INI_ARA_ADR(sys_ini)    ((sys_ini).ara_adr) /* sound area map 格納. */
#define SND_INI_ARA_SZ(sys_ini)     ((sys_ini).ara_sz)  /* sound area map size . */
/******** SndSeqStat(シーケンスステータス) ***********************************/
#define SND_SEQ_STAT_MODE(status)   ((status).mode)     /* song mode         */
#define SND_SEQ_STAT_STAT(status)   ((status).stat)     /* song status       */
/******** SndPcmStartPrm(PCM開始パラメータ) **********************************/
#define SND_PRM_MODE(prm)       ((prm).mode)        /* ステレオ・monaural+sampling rate */
#define SND_PRM_SADR(prm)       ((prm).sadr)        /* PCMstream buf start adrs.*/
#define SND_PRM_SIZE(prm)       ((prm).size)        /* PCMstream buf size    */
#define SND_PRM_OFSET(prm)      ((prm).ofset)       /* PCMstream 再生開始オフセット */
/******** SndPcmChgPrm(PCM変更パラメータ) ************************************/
#define SND_PRM_NUM(prm)        ((prm).num)         /* PCMstream 再生番号       */
#define SND_PRM_LEV(prm)        ((prm).lev)         /* Direct 音Level         */
#define SND_PRM_PAN(prm)        ((prm).pan)         /* Direct 音pan           */
#define SND_PRM_PICH(prm)       ((prm).pich)        /* PICHワード              */
#define SND_R_EFCT_IN(prm)      ((prm).r_efct_in)   /* Efect in select(右)   */
#define SND_R_EFCT_LEV(prm)     ((prm).r_efct_lev)  /* Efect send Level(右)  */
#define SND_L_EFCT_IN(prm)      ((prm).l_efct_in)   /* Efect in select(左)   */
#define SND_L_EFCT_LEV(prm)     ((prm).l_efct_lev)  /* Efect send Level(左)  */
#define SND_PRM_TL(prm)         ((prm).t_lev)       /* Master Level          */
/******** SndPcmPlayAdr(PCM再生アドレス) ****************************************/
#define SND_PCM_RADR(prm)       ((prm).radr)        /* 右出力(monaural)          */
#define SND_PCM_LADR(prm)       ((prm).ladr)        /* 左出力                 */
/******** SndCdHzSrVl(周波数帯域別ステレオ解析音量) ******************************/
#define SND_CD_LHIGH(hz_vl)     ((hz_vl).high.left) /* 左出力 高音域         */
#define SND_CD_LMID(hz_vl)      ((hz_vl).mid.left)  /* 左出力 中音域         */
#define SND_CD_LLOW(hz_vl)      ((hz_vl).low.left)  /* 左出力 低音域         */
#define SND_CD_RHIGH(hz_vl)     ((hz_vl).high.right)/* 右出力 高音域         */
#define SND_CD_RMID(hz_vl)      ((hz_vl).mid.right) /* 右出力 中音域         */
#define SND_CD_RLOW(hz_vl)      ((hz_vl).low.right) /* 右出力 低音域         */

#define SND_POKE_B(adr, data)   (*((volatile Uint8 *)(adr)) = ((Uint8)(data))) /* バイト*/
#define SND_POKE_W(adr, data)   (*((volatile Uint16 *)(adr)) = ((Uint16)(data)))
#define SND_PEEK_B(adr)         (*((volatile Uint8 *)(adr)))                   /* バイト*/

/**** システムインタフェースワークオフセット *********************************/
#define SND_ADR_INTR_CTRL_WORD  (0x00)          /* Interrupt control word   */
#define SND_ADR_INTR_MODE       (0x01)          /* Interrupt mode           */
#define SND_ADR_PCM_PLAY_NUM    (0x02)          /* PCM play num             */
#define SND_ADR_SND_CTRL        (0x03)          /* Sound control            */
#define SND_CMD_TIMING_FLAG     (0x60)          /* command timing flag      */
#define SND_CMD_SEQ_CTRL_WORD   (0x61)          /* command mode             */
/**** アドレス ***************************************************************/
#define SND_ADR_INTR_RESET  ((volatille Uint16 *)0x25b0042e)  /* SCSP割り込みreset reg. */

/*割り込み *******************************************************************/
/********************************************************************/
/*  function name                                                   */
/*      SND_SET_ENA_INT()           - 割り込み許可設定              */
/*  parameters                                                      */
/*      (1) Uint8 ena_bit           - <i> 割り込み許可ビット        */
/*  description                                                     */
/*      割り込み許可を設定します                                    */
/*  preconditions                                                   */
/*      なし                                                        */
/*  postconditions                                                  */
/*      なし                                                        */
/*  caveats:                                                        */
/*      なし                                                        */
/********************************************************************/

#define SND_SET_ENA_INT(ena_bit)\
    do{\
        snd_msk_work_work = get_imask();           /* sound 割り込みをDisable*/\
        set_imask(15);\
        SND_POKE_B(snd_adr_sys_int_work + SND_ADR_INTR_CTRL_WORD, (ena_bit));\
        set_imask(snd_msk_work_work);\
    }while(FALSE)

/********************************************************************/
/*  function name                                                   */
/*      SND_GET_ENA_INT()           - 割り込み許可取得              */
/*  PARAMETERS                                                      */
/*      なし                                                        */
/*  DESCRIPTION                                                     */
/*      割り込み許可を取得します                                    */
/*  PRECONDITIONS                                                   */
/*      なし                                                        */
/*  POSTCONDITIONS                                                  */
/*      (1) Uint8                   - <o> 割り込み許可ビット        */
/*  CAVEATS                                                         */
/*      なし                                                        */
/********************************************************************/

#define SND_GET_ENA_INT()\
        (SND_PEEK_B(snd_adr_sys_int_work + SND_ADR_INTR_CTRL_WORD))

/********************************************************************/
/*  function name                                                   */
/*      SND_GET_FCT_INT()           - 割り込み要因取得              */
/*  parameters                                                      */
/*      なし                                                        */
/*  description                                                     */
/*      割り込み要因を取得します                                    */
/*  preconditions                                                   */
/*      なし                                                        */
/*  postconditions                                                  */
/*      (1) Uint8                   - <o> 割り込み要因              */
/*  caveats                                                         */
/*      read memory ($25a00481)                                     */
/********************************************************************/

#define SND_GET_FCT_INT()\
        (SND_PEEK_B(snd_adr_sys_int_work + SND_ADR_INTR_MODE))

/********************************************************************/
/*  function name                                                   */
/*      SND_RESET_INT()             - 割り込みリセット              */
/*  parameters                                                      */
/*      なし                                                        */
/*  description                                                     */
/*      割り込みをリセットします                                    */
/*  preconditions                                                   */
/*      なし                                                        */
/*  postconditions                                                  */
/*      なし                                                        */
/*  caveats                                                         */
/*      なし                                                        */
/********************************************************************/

#define SND_RESET_INT()\
        (SND_POKE_W(SND_ADR_INTR_RESET, (1 << 5)))

/********************************************************************/
/*  function name                                                   */
/*      SND_GET_INT_STAT()          - PCM実行アドレス更新           */
/*                                    割り込みステータス取得        */
/*  parameters                                                      */
/*      なし                                                        */
/*  description                                                     */
/*      PCM実行アドレス更新割り込みステータスを取得します           */
/*  preconditions                                                   */
/*      なし                                                        */
/*  postconditions                                                  */
/*      (1) SndPcmIntStat *         - <o> PCM実行アドレス更新       */
/*                                        割り込みステータス        */
/*  caveats                                                         */
/*      ream memory ($25a00482)                                     */
/********************************************************************/

#define SND_GET_INT_STAT()\
        (SND_PEEK_B(snd_adr_sys_int_work + SND_ADR_PCM_PLAY_NUM))

/*その他 ****************************************************************/
/********************************************************************/
/*  function name                                                   */
/*      SND_SET_SND_CTRL            - サウンドコントロール情報設定  */
/*  parameters                                                      */
/*      (1) Uint8   ctrl_bit        - <i> 割り込み許可ビット        */
/*  description                                                     */
/*      サウンドコントロール情報を設定します                        */
/*  preconditions                                                   */
/*      なし                                                        */
/*  postconditions                                                  */
/*      なし                                                        */
/*  caveats                                                         */
/*      なし                                                        */
/********************************************************************/

#define SND_SET_SND_CTRL(ctrl_bit)\
    do{\
        SND_POKE_B(snd_adr_sys_int_work + SND_ADR_SND_CTRL, (ctrl_bit));\
    }while(FALSE)

/********************************************************************/
/*  function name                                                   */
/*      SND_GET_SND_CTRL            - サウンドコントロール情報取得  */
/*  parameters                                                      */
/*      なし                                                        */
/*  description                                                     */
/*      サウンドコントロール情報を取得します                        */
/*  preconditions                                                   */
/*      なし                                                        */
/*  postconditions                                                  */
/*      なし                                                        */
/*  caveats                                                         */
/*      なし                                                        */
/********************************************************************/

#define SND_GET_SND_CTRL()\
        (SND_PEEK_B(snd_adr_sys_int_work + SND_ADR_SND_CTRL))

/********************************************************************/
/*  function name                                                   */
/*      SND_CMD_HANDSHAKE                                           */
/*  parameters                                                      */
/*      (1) Uint8   ctrl_bit        - <i> 割り込み許可ビット        */
/*  description                                                     */
/*      サウンドコマンドハンドシェイク情報を設定します              */
/*  preconditions                                                   */
/*      なし                                                        */
/*  postconditions                                                  */
/*      なし                                                        */
/*  caveats                                                         */
/*      なし                                                        */
/********************************************************************/

#define SND_CMD_HANDSHAKE(ctrl_bit)\
    do{\
        SND_POKE_B(snd_adr_sys_int_work + SND_CMD_SEQ_CTRL_WORD, (ctrl_bit));\
    }while(FALSE)

/********************************************************************/
/*  function name                                                   */
/*      SND_CMD_SET                                                 */
/*  parameters                                                      */
/*      なし                                                        */
/*  description                                                     */
/*      サウンドコマンドを設定します                                */
/*  preconditions                                                   */
/*      なし                                                        */
/*  postconditions                                                  */
/*      なし                                                        */
/*  caveats                                                         */
/*      なし                                                        */
/********************************************************************/

#define SND_CMD_SET()\
        (SND_POKE_B(snd_adr_sys_int_work + SND_CMD_TIMING_FLAG, (0x80))

/************************************************************************/
/*  STRUCTURE DECLARATIONS                                              */
/************************************************************************/

/************************************************************************/
/*  TYPEDEFS                                                            */
/************************************************************************/

/**** 共通 **************************************************************/
typedef struct{
    Uint16 *prg_adr;                /* 68Kprogram 格納先頭 address  */
    Uint16 prg_sz;                  /* 68Kprogram size size         */
    Uint16 *ara_adr;                /* sound area map 格納先頭address   */
    Uint16 ara_sz;                  /* sound area map size (ワード単位) */
}SndIniDt;                          /* システム起動データ型         */

typedef Uint8 SndAreaMap;           /* sound area map データ型      */
typedef Uint8 SndTlVl;              /* 全体音量データ型             */
typedef Uint8 SndEfctBnkNum;        /* Effct bank numberデータ型    */
typedef Uint8 SndToneBnkNum;        /* 音色 bank numberデータ型     */
typedef Uint8 SndMixBnkNum;         /* Mixer numberデータ型         */
typedef Uint8 SndEfctOut;           /* Effect out selectデータ型    */
typedef Uint8 SndLev;               /* Levelデータ型                */
typedef Sint8 SndPan;               /* PANデータ型                  */
typedef Uint8 SndRet;               /* command 実行状態データ型     */
typedef Uint8 SndHardPrm;           /* hardware check parameter data型  */
typedef Uint16 SndHardStat;         /* hardware check status データ型   */

/**** シーケンス *************************************************************/
typedef Uint8 SndSeqNum;            /* 発音管理番号                 */
typedef Uint8 SndSeqBnkNum;         /* Sequence bank number         */
typedef Uint8 SndSeqSongNum;        /* Sequence song number         */
typedef Uint8 SndSeqPri;            /* Priorty level                */
typedef Uint8 SndSeqVl;             /* Sequence Volume              */
typedef Uint8 SndFade;              /* fade Rate                    */
typedef Sint16 SndTempo;            /* Tempo                        */
typedef struct{
    Uint16 mode;                    /* song mode                    */
    Uint8 stat;                     /* song status                  */
}SndSeqStat;                        /* シーケンスステータス         */
typedef Uint16 SndSeqPlayPos;       /* 発音管理番号再生位置         */

/**** PCM ********************************************************************/
typedef struct{
    Uint8 mode;                     /* stereo/monaural+sampling rate    */
    Uint16 sadr;                    /* PCM stream buffer start address  */
    Uint16 size;                    /* PCM stream buffer size       */
}SndPcmStartPrm;                    /* PCM開始パラメータ            */

typedef Uint8 SndPcmNum;            /* PCM stream 再生番号          */
typedef Uint8 SndEfctIn;            /* Efect in select              */
typedef struct{
    SndPcmNum num;                  /* PCM stream 再生番号          */
    SndLev lev;                     /* Direct 音Level               */
    SndPan pan;                     /* Direct 音pan                 */
    Uint16 pich;                    /* PICHワード                   */
    SndEfctIn r_efct_in;            /* Efect in select(右出力)      */
    SndLev r_efct_lev;              /* Efect send Level(右出力)     */
    SndEfctIn l_efct_in;            /* Efect in select(左出力)      */
    SndLev l_efct_lev;              /* Efect send Level(左出力)     */
    SndLev t_lev;                   /* Total level                  */
}SndPcmChgPrm;                      /* PCM変更パラメータ            */

typedef struct{
    Uint8 radr;                     /* 右出力(monaural)             */
    Uint8 ladr;                     /* 左出力                       */
}SndPcmPlayAdr;                     /* PCM再生 address              */

typedef Uint8 SndPcmIntStat;        /* PCM address 更新割り込み status  */

/**** CD *********************************************************************/
typedef Uint16 SndCdVlAnl;          /* 解析音量                     */

typedef struct{
    SndCdVlAnl left;                /* 左出力                       */
    SndCdVlAnl right;               /* 右出力                       */
}SndCdHzVl;                         /* 周波数帯域別ステレオ解析音量 */

typedef struct{
    SndCdHzVl high;                 /* 高音域                       */
    SndCdHzVl mid;                  /* 中音域                       */
    SndCdHzVl low;                  /* 低音域                       */
}SndCdHzSrVl;                       /* 周波数帯域別解析音量         */

/*
 * EXTERNAL VARIABLE DECLARATIONS
 */
extern Uint8 *snd_adr_sys_int_work; /* system i/f work top address 格納 */
extern Uint32 snd_msk_work_work;    /* sound priority msk               */

/*
 * EXTERNAL FUNCTION PROTOTYPE  DECLARATIONS
 */
void SND_Init(SndIniDt *);
void SND_MoveData(Uint16 *, Uint32, Uint16, Uint16);
SndRet SND_ChgMap(SndAreaMap);
SndRet SND_SetTlVl(SndTlVl);
SndRet SND_ChgEfct(SndEfctBnkNum);
SndRet SND_ChgMix(SndToneBnkNum, SndMixBnkNum);
SndRet SND_ChgMixPrm(SndEfctOut, SndLev, SndPan);
SndRet SND_ChkHard(SndHardStat *, SndHardPrm);
SndRet SND_StopDsp(void);
SndRet SND_OffAllSound(void);
SndRet SND_StartSeq(SndSeqNum, SndSeqBnkNum, SndSeqSongNum, SndSeqPri);
SndRet SND_StopSeq(SndSeqNum);
SndRet SND_PauseSeq(SndSeqNum);
SndRet SND_ContSeq(SndSeqNum);
SndRet SND_SetSeqVl(SndSeqNum, SndSeqVl, SndFade);
SndRet SND_ChgTempo(SndSeqNum, SndTempo);
SndRet SND_CtrlDirMidi(SndSeqNum, SndSeqPri, Uint8, Uint8, Uint8, Uint8);
SndRet SND_SetSeqPan(SndSeqNum, Uint8, Uint8 );
SndRet SND_StartPcm(SndPcmStartPrm *, SndPcmChgPrm *);
SndRet SND_StartPcmTL(SndPcmStartPrm *, SndPcmChgPrm *);
SndRet SND_StopPcm(SndPcmNum);
SndRet SND_ChgPcm(SndPcmChgPrm *);
SndRet SND_ChgPcmTL(SndPcmChgPrm *);
SndRet SND_StartVlAnl(void);
SndRet SND_StopVlAnl(void);
SndRet SND_SetCdDaLev(SndLev, SndLev);
SndRet SND_SetCdDaPan(SndPan, SndPan);
void SND_GetSeqStat(SndSeqStat *, SndSeqNum);
void SND_GetSeqPlayPos(SndSeqPlayPos *, SndSeqNum);
void SND_GetPcmPlayAdr(SndPcmPlayAdr *, SndPcmNum);
void SND_GetAnlTlVl(SndCdVlAnl *, SndCdVlAnl *);
void SND_GetAnlHzVl(SndCdHzSrVl *);
SndRet SND_SetQSound(Uint8,Uint8);
SndRet SND_Set3D_Stat(Uint8,Uint8,Uint8);
SndRet SND_Set3D_Init(void);
SndRet SND_Init_Sound(Uint8,Uint8,Uint8,Uint8,Uint8);
SndRet SND_SetTempoMode(SndSeqNum, Uint8);
SndRet SND_SetTempoRatio(SndSeqNum, Uint8);
SndRet SND_AllocPcm(SndPcmChgPrm *);
SndRet SND_ReleasePcm(SndPcmChgPrm *);

#endif  /* ifndef SND_MAIN_H */
