/*---------------------------------------------------------
 * SGLライブラリCD関数用	1995.10.30
 *---------------------------------------------------------*/
#include	<stdlib.h>
#include	"sgl_cd.h"
#include	"sl_def.h"
#include	"stm_def.h"
#include	"stm_loc.h"
#include	"gfs_dir.h"

/*----------------------------------------------------------------------*/

/* リカバリ処理の回数	*/
#define		MAX_READERR	10

/*----------------------------------------------------------------------*/

/* ファイルオープン情報	*/
typedef struct {
    StmHn	stm;
    CDKEY	key;
    CDBUF	buf;
} CDMNG;

/*----------------------------------------------------------------------*/

/* ライブラリ作業領域	*/
Uint32		slcd_gfswork[GFS_WORK_SIZE(SLCD_MAX_OPEN)/sizeof(Uint32)];
Uint32		slcd_stmwork[STM_WORK_SIZE(SLCD_MAX_OPEN, 
					SLCD_MAX_OPEN)/sizeof(Uint32)];

/* ファイルオープン情報	*/
CDMNG		slcd_mngtbl[SLCD_MAX_OPEN];

GfsDirId	slcd_rootdir;		/* ルートディレクトリ		*/
GfsDirId	slcd_curdir;		/* カレントディレクトリ		*/
GfsDirName	*slcd_dirtbl;		/* ディレクトリテーブル		*/
Sint32		slcd_ndir;		/* ディレクトリテーブルサイズ	*/
Sint32		slcd_rderr;		/* リードエラーのカウンタ	*/
Bool		slcd_rderrflag;		/* リードエラー検出フラグ	*/

/*----------------------------------------------------------------------*/

/********************************************************
 * ディレクトリ構造体を使用したディレクトリ移動		*
 * 関数値	ディレクトリのファイル数		*
 *******************************************************/
Sint32	slcd_chgDir(GfsDirId *dir)
{
    GfsDirTbl	dirtbl;
    Sint32	nfile;

    GFS_DIRTBL_TYPE(&dirtbl) = GFS_DIR_ID;
    GFS_DIRTBL_DIRID(&dirtbl) = dir;
    GFS_DIRTBL_NDIR(&dirtbl) = slcd_ndir;
    GFS_SetDir(&dirtbl);
    GFS_DIRTBL_TYPE(&dirtbl) = GFS_DIR_NAME;
    GFS_DIRTBL_DIRNAME(&dirtbl) = slcd_dirtbl;
    nfile = GFS_LoadDir(0, &dirtbl);
    if (nfile < 0)	return (nfile);
    GFS_SetDir(&dirtbl);
    return (nfile);
}


/********************************************************
 * ルートディレクトリへの移動				*
 * 関数値	ルートディレクトリのファイル数		*
 *******************************************************/
Sint32	slcd_chgRoot(void)
{
    Sint32	nfile;

    nfile = slcd_chgDir(&slcd_rootdir);
    if (nfile < 0)	return (nfile);
    memcpy(&slcd_curdir, &slcd_rootdir, sizeof(GfsDirId));
    return (nfile);
}


/********************************************************
 * 小文字から大文字への変換				*
 *******************************************************/
void	slcd_ltou(Sint8 *src, Sint8 *dst)
{
    for ( ; *src != '\0'; src++, dst++) {
	if ((*src >= 'a')&&(*src <= 'z')) {
	    *dst = *src - ('a' - 'A');
	} else {
	    *dst = *src;
	}
    }
    *dst = '\0';
}


/********************************************************
 * ファイル識別子の取得					*
 * name		ファイル名（小文字可）			*
 * 関数値	ファイル識別子				*
 *******************************************************/
Sint32	slcd_nameToId(Sint8 *name)
{
    Sint32	fid;
    Sint8	big[GFS_FNAME_LEN+1];   /* fix at 95.05.09      */

    slcd_ltou(name, big);	/* 小文字から大文字への変換	*/
    fid = GFS_NameToId(big);
    return (fid);
}


/********************************************************
 * 最終文字の位置の取得					*
 * str		文字列					*
 * ch1		検索するキャラクタ1			*
 * ch2		検索するキャラクタ2			*
 * 関数値	最後に見つかった文字の位置		*
 *******************************************************/
Sint8	*slcd_strLstCh(Sint8 *str, Sint8 ch1, Sint8 ch2)
{
    Sint32	i;

    for (i = strlen(str) - 1; i >= 0; i--) {
	if (*(str + i) == ch1) {
	    return (str + i);
	}
	if (*(str + i) == ch2) {
	    return (str + i);
	}
    }
    return (NULL);
}


/********************************************************
 * ストリーム管理テーブルの検索				*
 * stm		ストリームハンドル			*
 * 関数値	ストリーム管理テーブル			*
 *******************************************************/
CDMNG	*slcd_searchCdmng(StmHn stm)
{
    Sint32	i;

    for (i = 0; i < SLCD_MAX_OPEN; i++) {
	if (slcd_mngtbl[i].stm == stm) {
	    return (slcd_mngtbl + i);
	}
    }
    return (NULL);
}


/********************************************************
 * 転送関数						*
 *******************************************************/
Sint32	slcd_trfunc(void *obj, StmHn stm, Sint32 nsct)
{
    TRANS_FUNC	*trfunc;
    Uint32	*addr;
    Sint32	adinc;
    Sint32	ret;

    trfunc = (TRANS_FUNC *)obj;
    addr = STM_StartTrans(stm, &adinc);
    ret = (trfunc->func)(trfunc->obj, addr, adinc, nsct);
    return (ret);
}


/********************************************************
 * ストリームシステムエラー関数				*
 *******************************************************/
void	slcd_errfunc(void *obj, Sint32 ec)
{
    if (ec == STM_ERR_CDRD) {
	STM_Recover();
	slcd_rderrflag = TRUE;
    }
}


/*----------------------------------------------------------------------*/

/********************************************************
 * 初期化						*
 * nfile	１ディレクトリ中の最大ファイル数	*
 * work		作業領域				*
 * 関数値	ルートディレクトリのファイル数		*
 *******************************************************/
Sint32	slCdInit(Sint32 nfile, void *work)
{
    GfsDirTbl	dirtbl;			/* ディレクトリ情報管理領域	*/
    Sint32	root;			/* 抂鎮洶攜椿のファイル数	*/
    
    /* ファイルシステム初期化		*/
    GFS_DIRTBL_TYPE(&dirtbl) = GFS_DIR_NAME;
    slcd_dirtbl = (GfsDirName *)work;
    GFS_DIRTBL_DIRNAME(&dirtbl) = slcd_dirtbl;
    GFS_DIRTBL_NDIR(&dirtbl) = nfile;
    slcd_ndir = nfile;
    root = GFS_Init(SLCD_MAX_OPEN, slcd_gfswork, &dirtbl);
    if (root < 0)	return (root);

    /* ルートディレクトリ取得		*/
    GFS_GetDirInfo(0, &slcd_rootdir);
    slcd_rootdir.dirrec.atr |= GFS_ATR_END_TBL;
    memcpy(&slcd_curdir, &slcd_rootdir, sizeof(GfsDirId));

    /* ストリームシステム初期化		*/
    STM_Init(SLCD_MAX_OPEN, SLCD_MAX_OPEN, slcd_stmwork);

    /* ファイルオープン情報初期化	*/
    memset(slcd_mngtbl, 0, sizeof(CDMNG) * SLCD_MAX_OPEN);

    /* リードエラーのカウンタの初期化	*/
    slcd_rderr = 0;

    return (root);
}


/********************************************************
 * ディレクトリ移動					*
 * pathname	パス名					*
 * 関数値	移動先のディレクトリ内のファイル数	*
 *******************************************************/
Sint32	slCdChgDir(Sint8 *pathname)
{
    Sint8	dirname[GFS_FNAME_LEN+1];   /* fix at 95.05.09      */
    GfsDirTbl	dirtbl;			/* ディレクトリ情報管理領域	*/
    GfsDirId	curdir;
    Sint32	ret = 0;
    Sint32	i;

    memcpy(&curdir, &slcd_curdir, sizeof(GfsDirId));

    /* 絶対パス指定の場合	*/
    if ((*pathname == '/')||(*pathname == '\\')) {
	ret = slcd_chgRoot();
	if (ret < 0)		goto	err;
	pathname++;
    }

    while (*pathname != '\0') {
	for (i = 0; i < GFS_FNAME_LEN; i++, pathname++) {
	    if ((*pathname == '/')||(*pathname == '\\')||(*pathname == '\0')) {
		break;
	    }
	    dirname[i] = *pathname;
	}
	dirname[i] = '\0';
	if (*pathname != '\0')	pathname++;
	ret = slcd_nameToId(dirname);
	if (ret < 0)		goto	err;
	GFS_GetDirInfo(ret, &slcd_curdir);
	GFS_DIRTBL_TYPE(&dirtbl) = GFS_DIR_NAME;
	GFS_DIRTBL_DIRNAME(&dirtbl) = slcd_dirtbl;
	GFS_DIRTBL_NDIR(&dirtbl) = slcd_ndir;
    	ret = GFS_LoadDir(ret, &dirtbl);
	if (ret < 0)		goto	err;
	GFS_SetDir(&dirtbl);
    }
    return (ret);
err:
    slcd_chgDir(&curdir);
    memcpy(&slcd_curdir, &curdir, sizeof(GfsDirId));
    return (ret);
}


/********************************************************
 * ファイルオープン					*
 * pathname	パス名					*
 * key		ストリームデータを分類するためのキー	*
 * 関数値	ファイルハンドル			*
 *******************************************************/
CDHN	slCdOpen(Sint8 *pathname, CDKEY key[])
{
    Sint8	*fname;
    GfsDirId	curdir;
    Sint32	nfile;
    Sint32	fid;
    StmKey	stmkey;
    Bool	chgdir = FALSE;
    Sint32	i, j;
    StmHn	stm;
    CDHN	cdhn;
    CDMNG	*mng[SLCD_MAX_OPEN];
    CDKEY	nulkey[2];
    Sint8	ch;

    fname = slcd_strLstCh(pathname, '\\', '/');
    if (fname != NULL) {	/* パス指定されている場合	*/
	chgdir = TRUE;
	memcpy(&curdir, &slcd_curdir, sizeof(GfsDirId));
	fname++;
	ch = *fname;
	*fname = '\0';
	nfile = slCdChgDir(pathname);
	*fname = ch;
	if (nfile < 0)		return (NULL);
    } else {			/* ファイル名のみの場合		*/
	fname = pathname;
    }
    fid = slcd_nameToId(fname);
    if (fid < 0)		goto	err1;
    cdhn = STM_OpenGrp();
    if (cdhn == NULL)		goto	err1;
    memset(mng, 0, sizeof(CDMNG *) * SLCD_MAX_OPEN);
    if (key == NULL) {
	nulkey[0].cn = nulkey[0].sm = nulkey[0].ci = CDKEY_NONE;
	nulkey[1].cn = CDKEY_TERM;
	key = nulkey;
    }
    for (i = 0; i < SLCD_MAX_OPEN; i++) {
	if (key[i].cn == CDKEY_TERM)	break;
	STM_KEY_CN(&stmkey) = key[i].cn;
	STM_KEY_SMMSK(&stmkey) = STM_KEY_SMVAL(&stmkey) = key[i].sm;
	STM_KEY_CIMSK(&stmkey) = STM_KEY_CIVAL(&stmkey) = key[i].ci;
	if (IS_CDFILE(GFS_DIR_ATR(slcd_dirtbl + fid)) == TRUE) {
	    stm = STM_OpenFid(cdhn, fid, &stmkey, STM_LOOP_NOREAD);
	} else {
	    stm = STM_OpenResi(cdhn, fid, &stmkey, STM_FAD_CDTOP);
	}
	if (stm == NULL)	goto	err2;
	STM_ResetTrBuf(stm);
	STM_SetTrPara(stm, STM_TR_ALL);
	mng[i] = slcd_searchCdmng(NULL);
	mng[i]->stm = stm;
	memcpy(&(mng[i]->key), &(key[i]), sizeof(CDKEY));
    }
    STM_SetLoop(cdhn, NULL, 1);
    if (chgdir == TRUE) {
	slcd_chgDir(&curdir);
	memcpy(&slcd_curdir, &curdir, sizeof(GfsDirId));
    }
    return (cdhn);
err2:
    for (j = 0; j < i; j++) {
	mng[i]->stm = NULL;
    }
    STM_CloseGrp(cdhn);
err1:
    if (chgdir == TRUE) {
	slcd_chgDir(&curdir);
	memcpy(&slcd_curdir, &curdir, sizeof(GfsDirId));
    }
    return (NULL);
}


/********************************************************
 * ＣＤ用イベント関数
 *******************************************************/
void	slCdEvent(void)
{
    slcd_rderrflag = FALSE;	/* リードエラー発生フラグの初期化	*/
    STM_ExecServer();
    if (slcd_rderrflag == TRUE) {	/* リードエラー発生		*/
	slcd_rderr++;
    } else {				/* リードエラーは発生しなかった	*/
	slcd_rderr = 0;
    }
}

void	slCdNlEvent(void)
{
}


/********************************************************
 * ファイルの読み込み					*
 * cdhn		ファイルハンドル			*
 * buf		読み込み領域情報			*
 * 関数値	エラーコード				*
 *******************************************************/
Sint32	slCdLoadFile(CDHN cdhn, CDBUF buf[])
{
    Sint32	i, nstm;
    StmHn	stm;
    CDMNG	*cdmng;
    TRANS_COPY	*copy;
    TRANS_FUNC	*trfunc;

    nstm = STM_GetStmNum(cdhn);
    for (i = 0; i < nstm; i++) {
	stm = STM_GetStmHndl(cdhn, i);
	if (buf[i].type == CDBUF_TERM) {
	    break;
	} else {
	    cdmng = slcd_searchCdmng(stm);
	    memcpy(&(cdmng->buf), &(buf[i]), sizeof(CDBUF));
	    if (buf[i].type == CDBUF_COPY) {
	        copy = &(buf[i].trans.copy);
	        STM_SetTrBuf(stm, copy->addr, copy->size, copy->unit);
		if (copy->addr == NULL) {
		    STM_SetTrMode(stm, STM_TR_CPU);
		} else {
		    STM_SetTrMode(stm, STM_TR_SDMA0);
		}
	    } else if (buf[i].type == CDBUF_FUNC) {
		trfunc = &(cdmng->buf.trans.func);
		STM_SetTrFunc(stm, slcd_trfunc, trfunc);
	    }
	}
    }
    STM_NwSetExecGrp(cdhn);
    SetCDFunc((void (*)())slCdEvent);
    slcd_rderr = 0;
    return (CDERR_OK);
}


/********************************************************
 * ストリームの転送					*
 * cdhn		ファイルハンドル			*
 * buf		読み込み領域情報			*
 * 関数値	エラーコード				*
 *******************************************************/
Sint32	slCdTrans(CDHN cdhn, CDBUF buf[], Sint32 ndata[])
{
    Sint32	nstm;
    Sint32	i;
    StmHn	stm;
    CDMNG	*cdmng;
    TRANS_COPY	*copy;
    TRANS_FUNC	*trfunc;

    nstm = STM_GetStmNum(cdhn);
    /* 転送領域が未設定のストリームに対して転送先を設定する	*/
    for (i = 0; i < nstm; i++) {
	stm = STM_GetStmHndl(cdhn, i);
	cdmng = slcd_searchCdmng(stm);
	if ((cdmng->buf.type == CDBUF_COPY)&&
		(cdmng->buf.trans.copy.addr == NULL)) {
	    if (buf[i].type == CDBUF_COPY) {
		copy = &(buf[i].trans.copy);
	        STM_SetTrBuf(stm, copy->addr, copy->size, copy->unit);
		STM_SetTrMode(stm, STM_TR_CPU);
	    } else if (buf[i].type == CDBUF_FUNC) {
		trfunc = &(cdmng->buf.trans.func);
		STM_SetTrFunc(stm, slcd_trfunc, trfunc);
	    }
	    STM_ExecTrans(stm);		/* 転送実行	*/
	    if (ndata != NULL) {
	        ndata[i] = STM_GetLenTrBuf(stm) * sizeof(Uint16);
	    }
	} else {
	    /* 転送先が設定されているものは転送ゲートを閉じておく	*/
	    STM_SetTrGate(stm, STM_GATE_CLOSE);
	}
    }

    for (i = 0; i < nstm; i++) {
	stm = STM_GetStmHndl(cdhn, i);
	cdmng = slcd_searchCdmng(stm);
	if ((cdmng->buf.type == CDBUF_COPY)&&
		(cdmng->buf.trans.copy.addr == NULL)) {
	    /* 転送領域をクリアする		*/
	    STM_SetTrBuf(stm, NULL, 0, STM_UNIT_WORD);
	    STM_SetTrFunc(stm, STM_TR_NULLFUNC, NULL);
	} else {
	    /* 転送ゲートを開ける		*/
	    STM_SetTrGate(stm, STM_GATE_OPEN);
	}
    }
    return (CDERR_OK);
}
	    

/********************************************************
 * 転送領域のリセット					*
 * cdhn		ファイルハンドル			*
 * key		ストリームデータを分類するためのキー	*
 * 関数値	TRUE	:リセットできた			*
 *		FALSE	:リセットできなかった		*
 *******************************************************/
Bool	slCdResetBuf(CDHN cdhn, CDKEY *key)
{
    Sint32	nstm;
    StmHn	stm;
    CDMNG	*cdmng;
    Sint32	i;
    Bool	ret = FALSE;

    nstm = STM_GetStmNum(cdhn);
    for (i = 0; i < nstm; i++) {
	stm = STM_GetStmHndl(cdhn, i);
	cdmng = slcd_searchCdmng(stm);
	if ((key == NULL)||
	    ((key->cn == cdmng->key.cn)&&
	     (key->sm == cdmng->key.sm)&&
	     (key->ci == cdmng->key.ci))) {
	    STM_ResetTrBuf(stm);
	    if (key == NULL) {
		ret = TRUE;
	    } else {
		return (TRUE);
	    }
	}
    }
    return (ret);
}


/********************************************************
 * 読み込み中断						*
 * cdhn		ファイルハンドル			*
 * 関数値	エラーコード				*
 *******************************************************/
Sint32	slCdAbort(CDHN cdhn)
{
    Sint32	nstm;
    Sint32	i;
    CDMNG	*cdmng;
    StmHn	stm;

    nstm = STM_GetStmNum(cdhn);
    for (i = 0; i < nstm; i++) {
	stm = STM_GetStmHndl(cdhn, i);
	cdmng = slcd_searchCdmng(stm);
	cdmng->stm = NULL;
    }
    STM_CloseGrp(cdhn);
    SetCDFunc((void (*)())slCdNlEvent);
    return (CDERR_OK);
}


/********************************************************
 * 読み込み一時停止					*
 * cdhn		ファイルハンドル			*
 * 関数値	エラーコード				*
 *******************************************************/
Sint32	slCdPause(CDHN cdhn)
{
    Bool	ret;

    ret = STM_NwSetExecGrp(NULL);
    if (ret == FALSE) {
	STM_ExecServer();
	return (CDERR_BUSY);
    }
    return (CDERR_OK);
}


/********************************************************
 * ステータスの取得					*
 * cdhn		ファイルハンドル			*
 * ndata	転送領域の有効データ数			*
 * 関数値	ステータス				*
 *******************************************************/
Sint32	slCdGetStatus(CDHN cdhn, Sint32 ndata[])
{
    Sint32	nsct;
    Sint32	stat;
    Sint32	nstm;
    Sint32	i;
    StmHn	stm;
    CdcStat	drvstat;
    Sint32	fad;
    Uint16	hirq;

    if (cdhn == NULL)	return (CDSTAT_PAUSE);

    /* 有効データ数の設定	*/
    /* CDブロック状態の取得	*/
    if (cdhn == CDREQ_FREE) {		/* 空きセクタ数の取得		*/
	nsct = STL_CsctGetFreeSctnum();
	return (nsct);
    } else {
	hirq = GFCD_GetStat(&drvstat);
    	if (hirq & CDC_HIRQ_DCHG) {
	    return (CDERR_OPEN);
	}
	if (cdhn == CDREQ_FAD) {	/* ピックアップの位置の取得	*/
	    return (CDC_STAT_FAD(&drvstat));
	} else if (cdhn == CDREQ_DRV) {	/* ドライブ状態の取得		*/
	    return (CDC_STAT_STATUS(&drvstat) & 0x0f);
	} else if (ndata != NULL) {
	    nstm = STM_GetStmNum(cdhn);
	    for (i = 0; i < nstm; i++) {
		stm = STM_GetStmHndl(cdhn, i);
		ndata[i] = STM_GetLenTrBuf(stm) * sizeof(Uint16);
	    }
	}
    }

    /* リードエラーのチェック	*/
    if ((cdhn == MNG_CURGRP(stm_mng_ptr))&&(slcd_rderr > MAX_READERR)) {
	return (CDERR_RDERR);
    }
    /* 読み込みのステータス取得	*/
    stat = STM_GetExecStat(cdhn, &fad);
    if ((stat != CDSTAT_PAUSE)&&(stat != CDSTAT_WAIT)&&
	(stat != CDSTAT_DOING)&&(stat != CDSTAT_COMPLETED)) {
	stat = CDSTAT_DOING;
    } else if (stat == CDSTAT_COMPLETED) {
	if (stat == CDSTAT_COMPLETED) {
	    slCdAbort(cdhn);
	}
    }

    return (stat);
}


/* end of sgl_cd.c	*/




