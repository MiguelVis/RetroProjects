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

	Last revision:

	04 Oct 2016
*/

/* Standard MESCC library
   ----------------------
*/
#include <mescc.h>

/* Dependencies
   ------------
*/
#include <printf.h>
#include <string.h>

/* Project libraries
   -----------------
*/
#include "ks.h"

/* Program entry
   -------------
*/
main(argc, argv)
int argc;
unsigned int argv[]; // char *argv[] - unsupported by MESCC (yet?)
{
	// Check command line arguments
	if(argc != 2) {
		printf("Usage: ks tty_name\n"); return;
	}

	// Start KS
	if(KsInit(KsGetCode(argv[1])) == -1) {
		printf("Unknown TTY: %s\n", argv[1]); return;
	}

	KsClrScr();

	KsCenterStr(1, "Hello World!");

	KsPutStr("\n\n");

	KsPutStr("Can Show / Hide Cursor: "); KsPutStr(KsCanShowCursor() ? "YES" : "NO"); KsPutCh('\n');

	KsPutStr("Can Reverse:            ");

	if(KsCanReverse()) {
		KsPutStr("YES  "); KsReverse(1); KsPutStr("Reverse"); KsReverse(0);
	}
	else {
		KsPutStr("NO");
	}

	KsPutCh('\n');

	KsPutStr("Can Underline:          ");

	if(KsCanUnderline()) {
		KsPutStr("YES  "); KsUnderline(1); KsPutStr("Underline"); KsUnderline(0);
	}
	else {
		KsPutStr("NO");
	}

	KsPutCh('\n');

	// Quit KS
	KsExit();
}
