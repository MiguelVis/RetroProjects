/*	pwd.c

	Unix-like pwd for CP/M.

	Prints current working directory.

	Copyright (c) Miguel I. Garcia Lopez.

	18 Dec 2014 : 1.00 : 1st version.
	08 Jan 2016 : 2.00 : Reworked.

	Notes:

	Supports redirection and user numbers in filenames - ie:

	pwd > a2:pwd.txt
*/

/* Defines for MESCC libraries
   ---------------------------
*/
#define CC_STDIO        // Support for stdin, stdout, stderr.
#define CC_REDIR        // Support for command line redirection - needs CC_STDIO.

#define CC_FCX          // Support for user number in filenames.

#define CC_FILEIO_SMALL // Exclude fread(), fwrite() and fgets().

#define CC_NO_SWITCH    // Exclude support for SWITCH.

/* Standard MESCC library
   ----------------------
*/
#include <mescc.h>

/* Standard MESCC libraries
   ------------------------
*/
#include <cpm.h>
#include <printf.h>
#include <fileio.h>
#include <redir.h>

/* Project defs.
   -------------
*/
#define APP_NAME    "pwd"
#define APP_VERSION "v2.00 / 08 Jan 2016"
#define APP_COPYRGT "(c) 2014-2016 FloppySoftware"
#define APP_USAGE   "pwd"

main(argc, argv)
int argc, argv[];
{
	int drv, usr;

	drv = bdos_a(BF_GETDRV, 0xCACA);
	usr = bdos_a(BF_USER,   0xFFFF);

	printf("%c:/%02d\n", drv + 'A', usr);
}
