/*	cat.c

	Unix-like cat for CP/M.

	(Con)catenate files.

	Copyright (c) Miguel I. Garcia Lopez.

	29 May 2007 : 1.00 : Version 1.00.
	16 Dec 2014 : 2.00 : Samarux version.
	08 Jan 2016 : 2.10 : Reworked.

	Notes:

	Supports redirection and user numbers in filenames - ie:

	cat jan.txt feb.txt mar.txt > a2:report.txt

	Supports - as filename for stdin.
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
#include <conio.h>
#include <fileio.h>
#include <redir.h>
#include <fprintf.h>

/* Project defs.
   -------------
*/
#define APP_NAME    "cat"
#define APP_VERSION "v2.10 / 08 Jan 2016"
#define APP_COPYRGT "(c) 2007-2016 FloppySoftware"
#define APP_USAGE   "cat [filename...]"

main(argc, argv)
int argc, argv[];
{
	int i;

	if(argc == 1)
		dump("-");
	else
	{
		for(i = 1; i < argc; ++i)
			dump(argv[i]);
	}
}

dump(fn)
char *fn;
{
	FILE *fp;
	char ch;

	if(*fn == '-' && !fn[1])
		fp = stdin;
	else if((fp = fopen(fn, "r")) == NULL)
		error("Opening", fn);

	while((ch = fgetc(fp)) != EOF)
		putchar(ch);

	if(fp != stdin)
		fclose(fp);
}

error(msg, fn)
char *msg, *fn;
{
	fprintf(stderr, "%s: %s <%s>.\n", APP_NAME, msg, fn);

	exit(-1);
}

