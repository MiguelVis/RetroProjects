/*	sx_man.c

	MAN for Samarux.

	Manual.

	Copyright (c) 2007 - 2015 Miguel I. Garcia Lopez.

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 2, or (at your option) any
	later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

	Usage:

	man [[file] topic | ALL | TOPICS]

	Examples:

	man           -- Print available topics in samarux.man
	man cat       -- Print cat topic help in samarux.man
	man ALL       -- Print all contents in samarux.man
	man wp TOPICS -- Print available topics in wp.man
	man wp fi     -- Print fi topic help in wp.man

	Changes:

	19 Dec 2014 : 1.00 : 1st version.
	24 Dec 2014 : 1.01 : Minor changes.
	30 Dec 2014 : 1.02 : Added \n to the start of sections.
	04 Jan 2015 : 1.03 : Added NOTES section.
	08 Jan 2015 : 1.04 : Changed man file constant. Minor changes. Added comments.
	12 Jan 2015 : 1.05 : Added option to print all contents.
	17 Feb 2015 : 1.06 : Close file if not enough memory for buffer.
	26 Feb 2015 : 1.07 : Use of TermCols().
	08 Mar 2015 : 1.08 : Added support for MANPATH environment variable.
	08 Apr 2015 : 1.09 : Added support for external manual files.
	29 Nov 2015 : 1.10 : Added support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_MAN

#else

#include "samarux.h"

#define ManMain main

#endif

#define MAN_FILE "samarux.man"

ManMain(argc, argv)
int argc, argv[];
{
	FILE *fp;
	char *bf, *fn, *topic;
	int k, err, flg_found, flg_output, opt_all, cols;

	/* Default values */

	fn = MAN_FILE; opt_all = flg_output = flg_found = 0; cols = TermCols();

	/* Check arguments */

	switch(argc)
	{
		case 1 : /* man */
			topic = NULL;
			break;
		case 2 : /* man topic */
			topic = argv[1];
			break;
		case 3 : /* man file topic */
			fn = argv[1];
			topic = argv[2];
			break;
		default: /* error */
			return ErrorPars();
	}

	/* Check for special topic */

	if(topic != NULL)
	{
		if(!strcmp(topic, "TOPICS"))
			topic = NULL;
		else if(!strcmp(topic, "ALL"))
			++opt_all;
	}

	/* Open */

	if((fp = fopen(ManFn(fn), "r")) == NULL)
		return ErrorOpen();

	/* Buffer */

	if((bf = malloc(cols)) == NULL)
	{
		fclose(fp);

		return ErrorMem();
	}

	/* Process */

	if(opt_all)
		++flg_found;

	err = 0;

	while(1)
	{
		/* Read line */

		if(fgets(bf, cols, fp) == NULL)
			break;

		k = strlen(bf);

		if(bf[k - 1] != '\n')
		{
			err = ErrorTooLong(); break;
		}

		bf[k - 1] = 0;

		/* Print available topics */

		if(topic == NULL)
		{
			if(*bf == '@')
				puts(bf + 1);

			continue;
		}
	
		/* Print topic help */

		if(flg_found)
		{
			/* Check end of topic section */

			if(*bf == '@')
			{
				if(opt_all)
					continue;

				break;
			}				

			/* Print section titles */

			if(*bf == '$')
			{
				if(flg_output)
					putchar('\n');

				if(!strcmp(bf + 1, "NM"))
					puts("NAME");
				else if(!strcmp(bf + 1, "SY"))
					puts("SYNOPSIS");
				else if(!strcmp(bf + 1, "DE"))
					puts("DESCRIPTION");
				else if(!strcmp(bf + 1, "OP"))
					puts("OPTIONS");
				else if(!strcmp(bf + 1, "EX"))
					puts("EXAMPLES");
				else if(!strcmp(bf + 1, "NT"))
					puts("NOTES");
				else
				{
					err = Error("Unknown $section"); break;
				}
			}
			else if(*bf != '#')
			{
				putchar('\t'); puts(bf); ++flg_output;
			}

			continue;
		}

		/* Find topic section */

		if(*bf == '@')
		{
			if(!strcmp(bf + 1, topic))
				++flg_found;
		}
	}

	/* Close */

	fclose(fp);

	/* Free buffer */

	free(bf);

	/* Failure? */

	if(topic != NULL && !flg_found && !err)
		return Error("Topic not found");

	/* Return success or failure */

	return err;
}

ManFn(fn)
char *fn;
{
	char *pfn;

	/* If MANPATH exists and seems to be a valid path, use it */

	if((pfn = AddPathToFn(EnvGet("MANPATH"), fn)) != NULL)
		fn = pfn;

	/* Try to add the default type */

	if((pfn = AddTypeToFn(".man", fn)) != NULL)
		fn = pfn;

	/* Return the (possibly) modified file name */

	return fn;
}

#undef MAN_FILE

