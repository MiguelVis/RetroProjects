/*	sx_head.c

	HEAD for SamaruX.

	Print first lines from files.

	Copyright (c) 2016 Miguel Garcia / FloppySoftware.

	Usage:

	head [-nNUMBER] [file ...]

	Options:

	-nNUMBER: Print n lines.

	Examples:

	head -n6 myletter.doc resume.txt
	head calendar.txt

	Changes:

	06 Jun 2016 : v1.00 : Built-in and external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_HEAD

#else

#include "samarux.h"

#define HeadMain main

#endif

HeadMain(argc, argv)
int argc, argv[];
{
	char *pch;
	int lines, i, retcode, argx;

	/* Default values */

	lines = 10;  /* Print # lines */

	/* Parse command line arguments */

	for(i = 1; i < argc; ++i)
	{
		pch = argv[i];

		if(*pch == '-')  /* Look for -options */
		{
			if(*(++pch) == 'n')
			{
				lines = atoi(++pch);

				if(lines < 1 || lines > 32000)
					return Error("Bad # of lines");
			}
			else
				return ErrorOpt();
		}
		else  /* Look for files */
			break;
	}

	argx = i;

	/* Process files */

	if(argx == argc)
		retcode = HeadOut("-", lines);
	else
	{
		while(argx < argc)
		{
			if((retcode = HeadOut(argv[argx++], lines)))
				break;
		}
	}

	/* Success or failure */

	return retcode;
}

HeadOut(fn, lines)
char *fn; int lines;
{
	FILE *fp;
	int ch;

	/* Open the file */

	if(fn[0] == '-' && fn[1] == 0)
		fp = stdin;
	else if((fp = fopen(fn, "r")) == NULL)
		return ErrorOpen();

	/* Read and print the lines */

	while(lines) {

		/* Read char. */

		ch = fgetc(fp);

		/* EOL ? */

		if(ch == '\n')
			--lines;
		else if(ch == EOF)
			break;
		
		/* Print char. */

		putchar(ch);
	}

	/* Close the file */

	if(fp != stdin)
		fclose(fp);

	/* Success */

	return 0;
}
