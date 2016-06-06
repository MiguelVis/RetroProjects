/*	sx_tail.c

	TAIL for SamaruX.

	Print last lines from files.

	Copyright (c) 2015-2016 Miguel Garcia / FloppySoftware.

	Usage:

	tail [-nNUMBER] [file ...]

	Options:

	-nNUMBER: Print n lines.

	Examples:

	tail -n6 myletter.doc resume.txt
	tail calendar.txt

	Changes:

	07 Sep 2015 : v1.00 : Version for CP/M.
	07 Sep 2015 : v1.00 : Version for SamaruX as external command.
	03 Jun 2016 : v1.01 : Built-in and external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_TAIL

#else

#include "samarux.h"

#define TailMain main

#endif

#define TAIL_MX_LINES 16
#define TAIL_MX_LNBUF 80

TailMain(argc, argv)
int argc, argv[];
{
	char *pch;
	WORD *linarr; /* char *linarr[] */
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

				if(lines < 1 || lines > TAIL_MX_LINES)
					return Error("Bad # of lines");
			}
			else
				return ErrorOpt();
		}
		else  /* Look for files */
			break;
	}

	argx = i;

	/* Get memory for lines */

	if((linarr = malloc(lines + lines)) == NULL)
	{
		return ErrorMem();
	}

	for(i = 0; i < lines; ++i)
	{
		if((linarr[i] = malloc(TAIL_MX_LNBUF)) == NULL)
		{
			while(--i >= 0)
			{
				free(linarr[i]);
			}

			free(linarr);

			return ErrorMem();
		}
	}

	/* Process files */

	if(argx == argc)
		retcode = TailOut("-", lines, linarr);
	else
	{
		while(argx < argc)
		{
			if((retcode = TailOut(argv[argx++], lines, linarr)))
				break;
		}
	}

	/* Free memory */

	for(i = 0; i < lines; ++i)
	{
		free(linarr[i]);
	}

	free(linarr);

	/* Success or failure */

	return retcode;
}

TailOut(fn, lines, arr)
char *fn; int lines; WORD *arr; /* char *arr[] */
{
	FILE *fp;
	int i, k, over;

	/* Open the file */

	if(fn[0] == '-' && fn[1] == 0)
		fp = stdin;
	else if((fp = fopen(fn, "r")) == NULL)
		return ErrorOpen();

	/* Read the file */

	i = over = 0;

	while(fgets(arr[i], TAIL_MX_LNBUF, fp) != NULL)
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

#undef TAIL_MX_LINES
#undef TAIL_MX_LNBUF
