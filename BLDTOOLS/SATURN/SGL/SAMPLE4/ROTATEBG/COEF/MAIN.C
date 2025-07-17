//
//	RBG0 KTable Sample
//		åWêîÉeÅ[ÉuÉãÇÃçÏê¨(1 Byte Mode)
//
//				Programmed by Matsuda Hakuroh
//				1997/06/20~
//

#include	"sgl.h"
#include    "./map/flower.h"

#define		BACK_COL_ADR		( VDP2_VRAM_B1 + 0x1fffe )
#define		RBG0_COL_ADR		VDP2_COLRAM
#define		RBG0_CEL_ADR		VDP2_VRAM_A1
#define		RBG0_MAP_ADR		VDP2_VRAM_A0
#define		RBG0_PAR_ADR		( VDP2_VRAM_B1 + 0x1fe00 )
#define		RBG0_KTABLE_ADR		( VDP2_VRAM_B0 )

#define WIDTH 320
#define HEIGHT 224
#define R 80
#define RX 160
#define RY 120

void Map2VRAM( Uint16 *Map_Data , void *Map_Adr , Uint16 suuj , Uint16 suui , Uint16 palnum ,Uint32 mapoff) ;
void Cel2VRAM( Uint8 *Cel_Data , void *Cell_Adr , Uint32 suu ) ;
void Pal2CRAM( Uint16 *Pal_Data , void *Col_Adr , Uint32 suu ) ;

void MakeKtable (void * lpAdr, void * lpRpara)
{
	Uint16 * lpDest = (Uint16 *)lpAdr ;
	int i, j, k ;
	FIXED fR ;
	
	for (i=0; i<HEIGHT; i ++)
	{
		for (j=0; j<WIDTH; j +=2)
		{
			if (((RX-j)*(RX-j)+(RY-i)*(RY-i))<R*R)
			{
				fR = toFIXED(((RX-j)*(RX-j)+(RY-i)*(RY-i)));
				fR = slDivFX(toFIXED(R*R), fR) ;
				fR = slMulFX (toFIXED(1024-128), fR) ;
				k = (fR>>16) + 128 ;
			}
			else
			{
				k = 1 << 10 ;
			}
			*lpDest++ = (Uint16)(k)&0x7fff ;
		}
	}
}
#define POSADD toFIXED (1.0)
void ss_main()
{
	Uint16 data ;
	static ANGLE ang ;
	static FIXED fX, fY ;
	
	slInitSystem (TV_320x224, NULL, 1);
	
	slTVOff () ;

	slColRAMMode(CRM16_2048);
	slBack1ColSet((void *)BACK_COL_ADR , 0);

	slRparaInitSet((void *)RBG0_PAR_ADR);
	slCharRbg0(S2D_COLTYPE , S2D_CHRSIZE);
	slPageRbg0((void *)RBG0_CEL_ADR , (void *)RBG0_COL_ADR , S2D_PNBSIZE);
	slPlaneRA(PL_SIZE_1x1);
	sl1MapRA((void *)RBG0_MAP_ADR);
	slOverRA(2);
	
	MakeKtable ((void*)RBG0_KTABLE_ADR,(void*)RBG0_PAR_ADR) ;
	slKtableRA ((void*)RBG0_KTABLE_ADR, K_ON|K_1WORD|K_MODE0|K_DOT|K_FIX) ;

	Cel2VRAM((void*)S2D_CEL, (void*)RBG0_CEL_ADR, (Uint32)S2D_CELSIZE);
	Map2VRAM((void*)S2D_MAP, (void*)RBG0_MAP_ADR, MAP_HSIZE*64, MAP_VSIZE*64, 0, ((RBG0_CEL_ADR-VDP2_VRAM_A0)/32)) ;
	Pal2CRAM((void*)S2D_PAL, (void*)RBG0_COL_ADR, (Uint32)S2D_PALSIZE);

	slScrAutoDisp( NBG0OFF | NBG1OFF | NBG2OFF | NBG3OFF | RBG0ON);

	slTVOn () ;
	ang = 0 ;
	fX = toFIXED (160.0) ;
	fY = toFIXED (112.0) ;
	while(-1){

		data = Smpc_Peripheral [0].data ;
		if ((data & PER_DGT_TL)==0)
		{
			ang += 0xff*5 ;
		}
		else if ((data & PER_DGT_TR)==0)
		{
			ang -= 0xff*5 ;
		}
		if ((data & PER_DGT_KL)==0)
		{
			fX += POSADD ;
		}
		else if ((data & PER_DGT_KR)==0)
		{
			fX -= POSADD ;
		}
		if ((data & PER_DGT_KU)==0)
		{
			fY += POSADD ;
		}
		else if ((data & PER_DGT_KD)==0)
		{
			fY -= POSADD ;
		}
		
		slLookR (fX, fY) ;
		slZrotR (ang) ;

		slSynch();
		*(Uint16*)(RBG0_PAR_ADR+0x58)=160 ; //320;
		*(Uint32*)(RBG0_PAR_ADR+0x5c)=0x8000;
	}
}

void Cel2VRAM( Uint8 *Cel_Data , void *Cell_Adr , Uint32 suu )
{
	Uint32 i;
	Uint8 *VRAM;

	VRAM = (Uint8 *)Cell_Adr;

	for( i = 0; i < suu; i++ )
		*(VRAM++) = *(Cel_Data++);
}

void Map2VRAM( Uint16 *Map_Data , void *Map_Adr , Uint16 suuj , Uint16 suui , Uint16 palnum ,Uint32 mapoff)
{
	//for 2 Word Mode
	Uint16 i , j;
	Uint32 paloff;
	Uint32 *VRAM;
	Uint32 *Src ;

	paloff = ((Uint32)palnum) << 16;
	VRAM = (Uint32 *)Map_Adr;
	Src = (Uint32*)Map_Data ;

	for( i = 0; i < suui; i++ ) {
		for( j = 0; j < suuj; j++ ) {
			*VRAM++ = ((*Src++)+mapoff + paloff);
		}
		//VRAM += (64 - suuj);
	}
}

void Pal2CRAM( Uint16 *Pal_Data , void *Col_Adr , Uint32 suu )
{
	Uint16 i;
	Uint16 *VRAM;

	VRAM = (Uint16 *)Col_Adr;

	for( i = 0; i < suu; i++ )
		*(VRAM++) = *(Pal_Data++);
}
