/*	sx_goto.c

	GOTO for Samarux.

	Jump to a batch label.

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

	goto label

	Options:

	None.

	Examples:

	goto quit
	...
	# quit:

	Changes:

	15 Feb 2015 : 1.00 : 1st version.
	23 Feb 2015 : 1.01 : Now uses BatchJump().
*/

#define SX_GOTO

#define GOTO_LAB_LEN 8  /* Max. length of label name */
#define GOTO_LAB_SIZ 12 /* '# 12345678:',0 */

GotoMain(argc, argv)
int argc, argv[];
{
	int i;
	char lab[GOTO_LAB_SIZ];

	/* Run only within a batch file */
	if(!sv_batch)
		return Error("Batch is not running");

	/* Two arguments, please */
	if(argc != 2)
		return ErrorPars();

	/* Check argument length */
	if(strlen(argv[1]) > GOTO_LAB_LEN)
		return Error("Label too long");

	/* Build label to search */
	strcpy(lab, "# ");
	strcat(lab, argv[1]);
	strcat(lab, ":");

	/* Search label */
	if(BatchJump(lab))
		return Error("Label not found");

	/* Success */
	return 0;
}

#undef GOTO_LAB_LEN
#undef GOTO_LAB_SIZ

