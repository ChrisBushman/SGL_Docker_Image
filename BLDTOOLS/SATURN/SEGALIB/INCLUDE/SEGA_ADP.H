/******************************************************************************
 *	ソフトウェアライブラリ
 *
 *	Copyright 1991 CSK Research Institute Corp.
 *	Copyright (c) 1994,1995,1996,1997 SEGA
 *
 * Library	:ＡＤＰＣＭ伸張ライブラリ
 * Module	:ライブラリヘッダ
 * File		:sega_adp.h
 * Date		:1997-06-10
 * Version	:1.22
 * Author	:A.H
 *
 *--------------------------------------------------------------------------*
 * Update	:1994-12-14	1.01	Y.T	新規作成
 *			:1996-10-14	1.21	Y.H	SHC Ver. 3.0F対応
 *			:1997-06-10	Ver 1.22	A.H	SHC Ver. 3.0G対応
 *
 ****************************************************************************/


#if !defined(SEGA_ADP_H)
#define SEGA_ADP_H


/************************************************************************/
/*		□ヘッダファイル												*/
/************************************************************************/
#include "sega_xpt.h"




/************************************************************************/
/*		□定数マクロ													*/
/************************************************************************/
/* １サウンドグループの圧縮時サイズ */
#define ADP_SIZE_SG_CODE    (128)

/* １サウンドグループの伸張時サイズ */
#define ADP_SIZE_SG_DECO    (448)

/* １サウンドグループの伸張時サイズ[byte/1ch] */
#define ADP_SIZE_SG_DECO_1  (ADP_SIZE_SG_DECO / 2)

/* １サウンドグループの伸張時サイズ[dword/1ch] */
#define ADP_DSIZE_SG_DECO_1  (ADP_SIZE_SG_DECO_1 / 4)

/* １サウンドグループの圧縮時サイズ（ワード数） */
#define ADP_WSIZE_SG_CODE   (ADP_SIZE_SG_CODE/2)

/* １サウンドグループの伸張時サイズ（ワード数） */
#define ADP_WSIZE_SG_DECO    (ADP_SIZE_SG_DECO/2)




/************************************************************************/
/*		□関数宣言														*/
/************************************************************************/
/*    ストリーム形式数ＳＧのステレオ伸張        */
/* void ADP_DecStereoStm(Uint16 *in, void *out, Sint32 nsg); */
                                                                               
/*    ストリーム形式数ＳＧのモノラル伸張        */
/* void ADP_DecMonoStm(Uint16 *in, void *out, Sint32 nsg); */
                                                                               
/*    バッファ形式数ＳＧのステレオ伸張          */
void ADP_DecStereoBuf(void *in, void *out, Sint32 nsg);
                                                                               
/*    バッファ形式数ＳＧのモノラル伸張          */
void ADP_DecMonoBuf(void *in, void *out, Sint32 nsg);
                                                                               
/*    バッファ形式１ＳＧのステレオ伸張          */
void ADP_DecStereo(void *in, void *out);
                                                                               
/*    バッファ形式１ＳＧのモノラル伸張          */
void ADP_DecMono(void *in, void *out);

#endif	/* SEGA_ADP_H */


/*  end of file  */
