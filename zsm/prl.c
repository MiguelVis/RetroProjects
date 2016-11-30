/*	prl.c

	Make PRL file.

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

		cc prl
		ccopt prl
		zsm prl
		hextocom prl

	Usage:

		prl filename[.com] filename[.com] filename[.prl]

	Revisions:

		23 Nov 2016 : Start.
*/

/* Defines for MESCC libraries
   ---------------------------
*/
#define CC_FREAD	// Include fread()
#define CC_FWRITE	// Include fwrite()

/* Standard MESCC library
   ----------------------
*/
#include <mescc.h>

/* Standard MESCC libraries
   ------------------------
*/
#include <alloc.h>
//#include <atexit.h>
//#include <bsearch.h>
//#include <clock.h>
//#include <conio.h>
#include <cpm.h> // For get_rsize()
//#include <ctype.h>
#include <fileio.h>
//#include <fprintf.h>
#include <mem.h>
#include <printf.h>
//#include <qsort.h>
//#include <rand.h>
//#include <redir.h>
//#include <setjmp.h>
//#include <sprintf.h>
//#include <string.h>
//#include <xprintf.h>
//#include <z80.h>

/* Project defs.
   -------------
*/
#define APP_NAME    "PRL"
#define APP_VERSION "v1.00 / 18 Nov 2016"
#define APP_COPYRGT "(c) 2016 FloppySoftware"
#define APP_USAGE   "prl filename[.com] filename[.com] filename[.prl]"

#define TMP_FNAME   "prl.$$$"

#define PRL_HEADER_SIZE 256

#define PRL_H_BYTES  1  // WORD: Bytes in program image (code + initialised data)
#define PRL_H_BSS    4  // WORD: Bytes reserved for unitialised data
#define PRL_H_LADR   7  // WORD: Load address
#define PRL_H_CBASE  10 // WORD: BIOS link

/* Globals
   -------
*/
int task = -1;

char cm1_fname[FILENAME_MAX];
char cm2_fname[FILENAME_MAX];
char prl_fname[FILENAME_MAX];
char tmp_fname[FILENAME_MAX];

FILE *cm1_fp;
FILE *cm2_fp;
FILE *tmp_fp;

BYTE prl_header[PRL_HEADER_SIZE];

/* Program entry
   -------------
*/
main(argc, argv)
int argc;
unsigned int argv[]; // char *argv[] - unsupported by MESCC (yet?)
{
	// Show usage if there are no arguments
	if(argc == 1)
	{
		usage();
	}

	// Check arguments
	if(argc != 4)
	{
		error_bad_cmdline();
	}

	// Take COM filename #1
	get_fname(cm1_fname, argv[1], "COM");

	// Take COM filename #2
	get_fname(cm2_fname, argv[2], "COM");

	// Take PRL filename
	get_fname(prl_fname, argv[3], "PRL");

	// Take TMP filename
	get_temp_fname(prl_fname);

	// Process
	process();

	// Success
	exit(0);
}

/* Process files
   -------------
*/
process()
{
	WORD recs, *pw;
	int i, k, b, h, bitmap_size;
	BYTE rec_bf1[128], rec_bf2[128], *pb_bitmap, byte, mask;

	// Open COM filename #1
	if(!(cm1_fp = fopen(cm1_fname, "rb")))
	{
		error_open();
	}

	// Open COM filename #2
	if(!(cm2_fp = fopen(cm2_fname, "rb")))
	{
		error_open();
	}

	// Open TMP filename
	if(!(tmp_fp = fopen(tmp_fname, "wb")))
	{
		error_open();
	}

	// Get COM file size in CP/M records
	if(!(recs = get_rsize(cm1_fname)))
	{
		error("File is empty");
	}

	// Check valid file size
	if(recs > 512)
	{
		error("File is too big");
	}

	// Check size coincidence between both COM files
	if(recs != get_rsize(cm2_fname))
	{
		error("Files differ in size");
	}

	// Alloc memory for relocation bitmap
	bitmap_size = recs * 16;

	if(!(pb_bitmap = malloc(bitmap_size)))
	{
		error("No memory for relocation bitmap");
	}

	// Initialize PRL header
	*(pw = prl_header + PRL_H_BYTES) = recs * 128;

	// Write PRL header
	if(fwrite(prl_header, PRL_HEADER_SIZE, 1, tmp_fp) != 1)
	{
		error_write();
	}

	// Write COM file #1 contents to TMP -- FIXME -- try to speed up this!
	for(i = b = 0; i < recs; ++i)
	{
		if(fread(rec_bf1, 128, 1, cm1_fp) != 1)
		{
			error_read();
		}

		if(fwrite(rec_bf1, 128, 1, tmp_fp) != 1)
		{
			error_write();
		}

		if(fread(rec_bf2, 128, 1, cm2_fp) != 1)
		{
			error_read();
		}

		for(k = 0; k < 128; k += 8)
		{
			mask = 128;

			for(h = byte = 0; h < 8; ++h)
			{
				if(rec_bf1[k + h] != rec_bf2[k + h])
				{
					byte |= mask;
				}

				mask = mask >> 1;
			}

			pb_bitmap[b++] = byte;
		}
	}
	
	// Write PRL relocation bitmap to TMP
	if(fwrite(pb_bitmap, bitmap_size, 1, tmp_fp) != 1)
	{
		error_write();
	}

	// Close COM filename #1
	fclose(cm1_fp);

	// Close COM filename #2
	fclose(cm2_fp);

	// Close TMP filename
	if(fclose(tmp_fp))
	{
		error_close();
	}

	// Delete old PRL file if exists
	remove(prl_fname);

	// Rename TMP file to PRL filename
	if(rename(tmp_fname, prl_fname))
	{
		error("Can't rename file");
	}
}

/* Copy a filename and add a file type if needed
   ---------------------------------------------
*/
get_fname(dest, fname, ftype)
char *dest, *fname, *ftype;
{
	int len_type;

	len_type = (strchr(fname, '.') ? 0 : strlen(ftype) + 1);

	if(strlen(fname) + len_type >= FILENAME_MAX)
	{
		error_fname();
	}

	strcpy(dest, fname);

	if(len_type)
	{
		strcat(dest, "."); strcat(dest, ftype);
	}
}

/* Generate temporary filename
   ---------------------------
*/
get_temp_fname(fname)
char *fname;
{
	char *pc;
	int k;

	pc = strchr(fname, ':');

	k = (pc ? pc - fname + 1 : 0);

	pc = TMP_FNAME;

	if(k + strlen(pc) >= FILENAME_MAX)
	{
		error_fname();
	}

	if(k)
	{
		memcpy(tmp_fname, fname, k);
	}

	strcpy(tmp_fname + k, pc);
}

/* Get file size in records of 128 bytes each
   ------------------------------------------
*/
get_rsize(fname)
char *fname;
{
	BYTE fcb[36];
	WORD *pw;

	if(!setfcb(fname, fcb))
	{
		if(!bdos_a(BF_FSIZE, fcb))
		{
			return *(pw = fcb + 33);
		}
	}

	error("Can't get file size");
}

/* Show usage and exit
   -------------------
*/
usage()
{
	printf("%s %s - %s\n\n", APP_NAME, APP_VERSION, APP_COPYRGT);
	printf("Make PRLs - Page ReLocatable files.\n\n");
	printf("Usage: %s\n\n", APP_USAGE);

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

/* Error: bad command line
   -----------------------
*/
error_bad_cmdline()
{
	error("Bad command line");
}

/* Error: filename too long
   ------------------------
*/
error_fname()
{
	error("Filename is too long");
}

/* Error: can't open file
   ----------------------
*/
error_open()
{
	error("Can't open file");
}

/* Error: can't close file
   -----------------------
*/
error_close()
{
	error("Can't close file");
}

/* Error: reading file
   -------------------
*/
error_read()
{
	error("Reading file");
}

/* Error: writing file
   -------------------
*/
error_write()
{
	error("Writing file");
}

