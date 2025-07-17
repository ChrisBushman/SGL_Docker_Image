/***********************************************************************
 *      サターンソフトウェアライブラリ
 *
 *      Copyright (c) 1994,1996 SEGA
 *
 * Library : バックアップライブラリ（BUP）
 * Module  : BUPライブラリ用ヘッダファイル
 * File    : sega_bup.h
 * Date    : 1997-06-20
 * Version : Ver.1.25
 * Author  : H.K
 *
 * History :
 *  ・1994-07-13 (Ver.1.00) : 新規作成 by K.M.
 *  ・1994-07-29 (Ver.1.00) : 更新 by K.M.
 *  ・1996-04-24 (Ver.1.10) : FDD対策用の修正 by M.M.
 *  ・1996-05-16 (Ver.1.??) : SYS関数の修正（割り込み）by H.K
 *  ・1996-06-19 (Ver.1.20) : Ver.1.??のバグ修正 by H.K
 *  ・1996-08-06 (Ver.1.21) : Ver.1.20をコンパイルし直す by H.K
 *  ・1996-10-01 (Ver.1.22) : Ver.1.21をオプションを変えてコンパイル by H.K
 *  ・1997-01-08 (Ver.1.23) : BUP_GetDateの日付計算の間違いにパッチを当てる
 *                                                                  by H.K
 *  ・1997-03-12 (Ver.1.24) : リセットコマンド操作を内部で実行しない
 *                      BUP_GetDate,BUP_SetDate内部の割込み禁止を削除 by H.K
 *  ・1997-06-20 (Ver.1.25) A.H
 *                          : ライブラリ使用時に必要なワークRAMのサイズを
 *                            マクロで追加
 *                             BUPライブラリ本体がロードされる領域のサイズ
 *                                   BUP_LIB_SIZE4
 *                             BUPライブラリの作業領域のサイズ
 *                                   BUP_WORK_SIZE4
 *
 ***********************************************************************/

/* 多重インクルードへの対応 */
#ifndef SEGA_BUP_H
#define SEGA_BUP_H

/************************************************************************
 *      インクルードファイル
 ***********************************************************************/
#include "sega_xpt.h"

/************************************************************************
 *      定数マクロ
 ***********************************************************************/

/* バージョン */
                    /*             1         2         3         4    */
                    /*    1234567890123456789012345678901234567890    */
                    /*    ++++++++++++++++++++++++++++++++++++        */
#define BUP_VERSION_STR ("BUP Version 1.25 1997-06-20")
#define BUP_VERSION_LEN         (32)


/* BUPライブラリ本体がロードされる領域のサイズ(long単位)  16K(0x4000)バイト*/
#define BUP_LIB_SIZE4         (16 * 1024 / sizeof(Uint32))

/* BUPライブラリの作業領域のサイズ(long単位)  8312(0x2078)バイト*/
#define BUP_WORK_SIZE4        (8312 / sizeof(Uint32))

/* BUP_Init関数のアドレス */
#define BUP_LIB_ADDRESS         (*(Uint32 *)(0x6000350+8))

/* BUPライブラリ作業領域のアドレス */
#define BUP_VECTOR_ADDRESS      (*(Uint32 *)(0x6000350+4))

/* ユニットＩＤ */
#define BUP_MAIN_UNIT   (1)
#define BUP_CURTRIDGE   (2)

/* 言語種類 */
#define BUP_JAPANESE    (0)
#define BUP_ENGLISH     (1)
#define BUP_FRANCAIS    (2)
#define BUP_DEUTSCH     (3)
#define BUP_ESPANOL     (4)
#define BUP_ITALIANO    (5)

/* 装置状態 */
#define BUP_NON                 (1)
#define BUP_UNFORMAT            (2)
#define BUP_WRITE_PROTECT       (3)
#define BUP_NOT_ENOUGH_MEMORY   (4)
#define BUP_NOT_FOUND           (5)
#define BUP_FOUND               (6)
#define BUP_NO_MATCH            (7)
#define BUP_BROKEN              (8)

/************************************************************************
 *      フックテーブル用関数マクロ
 ***********************************************************************/

#define BUP_HK_INIT(libaddr, workbuff, conf) \
    (*(BupFnInit)(BUP_LIB_ADDRESS))(libaddr, workbuff, conf)

#define BUP_HK_SELPART(device, num) \
    (**(BupFnSelPart)(BUP_VECTOR_ADDRESS+4))(device, num)

#define BUP_HK_FORMAT(device) \
    (**(BupFnFormat)(BUP_VECTOR_ADDRESS+8))(device)

#define BUP_HK_STAT(device, datasize, stat) \
    (**(BupFnStat)(BUP_VECTOR_ADDRESS+12))(device, datasize, stat)

#define BUP_HK_WRITE(device, dir, data, owsw) \
    (**(BupFnWrite)(BUP_VECTOR_ADDRESS+16))(device, dir, data, owsw)

#define BUP_HK_READ(device, fname, data) \
    (**(BupFnRead)(BUP_VECTOR_ADDRESS+20))(device, fname, data)

#define BUP_HK_DELETE(device, fname) \
    (**(BupFnDelete)(BUP_VECTOR_ADDRESS+24))(device, fname)

#define BUP_HK_DIR(device, fname, dirsize, dir) \
    (**(BupFnDir)(BUP_VECTOR_ADDRESS+28))(device, fname, dirsize, dir)

#define BUP_HK_VERIFY(device, fname, data) \
    (**(BupFnVerify)(BUP_VECTOR_ADDRESS+32))(device, fname, data)

#define BUP_HK_GETDATE(pdate, date) \
    (**(BupFnGetDate)(BUP_VECTOR_ADDRESS+36))(pdate, date)

#define BUP_HK_SETDATE(date) \
    (**(BupFnSetDate)(BUP_VECTOR_ADDRESS+40))(date)

/************************************************************************
 *      データ型の宣言
 ***********************************************************************/

/******************************************
 * 記憶装置接続情報テーブル               *
 ******************************************/
typedef struct BupConfig {
        Uint16  unit_id;        /* ユニットＩＤ */
        Uint16  partition;      /* パーティション数 */
} BupConfig;

/******************************************
 * ステータス情報テーブル                 *
 ******************************************/
typedef struct BupStat {
        Uint32  totalsize;      /* 全容量（Byte） */
        Uint32  totalblock;     /* 全ブロック数 */
        Uint32  blocksize;      /* 1ブロックのサイズ（Byte） */
        Uint32  freesize;       /* 空き容量 */
        Uint32  freeblock;      /* 空ブロック数 */
        Uint32  datanum;
} BupStat;

/******************************************
 * ディレクトリ情報テーブル               *
 ******************************************/
typedef struct BupDir {
        Uint8   filename[12];   /* ファイル名 */
        Uint8   comment[11];    /* コメント */
        Uint8   language;       /* コメントの言語種類 */
        Uint32  date;           /* タイムスタンプ */
        Uint32  datasize;       /* データサイズ（Byte） */
        Uint16  blocksize;      /* データサイズ（ブロック数） */
} BupDir;

/******************************************
 * 日付時間テーブル                       *
 ******************************************/
typedef struct BupDate {
        Uint8   year;           /* タイムスタンプ（年 -1980） */
        Uint8   month;          /* タイムスタンプ（月 1～12） */
        Uint8   day;            /* タイムスタンプ（日 1～31） */
        Uint8   time;           /* タイムスタンプ（時 0～23） */
        Uint8   min;            /* タイムスタンプ（分 0～59） */
        Uint8   week;           /* タイムスタンプ (曜日 日0～土6)*/
} BupDate;

/************************************************************************
 *      フックテーブル用関数ポインタ型の宣言
 ***********************************************************************/
typedef void    (*BupFnInit)(Uint32 *libaddr, Uint32 *workbuff,
                             BupConfig conf[]);
typedef Sint32  (**BupFnSelPart)(Uint32 device, Uint16 num);
typedef Sint32  (**BupFnFormat)(Uint32 device);
typedef Sint32  (**BupFnStat)(Uint32 device, Uint32 datasize,
                              BupStat *stat);
typedef Sint32  (**BupFnWrite)(Uint32 device, BupDir *dir, Uint8 *data,
                               Uint8 owsw);
typedef Sint32  (**BupFnRead)(Uint32 device, Uint8 *fname, Uint8 *data);
typedef Sint32  (**BupFnDelete)(Uint32 device, Uint8 *fname);
typedef Sint32  (**BupFnDir)(Uint32 device, Uint8 *fname, Uint16 dirsize,
                             BupDir *dir);
typedef Sint32  (**BupFnVerify)(Uint32 device, Uint8 *fname, Uint8 *data);
typedef void    (**BupFnGetDate)(Uint32 pdate, BupDate *date);
typedef Uint32  (**BupFnSetDate)(BupDate *date);

/************************************************************************
 *      変数の宣言
 ***********************************************************************/

/************************************************************************
 *      関数の宣言
 ***********************************************************************/
void    BUP_Init(Uint32 *libaddr, Uint32 *workbuff, BupConfig conf[]);
Sint32  BUP_SelPart(Uint32 device, Uint16 num);
Sint32  BUP_Format(Uint32 device);
Sint32  BUP_Stat(Uint32 device, Uint32 datasize, BupStat *stat);
Sint32  BUP_Write(Uint32 device, BupDir *dir, Uint8 *data, Uint8 owsw);
Sint32  BUP_Read(Uint32 device, Uint8 *fname, Uint8 *data);
Sint32  BUP_Delete(Uint32 device, Uint8 *fname);
Sint32  BUP_Dir(Uint32 device, Uint8 *fname, Uint16 dirsize, BupDir *dir);
Sint32  BUP_Verify(Uint32 device, Uint8 *fname, Uint8 *data);
void    BUP_GetDate(Uint32 pdate, BupDate *date);
Uint32  BUP_SetDate(BupDate *date);

#endif /* ifndef SEGA_BUP_H */

/* end of file */
