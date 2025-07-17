#include "sprite.h"

#define CGTop    0x10000
#define VDP1VRAM 0x25c00000

#define Sonic_Width   64
#define Sonic_Height  96

#define SonicPalOff ( VDP2_COLRAM )
#define SonicLUTOff ( VDP1VRAM + CGTop + 0xb000 )

#define SonicTxrOff  ( VDP1VRAM + CGTop )
#define ShadowTxrOff ( VDP1VRAM + CGTop + sizeof( sonic_tex ) )
