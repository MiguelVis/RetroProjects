/*	echo.c

	Unix-like echo for CP/M.

	Echo its arguments to stdout.

	Copyright (c) Miguel I. Garcia Lopez.

	17 Dec 2014 : 1.00 : 1st version.
	08 Jan 2016 : 2.00 : Reworked.

	Notes:

	Supports redirection and user numbers in filenames - ie:

	echo Hello World > a2:echo.txt
*/

/* Defines for MESCC libraries
   ---------------------------
*/
#define CC_STDIO        // Support for stdin, stdout, stderr.
#define CC_REDIR        // Support for command line redirection - needs CC_STDIO.

#define CC_FCX          // Support for user number in filenames.

#define CC_FILEIO_SMALL // Exclude fread(), fwrite() and fgets().

#define CC_NO_DIV       // Exclude support for DIVISION and MODULUS.
#define CC_NO_SWITCH    // Exclude support for SWITCH.

/* Standard MESCC library
   ----------------------
*/
#include <mescc.h>

/* Standard MESCC libraries
   ------------------------
*/
#include <conio.h>
#include <fileio.h>
#include <redir.h>

/* Project defs.
   -------------
*/
#define APP_NAME    "echo"
#define APP_VERSION "v2.00 / 08 Jan 2016"
#define APP_COPYRGT "(c) 2014-2016 FloppySoftware"
#define APP_USAGE   "echo [arg...]"

main(argc, argv)
int argc, argv[];
{
	int i;

	for(i = 1; i < argc; ++i)
	{
		putstr(argv[i]);

		if(i != argc - 1)
			putchar(' ');
	}

	putchar('\n');
}
