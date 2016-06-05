/*	sx_exit.c

	EXIT for Samarux.

	Exit of shell.

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

	exit

	Options:

	None

	Examples:

	exit

	Changes:

	28 Dec 2014 : 1.00 : 1st version.
	08 Jan 2015 : 1.01 : Adapted to changes in shell.
*/

#define SX_EXIT

ExitMain(argc, argv)
int argc, argv[];
{
	/* Check arguments */

	if(argc != 1)
		return ErrorPars();

	/* Tell Samarux */

	sv_flg_quit = 1;

	/* Success */

	return 0;
}
