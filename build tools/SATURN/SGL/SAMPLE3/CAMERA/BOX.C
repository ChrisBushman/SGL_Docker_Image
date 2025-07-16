#include "sgl.h"
#include	"sglcolli.h"
#include	"./map/miti.mdl"
#include	"./map/miti.ata"
#include	"./map/tex/tex.txr"

POINT	point_BOX[]={
	POStoFIXED(  12.00, 12.00, 12.00),
	POStoFIXED(  12.00,-12.00, 12.00),
	POStoFIXED( -12.00,-12.00, 12.00),
	POStoFIXED( -12.00, 12.00, 12.00),
	POStoFIXED(  12.00, 12.00,-12.00),
	POStoFIXED(  12.00,-12.00,-12.00),
	POStoFIXED( -12.00,-12.00,-12.00),
	POStoFIXED( -12.00, 12.00,-12.00),
};

POLYGON polygon_BOX[]={
	NORMAL(    0,    0,    1), VERTICES(  0,  1,  2,  3),
	NORMAL(    1,    0,    0), VERTICES(  1,  0,  4,  5),
	NORMAL(    0,   -1,    0), VERTICES(  2,  1,  5,  6),
	NORMAL(   -1,    0,    0), VERTICES(  3,  2,  6,  7),
	NORMAL(    0,    1,    0), VERTICES(  0,  3,  7,  4),
	NORMAL(    0,    0,   -1), VERTICES(  7,  6,  5,  4),
};
ATTR attribute_BOX[]={
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,C_RGB( 7,10,16),No_Gouraud,MESHoff,sprPolygon,No_Option),
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,C_RGB(14,16, 7),No_Gouraud,MESHoff,sprPolygon,No_Option),
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,C_RGB(15,15,17),No_Gouraud,MESHoff,sprPolygon,No_Option),
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,C_RGB(21, 0, 5),No_Gouraud,MESHoff,sprPolygon,No_Option),
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,C_RGB(18,12, 5),No_Gouraud,MESHoff,sprPolygon,No_Option),
	ATTRIBUTE(Single_Plane,SORT_CEN,No_Texture,C_RGB(21, 7,14),No_Gouraud,MESHoff,sprPolygon,No_Option),
};
PDATA PD_BOX = {
	point_BOX,sizeof(point_BOX)/sizeof(POINT),
	polygon_BOX,sizeof(polygon_BOX)/sizeof(POLYGON),
	attribute_BOX
};
