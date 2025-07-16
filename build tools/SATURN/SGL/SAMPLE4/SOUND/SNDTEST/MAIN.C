#include "sgl.h"

#define SEQ_START 0x25a0b000
#define MAP_ADDR  0x25a00500

/*
   Import symbols about sound driver.
*/
extern char sddrvstsk[];
extern long sddrvsize;

extern char map[];
extern char seq1[];
extern char seq2[];
extern long map_size;
extern long seq1_size;
extern long seq2_size;

extern Uint8 SynchConst;
extern Uint8 SynchCount;

typedef struct _SeqStat {
  Uint8  Bank;
  Uint8  Song;
  Sint8  Pan;
  Uint8  Volume;
  Sint16 Tempo;
} SeqStat;

typedef struct _AllStat {
  Uint8  DSP;
  Uint8  Mixer;
  Uint16 TotalVolume;
  Uint8  MaxBank;
  Uint8  MaxDSP;
  Uint8  AreaMap;
} AllStat;

AllStat sndstat;

/*
   Set Transfer end flag in Sound area map.
*/
void TransferEnd( void ) {
  Uint8 *addr;

  addr = ( Uint8 * )MAP_ADDR;
  while ( *( addr ) != 0xff ) {
    *( addr + 4 ) |= 0x80;
    addr += 8;
  }
}

/*
   Search Map and Tone bank element.
*/
void MapAnalyze( AllStat *stat ) {
  Uint8  i;
  Uint8 *addr;

  stat->MaxBank = stat->MaxDSP = 0;

  addr = ( Uint8 * )MAP_ADDR;
  while ( ( i = *( addr ) ) != 0xff ) {
    if ( ( i & 0xf0 ) == 0x10 ) ++stat->MaxBank;
    if ( ( i & 0xf0 ) == 0x20 ) ++stat->MaxDSP;
    addr += 8;
  }
}

/*
   Make control screen.
*/
void MakeScreen( SeqStat *parm, AllStat *stat ) {
  slPrintHex( stat->AreaMap, slLocate( 16, 4 ) );

  slPrintHex( parm->Bank, slLocate( 16, 7 ) );
  slPrintHex( parm->Song, slLocate( 16, 8 ) );

  slPrintHex( parm->Volume, slLocate( 16, 10 ) );
  slPrintHex( parm->Tempo, slLocate( 16, 11 ) );
  slPrintHex( parm->Pan, slLocate( 16, 12 ) );

  if ( stat->DSP == 0 ) {
    slPrint( "OFF", slLocate( 21, 14 ) );
  } else {
    slPrintHex( stat->DSP - 1, slLocate( 16, 14 ) );
  }
  slPrintHex( stat->Mixer, slLocate( 16, 15 ) );

  slPrintHex( stat->TotalVolume, slLocate( 16, 17 ) );

  slPrint( "Area Map Number:", slLocate( 4, 4 ) );
  slPrint( "Bank Number    :", slLocate( 4, 7 ) );
  slPrint( "Song Number    :", slLocate( 4, 8 ) );

  slPrint( "Sequence Volume:", slLocate( 4, 10 ) );
  slPrint( "Sequence Tempo :", slLocate( 4, 11 ) );
  slPrint( "Sequence Pan   :", slLocate( 4, 12 ) );

  slPrint( "DSP Number     :", slLocate( 4, 14 ) );
  slPrint( "Mixer Number   :", slLocate( 4, 15 ) );

  slPrint( "Total Volume   :", slLocate( 4, 17 ) );
}

/*
   Reset All Parameters and Sound system.
*/
void Reset( SeqStat *seq, Uint8 num ) {
  Uint8 i;

  slSoundAllOff();

  sndstat.DSP         = 0;
  sndstat.Mixer       = 0;
  sndstat.TotalVolume = 15;
  sndstat.MaxBank     = 0;
  sndstat.MaxDSP      = 0;
  sndstat.AreaMap     = 0;

  for ( i = 0 ; i < num ; ++i ) {
    ( seq + i )->Bank   = 0;
    ( seq + i )->Song   = 0;
    ( seq + i )->Pan    = 0;
    ( seq + i )->Volume = 127;
    ( seq + i )->Tempo  = 0;
  }

  slDSPOff();
  slDMACopy( seq1, ( void * )SEQ_START, seq1_size );
  slWaitSound( slSndMapChange( sndstat.AreaMap ) );
  TransferEnd();
  slSndVolume( sndstat.TotalVolume << 3 );
  MapAnalyze( &sndstat );
}

/*
   Change Tone bank number.
*/
void ChgBank( SeqStat *seq, AllStat *stat, Uint16 pad, Uint8 num ) {
  if ( !( pad & PER_DGT_TA ) ) {
    if ( num != 0 ) {
      slSoundRequest( "bbbb", SND_SEQ_START, num, seq->Bank, seq->Song, 10 );
      slSequenceFade( num, seq->Volume, 0 );
      slSequencePan( num, seq->Pan );
    } else {
      slBGMOn( ( seq->Bank << 8 ) | seq->Song, 0, seq->Volume, 0 );
    }
  }
  if ( !( pad & PER_DGT_TB ) ) {
    if ( num != 0 ) {
      slSequenceOff( num );
    } else {
      slBGMOff();
    }
  }
  if ( !( pad & PER_DGT_KL ) ) {
    seq->Bank = ( seq->Bank + stat->MaxBank - 1 ) % stat->MaxBank;
  }
  if ( !( pad & PER_DGT_KR ) ) {
    seq->Bank = ( seq->Bank + stat->MaxBank + 1 ) % stat->MaxBank;
  }
}

/*
   Change song number.
*/
void ChgSong( SeqStat *seq, AllStat *stat, Uint16 pad, Uint8 num ) {
  if ( !( pad & PER_DGT_TA ) ) {
    if ( num != 0 ) {
      slSoundRequest( "bbbb", SND_SEQ_START, num, seq->Bank, seq->Song, 10 );
      slSequenceFade( num, seq->Volume, 0 );
      slSequencePan( num, seq->Pan );
    } else {
      slBGMOn( ( seq->Bank << 8 ) | seq->Song, 0, seq->Volume, 0 );
    }
  }
  if ( !( pad & PER_DGT_TB ) ) {
    if ( num != 0 ) {
      slSequenceOff( num );
    } else {
      slBGMOff();
    }
  }
  if ( !( pad & PER_DGT_KL ) ) {
    //    seq->Bank = ( seq.Bank + stat.MaxBank - 1 ) % stat.MaxBank;
    ++seq->Song;
  }
  if ( !( pad & PER_DGT_KR ) ) {
    //    seq->Bank = ( seq.Bank + stat.MaxBank + 1 ) % stat.MaxBank;
    --seq->Song;
  }
}

/*
   Change sequence volume.
*/
void ChgSequenceVolume( SeqStat *seq, AllStat *stat, Uint16 pad, Uint8 num ) {
  if ( !( pad & PER_DGT_KL ) ) {
    seq->Volume = ( seq->Volume + 128 - 1 ) % 128;
    if ( num != 0 ) {
      slSequenceFade( num, seq->Volume,  0 );
    } else {
      slBGMFade( seq->Volume, 0 );
    }
  }
  if ( !( pad & PER_DGT_KR ) ) {
    seq->Volume = ( seq->Volume + 128 + 1 ) % 128;
    if ( num != 0 ) {
      slSequenceFade( num, seq->Volume,  0 );
    } else {
      slBGMFade( seq->Volume, 0 );
    }
  }
}

/*
   Change Sequence Tempo.
*/
void ChgSequenceTempo( SeqStat *seq, AllStat *stat, Uint16 pad, Uint8 num ) {
  static Sint16 unit = 1;

  if ( !( pad & PER_DGT_TA ) ) {
    unit = 0x1;
    seq->Tempo = 0;
    if ( num != 0 ) {
      slSequenceTempo( num, seq->Tempo );
    } else {
      slBGMTempo( seq->Tempo );
    }
  }
  if ( !( pad & PER_DGT_TB ) ) {
    unit = 0x10;
  }
  if ( !( pad & PER_DGT_TC ) ) {
    unit = 0x100;
  }  
  if ( !( pad & PER_DGT_TX ) ) {
    unit = 0x8;
  }
  if ( !( pad & PER_DGT_TY ) ) {
    unit = 0x80;
  }
  if ( !( pad & PER_DGT_TZ ) ) {
    unit = 0x800;
  }  
  if ( !( pad & PER_DGT_KL ) ) {
    if ( seq->Tempo > -32767 + unit ) seq->Tempo -= unit;
    else seq->Tempo = -32768;
    if ( num != 0 ) {
      slSequenceTempo( num, seq->Tempo );
    } else {
      slBGMTempo( seq->Tempo );
    }
  }
  if ( !( pad & PER_DGT_KR ) ) {
    if ( seq->Tempo < 32766 - unit ) seq->Tempo += unit;
    else seq->Tempo = 32767;
    if ( num != 0 ) {
      slSequenceTempo( num, seq->Tempo );
    } else {
      slBGMTempo( seq->Tempo );
    }
  }
}

/*
   Change Sequence Pan.
*/
void ChgSequencePan( SeqStat *seq, AllStat *stat, Uint16 pad, Uint8 num ) {
  if ( !( pad & PER_DGT_KL ) ) {
    if ( seq->Pan > -119 ) seq->Pan -= 8;
    else seq->Pan = -127;
    if ( num != 0 ) {
      slSequencePan( num, seq->Pan );
    } else {
      /* Equal slBGMPan( Sint8 pan ) */
      slSoundRequest( "bb", SND_SEQ_PAN, num, ( seq->Pan / 2 ) + 0x40 );
    }
  }
  if ( !( pad & PER_DGT_KR ) ) {
    if ( seq->Pan < 119 ) seq->Pan += 8;
    else seq->Pan = 127;
    if ( num != 0 ) {
      slSequencePan( num, seq->Pan );
    } else {
      /* Equal slBGMPan( Sint8 pan ) */
      slSoundRequest( "bb", SND_SEQ_PAN, num, ( seq->Pan / 2 ) + 0x40 ); 
    }
  }
  if ( !( pad & PER_DGT_TA ) ) {
    seq->Pan = 0;
  }
}

/*
   Change DSP Program number.
*/
void ChgDSP( SeqStat *seq, AllStat *stat, Uint16 pad, Uint8 num ) {
  if ( !( pad & PER_DGT_TA ) ) {
    if ( stat->DSP == 0 ) {
      slWaitSound( ( void * )slDSPOff() );
    } else {
      slSndEffect( stat->DSP - 1 );
      slSndMixChange( seq->Bank, stat->Mixer );
    }
  }
  if ( !( pad & PER_DGT_KL ) ) {
    stat->DSP = ( stat->DSP + stat->MaxDSP + 1 - 1 ) % ( stat->MaxDSP + 1 );
  }
  if ( !( pad & PER_DGT_KR ) ) {
    stat->DSP = ( stat->DSP + stat->MaxDSP + 1 + 1 ) % ( stat->MaxDSP + 1 );
  }
}

/*
   Change tone bank mixer.
*/
void ChgMixer( SeqStat *seq, AllStat *stat, Uint16 pad, Uint8 num ) {
  if ( !( pad & PER_DGT_KL ) ) {
    //    stat.DSP = ( stat.DSP + stat.MaxDSP - 1 ) % stat.MaxDSP;
    ++stat->Mixer;
  }
  if ( !( pad & PER_DGT_KR ) ) {
    //    stat.DSP = ( stat.DSP + stat.MaxDSP + 1 ) % stat.MaxDSP;
    --stat->Mixer;
  }
  if ( !( pad & PER_DGT_TA ) ) {
    slSndMixChange( seq->Bank, stat->Mixer );
  }
}

/*
   Change Total Volume.
*/
void ChgTotalVolume( SeqStat *seq, AllStat *stat, Uint16 pad, Uint8 num ) {
  if ( !( pad & PER_DGT_KL ) ) {
    stat->TotalVolume = ( stat->TotalVolume + 16 - 1 ) % 16;
    slSndVolume( stat->TotalVolume << 3 );
  }
  if ( !( pad & PER_DGT_KR ) ) {
    stat->TotalVolume = ( stat->TotalVolume + 16 + 1 ) % 16;
    slSndVolume( stat->TotalVolume << 3 );
  }
}
  
void ss_main() {
  Uint16  pad;
  SeqStat stat[ 8 ];
  Uint8   num = 0;
  Uint8   pos = 0;
  Uint8   cur[] = { 7, 8, 10, 11, 12, 14, 15, 17 };
  void ( *efect )( SeqStat *, AllStat *, Uint16, Sint16 );
  void *func[] = {
    ChgBank,
    ChgSong,
    ChgSequenceVolume,
    ChgSequenceTempo,
    ChgSequencePan,
    ChgDSP,
    ChgMixer,
    ChgTotalVolume
  };

  slInitSystem( TV_320x240, NULL, 3 );
  slPrint( "Sequence Test", slLocate( 10, 2 ) );
  slPrint( ">", slLocate( 2, cur[ pos ] ) );

  /* Initialize Sound */
  slInitSound( sddrvstsk, sddrvsize, map, map_size );
  Reset( stat, 8 );

  while( -1 ) {
    pad  = Smpc_Peripheral[ 0 ].push;

    if ( !( pad & PER_DGT_ST ) ) {
      slPrint( " ", slLocate( 2, cur[ pos ] ) );
      pos = 0;
      slPrint( ">", slLocate( 2, cur[ pos ] ) );
      Reset( stat, 8 );
    }
    if ( !( pad & PER_DGT_TL ) ) {
      sndstat.AreaMap = 0;
      slDSPOff();
      slSoundAllOff();
      slDMACopy( seq1, ( void * )SEQ_START, seq1_size );
      slWaitSound( slSndMapChange( sndstat.AreaMap ) );
      TransferEnd();
      MapAnalyze( &sndstat );
      slSndMixChange( stat[ num ].Bank, sndstat.Mixer );
    }
    if ( !( pad & PER_DGT_TR ) ) {
      sndstat.AreaMap = 1;
      slDSPOff();
      slSoundAllOff();
      slDMACopy( seq2, ( void * )SEQ_START, seq2_size );
      slWaitSound( slSndMapChange( sndstat.AreaMap ) );
      TransferEnd();
      MapAnalyze( &sndstat );
      slSndMixChange( stat[ num ].Bank, sndstat.Mixer );
    }
    if ( !( pad & PER_DGT_KU ) ) {
      slPrint( " ", slLocate( 2, cur[ pos ] ) );
      pos = ( pos + sizeof( cur ) - 1 ) % sizeof( cur );
      slPrint( ">", slLocate( 2, cur[ pos ] ) );
    }
    if ( !( pad & PER_DGT_KD ) ) {
      slPrint( " ", slLocate( 2, cur[ pos ] ) );
      pos = ( pos + sizeof( cur ) + 1 ) % sizeof( cur );
      slPrint( ">", slLocate( 2, cur[ pos ] ) );
    }

    /* Service routine */
    efect = func[ pos ];
    ( *efect )( &stat[ num ], &sndstat, pad, num );

    MakeScreen( &stat[ num ], &sndstat );

    SynchCount = SynchConst;
    slSynch();
  }
}

