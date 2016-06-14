/*	sx_tty.c

	TTY for Samarux.

	Return user's terminal name.

	Copyright (c) 2016 Miguel I. Garcia Lopez.

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

	tty

	Examples:

	tty

	Changes:

	14 Jun 2016 : 1.00 : 1st version.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_TTY

#else

#include "samarux.h"

#define TtyMain main

#endif

TtyMain(argc, argv)
int argc, argv[];
{
	char *term;

	/* Check arguments */

	if(argc != 1)
		return ErrorPars();

	/* Standard input is a terminal? */

	if(stdin == NULL) {

		/* Yes; get the terminal name */

		term = EnvGet("TERM");

		/* Print the terminal name */

		puts(term != NULL ? term : "unknown");

		/* Exit with value 0 according to POSIX */

		return 0;
	}

	/* No; tell it */

	puts("not a tty");

	/* Exit with value 1 according to POSIX */

	return 1;
}
