/*	bmptopcw.c

	Convert BMP image files for use in the Amstrad PCW.

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

		cc bmptopcw
		ccopt bmptopcw
		zsm bmptopcw
		hextocom bmptopcw

	Usage:

		bmptopcw [-option...] filename

	Options:

		-C  : Write output as a C source file (.C).
		-A  : Write output as an assembler file (.ZSM).
		-B  : Write output as a binary file (.PCW).     <-- DEFAULT
		-H+ : Include header data in the output.        <-- DEFAULT
		-H- : Don't include header data in the output.
		-R+ : Resize height to half.
		-R- : Don't resize height to half.              <-- DEFAULT

		Header data is as follows:

			BYTE - width in PCW columns
			BYTE - height in PCW rows

	Revisions:

		16 Nov 2016 : ----- : Start.
		17 Nov 2016 : v1.00 : Working.
*/

/* Standard MESCC library
   ----------------------
*/
#include <mescc.h>

/* Standard MESCC libraries
   ------------------------
*/
#include <alloc.h>
#include <ctype.h>
#include <fileio.h>
#include <fprintf.h>
#include <printf.h>
#include <string.h>

/* Project defs.
   -------------
*/
#define APP_NAME    "BmpToPcw"
#define APP_VERSION "v1.00 / 17 Nov 2016"
#define APP_COPYRGT "(c) 2016 FloppySoftware"
#define APP_USAGE   "bmptopcw [-option...] filename"

#define OUT_C       0  // C language
#define OUT_ASM     1  // Assembler
#define OUT_BIN     2  // Binary

#define YES         1
#define NO          0

/* Globals
   -------
*/
int out_type      = OUT_BIN;  // Option -B
int inc_header    = YES;      // Option -H+
int resize_height = NO;       // Option -R-

char fname_in[FILENAME_MAX];  // Source filename
char fname_out[FILENAME_MAX]; // Destination filename

char label[FILENAME_MAX];     // Label for some output types

FILE *fp_in;      // FP for source file
FILE *fp_out;     // FP for destination file

int bmp_width;    // Bitmap width in pixels
int bmp_height;   // Bitmap height in pixels
int bmp_lnsize;   // Bitmap line size
BYTE *bmp_buf;    // Bitmap buffer

int pcw_rows;     // PCW rows
int pcw_cols;     // PCW columns

/* Program entry
   -------------
*/
main(argc, argv)
int argc;
unsigned int argv[]; // char *argv[] - unsupported by MESCC (yet?)
{
	char *pch; int i, flag;

	// Show usage if there are no arguments
	if(argc == 1)
	{
		usage();
	}

	// Check arguments in command line
	for(i = 1; i < argc; ++i)
	{
		pch = argv[i];

		if(*pch == '-')
		{
			// It's an option
			if(pch[1])
			{
				if(!pch[2])
				{
					// -X
					switch(pch[1])
					{
						case 'C' : out_type = OUT_C;   break;
						case 'A' : out_type = OUT_ASM; break;
						case 'B' : out_type = OUT_BIN; break;
						default  : error_bad_option(); break;
					}
				}
				else if((pch[2] == '+' || pch[2] == '-') && !pch[3])
				{
					// -X+  |  -X-
					flag = (pch[2] == '+' ? YES : NO);

					switch(pch[1])
					{
						case 'H' : inc_header    = flag; break;
						case 'R' : resize_height = flag; break;
						default  : error_bad_option();   break;
					}
				}
				else
				{
					error_bad_option();
				}
			}
			else
			{
				error_bad_option();
			}
		}
		else
		{
			// It's not an option: maybe a filename?
			break;
		}
	}

	// Take filename
	if(i == argc - 1)
	{
		// Don't include the file type
		if(strchr(argv[i], '.'))
		{
			error("Don't include the file type, please");
		}

		// Can hold the file type?
		if(strlen(argv[i]) + 4 >= FILENAME_MAX)
		{
			error("Filename is too long");
		}

		// Source filename
		strcpy(fname_in, argv[i]);
		strcat(fname_in, ".BMP");

		// Destination filename
		strcpy(fname_out, argv[i]);

		switch(out_type)
		{
			case OUT_C   : strcat(fname_out, ".C");   break;
			case OUT_ASM : strcat(fname_out, ".ZSM"); break;
			default      : strcat(fname_out, ".PCW"); break;
		}

		// Label for C and ASM output types
		strcpy(label, argv[i]);

		for(i = 0; i < strlen(label); ++i)
		{
			label[i] = tolower(label[i]);
		}
	}
	else
	{
		error("Bad command line");
	}

	// Do it the hard work
	process();

	// Success
	exit(0);
}

/* Process input file
   ------------------
*/
process()
{
	int row, col, x, y, ay;
	BYTE *pb, b;

	// Open BMP file
	if(!(fp_in = fopen(fname_in, "rb")))
	{
		error("Can't open source file");
	}

	// Skip some bytes
	skip_bytes(18);

	// Get width in pixels
	if((bmp_width = read_word()) % 8)
	{
		error("Width in pixels must be a multiply of 8");
	}

	// Skip some bytes
	skip_bytes(2);

	// Get height in pixels
	bmp_height = read_word();

	if(resize_height && bmp_height % 16)
	{
		error("Height in pixels must be a multiply of 16");
	}
	else if(bmp_height % 8)
	{
		error("Height in pixels must be a multiply of 8");
	}

	// Calculate BMP line size in bytes (multiply of 4)
	bmp_lnsize = ((bmp_width + 31) / 32) * 4;

	// Skip some bytes
	skip_bytes(38);

	// Calculate PCW columns
	pcw_cols = (bmp_width + 7) / 8;

	// Calculate PCW rows
	pcw_rows = (resize_height ? (bmp_height + 15) / 16 : (bmp_height + 7) / 8);

	// Allocate buffer
	if(!(bmp_buf = malloc(pcw_rows * 8 * pcw_cols)))
	{
		error("No memory for bitmap buffer");
	}

	// Open destination file
	if(!(fp_out = fopen(fname_out, "wb")))
	{
		error("Can't open destination file");
	}

	// Write header of file
	switch(out_type)
	{
		case OUT_C   :
			write_string("unsigned char "); write_string(label); write_string("[] = {\r\n");

			if(inc_header)
			{
				write_byte('\t'); write_decimal(pcw_cols); write_string(",\r\n");
				write_byte('\t'); write_decimal(pcw_rows); write_string(",\r\n");
			}
			break;
		case OUT_ASM :
			write_string(label); write_byte('\r'); write_byte('\n');

			if(inc_header)
			{
				write_string("\tdefb "); write_decimal(pcw_cols); write_byte('\r'); write_byte('\n');
				write_string("\tdefb "); write_decimal(pcw_rows); write_byte('\r'); write_byte('\n');
			}
			break;
		default      :
			if(inc_header)
			{
				write_byte(pcw_cols);
				write_byte(pcw_rows);
			}
			break;
	}

	// Read bitmap data and store it in reverse order
	for(y = bmp_height; y; --y)
	{
		if(resize_height && (y & 1))
		{
			skip_bytes(pcw_cols);
		}
		else
		{
			ay = (resize_height ? y / 2 : y);

			pb = bmp_buf + (ay - 1) * pcw_cols;

			for(x = pcw_cols; x; --x)
			{
				*pb++ = read_byte();
			}
		}

		skip_bytes(bmp_lnsize - pcw_cols);
	}

	// Write bitmap data to destination file
	for(y = 0; y < bmp_height; y += (resize_height ? 16 : 8))
	{
		if(!(resize_height && (y & 1)))
		{
			ay = (resize_height ? y / 2 : y);

			for(col = 0; col < pcw_cols; ++col)
			{
				// Start of line
				switch(out_type)
				{
					case OUT_C   :
						write_byte('\t');
						break;
					case OUT_ASM :
						write_string("\tdefb ");
						break;
					default      :
						break;
				}

				// Line data
				for(row = 0; row < 8; ++row)
				{
					b = bmp_buf[ay * pcw_cols + col + row * pcw_cols];

					switch(out_type)
					{
						case OUT_C   :

							// P'abajo

						case OUT_ASM :
							write_decimal(b);

							if(row != 7)
							{
								write_byte(','); write_byte(' ');
							}
							break;
						default      :
							write_byte(b);
							break;
					}
				}

				// End of line
				switch(out_type)
				{
					case OUT_C   :
						if(y != bmp_height - (resize_height ? 16 : 8) || col != pcw_cols - 1)
						{
							write_byte(',');
						}

						// P'abajo

					case OUT_ASM :
						write_byte('\r'); write_byte('\n');
						break;
					default      :
						break;
				}
			}
		}
	}

	// Write footer of file
	switch(out_type)
	{
		case OUT_C   :
			write_string("};\r\n");
			break;
		case OUT_ASM :

			// P'abajo

		default      :
			break;
	}

	// Close files
	fclose(fp_in);

	if(fclose(fp_out) == EOF)
	{
		error("Can't close destination file");
	}
}

/* Read a byte from input
   ----------------------
*/
read_byte()
{
	int byte;

	if((byte = fgetc(fp_in)) == EOF)
	{
		error("Can't read source file");
	}

	return byte;
}

/* Read a word from input
   ----------------------
*/
read_word()
{
	return read_byte() | (read_byte() << 8);
}

/* Skip some bytes from input
   --------------------------
*/
skip_bytes(n)
int n;
{
	while(n--)
	{
		read_byte();
	}
}

/* Write a byte to output
   ----------------------
*/
write_byte(b)
int b;
{
	if(fputc(b, fp_out) == EOF)
	{
		error("Can't write destination file");
	}
}

/* Write a word to output
   ----------------------
*/
//write_word(w)
//unsigned int w;
//{
//	write_byte(w & 0xFF); write_byte((w >> 8) & 0xFF);
//}

/* Write a string to output
   ------------------------
*/
write_string(s)
char *s;
{
	while(*s)
	{
		write_byte(*s++);
	}
}

/* Write decimal number
   --------------------
*/
write_decimal(n)
int n;
{
	fprintf(fp_out, "%3d", n);
}

/* Write padded decimal number
   ---------------------------
*/
//write_3decimal(n)
//int n;
//{
//	fprintf(fp_out, "%3d", n);
//}

/* Show usage and exit
   -------------------
*/
usage()
{
	printf("%s %s - %s\n\n", APP_NAME, APP_VERSION, APP_COPYRGT);
	printf("Convert a BMP image file for use in the Amstrad PCW.\n\n");
	printf("Usage: %s\n\n", APP_USAGE);
	printf("-C  : Write as C source         - file type .C\n");
	printf("-A  : Write as assembler source - file type .ZSM\n");
	printf("-B  : Write as binary           - file type:.PCW\n");
	printf("-H? : Include header data in the output?\n");
	printf("-R? : Resize height to half?\n\n");
	printf("Default options are (+ for YES, - for NO):\n\n");
	printf("\t-B -H+ -R-\n\n");
	printf("Source file type is .BMP\n\n");
	printf("Example: bmptopcw -C -H- mylogo\n");

	exit(0);
}

/* Print error and exit
   --------------------
*/
error(msg)
char *msg;
{
	printf("%s: %s\n", APP_NAME, msg);

	exit(-1);
}

/* Error: bad option
   -----------------
*/
error_bad_option()
{
	error("Bad option");
}

