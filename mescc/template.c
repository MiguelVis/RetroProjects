/*	program.c

	Short description.

	Copyright (c) 2016 Miguel Garcia / FloppySoftware

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

	Author's contact:

		www.floppysoftware.es
		cpm-connections.blogspot.com
		floppysoftware@gmail.com

	To compile with MESCC:

		cc program
		ccopt program
		zsm program
		hextocom program

	Usage:

		program [-options...] [filename...]

	Revisions:

		06 Jan 2016 : Last update of template.
		02 Jun 2016 : Added stderr support in usage(). Added error(). Added some comments.
		20 Jul 2016 : Removed CC_FILEIO_SMALL. Added CC_FOPEN_A, CC_FREAD, CC_FWRITE, CC_FGETS.
*/

/* Defines for MESCC libraries
   ---------------------------
   Leave only what you need.
*/
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
#include <alloc.h>
#include <atexit.h>
#include <bsearch.h>
#include <clock.h>
#include <conio.h>
#include <cpm.h>
#include <ctype.h>
#include <fileio.h>
#include <fprintf.h>
#include <mem.h>
#include <printf.h>
#include <qsort.h>
#include <rand.h>
#include <redir.h>
#include <setjmp.h>
#include <sprintf.h>
#include <string.h>
#include <xprintf.h>
#include <z80.h>

/* Project libraries
   -----------------
*/
#include "program.h"

/* Project defs.
   -------------
*/
#define APP_NAME    "program"
#define APP_VERSION "v1.00 / 06 Jan 2016"
#define APP_COPYRGT "(c) 2016 FloppySoftware"
#define APP_USAGE   "program [-options...] [filename...]"

/* Program entry
   -------------
*/
main(argc, argv)
int argc;
unsigned int argv[]; // char *argv[] - unsupported by MESCC (yet?)
{
	char *pch; int i;

	// Show usage if there are no arguments

	if(argc == 1)
		usage();

	// Check options in command line

	for(i = 1; i < argc; ++i)
	{
		pch = argv[i];

		if(*pch == '-')
		{
			// Option

			switch(pch[1])
			{
				case 'A' : break;
				...
				default  : error("Bad option"); break;
			}
		}
		else
		{
			// Filename

		}
	}

	// Success

	exit(0);
}

/* Show usage and exit
   -------------------
*/
usage()
{
#ifdef CC_REDIR
	fprintf(stderr, "%s %s\n\n", APP_NAME, APP_VERSION);
	fprintf(stderr, "%s\n\n", APP_COPYRGT);
	fprintf(stderr, "Usage: %s\n", APP_USAGE);
#else
	printf("%s %s\n\n", APP_NAME, APP_VERSION);
	printf("%s\n\n", APP_COPYRGT);
	printf("Usage: %s\n", APP_USAGE);
#endif

	exit(0);
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
