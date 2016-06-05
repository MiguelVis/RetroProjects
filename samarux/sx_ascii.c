/*	sx_ascii.c

	ASCII for Samarux.

	Prints the character table.

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

	ascii

	Examples:

	ascii

	Changes:

	19 Dec 2014 : 1.00 : 1st version.
	24 Dec 2014 : 1.01 : Added parameters control.
	29 Nov 2015 : 1.02 : Support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_ASCII

#else

#include "samarux.h"

#define AsciiMain main

#endif

AsciiMain(argc, argv)
int argc, argv[];
{
	int ch, nib;

	/* Check arguments */

	if(argc != 1)
		return ErrorPars();

	/* Print table */

	putstr("   0 1 2 3 4 5 6 7 8 9 A B C D E F");

	for(ch = 0; ch < 256; ++ch)
	{
		if(!(ch % 16))
		{
			putchar('\n');
			nib = ch / 16;
			putchar(nib < 10 ? '0' + nib : 'A' +nib - 10);
			putchar('x');
		}

		putchar(' ');

		putchar(ch > 31 ? ch : 'x');
	}

	putchar('\n');

	return 0;
}

