#include "sgl.h"
#include "../../scroll/flower.map"
#include "../../scroll/flower.cel"
#include "../../scroll/flower.pal"
#include "../../scroll/yama.map"
#include "../../scroll/yama.cel"
#include "../../scroll/yama.pal"

#include "scroll.h"

void VDP2DataSet( void ) {
  SCROLL scr;

  scr.CMode         = CRM16_1024;
  scr.DATA_TYPE     = PNB_2WORD;
  scr.CHAR_SIZE     = CHAR_SIZE_2x2 | COL_TYPE_256;
  scr.PLANE_SIZE    = PL_SIZE_2x2;
  scr.MapHSize      = 32;
  scr.PalleteSrcs   = pal_flower;
  scr.PalleteSize   = sizeof( pal_flower );
  scr.PalleteOffset = 0x200;
  scr.CellSrcs      = cel_flower;
  scr.CellOffset    = 0x20000;
  scr.CellSize      = sizeof( cel_flower );
  scr.MapSrcs       = map_flower;
  scr.MapSize       = sizeof( map_flower );
  scr.MapOffset     = 0x8000;
  scr.FLAGS         = COL_TRNS | CEL_TRNS | MAP_TRNS | NBG2;
  ScrollDataSet( scr );

  scr.DATA_TYPE     = PNB_1WORD | CN_12BIT;
  scr.CHAR_SIZE     = CHAR_SIZE_1x1 | COL_TYPE_256;
  scr.PLANE_SIZE    = PL_SIZE_1x1;
  scr.MapHSize      = 32;
  scr.PalleteSrcs   = yama_pal;
  scr.PalleteSize   = sizeof( yama_pal );
  scr.PalleteOffset = 0x400;
  scr.CellSrcs      = yama_cel;
  scr.CellOffset    = 0x10000;
  scr.CellSize      = sizeof( yama_cel );
  scr.MapSrcs       = yama_map;
  scr.MapSize       = sizeof( yama_map );
  scr.MapOffset     = 0x4000;
  scr.FLAGS         = COL_TRNS | CEL_TRNS | MAP_TRNS | NBG1;
  ScrollDataSet( scr );
}
