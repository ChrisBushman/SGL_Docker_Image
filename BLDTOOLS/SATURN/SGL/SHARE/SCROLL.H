#include "sgl.h"

#ifndef __SCROLL_H__
#define __SCROLL_H__

#define COL_RAM 0x25f00000

#define COL_TRNS 0x01
#define CEL_TRNS 0x02
#define MAP_TRNS 0x04
#define NBG0     0x00
#define NBG1     0x08
#define NBG2     0x10
#define NBG3     0x18

typedef struct scroll {
  Uint16 CMode;         /* Colour mode for Scroll.                          */
  void   *PalleteSrcs;  /* Pallete source data address.                     */
  Uint32 PalleteSize;   /* Pallete data size.                               */
  Uint32 PalleteOffset; /* Pallete data offset from Color RAM( 0x25f00000 ) */
  void   *CellSrcs;     /* Source address for Cel data.                     */
  Uint32 CellOffset;    /* Cell data offset from VDP2 VRAM ( 0x25e00000 )   */
  Uint32 CellSize;      /* Cell data size.                                  */
  void   *MapSrcs;      /* Map data source address.                         */
  Uint32 MapOffset;     /* Map data offset from VDP2 VRAM ( 0x25e00000 )    */
  Uint16 MapSize;       /* Pattern Name Data size.                          */
  Uint16 MapHSize;      /* Map data Horizontal size.                        */
  Uint16 DATA_TYPE;     /* Scroll data type.                                */
  Uint16 CHAR_SIZE;     /* Character size unit by cell.                     */
  Uint8  FLAGS;         /* Which function cancel?                           */
  Uint8  PLANE_SIZE;    /* Plane size for NBG                               */
} SCROLL;

#endif
