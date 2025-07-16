/********************************************************************
													H.Sano
********************************************************************/

#include	"sgl.h"
#include	"sega_sys.h"
#include	"sega_sys.h"
#include	"sglcolli.h"

#define	CAMERA_ZDEF	-100.0
#define	OFFSET_SCL	20.0
#define	pol_map_x	32
#define	pol_map_y	32

typedef struct{
	FIXED	cpos[XYZ];
	FIXED	ctarget[XYZ];
	ANGLE	cangle[XYZ];
	FIXED	pos[XYZ];
	ANGLE	ang[XYZ];
	FIXED	scl[XYZ];
}Sega3D, *Sega3DPtr;

extern	Uint8	tuti_cel[];
extern	Uint16	tuti_map[];
extern	Uint16	tuti_pal[];

extern	PDATA	PD_BOX[];
extern	PDATA	*map_data[];
extern	CDATA 	*collison_data[];

#define		RBG0RB_CEL_ADR			(VDP2_VRAM_A0            )
#define		RBG0RB_MAP_ADR			(VDP2_VRAM_B0            )
#define		RBG0RB_COL_ADR			(VDP2_COLRAM    + 0x00200)

#define		RBG0RA_CEL_ADR			(RBG0RB_CEL_ADR + 0x06e80)
#define		RBG0RA_MAP_ADR			(RBG0RB_MAP_ADR + 0x02000)
#define		RBG0RA_COL_ADR			(RBG0RB_COL_ADR + 0x00200)

#define		RBG0_KTB_ADR			(VDP2_VRAM_A1            )
#define		RBG0_PRA_ADR			(VDP2_VRAM_A1   + 0x1fe00)
#define		RBG0_PRB_ADR			(RBG0_PRA_ADR   + 0x00080)
#define		BACK_COL_ADR			(VDP2_VRAM_A1   + 0x1fffe)

static void init_matrix(FIXED *, ANGLE *, FIXED *);

void	main_came(EVENT *);
void	main_pad(EVENT *);
void	main_Pmap(EVENT *);
void	Collison_put(FIXED,FIXED,FIXED);

extern	ROTSCROLL RotScrParA;
extern	TEXTURE	tex_sample[];
extern	PICTURE	pic_sample[];

FIXED	c_pos[XYZ];
FIXED	t_pos[XYZ];
FIXED	uu_hight;
ANGLE	t_ang;

#define	GRTBL(r,g,b)	(((b&0x1f)<<10) | ((g&0x1f)<<5) | (r&0x1f) )
static	Uint16	DepthData[32] = {
	GRTBL( 16, 16, 16 ),
	GRTBL( 16, 16, 16 ),
	GRTBL( 16, 16, 16 ),
	GRTBL( 16, 16, 16 ),
	GRTBL( 16, 16, 16 ),
	GRTBL( 16, 16, 16 ),
	GRTBL( 16, 16, 16 ),
	GRTBL( 16, 16, 16 ),
	GRTBL( 16, 16, 16 ),
	GRTBL( 16, 16, 16 ),
	GRTBL( 16, 16, 16 ),
	GRTBL( 16, 16, 16 ),
	GRTBL( 16, 16, 16 ),
	GRTBL( 16, 16, 16 ),
	GRTBL( 16, 16, 16 ),
	GRTBL( 16, 16, 16 ),
	GRTBL( 15, 15, 15 ),
	GRTBL( 14, 14, 14 ),
	GRTBL( 13, 13, 13 ),
	GRTBL( 12, 12, 12 ),
	GRTBL( 11, 11, 11 ),
	GRTBL( 10, 10, 10 ),
	GRTBL(  9,  9,  9 ),
	GRTBL(  8,  8,  8 ),
	GRTBL(  7,  7,  7 ),
	GRTBL(  6,  6,  6 ),
	GRTBL(  5,  5,  5 ),
	GRTBL(  4,  4,  4 ),
	GRTBL(  3,  3,  3 ),
	GRTBL(  2,  2,  2 ),
	GRTBL(  1,  1,  1 ),
	GRTBL(  0,  0,  0 ),
};

int		col_hight;
int		col_att;
int		col_grp;

void set_texture(PICTURE *pcptr , Uint32 NbPicture)
{
	TEXTURE *txptr;

	for(; NbPicture-- > 0; pcptr++){
		txptr = tex_sample + pcptr->texno;
		slDMACopy((void *)pcptr->pcsrc,
			(void *)(SpriteVRAM + ((txptr->CGadr) << 3)),
			(Uint32)((txptr->Hsize * txptr->Vsize * 4) >> (pcptr->cmode)));
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
	Uint16 i , j;
	Uint16 paloff;
	Uint16 *VRAM;

	paloff= palnum << 12;
	VRAM = (Uint16 *)Map_Adr;
	

	for( i = 0; i < suui; i++ ) {
		for( j = 0; j < suuj; j++ ) {
			*VRAM++ = (*Map_Data | paloff) + mapoff;
			Map_Data++;
		}
		VRAM += (64 - suuj);
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
void	ss_main(void)
{
	slInitSystem(TV_352x224,tex_sample,2);
	slTVOff();
	slColRAMMode(CRM16_1024);
	slPrint("sample X-Z",slLocate(10,3));
/*------------------------------------------------------*/
	uu_hight	=toFIXED(30);
	set_texture(pic_sample,38);
/*------------------------------------------------------*/
	slTVOn();
/*------------------------------------------------------*/
	t_pos[X]	=toFIXED(459.0);
	t_pos[Y]	=toFIXED(-100.0);
	t_pos[Z]	=toFIXED(716.0);
	t_ang		=DEGtoANG(0.0);
	slZdspLevel(7);
/*----------------------------------------------------------------------*/
	slSetDepthLimit(0,10,5);
	slSetDepthTbl(DepthData,0xf000,32);
/*----------------------------------------------------------------------*/
	slInitEvent();							/*  イベントの初期化		*/
	slSetEvent((void *)main_pad);			/*  パッド入力関係			*/
	slSetEvent((void *)main_came);			/*  カメラ位置指定関係		*/
	slSetEvent((void *)main_Pmap);			/*  ポリゴンマップ表示関係	*/
/*----------------------------------------------------------------------*/
	while(1)
	{
		slExecuteEvent();					/*	イベントの実行			*/
		slSynch();							/*	シンク					*/
	} 
}

/*--------------------------------------*/
/*	カメラ関係							*/
/*--------------------------------------*/
void	main_came(EVENT *evptr)
{
	slUnitMatrix(CURRENT);
	slLookAt(c_pos,t_pos,DEGtoANG(0.0));
}
/*--------------------------------------*/
/*	ポリゴンマップ関係の表示			*/
/*--------------------------------------*/
void	main_Pmap(EVENT *evptr)
{
	int	ax_max,ax_min;
	int	ay_max,ay_min;
	PDATA	*pol;
	Sint32	ax,ay;
	Sint32	my_xx,my_yy;
	
	ax	=pol_map_x-(t_pos[X]>>23)-1;
	ay	=t_pos[Z]>>23;

	ax_min	=ax-6;
	ax_max	=ax+7;
	if(ax_min <  0)ax_min=0;
	if(ax_max >= pol_map_x)ax_max=pol_map_x;
	ay_min	=ay-6;
	ay_max	=ay+7;
	if(ay_min <  0)ay_min=0;
	if(ay_max >= pol_map_y)ay_max=pol_map_y;

	slPushMatrix();
	{
		for(ax = ax_min ; ax < ax_max ; ax++)
		{
			for(ay = ay_min ; ay < ay_max ; ay++)
			{
				pol	=map_data[ax+ay*pol_map_x];
				if(pol->nbPolygon != 0)
				{
					slPutPolygonS(pol);
				}
			}
		}
	}
	slPopMatrix();

	slPushMatrix();
	{
		slTranslate(t_pos[X],t_pos[Y]+uu_hight-toFIXED(12),t_pos[Z]);
		slPutPolygon(PD_BOX);
	}
	slPopMatrix();
}
/*--------------------------------------*/
/*	パッド関係							*/
/*--------------------------------------*/
void	main_pad(EVENT *evptr)
{
	Uint16	data;
	FIXED	ux,uy;
	
	ux	=0;
	uy	=0;
	data = Smpc_Peripheral[0].data;
	if((data & PER_DGT_TB) == 0)
	{
		ux	=8*slSin(t_ang);
		uy	=8*slCos(t_ang);
	}
	if((data & PER_DGT_TY) == 0)
	{
		ux	=-8*slSin(t_ang);
		uy	=-8*slCos(t_ang);
	}
	if((data & PER_DGT_KL) == 0)t_ang	-=DEGtoANG(4.0);
	if((data & PER_DGT_KR) == 0)t_ang	+=DEGtoANG(4.0);

	if((data & PER_DGT_TC) == 0)t_pos[Y]	+=toFIXED(4.0);
	if((data & PER_DGT_TZ) == 0)t_pos[Y]	-=toFIXED(4.0);

	t_pos[X]	+=ux;
	t_pos[Z]	+=uy;
	Collison_put(t_pos[X],t_pos[Z],t_pos[Y]);
	t_pos[Y]	=toFIXED(col_hight)-uu_hight;
	if(col_att != 1)
	{
		t_pos[X]	-=ux;
		t_pos[Z]	-=uy;
	}
	c_pos[X]	=t_pos[X]-slSin(t_ang)*50;
	c_pos[Z]	=t_pos[Z]-slCos(t_ang)*50;
	Collison_put(c_pos[X],c_pos[Z],c_pos[Y]);
	c_pos[Y]	=toFIXED(col_hight-40)-uu_hight;
}
/*-------------------------------------------------*/
/*	コリジョン	高さ抽出							*/
/*-------------------------------------------------*/
void	Collison_put(FIXED x,FIXED y,FIXED z)
{
	FIXED		li;
	FIXED		li_min;
	COLLISON	*GetCol;
	int			GetColNo;
	int			sx,sy,sz;
	int			A,B,C;
	FIXED		poa,pob,poc;
	int			aa_loop,aa;
	int			ax,ay,ux,uy;
	int			ax_min,ax_max;
	int			ay_min,ay_max;
	CDATA		*a_cdata;
	COLLISON	*a_collison;

	ux	=x>>(23);
	uy	=y>>(23);
	
	ax_min	=ux-1;
	ax_max	=ux+2;
	if(ax_min<0)ax_min=0;
	if(ax_max > pol_map_x)ax_max=pol_map_x;
	ay_min	=uy-1;
	ay_max	=uy+2;
	if(ay_min<0)ay_min=0;
	if(ay_max > pol_map_y)ay_max=pol_map_y;

	GetColNo	=100;
	li_min	=toFIXED(7000);
	for(ay = ay_min ; ay < ay_max ; ay++)
	{
		for(ax = ax_min ; ax < ax_max ; ax++)
		{
			a_cdata	=(CDATA *)collison_data[ax+ay*pol_map_x];
			aa_loop	=a_cdata->nbCo;
			if(aa_loop != 0)
			{
				a_collison	=(COLLISON *)a_cdata->cotbl;
				for(aa = 0 ; aa < aa_loop ; aa++)
				{
					sx	=(-a_collison[aa].cen_x+x)>>16;
					sy	=(-a_collison[aa].cen_y+y)>>16;
					sz	=(-a_collison[aa].cen_z+z)>>16;
#if 0
					li	=slSquart(sx*sx+sy*sy+sz*sz);	/*ここは処理が重い*/
#else
					li	=(sx*sx+sy*sy+sz*sz);		/*ここは処理が少し軽い*/
#endif
					if(li_min > li)
					{
						li_min		=li;
						GetCol		=(COLLISON *)a_collison;
						GetColNo	=aa;
					}
				}
			}
		}
	}
	if(GetColNo == 100)
	{
		col_hight		=0;
		col_att			=0;
		col_grp			=0;
	}
	else
	{
		A	=GetCol[GetColNo].norm[X];
		B	=GetCol[GetColNo].norm[Y];
		C	=GetCol[GetColNo].norm[Z];

		poa	=-GetCol[GetColNo].cen_x;
		pob	=-GetCol[GetColNo].cen_y;
		poc	=-GetCol[GetColNo].cen_z;
		if( C != 0)
		{
			col_hight		=-(A*((-x-poa)>>16)+B*((-y-pob)>>16))/C-(poc>>16);
		}
		else
		{
			col_hight		=-poc>>16;
		}
		col_att		=GetCol[GetColNo].att;
		col_grp		=GetCol[GetColNo].gru;
	}
}
