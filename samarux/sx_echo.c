/*	sx_echo.c

	ECHO for Samarux.

	Outputs its arguments to the console.

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

	echo [-n] [arg ...]

	Options:

	-n New line: Supress final new line.

	Examples:

	echo Hello world
	echo -n Hello world

	Changes:

	19 Dec 2014 : 1.00 : 1st version.
	24 Dec 2014 : 1.01 : Minor changes.
	28 Dec 2014 : 1.02 : Added return value.
	17 Feb 2015 : 1.03 : Minor changes.
	29 Nov 2015 : 1.04 : Added support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_ECHO

#else

#include "samarux.h"

#define EchoMain main

#endif

EchoMain(argc, argv)
int argc, argv[];
{
	int flag_n;
	int i;

	/* Defaults */

	flag_n = 0;
	i = 1;

	/* Check arguments */

	if(argc > 1)
	{
		if(!strcmp(argv[1],  "-n"))
		{
			++flag_n; ++i;
		}
	}

	/* Print arguments */

	while(i < argc)
	{
		putstr(argv[i]);

		if(++i != argc)
			putchar(' ');
	}

	/* New line */

	if(!flag_n)
		putchar('\n');

	/* Success */

	return 0;
}

