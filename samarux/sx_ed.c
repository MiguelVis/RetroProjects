/*	sx_ed.c

	ED for Samarux.

	Edit small text files.

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

	ed [file]

	Options:

	None

	Examples:

	ed
	ed mydoc.txt

	Internal commands:

	edit line_number
	insert line_number
	remove line_number
	copy line_number
	paste
	append
	print [line_number [how_many_lines]]
	write [file_name]
	info
	help
	exit

	Changes:

	15 Jan 2015 : 1.00 : 1st version.
	16 Jan 2015 : 1.01 : Removed argument parsing code (now it's done by a Samarux function).
	19 Feb 2015 : 1.02 : Some tasks are now done by Samarux.
	26 Feb 2015 : 1.03 : Use of TermRows() and TermCols().
	30 Mar 2015 : 1.04 : Added copy & paste commands.
	05 Jun 2016 : 1.05 : Added support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_ED

#else

#include "samarux.h"

#define EdMain main

#endif

#define ED_MAX_LINES 48

int *ed_buf;   /* Array of text lines - char *ed_buf[] */
char *ed_lin;  /* Buffer for line editing */
char *ed_copy; /* Buffer for copy & paste commands */
int ed_cols;   /* Max. # of columns (characters) in a line */
char *ed_file; /* File name */

EdMain(argc, argv)
int argc, argv[];
{
	int i, err;

	/* Check arguments */

	if(argc > 2)
		return ErrorPars();

	/* Setup variables */

	ed_cols = TermCols() - 10; /* FIXME */

	ed_lin = ed_copy = ed_file = ed_buf = err = 0;

	/* Create buffers */

	ed_lin = malloc(ed_cols + 1);
	ed_copy = malloc(ed_cols + 1);
	ed_file = malloc(FILENAME_MAX + 1);

	/* Proceed */

	if(ed_lin != NULL && ed_copy != NULL && ed_file != NULL)
	{
		/* Clean copy & paste buffer */

		*ed_copy = 0;

		/* Start editing */

		if(argc == 1)
		{
			/* New file */

			*ed_file = 0;

			if((ed_buf = KeyAlloc(ED_MAX_LINES)) == NULL)
				err = ErrorMem();
		}
		else if(strlen(argv[1]) > FILENAME_MAX)
			err = ErrorFName();
		else
		{
			/* Existent file */

			strcpy(ed_file, argv[1]);

			if((ed_buf = ReadFile(ed_file, ED_MAX_LINES, ed_cols)) == NULL)
				++err;
		}

		if(!err)
			err = EdEditBuf();
	}
	else
		err = ErrorMem();

	/* Free memory */

	if(ed_buf != NULL)
		KeyFree(ed_buf, ED_MAX_LINES);

	free(ed_lin);
	free(ed_file);
	free(ed_copy);

	/* Return success or failure */

	return err;
}

EdEditBuf()
{
	int argv[2]; int err, argc; char *cmd, *ar1;

	/* Show info */

	EdCmInfo();

	err = 0;

	while(1)
	{
		*ed_lin = 0;

		while(!(*ed_lin))
		{
			putstr("ed> "); ReadLine(ed_lin, ed_cols); putchar('\n');

		}

		if((argc = ParseArgs(ed_lin, argv, 2)) == -1)
		{
			Error("Bad quoting or too much arguments"); continue;
		}

		if(!argc)
			continue;

		cmd = argv[0];
		ar1 = argc < 2 ? NULL : argv[1];

		if(EdMatchStr("exit", cmd) && argc == 1)
			break;
		else if(EdMatchStr("edit", cmd) && argc == 2)
			EdCmEdit(ar1);
		else if(EdMatchStr("remove", cmd) && argc == 2)
			EdCmRemove(ar1);
		else if(EdMatchStr("insert", cmd) && argc == 2)
			EdCmInsert(ar1);
		else if(EdMatchStr("copy", cmd) && argc == 2)
			EdCmCopy(ar1);
		else if(EdMatchStr("paste", cmd) && argc == 2)
			EdCmPaste(ar1);
		else if(EdMatchStr("append", cmd) && argc == 1)
			EdCmAppend();
		else if(EdMatchStr("print", cmd) && argc < 3)
			EdCmPrint(ar1);
		else if(EdMatchStr("write", cmd) && argc < 3)
			EdCmWrite(ar1);
		else if(EdMatchStr("info", cmd) && argc == 1)
			EdCmInfo();
		else if(EdMatchStr("help", cmd) && argc == 1)
			EdCmHelp();
		else
			Error("Bad command");
	}

	return err;
}

/* write [file_name] */

EdCmWrite(fn)
char *fn;
{
	FILE *fp; int i, err; char *p, *tmp;

	/* Set file name */

	if(fn == NULL)
	{
		if(*ed_file)
			fn = ed_file;
		else
		{
			Error("No file name"); return;
		}
	}
	else if(strlen(fn) > FILENAME_MAX)
	{
		ErrorFName(); return;
	}

	/* Write buffer to temporary file */

	tmp = "edtmp.txt";

	if((fp = fopen(tmp, "w")) == NULL)
	{
		ErrorOpen(); return;
	}

	for(i = err = 0; i < ED_MAX_LINES && (p = ed_buf[i]) != NULL && !err; ++i)
	{
		while(*p)
		{
			if(fputc(*p++, fp) == EOF)
			{
				++err; break;
			}
		}

		if(!err)
		{
			if(fputc('\n', fp) == EOF)
				++err;
		}
	}

	if(fclose(fp))
		++err;

	if(err)
	{
		
		ErrorWrite(); return;
	}

	/* Delete destination file */

	remove(fn);

	/* Rename temporary file to destination file name */

	if(rename(tmp, fn))
	{
		ErrorRename(); return;
	}

	/* Change file name */

	if(strcmp(fn, ed_file))
	{
		strcpy(ed_file, fn);

		printf("Now file name is %s\n", ed_file);
	}
}


/* help */

EdCmHelp()
{
	puts("Commands:");
	puts("\tedit line_number");
	puts("\tinsert line_number");
	puts("\tremove line_number");
	puts("\tcopy line_number");
	puts("\tpaste line_number");
	puts("\tappend");
	puts("\tprint [line_number]");
	puts("\twrite [file_name]");
	puts("\tinfo");
	puts("\thelp");
	puts("\texit");
}

/* info */

EdCmInfo()
{
	printf("File :  %s\n", *ed_file ? ed_file : "-");
	printf("Lines:  %d/%d\n", EdCountLines(), ED_MAX_LINES);
}

/* edit line_number */

EdCmEdit(ar1)
char *ar1;
{
	int line;

	if((line = EdToLine(ar1)) != -1)
		EdEditLine(line);
}

/* remove line_number */

EdCmRemove(ar1)
char *ar1;
{
	int line, i, max;

	if((line = EdToLine(ar1)) == -1)
		return;

	free(ed_buf[line]);

	max = EdCountLines() - 1;

	for(i = line; i < max; ++i)
		ed_buf[i] = ed_buf[i + 1];

	ed_buf[i] = NULL;
}

/* insert line_number */

EdCmInsert(ar1)
char *ar1;
{
	int line, i, max; char *p;

	if((line = EdToLine(ar1)) == -1)
		return;

	if((max = EdCountLines()) == ED_MAX_LINES)
	{
		Error("Too much lines"); return;
	}

	if((p = malloc(1)) == NULL)
	{
		ErrorMem(); return;
	}

	*p = 0;

	for(i = max /**- 1**/; i > line; --i)
	{
		ed_buf[i] = ed_buf[i - 1];
	}

	ed_buf[line] = p;

	EdEditLine(line);
}

/* copy line_number */

EdCmCopy(ar1)
char *ar1;
{
	int line;

	if((line = EdToLine(ar1)) != -1)
		strcpy(ed_copy, ed_buf[line]);
}

/* paste line_number */

EdCmPaste(ar1)
char *ar1;
{
	int line;

	if((line = EdToLine(ar1)) != -1)
		strcpy(ed_buf[line], ed_copy);
}

/* append */

EdCmAppend()
{
	int line; char *p;

	if((line = EdCountLines()) == ED_MAX_LINES)
	{
		Error("Too much lines"); return;
	}

	if((p = malloc(1)) == NULL)
	{
		ErrorMem(); return;
	}

	*p = 0;

	ed_buf[line] = p;

	EdEditLine(line);
}

/* print [first_line_number] */

EdCmPrint(ar1)
char *ar1;
{
	int i, first, howmany;

	if((first = (ar1 == NULL) ? 0 : EdToLine(ar1)) == -1)
		return;

	howmany = TermRows() - 1; /* FIXME */

	for(i = first; i < ED_MAX_LINES; ++i)
	{
		if(ed_buf[i] == NULL || i == first + howmany)
			break;

		printf("%5d: %s\n", i, ed_buf[i]);
	}
}

EdCountLines()
{
	return KeyCount1st(ed_buf, ED_MAX_LINES);
}

EdToLine(s)
char *s;
{
	int n;

	if(isdigit(*s) && (n = atoi(s)) >= 0)
	{
		if(n < ED_MAX_LINES)
		{
			if(ed_buf[n] != NULL)
				return n;

			Error("Empty line");

			return -1;
		}
	}

	Error("Bad line number");

	return -1;
}

EdEditLine(line)
int line;
{
	int k; char *p;

	strcpy(ed_lin, ed_buf[line]);

	printf("%5d: ", line);

	ReadLine(ed_lin, ed_cols); putchar('\n');

	k = strlen(ed_lin);

	if(k != strlen(ed_buf[line]))
	{
		if((p = malloc(++k)) == NULL)
		{
			ErrorMem(); return;
		}

		free(ed_buf[line]);

		ed_buf[line] = p;
	}

	strcpy(ed_buf[line], ed_lin);
}

EdMatchStr(s1, s2)
char *s1, *s2;
{
	return !strcmp(s1, s2);
}

#undef ED_MAX_LINES
