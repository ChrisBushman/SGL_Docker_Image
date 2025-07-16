/*****************************************************************************
 *      Include files.
 *****************************************************************************/
#include "sgl.h"
#include "sgl_cd.h"
#include "sega_snd.h"

/*****************************************************************************
 *      Constant macros.
 *****************************************************************************/
/* Use Frame address */
/*#define FAD */
/* Track number. Espesially for saturn, CDDA track not start track number 1.
   For example,
     Not use MODE2 Track, CDDA Truck starts 2.
     Use MODE2 Track, CDDA Tauck starts after 3.
*/
#define TRACK_NO 2 
/* Use SGL sound functions (slCDDAOn) */
#define SGL_SND

#define USE_FILE_NAME
/*****************************************************************************
 *      Define functions.
 *****************************************************************************/
void sndInit( void );

/*****************************************************************************
 *      Define variables
 *****************************************************************************/
extern char sddrvstsk[];
extern long sddrvsize;

#ifdef USE_FILE_NAME
char fname[] = "CDDA1";

/*
  Maximum files in root directory.
  If over number of file in CD's root directory, GFS will fail operation.
*/
#define MAX_ROOT_DIR		100

/* Maximum files at the same time. */
#define OPEN_MAX	5

/* Directory information management area. (root) */
static GfsDirTbl	dir_root_tbl;

/* Directory information with file name. */
static GfsDirName dir_root_name[ MAX_ROOT_DIR ];

/* Work area of File System (GFS) */
Uint32 g_gfs_work[ ( GFS_WORK_SIZE( OPEN_MAX ) + 3 ) / 4 ];

#endif

/*****************************************************************************
 *      Define functions.
 *****************************************************************************/
#ifdef USE_FILE_NAME
void InitGFS( void ) {
  Sint32 file_num;

  /* Initialize of GFS */
  GFS_DIRTBL_TYPE( &dir_root_tbl )    = GFS_DIR_NAME;
  GFS_DIRTBL_DIRNAME( &dir_root_tbl ) = dir_root_name;
  GFS_DIRTBL_NDIR( &dir_root_tbl )    = MAX_ROOT_DIR;

  /* Initialize of File system */
  file_num = GFS_Init( OPEN_MAX, g_gfs_work, &dir_root_tbl );
  if ( file_num < 0 ) {
    slPrint( "Error! GFS Init.", slLocate( 5, 2 ) );
    slPrintHex( file_num, slLocate( 20, 2 ) );
    while( 1 ) slSynch();
  }
}

Sint32 GetFrameAddress( char *name ) {
  Sint32   fid;
  GfsHn    gfs;
  GfsDirId tbl;
  Uint32   i;

  if ( ( fid = GFS_NameToId( name ) ) < 0 ) {
    slPrint( "Error! No such filename.", slLocate( 5, 2 ) );
    slPrintHex( fid, slLocate( 15, 3 ) );
    while( 1 ) slSynch();
  }
  
  if ( GFS_GetDirInfo( fid, &tbl ) < 0 ) {
    slPrint( "Error! Can't get directory information.", slLocate( 2, 2 ) );
    while( 1 ) slSynch();
  }

  return GFS_DIR_FAD( &tbl );
}
#endif
/*
   Initialize Sound.
*/
void sndInit(void)
{
  Uint8 bootsnd_map[] = { 0xff, 0xff };
#ifdef SGL_SND
  slInitSound( sddrvstsk, sddrvsize, bootsnd_map, 2 );

  slCDDAOn(127, 127, 0, 0);
#else
  SndIniDt 	snd_init;

  SND_INI_PRG_ADR(snd_init)       = (Uint16 *)(&sddrvstsk);
  SND_INI_PRG_SZ(snd_init)        = sddrvsize;
  SND_INI_ARA_ADR(snd_init)       = (Uint16 *)(&bootsnd_map);
  SND_INI_ARA_SZ(snd_init)        = 1;

  SND_Init(&snd_init);
  SND_ChgMap(0);
  SND_SetCdDaLev( 7, 7 );
  SND_SetCdDaPan( -15, 15 );
#endif
}

/*
   Event routine. Do polling CD status.
*/
void event( EVENT *evnt ) {
  CdcStat stwork, *st = &stwork;

  while ( ( CDC_GetPeriStat( st ) != CDC_ERR_OK ) );
  slPrint( "Frame Address:", slLocate( 2, 15 ) );
  slPrintHex( CDC_STAT_FAD( st ), slLocate( 15, 15 ) );
  slPrint( "Track Number :", slLocate( 2, 16 ) );
  slPrintHex( CDC_STAT_TNO( st ), slLocate( 15, 16 ) );
  switch( CDC_STAT_STATUS( st ) & 0x0f ) {
  case CDC_ST_BUSY:
    slPrint( "BUSY         ", slLocate( 10, 10 ) );
    break;
  case CDC_ST_PAUSE:
    slPrint( "PAUSE        ", slLocate( 10, 10 ) );
    break;
  case CDC_ST_STANDBY:
    slPrint( "STANDBY      ", slLocate( 10, 10 ) );
    break;
  case CDC_ST_PLAY:
    slPrint( "NOW PLAYING!!", slLocate( 10, 10 ) );
    break;
  case CDC_ST_SEEK:
    slPrint( "SEEK         ", slLocate( 10, 10 ) );
    break;
  case CDC_ST_SCAN:
    slPrint( "SCAN         ", slLocate( 10, 10 ) );
    break;
  case CDC_ST_OPEN:
    slPrint( "TOREY OPEN   ", slLocate( 10, 10 ) );
    break;
  case CDC_ST_NODISC:
    slPrint( "NO DISC      ", slLocate( 10, 10 ) );
    break;
  case CDC_ST_RETRY:
    slPrint( "READ RETRY   ", slLocate( 10, 10 ) );
    break;
  case CDC_ST_ERROR:
    slPrint( "READ ERROR!! ", slLocate( 10, 10 ) );
  default:
    
  }
}

/*
   Main loop
*/
void ss_main() {
  CdcPos  poswork, *pos = &poswork;
  CdcPly  plywork, *ply = &plywork;
  CdcStat stwork, *st = &stwork;
  Sint32  ret;
#ifdef USE_FILE_NAME
  Sint32  fad;
#endif

  slInitSystem( TV_320x240, NULL, 1 );
  
  /* Initialize event */
  slInitEvent();
  slSetEvent( event );
  slIntFunction( slExecuteEvent );

#ifdef USE_FILE_NAME
  InitGFS();
  fad = GetFrameAddress( fname );
#endif

  sndInit();

  /* Pause */
  CDC_POS_PTYPE( pos ) = CDC_PTYPE_NOCHG;
  ret = CDC_CdSeek( pos );

  /* Wait stop pick */
  do {
    while ( ( CDC_GetPeriStat( st ) != CDC_ERR_OK ) );
  } while( ( ret = ( CDC_STAT_STATUS( st ) & 0x0f ) ) != CDC_ST_PAUSE );

#ifdef FAD
  /* Move pick to start of Track. */ 
  CDC_POS_PTYPE( pos ) = CDC_PTYPE_TNO;
  CDC_POS_TNO( pos )   = TRACK_NO;
  ret = CDC_CdSeek( pos );

  /* Wait stop pick */
  do {
    while ( ( CDC_GetPeriStat( st ) != CDC_ERR_OK ) );
  } while( ( ret = ( CDC_STAT_STATUS( st ) & 0x0f ) ) != CDC_ST_PAUSE );

#ifdef SCAN
  /* Scan 150 frame address */
  CDC_POS_PTYPE( pos ) = CDC_PTYPE_FAD;
  CDC_POS_FAD( pos )   = CDC_STAT_FAD( st ) + 150;
  ret = CDC_CdSeek( pos );

  /* Wait stop pick */
  do {
    while ( ( CDC_GetPeriStat( st ) != CDC_ERR_OK ) );
  } while( ( ret = ( CDC_STAT_STATUS( st ) & 0x0f ) ) != CDC_ST_PAUSE );
#endif

  /* Play */
  CDC_PLY_STYPE( ply ) = CDC_PTYPE_FAD;
  CDC_PLY_SFAD( ply )  = CDC_STAT_FAD( st );
  CDC_PLY_ETYPE( ply ) = CDC_PTYPE_FAD;
#elif defined( USE_FILE_NAME )
  CDC_PLY_STYPE( ply ) = CDC_PTYPE_FAD;
  CDC_PLY_SFAD( ply )  = fad;
  CDC_PLY_ETYPE( ply ) = CDC_PTYPE_FAD;
#else
  /* Track no play */
  CDC_PLY_STYPE( ply ) = CDC_PTYPE_TNO;
  CDC_PLY_STNO( ply )  = TRACK_NO;
  CDC_PLY_SIDX( ply )  = 0;
  CDC_PLY_PMODE( ply ) = CDC_PM_DFL;
#endif
  ret = CDC_CdPlay( ply );

  while( -1 ) {
    slSynch();
  }
}



