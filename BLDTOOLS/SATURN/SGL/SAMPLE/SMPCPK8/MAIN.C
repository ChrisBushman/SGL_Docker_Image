/*******************************************************************
 *
 *                       Cinepak for SATURN Player 
 *                                by SOJ
 *                         usage sample program
 *
 *               マルチムービ再生のサンプル
 *               SAMPLE0.CPK をまずメモリに読み込み,
 *               SAMPLE1.CPK と SAMPLE2.CPK と SAMPLE3.CPK はＣＤからデータを
 *               読み込みならが４つのムービを同時に再生する。
 *               [注意]
 *               ４つのムービは同じコマ数にすること。
 *               ４つのムービは同じ画像サイズ(120, 80)にすること。
 *               SAMPLE1.CPK と SAMPLE2.CPK と SAMPLE3.CPK はボリュームを０に
 *               している。
 *               SAMPLE0.CPK は １ＭＢ以下にすること。
 *               SAMPLE1.CPK と SAMPLE2.CPK と SAMPLE3.CPK はデータレートを
 *               100KB/S 以下にすること。
 *
 *                      Copyright(c) 1994,1995 SEGA
 *
 *                gcc version cygnus-2.7-95q3-SOA-951018
 *                ld version 2.5-95q3 (with BFD 2.5-95q3)
 *                  SGL2.0A for 3rd Party Version
 *        GNU Make version 3.71, by Richard Stallman and Roland McGrath.
 *
 *******************************************************************/
/*--------------------------------------------------------------------------*/
/*								 Include									*/
/*--------------------------------------------------------------------------*/
#include	"sgl.h"
#include	"sgl_cd.h"
#include	"sega_cpk.h"
#include	"sega_snd.h"
/*--------------------------------------------------------------------------*/
/*								 Prototype									*/
/*--------------------------------------------------------------------------*/
CpkHn createMemMovie(void) ;
void stmClose(StmHn fp) ;
StmHn stmOpen(char *fname) ;
void setSprite(void) ;
CpkHn createMovie(StmHn stm, Sint32 num) ;
void cpkInit(void) ;
void fileInit(void) ;
GfsHn fileOpen(char *fname) ;
void fileClose(GfsHn fp) ;
void sndInit(void) ;
void errGfsFunc(void *obj, Sint32 ec) ;
void errCpkFunc(void *obj, Sint32 ec) ;
void smpVblIn(void) ;
Sint32 fileRead(char *filename, void *addr) ;
/*--------------------------------------------------------------------------*/
/*								 Define										*/
/*--------------------------------------------------------------------------*/
#define	FILE_NUM			4

/* メモリにロードするアドレス */
#define	 LOAD_MEM_ADDR 		0x200000

/* ロードサイズ */
#define	LOAD_MEM_SIZE  		0x100000

#define	VDP1_MOVIE_1		(0x10000)
#define	VDP1_MOVIE_2		(0x18000)
#define	VDP1_MOVIE_3		(0x20000)
#define	VDP1_MOVIE_4		(0x28000)

#define	RING_BUF_SIZ		(1024L*100)

/* ウェーブＲＡＭの転送アドレスとサンプル数 */
#define	PCM_ADDR			(0x25a0c000)
#define	PCM_SIZE			(4096L*14)

#define	WIDTH_H				320
#define	WIDTH_V				224

#define	MOVIE_X				120
#define	MOVIE_Y				80

/*--------------------------------------------------------------------------*/
/*								 Extern										*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*								Structure									*/
/*--------------------------------------------------------------------------*/
SPRITE film_spr[FILE_NUM] ;

Uint32 work_ram_buf[FILE_NUM][MOVIE_X * MOVIE_Y / 2];
Uint32 cp_size;
Uint32 g_movie_work[FILE_NUM][CPK_15WORK_DSIZE];	/* Work Buffer */
Uint32 g_movie_buf[FILE_NUM][RING_BUF_SIZ / sizeof(Uint32)];	/* Ring Buffer */
char *g_filename[]={"SAMPLE01.CPK","SAMPLE02.CPK","SAMPLE03.CPK","SAMPLE00.CPK"};

void *g_pcm_addr[] = {	(void *)PCM_ADDR, 
			  (void *)(PCM_ADDR+PCM_SIZE*2),
			  (void *)(PCM_ADDR+PCM_SIZE*4),
			  (void *)(PCM_ADDR+PCM_SIZE*6)};
/*--------------------------------------------------------------------------*/
/*								Main Routin									*/
/*--------------------------------------------------------------------------*/
void ss_main(void)
{
  CpkHn 			cpk[FILE_NUM];
  StmHn			stm[FILE_NUM];
  Uint32			restart;
  Sint32			i ;
  Sint32			frame_chg_flag ;
  Sint32			start_flag;
  Sint32			all_stop;


  slInitSystem(TV_320x224,NULL,1);

  slIntFunction((void*)smpVblIn) ;

  fileInit() ;

  sndInit();

  cpkInit();

  /* メモリ読み込み */
  if (fileRead(g_filename[FILE_NUM-1], (void *)LOAD_MEM_ADDR) <= 0) {
    slPrint("Error fileRead" , slLocate(9,3));
    while( -1 ) slSynch();
  }

  setSprite() ;

  restart = 1;

  while (1) {
    if (restart) {

      /* メモリ再生ムービの生成 */
      if ((cpk[FILE_NUM-1] = createMemMovie()) == NULL) {
	slPrint("Error createMemMovie" , slLocate(9,3));
	while( -1 ) slSynch();
      }

      for (i=0; i<FILE_NUM-1; i++) {
	/* ストリームオープン */
	if ((stm[i] = stmOpen(g_filename[i])) == NULL) {
	  slPrint("Error stmOpen" , slLocate(9,3));
	  while( -1 ) slSynch();
	}

	/* ムービハンドル生成 */
	if ((cpk[i] = createMovie(stm[i], i)) == NULL) {
	  slPrint("Error createMovie" , slLocate(9,3));
	  while( -1 ) slSynch();
	}
	/* ムービ開始 */
	CPK_Start(cpk[i]);

      }
      start_flag = FALSE ;
      /* ムービの再生処理 */
      while(start_flag == FALSE) {
	for (i = 0; i <FILE_NUM-1; i++) {
	  CPK_Task(cpk[i]);
	  if (CPK_GetPlayStatus(cpk[0]) == CPK_STAT_PLAY_TIME &&
	      CPK_GetPlayStatus(cpk[1]) == CPK_STAT_PLAY_TIME &&
	      CPK_GetPlayStatus(cpk[2]) == CPK_STAT_PLAY_TIME ) {
	    start_flag = TRUE;
	  }
	}
      }
      CPK_Start(cpk[FILE_NUM-1]);
      while(1) {
	CPK_Task(cpk[FILE_NUM-1]);
	if (CPK_GetPlayStatus(cpk[FILE_NUM-1]) == CPK_STAT_PLAY_TIME) {
	  break;
	}
      }

      restart = 0;
    }

    for (i=0; i<FILE_NUM; i++) {
      CPK_Task(cpk[i]) ;
    }

    frame_chg_flag = TRUE ;
    for (i=0; i<FILE_NUM; i++) {
      /* 画面表示要求のチェック */
      if (CPK_IsDispTime(cpk[i]) != TRUE) {
	frame_chg_flag = FALSE ;
      }
    }
    if (frame_chg_flag == TRUE) {

      DMA_ScuMemCopy(	(void*)(0x25c00000+VDP1_MOVIE_1),
		     (void *) work_ram_buf[0],
		     cp_size) ;
      DMA_ScuMemCopy(	(void*)(0x25c00000+VDP1_MOVIE_2),
		     (void *) work_ram_buf[1],
		     cp_size) ;
      DMA_ScuMemCopy(	(void*)(0x25c00000+VDP1_MOVIE_3),
		     (void *) work_ram_buf[2],
		     cp_size) ;
      DMA_ScuMemCopy(	(void*)(0x25c00000+VDP1_MOVIE_4),
		     (void *) work_ram_buf[3],
		     cp_size) ;

      /* 表示完了の通知 */
      for (i=0; i<FILE_NUM; i++) {
	CPK_CompleteDisp(cpk[i]);
      }
      slSetSprite(&film_spr[0], toFIXED(170.0)) ;
      slSetSprite(&film_spr[1], toFIXED(170.0)) ;
      slSetSprite(&film_spr[2], toFIXED(170.0)) ;
      slSetSprite(&film_spr[3], toFIXED(170.0)) ;
      slSynch() ;
    }

    /* ムービの終了判定 */
    all_stop = TRUE;
    for (i = 0; i < FILE_NUM; i++) {
      if (CPK_GetPlayStatus(cpk[i]) != CPK_STAT_PLAY_END) {
	all_stop = FALSE;
      }
    }

    if (all_stop) {
      restart = 1;

      for (i = 0; i <FILE_NUM-1; i++) {
	/* ムービの放棄 */
	CPK_DestroyStmMovie(cpk[i]);

	/* ストリームのクローズ*/
	stmClose(stm[i]);
      }

      if (CPK_GetPlayStatus(cpk[FILE_NUM-1]) == CPK_STAT_PLAY_END) {
	CPK_DestroyMemMovie(cpk[FILE_NUM-1]);
      }

    }

  }
}

/*--------------------------------------------------------------------------*/
/*								GFS Routin									*/
/*--------------------------------------------------------------------------*/

/* ルートディレクトリにあるファイルの最大数 */
#define MAX_DIR		100

/* 同時に開くファイルの最大数 */
#define OPEN_MAX	5

/* ストリームグループのＩＤ */
StmGrpHn grp_hd;

/* ディレクトリ情報管理領域 */
GfsDirTbl	dir_tbl;

/* ファイル名を含んだ情報 */
GfsDirName dir_name[MAX_DIR];

/* ＧＦＳの作業領域 */
Uint8 gfs_work[GFS_WORK_SIZE(OPEN_MAX)];
     
/* ＳＴＭの作業領域 */
Uint8 stm_work[STM_WORK_SIZE(12, 24)];
     
     
void errStmFunc(void *obj, Sint32 ec)
{
  slPrint("Error Stm Func" , slLocate(9,3));
  slPrintHex( ec, slLocate( 26, 3 ) );
  while( -1 ) slSynch();
  /* エラー処理 */
}

void errGfsFunc(void *obj, Sint32 ec)
{
  slPrint("Error Gfs Func" , slLocate(9,3));
  slPrintHex( ec, slLocate( 26, 3 ) );
  while( -1 ) slSynch();
  /* エラー処理 */
}

void fileInit(void)
{
  Sint32 file_num;

  /* GFSの初期化 */
  GFS_DIRTBL_TYPE(&dir_tbl) = GFS_DIR_NAME;
  GFS_DIRTBL_DIRNAME(&dir_tbl) = dir_name;
  GFS_DIRTBL_NDIR(&dir_tbl) = MAX_DIR;

  /* ファイルシステムの初期化 */
  file_num = GFS_Init(OPEN_MAX, gfs_work, &dir_tbl);
  if (file_num < 0) {
    slPrint( "Error in GFS Init", slLocate( 9, 3 ) );
    while( -1 ) slSynch();
  }

  /* ストリームシステムの初期化 */
  STM_Init(12, 24, stm_work);

  /* エラー関数の設定 */
  GFS_SetErrFunc(errGfsFunc, NULL);
  STM_SetErrFunc(errStmFunc, NULL);

  /* ストリームグループのオープン */
  grp_hd = STM_OpenGrp();
  if (grp_hd == NULL) {
    slPrint( "Error open stream group", slLocate( 9, 3 ) );
    while( -1 ) slSynch();
  }
  STM_SetLoop(grp_hd, STM_LOOP_DFL, STM_LOOP_ENDLESS);
  STM_SetExecGrp(grp_hd);

}

StmHn stmOpen(char *fname)
{
  Sint32 fid;
  StmKey key;

  /* ファイル名からファイル識別子を求める */
  fid = GFS_NameToId(fname);
  STM_KEY_FN(&key) = STM_KEY_CN(&key) = STM_KEY_SMMSK(&key) = 
    STM_KEY_SMVAL(&key) = STM_KEY_CIMSK(&key) = STM_KEY_CIVAL(&key) =
      STM_KEY_NONE;
  return STM_OpenFid(grp_hd, fid, &key, STM_LOOP_NOREAD);
}

void stmClose(StmHn fp)
{
  STM_Close(fp);
}



GfsHn fileOpen(char *fname)
{
  Sint32 fid;

  /* ファイル名からファイル識別子を求める */
  fid = GFS_NameToId(fname);
  return GFS_Open(fid);
}

void fileClose(GfsHn fp)
{
  GFS_Close(fp);
}

/*--------------------------------------------------------------------------*/
/*								 VBL Routin									*/
/*--------------------------------------------------------------------------*/
void smpVblIn(void)
{
  /* Ｃｉｎｅｐａｋの VblIn ルーチンをコール */
  CPK_VblIn();
}

/*--------------------------------------------------------------------------*/
/*								 Cinepak Routin								*/
/*--------------------------------------------------------------------------*/
void errCpkFunc(void *obj, Sint32 ec)
{
  slPrint("Error Cpk Func" , slLocate(9,3));
  slPrintHex( ec, slLocate( 26, 3 ) );
  while( -1 ) slSynch();
  /* エラー処理 */
}
/* シネパックの初期化 */
void cpkInit(void)
{
  /* シネパックの初期化 */
  CPK_Init();

  /* エラー関数の設定 */
  CPK_SetErrFunc(errCpkFunc, NULL);

}
/* ムービハンドル生成 */
CpkHn createMovie(StmHn stm, Sint32 num)
{
  CpkCreatePara	para;
  CpkHeader		*header;
  CpkHn			cpk;

  /* ムービハンドル生成 */
  CPK_PARA_WORK_ADDR(&para) = g_movie_work[num];
  CPK_PARA_WORK_SIZE(&para) = CPK_15WORK_BSIZE;
  CPK_PARA_BUF_ADDR(&para) = g_movie_buf[num];
  CPK_PARA_BUF_SIZE(&para) = RING_BUF_SIZ;
  CPK_PARA_PCM_ADDR(&para) = g_pcm_addr[num];
  CPK_PARA_PCM_SIZE(&para) = PCM_SIZE;
  cpk = CPK_CreateStmMovie(&para, stm);
  if (cpk == NULL) {
    slPrint( "Error Open handle", slLocate( 9, 3 ) );
    while( -1 ) slSynch();
  }

  /* 表示色数を３２０００色に設定 */
  CPK_SetColor(cpk, CPK_COLOR_15BIT);

  cp_size = (Uint32)(MOVIE_X * MOVIE_Y * 2);
  /* ムービの展開アドレスを設定 */
  CPK_SetDecodeAddr(cpk, (void *)work_ram_buf[num], 2 * MOVIE_X);
  CPK_SetPcmStreamNo(cpk, num);
  CPK_SetVolume(cpk, 0);

  return cpk;
}

/*--------------------------------------------------------------------------*/
/*								 Sound Routin								*/
/*--------------------------------------------------------------------------*/
extern Uint32 sddrvsize ;
extern Uint32 bootsndsize ;
extern char sddrvstsk[];
extern char bootsnd[];

void sndInit(void)
{
#if 0
  slInitSound((void*)sddrvstsk, sddrvsize, (void*)bootsnd, bootsndsize) ;
  *(volatile Uint8 *)(0x25a004e1) = 0x00 ; /* Hand Shake Off */
#else
  SndIniDt 	snd_init;

  SND_INI_PRG_ADR(snd_init) 	= (Uint16 *)sddrvstsk;
  SND_INI_PRG_SZ(snd_init) 	= (Uint16 )sddrvsize;
  SND_INI_ARA_ADR(snd_init) 	= (Uint16 *)bootsnd;
  SND_INI_ARA_SZ(snd_init) 	= (Uint16 )bootsndsize;
  SND_Init(&snd_init);
  SND_ChgMap(0);
#endif
}

void setSprite(void)
{
  Sint32 i ;

  i = 0 ;
  film_spr[i].CTRL = 0 ;
  film_spr[i].LINK = 0 ;
  film_spr[i].PMOD = (ECdis | CL32KRGB) ;
  film_spr[i].COLR = 0 ;
  film_spr[i].SRCA = (VDP1_MOVIE_1/8) ;
  film_spr[i].SIZE = (MOVIE_X/8)<<8 | MOVIE_Y;
  film_spr[i].XA = 16-(WIDTH_H/2);
  film_spr[i].YA = 16-(WIDTH_V/2);
  film_spr[i].GRDA = 0 ;
  film_spr[i].DMMY = 0 ;

  i = 1 ;
  film_spr[i].CTRL = 0 ;
  film_spr[i].LINK = 0 ;
  film_spr[i].PMOD = (ECdis | CL32KRGB) ;
  film_spr[i].COLR = 0 ;
  film_spr[i].SRCA = ((VDP1_MOVIE_2)/8) ;
  film_spr[i].SIZE = (MOVIE_X/8)<<8 | MOVIE_Y;
  film_spr[i].XA = (WIDTH_H/2)+16-(WIDTH_H/2);
  film_spr[i].YA = 16-(WIDTH_V/2);
  film_spr[i].GRDA = 0 ;
  film_spr[i].DMMY = 0 ;

  i = 2 ;
  film_spr[i].CTRL = 0;
  film_spr[i].LINK = 0 ;
  film_spr[i].PMOD = (ECdis | CL32KRGB) ;
  film_spr[i].COLR = 0 ;
  film_spr[i].SRCA = ((VDP1_MOVIE_3)/8) ;
  film_spr[i].SIZE = (MOVIE_X/8)<<8 | MOVIE_Y;
  film_spr[i].XA = 16-(WIDTH_H/2);
  film_spr[i].YA = (WIDTH_V/2)+16-(WIDTH_V/2);
  film_spr[i].GRDA = 0 ;
  film_spr[i].DMMY = 0 ;

  i = 3 ;
  film_spr[i].CTRL = 0;
  film_spr[i].LINK = 0 ;
  film_spr[i].PMOD = (ECdis | CL32KRGB) ;
  film_spr[i].COLR = 0 ;
  film_spr[i].SRCA = ((VDP1_MOVIE_4)/8) ;
  film_spr[i].SIZE = (MOVIE_X/8)<<8 | MOVIE_Y;
  film_spr[i].XA = (WIDTH_H/2)+16-(WIDTH_H/2);
  film_spr[i].YA = (WIDTH_V/2)+16-(WIDTH_V/2);
  film_spr[i].GRDA = 0 ;
  film_spr[i].DMMY = 0 ;

}

CpkHn createMemMovie(void)
{
  CpkHn cpk;
  CpkCreatePara	para;
  CpkHeader		*header;
  int				file_no = FILE_NUM-1;
  Sint32			load_size;

  /* ムービの生成 */
  CPK_PARA_WORK_ADDR(&para) = g_movie_work[file_no];
  CPK_PARA_WORK_SIZE(&para) = CPK_15WORK_BSIZE;
  CPK_PARA_BUF_ADDR(&para) = (Uint32 *)LOAD_MEM_ADDR;
  CPK_PARA_BUF_SIZE(&para) = LOAD_MEM_SIZE;
  CPK_PARA_PCM_ADDR(&para) = g_pcm_addr[file_no];
  CPK_PARA_PCM_SIZE(&para) = PCM_SIZE;
  cpk = CPK_CreateMemMovie(&para);
  if (cpk == NULL) {
    slPrint( "Error Open Handle", slLocate( 9, 3 ) );
    while( -1 ) slSynch();
  }

  /* 表示色数の設定 */
  CPK_SetColor(cpk, CPK_COLOR_15BIT);

  /* メモリ上のムービのファイルサイズを設定 */
  CPK_NotifyWriteSize(cpk, LOAD_MEM_SIZE);

  CPK_SetDecodeAddr(cpk, (void *)work_ram_buf[file_no], 2 * MOVIE_X);
  CPK_SetPcmStreamNo(cpk, file_no);
  return cpk;
}
Sint32 fileRead(char *filename, void *addr)
{
  Sint32 		fid;
  GfsHn  		gfs;
  Sint32		sctsize, nsct, lastsize;
  Sint32		load_size;

  STM_SetExecGrp(NULL);
  fid = GFS_NameToId(filename);
  gfs = GFS_Open(fid);
  load_size = 0;
  if (gfs != NULL) {
    GFS_GetFileSize(gfs, &sctsize, &nsct, &lastsize);
    load_size = GFS_Fread(gfs, nsct, addr, sctsize * (nsct - 1) +lastsize);
    GFS_Close(gfs);
  }
  STM_SetExecGrp(grp_hd);
  return load_size;
}
