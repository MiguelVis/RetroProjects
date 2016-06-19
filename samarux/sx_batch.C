/*	sx_batch.c

	BATCH for Samarux.

	Execute commands from a text file.

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

	batch file [param ...]

	Examples:

	batch backup
	batch backup A: Z:

	Changes:

	25 Dec 2014 : 1.00 : 1st version.
	28 Dec 2014 : 1.01 : $0 is the batch file name. $1 to $9 are the arguments.
	30 Dec 2014 : 1.02 : Adapted to shell changes.
	08 Jan 2015 : 1.03 : Avoid execution of empty lines. Adapted to changes in shell.
	19 Jan 2015 : 1.04 : Adapted to shell changes.
	15 Feb 2015 : 1.05 : Re-written to support flow control.
	23 Feb 2015 : 1.05 : Added BatchArgc() and BatchJump().
	                     Added support for nexted batch commands.
	08 Mar 2015 : 1.06 : Increased BATCH_LINES to 64.
	09 Mar 2015 : 1.07 : Default type for file name.
	14 Jun 2016 : 1.08 : Increased BATCH_LINES to 72.
	19 Jun 2016 : 1.09 : Added BatchShift() for shift command.
*/

#define SX_BATCH

#define BATCH_MAX   3      /* Max. number of nexted batch */
#define BATCH_LINES 72     /* Max. number of lines in batch file */
#define BATCH_TYPE  ".sx"  /* Default type for file name */

int batch_argc;    /* # of arguments */
int *batch_argv;   /* Arguments - char *batch_argv[] */
int *batch_ln;     /* File lines - char *batch_ln[] */
int batch_lines;   /* # of file lines */
int batch_pc;      /* Next file line to execute (modified by goto) */

BatchMain(argc, argv)
int argc, argv[];
{
	int code;
	int old_argc, *old_argv, *old_ln, old_lines;

	/* Nexted batch? */
	if(sv_batch == BATCH_MAX)
		return Error("Too much nexted batch commands");

	/* Check arguments */
	if(argc < 2 || argc > 11)
		return ErrorPars();

	/* Tell system we are executing batch */
	++sv_batch;

	/* Save current batch values */
	/* It's not necessary to save batch_pc value */
	old_argc = batch_argc;
	old_argv = batch_argv;
	old_ln = batch_ln;
	old_lines = batch_lines;

	/* Set $0 ... $9 variables */
	batch_argc = argc - 1;
	batch_argv = argv + 1;

	batch_argv[0] = BatchFn(argv[1]);

	/* Read file */
	if((batch_ln = ReadFile(batch_argv[0], BATCH_LINES, 127)) != NULL)
	{
		/* # of lines read */
		batch_lines = KeyCount1st(batch_ln, BATCH_LINES);

		/* Execute command lines */
		BatchExe();

		/* Free buffers */
		KeyFree(batch_ln, BATCH_LINES);

		/* Success */
		code = 0;
	}
	else
		code = 1;

	/* This batch has ended */
	--sv_batch;

	/* Restore old batch values */
	batch_argc = old_argc;
	batch_argv = old_argv;
	batch_ln = old_ln;
	batch_lines = old_lines;

	/* Return success or failure */
	return code;
}

BatchExe()
{
	int i;

	batch_pc = -1;

	for(i = 0; i < batch_lines; ++i)
	{
		if(batch_ln[i] == NULL)
			break;

		Execute(batch_ln[i]);

		/* At this point, a flow control command
		   could have changed batch_pc */
		if(batch_pc != -1)
		{
			i = batch_pc - 1;
			batch_pc = -1;
		}
	}
}

BatchArg(num)
int num;
{
	if(num >= 0 && num < batch_argc)
		return batch_argv[num];

	return NULL;
}

BatchArgc()
{
	char *s;

	s = "?";

	/* # of args. without including $0 */

	*s = '0' + batch_argc - 1;

	return s;
}

BatchJump(label)
char *label;
{
	if((batch_pc = KeyFind(batch_ln, batch_lines, label)) != -1)
		return 0;

	return 1;
}

BatchShift()
{
	int i;

	if(batch_argc > 1) {
		for(i = 2; i < batch_argc; ++i)
			batch_argv[i - 1] = batch_argv[i];

		--batch_argc;
	}
}

BatchFn(fn)
char *fn;
{
	char *pfn;

	/* Try to add the default type */

	if((pfn = AddTypeToFn(BATCH_TYPE, fn)) != NULL)
		return pfn;

	return fn;
}

#undef BATCH_MAX
#undef BATCH_LINES
#undef BATCH_TYPE

