/*	sx_hist.c

	HISTORY for Samarux.

	Command line history.

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

	history [number]

	Options:

	None

	Examples:

	history    -- To print all command line history
	history 3  -- To select command line history entry # 3

	Changes:

	28 Dec 2014 : 1.00 : 1st version.
	08 Jan 2014 : 1.01 : Adapted to changes in shell.
*/

#define SX_HISTORY

HistMain(argc, argv)
int argc, argv[];
{
	int i;

	/* Don't save this command in the history */

	sv_flg_hist = 1;

	/* Check arguments */

	switch(argc)
	{
		case 1 :
			for(i = 0; i < SX_MAX_HIST; ++i)
			{
				if(sv_hist[i] != NULL)
					printf("%d: %s\n", i, sv_hist[i]);
			}
 			break;
		case 2 :
			i = atoi(argv[1]);
			if(i < 0 || i  >= SX_MAX_HIST)
				return Error("Bad history number");

			if(sv_hist[i] != NULL)
			{
				strcpy(sv_ed_buf, sv_hist[i]);
				sv_flg_buf = 1;
			}
			break;
		default :
			return ErrorPars();
	}

	/* Success */

	return 0;
}


