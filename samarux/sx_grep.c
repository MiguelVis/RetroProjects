/*	sx_grep.c

	GREP for Samarux.

	Find text patterns on files (filter).

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

	grep [-options] pattern [file ...]

	File - refers to standard input.

	Options:

	-c : Count [non] matching lines.
	-i : Ignore case.
	-v : Select non matching lines instead of matching ones.

	Examples:

	grep -i Elvis bio.txt story.txt article.doc

	Changes:

	19 Feb 2015 : 1.00 : 1st version.
	29 Nov 2015 : 1.01 : Added support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_GREP

#else

#include "samarux.h"

#define GrepMain main

#endif

#define GREP_COLS 80

GrepMain(argc, argv)
int argc, argv[];
{
	int i, count;
	char *p, *buf;
	int flg_cnt, flg_case, flg_rev;

	/* Check arguments */

	if(argc == 1)
		return ErrorPars();

	/* Defaults */

	flg_cnt = flg_case = flg_rev = 0;

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
					case 'c' : ++flg_cnt ; break;
					case 'i' : ++flg_case ; break;
					case 'v' : ++flg_rev ; break;
					default  : return ErrorOpt();
				}
			}
		}
		else
			break;
	}

	/* Pattern */

	if(i == argc)
		return ErrorPars();

	p = argv[i++];

	/* Buffer */

	if((buf = malloc(GREP_COLS)) == NULL)
		return ErrorMem();

	/* Process files */

	if(i == argc)
		count = GrepOut("-", p, buf, flg_cnt, flg_case, flg_rev);
	else
	{
		count = 0;

		while(i < argc)
			count += GrepOut(argv[i++], p, buf, flg_cnt, flg_case, flg_rev);
	}

	if(flg_cnt)
		printf("%d\n", count);

	/* Free buffer */

	free(buf);

	/* Return success or failure */

	return count ? 0 : 1;
}

GrepOut(fn, pat, buf, fcount, fcase, frev)
char *fn, *pat, *buf; int fcount, fcase, frev;
{
	int count, k;
	FILE *fp;

	/* Open */

	if(*fn == '-' && !fn[1])
		fp = stdin;
	else if((fp = fopen(fn, "r")) == NULL)
		return ErrorOpen();

	/* Process */

	count = 0;

	while(fgets(buf, GREP_COLS, fp) != NULL)
	{
		k = strlen(buf);

		if(buf[k - 1] != '\n')
		{
			ErrorTooLong(); break;
		}

		buf[k - 1] = 0;

		k = GrepMatch(pat, buf, fcase);

		if(frev)
			k = 1 - k;

		if(k)
		{
			if(!fcount)
				puts(buf);

			++count;
		}
	}

	/* Close */

	if(fp != stdin)
		fclose(fp);

	/* Return count of [non] matching lines */

	return count;
}

GrepMatch(pat, buf, fcase)
char *pat, *buf; int fcase;
{
	int p1, c1, c2;
	char *pp, *pb;

	p1 = *pat;

	if(fcase)
		p1 = toupper(p1);

	while((c2 = *buf++))
	{
		if(fcase)
			c2 = toupper(c2);

		if(p1 == c2)
		{
			pp = pat + 1;
			pb = buf;

			while(*pp)
			{
				c1 = *pp;
				c2 = *pb++;

				if(fcase)
				{
					c1 = toupper(c1); c2 = toupper(c2);
				}

				if(c1 != c2)
					break;

				++pp;
			}

			/* Success? */

			if(!*pp)
				return 1;
		}
	}

	/* Return failure */

	return 0;
}

#undef GREP_COLS


