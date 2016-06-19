/*	sx_shift.c

	SHIFT for Samarux.

	Shift positional parameters to the left.

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

	shift

	Options:

	None.

	Examples:

	shift

	Changes:

	19 Jun 2016 : 1.00 : 1st version.
*/

#define SX_SHIFT

ShiftMain(argc, argv)
int argc, argv[];
{
	/* Run only within a batch file */
	if(!sv_batch)
		return Error("Batch is not running");

	/* Check arguments */
	if(argc != 1)
		return ErrorPars();

	/* Perform shift */
	BatchShift();

	/* Success */
	return 0;
}
