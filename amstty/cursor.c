/*	cursor.c

	Set the screen cursor of the Amstrad CP/M.

	Copyright (c) 2015 Miguel I. Garcia Lopez / FloppySoftware

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

	Contact:

	www.floppysoftware.es
	cpm-connections.blogspot.com
	floppysoftware@gmail.com

	Usage:

	cursor on | off

	Examples:

	cursor on
	cursor off

	To compile with MESCC:

	cc cursor
	zsm cursor
	hextocom cursor

	Changes:

	20 Mar 2015 : 1.00 : First version.
*/

/* DEFs TO DECREASE CODE SIZE
   --------------------------
*/

#define CC_NO_DIV    /* No division & modulus */
#define CC_NO_SWITCH /* No switch / case */

/* MESCC DEFs
   ----------
*/

#include <mescc.h>
#include <conio.h>
#include <string.h>

/* PROGRAM CODE
   ------------
*/

main(argc, argv)
int argc, argv[];
{
	/* Check arguments */

	if(argc == 2)
	{
		/* Check option */

		if(!strcmp(argv[1], "ON"))
		{
			putch(27); putch('e'); return;   /* Cursor ON */
		}
		else if(!strcmp(argv[1], "OFF"))
		{
			putch(27); putch('f'); return;   /* Cursor OFF */
		}
	}

	/* Error: show usage */

	puts("\n(c) 2015 Miguel Garcia / FloppySoftware\n\nUsage: CURSOR ON | OFF\n");
}

