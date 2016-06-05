/*	sx_mv.c

	MV for Samarux.

	Rename files.

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

	mv old_name new_name

	Examples:

	mv schedule.txt schedule.bak
	mv todo.txt done.txz

	Changes:

	21 Dec 2014 : 1.00 : 1st version.
	24 Dec 2014 : 1.01 : Minor changes.
	04 Jan 2015 : 1.02 : Re-written.
	29 Nov 2015 : 1.03 : Added support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_MV

#else

#include "samarux.h"

#define MvMain main

#endif

MvMain(argc, argv)
int argc, argv[];
{
	/* Check arguments */

	if(argc != 3)
		return ErrorPars();

	/* File names can't be ambiguous */

	if(FnIsAmb(argv[1]) || FnIsAmb(argv[2]))
		return ErrorAmb();

	/* Rename file */

	/* We assume that rename fn. checks other issues */

	if(rename(argv[1], argv[2]))
		return Error("Can't rename");

	/* Success */

	return 0;
}

