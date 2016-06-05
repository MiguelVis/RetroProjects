/*	sx_mem.c

	MEM for Samarux.

	Prints Samarux memory usage.

	Copyright (c) 2015 Miguel I. Garcia Lopez.

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

	mem

	Examples:

	mem

	Changes:

	30 Mar 2015 : 1.00 : 1st version.

	Notes:

	This program will work only with MESCC.
*/

#define SX_MEM

extern char ccfreemem;

MemMain(argc, argv)
int argc, argv[];
{
	BYTE *bdos;
	unsigned int mem_tpa, mem_static, mem_free;

	/* Check arguments */

	if(argc != 1)
		return ErrorPars();

	/* Set globals */

	bdos = 7;

	mem_tpa = (*bdos << 8) - 256;
	mem_static = &ccfreemem - 255;
	mem_free = MemHeap();

	printf("CP/M TPA size: %5u bytes\n", mem_tpa);
	printf("Used static:   %5u bytes\n", mem_static);
	printf("Used dynamic:  %5u bytes\n", mem_tpa - mem_static - mem_free);
	printf("Free dynamic:  %5u bytes\n", mem_free);

	/* Success */

	return 0;
}

MemHeap()
{
	unsigned char *pb;
	unsigned int *pw, free;

	free = 0;

	for(pb = xm_top; pb != xm_end; pb += *pw + 3)
	{
		pw = pb;

		if(!pb[2])
			free += *pw;
	}

	return free;
}

