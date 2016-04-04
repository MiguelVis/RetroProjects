/*	alloc.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Dynamic memory functions.

	Copyright (c) 1999-2015 Miguel I. Garcia Lopez / FloppySoftware, Spain

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 2, or (at your option) any
	later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

	Revisions:

	13 Dec 2000 : Last revision.
	16 Apr 2007 : GPL'd.
	26 Aug 2012 : Changed some things for more speed.
	19 Feb 2015 : Now free() checks if pointer is NULL.

	Public:

	void *malloc(unsigned int size)
	void free(void *)

	Private:

	int xm_gw(int *ptr)
	void xm_pw(int *ptr, int val)

	Notes:

	Each memory block contains following data:

	WORD size;		Size of data
	BYTE used;		0=No, 1=Yes
	BYTE data[size];	Data
*/

#ifndef ALLOC_H

#define ALLOC_H

/* #define DEBUG_ALLOC */

/*	RUNTIME VARIABLES
*/

extern BYTE *ccfreefirst;
extern WORD ccfreebytes;

/*	PRIVATE VARIABLES
*/

BYTE	xm_ini,		/* if initialized == 0x69 */
	*xm_top,	/* First block */
	*xm_end;	/* Last block */

/*	void *malloc(unsigned int size)

	Memory allocation.

	Return pointer, or NULL if not enough memory.
*/

malloc(size)
WORD size;
{
	BYTE *memptr;
	WORD memsize;

	/* Need initialization ? */

	if(xm_ini!=0x69)
	{
		if(ccfreebytes < 7)
			return 0;

		xm_top=ccfreefirst;
		xm_end=xm_top + ccfreebytes - 3;

		xm_pw(xm_top,ccfreebytes - 6);
		xm_top[2]=0;

		xm_pw(xm_end,0);
		xm_end[2]=1;

		xm_ini=0x69;
	}

	/* Search free block */

	for(memptr=xm_top; memptr!=xm_end; memptr+=memsize+3)
	{
		memsize=xm_gw(memptr);

		if(memptr[2]==0)
		{
			if(memsize >= size)
			{
				memptr[2]=1;

				if(memsize >= size + 3)
				{
					xm_pw(memptr,size);
					xm_pw(memptr+size+3,memsize-size-3);
					*(memptr+size+5)=0;
				}

#ifdef DEBUG_ALLOC
	alloc_dbg("malloc", size);
#endif
				return memptr + 3;
			}
		}
	}

#ifdef DEBUG_ALLOC
	alloc_dbg("malloc", size);
#endif

	return 0;
}

/*	void free(void *ptr)

	Dealloc memory.
*/

free(ptr)
BYTE *ptr;
{
	char *memptr;
	unsigned memsize;

	/* Check NULL pointer */

	if(ptr == NULL)
		return;

	/* Make free */

	*(ptr - 1)=0;

	/* Compact block if possible */

	for(memptr=xm_top; memptr!=xm_end; memptr+=memsize+3)
	{
		memsize=xm_gw(memptr);

		if(memptr[2]==0)
		{
			if(*(memptr+memsize+5)==0)
			{
				memsize+=xm_gw(memptr+memsize+3)+3;

				if(*(memptr+memsize+5)==0)
					memsize+=xm_gw(memptr+memsize+3)+3;

				xm_pw(memptr,memsize);

#ifdef DEBUG_ALLOC
	alloc_dbg("free", ptr);
#endif
				return;
			}
		}
	}

#ifdef DEBUG_ALLOC
	alloc_dbg("free", ptr);
#endif
}

/*	int xm_gw(int *ptr)

	Get a int from ptr.
*/

xm_gw(ptr)
WORD *ptr;
{
	return *ptr;
}

/*	void xm_pw(int *ptr, int val)

	Put a int into ptr.
*/

xm_pw(ptr, val)
WORD *ptr, val;
{
	*ptr=val;
}

#ifdef DEBUG_ALLOC

alloc_dbg(fn, wrd)
char *fn; unsigned int wrd;
{
	unsigned char *pb, data[4];
	unsigned int size, *pw;
	int use;

	printf("SP = %04x (%s %04x)\n", alloc_sp(), fn, wrd);

	printf("Addr Size       Next Use Data\n");

	for(pb = xm_top; pb != xm_end; pb += size + 3)
	{
		pw = pb;
		size = *pw;
		use = pb[2];
		memcpy(data, pb + 3, 4);

		printf("%04x %04x %5u %04x %s \n", pb, size, size, pb + 3 + size, use ? "Used" : "Free");
	}

	getch();
}

#asm
alloc_sp:
	LD  HL,0
	ADD HL,SP
	RET
#endasm

#endif

#endif
