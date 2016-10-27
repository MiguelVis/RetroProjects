/*	readline.c

	Some tests for the ReadLine library.

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

		cc readline
		ccopt readline
		zsm readline
		hextocom readline

	Usage:

		readline

	Revisions:

		26 Oct 2016 : Start.
		27 Oct 2016 : Added tests for extended mode.
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
*/
//#include <alloc.h>
//#include <atexit.h>
//#include <bsearch.h>
//#include <clock.h>
#include <conio.h>
//#include <cpm.h>
#include <ctype.h>
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

/* Project libraries
   -----------------
*/
#define RL_EXTENDED
#include "readline.h"

/* Project defs.
   -------------
*/

/* Project globals
   ---------------
*/
char buf[129];

/* Program entry
   -------------
*/
main()
{
	Input("Regular text", 16);

#ifdef RL_EXTENDED

	InputEx("Upper case", 16, RL_UCASE);
	InputEx("Lower case", 16, RL_LCASE);
	InputEx("Decimal number", 16, RL_RESET | RL_DECIMAL);
	InputEx("Password", 16, RL_PASSWORD);
	InputEx("Upper case | Password", 16, RL_PASSWORD | RL_UCASE);

#endif

}


Input(prompt, maxlen)
char *prompt;
int maxlen;
{
	printf("%s > ", prompt);

	ReadLine(buf, maxlen);

	printf("\n\t\"%s\"\n\n", buf);
}

#ifdef RL_EXTENDED

InputEx(prompt, maxlen, mode)
char *prompt;
int maxlen;
unsigned int mode;
{
	printf("%s > ", prompt);

	ReadLineEx(buf, maxlen, mode);

	printf("\n\t\"%s\"\n\n", buf);
}

#endif

