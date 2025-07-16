/********************************************************************
*  FILE:    btest.c
*
*   Copyright(c) 1994 SEGA
*
*  PURPOSE:
*   「バックアップライブラリ」テストプログラム
*
*  AUTHOR(S):
*   K.M
*
*  MOD HISTORY:
*   Written by K.M on 1994-08-27 Ver.1.00
*   Updated by K.M on 1994-09-12 Ver.1.00
*   Updated by M.K on 1994-09-12 Ver.1.10
*   Updated by H.K on 1996-08-06 Ver.1.21
*   Updated by H.K on 1996-10-01 Ver.1.22
*   Updated by H.K on 1997-01-08 Ver.1.23
*   Updated by H.K on 1997-03-12 Ver.1.24
*   Updated by A.H on 1997-08-04 Ver.1.25
*           1.バックアップライブラリ Ver1.25 で追加したマクロに対応
*           2.BUPデータのアクセス前後に、リセットボタン無効処理を追加
*           3.BUPデータのリードバッファ容量を定義
*           4.リードバッファを超えるサイズのデータを、リード前にチェック
*           5.過剰な volatile指定を、削除
*   Updated by A.H on 1997-08-30 Ver.1.26
*           1.メッセージ表示に使用していた半角カナコードを、削除。
*
********************************************************************/

#include    <stdio.h>
#include    <machine.h>
#include    "sega_scl.h"
#include    "sega_per.h"
#include    "sega_bup.h"
#if 0
#include    "..\v_blank\v_blank.h"
#else
#include    "v_blank.h"
#endif
#if 0
#include    "font\smp_font.h"
#else
#include    "smp_font.h"
#endif
#include    "sega_sys.h"
#include    "sega_cdc.h"


#define     CLOCK_26    0
#define     CLOCK_28    1

#define		BUFFER_SIZE		( 1024L )		/* リードバッファサイズ */
#define		DIR_SIZE		1000
#define		SIZE_ERR		20

#define		RESET_PAD		(PAD_A | PAD_B | PAD_C | PAD_S)

/* バックアップライブラリを使用する為に必要な変数の定義 */
Uint32  BackUpLibWork[BUP_LIB_SIZE4];   /* ライブラリ本体の展開領域 */
Uint32  BackUpRamWork[BUP_WORK_SIZE4];  /* ライブラリ用ワーク領域 */
BupConfig   CnTb[3];
BupDir      DirTb[DIR_SIZE];
BupDate     DateTb;
BupStat     StTb;
Uint32      device;
Uint32      clock;


/* 関数(マクロ)宣言 */
/*------ A & B & C & START -- Go MP-------*/
#define		CHK_softreset()	\
        if( ((PadData1 & RESET_PAD)==RESET_PAD) && (PadData1E & PAD_S) ) { \
            SYS_Exit(0); \
        }

Bool isHirqOn(Sint32 flag);

void CDOpenCheck(void);

void    DispInit(Uint32 clock);
Uint32  MainMenu(Uint32 *sel);
void    PadInputWait(void);
void    ClrVram(Uint8 *buff);
void    err_disp(Sint32 err,Sint32 sw);
void    BackUpInit(BupConfig cntb[3]);
Sint32  BackUpWrite(Uint32 device,BupDir *dir,Uint8 *data,Uint8 sw);
Sint32  BackUpDelete(Uint32 device,Uint8 *filename);
Sint32  BackUpFormat(Uint32 device);
void    DispDate(char *comment,Uint16 x,Uint16 y);
Uint32  datasize_chk( Uint8 * );


/* 変数定義 */
Uint8   DummyData[] = "Dummy Data";
Uint8   DummyData2[] = "Bench Test";

Uint8   Week[7][4] = {
    "SUN",
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT"
};

Uint8	ReadBuffer[ BUFFER_SIZE ];

#if 0
#ifdef __GNUC__
void _main(void)
{
}
#endif
#endif


void main(void)
{
/* #if !(__GNUC__) */
#ifndef __GNUC__
    Uint32      i,fp;
    Sint32      num,ret;
    Uint32      sel,dummy,size,count;
    Uint16      now_part[3];
    Uint8       stat,work;
#else
    Uint32      fp;
    Sint32      num, ret;
    Uint32      sel, size, count;
    Uint16      i, now_part[3], dummy;
    Uint8       work;
#endif
    Uint8       buff[100];


	CDOpenCheck();    /* 最初に トレイオープンチェック */

    i=0;
    dummy = 0;
    sel   = 0;
    now_part[0] = now_part[1] = now_part[2] = 0;
    size  = 0;


    clock = CLOCK_26;
    DispInit(clock);

    /* バックアップライブラリ初期化 */
    BackUpInit(CnTb);

    while(1)
    {
    	switch(MainMenu(&sel)){
        case    1:
            /* バックアップライブラリ初期化 */
            BackUpInit(CnTb);
/* #if !(__GNUC__) */
#ifndef __GNUC__
            ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);
            sprintf((char *)buff,"Main Unit:Id=%d,Part=%d",
                                        CnTb[0].unit_id,CnTb[0].partition);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,40,2,0);
            sprintf((char *)buff,"Cartridge:Id=%d,Part=%d",
                                        CnTb[1].unit_id,CnTb[1].partition);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,60,2,0);
            sprintf((char *)buff,"Serial   :Id=%d,Part=%d",
                                        CnTb[2].unit_id,CnTb[2].partition);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,80,2,0);
#else
            ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);
            sprintf((char *)buff,"Main Unit:Id=%d,Part=%d",
                                        CnTb[0].unit_id,CnTb[0].partition);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,40,2,0);
            sprintf((char *)buff,"Cartridge:Id=%d,Part=%d",
                                        CnTb[1].unit_id,CnTb[1].partition);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,60,2,0);
            sprintf((char *)buff,"Serial   :Id=%d,Part=%d",
                                        CnTb[2].unit_id,CnTb[2].partition);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,80,2,0);
#endif
            now_part[0] = now_part[1] = now_part[2] = 0;
            PadInputWait();/* パッド入力待ち */
            dummy = 0;
            break;
        case    2:
            /* パーティション選択 */
            if(CnTb[device].unit_id){  /* デバイス接続チェック */
            	if(CnTb[device].partition > 1)   /* パーティション数チェック */
            	{
            		now_part[device]++;
            		now_part[device] &= 1;
            		PER_SMPC_RES_DIS();/* リセットボタン無効 */
            			err_disp( BUP_SelPart(device,now_part[device]),ON);
            		PER_SMPC_RES_ENA();/* リセットボタン有効 */
            		break;
            	}else{   /* NG !!  */
            		ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);
            		sprintf((char *)buff,"Error!" );
            		FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,40,2,0);
            		sprintf((char *)buff,"   1 Partition only." );
            		FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,60,2,0);
            		PadInputWait();/* パッド入力待ち */
            		break;
            	}
            }else{
            	ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);
            	sprintf((char *)buff,"Error!" );
            	FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,40,2,0);
            	sprintf((char *)buff,"   DEVICE not READY." );
            	FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,60,2,0);
            	PadInputWait();/* パッド入力待ち */
            	break;
            }
        case    3:
            /* Ｆｏｒｍａｔ */
/* #if !(__GNUC__) */
#ifndef __GNUC__
            ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);
            sprintf((char *)buff,"Format ");
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20, 40,2,0);
#else
            ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);
            sprintf((char *)buff,"Format ");
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20, 40,2,0);
#endif
            err_disp( BackUpFormat(device),ON);
            break;
        case    4:
            /* ステイタス取得 */
            PER_SMPC_RES_DIS();/* リセットボタン無効 */
                err_disp( BUP_Stat(device,10,&StTb),OFF);
            PER_SMPC_RES_ENA();/* リセットボタン有効 */
            sprintf((char *)buff,"Total Size  :%10ld",StTb.totalsize);
/* #if !(__GNUC__) */
#ifndef __GNUC__
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20, 40,2,0);
            sprintf((char *)buff,"Total Block :%10ld",StTb.totalblock);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20, 60,2,0);
            sprintf((char *)buff,"Block Size  :%10ld",StTb.blocksize);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20, 80,2,0);
            sprintf((char *)buff,"Free Size   :%10ld",StTb.freesize);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,100,2,0);
            sprintf((char *)buff,"Free Block  :%10ld",StTb.freeblock);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,120,2,0);
            sprintf((char *)buff,"Data Num    :%10ld",StTb.datanum);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,140,2,0);
#else
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20, 40,2,0);
            sprintf((char *)buff,"Total Block :%10ld",StTb.totalblock);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20, 60,2,0);
            sprintf((char *)buff,"Block Size  :%10ld",StTb.blocksize);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20, 80,2,0);
            sprintf((char *)buff,"Free Size   :%10ld",StTb.freesize);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,100,2,0);
            sprintf((char *)buff,"Free Block  :%10ld",StTb.freeblock);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,120,2,0);
            sprintf((char *)buff,"Data Num    :%10ld",StTb.datanum);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20,140,2,0);
#endif
            PadInputWait();
            break;
        case    5:
            /* ファイル書き込み */
            sprintf((char *)DirTb[0].filename,"DUMMYDAT%03d",dummy);
            sprintf((char *)DirTb[0].comment,"COMMENT%03d",dummy);
            DirTb[0].language = BUP_JAPANESE;
            DirTb[0].datasize = sizeof(DummyData);
            DirTb[0].date = 0;
            err_disp(BackUpWrite(device,&DirTb[0],DummyData,(Uint8 )ON),ON);
            dummy++;
            break;
        case    6:
            /* ファイル読み込み */
			if (  BUFFER_SIZE > DirTb[fp].datasize ){  /* データサイズチェック */
	            PER_SMPC_RES_DIS();/* リセットボタン無効 */
/* #if !(__GNUC__) */
#ifndef __GNUC__
	            err_disp( BUP_Read(device,DirTb[fp].filename,
                                            ReadBuffer),OFF);
#else
	            err_disp( BUP_Read(device,DirTb[fp].filename,
                                            ReadBuffer),OFF);
#endif
	            PER_SMPC_RES_ENA();/* リセットボタン有効 */
/* #if !(__GNUC__) */
#ifndef __GNUC__
	            work = (ReadBuffer)[100];
	            (ReadBuffer)[100] = 0;   /* 画面表示の強制中断用 NULL */
	            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                                        ReadBuffer,20,40,2,0);
	            PadInputWait();
	            (ReadBuffer)[100] = work;
#else
	            work = (ReadBuffer)[100];
	            (ReadBuffer)[100] = 0;
	            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                                ReadBuffer,20,40,2,0);
	            PadInputWait();
	            (ReadBuffer)[100] = work;
#endif
			}else{
				/* データサイズが、リードバッファ容量を超えた */
				err_disp( SIZE_ERR,ON);
			}
            break;
        case    7:
            /* ファイルの削除 */
            err_disp( BackUpDelete(device,DirTb[fp].filename),ON );
            break;
        case    8:
            /* ファイル一覧取得 */
            PER_SMPC_RES_DIS();/* リセットボタン無効 */
                num = BUP_Dir(device ,(Uint8 *)"" ,DIR_SIZE ,DirTb);
            PER_SMPC_RES_ENA();/* リセットボタン有効 */
/* #if !(__GNUC__) */
#ifndef __GNUC__
            ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);
#else
            ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);
#endif

            sprintf((char *)buff,"Hit:%08ld",num);
/* #if !(__GNUC__) */
#ifndef __GNUC__
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,50,10,2,0);
#else
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,50,10,2,0);
#endif
            if( num <= 0 )    PadInputWait();

            fp = 0;
            for(i=0;i<num;i++){
                sprintf((char *)buff,"FileName :%11s",DirTb[i].filename);
/* #if !(__GNUC__) */
#ifndef __GNUC__
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20, 40,2,0);
                sprintf((char *)buff,"Comment  :%10s",DirTb[i].comment);
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20, 60,2,0);
                sprintf((char *)buff,"Language :%10d",DirTb[i].language);
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20, 80,2,0);
#else
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                                                        buff,20, 40,2,0);
                sprintf((char *)buff,"Comment  :%10s",DirTb[i].comment);
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                                                        buff,20, 60,2,0);
                sprintf((char *)buff,"Language :%10d",DirTb[i].language);
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                                                        buff,20, 80,2,0);
#endif
                /* データアクセスを伴わない為、リセットボタン処理は不要 */
                BUP_GetDate(DirTb[i].date,&DateTb);
                sprintf((char *)buff,"Date     :%04d-%02d-%02d(%s)  %2d:%02d",
                        ((Uint16)DateTb.year)+1980,
                        DateTb.month,DateTb.day,
                        Week[DateTb.week],
                        DateTb.time,DateTb.min);
/* #if !(__GNUC__) */
#ifndef __GNUC__
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20, 100,2,0);
#else
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                                                        buff,20, 100,2,0);
#endif
                sprintf((char *)buff,"Size     :%10ld",DirTb[i].datasize);
/* #if !(__GNUC__) */
#ifndef __GNUC__
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20, 120,2,0);
#else
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                                                        buff,20, 120,2,0);
#endif
/* #if !(__GNUC__) */
#ifndef __GNUC__
                sprintf((char *)buff,"BlockSize:%10ld",DirTb[i].blocksize);
#else
                sprintf((char *)buff,"BlockSize:%10ld",(Sint32)DirTb[i].blocksize);
#endif
/* #if !(__GNUC__) */
#ifndef __GNUC__
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,20, 140,2,0);
#else
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                                                        buff,20, 140,2,0);
#endif
                PadInputWait();
                if(PadData1 & PAD_B){
                    fp=i;
                    i=num;
                }
            }
            break;
        case    9:
            /* ベリファイ */
           PER_SMPC_RES_DIS();/* リセットボタン無効 */
/* #if !(__GNUC__) */
#ifndef __GNUC__
            err_disp( BUP_Verify(device,DirTb[fp].filename,
                                                ReadBuffer),ON);
#else
            err_disp( BUP_Verify(device,DirTb[fp].filename,
                                        ReadBuffer),ON);
#endif
           PER_SMPC_RES_ENA();/* リセットボタン有効 */
            break;
        case    10:
            /* クロックチェンジ */
            if(clock == CLOCK_26)   clock = CLOCK_28;
            else            clock = CLOCK_26;
            DispInit(clock);
            BackUpInit(CnTb);
            break;
        case    15:
            /*  Dummy FULL */
            PER_SMPC_RES_DIS();/* リセットボタン無効 */
                BUP_Stat(device,0,&StTb);
            PER_SMPC_RES_ENA();/* リセットボタン有効 */
            size = StTb.freesize;
        case    11:
            /*  Dummy 100 */
            if(!size)    size =    100;
        case    12:
            /*  Dummy 1000 */
            if(!size)    size =    1000;
        case    13:
            /*  Dummy 10000 */
            if(!size)    size =    10000;
        case    14:
            /*  Dummy 100000 */
            if(!size)    size =    100000;
            /* ファイル書き込み */
            sprintf((char *)DirTb[0].filename,"DUMMYDAT%03d",dummy);
            sprintf((char *)DirTb[0].comment,"COMMENT%03d",dummy);
            DirTb[0].language = BUP_JAPANESE;
            DirTb[0].datasize = size;
            size = 0;
            DirTb[0].date = 0;
/* #if !(__GNUC__) */
#ifndef __GNUC__
            err_disp(BackUpWrite(device,&DirTb[0],
                                (Uint8 *)0x00200000,(Uint8 )ON),ON);
#else
            err_disp(BackUpWrite(device,&DirTb[0],
                                (Uint8 *)0x00200000,(Uint8 )ON),ON);
#endif
            dummy++;
            break;
        case    16:
            /* ファイルの無限オーバーライト */
/* #if !(__GNUC__) */
#ifndef __GNUC__
            ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"SEGA_BUP Lib Test Program",40,10,2,0);
#else
            ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"SEGA_BUP Lib Test Program",40,10,2,0);
#endif
            dummy = 0;
            count = 0;
            DispDate("Start:",50,100);

            DirTb[0].language = BUP_JAPANESE;
            DirTb[0].datasize = sizeof(DummyData2);
            DirTb[0].date = 0;

#if 0   /* 1997-09-03 A.H(SOJ) */
		    PadData1E = 0;
#else
			CHK_softreset();
			CDOpenCheck();
			SCL_DisplayFrame();
#endif
            while( !(PadData1E & (PAD_A | PAD_B | PAD_C)) ){
                sprintf((char *)DirTb[0].filename,"DUMMYDA%04d",dummy);
                sprintf((char *)DirTb[0].comment,"COMMENT%04d",dummy);
                ret = BackUpWrite(device,&DirTb[0],DummyData2,(Uint8 )OFF);
                sprintf((char *)buff,"Write :%s",DirTb[0].filename);
/* #if !(__GNUC__) */
#ifndef __GNUC__
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,50,40,2,0);
#else
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,50,40,2,0);
#endif
                if(ret == BUP_NOT_ENOUGH_MEMORY){
                    count++;
                    sprintf((char *)buff,"Count :%11ld",count);
/* #if !(__GNUC__) */
#ifndef __GNUC__
                    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,50,60,2,0);
#else
                    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,50,60,2,0);
#endif
                    if( (PadData1E & PAD_C) ){
#if 0
                       PadData1E |= PAD_B;
#endif
                    }else for(i=0;i<dummy;i++){
                        sprintf((char *)DirTb[0].filename,"DUMMYDA%04d",i);
                        BackUpDelete(device,DirTb[0].filename);
                        sprintf((char *)buff,"Delete:%s",DirTb[0].filename);
/* #if !(__GNUC__) */
#ifndef __GNUC__
                        FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,50,40,2,0);
#else
                        FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                                                            buff,50,40,2,0);
#endif
                    }
                    DispDate("Now  :",50,120);
/* #if !(__GNUC__) */
#ifndef __GNUC__
                    sprintf((char *)buff,"Sector:%11ld",dummy);
#else
                    sprintf((char *)buff,"Sector:%11ld",(Sint32)dummy);
#endif
/* #if !(__GNUC__) */
#ifndef __GNUC__
                    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,50,80,2,0);
#else
                    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                                                            buff,50,80,2,0);
#endif
                    dummy = 0;
                    if( (PadData1E & PAD_A) ){
#if 0
                        PadData1E |= PAD_B;
#endif
                    }
                }  /* if(ret == BUP_NOT_ENOUGH_MEMORY)  */
                else    dummy++;
#if 1   /* 1997-09-03 追加  A.H(SOJ) */
		    	CHK_softreset();
		    	CDOpenCheck();
		    	SCL_DisplayFrame();
#endif
            }  /*   while( !(PadData1E & PAD_B) )  */
            DispDate("End  :",50,120);
            PadInputWait();
            break;    /* ファイルの無限オーバーライト */
        }  /* switch(MainMenu(&sel)) */
#if 0   /* 1997-09-03 追加  A.H(SOJ) */
    	CHK_softreset();
    	CDOpenCheck();
    	SCL_DisplayFrame();
#endif
    }  /* while(1) */
}


void    ClrVram(Uint8 *buff)
{
    Sint32  i;

#if 0
    for(i=0;i<(512 * 256);i++)  buff[i]=0x00;
#else
#if 1
    for(i=0;i<(512 * 256 / 4);i++)  ((volatile int *)buff)[i]=0x00;
#else
    for(i=((512 * 256 - 1) / 4);i > -1 ;i--)  ((volatile int *)buff)[i]=0x00;
#endif
#endif
}


static  Uint32  color[7] = {
    0x00000000,/* 透明色になるところだから何を入れても良い */
    0x00000000,/* 黒 */
    0x00FFFFFF,/* 白 */
    0x00FF0000,/* 青 */
    0x0000FF00,/* 緑 */
    0x000000FF,/* 赤 */
    0x00008000 /* 深緑 */
};

#define BLACK   1
#define WHITE   2
#define BLUE    3
#define GREEN   4
#define RED 5
#define DGREEN  6


void    DispInit(Uint32 clock)
{
    Uint32      i;
    SclConfig   Nbg0Scfg;
    Uint16      BackCol;
	int imask;


    SCL_Vdp2Init();
    SCL_SetFrameInterval(1);
	ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);

	imask = get_imask();
	set_imask(15);		/* SH2 割込み 全マスク */
    SetVblank();  /* V-BLANKルーチンの登録 */
    set_imask(imask);
    *(volatile Uint16 *)0x25F80000 &= 0x7fff;/* 画面表示をＯＦＦにする */

    if(clock == CLOCK_26)
        SCL_SetDisplayMode(SCL_NON_INTER,SCL_240LINE,SCL_NORMAL_A);
    else
        SCL_SetDisplayMode(SCL_NON_INTER,SCL_240LINE,SCL_NORMAL_B);
    PER_SMPC_RES_ENA();/* リセットボタン有効 */

    SCL_SetPriority(SCL_NBG0,0);
    SCL_SetColRamMode(SCL_CRM24_1024);
    BackCol = RGB16_COLOR(0,0,0) & 0x7fff;
/* #if !(__GNUC__) */
#ifndef __GNUC__
    SCL_SetBack(SCL_VDP2_VRAM+0x80000-2,1,&BackCol);
#else
    SCL_SetBack((volatile)(SCL_VDP2_VRAM+0x80000-2),1,&BackCol);
#endif
    FNT_SetBuffSize(512,256,FNT_JAPAN);


    while( NULL == SCL_AllocColRam(SCL_NBG0,256,OFF) )
	    SCL_DisplayFrame();
    SCL_SetColRam(SCL_NBG0,0,7,color);


    /* サイクルパターン設定 */
#if 0
	Scl_s_reg.vramcyc[0] = 0x44FF;  /* NBG0 256 Bitmap */
#else
	{
	Uint16 cycletable[] = { 0x44fe,0xeeee,
	                        0xfffe,0xeeee,
	                        0xffff,0xffff,
	                        0xffff,0xffff,
	                      };
	SCL_SetCycleTable( cycletable );
	}
#endif

   /* NBG0 の設定 */
    SCL_InitConfigTb(&Nbg0Scfg);
        Nbg0Scfg.dispenbl = ON;
        Nbg0Scfg.coltype  = SCL_COL_TYPE_256;
        Nbg0Scfg.datatype = SCL_BITMAP;
        for(i=0;i<4;i++)
/* #if !(__GNUC__) */
#ifndef __GNUC__
        Nbg0Scfg.plate_addr[i] = SCL_VDP2_VRAM_A0;
#else
        Nbg0Scfg.plate_addr[i] = (volatile) SCL_VDP2_VRAM_A0;
#endif
    SCL_SetConfig(SCL_NBG0, &Nbg0Scfg);


    /**************************************
    *   スクロール画面の初期描画      *
    **************************************/
    SCL_Open(SCL_NBG0);
        SCL_MoveTo(FIXED(0), FIXED(0), FIXED(0));
        SCL_Scale(FIXED(1.0), FIXED(1.0));
    SCL_Close();

    SCL_DisplayFrame();

    SCL_SetPriority(SCL_NBG0,5);
}


Uint32  MainMenu(Uint32 *sel)
{
    Uint32  sw,i;
    Uint16  back[20];
    Uint16  PadData1EW;

/* #if !(__GNUC__) */
#ifndef __GNUC__
    ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
        (Uint8 *)"SEGA_BUP Lib Test Program",40,10,2,0);
#else
    ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
        (Uint8 *)"SEGA_BUP Lib Test Program",40,10,2,0);
#endif
    for(i=0;i<16;i++)   back[i] = 0;

#if 0   /* 1997-09-03 A.H(SOJ) */
	PadData1E = 0;
#endif
    PadData1EW = PadData1E;
    sw=1;
    /* A～C が押されない限り、ループ */
    while( !(PadData1EW & (PAD_A | PAD_B | PAD_C)) ){
        CHK_softreset();
        PadData1EW = PadData1E;
#if 0   /* 1997-09-03 A.H(SOJ) */
		PadData1E = 0;
#else
		CHK_softreset();
		CDOpenCheck();
		SCL_DisplayFrame();
#endif

        if(PadData1 & PAD_X){
            device = 0;
            sw = 1;
        }
        if(PadData1 & PAD_Y){
            device = 1;
            sw = 1;
        }
        if(PadData1 & PAD_Z){
            device = 2;
            sw = 1;
        }

#if 0
        if(PadData1EW & PAD_R){
            if(*sel < 8)    *sel += 8;
            sw=1;
        }else if(PadData1EW & PAD_L){
            if(*sel >= 8)   *sel -= 8;
            sw=1;
        }else if(PadData1EW & PAD_U){
            if(*sel < 8)    *sel=(*sel+7)%8;
            else        *sel=8+(*sel-8+7)%8;
            sw=1;
        }else if(PadData1EW & PAD_D){
            if(*sel < 8)    *sel=(*sel+1)%8;
            else        *sel=8+(*sel-8+1)%8;
            sw=1;
        }
#else
        if(PadData1E & PAD_R){
            if(*sel < 8)    *sel += 8;
            sw=1;
        }else if(PadData1E & PAD_L){
            if(*sel >= 8)   *sel -= 8;
            sw=1;
        }else if(PadData1E & PAD_U){
            if(*sel < 8)    *sel=(*sel+7)%8;
            else        *sel=8+(*sel-8+7)%8;
            sw=1;
        }else if(PadData1E & PAD_D){
            if(*sel < 8)    *sel=(*sel+1)%8;
            else        *sel=8+(*sel-8+1)%8;
            sw=1;
        }
#endif
        if(sw){
            if(clock == CLOCK_26)
/* #if !(__GNUC__) */
#ifndef __GNUC__
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                    (Uint8 *)"CPU Clock:26MHz", 20, 40,GREEN,0);
            else    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                    (Uint8 *)"CPU Clock:28MHz", 20, 40,BLACK,GREEN);
#else
                FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                    (Uint8 *)"CPU Clock:26MHz", 20, 40,GREEN,0);
            else    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                    (Uint8 *)"CPU Clock:28MHz", 20, 40,BLACK,GREEN);
#endif

            switch(device){
                case    0:
                    back[*sel] = BLUE;
/* #if !(__GNUC__) */
#ifndef __GNUC__
                    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                        (Uint8 *)"INTERNAL     ",184, 40,WHITE,0);
#else
                    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                        (Uint8 *)"INTERNAL     ",184, 40,WHITE,0);
#endif
                    break;
                case    1:
                    back[*sel] = DGREEN;
/* #if !(__GNUC__) */
#ifndef __GNUC__
                    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                        (Uint8 *)"EXT.(A-BUS)  ",184, 40,WHITE,0);
#else
                    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                        (Uint8 *)"EXT.(A-BUS)  ",184, 40,WHITE,0);
#endif
                    break;
                case    2:
                    back[*sel] = RED;
/* #if !(__GNUC__) */
#ifndef __GNUC__
                    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                        (Uint8 *)"FDD(SERIAL)   ",184, 40,WHITE,0);
#else
                    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                        (Uint8 *)"FDD(SERIAL)   ",184, 40,WHITE,0);
#endif
                    break;
            }

/* #if !(__GNUC__) */
#ifndef __GNUC__
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Init    ", 20, 60,WHITE,back[ 0]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_SelPart ", 20, 80,WHITE,back[ 1]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Format  ", 20,100,WHITE,back[ 2]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Stat    ", 20,120,WHITE,back[ 3]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Write   ", 20,140,WHITE,back[ 4]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Read    ", 20,160,WHITE,back[ 5]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Delete  ", 20,180,WHITE,back[ 6]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Dir     ", 20,200,WHITE,back[ 7]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Verify  ",180, 60,WHITE,back[ 8]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"Clock Change",180, 80,WHITE,back[ 9]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"Dummy    100",180,100,WHITE,back[10]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"Dummy   1000",180,120,WHITE,back[11]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"Dummy  10000",180,140,WHITE,back[12]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"Dummy 100000",180,160,WHITE,back[13]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"Dummy   FULL",180,180,WHITE,back[14]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"Dummy  Bench",180,200,WHITE,back[15]);
#else
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Init    ", 20, 60,WHITE,back[ 0]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_SelPart ", 20, 80,WHITE,back[ 1]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Format  ", 20,100,WHITE,back[ 2]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Stat    ", 20,120,WHITE,back[ 3]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Write   ", 20,140,WHITE,back[ 4]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Read    ", 20,160,WHITE,back[ 5]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Delete  ", 20,180,WHITE,back[ 6]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Dir     ", 20,200,WHITE,back[ 7]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"BUP_Verify  ",180, 60,WHITE,back[ 8]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"Clock Change",180, 80,WHITE,back[ 9]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"Dummy    100",180,100,WHITE,back[10]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"Dummy   1000",180,120,WHITE,back[11]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"Dummy  10000",180,140,WHITE,back[12]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"Dummy 100000",180,160,WHITE,back[13]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"Dummy   FULL",180,180,WHITE,back[14]);
            FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,
                (Uint8 *)"Dummy  Bench",180,200,WHITE,back[15]);
#endif
            back[*sel] = 0;
            sw = 0;
        }  /*  if(sw)  */
    } /* while( !(PadData1EW & (PAD_A | PAD_B | PAD_C)) ) */
    return(*sel+1);
}

/* パッド入力待ち */
void    PadInputWait(void)
{
#if 0
    PadData1E = 0;
#else
    CHK_softreset();
	CDOpenCheck();
	SCL_DisplayFrame();
#endif
    while( !(PadData1E) ){
    	CHK_softreset();
    	CDOpenCheck();
    	SCL_DisplayFrame();
    }
   	CHK_softreset();
	CDOpenCheck();
	SCL_DisplayFrame();
}

void    err_disp(Sint32 err,Sint32 wait)
{
    Uint8   buff[40];

/* #if !(__GNUC__) */
#ifndef __GNUC__
    ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);
#else
    ClrVram((Uint8 *)SCL_VDP2_VRAM_A0);
#endif

    switch(err){
        case    0:
            sprintf((char *)buff,"NO ERROR");
            break;
        case    SIZE_ERR:
            sprintf((char *)buff,"READ BUFFER OVER");
            break;
        case    BUP_NON:
            sprintf((char *)buff,"BUP_NON");
            break;
        case    BUP_UNFORMAT:
            sprintf((char *)buff,"BUP_UNFORMAT");
            break;
        case    BUP_WRITE_PROTECT:
            sprintf((char *)buff,"BUP_WRITE_PROTECT");
            break;
        case    BUP_NOT_ENOUGH_MEMORY:
            sprintf((char *)buff,"BUP_NOT_ENOUGH_MEMORY");
            break;
        case    BUP_NOT_FOUND:
            sprintf((char *)buff,"BUP_NOT_FOUND");
            break;
        case    BUP_FOUND:
            sprintf((char *)buff,"BUP_FOUND");
            break;
        case    BUP_NO_MATCH:
            sprintf((char *)buff,"BUP_NO_MATCH");
            break;
        case    BUP_BROKEN:
            sprintf((char *)buff,"BUP_BROKEN");
            break;
        default:
/* #if !(__GNUC__) */
#ifndef __GNUC__
            sprintf((char *)buff,"ErrNO:%08x",err);
#else
            sprintf((char *)buff,"ErrNO:%08x",(Uint16)err);
#endif
            break;
    }
/* #if !(__GNUC__) */
#ifndef __GNUC__
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,50,10,2,0);
#else
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,50,10,2,0);
#endif
    if(wait)    PadInputWait();
}


/****************************/
/* 書き込み中断防止特別処理 */
/****************************/
void    BackUpInit(BupConfig cntb[3])
{
/* #if !(__GNUC__) */
    PER_SMPC_RES_DIS();/* リセットボタン無効 */
#ifndef __GNUC__
	    BUP_Init(BackUpLibWork,BackUpRamWork,cntb);
#else
	    BUP_Init(BackUpLibWork,BackUpRamWork,cntb);
#endif
    PER_SMPC_RES_ENA();/* リセットボタン有効 */
}

void    DispDate(char *comment,Uint16 x,Uint16 y)
{
    Uint8   *time;
    Uint8   buff[32];

    time = PER_GET_TIM();
#if 0 /* 95/10/03 k.kawai */
    sprintf(buff,"%s%02x%02x-%02x-%02x %02x:%02x:%02x",comment,
#else
    sprintf((char *)buff,"%s%02x%02x-%02x-%02x %02x:%02x:%02x",comment,
#endif
            time[6],time[5],time[4] & 0x0F,time[3],
            time[2],time[1],time[0]);
/* #if !(__GNUC__) */
#ifndef __GNUC__
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,x,y,2,0);
#else
    FNT_Print256((Uint8 *)SCL_VDP2_VRAM_A0,buff,x,y,2,0);
#endif
}

#if 0
Uint32 datasize_chk( Uint8 *fname )
{
	BupDir DirTb_tmp;

	PER_SMPC_RES_DIS();   /* リセットボタン無効 */
		BUP_Dir(device ,fname ,1 ,&DirTb_tmp);
	PER_SMPC_RES_ENA();   /* リセットボタン有効 */
	if( BUFFER_SIZE < DirTb_tmp.datasize ){
		return 0;  /* エラー */
	}else{
		return 1;  /* OK */
	}
}
#endif

/* #if !(__GNUC__) */
#ifndef __GNUC__
Sint32  BackUpWrite(Uint32 device,BupDir *dir,Uint8 *data,Uint8 sw)
#else
Sint32  BackUpWrite(Uint32 device,BupDir *dir,Uint8 *data,Uint8 sw)
#endif
{
    Sint32  ret;
    Uint8   *time;
    BupDate date;

    /* 日付データが０だったら現在の日付を入力する */
    if(!dir->date){
        time = PER_GET_TIM();
        date.year = (Uint8 )( (Uint16 )(time[6]>>4) * 1000
                + (Uint16 )(time[6] & 0x0F) * 100
                + (Uint16 )(time[5]>>4)     * 10
                + (Uint16 )(time[5] & 0x0F) - 1980);
        date.month = time[4] & 0x0F;
        date.day   = (time[3]>>4)*10 + (time[3] & 0x0F);
        date.time  = (time[2]>>4)*10 + (time[2] & 0x0F);
        date.min   = (time[1]>>4)*10 + (time[1] & 0x0F);
        dir->date = BUP_SetDate(&date);
    }

    PER_SMPC_RES_DIS();/* リセットボタン無効 */
        ret = BUP_Write(device,dir,data,sw);
    PER_SMPC_RES_ENA();/* リセットボタン有効 */

    return(ret);
}

Sint32  BackUpDelete(Uint32 device,Uint8 *filename)
{
    Sint32  ret;

    PER_SMPC_RES_DIS();/* リセットボタン無効 */
        ret = BUP_Delete(device,filename);
    PER_SMPC_RES_ENA();/* リセットボタン有効 */

    return(ret);
}

Sint32  BackUpFormat(Uint32 device)
{
    Sint32  ret;

    PER_SMPC_RES_DIS();/* リセットボタン無効 */
        ret = BUP_Format(device);
    PER_SMPC_RES_ENA();/* リセットボタン有効 */

    return(ret);
}



#if 1
/*--------------------------------------------------*/
/*  割り込み要因のレジスターのビットが1か？         */
/*--------------------------------------------------*/
Bool isHirqOn(Sint32 flag)
{
    return((CDC_GetHirqReq() & flag) != 0);
}

/*--------------------------------------------------*/
/*  CDのオープンチェック　                          */
/*--------------------------------------------------*/
void CDOpenCheck(void)
{
    CdcStat stat;

    if(isHirqOn(CDC_HIRQ_DCHG)){
        SYS_EXECDMP();
    }

    CDC_GetPeriStat(&stat);

}
#endif
