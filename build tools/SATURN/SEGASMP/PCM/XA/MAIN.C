/******************************************************************************
 *	
 *
 *	Copyright (c) 1994,1995 SEGA
 *
 * Library	:PCM/ADPCM LIB
 * Module 	:sample program of CDROM-XA AUDIO
 * File		:main.c
 * Date		:1995-03-31
 * Version	:1.20
 * Auther	:Y.T
 * 更新履歴 :1997-02-19 by A.H(SOJ)
 *          :1997-03-04 by A.H(SOJ)
 *              gfs,stmのワークを４バイトバウンダリに変更
 *              ディレクトリチェンジの位置を、STM_Init()の後に変更
 *          :1997-09-03 by A.H(SOJ)
 *              サウンドドライバを、外部リンケージで参照。
 *
 * Comment	:
 *		CD-ROM XA Audio
 *		
 ****************************************************************************/
/* DISC type */
#define CD_ROM_XA_AUDIO

/* 再生するファイル名 */
#define FILENAME	"SAMPLE1.ADP"
#if 0
#define FILENAME	"END_SNG1.ADP"
#endif


/* サブディレクトリ名 */
/*    SUB_DIR_NAME  が定義されていると、その名前のサブディレクトリに
      移動します。  */
#if 0
#define SUB_DIR_NAME  "ADPCM"
#define SUB_DIR_NAME  "PHOTO_CD"
#endif


/* PLAY-MAX */
#define	FILE_NUM		1

/* CDROM-XA DATA */
	static char *filename[FILE_NUM] = { FILENAME };

/* USE STM_ExecServer ALLWAYS */
#if 1
#define ALLWAYS_STM_EXECSERVER
#endif


/* USE STATIC */
#define USE_STATIC    0



#if ( USE_STATIC == 1)
#define STATIC static
#else
#define STATIC
#endif

/*------------------------- <INCLUDE> -------------------------*/
#include <stdio.h>
#include <machine.h>
#include <string.h>
#define _SH
#include "sega_xpt.h"
#include "sega_sys.h"
#include "sega_def.h"
#include "sega_mth.h"
#include "sega_scl.h" 
#include "sega_int.h"
#define  _SPR2_
#include "sega_spr.h"
#include "sega_dma.h"
#include "sega_cdc.h"
#include "sega_gfs.h"
#include "sega_stm.h"
#include "sega_snd.h"
#include "sega_pcm.h"

#if 0
                         /*  use sddrv.tsk ver2.10 */
#include "sddrvs.dat"    /*  "SATURN\SGL\INC\SDDRV.DAT"  */

int sound_map[] = {0x0001,0x0000,0x0001,0x4000,
					0x0102,0x4000,0x0001,0x4000,
					0xffff };
#endif

#if 0
	#define SMPPCMD_VblIn()				
	#define SMPPCMD_Init(a)				
	#define SMPPCMD_MON_Reset()			
	#define SMPPCMD_PCM_Task			PCM_Task
	#define VTV_Printf(a)				
	#define VTV_PRINTF(a)				
	#define _VTV_Printf(a)				
	#define _VTV_PRINTF(a)				
#endif


/*--------------------------- <FUNCTIONS> ---------------------------*/

STATIC void smpVblIn(void);
STATIC void smpVblOut(void);


/*------------------------- <MACRO> -------------------------*/

/* VDP1 VRAM-ADDR */
#define ADDR_VDP1 				(0x25C00000)

/* VRAM TRANS ADDR(DIST) */
#define ADDR_VRAM_PCM 			(0x25C08000)

/* WAVE-RAM */
#define	PCM_ADDR	((void*)0x25a20000)     /* ADDR */
#define	PCM_SIZE	(4096L*2)				/* SAMPLES  2.. */

/* SECTOR SIZE */
#ifdef CD_ROM_XA_AUDIO
	#define	SECTOR_SIZE		(2324L)
#else
	#define	SECTOR_SIZE		(2048L)
#endif

/* RING BUFFER SIZE */
#define	RING_BUF_SIZE	(SECTOR_SIZE * 100)		/* 10.. */

/* WORKSIZE FOR PAUSE */
#define PWORK_SIZE		(2 * 4096)

/* TV SIZE H-V */
#define DISP_XSIZE		(320)
#define DISP_YSIZE		(224)

/* VOLUME */
#define LEVEL_MIN	(0)
#define LEVEL_MAX	(7)

/* SOUND-PAN */
#define PAN_MIN		(0)					/* LEFT-MAX    RIGHT-MIN */
#define PAN_MAX		(31)				/* LEFT-MIN    RIGHT-MAX */
#define PAN_CENTER	((PAN_MAX + 1) / 2)	/* LEFT-MAX    RIGHT-MAX */

/* V-BLANK-SKIPS(CALL STM_ExecServer */
#define VBL_RATE_STM_EXEC_SERVER		(1)

/* MAX-TRANS(SECTOR) */
#define SMP_LOAD_NUM			(10)

/* LIMIT(1TASK) [sample/1ch] */
/* #define SMP_1TASK_SAMPLE		(512) */
/* #define SMP_1TASK_SAMPLE		(600) */	/* LIMIT(CD-ROM XA mode-B)  */
#define SMP_1TASK_SAMPLE		(1024) 
/* #define SMP_1TASK_SAMPLE		(2048) */

/* TRANS-MODE (CD-BLOCK -> RING-BUFFER) */
/*		PCM_TRMODE_CPU or PCM_TRMODE_SDMA or PCM_TRMODE_SCU		*/
#define SMP_TR_MODE_CD			(PCM_TRMODE_SDMA)

/*----------------------- <GLOBAL> -----------------------*/

/* WORK BUFFER */
STATIC PcmWork g_movie_work[FILE_NUM];

/* RING BUFFER */
STATIC Uint32 g_movie_buf[FILE_NUM][RING_BUF_SIZE / sizeof(Uint32)];

/* VOLUME */
STATIC Sint32 g_level = LEVEL_MAX;

/* PAN  (0..31) */
STATIC Sint32 g_pan = PAN_CENTER;

/* PAN-TABLE */
STATIC Sint32 g_pan_tbl[PAN_MAX + 1] = {
	0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 
	0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

/* COUNTER(VblIn) */
Sint32 cnt_vbl_in = 0;

/* TIME(CALL STM_ExecServer) [vbl/vbl_rate] */
Sint32 call_stm_exec_server;

/* STM ERR OBJ */
void *obj_err_stm;
/*---------------------------- <ERROR FUNCTIONS> ----------------------------*/

void errGfsFunc(void *obj, Sint32 ec)
{
#if 0
	SYS_EXECDMP();
#else
	while(1);
#endif
}

void errStmFunc(void *obj, Sint32 ec)
{
#if 0
	SYS_EXECDMP();
#else
	while(1);
#endif
}

void errPcmFunc(void *obj, Sint32 ec)
{
#if 0
	SYS_EXECDMP();
#else
	while(1);
#endif
}


/*====================== V-BLANK ===========================*/

STATIC void	vblInit( void ){
	
	/* SET INT */
	INT_ChgMsk_NR(INT_MSK_NULL,INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);
	INT_SetScuFunc(INT_SCU_VBLK_IN,smpVblIn);
	INT_SetScuFunc(INT_SCU_VBLK_OUT,smpVblOut);
	INT_ChgMsk_NR(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT,INT_MSK_NULL);
}

STATIC void	smpVblIn( void ){
	cnt_vbl_in++;
	
	/* PCM(VBLANK-TASK) */
	PCM_VblIn();
}

STATIC void	smpVblOut( void ){
}

/*====================== SOUND ===========================*/
#if 1   /* サウンドドライバを 外部リンケージで参照 */
extern Uint32 sddrvsize ;
extern Uint32 bootsndsize ;
extern char sddrvstsk[];
extern Uint8 bootsnd[];
#endif

STATIC void sndInit(void)
{
	SndIniDt 	snd_init;
	Uint32	errChk = 0;
	
#if 0  /*  use sddrv.tsk ver2.10  97-02-14 a.h */
	if (fileLoad("SDDRVS.TSK", (void *)sddrvs_tsk, SDDRVS_TSK_SIZE)) {
		SYS_EXECDMP();
	}
	if (fileLoad("BOOTSND.MAP", (void *)bootsnd_map, BOOTSND_MAP_SIZE)) {
		SYS_EXECDMP();
	}
	SND_INI_PRG_ADR(snd_init) 	= (Uint16 *)sddrvs_tsk;
	SND_INI_PRG_SZ(snd_init) 	= (Uint16 )SDDRVS_TSK_SIZE;
	SND_INI_ARA_ADR(snd_init) 	= (Uint16 *)bootsnd_map;
	SND_INI_ARA_SZ(snd_init) 	= (Uint16)BOOTSND_MAP_SIZE;
#else  /*  use sddrv.dat ver2.20  97-09-03 A.H(SOJ) */
	SND_INI_PRG_ADR(snd_init) 	= (Uint16 *)sddrvstsk;
	SND_INI_PRG_SZ(snd_init) 	= (Uint16 )sddrvsize;
	SND_INI_ARA_ADR(snd_init) 	= (Uint16 *)bootsnd;
	SND_INI_ARA_SZ(snd_init) 	= (Uint16 )bootsndsize;
#endif
	SND_Init(&snd_init);
    
    while(SND_ChgMap(0));
      if (errChk++ > 512) SYS_EXECDMP();
}


/*====================== FILE ===========================*/

/* ルートディレクトリにあるファイルの最大数 */
#define MAX_DIR_ROOT		300
/* サブディレクトリにあるファイルの最大数 */
#define MAX_DIR_SUB		40

/* 同時に開くファイルの最大数 */
#define OPEN_MAX	1

/* 同時に開くストリームグループの最大数 */
#define GRP_MAX		1

/* STREAM GROUP  */
STATIC StmGrpHn grp_hd;

#ifdef SUB_DIR_NAME
/* サブディレクトリ情報 fid */
Sint32 dir_sub_fid;
#endif

/* ディレクトリ情報管理領域 */
static GfsDirTbl	dir_tbl_root;
#ifdef SUB_DIR_NAME
static GfsDirTbl	dir_tbl_sub;
#endif

/* ファイル名を含んだ情報 */
static GfsDirName dir_name_root[MAX_DIR_ROOT];
#ifdef SUB_DIR_NAME
static GfsDirName dir_name_sub[MAX_DIR_SUB];
#endif

/* GFS,STM-WORK */
STATIC Uint32 g_gfs_work[ ((GFS_WORK_SIZE(OPEN_MAX) + 3) / 4)];
STATIC Uint32 g_stm_work[ ((STM_WORK_SIZE(GRP_MAX, OPEN_MAX) + 3) / 4)];


STATIC void fileInit(void)
{
	Sint32 file_num;

	/* INIT GFS */
  GFS_DIRTBL_TYPE(&dir_tbl_root) = GFS_DIR_NAME;
  GFS_DIRTBL_DIRNAME(&dir_tbl_root) = dir_name_root;
  GFS_DIRTBL_NDIR(&dir_tbl_root) = MAX_DIR_ROOT;

#ifdef SUB_DIR_NAME
  GFS_DIRTBL_TYPE(&dir_tbl_sub) = GFS_DIR_NAME;
  GFS_DIRTBL_DIRNAME(&dir_tbl_sub) = dir_name_sub;
  GFS_DIRTBL_NDIR(&dir_tbl_sub) = MAX_DIR_SUB;
#endif

	do {    /*  本来は、エラー処理を行う事 */
	    file_num = GFS_Init(OPEN_MAX, g_gfs_work, &dir_tbl_root);
	}while(file_num < 0);

#ifdef SUB_DIR_NAME
	dir_sub_fid = GFS_NameToId( SUB_DIR_NAME );
	GFS_LoadDir( dir_sub_fid, &dir_tbl_sub);
	GFS_SetDir(&dir_tbl_sub);
#endif

	/* SET GFS-ERROR-TRAP */
	GFS_SetErrFunc(errGfsFunc, NULL);
}

STATIC void stmInit(void)
{
	/* INIT STREAM-SYS */
	STM_Init(GRP_MAX, OPEN_MAX, g_stm_work);

	/* SET STREAM-ERROR-TRAP */
	STM_SetErrFunc(errStmFunc, obj_err_stm);

	/* OPEN STREAM GROUP */
	do{
		grp_hd = STM_OpenGrp();
	}while(grp_hd == NULL);

	STM_SetLoop(grp_hd, STM_LOOP_DFL, STM_LOOP_ENDLESS);
	STM_SetExecGrp(grp_hd);
}

STATIC StmHn stmOpen(char *fname)
{
    Sint32 fid;
	StmKey key;
	Sint32 fad;

    /* FILENAME TO FID */
    fid = GFS_NameToId((Sint8 *)fname);
#if 0
	STM_KEY_FN(&key) = STM_KEY_CN(&key) = STM_KEY_SMMSK(&key) = 
		STM_KEY_SMVAL(&key) = STM_KEY_CIMSK(&key) = STM_KEY_CIVAL(&key) =
		STM_KEY_NONE;
#else
	STM_KEY_FN(&key) = STM_KEY_CIMSK(&key) = STM_KEY_CIVAL(&key) =
		STM_KEY_NONE;
#endif

	STM_KEY_SMMSK(&key) = STM_KEY_SMVAL(&key) = 0x04;/* Audio Sector */
	STM_KEY_CN(&key) = 0;	/* channel No. */
	/* Stream system (Ver1.13) can't get correct file size, that file was 
	 * interleaveed in Photo CD disc. 
	 */

	/* GET FAD with fid&OFFSET(==0) */
	fad = GFS_GetFad(fid, 0);

	return STM_OpenFid(grp_hd, fid, &key, STM_LOOP_NOREAD);
}


Sint32 smpIsInFrage(Sint32 fad, StmHn stm)
{
	Sint32		fid;
	StmFrange	frange;
	Sint32		bn;
	StmKey		stmkey;

	/* GET STREAM-INFO	*/
	STM_GetInfo(stm, &fid, &frange, &bn, &stmkey);

	if (fad >= STM_FRANGE_SFAD(&frange) &&
		fad < STM_FRANGE_SFAD(&frange) + STM_FRANGE_FASNUM(&frange)) {
		return 1;
	} else {
		return 0;
	}
}

/* EXECUTE STREAM-SERVER */
STATIC void smpStmTask(StmHn stm[])
{
#ifndef ALLWAYS_STM_EXECSERVER
	Sint32 	 flag_stm_exec_trans;
#endif
	if (call_stm_exec_server < cnt_vbl_in / VBL_RATE_STM_EXEC_SERVER) {

#ifdef ALLWAYS_STM_EXECSERVER
		STM_ExecServer();
#else
		if (call_stm_exec_server < 0) {
			STM_ExecServer();
		} else {

			/* only needs Calling STM_ExecServer */
			flag_stm_exec_trans = 0;
			if (STM_GetNumCdbuf(stm[0]) > 0) {
				STM_ExecTrans(stm[0]);
				flag_stm_exec_trans = 1;
			}
			if (STM_GetNumCdbuf(stm[1]) > 0) {
				STM_ExecTrans(stm[1]);
				flag_stm_exec_trans = 1;
			}
			if (flag_stm_exec_trans == 0) {
				STM_ExecServer();
			}
		}
#endif

		if (call_stm_exec_server < 0) {
			call_stm_exec_server++;
		} else {
			call_stm_exec_server = cnt_vbl_in / VBL_RATE_STM_EXEC_SERVER;
		}
	}
}

/* INIT PCM-LIB */
void smpPcmInit(void)
{
	/* INIT PCM-LIB */
	PCM_Init();

	/* DEFINE  USE-ADPCM (LINKING ADPCM-DECORD-LIB) */
	PCM_DeclareUseAdpcm();

	/* SET  PCM-ERROR-TRAP */
	PCM_SetErrFunc(errPcmFunc, NULL);
}


STATIC PcmHn createHandle(StmHn stm, int file_no)
{
	PcmCreatePara	para;
	PcmHn			pcm;

	/* CREATE PCM-HANDLE */
	PCM_PARA_WORK(&para) = (struct PcmWork *)&g_movie_work[file_no];
	PCM_PARA_RING_ADDR(&para) = (Sint8 *)g_movie_buf[file_no];
	PCM_PARA_RING_SIZE(&para) = RING_BUF_SIZE;
	PCM_PARA_PCM_ADDR(&para) = PCM_ADDR;
	PCM_PARA_PCM_SIZE(&para) = PCM_SIZE;
	pcm = PCM_CreateStmHandle(&para, stm);
	if (pcm == NULL) {
		return NULL;
	}

	if (file_no == 0) {
		/* MOVE PICKUP */
		STM_MovePickup(stm, 0);
	}

	PCM_SetVolume(pcm, g_level);
	PCM_SetPan(pcm, g_pan_tbl[g_pan]);
	return pcm;
}

STATIC void pcmTask(PcmHn pcm[], StmHn stm[])
{
	int		i;

	/* EXEC STREAM TASK */
	smpStmTask(stm);

	/* PCM TASK */
	PCM_Task(pcm[0]);

	for (i = 1; i < FILE_NUM; i++) {
		/* PCM TASK */
		PCM_Task(pcm[i]);
	}
}

#ifdef SMPPCM_DEBUG
	#define SMP_PAD_MODE_NUM		(4)
#else
	#define SMP_PAD_MODE_NUM		(1)
#endif
Sint32 smp_pad_mode = 0;


void main(void)
{
	PcmInfo 	info;
	PcmHn		pcm[FILE_NUM];
	StmHn		stm[FILE_NUM];
	Uint32		restart;
	int			i;

	/* INIT PCM-LIB */
	smpPcmInit();

#ifdef USE_VDP
	/* INIT VDP1 */
	dispInit();
#else
	/* INIT V-BLANK */
	vblInit();
#endif


	/* INIT FILE-SYSTEM */
	fileInit();

	/* INIT SOUNDO-SYSTEM */
	sndInit();

	/* INIT STREAM-SYSTEM */
	stmInit();

	restart = 1;

	for (;;) {
		if (restart) {

			/* OPEN STREAM */
			for (i = 0; i < FILE_NUM; i++) {
				if ((stm[i] = stmOpen(filename[i])) == NULL) {
					return;
				}
				/* RESET BUFFER */
				STM_ResetTrBuf(stm[i]);

				/* createHandle */
				if ((pcm[i] = createHandle(stm[i], i)) == NULL) {
					return;
				}
#ifdef CD_ROM_XA_AUDIO
				/* INFO(CD-ROM XA Audio sector ) */
				PCM_INFO_FILE_TYPE(&info) = PCM_FILE_TYPE_NO_HEADER;
				PCM_INFO_DATA_TYPE(&info) = PCM_DATA_TYPE_ADPCM_SCT;
				PCM_SetInfo(pcm[i], &info);
#endif
				PCM_SetLoadNum(pcm[i], SMP_LOAD_NUM);
				PCM_SetTrModeCd(pcm[i], SMP_TR_MODE_CD);
				PCM_Set1TaskSample(pcm[i], SMP_1TASK_SAMPLE);
			}

			/* START */
			PCM_Start(pcm[0]);

			/* 1st CALL (STM_ExecServer) */
			call_stm_exec_server = -1;

			restart = 0;

		}

		/* PLAY TASK */
		pcmTask(pcm, stm);

		/* CHECK EOF */
		if (PCM_GetPlayStatus(pcm[FILE_NUM-1]) == PCM_STAT_PLAY_END) {

			for (i = 0; i < FILE_NUM; i++) {
				/* DEL StmHandle */
				PCM_DestroyStmHandle(pcm[i]);

				/* CLOSE */
				STM_Close(stm[i]);
			}

			restart = 1;
		}
	}
}
