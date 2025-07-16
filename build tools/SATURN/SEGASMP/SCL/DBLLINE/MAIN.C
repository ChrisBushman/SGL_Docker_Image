/*------------------------------------------------------------------------
 *  FILE:	SmpScl3.c
 *
 *      Copyright (c) by SEGA Enterprises Ltd. 1994. All rights reserved. 
 *
 *  PURPOSE:
 *		VDPII Test Program
 *
 *  AUTHOR(S):
 *		K.M
 *		C.Y
 *		1996-10-23  ラインスクロール2画面同時使用時のライブラリバグ修正
 *		            動作テストの為にサンプルに追加・変更
 *
 *------------------------------------------------------------------------
 */
#include	<machine.h>
#include	<sega_xpt.h>
#include	<sega_def.h> 
#include	<sega_mth.h> 
#include	<sega_scl.h> 
#include	"botan.h"

#define		MAP_OFFSET		0x0800
#define		PN_NBG1_ADDR	(SCL_VDP2_VRAM_B0 + 4096*2)
#define		LineTable_NBG0_ADDR		SCL_VDP2_VRAM_B1
#define		LineTable_NBG1_ADDR		(SCL_VDP2_VRAM_B1 + 65536)
#define		win_move_up		1
#define		win_move_down	0
#define		lineTableReset 	2

void SetAutoLinePara(Uint16 SclNum, void *);
void ClrLinePara(Uint16 SclNum);
void  set_gdata_NBG1(void);

extern SetVblank(void);				/*	追加 95.7.11 Chikahiro Yoshida	*/
extern FirstData(Uint32 sclnum);	/*	追加 95.7.11 Chikahiro Yoshida	*/
extern SecondData(Uint32 sclnum);	/*	追加 95.7.11 Chikahiro Yoshida	*/
extern	Uint32	BackPalette[];		/* NBG1:鉄板風タイルのパレット   */
extern	Uint8	BackCharPatData[];	/* NBG1:鉄板風タイルのデータ本体 */
Uint32	lineStatus[4];


/********************************
 * サイクルパターンテーブル               *
 ********************************/
Uint16	CycleTb[]={
	0xffff,0x44ff,  /*  A0: NBG0 CP DATA 格納 */
	0x0fff,0xffff,  /*  A1: NBG0 PN DATA 格納 */
	0xf1fE,0xEE55,  /*  B0: NBG1  CP DATA,PN DATA 格納  */
	0xfffE,0xEEff   /*  B1: ラインスクロールテーブルDATA 格納  */
};


void main()
{
	SclVramConfig	tp;
    SclConfig	scfg;
    Uint32	Sflag;
    Uint8	Key;
    Uint8	sw;
    Uint8	dir = win_move_up;
	Uint16	 win_left=0, win_up=120, win_right=319, win_down=223;

	
   /*******************************************
    *	ＶＤＰ２ライブラリの初期化            *
    *******************************************/
    SCL_Vdp2Init();
    SCL_SetFrameInterval(1);

   /*******************************************
    *	カラーＲＡＭモードを1600万色に設定    *
    *******************************************/
    SCL_SetColRamMode(SCL_CRM24_1024);

   /*******************************
    *	V_Blankルーチンの登録     *
    *******************************/
    SetVblank();
   /*******************************************
    *	割り込みマスクを解除                  *
    *******************************************/
    set_imask(0);
    /*******************************************
    *	カラーモード設定の為Ｖ－ＩＮＴ待ち     *
    ********************************************/
    SCL_DisplayFrame();

   /*******************************************
    *	スクロールデータのセット              *
    *******************************************/
    FirstData(SCL_NBG0);
	set_gdata_NBG1();
   /*******************************************
    *	スクロールコンフィグレーションの設定                *
    *******************************************/
    SCL_InitConfigTb(&scfg);
    scfg.dispenbl      = ON;
    scfg.charsize      = SCL_CHAR_SIZE_1X1;
    scfg.pnamesize     = SCL_PN2WORD;
    scfg.platesize     = SCL_PL_SIZE_1X1;
    scfg.coltype       = SCL_COL_TYPE_256;
    scfg.datatype      = SCL_CELL;
    scfg.plate_addr[0] = SCL_VDP2_VRAM_A1;
    scfg.plate_addr[1] = SCL_VDP2_VRAM_A1;
    scfg.plate_addr[2] = SCL_VDP2_VRAM_A1;
    scfg.plate_addr[3] = SCL_VDP2_VRAM_A1;
    SCL_SetConfig(SCL_NBG0, &scfg);   /*  NBG0 土パターン  */

	scfg.dispenbl      = ON;
    scfg.charsize      = SCL_CHAR_SIZE_2X2;
    scfg.pnamesize     = SCL_PN1WORD;
    scfg.flip          = SCL_PN_12BIT;
    scfg.platesize     = SCL_PL_SIZE_1X1;
    scfg.coltype       = SCL_COL_TYPE_256;
    scfg.datatype      = SCL_CELL;
    scfg.plate_addr[0] = PN_NBG1_ADDR;
    scfg.plate_addr[1] = PN_NBG1_ADDR;
    scfg.plate_addr[2] = PN_NBG1_ADDR;
    scfg.plate_addr[3] = PN_NBG1_ADDR;
    SCL_SetConfig(SCL_NBG1, &scfg);   /*  NBG1  タイルパターン  */

   /*******************************************
    *	サイクルパターンの設定                *
    *******************************************/
    SCL_SetCycleTable(CycleTb);

	/*******************************************
	*	ＶＲＡＭの使用方法の設定           *
	*******************************************/
	SCL_InitVramConfigTb(&tp);
	tp.vramModeA  = ON;		/* VRAM A を分割する */
	tp.vramModeB  = ON;		/* VRAM B を分割する */
	SCL_SetVramConfig(&tp);

   /*******************************************
    *	プライオリティを設定(0～7)                  *
    *******************************************/
    SCL_SetPriority(SCL_NBG1,7);
    SCL_SetPriority(SCL_NBG0,6);
    SCL_SetPriority(SCL_SPR,0);

   /*******************************************
    *	透明処理ウィンドウ(NBG1)              *
    *******************************************/
	SCL_SetWindow(SCL_W0, (Uint32)NULL, SCL_NBG1, (Uint32)NULL,
				 win_left, win_up, win_right, win_down);

   /*******************************************
    *	ラインスクロールテーブル初期化        *
    *******************************************/
/*    SetAutoLinePara(SCL_NBG1, (void *)LineTable_NBG1_ADDR);  */
    SetAutoLinePara(SCL_NBG0, (void *)LineTable_NBG0_ADDR);
    SetAutoLinePara(SCL_NBG1, (void *)LineTable_NBG1_ADDR);

#if 0  /* ライブラリの修正により削除(96.10.23) */
   /*  ライブラリ バグ対応部分  */
	Scl_n_reg.linecontrl = 0x0e0e; 
	SclProcess = 1;
#endif

    SCL_Open(SCL_NBG0);
	SCL_MoveTo(FIXED(0), FIXED(0),0);/* Home Position */
	SCL_Scale(FIXED(1.0), FIXED(1.0));
    SCL_Close();

    SCL_Open(SCL_NBG1);
	SCL_MoveTo(FIXED(0), FIXED(50),0);/* Home Position */
	SCL_Scale(FIXED(1.0), FIXED(1.0));
    SCL_Close();

    SCL_DisplayFrame();

    Key=DOWN_BUTTON;
    sw=1;
    Sflag = 0;
    while(1) {
	/* SBL Scroll Controll	*/
	switch(Key) {
	    case UP_LEFT:
		SCL_Open(SCL_NBG0);
		    SCL_Move(FIXED(2), FIXED(2),0);
		SCL_Close();
		SCL_Open(SCL_NBG1);
		    SCL_Move(-FIXED(2), -FIXED(2),0);
		SCL_Close();
		SCL_DisplayFrame();
		if(sw==0) Key = LEFT_BUTTON;
		break;
	    case UP_RIGHT:
		SCL_Open(SCL_NBG0);
		    SCL_Move(-FIXED(2), FIXED(2), 0);
		SCL_Close();
		SCL_Open(SCL_NBG1);
		    SCL_Move(FIXED(2),-FIXED(2),0);
		SCL_Close();
		SCL_DisplayFrame();
		if(sw==0) Key = UP_BUTTON;
		break;
	    case UP_BUTTON:
		SCL_Open(SCL_NBG0);
		    SCL_Move(0, FIXED(2), 0);
		SCL_Close();
		SCL_Open(SCL_NBG1);
		    SCL_Move(0, -FIXED(2),0);
		SCL_Close();
		SCL_DisplayFrame();
		if(sw==0) Key = UP_LEFT;
		break;
	    case DOWN_LEFT:
		SCL_Open(SCL_NBG0);
		    SCL_Move(FIXED(2), -FIXED(2), 0);
		SCL_Close();
		SCL_Open(SCL_NBG1);
		    SCL_Move(-FIXED(2), FIXED(2),0);
		SCL_Close();
		SCL_DisplayFrame();
		if(sw==0) Key = DOWN_BUTTON;
		break;
	    case DOWN_RIGHT:
		SCL_Open(SCL_NBG0);
		    SCL_Move(-FIXED(2), -FIXED(2), 0);
		SCL_Close();
		SCL_Open(SCL_NBG1);
		    SCL_Move(FIXED(2), FIXED(2),0);
		SCL_Close();
		SCL_DisplayFrame();
		if(sw==0) Key = RIGHT_BUTTON;
		break;
	    case DOWN_BUTTON:
		SCL_Open(SCL_NBG0);
		    SCL_Move(0, -FIXED(2), 0);
		SCL_Close();
		SCL_Open(SCL_NBG1);
		    SCL_Move(0, FIXED(2),0);
		SCL_Close();
		SCL_DisplayFrame();
		if(sw==0) Key = DOWN_RIGHT;
		break;
	    case LEFT_BUTTON:
		SCL_Open(SCL_NBG0);
		    SCL_Move(FIXED(2), 0, 0);
		SCL_Close();
		SCL_Open(SCL_NBG1);
		    SCL_Move(-FIXED(2), 0, 0);
		SCL_Close();
		SCL_DisplayFrame();
		if(sw==0) Key = DOWN_LEFT;
		break;
	    case RIGHT_BUTTON:
		SCL_Open(SCL_NBG0);
		    SCL_Move(-FIXED(2), 0, 0);
		SCL_Close();
		SCL_Open(SCL_NBG1);
		    SCL_Move(FIXED(2), 0, 0);
		SCL_Close();
		SCL_DisplayFrame();
		if(sw==0) Key = UP_RIGHT;
		break;
	}  /*  switch(Key)  */
	sw++;
	switch(dir){  /* ラインスクロールテーブルの更新処理+ウィンドウの移動 */
		case win_move_up:
			if(win_up > 0){
				win_up--;
				win_down--;
				if(win_up == 60 )
					ClrLinePara(SCL_NBG1);
			}else{
				SetAutoLinePara(SCL_NBG0, (void *)LineTable_NBG0_ADDR);
				dir = win_move_down;
			}
			break;
		case win_move_down:
			if(win_down < 223){
				win_up++;
				win_down++;
				if(win_up == 60 )
				    SetAutoLinePara(SCL_NBG1, (void *)LineTable_NBG1_ADDR);
			}else{
				ClrLinePara(SCL_NBG0);
				dir = win_move_up;
			}
			break;
	}/*  switch(dir)  */
#if 0    /*  ここのコメントを外すと、透明ウィンドウエリアが自動的に移動 */
	SCL_SetWindow(SCL_W0, (Uint32)NULL, SCL_NBG1, (Uint32)NULL,
				 win_left, win_up, win_right, win_down);
#endif
#if 1   /*  ライブラリ バグ検証用  */
	lineStatus[0] =  Scl_n_reg.lineaddr[0];  /*  NBG0 用 */
	lineStatus[1] =  Scl_n_reg.lineaddr[1];  /*  NBG1 用 */
	lineStatus[2] =  Scl_n_reg.linecontrl;
	lineStatus[3] =  Scl_n_reg.celladdr;
#endif
	}  /*  while(1)  */
}

/********************************************************************
 * ラインパラメータテーブルのダミーを作成する                       *
 ********************************************************************/
#define X_SIZE		320
#define Y_SIZE		224
#define Y_SIZE_H	112

void SetAutoLinePara(Uint16 SclNum, void *lineTableAddr)
{
    Fixed32 count;
    SclLineparam lp;

    SCL_InitLineParamTb(&lp);

    lp.delta_enbl=ON;
    lp.v_enbl=ON;
    lp.h_enbl=ON;
    lp.line_addr = (Uint32)lineTableAddr;
    lp.interval=SCL_1_LINE;

	if(SclNum == SCL_NBG0 ){
	    for(count=0;count<Y_SIZE;count++)
    	{
	    	lp.line_tbl[count].h = FIXED(count)/2;
			lp.line_tbl[count].v = FIXED(count)*2;
			lp.line_tbl[count].dh
			= DIV_FIXED((FIXED(X_SIZE)-(2* lp.line_tbl[count].h)),FIXED(X_SIZE));
    	}
	}else{  /*  NBG1  */
	    for(count=0;count<Y_SIZE;count++)
    	{
	    	lp.line_tbl[count].h = FIXED(Y_SIZE - count)/2;
			lp.line_tbl[count].v = FIXED(count - Y_SIZE)*2;
			lp.line_tbl[count].dh
			= DIV_FIXED((FIXED(X_SIZE)-(2* lp.line_tbl[count].h)),FIXED(X_SIZE));
    	}
	}
    SCL_Open(SclNum);
	SCL_SetLineParam(&(lp));
    SCL_Close();
}

void  set_gdata_NBG1(void){
	Uint16		*wp;
	Uint32		i,j;

	/*****************************************
	*	背景のタイルをＶＲＡＭに転送     *
	*****************************************/
	/* カラーＲＡＭの確保とパレットの転送 */
	SCL_AllocColRam(SCL_NBG1,256,ON);
	SCL_SetColRam(SCL_NBG1,1,256,BackPalette);

	/* キャラクタパターンデータをＶＲＡＭに転送 */
/*	memcpy(SCL_VDP2_VRAM_B0,BackCharPatData,1024);		*/
	memcpy((void *)SCL_VDP2_VRAM_B0,(void *)BackCharPatData,1024);
							/* henko 95.7.11 Chikahiro Yoshida	*/

	/* パターンネームデータをＶＲＡＭに転送 */
	for(i=0;i<16;i++)	for(j=0;j<16;j++) {
		wp  = (Uint16 *)(PN_NBG1_ADDR + i*2*64 + j*2*2);
		*wp = MAP_OFFSET+0;
		wp++;
		*wp = MAP_OFFSET+2;
		wp += 31;
		*wp = MAP_OFFSET+4;
		wp++;
		*wp = MAP_OFFSET+6;
	}

}

/********************************************************************
 * ラインパラメータテーブルを無効にする                             *
 ********************************************************************/
void ClrLinePara(Uint16 SclNum)
{
    SclLineparam lp;

    SCL_InitLineParamTb(&lp);

    SCL_Open(SclNum);
	SCL_SetLineParam(&(lp));
    SCL_Close();
}
