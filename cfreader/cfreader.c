/*	cfreader.c

	Simple test for my cfreader.h library.

	Copyright (c) 2019 Miguel Garcia / FloppySoftware

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

	To compile with MESCC under CP/M:

		cc cfreader
		ccopt cfreader
		zsm cfreader
		hextocom cfreader

	Usage:

		cfreader

	Revisions:

		02 Nov 2019 : First version.
*/

/* Defines for MESCC libraries
   ---------------------------
   Leave only what you need.
*/
//#define CC_STDIO      // Support for stdin, stdout, stderr.
//#define CC_REDIR      // Support for command line redirection - needs CC_STDIO.

//#define CC_CONIO_BIOS // Console I/O through BIOS instead of BDOS.

//#define CC_FCX        // Support for user number in filenames - ie: "a01:letter.doc".
//#define CC_FCX_DIR    // Support for named directories (aliases) - needs CC_FCX and DirToDrvUsr() - ie: "docs:letter.doc".

//#define CC_FOPEN_A    // Enable modes "a" and "ab" for fopen().
//#define CC_FREAD      // Include fread().
//#define CC_FWRITE     // Include fwrite().
#define CC_FGETS      // Include fgets().

//#define CC_NO_MUL     // Exclude support for MULTIPLICATION.
//#define CC_NO_DIV     // Exclude support for DIVISION and MODULUS.
//#define CC_NO_SWITCH  // Exclude support for SWITCH.

#define CC_NO_ARGS    // Exclude support for ARGC and ARGV in main().

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
//#include <clock.h>
//#include <conio.h>
//#include <cpm.h>
#include <ctype.h>
#include <fileio.h>
//#include <fprintf.h>
//#include <mem.h>
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
#include "cfreader.h"

/* Project defs.
   -------------
*/
#define APP_NAME    "cfreader"
#define APP_VERSION "v1.00 / 02 Nov 2019"
#define APP_COPYRGT "(c) 2019 FloppySoftware"
#define APP_INFO    "This program tests the cfreader.h library."
#define APP_USAGE   "cfreader"

#define CF_FILE     "cfreader.cf"

/* Program entry
   -------------
*/
main()
{
	int ret, line;

	if((ret = CfReader(CF_FILE, callback, &line)) != CFR_SUCCESS) {
		printf("cfreader: error #%d reading line #%d of file \"%s\".\n\n", ret, line, CF_FILE);
	}
}

/* Callback
*/
callback(line, key, value)
int line; char *key, *value;
{
	printf("Line %4d : %16s : \"%s\"\n", line, key, value);

	return 0;
}

