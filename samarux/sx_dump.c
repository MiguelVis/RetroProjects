/*	sx_dump.c

	DUMP for Samarux.

	Print a file in hexadecimal and text format like CP/M DUMP.

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

	dump file ...

	Examples:

	dump dump.com
	dump login.com cat.com

	Changes:

	19 Dec 2014 : 1.00 : 1st version.
	24 Dec 2014 : 1.01 : Minor changes.
	30 Dec 2014 : 1.02 : Removed wrong external reference.
	29 Nov 2015 : 1.03 : Added support for builtin / external.

*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_DUMP

#else

#include "samarux.h"

#define DumpMain main

#endif

DumpMain(argc, argv)
int argc, argv[];
{
	FILE *fp;
	int i;

	/* Check arguments */

	if(argc == 1)
		return ErrorPars();

	/* Process */

	for(i = 1; i < argc; ++i)
	{
		if((fp = fopen(argv[i], "rb")) == NULL)
			return ErrorOpen();

		DumpGo(fp);
		fclose(fp);
	}

	/* Success */

	return 0;
}

DumpGo(fp)
FILE *fp;
{
	WORD adr; int i; BYTE buf[16];

	adr = 0;

	while(1)
	{
		for(i = 0; i < 16; ++i)
		{
			if((buf[i] = fgetc(fp))==EOF)
				break;
		}

		if(i < 16)
			break;

		printf("%04x : ", adr);

		for(i = 0; i < 16; ++i)
			printf("%02x ", buf[i]);

		for(i = 0; i < 16; ++i)
			putchar(buf[i] > 31 && buf[i] < 128 ? buf[i] : '.');

		putchar('\n');

		adr += 16;
	}
}

