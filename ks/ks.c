/*	program.c

	Short description.

	Copyright (c) 2016, 2017 Miguel Garcia / FloppySoftware

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

		cc ks
		ccopt ks
		zsm ks
		hextocom ks

	Usage:

		ks tty_name

	Last revision:

	15 Jul 2017
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
#include "ks.h"       // Base
#include "ks_cent.h"  // Center text

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
	if(KsHello(KsGetCode(argv[1])) == -1) {
		printf("Unknown TTY: %s\n", argv[1]); return;
	}

	KsClear();

	KsCenterStr(1, "Hello World!");

	KsPutStr("\n\n");

	KsPutStr("Can Show / Hide Cursor: "); KsPutStr(KsCan(KS_CAN_HIDE) ? "YES" : "NO"); KsPutCh('\n');

	KsPutStr("Can Reverse:            ");

	if(KsCan(KS_CAN_REVERSE)) {
		KsPutStr("YES  "); KsReverse(1); KsPutStr("Reverse"); KsReverse(0);
	}
	else {
		KsPutStr("NO");
	}

	KsPutCh('\n');

	KsPutStr("Can Underline:          ");

	if(KsCan(KS_CAN_UNDERLINE)) {
		KsPutStr("YES  "); KsUnderline(1); KsPutStr("Underline"); KsUnderline(0);
	}
	else {
		KsPutStr("NO");
	}

	KsPutCh('\n');

	// Quit KS
	KsBye();
}


