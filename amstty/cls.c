/*	status.c

	Clear the screen of the Amstrad CP/M.

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

	www.floppysoftware.vacau.com
	cpm-connections.blogspot.com
	floppysoftware@gmail.com

	Usage:

	cls

	Examples:

	cls

	To compile with MESCC:

	cc cls
	zsm cls
	hextocom cls

	Changes:

	20 Mar 2015 : 1.00 : First version.
*/

/* DEFs TO DECREASE CODE SIZE
   --------------------------
*/

#define CC_NO_ARGS   /* No ARGC & ARGV for main() */
#define CC_NO_MUL    /* No multiplication */
#define CC_NO_DIV    /* No division & modulus */
#define CC_NO_SWITCH /* No switch / case */

/* MESCC DEFs
   ----------
*/

#include <mescc.h>
#include <conio.h>

/* PROGRAM CODE
   ------------
*/

main()
{
	putch(27); putch('H');  /* Send cursor to HOME */
	putch(27); putch('E');  /* Clear the screen */
}


