/*------------------------------------------------------------------------
 *  FILE: dblbmp.c
 *
 *	Copyright(c) 1994 SEGA
 *
 *  PURPOSE:
 *		VDP2 Test Program
 *         NBG0(8BIT BMP 512*256) & NBG1(8BIT BMP 512*256)
 *
 *  AUTHOR(S):
 *		A.H
 *
 *  MOD HISTORY:  dblbmp.c
 *		Written by A.H on 1997-01-22 Ver.1.00
 *  MOD HISTORY:  smpscl5.c
 *		Written by K.M on 1994-07-29 Ver.1.00
 *		Updated by K.M on 1994-07-29 Ver.1.00
 * 		Updatad by C.Y on 1995-07-11 Ver.1.00a
 *------------------------------------------------------------------------
 */
#include	<stdio.h>
#include	<machine.h>
#include	<sega_scl.h> 

#include	"..\..\v_blank\v_blank.h"

	Uint16		PadData1EW,PadData2EW;
	SclConfig	Nbg0Scfg,Nbg1Scfg;

#define X_SIZE	512
#define Y_SIZE	256


/* フォントデータマッチングテーブル */
static	Uint8	bitpat[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
static	Uint8	Japan=1;	/* かな表示モードにする */

void	Print256(Uint8 *vram,Uint8 *str,Uint16 x,Uint16 y,Uint8 col,Uint16 back);
void	Print256S(Uint8 *vram,Uint8 *str,Uint16 x,Uint16 y,Uint8 col,Uint16 back);
void	ClrVram(Uint8 *buff);

extern	Uint8	AsciiFontData[];/* 8x16 ASCII Font アルファベット部分 */
extern	Uint8	KanaFontData[];	/* 8x16 ASCII Font かな部分 */
extern	Uint8	EouFontData[];	/* 8x16 ASCII Font ヨーロッパ文字？部分 */

/* スクロール用グラフィックデータのポインタ */

Uint16	CycleTb[]={  /* サイクルパターン */
	0x44FF,0xffff,
	0xffff,0xffff,
	0x55FF,0xffff,
	0xffff,0xffff
};


Uint8 *SerchFont8(Uint8 code)
{
	if(code >= 0x20 && code < 0x80)
	    return((Uint8 *)(AsciiFontData+((Uint32 )(code-0x20))*16));
	else if(code > 0xa0) {
	    if(Japan)	return((Uint8 *)(KanaFontData+((Uint32 )(code-0xa1))*16) );
	    else	return((Uint8 *)(EouFontData+((Uint32 )(code-0xa1))*16) );
	}
	return((Uint8 *)AsciiFontData);
}


/*****************************************************************
 * テクスチャーバッファにアスキーコード文字を１文字書く          *
 *****************************************************************/
void  Print1Char256_8x16(Uint8 *vram,Uint8 codedat,Uint16 x,Uint16 y,Uint8 col,Uint16 back)
{
	Uint32	wx,wy,wp;
	Uint32	i,j;
	Uint8	*code;

	code = SerchFont8(codedat);

	wx = x;
	wy = y;

	wp = wx	+ wy * X_SIZE;
	if(back < 256) {
		for(i=0;i<16;i++) {
			for(j=0;j<8;j++) {
				if(code[i] & bitpat[j])	vram[wp]= col;
				else			vram[wp]= back;
				wp++;
			}
			wp += (X_SIZE - 8);
		}
	}else{
		for(i=0;i<16;i++) {
			for(j=0;j<8;j++) {
				if(code[i] & bitpat[j])	vram[wp]= col;
				wp++;
			}
			wp += (X_SIZE - 8);
		}
	}
}

/*****************************************************************
 * テクスチャーバッファにアスキーコード文字列を書く              *
 *****************************************************************/
void  Print256(Uint8 *vram,Uint8 *str,Uint16 x,Uint16 y,Uint8 col,Uint16 back)
{
	Uint32	i;
	Uint16	wx,wy;

	wx = x;wy = y;

	/* センタリング */
	if(wx > X_SIZE)	wx = X_SIZE/2 - strlen(str)*4;

	i=0;
	while(str[i]) {
		if(str[i] > 0x80 && str[i] < 0xA0) {
			/* シフトＪＩＳ全角コードだったらスペースをあける。 */
			wx += 16;
			i++;
		}else if(str[i]=='\\'){
			i++;
			if(str[i]=='n') {
				wx = x;wy += 18;/* 改行 */
			}
		}else if(str[i]==0x0a) {
			wx = x;wy += 18;/* 改行 */
		}else{
			Print1Char256_8x16(vram,str[i],wx,wy,col,back);
			wx += 8;
		}
		if(wx+8 >= X_SIZE) {
			wx = x;wy += 18;/* 改行 */
		}
		i++;
	}
}

void	ClrVram(Uint8 *buff)
{
	Uint32	i;

	for(i=0;i<(X_SIZE * Y_SIZE);i++)	buff[i]=0x00;
}

void	Print256S(Uint8 *vram,Uint8 *str,Uint16 x,Uint16 y,Uint8 col,Uint16 back)
{
	/* 文字の台 */
	Print256(vram,str,x+1,y+1,1,back);
	Print256(vram,str,x+1,y  ,1,300);
	Print256(vram,str,x  ,y+1,1,300);
	/* 文字の本体 */
	Print256(vram,str,x,y,col,300);
}


void main()
{
	SclVramConfig	tp;
	Uint16		BackCol;
	Fixed32		r;
	Uint32		Color[16];
	Uint32		Surface;
	Uint8		sw,PrSw;


	/*******************************************
	*	各種初期化処理                     *
	*******************************************/
	SCL_Vdp2Init();
	SetVblank();
	set_imask(0);
	SCL_SetFrameInterval(1);
	SCL_DisplayFrame();

	SCL_SetDisplayMode(SCL_NON_INTER,SCL_240LINE,SCL_NORMAL_A);
	SCL_SetCycleTable(CycleTb);

	SCL_SetColRamMode(SCL_CRM24_1024);

	/*******************************************
	*	ＶＲＡＭの使用方法の設定           *
	*******************************************/
	SCL_InitVramConfigTb(&tp);
	tp.vramModeA  = ON;		/* VRAM A を分割する */
	tp.vramModeB  = ON;		/* VRAM B を分割する */
	tp.colram     = SCL_NON;	/* カラーRAMに回転係数テーブルを置かない */
	SCL_SetVramConfig(&tp);

	/****************************************
	*	プライオリティの設定            *
	****************************************/
	SCL_SetPriority(SCL_NBG0,4);
	SCL_SetPriority(SCL_NBG1,3);

  /* Set Color Data into Color RAM */

	    Color[0] = 0x00000000;
	    Color[1] = RGB32_COLOR(  0,  0,  0) & 0x00ffffff;
	    Color[2] = RGB32_COLOR(255,255,255) & 0x00ffffff;
	    Color[3] = RGB32_COLOR(255,  0,  0) & 0x00ffffff;
	    Color[4] = RGB32_COLOR(255,255,  0) & 0x00ffffff;
	    Color[5] = RGB32_COLOR(  0,  0,255) & 0x00ffffff;
	    Color[6] = RGB32_COLOR(  0,255,  0) & 0x00ffffff;

	SCL_AllocColRam(SCL_NBG0,256,OFF);
	SCL_SetColRam(SCL_NBG0,0,7,Color);
	SCL_AllocColRam(SCL_NBG1,256,OFF);
	SCL_SetColRam(SCL_NBG1,0,7,Color);

   /*  NBG0 */
	ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);
	Print256S((Uint8 *)SCL_VDP2_VRAM_A0,
		(Uint8 *)"VDP2 Test Program",10,20,2,0);
	Print256S((Uint8 *)SCL_VDP2_VRAM_A0,
		(Uint8 *)"Display NBG0 & NBG1",10,40,3,0);
	Print256S((Uint8 *)SCL_VDP2_VRAM_A0,
		(Uint8 *)" NBG0:CHAR(BitMap)",10,60,4,0);
	Print256S((Uint8 *)SCL_VDP2_VRAM_A0,
		(Uint8 *)"S BUTTON:INIT\n",20,100,6,0);
	Print256S((Uint8 *)SCL_VDP2_VRAM_A0,
		(Uint8 *)"A BUTTON:NBG0\nB BUTTON:NBG1\nC BUTTON:PRIORITY Change",180,100,6,0);

   /*  NBG1 */
	ClrVram((Uint8 *)SCL_VDP2_VRAM_B0);
	Print256S((Uint8 *)SCL_VDP2_VRAM_B0,
		(Uint8 *)"VDP2 Test Program",10,20,2,0);
	Print256S((Uint8 *)SCL_VDP2_VRAM_B0,
		(Uint8 *)"Display NBG0 & NBG1",10,40,3,0);
	Print256S((Uint8 *)SCL_VDP2_VRAM_B0,
		(Uint8 *)" NBG1: CHAR(BMP)",10,80,5,0);
	Print256S((Uint8 *)SCL_VDP2_VRAM_B0,
		(Uint8 *)"S BUTTON:INIT\n",20,100,6,0);
	Print256S((Uint8 *)SCL_VDP2_VRAM_B0,
		(Uint8 *)"A BUTTON:NBG0\nB BUTTON:NBG1\nC BUTTON:PRIORITY Change",180,100,6,0);

	/*******************************************
	*	バック画面の色を黒に設定               *
	*******************************************/
	BackCol = RGB16_COLOR(0,0,0) & 0x7fff;
	SCL_SetBack(SCL_VDP2_VRAM+0x80000-2,1,&BackCol);

	/**************************************
	*	スクロール画面の初期描画          *
	**************************************/
	/*************************************
	*	NBG0の設定                *
	*************************************/
	SCL_InitConfigTb(&Nbg0Scfg);
	    Nbg0Scfg.dispenbl = ON;
	    Nbg0Scfg.bmpsize = SCL_BMP_SIZE_512X256;
	    Nbg0Scfg.coltype  = SCL_COL_TYPE_256;
	    Nbg0Scfg.datatype = SCL_BITMAP;
		Nbg0Scfg.plate_addr[0] = SCL_VDP2_VRAM_A0; /* BMPデータ格納アドレス */
	SCL_SetConfig(SCL_NBG0, &Nbg0Scfg);

	/*************************************
	*	NBG1の設定                *
	*************************************/
	SCL_InitConfigTb(&Nbg1Scfg);
	    Nbg1Scfg.dispenbl = ON;
	    Nbg1Scfg.bmpsize = SCL_BMP_SIZE_512X256;
	    Nbg1Scfg.coltype  = SCL_COL_TYPE_256;
	    Nbg1Scfg.datatype = SCL_BITMAP;
		Nbg1Scfg.plate_addr[0] = SCL_VDP2_VRAM_B0; /* BMPデータ格納アドレス */
	SCL_SetConfig(SCL_NBG1, &Nbg1Scfg);

	SCL_Open(SCL_NBG0 | SCL_NBG1);
	  SCL_MoveTo(FIXED(0), FIXED(0), FIXED(0));/* Home Position */
	  SCL_Scale(FIXED(1.0), FIXED(1.0));
	SCL_Close();
	SCL_DisplayFrame();

	/**************************************
	*	パッドを使った画面の操作      *
	**************************************/
	r = FIXED(2);
	sw = 0;
	PrSw = ON;
	Surface = SCL_NBG0;
	while(1){
		PadData1EW=PadData1E;	PadData1E=0;
		PadData2EW=PadData2E;	PadData2E=0;

		if(PadData1 || PadData1EW){
			SCL_Open(Surface);
			if(PadData1 & PAD_U)
				SCL_Move( 0, r, 0);
			else if(PadData1 & PAD_D)
				SCL_Move( 0,-r, 0);
			if(PadData1 & PAD_R)
				SCL_Move(-r, 0, 0);
			else if(PadData1 & PAD_L)
				SCL_Move( r, 0, 0);
			if(PadData1 & PAD_RB)
				SCL_Move( 0, 0,-r);
			else if(PadData1 & PAD_LB)
				SCL_Move( 0, 0, r);
			if((PadData1 & PAD_S)) {
				SCL_MoveTo(0,0,0);
			}
			if((PadData1 & PAD_A))
				Surface = SCL_NBG0;
			if((PadData1 & PAD_B))
				Surface = SCL_NBG1;
			if((PadData1EW & PAD_C)) {
				if(PrSw) {
					SCL_SetPriority(SCL_NBG1,5);
					PrSw=OFF;
				}else{
					SCL_SetPriority(SCL_NBG1,3);
					PrSw=ON;
				}
			}
			SCL_Close();
		}
		SCL_DisplayFrame();
	}
}

/*****************************************************************
 * 文字コードから文字パターンの先頭アドレスを探して返す          *
 *****************************************************************/
