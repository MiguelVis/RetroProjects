/*	sx_diral.c

	DIRALIAS for Samarux.

	Manage directory names (alias for user and drive specifications).

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

	Usage:

	diralias [name [directory]]

	Examples:

	diralias           -- print all directory names
	diralias root a0:  -- set directory name
	diralias root      -- remove directory name

	Changes:

	03 Sep 2015 : 1.00 : 1st version.
	05 Jun 2016 : 1.01 : Added support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_DIRALIAS

#else

#include "samarux.h"

#define DiralMain main

#endif

DiralMain(argc, argv)
int argc, argv[];
{
	int x, drv, usr;
	char path[4];

	/* Check arguments */

	if(argc == 1)
		KeyPrint(sv_dir_path, sv_dir_alias, SX_MAX_DIR);
	else
	{
		/* Convert directory name to upper case */

		strupr(argv[1]);

		/* Search the directory name */

		x = KeyFind(sv_dir_alias, SX_MAX_DIR, argv[1]);

		if(argc == 2)
		{
			if(x != -1)
				KeyDel(sv_dir_path, sv_dir_alias, SX_MAX_DIR, x);
			else
				return Error("Can't find alias");
		}
		else if(argc == 3)
		{
			if(x != -1)
				return Error("Alias already exists");

			/* Extract the path (drive and user) */

			usr = UxGetPath(argv[2]);

			drv = (usr >> 8) & 0xFF;  /* 0...15 */
			usr = usr & 0xFF;         /* 0...15 */

			if(drv > 0x0F || usr > 0x0F)
				return ErrorDir();

			/* Compose the path string: "A00" */

			sprintf(path, "%c%02d", 'A' + drv, usr);

			/* Check if the drive and user specification,
			   already exists under another directory name. */

			if(KeyFind(sv_dir_path, SX_MAX_DIR, path) != -1)
				return Error("Already has an alias");

			/* Add the directory name */

			if(KeyAdd(sv_dir_path, sv_dir_alias, SX_MAX_DIR, path, argv[1]) == -1)
				return 1;
		}
		else
			return ErrorPars();
	}

	/* Success */

	return 0;
}

