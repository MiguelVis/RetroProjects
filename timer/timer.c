/*	timer.c

	Timer library demonstration. Needs CP/M 3.0 or higher.

	Copyright (c) 2016 Miguel Garcia / FloppySoftware

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 3, or (at your option) any
	later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

	Author's contact:

		www.floppysoftware.es
		cpm-connections.blogspot.com
		floppysoftware@gmail.com

	To compile with MESCC:

		cc timer
		ccopt timer
		zsm timer
		hextocom timer

	Usage:

		timer

	Revisions:

		10 Oct 2018 : v1.00.
*/

/* Defines for MESCC libraries
   ---------------------------
   Leave only what you need.
*/
/*
#define CC_STDIO        // Support for stdin, stdout, stderr.
#define CC_REDIR        // Support for command line redirection - needs CC_STDIO.

#define CC_CONIO_BIOS   // Console I/O through BIOS instead of BDOS.

#define CC_FCX          // Support for user number in filenames.
#define CC_FCX_DIR      // Support for named user numbers - needs CC_FCX and DirToDrvUsr().

#define CC_FOPEN_A	// Enable modes "a" and "ab" for fopen.
#define CC_FREAD	// Include fread().
#define CC_FWRITE	// Include fwrite().
#define CC_FGETS	// Include fgets().

#define CC_NO_MUL       // Exclude support for MULTIPLICATION.
#define CC_NO_DIV       // Exclude support for DIVISION and MODULUS.
#define CC_NO_SWITCH    // Exclude support for SWITCH.

#define CC_NO_ARGS      // Exclude support for ARGC and ARGV.
*/
/* Standard MESCC library
   ----------------------
   You must include this library;
   it contains some defs. and the runtime.
*/
#include <mescc.h>

/* Standard MESCC libraries
   ------------------------
   Leave only what you need.
*/
//#include <alloc.h>
//#include <atexit.h>
//#include <bsearch.h>
#include <clock.h>
#include <conio.h>
#include <cpm.h>
//#include <ctype.h>
//#include <fileio.h>
//#include <fprintf.h>
//#include <mem.h>
#include <printf.h>
//#include <qsort.h>
//#include <rand.h>
//#include <setjmp.h>
//#include <sprintf.h>
//#include <stdbool.h>
//#include <string.h>
//#include <z80.h>

#ifdef CC_REDIR
	#include <redir.h>
#endif

/* Project libraries
   -----------------
*/
#include "timer.h"

/* Project defs.
   -------------
*/
#define APP_NAME    "Timer"
#define APP_VERSION "v1.00 / 10 Oct 2018"
#define APP_COPYRGT "(c) 2018 FloppySoftware"
#define APP_INFO    "Timer library demonstration."
#define APP_USAGE   "timer"

/* Program entry
   -------------
*/
main()
{
	int timerSeconds, timerMinutes;
	int time, newTime;

	header();

	check_cpm();

	if((timerSeconds = Timer(1, TIMER_ENABLE, timer_seconds)) < 0) {
		error("Can't create timer for seconds.");
	}

	if((timerMinutes = Timer(60, TIMER_ENABLE, timer_minutes)) < 0) {
		error("Can't create timer for minutes.");
	}

	printf("Press any key to start.\n\n");

	while(!input())
		;

	printf("Press any key to finish.\n\n");

	time = get_time();

	while(!input()) {

		if((newTime = get_time()) != time) {
			time = newTime;

			TimerLoop();
		}
	}

	printf("\n\n");
}

/* Check CP/M version
   ------------------
*/
check_cpm()
{
	int cpm;

	cpm = bdos_hl(BF_OSVER, 0x0000);

	if((cpm & 0xFF00) || (cpm & 0x00FF) < 0x30) {
		error("Needs CP/M 3.0 or higher.");
	}
}

/* Check keyboard input
   --------------------
*/
input()
{
	if(kbhit()) {
		getch();

		return 1;
	}

	return 0;
}

/* Get CP/M 3 time (only seconds, really)
   --------------------------------------
*/
get_time()
{
	BYTE clk[CLOCK_LEN];

	getclock(clk);

	return clk[4];
}

/* Timer callback for seconds
   --------------------------
*/
timer_seconds(timerIndex)
int timerIndex;
{
	putchar('.');
}

/* Timer callback for minutes
   --------------------------
*/
timer_minutes(timerIndex)
int timerIndex;
{
	putchar('\n');
}

/* Show header
   -----------
*/
header()
{
	printf("%s %s - %s\n\n", APP_NAME, APP_VERSION, APP_COPYRGT);
	printf("%s\n\n", APP_INFO);
}

/* Print error and exit
   --------------------
*/
error(msg)
char *msg;
{
	printf("%s: %s\n", APP_NAME, msg);

	exit(-1);
}

