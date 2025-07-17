/*
   ★サウンドインターフェイスライブラリを用いたSGLサウンド関数の実現例★

     尚、ほとんどはマクロで定義できるのだが、sl_def.hの関数定義を
     そのまま生かす為に、敢えて関数形式にしてある。
     This Program based on Sound Interface library Ver. 1.30 above.
     If you use before Ver. 1.30, this program don't work well.

                                                             by  tany
*/

#include "sgl.h"
#include "sega_snd.h"

#define SND_BNK_OFF 0
#define SND_BNK_ON 1

Uint8 SndMngBnk[7];

void slInitSound( Uint8 *drv, Uint32 drvsz, Uint8 *map, Uint32 mapsz ) {
  SndIniDt     sndini;
  int          i;
  Uint8        *addr;

  /* Initialize internal bank management buffer */
  for ( i = 0 ; i < 7 ; ++i ) SndMngBnk[ i ] = SND_BNK_OFF;

  /* Initialize Sound Host */
  SND_INI_PRG_ADR( sndini ) = ( Uint16 * )drv;
  SND_INI_PRG_SZ( sndini )  = ( Uint16 )( drvsz );
  SND_INI_ARA_ADR( sndini ) = ( Uint16 * )map;
  SND_INI_ARA_SZ( sndini )  = ( Uint16 )( mapsz );

  SND_Init( &sndini );

  /* Map Change */
  SND_ChgMap( 0 );

  /* Set Transfer end flag */
  addr = ( Uint8 * )( 0x25a00500 );
  while( *addr != 0xff ) {
    addr += 4;
    *addr |= 0x80;
  }

  /* Set Total volume */
  SND_SetTlVl( 15 );
}

Bool slSndVolume( Uint8 vol ) {
  if ( SND_SetTlVl( ( SndTlVl )( vol >> 3 ) ) == SND_RET_NSET ) return FALSE;
  return TRUE;
}

Bool slBGMOn( Uint16 song, Uint8 prio, Uint8 vol, Uint8 rate ) {
  if ( SND_StartSeq( ( SndSeqNum )0, ( song & 0xff00 ) >> 8 ,
		    ( song & 0x00ff ), prio ) == SND_RET_NSET ) return FALSE;
  if ( SND_SetSeqVl( ( SndSeqNum )0, ( SndSeqVl )0, ( SndFade )0 )
      == SND_RET_NSET ) return FALSE;
  if ( SND_SetSeqVl( ( SndSeqNum )0, ( SndSeqVl )vol, ( SndFade )rate )
      == SND_RET_NSET ) return FALSE;
  return TRUE;
}

Bool slBGMOff() {
  if ( SND_StopSeq( ( SndSeqNum )0 ) == SND_RET_NSET ) return FALSE;
  return TRUE;
}

Bool slBGMPause() {
  if ( SND_PauseSeq( ( SndSeqNum )0 ) == SND_RET_NSET ) return FALSE;
  return TRUE;
}

Bool slBGMCont() {
  if ( SND_ContSeq( ( SndSeqNum )0 ) == SND_RET_NSET ) return FALSE;
  return TRUE;
}

Bool slBGMFade( Uint8 vol, Uint8 rate ) {
  if ( SND_SetSeqVl( 0, ( SndSeqVl )vol, ( SndFade )rate ) == SND_RET_NSET )
    return FALSE;
  return TRUE;
}

Bool slBGMTempo( Sint16 tempo ) {
  if ( SND_ChgTempo( ( SndSeqNum )0, ( SndTempo )tempo ) == SND_RET_NSET )
    return FALSE;
  return TRUE;
}

/*Bool slBGMStat() {
  // Defined as macros in sl_def.h
  SndSeqStat stat;
  
  SND_GetSeqStat( &stat, ( SndSeqNum )0 );
  
  if ( SND_SEQ_STAT_MODE( stat ) == SND_MD_PLAY ) return TRUE;
  return FALSE;
}
*/
Uint8 slSequenceOn( Uint16 song, Uint8 prio, Uint8 vol, Sint8 pan ) {
  int i;
  for ( i = 7 ; i !=0 ; --i ) if ( SndMngBnk[ i ] == SND_BNK_OFF ) break;
  if ( i == 0 ) return FALSE;

  if ( SND_SetSeqVl( ++i, ( SndSeqVl )vol, ( SndFade )0 ) == SND_RET_NSET )
    return FALSE;
  if ( SND_StartSeq( i, ( song & 0xff00 ) >> 8, ( song & 0x00ff ), prio )
      == SND_RET_NSET ) return FALSE;
  SndMngBnk[ i - 1 ] = SND_BNK_ON;
  return i;
}

Bool slSequenceOff( Uint8 num ) {
  if ( SND_StopSeq( ( SndSeqNum )num ) == SND_RET_NSET ) return FALSE;
  SndMngBnk[ num - 1 ] = SND_BNK_OFF;
  return TRUE;
}

Bool slSequencePause( Uint8 num ) {
  if ( SND_PauseSeq( ( SndSeqNum )num ) == SND_RET_NSET ) return FALSE;
  return TRUE;
}

Bool slSequenceCont( Uint8 num ) {
  if ( SND_ContSeq( ( SndSeqNum )num ) == SND_RET_NSET ) return FALSE;
  return TRUE;
}

Bool slSequenceFade( Uint8 num, Uint8 vol, Uint8 rate ) {
  if ( SND_SetSeqVl( ( SndSeqNum )num, ( SndSeqVl )vol, ( SndFade )rate )
      == SND_RET_NSET ) return FALSE;
  return TRUE;
}

Bool slSequencePan( Uint8 num, Sint8 pan ) {
  /* Second argument of SndSetSeqPan is Command Control Switch, this parameter
     is 00h to On, 80h to Off.
  */
  if ( SND_SetSeqPan( ( SndSeqNum )num, 0, ( pan >> 3 ) ) == SND_RET_NSET )
    return FALSE;
  return TRUE;
}

Bool slSequenceStat( Uint8 num ) {
  SndSeqStat stat;

  SND_GetSeqStat( &stat, ( SndSeqNum )num );

  if ( SND_SEQ_STAT_MODE( stat ) == SND_MD_PLAY ) return TRUE;
  return FALSE;
}

Bool slSequenceTempo( Uint8 num, Sint16 tempo ) {
  if ( SND_ChgTempo( ( SndSeqNum )num, ( SndTempo )tempo ) == SND_RET_NSET )
    return FALSE;
  return TRUE;
}

Bool slSequenceReset( Uint8 num ) {
  Bool ret = TRUE;
  if ( SND_SetSeqVl( ( SndSeqNum )num, ( SndTlVl )127, ( SndFade )0 )
      == SND_RET_NSET ) ret = FALSE;
  if ( SND_ChgTempo( ( SndSeqNum )num, ( SndTempo )0 ) == SND_RET_NSET )
    ret = FALSE;
  if ( SND_SetSeqPan( ( SndSeqNum )num, 0x80, 0 ) == SND_RET_NSET )
    ret = FALSE;

  return ret;
}
  
Bool slSndEffect( Uint8 eff ) {
  if ( SND_ChgEfct( ( SndEfctBnkNum )eff ) == SND_RET_NSET ) return FALSE;
  return TRUE;
}

void *slSndMapChange( Uint8 map ) {
  if ( SND_ChgMap( ( SndAreaMap )map ) == SND_RET_NSET ) return FALSE;
  return ( void * )( 0x25a00700 );
}

Bool slSndMixChange( Uint8 tbnk, Uint8 mixno ) {
  if ( SND_ChgMix( ( SndToneBnkNum )tbnk, ( SndMixBnkNum )mixno )
      == SND_RET_NSET ) return FALSE;
  return TRUE;
}

Bool slSndMixParmChange( Uint8 efct, Uint8 levl, Sint8 pan ) {
  if ( SND_ChgMixPrm( ( SndEfctOut )efct, ( SndLev )levl, ( SndPan )pan )
      == SND_RET_NSET ) return FALSE;
  return TRUE;
}

Uint8 slSndSeqNum() {
  Uint8 i;
  for ( i = 0 ; i < 7 ; ++i ) if ( SndMngBnk[ i ] == SND_BNK_OFF ) return ++i;
  return 0;
}

Bool slCDDAOn( Uint8 lev_l, Uint8 lev_r, Sint8 pan_l, Sint8 pan_r ) {
  Bool ret = TRUE;
  if ( SND_SetCdDaLev( ( SndLev )lev_l, ( SndLev )lev_r ) == SND_RET_NSET )
    ret = FALSE;
  if ( SND_SetCdDaPan( ( SndPan )pan_l, ( SndPan )pan_r ) == SND_RET_NSET )
    ret = FALSE;
  return ret;
}

Bool slCDDAOff() {
  if ( SND_SetCdDaLev( ( SndLev )0, ( SndLev )0 ) == SND_RET_NSET )
    return FALSE;
  return TRUE;
}

Bool slDSPOff() {
  if ( SND_StopDsp() == SND_RET_NSET ) return FALSE;
  return TRUE;
}

Bool slSoundAllOff() {
  SND_Init_Sound( 1, 0, 0, 0, 0 );
  return TRUE;
}

void slWaitSound( void *func ) {
  while( *( Uint8 * )func != 0 );
}

/*
   必ずしも全てが出来るわけじゃないさ。ハハハ...
*/
Sint8 slSoundRequest( const char *parm, ... ) {
  return FALSE;
}

Sint8 slPCMOn( PCM *pcm, void *data, Uint32 size ) {
  return FALSE;
}

Bool slPCMOff( PCM *pcm ) {
  return FALSE;
}
  
Bool slPCMStat( PCM *pcm ) {
  return FALSE;
}

Bool slPCMParmChange( PCM *pcm ) {
  SndPcmChgPrm chg;

  SND_PRM_NUM( chg )    = pcm->channel;
  SND_PRM_LEV( chg )    = pcm->level;
  SND_PRM_PAN( chg )    = pcm->pan;
  SND_PRM_PICH( chg )   = pcm->pitch;
  SND_R_EFCT_IN( chg )  = pcm->efselectR;
  SND_R_EFCT_LEV( chg ) = pcm->eflevelR;
  SND_L_EFCT_IN( chg )  = pcm->efselectL;
  SND_L_EFCT_LEV( chg ) = pcm->eflevelL;

  if ( SND_ChgPcm( &chg ) == SND_RET_NSET ) {
    return FALSE;
  }
  return TRUE;
}
