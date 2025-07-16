/*
 * Copyright (c) 1995 Cross Products Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the Cross Products Ltd, 23 The Calls, Leeds, LS2 7EH UK.  The
 * name of the company may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * examp.cpp	9/01/96
 *
 * Authors:: Gary Ellison & Tam Roberts
 */

//
// Basic C program for use with SNBUGSAT
//
// The progam is aimed at demonstrating some of the C support
// within the SNBUGSAT debugger.
//

//#include	<sgl.h>
//#include	<ss_scrol.h>
//#include	<ss_akira.h>
#include	<string.h>
#include	<stdio.h>

struct der_struct 
{
	char spzName[50];
	int		m_nXInc;
	int		m_nYInc;
	int		m_nXPos;
	int		m_nYPos;

	int		m_nOldXPos;
	int		m_nOldYPos;

	float	m_fAngle;
};

//typedef struct der_struct Derstruct;

void structDisplay(struct der_struct *s);
void structMove(struct der_struct *s);

//
// block of memory for crosslib heap
//

//static char mem_space[64*1024];

//
// Main routine
//

int main(void)
{
	// initialse a tempory heap for the fileserver
	//initialise_heap((int)mem_space, sizeof(mem_space));
	struct der_struct myStruct;
	strcpy(myStruct.spzName, "Hello World");
	myStruct.m_nXInc = 1;
	myStruct.m_nYInc = 1;
	myStruct.m_nXPos = 1;
	myStruct.m_nYPos = 1;
	myStruct.m_nOldXPos = 1;
	myStruct.m_nOldYPos = 1;
	myStruct.m_fAngle = 100.01;
	//struct der_struct *dptr= %myStruct;
	// initialise the SGL graphics system
	//slInitSystem(TV_320x224, NULL, 1);


	// loop forever
	while (-1)
		{
		// display the string
		structDisplay(&myStruct);

		// move it around the screen
		structMove(&myStruct);

		// swap the screen buffers
		//slSynch();
		}
}

void structDisplay(struct der_struct *s)
{
	printf("Dev Class:: %s\n", s->spzName);
	printf("Base Class:: pos = (%d, %d) angle = %f\n",
	       s->m_nXPos, s->m_nYPos, s->m_fAngle);
	//slPrint("                ", slLocate(s->m_nOldXPos,s->m_nOldYPos));
	//slPrint(s->spzName,slLocate(s->m_nXPos,s->m_nYPos));
}

void structMove(struct der_struct *s)
{
	// save the current pos
	s->m_nOldXPos = s->m_nXPos;
	s->m_nOldYPos = s->m_nYPos;

	// update the position
	s->m_nXPos += s->m_nXInc;
	s->m_nYPos += s->m_nYInc;

	// apply limits to movement
	if (s->m_nXPos >= 30)
		s->m_nXInc  = -1;
	if (s->m_nXPos <= 1)
		s->m_nXInc  = 1;

	if (s->m_nYPos >= 24)
		s->m_nYInc  = -1;
	if (s->m_nYPos <= 1)
		s->m_nYInc  = 1;
}
