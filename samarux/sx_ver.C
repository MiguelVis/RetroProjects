/*	sx_ver.c

	VER for Samarux.

	Prints SamaruX and CP/M version and facts.

	Copyright (c) 2007 - 2015 Miguel I. Garcia Lopez.

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

	ver

	Examples:

	ver

	Changes:

	24 Dec 2014 : 1.00 : 1st version.
	24 Aug 2015 : 1.01 : Print SamaruX version.
	05 Jun 2016 : 1.02 : Added support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_VER

#else

#include "samarux.h"

#define VerMain main

#endif

VerMain(argc, argv)
int argc, argv[];
{
	WORD ver;
	BYTE *bios, *bdos;

	/* Check arguments */

	if(argc != 1)
		return ErrorPars();
	
	/* Print SamaruX version */
	
	printf("SamaruX v%d.%02d / %s\n\n", SX_VERSION, SX_RELEASE, SX_APPDATE);

	/* Get OS type and version */

	bios = 2;
	bdos = 7;
	
	ver = bdos_hl(BF_OSVER, 0xCACA);

	/* Print OS name and version */

	printf("%cP/M v%d.%d\n\n", \
		ver & 0xFF00 ? 'M' : 'C', (ver >> 4) & 0xF, ver & 0xF);

	/* Print BDOS, BIOS and TPA data */

	printf("BDOS address: %02x00h\n",*bdos);
	printf("BIOS address: %02x00h\n\n",*bios);

	printf("TPA size:     %u bytes\n", (*bdos << 8)-256);

	/* Success */

	return 0;
}
