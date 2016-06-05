/*	sx_df.c

	DF for Samarux.

	Shows free disk space.

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

	df [drive ...]

	Examples:

	df a:
	df a: b:

	Changes:

	07 Jan 2014 : 1.00 : 1st version.
	29 Nov 2015 : 1.01 : Added support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_DF

#else

#include "samarux.h"

#define DfMain main

#endif

DfMain(argc, argv)
int argc, argv[];
{
	int i, du, drv, usr;

	/* Check arguments */

	if(argc == 1)
		return DfGo(sv_drive);

	/* Process */

	for(i = 1; i < argc; ++i)
	{
		if((du = UxGetPath(argv[i])) != -1)
		{
			drv = (du >> 8) & 0xFF;
			usr = du & 0xFF;

			if(usr == 0xDD)
			{
				if(drv == 0xDD)
					drv = sv_drive;

				if(DfGo(drv))
					return 1;
				else
					continue;
			}
		}

		return Error("Bad drive");
	}

	/* Success */

	return 0;
}

DfGo(drv)
int drv;
{
	unsigned int recs; unsigned int *p;

	/* Get available space */

	bdos_hl(BF_DMA, sv_dma);

	if(bdos_a(46, drv) == 0xFF)
	{
		return Error("Drive error");
	}

	p = sv_dma;

	recs = *p; /* FIXME: We are ignoring the 3rd byte */

	printf("%c: %4d KB (%5d recs.) free\n", 'A' + drv, recs / 8, recs);

	/* Success */

	return 0;
}
