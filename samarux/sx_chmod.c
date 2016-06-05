/*	sx_chmod.c

	CHMOD for Samarux.

	Change file permissions / attributes.

	Copyright (c) 2007 - 2015 Miguel I. Garcia Lopez.

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

	Usage:

	chmod mode file ...

	Modes:

	w : Write permission  (R/W & R/O in CP/M).
	s : System attribute  (DIR & SYS in CP/M).
	a : Archive attribute (NORMAL & ARCHIVED in CP/M 3).

	Examples:

	chmod +s login.com
	chmod +wa-s notes.txt calendar.doc

	Changes:

	21 Dec 2014 : 1.00 : 1st version.
	24 Dec 2014 : 1.01 : Re-written.
	11 Feb 2015 : 1.02 : Added effective change of file attributes.
	05 Jun 2016 : 1.03 : Support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_CHMOD

#else

#include "samarux.h"

#define ChmodMain main

#endif

ChmodMain(argc, argv)
int argc, argv[];
{
	char *p; int i, ch, op, wrt, sys, arc;

	/* Check arguments */

	if(argc < 3)
		return ErrorPars();

	/* Default values */

	wrt = sys = arc = op = 0;

	/* Get permissions */

	p = argv[1];

	while((ch = *p++))
	{
		switch(ch)
		{
			case '+' : op = 1; break;   /* Set (on) */
			case '-' : op = -1; break;  /* Reset (off) */
			case 'w' : wrt = op; break; /* Write perm. */
			case 's' : sys = op; break; /* System file (hidden) */
			case 'a' : arc = op; break; /* Archived att. */
			default  : return ErrorOpt();
		}
	}

	/* Check permissions */

	if(!wrt && !sys && !arc)
		return ErrorIll();

	/* Process files */

	for(i = 2; i < argc; ++i)
	{
		if(ChmodFile(argv[i], wrt, sys, arc))
			return 1;
	}

	/* Return success */

	return 0;
}

ChmodFile(fn, wrt, sys, arc)
char *fn; int wrt, sys, arc;
{
	int dir;

	/* Check file name is not a directory */

	if(FnIsDir(fn))
		return ErrorFName();

	/* Check file name is not ambiguous */

	if(FnIsAmb(fn))
		return ErrorAmb();

	/* Make source FCX */

	if(CpMakeFcx(fn, sv_fcx))
		ErrorFName();

	/* Get current file attributes */

	bdos_hl(BF_DMA, sv_dma);

	if((dir = bdos_fcx_a(BF_FIND1ST, sv_fcx)) == 0xFF)
		return ErrorNoFile();

	memcpy(sv_fcx + 2, sv_dma + dir * 32 + 1, 11);

	/* Set file attributes in FCX */

	if(wrt)
	{
		if(wrt == 1)
			sv_fcx[10] &= 0x7F;
		else
			sv_fcx[10] |= 0x80;
	}

	if(sys)
	{
		if(sys == 1)
			sv_fcx[11] |= 0x80;
		else
			sv_fcx[11] &= 0x7F;
	}

	if(arc)
	{
		if(arc == 1)
			sv_fcx[12] |= 0x80;
		else
			sv_fcx[12] &= 0x7F;
	}

	/* Write to disk */

	if(bdos_fcx_a(BF_ATTRIB, sv_fcx) == 0xFF)
		return ErrorWrite();

	/* Return success */

	return 0;
}


