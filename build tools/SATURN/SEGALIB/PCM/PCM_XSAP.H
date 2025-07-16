/******************************************************************************
 *	ソフトウェアライブラリ
 *
 *	Copyright (c) 1994,1995,1996,1997 SEGA
 *
 * Library	:ＰＣＭ・ＡＤＰＣＭ再生ライブラリ
 * Module 	:read SAP format
 * File		:pcm_xsap.h
 * Date		:1997-06-23
 * Version	:1.24
 * Author	:Y.H
 *
 *--------------------------------------------------------------------------*
 * Update	:1996-07-05	1.00	Y.H	新規作成
 *			:1996-10-14	1.21	Y.H	SHC Ver. 3.0F対応
 *
 ****************************************************************************/


#if !defined(PCM_SAP_H)
#define PCM_SAP_H


/************************************************************************/
/*		□定数マクロ													*/
/************************************************************************/
/* １ブロックのバイト数  */
#define	PCMSAP_BLOCK_SIZE		(2*2048)	/*  2 sector  */




/************************************************************************/
/*		□列挙定数														*/
/************************************************************************/
/*  チャネル  */
typedef enum {
	PCMSAP_CHANNEL_MONO		= 1,	/*  モノラル	*/
	PCMSAP_CHANNEL_STEREO	= 2		/*  ステレオ	*/
} PcmSapChannelType;

/*  量子化ビット  */
typedef enum {
	PCMSAP_BITS_8			= 8,	/*    ８ビット	*/
	PCMSAP_BITS_16			= 16	/*  １６ビット	*/
} PcmSapSampBits;




/************************************************************************/
/*		□処理マクロ													*/
/************************************************************************/
/*  SAP ファイル  */
#define	PCMSAP_FILE_HEADERP(sap)			(&((sap)->sapheader))
#define	PCMSAP_FILE_DATAP(sap)				(&((sap)->sapdata))

/*  SAP ファイルヘッダ  */
#define	PCMSAP_HEADER_SAMPLE(hd)			((hd)->sample)
#define	PCMSAP_HEADER_CHANNEL(hd)			((hd)->channel)
#define	PCMSAP_HEADER_BITS(hd)				((hd)->bits)
#define	PCMSAP_HEADER_FREQP(hd)				((hd)->freq)
#define	PCMSAP_HEADER_RSVP(hd)				((hd)->rsv)

/*  mono  8bit データへのポインタ  */
#define	PCMSAP_DATA_MONO8_DATAP(dat)		((dat)->mono8.data)

/*  mono 16bit データへのポインタ  */
#define	PCMSAP_DATA_MONO16_DATAP(dat)		((dat)->mono16.data)

/*  stereo 8bit データへのポインタ  */
#define	PCMSAP_DATA_STEREO8_LDATAP(dat)		((dat)->stereo8.ldata)
#define	PCMSAP_DATA_STEREO8_RDATAP(dat)		((dat)->stereo8.rdata)

/*  stereo 16bit データへのポインタ  */
#define	PCMSAP_DATA_STEREO16_LDATAP(dat)	((dat)->stereo16.ldata)
#define	PCMSAP_DATA_STEREO16_RDATAP(dat)	((dat)->stereo16.rdata)




/************************************************************************/
/*		□データ型宣言													*/
/************************************************************************/
/*  SAP ファイルヘッダ部			*/
/*  sizeof(PcmSapHeader) = 2048		*/
typedef struct {
	Sint32	sample;			/*  １チャネルあたりの総サンプル数  */
	Sint32	channel;		/* 　モノラル、ステレオ             */
	Sint32	bits;			/*   サンプリングビット数（8/16）	*/
	Uint8	freq[10];		/*   サンプリング周波数（AIFと同型）*/
	Uint8	rsv[2026];		/*   予約領域（2026 byte）			*/
} PcmSapHeader;

/*  SAP ファイルデータ部  */
typedef union {
	/*  mono   /  8bit  */
	struct {
		Sint8 data[PCMSAP_BLOCK_SIZE];
	} mono8;
	/*  mono   / 16bit  */
	struct {
		Sint16 data[PCMSAP_BLOCK_SIZE];
	} mono16;
	/*  stereo /  8bit  */
	struct {
		Sint8 ldata[PCMSAP_BLOCK_SIZE];
		Sint8 rdata[PCMSAP_BLOCK_SIZE];
	} stereo8;
	/*  stereo / 16bit  */
	struct {
		Sint16 ldata[PCMSAP_BLOCK_SIZE];
		Sint16 rdata[PCMSAP_BLOCK_SIZE];
	} stereo16;
} PcmSapData;

/*  SAP ファイル全体  */
typedef struct {
	/*  ヘッダ部  */
	PcmSapHeader	sapheader;		/*  2048 byte		*/

	/*  データ部  */
	PcmSapData		sapdata;		/*  4096*n byte		*/
} PcmSapFile;




/************************************************************************/
/*		□関数宣言														*/
/************************************************************************/


#endif		/*  end of PCM_SAP_H  */


/*  end of file  */
