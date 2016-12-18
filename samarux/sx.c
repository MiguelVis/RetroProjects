/*	sx.c

	+------------------------------------+
	| SamaruX - Unix like shell for CP/M |
	+------------------------------------+

	Copyright (c) 2007-2016 Miguel I. Garcia Lopez.

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

	Contact:

	www.floppysoftware.es
	cpm-connections.blogspot.com
	floppysoftware@gmail.com

	Compile with MESCC:

	cc sx
	ccopt sx
	zsm sx
	hextocom sx

	Revisions:

	11-May-2007 : MGL / Initial version
	28-Jul-2011 : MGL / Added support to UX (User numbers in file names...)
	12-Nov-2013 : MGL / Added redirection output support with '>' in command line.
	18-Nov-2013 : MGL / Uses the new UX
	11-Dec-2013 : MGL / Added man command
	03-Jun-2014 : MGL / LS rewritten.
	08 Dec 2014 : Removed redirection code. Now uses stdio.h and redir.h
	19 Dec 2014 : Removed ; stuff (multiple commands in one line - it was buggy).
	19 Dec 2014 : Rewritten some commands as external c files: ascii, cd,
	              clear, dump, echo, ls, man, prompt, pwd.
	19 Dec 2014 : Added cat command.
	20 Dec 2014 : Added cp command.
	21 Dec 2014 : Added rm command.
	21 Dec 2014 : RtSortFcx was buggy, rewritten.
	21 Dec 2014 : Added mv command.
	22 Dec 2014 : Added env command and preliminary environment variables support.
	23 Dec 2014 : Added chmod command.
	24 Dec 2014 : Now Error() returns 1.
	24 Dec 2014 : Added ver command.
	25 Dec 2014 : Added batch support.
	28 Dec 2014 : Added support for $variables in command line.
	28 Dec 2014 : Rewritten commands as external c files: exit, login, history.
	28 Dec 2014 : Now uses its own Redir().
	29 Dec 2014 : Added command name in errors.
	29 Dec 2014 : Added pipe support.
	30 Dec 2014 : Now Error() returns -1.
	30 Dec 2014 : Main shell functions re-written to support recursivity for batch.
	31 Dec 2014 : Added more command.
	31 Dec 2014 : Added comments support in command line with #.
	02 Jan 2015 : Added profile execution on start.
	02 Jan 2015 : Removed prompt command. Now prompt uses PROMPT environment variable.
	03 Jan 2015 : Automatic console set-up.
	03 Jan 2015 : Added escape sequences support to prompt.
	04 Jan 2015 : Added MakeFcx function.
	04 Jan 2015 : Added alias substitution.
	04 Jan 2015 : Added cpm command.
	04 Jan 2015 : Added -q command (quietly, no verbose) to support return from cpm command.
	06 Jan 2015 : Added some error messages.
	06 Jan 2015 : Added df command.
	08 Jan 2015 : Added some debug code. Changed the name of some global variables.
	16 Jan 2015 : Adapted to last changes in MESCC (SIZEOF_??? stuff).
	16 Jan 2015 : Changed argument parsing code and # comment stuff.
	19 Jan 2015 : Added command exit code control & pseudo-variable $?.
	19 Jan 2015 : Added key table contol (for env & alias).
	19 Jan 2015 : Now batch variables are not in the environment table.
	20 Jan 2015 : Added read command. Added ErrorFindVar().
	11 Feb 2015 : Removed login command. Reworked some code in profiling and the start message.
	14 Feb 2015 : A lot of cosmethic changes.
	15 Feb 2015 : Added if & goto commands. Added KeyFree(), ReadFile().
	16 Feb 2015 : Do profiling no verbose.
	18 Feb 2015 : Minor changes in ReadLine().
	18 Feb 2015 : Work on temporary disk drive and pipe filenames (CP/M 3).
	19 Feb 2015 : Added grep command. Added single quotes (') support to ParseArgs().
	              Added profile to CP/M mode.
	23 Feb 2015 : Added support for $# pseudo-variable (batch). Added IsSpace().
	26 Feb 2015 : Removed sv_tty_rows & sv_tty_cols. Added TermRows() & TermCols().
	              Added COLUMNS and LINES environment variables.
	              Added Ctrl-U, Ctrl-X and Ctrl-R support in ReadLine().
	27 Feb 2015 : Added TmpFile(), TmpDir(). Added TMPDIR environment variable.
	03 Mar 2015 : Added sort command.
	04 Mar 2015 : Added UnParseEx(). Added wc command.
	07 Mar 2015 : Added AddPathToFn().
	09 Mar 2015 : Added AddTypeToFn().
	15 Mar 2015 : Modified ReadFile() to support a last line without ending with CR/LF.
	21 Mar 2015 : Now, all alias can be executed with redirection.
	              Now, alias will run even if both alias and command line have arguments.
	22 Mar 2015 : Added HOME environment variable support for CD.
	              Solved bug in Execute(). In command lines like 'cat file | more ; echo text',
	              the input pipe was not logically closed after the command more.
	30 Mar 2015 : Added external commands support (PRL). Added mem command.
	
	v1.03
	
	02 Apr 2015 : Added tee command.
	07 Apr 2015 : Added date command.
	09 Apr 2015 : Modified LoadStatus(), SaveStatus(), and restore mode.
	              Added BinFile(). Added BINDIR environment variable.
	              Solved bug in AddPathToFn() - now tests if filename has already a path.
	              Removed TmpDir().

	v2.00
	
	20 Aug 2015 : Removed sx_links.h, ExtStdIn(), ExtStdOut() and ExtStdErr(), not needed anymore.
	              Added version number control in external commands.
	
	v2.01
	
	21 Aug 2015 : Solved bug in version number control.
	22 Aug 2015 : Added hook for exit(). Done some code optimizations.
	
	v2.02

	03 Sep 2015 : Added support for directory names (see cpm.h and CC_FCX_DIR).
	              Added diralias command.
	              Solved bug in Prompt() when user > 9.
	              Added PrintCWD().
	v2.03

	04 Sep 2015 : Modified AddPathToFn(): if the filename starts with ':', use
	              the current path.

	v2.04

	29 Nov 2015 : Added SX_SAMARUX def. for builtin / external commands.
	24 Jan 2016 : Added better support for builtin / external commands.

	v2.05

	03 Jun 2016 : Added tail and whoami commands.
	04 Jun 2016 : Added SX_MINIMAL to include only a minimal # of built-in commands.
	05 Jun 2016 : SamaruX defines are now in sx.h.
	06 Jun 2016 : Added head, true, and false commands.
	14 Jun 2016 : Added tty command.
	19 Jun 2016 : Added shift command.

	v2.06

	27 Nov 2016 : Added '>>' in output redirection. Optimized NULL comparisons.
	28 Nov 2016 : Added SX_CMDCPM def. to support cpm command.
	16 Dec 2016 : Added sleep command.
	17 Dec 2016 : Included rand.h library. Added fortune command.
	18 Dec 2016 : Solved bug in Execute().

	NOTES:
	
	- Don't forget to modify samarux.h when you compile this file.

	ISSUES:

	- Alias defined with arguments can't be executed with pipes (single alias can).
	- History saves again command lines retrieved from history.

	TO-DO & IDEAS:

	- Add commands: who, getconf, unalias, dirname, basename, printf.
	- Add environment variables: PWD - see command cd in POSIX specification.
	- Implement setenv(), unsetenv(), getenv(), putenv() - see POSIX specification.
	- Add options in compilation time to build for any CP/M, CP/M 2 or CP/M 3.
*/

/* DEFs FOR MESCC
   --------------
*/

#define CC_NO_ARGS    /* No command line arguments */
#define CC_FCX        /* Enable FCX support (user numbers in file names) */
#define CC_FCX_DIR    /* Enable named directories -- see DirToDrvUsr() below */
#define CC_STDIO      /* Enable stdin, stdout and stderr */
#define CC_FOPEN_A    /* To include "a" and "ab" modes for fopen() */
#define CC_FREAD      /* To include fread() */
#define CC_FGETS      /* To include fgets() */
#define CC_CONIO_BIOS /* Direct console I/O */

/* DEFs FOR SamaruX
   ----------------
*/

//#define SX_DEBUG      /* Debug */
#define SX_EXTCMDS    /* External commands support */
#define SX_HKEXIT     /* Hook for exit() */
#define SX_MINIMAL    /* Minimal # of built-in commands */
#define SX_CMDCPM     /* Support for cpm command */

/* MESCC LIBRARIES
   ---------------
*/

#include <mescc.h>

#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <mem.h>
#include <cpm.h>
#include <alloc.h>
#include <fileio.h>
#include <printf.h>
#include <fprintf.h>
#include <sprintf.h>
#include <clock.h>
#include <stdbool.h>
#include <rand.h>

#ifdef SX_HKEXIT

#include <setjmp.h>

#endif

/* SAMARUX LIBRARY
   ---------------
*/

#include "sx.h"

/* SAMARUX SYSTEM VARIABLES
   ------------------------
*/

int sv_cpmver; /* CP/M version */

int sv_drive;  /* Current drive: 0 .. 15 */
int sv_user;   /* Current user:  0 .. 15 */

/* char sv_tmp_du[FILENAME_MAX]; */ /* Temporary directory name */
char sv_tmp_fn[FILENAME_MAX]; /* Temporary file name */

int *sv_hist;        /* char *sv_hist[] - Commands line history entries */

int *sv_env_name;    /* char *sv_env_name[] - Name of environment variables */
int *sv_env_value;   /* char *sv_env_value[] - Value of environment variables */

int *sv_alias_name;  /* char *sv_alias_name[] - Name of aliases */
int *sv_alias_value; /* char *sv_alias_value[] - Value of aliases */

int *sv_dir_path;    /* char *sv_dir_path[] - Path */
int *sv_dir_alias;   /* char *sv_dir_alias[] - Alias */

int sv_flg_hist; /* NZ to not save last command in history */
int sv_flg_buf;  /* NZ to not delete command line buffer */
int sv_flg_quit; /* NZ to quit shell */
int sv_flg_cpm;	 /* NZ if CP/M mode */
int sv_batch;    /* NZ if BATCH is running */

BYTE sv_fcx[UX_FCX_SIZ];  /* FCX # 1 */
BYTE sv_fcx_two[UX_FCX_SIZ]; /* FCX # 2 */

BYTE *sv_dma;     /* DMA */

BYTE *sv_fcxbuf;  /* Buffer for file search */

int sv_cmd_max;   /* # of built-in commands */

WORD sv_cmd_name[SX_MAX_CMD]; /* Array of built-in command names */
WORD sv_cmd_fun[SX_MAX_CMD];  /* Array of built-in command functions */

char *sv_cmd_err; /* Current command name (for errors) */

int sv_cmd_exit;  /* Exit code of last executed command */

char sv_ed_buf[SX_LINESIZE];  /* Buffer for line editing */

char sv_pipe_in[FILENAME_MAX];  /* File name for input  pipe */
char sv_pipe_out[FILENAME_MAX]; /* File name for output pipe */

#ifdef SX_HKEXIT

/* HOOK FOR exit()
   ---------------
*/

jmp_buf sx_exit_jpbf; /* jmp_buf for hook */
int sx_exit_code;     /* exit code */

#endif

/*	BUILT-IN COMMANDS
	-----------------
*/

#define SX_SAMARUX

#ifdef SX_HKEXIT

#define exit HookExit

#endif

#include <sx_alias.c>

#ifndef SX_MINIMAL
#include <sx_ascii.c>
#endif

#include <sx_batch.c>
#include <sx_built.c>

#ifndef SX_MINIMAL
#include <sx_cat.c>
#endif

#include <sx_cd.c>

#ifndef SX_MINIMAL
#include <sx_chmod.c>
#endif

#include <sx_clear.c>

#ifndef SX_MINIMAL
#include <sx_cp.c>
#ifdef SX_CMDCPM
#include <sx_cpm.c>
#endif
#include <sx_date.c>
#include <sx_df.c>
#include <sx_diral.c>
#include <sx_dump.c>
#endif

#include <sx_echo.c>

#ifndef SX_MINIMAL
#include <sx_ed.c>
#endif

#include <sx_env.c>
#include <sx_exit.c>
#include <sx_false.c>

#ifndef SX_MINIMAL
#include <sx_fortu.c>
#endif

#include <sx_goto.c>

#ifndef SX_MINIMAL
#include <sx_grep.c>
#include <sx_head.c>
#endif

#include <sx_hist.c>
#include <sx_if.c>

#ifndef SX_MINIMAL
#include <sx_ls.c>
#include <sx_man.c>
#endif

#include <sx_mem.c>

#ifndef SX_MINIMAL
#include <sx_more.c>
#include <sx_mv.c>
#endif

#include <sx_pwd.c>
#include <sx_read.c>

#ifndef SX_MINIMAL
#include <sx_rm.c>
#endif

#include <sx_shift.c>

#ifndef SX_MINIMAL
#include <sx_sleep.c>
#include <sx_sort.c>
#include <sx_tail.c>
#include <sx_tee.c>
#endif

#include <sx_true.c>

#ifndef SX_MINIMAL
#include <sx_tty.c>
#include <sx_ver.c>
#include <sx_wc.c>
#include <sx_whoam.c>
#endif

#ifdef SX_HKEXIT

#undef exit

#endif

#undef SX_SAMARUX

/*	MAIN PROGRAM
	------------
*/

main()
{
	char *ptr;

	/* Build commands table */
	
	/*
	sv_cmd_max = 0;	*** Already initialized to 0
	*/

#ifdef SX_ALIAS	
	sv_cmd_name[sv_cmd_max]="alias";    sv_cmd_fun[sv_cmd_max++]=AliasMain;
#endif
#ifdef SX_ASCII
	sv_cmd_name[sv_cmd_max]="ascii";    sv_cmd_fun[sv_cmd_max++]=AsciiMain;
#endif
#ifdef SX_BATCH
	sv_cmd_name[sv_cmd_max]="batch";    sv_cmd_fun[sv_cmd_max++]=BatchMain;
#endif
#ifdef SX_BUILTIN
	sv_cmd_name[sv_cmd_max]="builtin";  sv_cmd_fun[sv_cmd_max++]=BuiltinMain;
#endif
#ifdef SX_CAT
	sv_cmd_name[sv_cmd_max]="cat";      sv_cmd_fun[sv_cmd_max++]=CatMain;
#endif
#ifdef SX_CD
	sv_cmd_name[sv_cmd_max]="cd";       sv_cmd_fun[sv_cmd_max++]=CdMain;
#endif
#ifdef SX_CHMOD
	sv_cmd_name[sv_cmd_max]="chmod";    sv_cmd_fun[sv_cmd_max++]=ChmodMain;
#endif
#ifdef SX_CLEAR
	sv_cmd_name[sv_cmd_max]="clear";    sv_cmd_fun[sv_cmd_max++]=ClearMain;
#endif
#ifdef SX_CP
	sv_cmd_name[sv_cmd_max]="cp";       sv_cmd_fun[sv_cmd_max++]=CpMain;
#endif
#ifdef SX_CPM
	sv_cmd_name[sv_cmd_max]="cpm";      sv_cmd_fun[sv_cmd_max++]=CpmMain;
#endif
#ifdef SX_DF
	sv_cmd_name[sv_cmd_max]="df";       sv_cmd_fun[sv_cmd_max++]=DfMain;
#endif
#ifdef SX_DIRALIAS
	sv_cmd_name[sv_cmd_max]="diralias"; sv_cmd_fun[sv_cmd_max++]=DiralMain;
#endif
#ifdef SX_DATE
	sv_cmd_name[sv_cmd_max]="date";     sv_cmd_fun[sv_cmd_max++]=DateMain;
#endif
#ifdef SX_DUMP
	sv_cmd_name[sv_cmd_max]="dump";     sv_cmd_fun[sv_cmd_max++]=DumpMain;
#endif
#ifdef SX_ECHO
	sv_cmd_name[sv_cmd_max]="echo";     sv_cmd_fun[sv_cmd_max++]=EchoMain;
#endif
#ifdef SX_ED
	sv_cmd_name[sv_cmd_max]="ed";       sv_cmd_fun[sv_cmd_max++]=EdMain;
#endif
#ifdef SX_ENV
	sv_cmd_name[sv_cmd_max]="env";      sv_cmd_fun[sv_cmd_max++]=EnvMain;
#endif
#ifdef SX_EXIT
	sv_cmd_name[sv_cmd_max]="exit";     sv_cmd_fun[sv_cmd_max++]=ExitMain;
#endif
#ifdef SX_FALSE
	sv_cmd_name[sv_cmd_max]="false";    sv_cmd_fun[sv_cmd_max++]=FalseMain;
#endif
#ifdef SX_FORTUNE
	sv_cmd_name[sv_cmd_max]="fortune";  sv_cmd_fun[sv_cmd_max++]=FortuneMain;
#endif
#ifdef SX_GOTO
	sv_cmd_name[sv_cmd_max]="goto";     sv_cmd_fun[sv_cmd_max++]=GotoMain;
#endif
#ifdef SX_GREP
	sv_cmd_name[sv_cmd_max]="grep";     sv_cmd_fun[sv_cmd_max++]=GrepMain;
#endif
#ifdef SX_HEAD
	sv_cmd_name[sv_cmd_max]="head";     sv_cmd_fun[sv_cmd_max++]=HeadMain;
#endif
#ifdef SX_HISTORY
	sv_cmd_name[sv_cmd_max]="history";  sv_cmd_fun[sv_cmd_max++]=HistMain;
#endif
#ifdef SX_IF
	sv_cmd_name[sv_cmd_max]="if";       sv_cmd_fun[sv_cmd_max++]=IfMain;
#endif
#ifdef SX_LS
	sv_cmd_name[sv_cmd_max]="ls";       sv_cmd_fun[sv_cmd_max++]=LsMain;
#endif
#ifdef SX_MAN
	sv_cmd_name[sv_cmd_max]="man";      sv_cmd_fun[sv_cmd_max++]=ManMain;
#endif
#ifdef SX_MEM
	sv_cmd_name[sv_cmd_max]="mem";      sv_cmd_fun[sv_cmd_max++]=MemMain;
#endif
#ifdef SX_MORE
	sv_cmd_name[sv_cmd_max]="more";     sv_cmd_fun[sv_cmd_max++]=MoreMain;
#endif
#ifdef SX_MV
	sv_cmd_name[sv_cmd_max]="mv";       sv_cmd_fun[sv_cmd_max++]=MvMain;
#endif
#ifdef SX_PWD
	sv_cmd_name[sv_cmd_max]="pwd";      sv_cmd_fun[sv_cmd_max++]=PwdMain;
#endif
#ifdef SX_READ
	sv_cmd_name[sv_cmd_max]="read";     sv_cmd_fun[sv_cmd_max++]=ReadMain;
#endif
#ifdef SX_RM
	sv_cmd_name[sv_cmd_max]="rm";       sv_cmd_fun[sv_cmd_max++]=RmMain;
#endif
#ifdef SX_SHIFT
	sv_cmd_name[sv_cmd_max]="shift";    sv_cmd_fun[sv_cmd_max++]=ShiftMain;
#endif
#ifdef SX_SLEEP
	sv_cmd_name[sv_cmd_max]="sleep";    sv_cmd_fun[sv_cmd_max++]=SleepMain;
#endif
#ifdef SX_SORT
	sv_cmd_name[sv_cmd_max]="sort";     sv_cmd_fun[sv_cmd_max++]=SortMain;
#endif
#ifdef SX_TAIL
	sv_cmd_name[sv_cmd_max]="tail";     sv_cmd_fun[sv_cmd_max++]=TailMain;
#endif
#ifdef SX_TEE
	sv_cmd_name[sv_cmd_max]="tee";      sv_cmd_fun[sv_cmd_max++]=TeeMain;
#endif
#ifdef SX_TRUE
	sv_cmd_name[sv_cmd_max]="true";     sv_cmd_fun[sv_cmd_max++]=TrueMain;
#endif
#ifdef SX_TTY
	sv_cmd_name[sv_cmd_max]="tty";      sv_cmd_fun[sv_cmd_max++]=TtyMain;
#endif
#ifdef SX_VER
	sv_cmd_name[sv_cmd_max]="ver";      sv_cmd_fun[sv_cmd_max++]=VerMain;
#endif
#ifdef SX_WC
	sv_cmd_name[sv_cmd_max]="wc";       sv_cmd_fun[sv_cmd_max++]=WcMain;
#endif
#ifdef SX_WHOAMI
	sv_cmd_name[sv_cmd_max]="whoami";   sv_cmd_fun[sv_cmd_max++]=WhoamiMain;
#endif

	/* Create some buffers and tables */

	/* FCXs */
	if(!(sv_fcxbuf = malloc(SX_MAX_FCX * 13)))
		return ErrorMem();

	/* Environment names */
	if(!(sv_env_name = KeyAlloc(SX_MAX_ENV)))
		return ErrorMem();

	/* Environment values */
	if(!(sv_env_value = KeyAlloc(SX_MAX_ENV)))
		return ErrorMem();

	/* Alias names */
	if(!(sv_alias_name = KeyAlloc(SX_MAX_ALIAS)))
		return ErrorMem();

	/* Alias values */
	if(!(sv_alias_value = KeyAlloc(SX_MAX_ALIAS)))
		return ErrorMem();

	/* Directory names */
	if(!(sv_dir_path = KeyAlloc(SX_MAX_DIR)))
		return ErrorMem();

	/* Directory values */
	if(!(sv_dir_alias = KeyAlloc(SX_MAX_DIR)))
		return ErrorMem();

	/* History */
	if(!(sv_hist = KeyAlloc(SX_MAX_HIST)))
		return ErrorMem();

	/* Set-up CP/M */
	SetUpCPM();

	/* Set-up some variables */
	sv_cmd_err = "SamaruX";

	/*
	sv_flg_hist = sv_flg_buf = sv_flg_quit = sv_flg_cpm = sv_cmd_exit = sv_batch = 0; *** Already initialized to 0
	*/

	/* Check if there is a CP/M command line */
	if(*(ptr = 0x0080))
	{
#ifdef SX_CMDCPM
		/* Check mode */

		if(*ptr > 4 && ptr[1] == ' ' && ptr[2] == '-' && ptr[3] == 'R' && ptr[4] == ' ')
		{
			/* Restore mode: comeback from a CP/M command */

			/* Set EOS */

			ptr[1 + (*ptr)] = 0;

			/* Load SamaruX status */

			LoadStatus(ptr + 5);
		}
		else
		{
#endif
			/* CP/M mode: execute command and return to CP/M */

			++sv_flg_cpm;

			/* Profile */
			Profile(SX_PROFCPM);

			/* Cook the command line a bit */
			ptr[*ptr + 1] = 0;

			while(*++ptr)
				*ptr = tolower(*ptr);

			/* Execute the command line */
			Execute(0x0081);

			/* Return to CP/M */
			return 0;
#ifdef SX_CMDCPM
		}
#endif
	}
	else
	{
		/* Interactive mode: execute commands until user quits the shell */
		printf("SamaruX v%d.%02d / %s\n\n%s\n\n", SX_VERSION, SX_RELEASE, SX_APPDATE, SX_COPYRGT);
		printf("CP/M v%d.%d\n\n", (sv_cpmver >> 4) & 0x0F, sv_cpmver & 0x0F);
		printf("%d built-in commands\n\n", sv_cmd_max);

		/* Profile */
		Profile(SX_PROFILE);
	}

	/* Interactive shell */
	while(1)
	{
		/* Re-use the buffer? */
		if(sv_flg_buf)
			sv_flg_buf = 0;
		else
			sv_ed_buf[0] = 0;

		/* Show prompt */
		Prompt();

		/* Read the command line */
		ReadLine(sv_ed_buf, SX_LINESIZE-1);
		putchar('\n');

		ptr = SkipSpaces(sv_ed_buf);

		if(*ptr)
		{
			/* Execute the command line */
			Execute(ptr);

			/* Quit the shell? */
			if(sv_flg_quit)
				break;

			/* Update history */
			UpdateHist();

			putchar('\n');
		}
	}

	/* Return to CP/M */
	return 0;
}

/* READ LINE
   ---------
*/

ReadLine(buf, width)
char *buf;
int width;
{
	int len, ch;

	putstr(buf); len = strlen(buf);

	while(1)
	{
		switch(ch = getch())
		{
			case 0x08 : /* BS  */
			case 0x7F : /* DEL */
				if(len)
				{
					putchar(8); putchar(' '); putchar(8);
					--len;
				}
				break;

			case 0x15 : /* Ctrl-U : Delete the line */
			case 0x18 : /* Ctrl-X : Delete the line */

			case 0x12 : /* Ctrl-R : Retype the line */
				if(len)
				{
					putchar('^'); putchar('@' + ch); putchar('\n');

					if(ch == 0x12)
					{
						buf[len] = 0;
						putstr(buf);
					}
					else
						*buf = len = 0;
				}
				break;

			case '\r' : /* CR */
			case '\n' : /* LF */
				buf[len] = 0;
				return;

			default :
				if(len < width && ch >= ' ')
					putchar(buf[len++] = ch);
				break;
		}
	}
}

/* CHANGE ENVIRONMENT VARIABLES IN COMMAND LINE
   --------------------------------------------
   Return 0 on success, else -1.
*/

ExeVars(argc, argv)
int argc, argv[]; /* char *argv[] */
{
	int i;
	char *p, *val;

	for(i = 0; i < argc; ++i)
	{
		p = argv[i];

		if(*p == '$')
		{
			val = NULL;

			if(p[1])
			{
				/* Normal environment variables */

				val = EnvGet(p + 1);

				/* Pseudo-environment variables: $?, $#, $0, $1, etc. */

				if(!val && !p[2])
				{
					if(p[1] == '?')
						val = (sv_cmd_exit ? "1" : "0");
					else if(sv_batch)
					{
					 	if(p[1] == '#')
							val = BatchArgc();
						else if(isdigit(p[1]))
							val = BatchArg(p[1] - '0');
					}
				}
			}

			if(val)
				argv[i] = val;
			else
				return ErrorFindVar();
		}
	}

	return 0;
}

/* COMMAND LINE REDIRECTION
   ------------------------
   Return number of arguments on success, else -1.
*/

ExeRedir(argc, argv, pipe_in, pipe_out)
int argc, argv[], pipe_in, pipe_out; /* char *argv[] */
{
	int i, k;
	char *p, *fnin, *fnout;
	bool fnout_append;
	FILE *fp;

#ifdef SX_DEBUG
	fprintf(stderr, "[ExeRedir START]");
#endif

	fnin = fnout = NULL;

	for(i = 0; i < argc; ++i)
	{
		p = argv[i];

		if(*p == '<' && !p[1])
		{
			if(i > 0 && i < argc - 1 && !fnin && !pipe_in)
				fnin = argv[i + 1];
			else
			{
#ifdef SX_DEBUG
				fprintf(stderr, "[ExeRedir ERR <]");
#endif
				return ErrorRedir();
			}
		}
		else if(*p == '>' && (!p[1] || (p[1] == '>' && !p[2])))
		{
			if(i > 0 && i < argc - 1 && !fnout && !pipe_out)
			{
				fnout = argv[i + 1]; fnout_append = p[1];
			}
			else
			{
#ifdef SX_DEBUG
				fprintf(stderr, "[ExeRedir ERR >]");
#endif
				return ErrorRedir();
			}
		}
		else
			continue;

		argc -= 2;

		for(k = i--; k < argc; ++k)
			argv[k] = argv[k + 2];
	}

	if(pipe_in)
		fnin = sv_pipe_in;

	if(pipe_out)
		fnout = sv_pipe_out;

	if(fnin)
	{
		if((fp = fopen(fnin, "r")))
			stdin = fp;
		else
		{
#ifdef SX_DEBUG
			fprintf(stderr, "[ExeRedir ERR fnin %s]", fnin);
#endif
			return ErrorRedir();
		}
	}

	if(fnout)
	{
		if((fp = fopen(fnout, (fnout_append ? "a" : "w"))))
			stdout = fp;
		else
		{
#ifdef SX_DEBUG
			fprintf(stderr, "[ExeRedir ERR fnout %s]", fnout);
#endif
			return ErrorRedir();
		}
	}

#ifdef SX_DEBUG
	fprintf(stderr, "[ExeRedir END]\n");
#endif

	return argc;
}

/* STOP COMMAND LINE REDIRECTION
   -----------------------------
*/

ExeRedirStop(pipe_in, pipe_out)
int pipe_in, pipe_out;
{

#ifdef SX_DEBUG
	fprintf(stderr, "[ExeRedirStop START]");
#endif

	if(stdin)
	{
		fclose(stdin);

		if(pipe_in)
			remove(sv_pipe_in);
	}

	if(stdout)
	{
		fclose(stdout);

		if(pipe_out)
		{
			remove(sv_pipe_in);
			rename(sv_pipe_out, sv_pipe_in);
		}
	}

	stdin = stdout = NULL;

#ifdef SX_DEBUG
	fprintf(stderr, "[ExeRedirStop END]\n");
#endif
}

/* EXECUTE SINGLE COMMAND
   ----------------------
   Return command exit code on success, else -1.
*/

ExeCommand(argc, argv, pipe_in, pipe_out)
int argc, argv[], pipe_in, pipe_out; /* char *argv[] */
{
	int i, code; char *oldcmd; WORD cmd;

#ifdef SX_EXTCMDS
	WORD mem;
#endif

#ifdef SX_DEBUG
	fprintf(stderr, "[ExeCommand CMD = ");

	for(i = 0; i < argc; ++i)
	{
		fprintf(stderr, "%s", argv[i]);

		if(i < argc - 1)
			fputc(' ', stderr);
	}

	fprintf(stderr, "]\n");
#endif

	/* Set up some values */

#ifdef SX_EXTCMDS
	mem = 
#endif

	cmd = NULL;

	/* Search built-in command */
	for(i = 0; i < sv_cmd_max; ++i)
	{
		if(!strcmp(sv_cmd_name[i], argv[0]))
		{
			cmd = sv_cmd_fun[i]; break;
		}
	}

#ifdef SX_EXTCMDS
	/* Search for a external command if it's not a built-in */
	if(i == sv_cmd_max)
	{
		/* Try to load */
		if((mem = /*cmd =*/ LoadPrl(BinFile(argv[0]))))
			cmd = 13 + ((mem & 0x00FF) ? (mem & 0xFF00) + 0x100 : mem);
	}
#endif

	/* Execute command if found */
	if(cmd)
	{
		/* Change environment variables in command line */
		if(!ExeVars(argc, argv))
		{
			/* Update file names for PIPES */
			strcpy(sv_pipe_in, TmpFile(SX_PIPEIN));
			strcpy(sv_pipe_out, TmpFile(SX_PIPEOUT));

			/* Start redirection (if any) */
			if((argc = ExeRedir(argc, argv, pipe_in, pipe_out)) != -1)
			{
				oldcmd = sv_cmd_err; sv_cmd_err = argv[0];

#ifdef SX_DEBUG
	fprintf(stderr, "[ExeCommand CALL = %04x]\n", cmd);
#endif

#ifdef SX_HKEXIT
				/* Test if returns from exit() hook */
				if(setjmp(sx_exit_jpbf)) {
					/* Yes: retrieve exit code */
					code = sx_exit_code;
				}
				else {
					/* No: execute command */
					code = cmd(argc, argv);
				}
#else
				/* Execute command */
				code = cmd(argc, argv);
#endif

#ifdef SX_DEBUG
	fprintf(stderr, "[ExeCommand CODE = %d]\n", code);
#endif
				sv_cmd_err = oldcmd;

				/* Stop redirection */
				ExeRedirStop(pipe_in, pipe_out);

#ifdef SX_EXTCMDS

#ifdef SX_DEBUG
	fprintf(stderr, "[ExeCommand FREE = %04x]\n", mem);
#endif
				/* Free memory (external command) */
				free(mem);
#endif
				/* Return command exit code */
				return code;
			}
		}
	}

#ifdef SX_DEBUG
	fprintf(stderr, "[ExeCommand FAILURE]\n");
#endif

	/* Return failure */

#ifdef SX_EXTCMDS
	return -1;
#else
	return Error("Unknown command");
#endif
}

/* EXECUTE (POSSIBLY MULTIPLE) COMMAND LINE
   ----------------------------------------
   Return 0 on success, else -1.
*/

Execute(cmdline)
char *cmdline;
{
	int i, argc, *argv, *pargv, pipe_in, pipe_out, cmd_nxt, cmd_nxt_pars, code, skip;
	char *p, *buf, *alias, *unparsed;

	/* Skip comments */
	if(*cmdline == '#')
		return 0;

#ifdef SX_DEBUG
	fprintf(stderr, "[Execute LINE = %s]\n", cmdline);
#endif

	/* Alloc memory for command line */
	if(!(buf = malloc(strlen(cmdline) + 1)))
		return (sv_cmd_exit = ErrorMem());

	strcpy(buf, cmdline);

	/* Alloc memory for arguments */
	if(!(argv = pargv = KeyAlloc(SX_MAX_ARGS)))
	{
		free(buf);

		return (sv_cmd_exit = ErrorMem());
	}

	/* Parse arguments */
	if((argc = ParseArgs(buf, argv, SX_MAX_ARGS)) == -1)
	{
		sv_cmd_exit = Error("Bad quoting or too much arguments");

		argc = 0;
	}

	pipe_in = code = 0;

	while(argc)
	{
		pipe_out = cmd_nxt = cmd_nxt_pars = skip = 0;

		/* Pipes and multi-command line */
		for(i = 0; i < argc; ++i)
		{
			p = argv[i];

			if(*p == '|' && !p[1])
				pipe_out = 1;
			else if(!(*p == ';' && !p[1]))
				continue;

			if(i > 0 && i < argc - 1)
			{
				cmd_nxt = i + 1;
				cmd_nxt_pars = argc - i - 1;
				argc = i;
			}
			else
				code = sv_cmd_exit = Error("Bad multi-command line");

			break;
		}

		if(code)
			break;

		/* Alias */
		if((alias = AliasGet(argv[0])))
		{
			if(strchr(alias, ' ')) /* FIXME : Tab? */
			{
				if(argc == 1)
					Execute(alias);
				else
				{
					/* Both alias and command line have arguments */

					if((unparsed = UnParse(argc - 1, argv + 1)))
					{
						if((p = malloc(strlen(alias) + strlen(unparsed) + 2)))
						{
							strcpy(p, alias); strcat(p, " "); strcat(p, unparsed);

							Execute(p);

							free(p);
						}
						else
							sv_cmd_exit = ErrorMem();

						free(unparsed);
					}
					else
						sv_cmd_exit = ErrorMem();
				}

				++skip;
			}
			else
				argv[0] = alias;
		}

		/* Execute single command */
		if(!skip)
			sv_cmd_exit = ExeCommand(argc, argv, pipe_in, pipe_out);

		/* Quit the shell? */
		if(sv_flg_quit)
			break;

		/* Next command */
		if(cmd_nxt)
		{
			argv += cmd_nxt;
			argc = cmd_nxt_pars;
			/* cmd_nxt = 0; *** will be 0 in next iteration */

/****************************
			if(pipe_out)
			{
				pipe_in = 1; pipe_out = 0;
			}
			else
				pipe_in = 0;
*************************************/

			pipe_in = pipe_out;

			/* pipe_out will be 0 in next iteration */
		}
		else
			break;
	}

	/* Free buffers */
	free(buf); free(pargv);

#ifdef SX_DEBUG
	fprintf(stderr, "[Execute END]\n");
#endif

	/* Return success or error */
	return (sv_cmd_exit ? -1 : 0);
}

/* UPDATE HISTORY
   --------------
   Return 0 on success, else -1.
*/

UpdateHist()
{
	int i;

	/* Update only if sv_flg_hist == 0 */
	if(sv_flg_hist)
		sv_flg_hist = 0;
	else
	{
		if(sv_hist[SX_MAX_HIST - 1])
			free(sv_hist[SX_MAX_HIST - 1]);

		for(i = SX_MAX_HIST - 1; i; --i)
			sv_hist[i] = sv_hist[i - 1];

		if(!(sv_hist[0] = malloc(strlen(sv_ed_buf) + 1)))
			return ErrorMem();

		strcpy(sv_hist[0], sv_ed_buf);
	}

	return 0;
}

/* SHOW PROMPT
   -----------
*/

Prompt()
{
	char *p, *px; int ch;

	if(!(p = EnvGet("PROMPT")))
		p = "%$";

	while((ch = *p++))
	{
		if(ch == '%')
		{
			switch(*p++)
			{
				case '$' : /* $ */
					putchar('$');
					break;
				case 'u' : /* Username */
					putstr((px = EnvGet("USER")) ? px : "UnkUsr");
					break;
				case 'w' : /* Working directory */
					PrintCWD();
					break;
				case '%' : /* % */
					putchar('%');
					break;
				case 0   :
					--p;
				default  :
					Error("Bad prompt");  /* FIXME: Delete the PROMPT variable? */
					break;
			}
		}
		else
			putchar(ch);
	}

	putchar(' ');
}

/* SKIP SPACES
   -----------
*/

SkipSpaces(ptr)
char *ptr;
{
	while(IsSpace(*ptr))
		++ptr;

	return ptr;
}

/* EXECUTE PROFILE
   ---------------
*/

Profile(fn)
char *fn;
{
	FILE *fp;
	char *cmd;

#ifdef SX_DEBUG
	fprintf(stderr, "[ Profile start ]\n");
#endif

	if((fp = fopen(fn, "r")))
	{
		fclose(fp);

		cmd = "batch 12345678.123";

		strcpy(cmd + 6, fn);  /* FIXME: Check overflow? */

		Execute(cmd);
	}

#ifdef SX_DEBUG
	fprintf(stderr, "[ Profile end ]\n");
#endif
}

/* EXPAND FCX
   ----------
   Return number of files on success, else -1.
*/

RtExpandFcx(fcx, buf, max)
BYTE *fcx, *buf; int max;
{
	int dir, files;

	bdos_hl(BF_DMA, sv_dma);

	dir=bdos_fcx_a(BF_FIND1ST, fcx);

	for(files=0; dir!=0xFF; ++files)
	{
		if(files == max)
			return -1;

		buf[UX_FCX_DRV] = fcx[UX_FCX_DRV];
		buf[UX_FCX_USR] = fcx[UX_FCX_USR];

		memcpy(buf + 2, sv_dma+dir*32+1, 11);

		buf+=13;

		dir=bdos_fcx_a(BF_FINDNEXT, fcx);
	}

	return files;
}

/* SORT FCXs (BUBBLE SORT)
   -----------------------
*/

RtSortFcx(fcxbuf, max)
char *fcxbuf; int max;
{
	int i, j; char tmp[13], *fcx1, *fcx2;

	for(i = 1; i < max; ++i)
	{
  		for(j = max - 1; j >= i; --j)
		{
			fcx2 = fcxbuf + j * 13;
			fcx1 = fcx2 - 13;

   			if(RtCompFcx(fcx1, fcx2) > 0)
			{
    				memcpy(tmp, fcx1, 13);
    				memcpy(fcx1, fcx2, 13);
    				memcpy(fcx2, tmp, 13);
   			}
  		}
 	}
}

/* COMPARE TWO FCXs
   ----------------
   Return FCX1 - FCX2.
*/

RtCompFcx(fcx1, fcx2)
char *fcx1, *fcx2;
{
	int i, dif;

	if((dif = fcx1[UX_FCX_DRV] - fcx2[UX_FCX_DRV]))
		return dif;

	if((dif = fcx1[UX_FCX_USR] - fcx2[UX_FCX_USR]))
		return dif;

	for(i = 2; i != 13; ++i)
	{
		if((dif = (fcx1[i] & 0x7F) - (fcx2[i] & 0x7F)))
			return dif;
	}

	return 0;
}

/* PRINT FCX
   ---------
*/

RtPrintFcx(fcx)
BYTE *fcx;
{
	int drv, usr, i;

	drv = fcx[UX_FCX_DRV];
	usr = fcx[UX_FCX_USR];

	printf("%c%02d:", 'a'+ (drv ? drv-1 : sv_drive), (usr ? usr-1 : sv_user));

	for(i = 2; i < 13; ++i)
	{
		if(i == 10) putchar('.');

		putchar(tolower(fcx[i] & 0x7F));
	}
}

/* FILE NAME IS A DIRECTORY?
   -------------------------
   Return 1 if afirmative, else 0.
*/

FnIsDir(fn)
char *fn;
{
	return UxGetPath(fn) == -1 ? 0 : 1;
}

/* FILE NAME IS AMBIGUOUS?
   -----------------------
   Return 1 if afirmative, else 0.
*/

FnIsAmb(fn)
char *fn;
{
	while(*fn)
	{
		if(*fn == '?' || *fn == '*')
			return 1;

		++fn;
	}

	return 0;
}

/* MAKE FCX FROM A DIRECTORY OR FILE NAME
   --------------------------------------
   Return 0 on success, else -1.
*/

MakeFcx(fn, fcx)
char *fn, *fcx;
{
	int du; int drv, usr;

	if((du = UxGetPath(fn)) != -1)
	{
		drv = (du >> 8) & 0xFF;
		usr = du & 0xFF;

		setfcx("*.*", fcx);

		fcx[UX_FCX_DRV] = drv == 0xDD ? 0 : drv + 1;
		fcx[UX_FCX_USR] = usr == 0xDD ? 0 : usr + 1;
	}
	else if(setfcx(fn, fcx))
		return -1;

	return 0;
}


/* pathflg (include path) = 0 never, 1 always, 2 partial path if in fcx, 3 full path if in fcx */

/*********************

FcxToStr(fcx, fn, pathflg)
char *fcx, *fn; int pathflg;
{
	int drv, usr, i, path, ch;

	drv = fcx[UX_FCX_DRV];
	usr = fcx[UX_FCX_USR];

	path = 0;

	if(pathflg == 1 || (pathflg == 2 && drv) || (pathflg == 3 && (drv || usr)))
	{
		*fn++ = 'a' + drv ? drv - 1 : sv_drive; ++path;
	}

	if(pathflg == 1 || (pathflg == 2 && usr) || (pathflg == 3 && (drv || usr)))
	{
		sprintf(fn, "%02d", usr ? usr - 1 : sv_user); fn += 2; ++path;
	}

	if(path)
		*fn++ = ':';

	for(i = 2; i < 13; ++i)
	{
		ch = fcx[i] & 0x7F;

		if(i == 10 && ch != ' ')
			*fn++ = '.';

		if(ch != ' ')
		{
			if(i == 10)
				*fn++ = '.';
			*fn++ = tolower(ch);
		}
	}

	*fn = 0;
}

*****************/


/* PARSE ARGUMENTS
   ---------------
*/

ParseArgs(buf, argv, maxarg)
char *buf; int argv[]; /* char *argv[] */
int maxarg;
{
	int i;

	for(i = 0; *buf; ++i)
	{
		/* Skip spaces */
		buf = SkipSpaces(buf);

		/* End of line or too much args? */
		if(!*buf || i >= maxarg)
			break;

		/* Single quoted strings */
		if(*buf == '\'')
		{
			/* New argument */
			argv[i] = ++buf;

			/* Go to the trailing quote */
			while(*buf != '\'')
			{
				/* End of line? */
				if(!(*buf++))
					return -1;
			}

			/* Uncomment this if you don't want empty strings
			if(buf == argv[i])
				return -1;
			*/

			/* Char. after the quote must be ZERO, SPACE or TAB */
			if(buf[1] && !IsSpace(buf[1]))
				return -1;
		}
		else
		{
			/* New argument */
			argv[i] = buf;

			/* Go to the next SPACE or end of line */
			while(*buf && !IsSpace(*buf))
			{
				/* Quotes in the middle of the argument? */
				if(*buf == '\'')
					return -1;

				++buf;
			}
		}

		/* ZERO at the end of the argument */
		if(*buf)
			*buf++=0;
	}

	/* Return success or failure */
	return *buf ? -1 : i;
}

/* IS SPACE?
   ---------
*/

IsSpace(c)
int c;
{
	return c == ' ' || c == '\t';
}

/* ALLOC AND SET-UP KEY TABLE
   --------------------------
   Return array address on success, or NULL.
*/

KeyAlloc(items)
int items;
{
	int *array; /* char *array[] */
	int i;

	if((array = malloc(items * SIZEOF_PTR)))
	{
		for(i = 0; i < items; ++i)
			array[i] = NULL;
	}

	return array;
}

/* FREE KEY TABLE
   --------------
*/

KeyFree(table, max)
int *table, max;
{
	int i;

	for(i = 0; i < max; ++i)
	{
		if(table[i])
			free(table[i]);
	}

	free(table);
}

/* PRINT KEY TABLE
   ---------------
*/

KeyPrint(names, values, max)
int *names, *values, max;
{
	int i;

	for(i = 0; i < max; ++i)
	{
		if(names[i])
			printf("%s = %s\n", names[i], values[i]);
	}
}

/* FIND ENTRY IN KEY TABLE
   -----------------------
   Return entry number (0..?) on success, else -1.
*/

KeyFind(names, max, name)
int *names, max; char *name;
{
	int i;

	for(i = 0; i < max; ++i)
	{
		if(names[i])
		{
			if(!strcmp(name, names[i]))
				return i;
		}
	}

	return -1;
}

/* RETURN ENTRY FROM KEY TABLE
   ---------------------------
   Return address on success, else NULL.
*/

KeyGet(names, values, max, name)
int *names, *values, max; char *name;
{
	int k;

	if((k = KeyFind(names, max, name)) != -1)
		return values[k];

	return NULL;
}

/* RETURN NUMBER OF NON NULL ENTRIES
   ---------------------------------
*/

KeyCount(table, max)
int *table, max;
{
	int i, k;

	for(i = k = 0; i < max; ++i)
	{
		if(table[i])
			++k;
	}

	return k;
}

/* RETURN NUMBER OF NON NULL ENTRIES,
   STOPPING ON FIRST NULL ENTRY
   ----------------------------------
*/

KeyCount1st(table, max)
int *table, max;
{
	int i;

	for(i = 0; i < max; ++i)
	{
		if(!table[i])
			break;
	}

	return i;
}

/* ADD ENTRY TO KEY TABLE
   ----------------------
   Return entry number (0..?) on success, else -1.
*/

KeyAdd(names, values, max, name, value)
int *names, *values, max; char *name, *value;
{
	int i, n, v;

	n = strlen(name) + 1;
	v = strlen(value) + 1;

	for(i = 0; i < max; ++i)
	{
		if(!names[i])
		{
			if((names[i] = malloc(n)))
			{
				if((values[i] = malloc(v)))
				{
					strcpy(names[i], name);
					strcpy(values[i], value);

					return i;
				}

				free(names[i]);
			}

			ErrorMem();

			return -1;
		}
	}

	Error("No free entries");

	return -1;
}

/* DELETE ENTRY IN KEY TABLE
   -------------------------
*/

KeyDel(names, values, max, pos)
int *names, *values, max; int pos;
{
	free(names[pos]);
	free(values[pos]);

	names[pos] = values[pos] = NULL;
}

/* READ TEXT FILE
   --------------
*/

ReadFile(fn, max_lines, max_len)
char *fn; int max_lines, max_len;
{
	FILE *fp;
	char *pbuf;
	int i, k, j, err, *arr_lines; /* char *arr_lines[] */

	/* Set-up some variables */

	fp = pbuf = arr_lines = NULL;
	err = 0;

	/* Create array */
	if((arr_lines = KeyAlloc(max_lines)))
	{
		/* Create line buffer */

		if((pbuf = malloc(max_len + 1)))
		{
			/* Open file */
			if((fp = fopen(fn, "r")))
			{
				for(i = 0; i < 32000; ++i)
				{
					if(!fgets(pbuf, max_len + 1, fp))
						break;

					if(i == max_lines)
					{
						err = Error("Too many lines"); break;
					}

					k = strlen(pbuf);

					/* The last line in the file can be finished without a '\n' - check this */

					if(pbuf[k - 1] == '\n')
						pbuf[--k] = 0;
					else if(k == max_len)
					{
						err = ErrorTooLong(); break;
					}

					/* **************************************
					if(pbuf[k - 1] != '\n')
					{
						err = ErrorTooLong(); break;
					}
	
					pbuf[--k] = 0;
					************************************** */

					for(j = 0; j < k; ++j)
					{
						if(pbuf[j] == '\t')
						{
							pbuf[j] = ' '; /* tabs = 1; */
						}
						else if(pbuf[j] < ' ')
						{
							err = Error("Bad character"); break;
						}
					}

					if(err)
						break;

					if(!(arr_lines[i] = malloc(k + 1)))
					{
						err = ErrorMem(); break;
					}

					strcpy(arr_lines[i], pbuf);
				}

				fclose(fp);
			}
			else
				err = ErrorOpen();
		}
		else
			err = ErrorMem();
	}
	else
		err = ErrorMem();

	if(pbuf)
		free(pbuf);

	if(err && arr_lines)
		KeyFree(arr_lines, max_lines);

	return err ? NULL : arr_lines;
}

/* UNPARSE
   -------
   Return address on success, else NULL.
*/

UnParse(argc, argv)
int argc, argv[]; /* char *argv[] */
{
	return UnParseEx(argc, argv, 1);
}

/* UNPARSE EXTENDED
   ----------------
   Return address on success, else NULL.
*/

UnParseEx(argc, argv, flag)
int argc, argv[], flag; /* char *argv[] */
{
	int i, k;
	char *p, *q;

	k = 0;

	for(i = k = 0; i < argc; ++i)
		k += strlen(argv[i]) + 1;

	if((p = q = malloc(k)))
	{
		for(i = 0; i < argc; ++i)
		{
			strcpy(p, argv[i]);

			if(i < argc - 1)
			{
				p += strlen(argv[i]);

				if(flag)
					*p++ = ' ';
			}
		}
	}
	else
		ErrorMem();

	return q;
}

/* SETUP CP/M
   ----------
   Setup some things according to the CP/M version we are running.
*/

SetUpCPM()
{
	sv_cpmver = bdos_a(BF_OSVER, 0xCACA);
	sv_dma    = 0x0080;
	sv_drive  = bdos_a(BF_GETDRV, 0xCACA);
	sv_user   = bdos_a(BF_USER, 0xFFFF);
}

#ifdef SX_CMDCPM

/* LOAD SAMARUX STATUS
   -------------------
*/

LoadStatus(fn)
char *fn;
{
	Profile(fn); remove(fn);
}

/* SAVE SAMARUX STATUS
   -------------------
*/

SaveStatus(fn)
char *fn;
{
	FILE *fp;
	int i;

	if((fp = fopen(fn, "w")))
	{
		for(i = 0; i < SX_MAX_ENV; ++i)
		{
			if(sv_env_name[i])
				fprintf(fp, "env %s '%s'\n", sv_env_name[i], sv_env_value[i]);
		}

		for(i = 0; i < SX_MAX_ALIAS; ++i)
		{
			if(sv_alias_name[i])
				fprintf(fp, "alias %s '%s'\n", sv_alias_name[i], sv_alias_value[i]);
		}

		fclose(fp);

		return 0;
	}

	return 1;
}

#endif

/* GET TERMINAL ROWS
   -----------------
*/

TermRows()
{
	int k;
	char pb[2];

	if((k = TermVal("LINES", 0x1C)))
		return k;

	return SX_TTY_ROWS;
}

/* GET TERMINAL COLUMNS
   --------------------
*/

TermCols()
{
	int k;
	char pb[2];

	if((k = TermVal("COLUMNS", 0x1A)))
		return k;

	return SX_TTY_COLS;
}

/* HELPER FOR TermRows() and TermCols()
   ------------------------------------
*/

TermVal(s, scbf)
char *s; int scbf;
{
	char *v, b[2];
	int i;

	if((v = EnvGet(s)))
	{
		i = atoi(v);

		if(i > 0 && i < 256)
			return i;
	}

	if(sv_cpmver >= 0x30)
	{
		b[0] = scbf; b[1] = 0;

		return bdos_a(0x31, b) + 1;
	}

	return 0;
}

/* APPEND A FILETYPE - IE: ".TXT"
   ------------------------------
   If the filename already has a type, return the filename.
   In other case, append the type and return the filename,
   or NULL on error.
*/

AddTypeToFn(type, fn)
char *type, *fn;
{
	if(!strchr(fn, '.'))
	{
		if(strlen(fn) + strlen(type) < FILENAME_MAX)
		{
			strcpy(sv_tmp_fn, fn); strcat(sv_tmp_fn, type);

			return sv_tmp_fn;
		}

		return NULL;
	}

	return fn;
}

/* PREFIX A FILENAME WITH A PATH - IE: "A01:"
   ------------------------------------------
   If the filename starts with ':', delete it and return the filename.
   If the filename already has a path, return the filename.
   If the path is NULL, return the filename.
   In other case, prefix the path and return the filename,
   or NULL on error.

   IE: :name    --> name
       sys:name --> sys:name
       name     --> path:name
*/

AddPathToFn(path, fn)
char *path, *fn;
{
	int k;

	if(*fn == ':')
	{
		strcpy(fn, fn + 1);
	}
	else if(path && !strchr(fn, ':'))
	{
		k = strlen(path);

		if(k + strlen(fn) < FILENAME_MAX && path[k - 1] == ':')
		{
			strcpy(sv_tmp_fn, path); strcat(sv_tmp_fn, fn);

			return sv_tmp_fn;
		}

		return NULL;
	}

	return fn;
}

/* PREFIX A FILENAME WITH THE TEMPORARY DIRECTORY
   ----------------------------------------------
*/

TmpFile(fn)
char *fn;
{
	char *t;

	if((t = AddPathToFn(EnvGet("TMPDIR"), fn)))
		return t;

	return fn;
}

/* PREFIX A FILENAME WITH THE BINARY DIRECTORY
   & APPEND THE DEFAULT FILETYPE.
   --------------------------------------------
*/

BinFile(fn)
char *fn;
{
	char *t;

	if((t = AddPathToFn(EnvGet("BINDIR"), fn)))
		fn = t;

	if((t = AddTypeToFn(".x", fn)))
		fn = t;

	return fn;
}

/* TRANSLATE A DIRECTORY NAME, TO A DRIVE AND USER
   SPECIFICATION, OR NULL IF NOT EXISTS (see cpm.h).
   -------------------------------------------------
*/

DirToDrvUsr(s)
char *s;
{
	/* Search the directory name */

	return KeyGet(sv_dir_alias, sv_dir_path, SX_MAX_DIR, s);
}

/* PRINT CURRENT WORKING DIRECTORY
   -------------------------------
*/
PrintCWD()
{
	char path[4]; /* ie: A00 */
	int x;

	/* If hasn't a name: A00:        */
	/* If has a name:    A00|SYSTEM: */

	sprintf(path, "%c%02d", 'A' + sv_drive, sv_user);

	putstr(path);

	if((x = KeyFind(sv_dir_path, SX_MAX_DIR, path)) != -1)
	{
		putchar('|'); putstr(sv_dir_alias[x]);
	}

	putchar(':');
}

/* ERROR OUTPUT
   ------------
   Return -1.
*/

Error(s)
char *s;
{
	fprintf(stderr, "\nERROR [%s]: %s\n", sv_cmd_err, s);

	return -1;
}

/* FATAL ERROR
   -----------
   Return to CP/M.
*/

ErrorFatal(s)
char *s;
{
	Error(s); exit(-1);
}

/* COMMON ERROR MESSAGES
   ---------------------
*/

ErrorPars()
{
	return Error("Wrong number of parameters");
}

ErrorOpt()
{
	return Error("Bad option");
}

ErrorIll()
{
	return Error("Illegal command line");
}

ErrorFName()
{
	return Error("Bad file name");
}

ErrorDir()
{
	return Error("Bad directory");
}

ErrorCreate()
{
	return Error("Creating file");
}

ErrorDelete()
{
	return Error("Deleting file");
}

ErrorOpen()
{
	return Error("Opening file");
}

ErrorRename()
{
	return Error("Renaming file");
}

ErrorMem()
{
	return Error("No memory");
}

ErrorClose()
{
	return Error("Closing file");
}

ErrorRead()
{
	return Error("Reading file");
}

ErrorWrite()
{
	return Error("Writing file");
}

ErrorAmb()
{
	return Error("Ambiguous file name");
}

ErrorTooMany()
{
	return Error("Too many files");
}

ErrorTooLong()
{
	return Error("Line too long");
}

ErrorNoFile()
{
	return Error("No file");
}

ErrorRedir()
{
	return Error("Bad redirection");
}

ErrorFindVar()
{
	return Error("Can't find $ variable");
}

#ifdef SX_EXTCMDS

/* LOAD EXTERNAL COMMAND
   ---------------------
*/

LoadPrl(fn)
char *fn;
{
	FILE *fp;
	BYTE *adr, *ptr;
	unsigned int size, i;
	int err, mask, rel;

#ifdef SX_DEBUG
	fprintf(stderr, "[LoadPrl FN = %s]\n", fn);
#endif

	/* Setup values */

	err = 0; adr = NULL;

	/* Open file */

	if((fp = fopen(fn, "rb")))
	{
		/* Discard 1 byte in PRL header */

		fgetc(fp);

		/* Read code size: 1 WORD */

		size = fgetc(fp) & 0xFF;
		size = (fgetc(fp) << 8) + size;

		/* Get enough memory: PRL code size + a page of 256 - 1 bytes, because
		   a PRL must start in an address like: 0x??00 */

		if((adr = malloc(size + 255)))
		{
#ifdef SX_DEBUG
	fprintf(stderr, "[LoadPrl ADR = %04x SIZE = %u]\n", adr, size + 255);
#endif
			/* Discard the rest of the header (256 - 3 bytes already read) */

			for(i = 3; i < 256; ++i)
				fgetc(fp);

			/* Compute start address of the code: 0x??00 */

			ptr = (adr & 0x00FF) ? (adr & 0xFF00) + 0x100 : adr;

#ifdef SX_DEBUG
	fprintf(stderr, "[LoadPrl INIT = %04x]\n", ptr);
#endif

			/* Read the code */

			if(fread(ptr, size, 1, fp) == 1)
			{
				/* Check SamaruX header */

				if(strcmp("SamaruX", ptr + 3))
					err = Error("Bad header");
				else if(ptr[11] != SX_VERSION || ptr[12] != SX_RELEASE)
					err = Error("Version mismatch");
				else
				{
					/* Read relocation bitmap and relocate code */

					for(i = mask = 0; i < size; ++i)
					{
						/* Read 1 byte from bitmap and reset bit mask */

						if(!mask)
						{
							rel = fgetc(fp); mask = 128;
						}

						/* Relocate byte if needed */

						if(mask & rel)
							ptr[i] += ((ptr >> 8) & 0xFF) - 1;

						/* Update bit mask */

						mask = (mask >> 1) /*** & 0xFF ***/;
					}
				}
			}
			else
				err = ErrorRead();
		}
		else
			err = ErrorMem();

		fclose(fp);
	}
	else
		err = Error("Not found");

#ifdef SX_DEBUG
	fprintf(stderr, "[LoadPrl RET = %04x]\n", err ? NULL : adr);
#endif

	/* Return success or failure */

	if(err)
	{
		free(adr);

		return NULL;
	}

	return adr;
}

#endif

#ifdef SX_HKEXIT

/* HOOK FOR exit()
   ---------------
*/
 
HookExit(code)
int code;
{
	/* Save code -- we can't use it directly because can be 0,
	   and it is not allowed for longjmp() */
	sx_exit_code = code;
	
	/* Long jump */
	longjmp(sx_exit_jpbf, 1);
}

#endif
