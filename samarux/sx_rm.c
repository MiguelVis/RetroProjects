/*	sx_rm.c

	RM for Samarux.

	Remove files.

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

	rm [-fi] dir|file ...

	Options:

	-f Force:       Delete without confirmation on directory or ambiguous file names.
	-i Interactive: Asks to user for each deletion. It has prevalence over -f option.
	-v Verbose.

	Examples:

	rm myfile.txt
	rm hello.c hello.h
	rm -f B01:
	rm -fi A02:*.bak

	Changes:

	21 Dec 2014 : 1.00 : 1st version.
	24 Dec 2014 : 1.01 : Minor changes.
	04 Jan 2015 : 1.02 : Removed -r recursive option. Added some code.
	06 Jan 2015 : 1.03 : Added -v option. Added real deletion code.
	09 Apr 2015 : 1.04 : Modified flag F test.
	05 Jun 2016 : 1.05 : Added support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_RM

#else

#include "samarux.h"

#define RmMain main

#endif

#define RM_FLG_F 1
#define RM_FLG_I 2
#define RM_FLG_V 4

RmMain(argc, argv)
int argc, argv[];
{
	int i, flags;
	char *p;

	/* Defaults */

	flags = 0;

	/* Check arguments */

	for(i = 1; i < argc; ++i)
	{
		p = argv[i];

		if(*p == '-')
		{
			while(*++p)
			{
				switch(*p)
				{
					case 'f' : flags |= RM_FLG_F; break;
					case 'i' : flags |= RM_FLG_I; break;
					case 'v' : flags |= RM_FLG_V; break;
					default  : return ErrorOpt();
				}
			}
		}
		else 
			break;
	}

	/* i == next arg */

	if(i == argc)
		return ErrorPars();

	/* Delete files */

	for(i = i; i < argc; ++i)
	{
		if(RmRemove(argv[i], flags))
			return 1;
	}

	/* Success */

	return 0;
}

RmRemove(fn, flags)
char *fn; int flags;
{
	int reply;

	if((FnIsAmb(fn) || FnIsDir(fn)) && !(flags & RM_FLG_F))
		return ErrorAmb();

	if(MakeFcx(fn, sv_fcx))
		return ErrorFName();

	if(flags & RM_FLG_I)
	{
		putstr("Remove "); RtPrintFcx(sv_fcx); putstr(" (y/n)? ");

		reply = getch(); putchar('\n');

		if(reply != 'y')
		{
			puts("File not removed."); return 0;
		}
	}

	if(flags & RM_FLG_V)
	{
		putstr("Removing "); RtPrintFcx(sv_fcx); putchar('\n');
	}

	bdos_hl(BF_DMA, sv_dma);

	if(bdos_fcx_a(BF_DELETE, sv_fcx) == 0xFF) /* FIXME: File can be R/O (or not exists) */
		return ErrorDelete();

	return 0;
}	

#undef RM_FLG_F
#undef RM_FLG_I
#undef RM_FLG_V


