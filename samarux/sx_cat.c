/*	sx_cat.c

	CAT for Samarux.

	Concatenate files.

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

	cat [-kn] [file ...]

	File - refers to standard input.

	Examples:

	cat - schedule.txt notes.txt
	cat myfile.doc

	Changes:

	19 Dec 2014 : 1.00 : 1st version.
	24 Dec 2014 : 1.01 : Minor changes.
	06 Mar 2015 : 1.02 : Added -k and -n options.
	29 Nov 2015 : 1.03 : Support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_CAT

#else

#include "samarux.h"

#define CatMain main

#endif

#define CAT_FLG_K 1
#define CAT_FLG_N 2

int cat_flags, cat_line;

CatMain(argc, argv)
int argc, argv[];
{
	int i;
	char *p;

	/* Defaults */

	cat_flags = 0; cat_line = 1;

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
					case 'k' : cat_flags |= CAT_FLG_K; break;
					case 'n' : cat_flags |= CAT_FLG_N; break;
					default  : return ErrorOpt();
				}
			}
		}
		else 
			break;
	}

	/* Process files */

	if(i == argc)
		CatOut("-");
	else
	{
		for(i = i; i < argc; ++i)
		{
			if(CatOut(argv[i]))
				return 1;
		}
	}

	/* Return success */

	return 0;
}

CatOut(fn)
char *fn;
{
	FILE *fp;
	int ch, nl, start;

	/* Open */

	if(*fn == '-' && !fn[1])
		fp = stdin;
	else if((fp = fopen(fn, "r")) == NULL)
		return ErrorOpen();

	/* Defaults */

	nl = start = 1;

	/* Read & write */

	while((ch = fgetc(fp)) != EOF)
	{
		/* New line ? */

		if(ch == '\n')
		{
			/* Empty line ? */

			if(nl)
			{
				/* Skip empty lines ? */

				if(cat_flags & CAT_FLG_K)
					continue;

				/* Mark start of line */

				start = 1;
			}
			else
				nl = 1;
		}
		else if(nl)
		{
			start = 1;
			nl = 0;
		}

		/* Start of line ? */

		if(start)
		{
			/* Print line numbers */

			if(cat_flags & CAT_FLG_N)
				printf("%5d ", cat_line++);

			start = 0;
		}

		/* Print character */

		putchar(ch);
	}

	/* Close */

	if(fp != stdin)
		fclose(fp);

	/* Success */

	return 0;
}

#undef CAT_FLG_K
#undef CAT_FLG_N



