/*	sx_tee.c

	TEE for Samarux.

	Copy standard input to standard output and to a file.

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

	tee file

	Examples:

	tee copy.txt

	Changes:

	02 Apr 2015 : 1.00 : 1st version.
	29 Nov 2015 : 1.01 : Added support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_TEE

#else

#include "samarux.h"

#define TeeMain main

#endif

TeeMain(argc, argv)
int argc, argv[];
{
	FILE *fp;
	int ch;

	/* Check arguments */

	if(argc != 2)
		return ErrorPars();

	/* Open output */

	if((fp = fopen(argv[1], "w")) == NULL)
		return ErrorOpen();

	/* Read & write */

	while((ch = getchar()) != EOF)
	{
		if(putchar(ch) == EOF || fputc(ch, fp) == EOF)
		{
			fclose(fp); return ErrorWrite();
		}
	}

	/* Close output */

	if(fclose(fp))
		return ErrorClose();

	/* Success */

	return 0;
}

