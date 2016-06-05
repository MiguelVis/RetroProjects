/*	sx_built.c

	BUILTIN for Samarux.

	Print Samarux internal commands.

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

	builtin

	Options:

	None

	Examples:

	builtin

	Changes:

	28 Dec 2014 : 1.00 : 1st version.
	20 Jan 2015 : 1.01 : Added trailing newline.
	11 Feb 2015 : 1.02 : Changed name to builtin (was cmds and commands before).
	03 Sep 2015 : 1.03 : Separate command names with a tab.
	04 Sep 2015 : 1.04 : Print command names in 3 columns.
*/

#define SX_BUILTIN

BuiltinMain(argc, argv)
int argc, argv[];
{
	int i;

	/* Check arguments */

	if(argc != 1)
		return ErrorPars();

	/* Print internal command names in 3 columns */

	for(i = 0; i < sv_cmd_max; ++i)
	{
		printf("%-8s", (sv_cmd_name[i]));

		putchar(((i + 1) % 3) ? ' ' : '\n');
	}

	/* Terminate last line */

	if((i + 1) % 3)
		putchar('\n');

	/* Success */

	return 0;
}
