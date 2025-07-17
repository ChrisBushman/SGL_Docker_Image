/* data structtures that I need for the 3d package */


typedef struct 
{
	Uint16	numpoints;		
	Uint16	numfaces;
	Uint32	flags;
	Uint32	pointlist[0];

} shape;

typedef struct 
{
	Uint16	type;				/* type of object */
	Uint16	visible;			/* visible flag */
	shape		*shapeptr;		/* pointer to the shape */
	Uint32	xpos;				/* x position in 3 space */
	Uint32	ypos;				/* y position in 3 space */
	Uint32	zpos;				/* z position in 3 space */
	Uint32	pitch;			/* pitch angle */
	Uint32	yaw;				/* yaw angle */
	Uint32	bank;				/* bank angle */

} object;




/******************************************************************
*		surface data structure
******************************************************************/

typedef struct 
{
	Uint16	type;
	Uint16	control;
	Uint16	drawmode;
	Uint16	color;
	Uint16	charaddr;
	Uint16	charsize;
	Uint16	shade;
	Uint16	bitmap;

} surface;



/******************************************************************
		face data structure
******************************************************************/

typedef struct 
{
	Uint16	p1;
	Uint16	p2;
	Uint16	p3;
	Uint16	p4;
	surface	*surface;

} face;


/*****************************************************************
*		sprite data structure
******************************************************************/

typedef struct 
{
	
	Uint16	type;				/* control 	*/
	Uint16	control;			/* link     */
	Uint16	drawmode;		/* drawMode */
	Uint16	color;			/* color    */
	Uint16	charaddr;		/* charAddr */
	Uint16	charsize;		/* charSize */
	Uint16	pointA[2];		/* ax, ay   */
	Uint16	pointB[2];		/* bx, by   */
	Uint16	pointC[2];		/* cx, cy   */
	Uint16	pointD[2];		/* dx, dy   */
	Uint16	shade;			/* grshAddr */
	Uint16	dummy;			/* dummy		*/

} sprite;
