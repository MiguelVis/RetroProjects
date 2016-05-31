/*	BinToAsm.c

	Converts a binary file into an assembler file for Z80.

	Copyright (C) 2004-2015 Miguel I. Garcia Lopez, FloppySoftware

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

	To compile with MESCC:

	cc bintoasm
	ccopt bintoasm.zsm
	zsm bintoasm
	hextocom bintoasm

	Revisions:

	22 Feb 2004 : v1.00
	03 Apr 2007 : v1.01 / Minor changes.
	10 Apr 2007 : Output 8 bytes in a line instead of 16.
	15 May 2007 : v1.02 / Added title & usage help.
	04 Sep 2015 : v1.03 / Amended some messages and comments.
*/

#include <mescc.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <fileio.h>
#include <printf.h>
#include <fprintf.h>

#define VERSION "1.03 / 04 Sep 2015\n\n(c) 2004-2015 FloppySoftware"

FILE *fpi, *fpo;

int data, chpos;
unsigned counter;

main(argc, argv)
int argc, argv[];
{
	printf("BinToAsm v%s\n\n", VERSION);

	if(argc!=3)
	{
		printf("Usage: bintoasm binfile asmfile\n");
		exit(1);
	}

	if((fpi=fopen(argv[1],"rb"))==NULL)
		error("Opening input file");

	if((fpo=fopen(argv[2],"w"))==NULL)
		error("Opening output file");

	counter=chpos=0;

	while((data=fgetc(fpi))!=EOF)
	{
		if(!chpos)
			fprintf(fpo, " DEFB");

		fprintf(fpo, " %03d", data);

		if(++chpos!=8)
			fprintf(fpo, ",");
		else
		{
			fprintf(fpo, "\n");
			chpos=0;
		}

		++counter;
	}

	printf("%d bytes\n", counter);

	if(fclose(fpi))
		error("Closing input file");

	if(fclose(fpo))
		error("Closing output file");
}

error(txt)
char *txt;
{
	printf("ERROR: %s\n", txt);
	exit(1);
}
