/*	sx_sleep.c

	SLEEP for Samarux.

	Suspend execution for a specified time.

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

	sleep #seconds

	Examples:

	sleep 60

	Changes:

	15 Dec 2016 : 1.00 : 1st version.

	Notes:

	Only works with CP/M 3.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_SLEEP

#else

#include "samarux.h"

#define SleepMain main

#endif

SleepMain(argc, argv)
int argc, argv[];
{
	int seconds;

	/* Check CP/M version */

	if(sv_cpmver < 0x30)
		return Error("Need CP/M 3");

	/* Check arguments */

	if(argc != 2)
		return ErrorPars();

	/* Get # of seconds to sleep */

	seconds = atoi(argv[1]);

	if(seconds < 1 || seconds > 32000)
		return Error("Bad # of seconds");

	/* Do it */

	SleepDoIt(seconds);

	/* Success */

	return 0;
}

SleepDoIt(seconds)
int seconds;
{
	unsigned char date_clk[CLOCK_LEN];
	unsigned char now;

	/* Get current time */

	getclock(date_clk); now = date_clk[4];

	/* Wait # of seconds */

	while(seconds)
	{
		/* Get current time */

		getclock(date_clk);

		/* Check for time changes */

		if(now != date_clk[4])
		{
			now = date_clk[4]; --seconds;
		}
	}
}

