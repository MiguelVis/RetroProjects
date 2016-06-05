/*	sx_wc.c

	WC for SamaruX.

	Count lines, words and characters.

	Copyright (c) 2015 Miguel I. Garcia Lopez.

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

	wc [file ...]

	File - refers to standard input.

	Examples:

	wc mytasks.txt
	grep 2015 diary.doc | wc

	Notes:

	The CR ('\r') characters will not be counted.

	Changes:

	04 Mar 2015 : 1.00 : 1st version.
	29 Nov 2015 : 1.01 : Added support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_WC

#else

#include "samarux.h"

#define WcMain main

#endif

int wc_lines, wc_words, wc_chars, wc_tlines, wc_twords, wc_tchars;

WcMain(argc, argv)
int argc, argv[];
{
	int i, err;
	char *p;

	/* Defaults */

	wc_tlines = wc_twords = wc_tchars = 0;

	/* Process files */

	if(argc == 1)
		WcCount("-");
	else
	{
		for(i = 1; i < argc; ++i)
		{
			if(WcCount(argv[i]))
				return 1;

			/* Add to totals */

			wc_tlines += wc_lines;
			wc_twords += wc_words;
			wc_tchars += wc_chars;
		}
	}

	/* Print totals */

	if(argc > 2)
	{
		putchar('\n');

		WcPrint(wc_tlines, wc_twords, wc_tchars, "Totals");
	}

	/* Success */

	return 0;
}

WcCount(fn)
char *fn;
{
	FILE *fp;
	int k, w;

	/* Open */

	if(*fn == '-' && !fn[1])
		fp = stdin;
	else if((fp = fopen(fn, "r")) == NULL)
		return ErrorOpen();

	/* Defaults */

	wc_lines = wc_words = wc_chars = w = 0;

	/* Read & count */

    	while((k = fgetc(fp)) != EOF)
	{
		++wc_chars;

		if(k == '\n' || k == ' ' || k == '\t')
		{
	    		if(w)
			{
				++wc_words; w = 0;
			}

			if(k == '\n')
				++wc_lines;
		}
		else
				w = 1;
	}

	/* Close */

	if(fp != stdin)
		fclose(fp);

	/* Print counters */

	WcPrint(wc_lines, wc_words, wc_chars, fn);

	/* Success */

	return 0;
}

WcPrint(lines, words, chars, subject)
int lines, words, chars; char *subject;
{
	printf("Lines: %5d - Words: %5d - Chars: %5d : %s\n", lines, words, chars, subject);
}



