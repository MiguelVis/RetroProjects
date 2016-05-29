/*	tail.c

	Unix-like tail for CP/M.

	Print last lines from files.

	Copyright (c) Miguel I. Garcia Lopez.

	Usage:

	tail [-nNUMBER] [file...]

	Example:

	tail -n6 myletter.doc resume.txt
	tail calendar.txt

	Revisions:

	07 Sep 2015 : v1.00 : Version for CP/M.
	07 Sep 2015 : v1.00 : Version for SamaruX as external command.
	29 May 2016 : v1.01 : Reworked.
*/

/* Defines for MESCC libraries
   ---------------------------
*/
#define CC_STDIO	// Support for stdin, stdout, stderr
#define CC_REDIR	// Support for CP/M command line redirection

#define CC_FCX          // Support for user number in filenames.

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
#include <printf.h>
#include <fprintf.h>
#include <redir.h>

/* Project defs.
   -------------
*/
#define APP_NAME    "tail"
#define APP_VERSION "v1.01 / 29 May 2016"
#define APP_COPYRGT "(c) 2015-2016 FloppySoftware"
#define APP_USAGE   "tail [-nNUMBER] [file...]"

#define MAX_LINES 16
#define MAX_LNBUF 80

/* The program
   -----------
*/
main(argc, argv)
int argc, argv[];
{
	char *pch, *fn;
	WORD *linarr; /* char *linarr[] */
	int lines, i, retcode, argx;

	/* No command line arguments: show usage and exit */

	if(argc == 1)
	{
		fprintf(stderr, "%s %s\n\n", APP_NAME, APP_VERSION);
		fprintf(stderr, "%s\n\n", APP_COPYRGT);
		fprintf(stderr, "Usage: %s\n", APP_USAGE);

		return 0;
	}

	/* Default values */

	lines = 10;  /* Print # lines */
	fn = NULL;   /* Filename */

	/* Parse command line arguments */

	for(i = 1; i < argc; ++i)
	{
		pch = argv[i];

		if(*pch == '-')  /* Look for -options */
		{
			if(*(++pch) == 'N')
			{
				lines = atoi(++pch);

				if(lines < 1 || lines > MAX_LINES)
					error("Bad # of lines");
			}
			else
				error("Bad option");
		}
		else  /* Look for files */
			break;
	}

	argx = i;

	/* Get memory for lines */

	if((linarr = malloc(lines + lines)) == NULL)
		error("No memory");

	for(i = 0; i < lines; ++i)
	{
		if((linarr[i] = malloc(MAX_LNBUF)) == NULL)
			error("No memory");
	}

	/* Process files */

	if(argx == argc)
		retcode = do_tail("-", lines, linarr);
	else
	{
		while(argx < argc)
		{
			if((retcode = do_tail(argv[argx++], lines, linarr)))
				break;
		}
	}

	/* Success or failure */

	if(retcode)
		error("Can't open file");

	return 0;
}

/* Do tail on a file
   -----------------
*/
do_tail(fn, lines, arr)
char *fn; int lines; WORD *arr; /* char *arr[] */
{
	FILE *fp;
	int i, k, over;

	/* Open the file */

	if(fn[0] == '-' && fn[1] == 0)
		fp = stdin;
	else if((fp = fopen(fn, "r")) == NULL)
		return -1;

	/* Read the file */

	i = over = 0;

	while(fgets(arr[i], MAX_LNBUF, fp) != NULL)
	{
		if(++i == lines)
		{
			i = 0;
			over = 1;
		}
	}

	/* Print the lines */

	if(!over)
	{
		lines = i;
		i = 0;
	}

	k = lines;

	while(k--)
	{
		putstr(arr[i++]); /* The string already has a \n -- well, maybe not */

		if(over && i == lines)
			i = 0;
	}

	/* Close the file */

	if(fp != stdin)
		fclose(fp);

	/* Success */

	return 0;
}

/* Print error and exit
   --------------------
*/
error(msg)
char *msg;
{
	fprintf(stderr, "%s: %s.\n", APP_NAME, msg);
	exit(-1);
}
