/*	atexit.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Library for atexit() function.

	Copyright (c) 2015 Miguel I. Garcia Lopez / FloppySoftware, Spain

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

	Revisions:

	30 Nov 2015 : First version.

	int atexit(void (*func)(void));

*/

#ifndef ATEXIT_H

#define ATEXIT_H

#define ATEXIT_MAX 3		/* Max. # of allowed functions */

int atexit_now;			/* Counter for # of stored functions */
WORD atexit_arr[ATEXIT_MAX];	/* Array for stored functions */

/*	int atexit(void (*func)(void))

	Adds a function to be called when the program terminates normally,
	with exit() or a return in main() function. Returns 0 on success.
*/

int atexit(func)
WORD func;
{
	/* Patch exit() */

#asm
	LD A,0CDH
	LD (exit),A
	LD HL,exit_patch
	LD (exit + 1),HL
#endasm

	if(atexit_now < ATEXIT_MAX) {
		atexit_arr[atexit_now++] = func; return 0;
	}

	return -1;
}

/*
*/

exit_patch()
{
	while(atexit_now)
		atexit_arr[--atexit_now]();
}

#undef ATEXIT_MAX

#endif
