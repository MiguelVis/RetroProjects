/*	sx_pwd.c

	PWD for Samarux.

	Print the current working directory.

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

	pwd

	Examples:

	pwd

	Changes:

	19 Dec 2014 : 1.00 : 1st version.
	24 Dec 2014 : 1.01 : Minor changes.
	04 Sep 2015 : 1.02 : Now uses PrintCWD().
*/

#define SX_PWD

PwdMain(argc, argv)
int argc, argv[];
{
	/* Check arguments */

	if(argc != 1)
		return ErrorPars();

	/* Print CWD */

	PrintCWD(); putchar('\n');

	/* Success */

	return 0;
}

