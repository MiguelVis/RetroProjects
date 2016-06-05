/*	sx_sort.c

	SORT for Samarux.

	Sort text lines.

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

	sort [-rn] [file ...]

	File - refers to standard input.

	Examples:

	sort -r words.txt
	sort -n phones.doc

	Changes:

	03 Mar 2015 : 1.00 : 1st version.
	29 Nov 2015 : 1.01 : Added support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_SORT

#else

#include "samarux.h"

#define SortMain main

#endif

#define SORT_FLG_R 1     /* Reverse */
#define SORT_FLG_N 2     /* Numbers */

#define SORT_MX_LN 1024  /* Max. # of lines */
#define SORT_BF_SZ 80    /* Buffer size */

SortMain(argc, argv)
int argc, argv[];
{
	int i, flags, code, lines, *arr_lines; /* char *arr_lines[] */
	char *p, *buf;

	/* Defaults */

	flags = code = lines = 0;

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
					case 'r' : flags |= SORT_FLG_R; break;
					case 'n' : flags |= SORT_FLG_N; break;
					default  : return ErrorOpt();
				}
			}
		}
		else 
			break;
	}

	/* Buffers */

	arr_lines = KeyAlloc(SORT_MX_LN);
	buf = malloc(SORT_BF_SZ);

	/* Proceed with files */

	if(arr_lines != NULL && buf != NULL)
	{
		/* i == next arg */

		if(i == argc)
		{
			if((lines = SortRead("-", buf, arr_lines, 0, SORT_MX_LN)) == -1)
				++code;
		}
		else
		{
			for(i = i; i < argc; ++i)
			{
				if((lines = SortRead(argv[i], buf, arr_lines, lines, SORT_MX_LN)) == -1)
				{
					++code; break;
				}
			}
		}

		if(!code)
		{
			/* Sort lines */

			SortSort(arr_lines, lines, flags);

			/* Print lines */

			for(i = 0; i < lines; ++i)
				puts(arr_lines[i]);
		}
	}
	else
		code = ErrorMem();

	/* Free buffers */

	if(arr_lines != NULL)
		KeyFree(arr_lines, SORT_MX_LN);

	if(buf != NULL)
		free(buf);

	/* Return success or failure */

	return code;
}

SortRead(fn, buf, arr, lines, max)
char *fn, *buf; int *arr, lines, max; /* char *arr[] */
{
	FILE *fp;
	int err, k;

	/* Open */

	if(*fn == '-' && !fn[1])
		fp = stdin;
	else if((fp = fopen(fn, "r")) == NULL)
		return ErrorOpen();

	/* Defaults */

	err = 0;

	/* Read */

	while(fgets(buf, SORT_BF_SZ, fp) != NULL)
	{
		if(lines >= max)
		{
			err = Error("Too many lines"); break;
		}

		k = strlen(buf);

		if(buf[k - 1] != '\n')
		{
			err = ErrorTooLong(); break;
		}
	
		buf[--k] = 0;

		if((arr[lines] = malloc(k + 1)) == NULL)
		{
			err = ErrorMem(); break;
		}

		strcpy(arr[lines++], buf);
	}

	/* Close */

	if(fp != stdin)
		fclose(fp);

	/* Return # of total lines, or -1 on failure */

	return err ? -1 : lines;
}

SortSort(arr, lines, flags)
int *arr, lines, flags; /* char *arr[] */
{
	int i, j;
	char *p;

	/* Sort all the lines */

	for(i = 0; i < lines; ++i)
	{
  		for(j = i + 1; j < lines; ++j)
		{
   			if(SortCmp(arr[i], arr[j], flags) > 0) /* (a[i] > a[j]) */
			{
				p = arr[i]; arr[i] = arr[j]; arr[j] = p;
   			}
  		}
 	}
}

SortCmp(a, b, flags)
char *a, *b; int flags;
{
	int c, d, result;

	/* Sort numbers or text ? */

	if(flags & SORT_FLG_N)
	{
		c = atoi(a);
		d = atoi(b);

		result = c - d;
	}
	else
		result = strcmp(a, b);

	/* Reverse order ? */

	if(flags & SORT_FLG_R)
		result *= -1;

	/* Return < 0 or == 0 or > 0 */

	return result;
}

#undef SORT_FLG_R
#undef SORT_FLG_N
#undef SORT_MX_LN
#undef SORT_BF_SZ
