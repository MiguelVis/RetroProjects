/*	hextobin.c

	Converts a HEX file into a binary file.
	Can be used to generate a COM file for CP/M if it is ORGed to 0x0100.

	(C) 2007-2015 Miguel I. Garcia Lopez, FloppySoftware.

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

	Compile with MESCC:

	cc hextobin
	ccopt hextobin.zsm
	zsm hextobin
	hextocom hextobin

	Revisions:

	18 Apr 2007 : v1.00
	21 Apr 2007 : Shows first and last addresses, code size.
	29 Apr 2007 : v1.01
			  Bug showing code size upper than 0x8000 (shows
			  negative number). Changed %d to %u to solve this bug.
	15 Apr 2007 : v1.02 / Change usage text.
	04 Sep 2015 : v1.03 / Modified some messages and comments.
*/

#include <mescc.h>

#include <printf.h>
#include <fileio.h>

#define VERSION "1.03 / 04 Sep 2015\n\n(c) 2007-2015 FloppySoftware"

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef WORD
#define WORD unsigned int
#endif

FILE	*fpi, /* HEX - Input file  */
	*fpo; /* COM - Output file */

BYTE chksum;	/* Checksum of current line */

WORD	adr,		/* Load address */
	ladr,		/* Load address of current line */
	first_adr;	/* First address */

int	lbytes,	/* Number of data bytes of current line */
	line,		/* Number of current line */
	run,		/* Zero to exit program */
	flag_adr;	/* Zero if load address undefined */

/* Show error and exit
*/

error(txt)
char *txt;
{
	printf("ERROR: %s.\n", txt);
	exit(1);
}

/* Show error with line number and exit
*/

errorln(txt)
char *txt;
{
	printf("ERROR in line %d: %s.\n", line, txt);
	exit(1);
}

/* Read hex nibble
*/

BYTE getnib()
{
	int c;

	c=fgetc(fpi);

	if(c>='0' && c<='9')
		return c-'0';
	else if(c>='A' && c<='F')
		return c-'A'+10;
	else if(c==EOF)
		error("Unexpected EOF");
	else
		error("Bad hex digit");
}

/* Read two hex nibbles
*/

BYTE gethex()
{
	BYTE nib, hex;

	nib=getnib();

	chksum+=(hex=(nib << 4) + getnib());

	return hex;
}

/* Write byte
*/

putbyte(byte)
BYTE byte;
{
	if(fputc(byte, fpo)==EOF)
		error("Writing binfile");
}

/* Main
 */

main(argc, argv)
int argc, argv[];
{
	int c;

	printf("HexToBin v%s\n\n", VERSION);

	if(argc!=3)
	{
		printf("Usage: hextobin hexfile binfile\n");
		exit(1);
	}

	if((fpi=fopen(argv[1], "r"))==NULL)
		error("Opening hexfile");

	if((fpo=fopen(argv[2], "wb"))==NULL)
		error("Opening binfile");

	first_adr=adr=0; /* Load address */
	line=run=1; /* Line number and loop variable */
	flag_adr=0; /* Load address undefined */

	while((c=fgetc(fpi))!=EOF && run)
	{
		if(c!=':')
			errorln("Missing colon");

		chksum=0;

		lbytes=gethex(); /* Bytes per line */

		ladr=gethex();
		ladr=(ladr<<8)+gethex(); /* Load adress */

		switch(gethex()) /* Line type */
		{
			case 0 : /* Data */
				if(!lbytes)
					break;

				if(!flag_adr)
				{
					first_adr=adr=ladr;
					++flag_adr;
				}
				else if(adr<ladr)
				{
					while(adr!=ladr)
					{
						putbyte(0);
						++adr;
					}
				}
				else if(adr>ladr)
					errorln("Bad address");

				adr+=lbytes;

				while(lbytes--)
					putbyte(gethex()); 
				break;
			case 1 : /* End */
				run=0;
				break;
			default :
				errorln("Unknown record type");
		}

		gethex(); /* Checksum */

		if(chksum)
			errorln("Bad checksum");

		if((c=fgetc(fpi))!='\n')
			errorln("Missing newline");

		++line;
	}

	if(fclose(fpi))
		error("Closing hexfile");

	if(fclose(fpo))
		error("Closing binfile");

	printf("First address: %04x\n", first_adr);
	printf("Last address:  %04x\n", adr-1);
	printf("Size of code:  %04x (%u dec) bytes\n", adr-first_adr, adr-first_adr);

}
