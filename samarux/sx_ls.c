/*	sx_ls.c

	LS for Samarux.

	List files.

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

	ls [-aksvfl ...] [dir|file ...]

	Options:

	-a Attributes: Print file attributes.
	-k Kbytes:     Print file size.
	-s Sort:       Sort file names.
	-v Verbose:    Verbose.
	-f Full:       Same as -aksv.
	-l Long:       Same as -aks.

	Examples:

	ls -f *.c
	ls -sv myfile.c myfile.h myfile.zsm

	Changes:

	19 Dec 2014 : 1.00 : 1st version.
	24 Dec 2014 : 1.01 : Minor changes.
	04 Jan 2015 : 1.02 : Changed code in make fcx section. Re-written flags code.
	08 Jan 2015 : 1.03 : Adapted to changes in shell.
	11 Feb 2015 : 1.04 : Solved bug in file attributes checking.
	17 Feb 2015 : 1.05 : Added -l option. Unix style output for attributes.
	19 Feb 2015 : 1.06 : Work on -f & -l options.
	05 Jun 2016 : 1.07 : Added support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_LS

#else

#include "samarux.h"

#define LsMain main

#endif

#define LS_FLG_A 1  /* Attributes */
#define LS_FLG_K 2  /* Kbytes (file size) */
#define LS_FLG_S 4  /* Sort */
#define LS_FLG_V 8  /* Verbose */

#define LS_FLG_F 15 /* Full: AKSV */
#define LS_FLG_L 7  /* Long: AKS */

LsMain(argc, argv)
int argc, argv[];
{
	int i, files, flags;
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
					case 'a' : flags |= LS_FLG_A ; break;
					case 'v' : flags |= LS_FLG_V ; break;
					case 'k' : flags |= LS_FLG_K ; break;
					case 's' : flags |= LS_FLG_S ; break;
					case 'l' : flags |= LS_FLG_L ; break;
					case 'f' : flags |= LS_FLG_F ; break;
					default  : return ErrorOpt();
				}
			}
		}
		else
			break;
	}

	/* Search files */

	if(i == argc)
		argv[--i] = "*.*";

	if((files = LsFind(argc - i, &argv[i], flags)) > 0)
		return LsPrint(files, flags);

	/* Return success or failure */

	return files ? 1 : 0;
}

LsFind(hm, fn, flags)
int hm, fn[], flags;
{
	int du, drv, usr, files, max, i, ver; char *buf;

	files = 0;
	max = SX_MAX_FCX;
	buf = sv_fcxbuf;
	ver = flags & LS_FLG_V;

	/* Process all file names */

	for(i = 0; i < hm; ++i)
	{
		/* Make FCX */

		if(MakeFcx(fn[i], sv_fcx))
			return ErrorFName();

		/* Search files */

		if(ver)
		{
			putstr("Searching: "); RtPrintFcx(sv_fcx);
		}

		if((files = RtExpandFcx(sv_fcx, buf, max)) == -1)
		{
			ErrorTooMany(); return -1;
		}

		if(ver)
			printf(" - %d file(s) found\n", files);

		max -= files;
		buf += files * 13;
	}

	files = SX_MAX_FCX - max;

	if(ver)
	{
		putchar('\n');

		if(hm > 1)
			printf("Total %d files(s) found\n\n", files);
	}

	/* Return # of files found */

	return files;
}


LsPrint(files, flags)
int files, flags;
{
	int i, ver;
	BYTE *buf; unsigned int *fsz, *fszbuf, kb;

	ver = flags & LS_FLG_V;

	/* Sort file names */

	if(flags & LS_FLG_S)
	{
		if(ver)
			printf("Sorting %d file(s)\n\n", files);

		RtSortFcx(sv_fcxbuf, files);
	}

	/* Get file size */

	if(flags & LS_FLG_K)
	{

		if(ver)
			printf("Getting size of file(s)\n\n");

		if((fszbuf = malloc(files + files)) == NULL)
			return ErrorMem();

		buf = sv_fcxbuf; fsz = sv_fcx + UX_FCX_RRC;

		for(i = 0; i < files; ++i)
		{
			memcpy(sv_fcx, buf, 13);

			bdos_fcx_a(BF_FSIZE, sv_fcx);

			fszbuf[i] = *fsz;

			buf += 13;
		}
	}

	/* Print results */

	buf = sv_fcxbuf;

	for(i = 0; i < files; ++i)
	{
		/* Print file permissions (attributes) */

		if(flags & LS_FLG_A)
		{
			putchar(buf[10] & 128 ? '-' : 'w'); /* R/O or R/W */
			putchar(buf[11] & 128 ? 's' : '-'); /* SYS or DIR */
			putchar(buf[12] & 128 ? 'a' : '-'); /* ARC or UPD */

			putchar(' ');
		}

		/* Print file size */

		if(flags & LS_FLG_K)
		{
			kb = fszbuf[i] / 8; if(fszbuf[i] % 8) ++kb;

			printf("%4u Kb %5u Rc ", kb, fszbuf[i]);
		}

		/* Print file name */

		RtPrintFcx(buf);

		putchar('\n');

		buf += 13;
	}

	/* Free buffer */

	if(flags & LS_FLG_K)
		free(fszbuf);

	/* Success */

	return 0;
}

#undef LS_FLG_A
#undef LS_FLG_V
#undef LS_FLG_K
#undef LS_FLG_S
#undef LS_FLG_F
#undef LS_FLG_L

