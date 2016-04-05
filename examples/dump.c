/*	dump.c

	Print contents of a file in byte and ascii format.

	(c) 2000-2015 Miguel I. Garcia Lopez, FloppySoftware.

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 2 of the License, or (at your
	option) any later version.

	This program is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

	Usage:

	dump filename

	To compile with MESCC:

	cc dump
	ccopt dump.zsm
	zsm dump
	hextocom dump

	Revisions:

	?? ??  2000 : v1.00
	10 Nov 2013 : v1.20
	04 Sep 2015 : v1.21 : Amended some messages and comments.
*/

/* LIBRARIES
*/

#include <mescc.h>
#include <conio.h>
#include <fileio.h>

/* DEFs
*/

#define VERSION "Dump v1.21 / 04 Sep 2015\n\n(c) 2000-2015 FloppySoftware"

/* PROGRAM
*/

main(argc,argv)
int argc;
int argv[];
{
	int	i,		/* Counter */
		offset;		/* Offset in file */

	FILE	*fp;		/* Channel for file */

	char	buffer[16];	/* Buffer for input file */

	/* Check right number of parameters */

	if(argc != 2)
	{
		puts(VERSION);
		puts("\nUsage: dump fname");
		return 1;
	}

	/* Open file in binary mode */

	if((fp = fopen((argv[1]),"rb")) == NULL)
		error("Can't open file");

	/* Initialize variables */

	offset=0;

	/* Start */

	while(fread(buffer, 16, 1, fp) == 1)
	{
		/* Print offset */

		puthex4(offset); putchar(' '); putchar(':'); putchar(' ');

		/* Print data in hexadecimal format */

		for(i = 0; i < 16; ++i)
		{
			puthex2(buffer[i]); putchar(' ');
		}

		/* Separator */

		putchar(':'); putchar(' ');

		/* Print data in ascii format */

		for(i = 0; i < 16; ++i)
		{
			if(buffer[i] > 31 && buffer[i] < 128)
				putchar(buffer[i]);
			else
				putchar('.');
		}

		/* End of line */

		putchar('\n');

		/* Update offset */

		offset += 16;
	}

	fclose(fp);

	return 0;
}

error(txt)
char *txt;
{
	puts(txt);

	exit(1);
}

puthex4(word)
int word;
{
	puthex2(word >> 8);
	puthex2(word);
}

puthex2(byte)
char byte;
{
	puthex1(byte >> 4);
	puthex1(byte);
}

puthex1(nibble)
char nibble;
{
	nibble &= 0x0F;

	putchar(nibble < 10 ? '0' + nibble : 'A' + nibble - 10);
}

