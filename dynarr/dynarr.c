/*	dynarr.c

	Test.

	Copyright (c) 2017 Miguel Garcia / FloppySoftware

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

		cc dynarr
		ccopt dynarr
		zsm dynarr
		hextocom dynarr

	Usage:

		dynarr

	Revisions:

		11 Jul 2017 : Start.
*/

/* Defines for MESCC libraries
   ---------------------------
*/
//#define CC_STDIO        // Support for stdin, stdout, stderr.
//#define CC_REDIR        // Support for command line redirection - needs CC_STDIO.

//#define CC_CONIO_BIOS   // Console I/O through BIOS instead of BDOS.

//#define CC_FCX          // Support for user number in filenames.
//#define CC_FCX_DIR      // Support for named user numbers - needs CC_FCX and DirToDrvUsr().

//#define CC_FOPEN_A	// Enable modes "a" and "ab" for fopen.
//#define CC_FREAD	// Include fread().
//#define CC_FWRITE	// Include fwrite().
//#define CC_FGETS	// Include fgets().

//#define CC_NO_MUL       // Exclude support for MULTIPLICATION.
//#define CC_NO_DIV       // Exclude support for DIVISION and MODULUS.
//#define CC_NO_SWITCH    // Exclude support for SWITCH.

#define CC_NO_ARGS      // Exclude support for ARGC and ARGV.

/* Standard MESCC library
   ----------------------
*/
#include <mescc.h>

/* Standard MESCC libraries
   ------------------------
   Leave only what you need.
*/
#include <alloc.h>
//#include <atexit.h>
//#include <bsearch.h>
//#include <clock.h>
#include <conio.h>
//#include <cpm.h>
//#include <ctype.h>
//#include <fileio.h>
//#include <fprintf.h>
#include <mem.h>
#include <printf.h>
//#include <qsort.h>
//#include <rand.h>
//#include <setjmp.h>
//#include <sprintf.h>
//#include <stdbool.h>
#include <string.h>
//#include <z80.h>

#ifdef CC_REDIR
	#include <redir.h>
#endif

/* Project libraries
   -----------------
*/
#include "dynarr.h"

/* Project defs.
   -------------
*/
#define APP_NAME    "dynarr"
#define APP_VERSION "v1.00 / 12 Jul 2017"
#define APP_COPYRGT "(c) 2017 FloppySoftware"
#define APP_INFO    "This program does something."
#define APP_USAGE   "dynarr"

#define MAX_ARR     10

/* Project globals
   ---------------
*/
DYNARR *dyn;

/* Program entry
   -------------
*/
main()
{
	if(!(dyn = DyAlloc())) {
		error("Not enough memory.");
	}

	DyAddStr(dyn, "This is the entry #0.");
	DyAddStr(dyn, "This is the entry #1.");
	DyAddStr(dyn, "This is the entry #2.");

	DyDel(dyn, 0);

	//Dyset(dyn, "The above entry should be NULL. This is #2.", 2);

	DyPrint(dyn);

	DyFree(dyn);
}

/* Print error and exit
   --------------------
*/
error(msg)
char *msg;
{
#ifdef CC_REDIR
	fprintf(stderr, "%s: %s\n", APP_NAME, msg);
#else
	printf("%s: %s\n", APP_NAME, msg);
#endif

	exit(-1);
}

