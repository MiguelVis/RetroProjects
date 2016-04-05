/*	cpmver.c

	Shows CP/M or MP/M version and tpa size.

	Copyright (C) 2007-2016 Miguel I. Garcia Lopez

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

	Revisions:

	30 Apr 2000 : v1.00
	04 Jan 2001 : v1.01 Minor changes.
	11 Feb 2001 : v1.02 Show also bdos & bios start, and tpa size.
	03 Apr 2007 : v1.03 Minor changes.
	05 Apr 2016 : v1.04 Minor changes.
*/

/* Some defs. to save memory
*/
#define CC_NO_SWITCH // To exclude SWITCH code.
#define CC_NO_ARGS   // To exclude ARGC & ARGV code.

/* Standard Libraries
*/
#include <mescc.h>
#include <string.h>
#include <printf.h>

/* The real code
*/
main()
{
	unsigned int ver, tpasize;
	unsigned char *bios, *bdos;

	// Some standard locations
	bios = 2;
	bdos = 7;

	// Get OS type and version

	ver = cpmver();

	// Print OS name and version

	printf("%cP/M v%d.%d\n\n", \
		ver & 0xFF00 ? 'M' : 'C', (ver >> 4) & 0xF, ver & 0xF);

	printf("BDOS address: %02x00h\n", *bdos);
	printf("BIOS address: %02x00h\n\n", *bios);

	tpasize = (*bdos << 8) - 256;

	printf("TPA size:     %u bytes\n", tpasize);
}

/* unsigned int cpmver(void)
   -------------------------

   Return OS type and version calling to BDOS.

   Return: H = 0 for CP/M, 1 for MP/M.
           L = 0x22H for v2.2, 0x31 for v3.1, etc.
*/

#asm
cpmver:
	LD  C,12
	JP  5
#endasm
