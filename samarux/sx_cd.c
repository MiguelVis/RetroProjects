/*	sx_cd.c

	CD for Samarux.

	Changes the current working directory.

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

	cd [directory]

	Examples:

	cd a01:
	cd 0:

	Changes:

	19 Dec 2014 : 1.00 : 1st version.
	24 Dec 2014 : 1.01 : Minor changes.
	05 Jan 2015 : 1.02 : Update drive and user Samarux variables.
	17 Feb 2015 : 1.03 : Minor changes.
	22 Mar 2015 : 1.04 : Added support for $HOME.

*/

#define SX_CD

CdMain(argc, argv)
int argc, argv[];
{
	/* Check arguments */

	if(argc == 1)
	{
		if((argv[1] = EnvGet("HOME")) == NULL)
			return Error("Can't find $HOME");
	}
	else if(argc != 2)
		return ErrorPars();

	/* Change directory */

	if(UxChdir(argv[1]) == -1)
		return ErrorDir();

	/* Update Samarux variables */

	sv_drive = bdos_a(BF_GETDRV, 0xCACA);
	sv_user = bdos_a(BF_USER, 0xFFFF);

	/* Success */

	return 0;
}


