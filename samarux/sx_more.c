/*	sx_more.c

	MORE for Samarux.

	Console output pager (filter).

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

	more [-num] [file ...]

	File - refers to standard input.

	Examples:

	more
	more myfile.doc
	more -24 file.txt letter.doc

	Changes:

	30 Dec 2014 : 1.00 : 1st version.
	31 Dec 2014 : 1.01 : Minor changes in console output.
	02 Jan 2015 : 1.02 : Prints banner if there is more than one file to be displayed.
	03 Jan 2015 : 1.03 : Gets tty size from Samarux variables.
	26 Feb 2015 : 1.04 : Gets tty size from TermRow() and TermCol().
	31 Mar 2015 : 1.05 : Change RETURN CODE behaviour.
	29 Nov 2015 : 1.06 : Added support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_MORE

#else

#include "samarux.h"

#define MoreMain main

#endif

MoreMain(argc, argv)
int argc, argv[];
{
	int i, argi, rows, cols, code;
	char *p, *buf;

	/* Defaults */

	argi = 1;  /* First file name */
	rows = TermRows(); /* Screen rows */
	cols = TermCols(); /* Screen columns */

	/* Check arguments */

	if(argc > 1)
	{
		p = argv[1];

		if(*p == '-' && isdigit(p[1]))
		{
			if((rows = atoi(p + 1)) < 8)
				return Error("Bad number");

			++argi;
		}
	}

	/* Buffer */

	if((buf = malloc(cols)) == NULL)
		return ErrorMem();

	/* Process */

	if(argi == argc)
		code = MoreOut("-", rows, cols, buf, 0, 1);
	else
	{
		for(i = argi; i < argc; ++i)
		{
			if((code = MoreOut(argv[i], rows, cols, buf, argi == argc - 1 ? 0 : 1, i == argc - 1 ? 1 : 0)))
				break;
		}
	}

	/* Free buffer */

	free(buf);

	/* Return success or failure */

	return code;
}

MoreOut(fn, rows, cols, buf, banner, last)
char *fn; int rows, cols; char *buf; int banner, last;
{
	FILE *fp; int k, lin, q;

	/* Open */

	if(*fn == '-' && !fn[1])
		fp = stdin;
	else if((fp = fopen(fn, "r")) == NULL)
		return ErrorOpen();

	lin = q = 0;

	if(banner)
	{
		printf("-- File: %s --\n", fn); ++lin;
	}

	/* Print */

	while(fgets(buf, cols, fp) != NULL)
	{
		putstr(buf);

		k = strlen(buf);

		if(buf[k - 1] != '\n')
			putchar('\n');

		if(++lin == rows - 1)
		{
			if((q = MorePause()))
				break;

			lin = 0;
		}
	}

	/* Close */

	if(fp != stdin)
		fclose(fp);

	if(banner && lin && !q && !last)
		MorePause();

	return 0;
}

MorePause()
{
	int k;

	putstr("-- More (q:quit) --");

	k = getch();

	putchar('\n');

	return k == 'q';
}


