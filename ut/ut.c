/*	ut.c

	Testing the unitary tests library for MESCC.

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

		cc mtest
		ccopt mtest
		zsm mtest
		hextocom mtest

	Usage:

		ut

	Revisions:

		18 Oct 2016 : Start of work.
		20 Oct 2016 : Updated with all supported tests.

*/

// Defines for MESCC libraries
// ---------------------------

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

// Standard MESCC library
// ----------------------

#include <mescc.h>

// Standard MESCC libraries
// ------------------------

//#include <alloc.h>
//#include <atexit.h>
//#include <bsearch.h>
//#include <clock.h>
#include <conio.h>
//#include <cpm.h>
//#include <ctype.h>
//#include <fileio.h>
//#include <fprintf.h>
//#include <mem.h>
#include <printf.h>
//#include <qsort.h>
//#include <rand.h>
//#include <redir.h>
//#include <setjmp.h>
//#include <sprintf.h>
#include <string.h>
//#include <xprintf.h>
//#include <z80.h>

// Project libraries
// -----------------

#include "ut.h"

// Program entry
// -------------

main()
{
	// Start tests
	UtStart();

	// Boolean tests
	UtTrue(1 == 1, "It should be true");
	UtFalse(4 == 6, "It should be false");

	// Int tests
	UtIntEq(1024, 1024, "They should be equal");
	UtIntNotEq(1024, 2048, "They should not be equal");

	// Unsigned int tests
	UtUintEq(1024, 1024, "They should be equal");
	UtUintNotEq(1024, 2048, "They should not be equal");

	// String tests
	UtStrNull(NULL, "It should be NULL");
	UtStrNotNull("String", "It should not be NULL");
	UtStrEq("String", "String", "They should be equal");
	UtStrNotEq("String #1", "String #2", "They should not be equal");

	// End tests
	UtEnd();
}

