/*	hello.c

	Hello world for MESCC.

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

	Revisions:

	23 Feb 2015 : v1.00
	05 Apr 2016 : v1.01 Minor changes.
*/

/* Some defs. to save memory
*/
#define CC_NO_MUL    // To exclude MULTIPLICATION code.
#define CC_NO_DIV    // To exclude DIVISION & MODULUS code.
#define CC_NO_SWITCH // To exclude SWITCH code.
#define CC_NO_ARGS   // To exclude ARGC & ARGV code.

/* Standard libraries
*/
#include "mescc.h"
#include "conio.h"

/* The real code
*/
main()
{
	puts("Hello world!");
}

