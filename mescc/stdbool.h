/*	stdbool.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Boolean type and values.

	Copyright (c) 1999-2016 Miguel I. Garcia Lopez

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

	13 May 2016 : 1st version.

	Notes:

	In standard / ISO C, bool type is _Bool, not int.
	ISO C defines bool_true_false_are_defined = 1.
*/

#ifndef STDBOOL_H

#define STDBOOL_H

#define bool  int
#define true  1
#define false 0

#endif
