/*	log.c

	Small test for log.h.

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

		cc log
		ccopt log
		zsm log
		hextocom log

	Usage:

		log

	Revisions:

		27 Dec 2016 : Done.
*/

/* Defines for MESCC libraries
   ---------------------------
*/
#define CC_STDIO        // Support for stdin, stdout, stderr.
#define CC_FOPEN_A	// Enable modes "a" and "ab" for fopen.
#define CC_NO_ARGS      // Exclude support for ARGC and ARGV.

/* Standard MESCC library
   ----------------------
*/
#include <mescc.h>

/* MESCC libraries
   ---------------
*/
#include <fileio.h>
#include <fprintf.h>

/* Project libraries
   -----------------
*/
#include "log.h"

/* Program entry
   -------------
*/
main()
{
	// Log configuration
	LogSetFile("debug.log");
	LogSetLevel(LOG_DEBUG);
	LogSetEcho(TRUE);

	LogWrite("LOG", LOG_DEBUG,   "Test #1");
	LogWrite("LOG", LOG_DEBUG,   "Test #2");
	LogWrite("LOG", LOG_WARNING, "Test #3");

	LogSetLevel(LOG_ERROR);

	LogWrite("LOG", LOG_DEBUG,   "Test #4 -- ignored");
	LogWrite("LOG", LOG_ERROR,   "Test #5");
}

