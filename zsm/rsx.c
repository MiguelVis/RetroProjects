/*	rsx.c

	Manage RSXs.

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

		cc rsx
		ccopt rsx
		zsm rsx
		hextocom rsx

	Usage:

		rsx -option... filename [filename]

	Revisions:

		18 Nov 2016 : Start.
		19 Nov 2016 : Added Info option.
		20 Nov 2016 : Added Add option.
		23 Nov 2016 : Check file close error in close_tmp(). Added get_temp_fname().
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
//#include <alloc.h>
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
#include <string.h>
//#include <xprintf.h>
//#include <z80.h>

/* Project defs.
   -------------
*/
#define APP_NAME    "RSX"
#define APP_VERSION "v1.00 / 18 Nov 2016"
#define APP_COPYRGT "(c) 2016 FloppySoftware"
#define APP_USAGE   "rsx -option filename [filename]"

#define COM_HEADER_SIZE 256
#define RSX_HEADER_SIZE 256

#define TMP_FNAME   "rsx.$$$"

#define TASK_INFO   0  // Show information about COM or RSX file
#define TASK_ADD    1  // Add RSX
#define TASK_DEL    2  // Delete RSX

/* Globals
   -------
*/
int task = -1;

char com_fname[FILENAME_MAX];
char rsx_fname[FILENAME_MAX];
char tmp_fname[FILENAME_MAX];

FILE *com_fp;
FILE *rsx_fp;
FILE *tmp_fp;

BYTE com_header[COM_HEADER_SIZE];
BYTE rsx_header[RSX_HEADER_SIZE];

BYTE *com_h_magic;
WORD *com_h_fsize;
BYTE *com_h_init;
BYTE *com_h_loader;
BYTE *com_h_rsxs;

WORD *rsx_h_psize;

BYTE rsx_name[8];

/* Program entry
   -------------
*/
main(argc, argv)
int argc;
unsigned int argv[]; // char *argv[] - unsupported by MESCC (yet?)
{
	char *pch; int i, args;

	// Show usage if there are no arguments
	if(argc == 1)
	{
		usage();
	}

	// Default values
	args = 0;

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
						case 'I' : task = TASK_INFO; args = 1; break;
						case 'A' : task = TASK_ADD;  args = 2; break;
						default  : error_bad_option(); break;
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

	// Check task and filename/s
	if(task == -1 || argc - i != args)
	{
		error_bad_cmdline();
	}

	// Take COM filename
	get_fname(com_fname, argv[i++], "COM");

	if(i != argc)
	{
		// Take RSX filename
		get_fname(rsx_fname, argv[i], "RSX");
	}

	// Setup values
	com_h_magic  = com_header;
	com_h_fsize  = com_header + 0x01;
	com_h_init   = com_header + 0x03;
	com_h_loader = com_header + 0x0D;
	com_h_rsxs   = com_header + 0x0F;

	rsx_h_psize  = rsx_header + 0x01;

	// Do task
	switch(task)
	{
		case TASK_INFO :
			do_info();
			break;
		case TASK_ADD :
			get_temp_fname(com_fname);
			do_add();
			break;
		default :
			error("Bad task"); // FIXME - internal error
			break;
	}

	// Success

	exit(0);
}

/* Task: Information
   -----------------
*/
do_info()
{
	int i, k;
	BYTE *pb;
	WORD *pw;

	// Open file
	open_com();

	// Read COM header
	if(!read_com_header())
	{
		error("The file hasn't a COM header");
	}

	// Close file
	close_com();

	// Print info.
	printf("COM size:    %u bytes\n", *com_h_fsize);
	printf("Loader flag: %s\n", (*com_h_loader ? "Yes" : "No"));
	printf("# of RSXs:   %d of 15\n", *com_h_rsxs);

	// Print RSXs info.
	if(*com_h_rsxs)
	{
		putchar('\n');

		for(i = 0; i < *com_h_rsxs; ++i)
		{
			pb = com_header + 0x10 + 16 * i;
			
			printf("#%2d: ", i);

			for(k = 0; k < 8; ++k)
			{
				putchar(pb[6 + k]);
			}

			printf("  Offset: %04xh  Size: %5u bytes%s\n", *(pw = pb), *(pw = pb + 2), (pb[4] ? "  Unbanked CP/M" : ""));
		}
	}
}

/* Task: Add RSX
   -------------
*/
do_add()
{
	unsigned int com_rsize, rsx_rsize;
	BYTE *rsx_record;
	WORD *rsx_r_offset, *rsx_r_psize;
	BYTE *rsx_r_banked, *rsx_r_name;
	BYTE rec_buf[128], rec_rsx[128];
	int i;

	// Open RSX file
	open_rsx();

	// Read RSX header
	read_rsx_header();

	// Set RSX filename
	set_rsx_name();

	// Open COM file
	open_com();

	// Get COM file size in CP/M records
	com_rsize = get_rsize(com_fname);

	// Get RSX file size in CP/M records
	rsx_rsize = get_rsize(rsx_fname);

	// Read COM header
	if(!read_com_header())
	{
		// The COM file has not a header
		memset(com_header, 0, COM_HEADER_SIZE);

		// Setup COM header
		*com_h_magic = 0xC9;
		*com_h_fsize = com_rsize * 128;
		*com_h_init  = 0xC9;

		// Adjust file size
		com_rsize += 2;

		// Reopen the COM file (a byte has been already read) -- FIXME -- find a better way to do this!
		close_com(); open_com();
	}

	// Check if the RSX is already attached
	if(find_rsx_rec() != -1)
	{
		error("RSX with same name already attached");
	}

	// Find a free RSX record
	if(*com_h_rsxs == 15)
	{
		// No free RSX records
		error("No more free RSX records");
	}

	// Compute address of RSX record
	rsx_record = com_header + 0x10 + *com_h_rsxs * 16;

	rsx_r_offset = rsx_record;
	rsx_r_psize  = rsx_record + 2;
	rsx_r_banked = rsx_record + 4;
	rsx_r_name   = rsx_record + 6;

	// Reset RSX record contents -- FIXME not needed if only it's used this program, but...
	memset(rsx_record, 0, 16);

	// Set RSX offset in COM file
	*rsx_r_offset = com_rsize * 128;

	// Set size of RSX code + initialised data
	*rsx_r_psize = *rsx_h_psize;

	// Set RSX name
	memcpy(rsx_r_name, rsx_name, 8);

	// Read first RSX record
	if(fread(rec_rsx, 128, 1, rsx_fp) != 1)
	{
		error_read();
	}

	// Set banked field -- GENCOM does this
	*rsx_r_banked = rec_rsx[15];

	// Update COM header
	++(*com_h_rsxs);

	// Patch first RSX record - GENCOM does this
	if(rec_rsx[15])
	{
		// RSX for non banked systems; remove RSX on program exit
		rec_rsx[14] = 0xFF;
	}

	rec_rsx[10] = 6; // Address of next RSX in memory
	rec_rsx[12] = 7; // Address of previous RSX in memory
	rec_rsx[24] = 0; // Loader flag

	// Open TMP file
	open_tmp();

	// Write COM header
	if(fwrite(com_header, COM_HEADER_SIZE, 1, tmp_fp) != 1)
	{
		error_write();
	}

	// Write COM file contents to TMP -- FIXME -- try to speed up this!
	for(com_rsize -= 2; com_rsize; --com_rsize)
	{
		if(fread(rec_buf, 128, 1, com_fp) != 1)
		{
			error_read();
		}

		if(fwrite(rec_buf, 128, 1, tmp_fp) != 1)
		{
			error_write();
		}
	}

	// Write RSX header -- NOTE: The RSX header is not written to the COM file
	//if(fwrite(rsx_header, RSX_HEADER_SIZE, 1, tmp_fp) != 1)
	//{
	//	error_write();
	//}

	// Write first RSX record (patched) to TMP
	if(fwrite(rec_rsx, 128, 1, tmp_fp) != 1)
	{
			error_write();
	}

	// Write RSX file contents to TMP
	for(rsx_rsize -= 3; rsx_rsize; --rsx_rsize)
	{
		if(fread(rec_buf, 128, 1, rsx_fp) != 1)
		{
			error_read();
		}

		if(fwrite(rec_buf, 128, 1, tmp_fp) != 1)
		{
			error_write();
		}
	}

	// Close TMP file
	close_tmp();

	// Close COM file
	close_com();

	// Close RSX file
	close_rsx();

	// Delete original COM file
	if(remove(com_fname))
	{
		error("Can't delete file");
	}

	// Rename TMP file to COM filename
	if(rename(tmp_fname, com_fname))
	{
		error("Can't rename file");
	}
}

/* Open COM file for reading
   -------------------------
*/
open_com()
{
	if(!(com_fp = fopen(com_fname, "rb")))
	{
		error_open();
	}
}

/* Close COM file
   --------------
*/
close_com()
{
	fclose(com_fp);
}

/* Read COM header
   ---------------
   Return 1 if header found, else 0.
*/
read_com_header()
{
	// Check the very first byte of the file
	switch(fgetc(com_fp))
	{
		case 0xC9 :
			com_header[0] = 0xC9;

			if(fread(com_header + 1, COM_HEADER_SIZE - 1, 1, com_fp) != 1)
			{
				error_read();
			}

			return 1;
		case EOF  :
			error("Empty file");
			break;

		default   :
			return 0;
	}
}

/* Open RSX file for reading
   -------------------------
*/
open_rsx()
{
	if(!(rsx_fp = fopen(rsx_fname, "rb")))
	{
		error_open();
	}
}

/* Close RSX file
   --------------
*/
close_rsx()
{
	fclose(rsx_fp);
}

/* Read RSX header
   ---------------
*/
read_rsx_header()
{
	// Check the very first byte of the file
	if(!fgetc(rsx_fp))
	{
		rsx_header[0] = 0;

		if(fread(rsx_header + 1, RSX_HEADER_SIZE - 1, 1, rsx_fp) != 1)
		{
			error_read();
		}

		return;
	}

	error("The file hasn't a RSX header");
}

/* Set RSX name from RSX filename
   ------------------------------
*/
set_rsx_name()
{
	BYTE *pb;
	int i;

	// Compute start of filename
	pb = strchr(rsx_fname, ':');
	pb = (pb ? ++pb : rsx_fname);

	// Copy upto '.' character
	for(i = 0; pb[i] != '.'; ++i)
	{
		rsx_name[i] = pb[i];
	}

	// Pad with spaces
	while(i < 8)
	{
		rsx_name[i++] = ' ';
	}
}

/* Open TMP file for writing
   -------------------------
*/
open_tmp()
{
	if(!(tmp_fp = fopen(tmp_fname, "wb")))
	{
		error_open();
	}
}

/* Close TMP file
   --------------
*/
close_tmp()
{
	if(fclose(tmp_fp))
	{
		error_close();
	}
}

/* Find RSX record in COM header
   -----------------------------
   Return # of record, else -1.
*/
find_rsx_rec()
{
	BYTE *rsx_record;
	int i;

	// Compute address of first RSX record in COM header
	rsx_record = com_header + 0x10;

	// Check all used RSX record
	for(i = 0; i < *com_h_rsxs; ++i)
	{
		// Check RSX name
		if(!memcmp(rsx_record + 6, rsx_name, 8))
		{
			// Found
			return i;
		}

		// Next record
		rsx_record += 16;
	}

	// Not found
	return -1;
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
	printf("Manage RSXs - Resident System eXtensions.\n\n");
	printf("Usage: %s\n\n", APP_USAGE);
	printf("-I fname[.com]              Print information\n");
	printf("-A fname[.com] fname[.rsx]  Add RSX to a COM file\n");

	/***

	printf("-U fname[.com] fname[.rsx]  Update RSX in a COM file\n");
	printf("-D fname[.com] fname[.rsx]  Delete RSX in a COM file\n");
	printf("-G fname[.com] fname[.rsx]  Get RSX from a COM file\n");

	***/

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