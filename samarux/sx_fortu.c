/*	sx_fortu.c

	FORTUNE for Samarux.

	Prints a randomly selected fortune.

	Copyright (c) 2007 - 2016 Miguel I. Garcia Lopez.

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

	fortune [file]

	Examples:

	fortune
	fortune jokes.txt

	Changes:

	17 Dec 2016 : 1.00 : 1st version.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_FORTUNE

#else

#include "samarux.h"

#define FortuneMain main

#endif

#define FTNE_FILE  "fortunes.txt"  /* Default fortunes file */
#define FTNE_LINES 128             /* Max. # of lines in a file */
#define FTNE_LSIZE 127             /* Max. line size in characters excluding trailing ZERO */

FortuneMain(argc, argv)
int argc, argv[];
{
	WORD *fortunes;  /* Array of text lines - char *fortune_buf[] */
	BYTE date_clk[CLOCK_LEN];

	/* Check arguments */
	if(argc > 2)
		return ErrorPars();

	/* Read file */
	if((fortunes = ReadFile((argc == 1 ? FTNE_FILE : argv[1]), FTNE_LINES, FTNE_LSIZE)))
	{
		/* Seed random */
		if(sv_cpmver >= 0x30)
		{
			getclock(date_clk);
			srand((date_clk[3] << 8) | date_clk[4]);
		}

		/* Print random fortune */
		puts(fortunes[rand() % KeyCount1st(fortunes, FTNE_LINES)]);

		/* Free buffers */
		KeyFree(fortunes, FTNE_LINES);

		/* Success */
		return 0;
	}

	/* Failure */
	return -1;
}

#undef FTNE_FILE
#undef FTNE_LINES
#undef FTNE_LSIZE
