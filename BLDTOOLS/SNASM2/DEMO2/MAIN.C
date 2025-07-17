/* this is a small test program */

#include "includes\portab.h"
#include "includes\memmap.h"
#include <stdio.h>
#include "includes\structs.h"
#include "includes\matrix.h"

extern	volatile Uint32 pad0;
extern	volatile Uint32 pad1;

extern	Sint32 camerax;
extern	Sint32 cameray;
extern	Sint32 cameraz;

extern	volatile Sint32 vcounter;
extern	Sint32 oldvcounter;
extern	volatile Uint32 drawflag;
extern	volatile Sint32 drawcounter;
extern	volatile Sint32 mathcounter;
extern	Sint32 numsprites;

extern	object cube_object1;
extern	object sonic_object1;
extern	object sonic_object2;
extern	object sonic_object3;

extern	Uint16 temppoints[];
extern	Uint16 *lastsprite;
extern	Uint16 spritebuf[];
extern	Uint16 spritelist[];

extern	Uint8 redsurface[];
extern	Uint8 greensurface[];
extern	Uint8 bluesurface[];
extern	Uint8 whitesurface[];
extern	Uint8 blacksurface[];

extern	Uint16 pueblo[];
extern	Uint8 redbox[];
extern	Uint8 bluebox[];
extern	Uint8 greenbox[];
extern	Uint8 goldbox[];
extern	Uint8 numbers[];
extern	Uint8 grass[];

extern	Uint32 sonic_faces[];

Uint8		hexstr[16];


#define	SPINSPEED	0x00100000
#define	ZOOMSPEED	(1*65536)

#define	VDP1RAM		((Uint16 *)0x25C00000)			/* VDP1 VRAM */

#define	SPRITEBASE		(65536*2)

#define	FIRSTSPRITE		4	/* number of the first unused sprite def in vram */

#define	BOXSIZE			(((16*16)*2)/8)
#define	NUMBERSIZE			(((32*32)*2)/8)
#define	redboxADDR		(SPRITEBASE/8)
#define	greenboxADDR	(redboxADDR+BOXSIZE)
#define	blueboxADDR		(greenboxADDR+BOXSIZE)
#define	goldboxADDR		(blueboxADDR+BOXSIZE)
#define	numberADDR		(goldboxADDR+BOXSIZE)
#define	grassADDR		(numberADDR+(NUMBERSIZE * 6))

Sint32		camera_pitch = 0;
Sint32		camera_yaw = 0;
Sint32		camera_bank = 0;

void init_irqs(void);
void init_vdp1(void);

void setvdp2mode(Uint32 mode);
void storepalette(Uint16 *pal, Uint32 numcolors);
void storebitmap(Uint8 *bitmap, Uint32 destaddr, Uint32 maptype);
void blit256(Uint32 xpos, Uint32 ypos, Uint8 *bitmap);
void blit16mil(Uint32 xpos, Uint32 ypos, Uint8 *bitmap);
void storesprites(Uint32 numsprites, Uint16 *spritelist, Uint16 *vramaddr);
void ItoH(Uint8 *string, Uint32 num, Uint32 numdigits);
void Gputs(Uint8 *string);

void addobject(object *object, Uint16 *points);

/*some address defs*/

volatile Uint8 *SMPC_COM = (Uint8 *) 0x2010001f; /*SMPC command reg*/
volatile Uint8 *SMPC_RET = (Uint8 *) 0x2010005f; /*SMPC result reg*/
volatile Uint8 *SMPC_SF = (Uint8 *) 0x20100063; /*SMPC status flag*/

const Uint8 SMPC_SSHON = 0x02;
const Uint8 SMPC_SSHOFF = 0x03;
void **slaveshentry = (void **)0x06000250;



/*second CPU running*/

void readjoy(void)
{
	static Uint32 *angle = &cube_object1.pitch;

	if(pad0 != NOPAD)
	{
		if(pad0 & PADLEFT)
		{
			camera_yaw += SPINSPEED;
			camera_yaw &= 0x1fffffff;
		}

		if(pad0 & PADRIGHT)
		{
			camera_yaw -= SPINSPEED;
			camera_yaw &= 0x1fffffff;
		}

		if(pad0 & PADUP)
		{

			if((camera_yaw > 0x07ffffff) && (camera_yaw < 0x18000000))
			{
				if(camera_yaw < 0x10000000)
					camerax += ((ZOOMSPEED * mycos(camera_yaw)) >> 16);
				else				
					camerax += ((ZOOMSPEED * mycos(camera_yaw-0x10000000)) >> 16);

				cameraz += ZOOMSPEED - ((ZOOMSPEED * mysin(camera_yaw)) >> 16);
			}
			else
			{
				if(camera_yaw < 0x10000000)
					camerax -= ((ZOOMSPEED * mycos(camera_yaw)) >> 16);
				else				
					camerax -= ((ZOOMSPEED * mycos(camera_yaw-0x10000000)) >> 16);

				cameraz -= ZOOMSPEED - ((ZOOMSPEED * mysin(camera_yaw)) >> 16);
			}
		
		}

		if(pad0 & PADDOWN)
		{
			if((camera_yaw > 0x07ffffff) && (camera_yaw < 0x18000000))
			{
				if(camera_yaw < 0x10000000)
					camerax -= ((ZOOMSPEED * mycos(camera_yaw)) >> 16);
				else				
					camerax -= ((ZOOMSPEED * mycos(camera_yaw-0x10000000)) >> 16);

				cameraz -= ZOOMSPEED - ((ZOOMSPEED * mysin(camera_yaw)) >> 16);
			}
			else
			{
				if(camera_yaw < 0x10000000)
					camerax += ((ZOOMSPEED * mycos(camera_yaw)) >> 16);
				else				
					camerax += ((ZOOMSPEED * mycos(camera_yaw-0x10000000)) >> 16);

				cameraz += ZOOMSPEED - ((ZOOMSPEED * mysin(camera_yaw)) >> 16);
			}

		}


		if(pad0 & PADSTART)
		{
			camera_pitch = 0;
			camera_yaw = 0;
			camera_bank = 0;
			camerax = 0;
			cameraz = 0;
		}

	}
}


void cmainloop(void)
{
	while(1)
	{
		mathcounter = vcounter;

		lastsprite = spritebuf;

		numsprites = 0;


		push_matrix();
		{

			rotate_z(-(camera_bank));
			rotate_x(-(camera_pitch));
			rotate_y(-(camera_yaw)); 

			translate(camerax, cameray, cameraz);


			push_matrix();
			{

				translate(cube_object1.xpos, cube_object1.ypos, cube_object1.zpos);

				rotate_y(cube_object1.yaw); 
				rotate_x(cube_object1.pitch);
				rotate_z(cube_object1.bank);

				addobject(&cube_object1, temppoints);

			}

			pop_matrix();

			push_matrix();
			{

				translate(sonic_object1.xpos, sonic_object1.ypos, sonic_object1.zpos);

				rotate_y(sonic_object1.yaw); 
				rotate_x(sonic_object1.pitch);
				rotate_z(sonic_object1.bank);

				addobject(&sonic_object1, temppoints);

			}

			pop_matrix();
		}

		pop_matrix();


		numsprites++;

		mathcounter = vcounter - mathcounter;

		waitvdp1();

		TextColor(0x0001, 0xFFFF);

		ItoH(hexstr, vcounter - oldvcounter, 2);
		TextXY(32, 16);
		Gputs(hexstr);

		oldvcounter = vcounter;

		ItoH(hexstr, mathcounter, 2);
		TextXY(32, 32);
		Gputs(hexstr);

		ItoH(hexstr, drawcounter, 2);
		TextXY(32, 48);
		Gputs(hexstr);

		ItoH(hexstr, numsprites-1, 4);
		TextXY(32, 64);
		Gputs(hexstr);



		drawflag = 1;

		waitvbl();

		store_sorted_sprites();

		readjoy();

		cube_object1.pitch += SPINSPEED;
		cube_object1.yaw -= SPINSPEED;

		sonic_object1.yaw -= SPINSPEED;

	}

}

void Smain(void)
{
	while(1);
}


void main(void)
{

	Uint32 i;

	/* startup the second CPU*/

	while((*SMPC_SF &0x01)==0x01);
	*SMPC_SF = 1;
	*SMPC_COM = SMPC_SSHOFF;
	
	while((*SMPC_SF&0x01)==0x01);
	
	for(i=0;i<1000;i++);

	*(void **) slaveshentry = (void *)Smain; /*set the PC address for the slave*/

	*SMPC_SF =1;
	*SMPC_COM = SMPC_SSHON;

	while((*SMPC_SF &0x01)==0x01);


	setvdp2mode(12);
	
	storepalette(pueblo+2, 256);

	blit256(0, 0, (Uint8 *)pueblo);

	init_irqs();


	storebitmap(redbox, redboxADDR*8, 0);
	storebitmap(greenbox, greenboxADDR*8, 0);
	storebitmap(bluebox, blueboxADDR*8, 0);
	storebitmap(goldbox, goldboxADDR*8, 0);
	storebitmap(numbers, numberADDR*8, 0);
	storebitmap(grass, grassADDR*8, 0);

	init_vdp1();
	
	lastsprite = spritebuf;

	storesprites(5, spritelist, VDP1RAM);

	vcounter = 0;

	initial_matrix();

	frontclip = 0x00400000;

	cameraz = 0x98C778;

	cmainloop();

}
