/*	rand.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Pseudo-random number generation.

	Copyright (c) 1999-2015 Miguel I. Garcia Lopez / FloppySoftware, Spain

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

	18 Mar 2015 : 1st version.
	23 Mar 2015 : Trying to improve rand().

	Public:

	int rand(void)
	void srand(unsigned seed)
	
*/

#ifndef RAND_H

#define RAND_H

#define RAND_MAX 32767

/*	PRIVATE VARIABLES
*/

extern int xs_seed;

#asm

xs_seed: defw 1

#endasm

/*	int rand(void)

	Returns a pseudo-random value between 0 and RAND_MAX (both included).
*/

rand()
{
	/* return (xs_seed = ((xs_seed * 3) + 1) & 0x7FFF); */

	return (xs_seed = (((xs_seed << 1) + xs_seed) + 1) & 0x7FFF);
}

/*	void srand(unsigned seed)

	Seeds the pseudo-random number generatod user by rand().
*/

srand(seed)
unsigned seed;
{
	xs_seed = seed;
}

#endif

