/*	WTD - WHAT TO DO

	A "to do list" manager for CP/M & Z80.

	Based on the "todo.txt" format by Gina Trapani. More info at 'http://todotxt.com'.

	(c) Floppy Software. Valencia, Spain.

	-----------------------------------------------------------------------

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

	-----------------------------------------------------------------------

	13 Aug 2014 : Start of coding
	28 Aug 2014 : v1.0
	02 Sep 2014 : v1.01

	To compile in MESCC (v1.02 or higher):

	cc wtd -a-
*/

/* 13/08/14 */
/* 15/08/14 : Added sort */
/* 16/08/14 : Added Add, Edit, Done, Kill */
/* 16/08/14 : Added Save */
/* 16/08/14 : Added two sort methods */
/* 17/08/14 : Added filtering */
/* 18/08/14 : Added help */
/* 20/08/14 : Added Pack */
/* 21/08/14 : Added optional parameter to Add, Edit, Done, Kill commands */
/* 22/08/14 : Added optional parameter to Print command */
/* 22/08/14 : Added 'saved' flag */
/* 24/08/14 : Help amended */
/* 24/08/14 : Now uses commands words instead of single characters */
/* 24/08/14 : Solved bug in Pack command */
/* 24/08/14 : Added Pending command */
/* 28/08/14 : Solved bug in KillTask function */

/* 02/09/14 : Added screen lines # and pause on PRINT */

/* MESCC libs.:
*/

#include <mescc.h>
#include <ctype.h>
#include <string.h>
#include <mem.h>
#include <conio.h>
#include <alloc.h>
#include <fileio.h>
#include <fprintf.h>
#include <printf.h>

/* Task defs.:
*/

#define MAX_TASKS 250 /* Max. number of tasks */

#define T_DONE_SZ 1   /* Size in bytes of field DONE */
#define T_PRIO_SZ 1   /* Size in bytes of field PRIORITY */
#define T_DATE_SZ 11  /* Size in bytes of field DATE */
#define T_SUBJ_SZ 48  /* Size in bytes of field SUBJECT */

#define T_DONE_LN 1   /* Length in chars. of field DONE */
#define T_PRIO_LN 1   /* Length in chars. of field PRIORITY */
#define T_DATE_LN 10  /* Length in chars. of field DATE */
#define T_SUBJ_LN 47  /* Length in chars. of field SUBJECT */

char *tsk_done,       /* Done? */
     *tsk_prio;       /* Priority */
WORD *tsk_date,       /* Date - char *tsk_date[] */
     *tsk_subj;       /* Subject - char *tsk_subject[] */

char *tsk_date_bf,    /* Buffer for date */
     *tsk_subj_bf;    /* Buffer for subject */

int tasks;            /* Number of stored tasks */

/* File defs.:
*/

#define F_SIZE    65  /* File buffer size in bytes for each line (task): */
                      /* 'x YYYY-MM-DD subject', 0 */

FILE *fpi,            /* Input file */
     *fpo;            /* Output file */

char *filbuf;         /* File buffer */

char *fn_todo,        /* Filename of data file */
     *fn_temp;        /* Filename of temporary file */

/* ReadLine (keyboard) defs.:
*/

#define RL_EMPTY 1    /* Delete buffer */
#define RL_UPPER 2    /* Upper case */

char *kbf_done,       /* Done? */
     *kbf_prio,       /* Priority */
     *kbf_date,       /* Date */
     *kbf_subj;       /* Subject */

/* Filter defs.:
*/

char *flt_done,       /* Done? */
     *flt_prio,       /* Priority */
     *flt_date,       /* Date */
     *flt_subj;       /* Subject */

BOOL filter;          /* TRUE if any filter is active */

/* Other defs.
*/

BOOL saved;           /* TRUE if data is saved on file (updated) */

int scr_lines;        /* Number of lines in screen (for PRINT pause) */

/* Program entry
*/

main(argc, argv)
int argc; WORD argv[];
{
	Intro();

	Setup();

	ReadFile();

	Menu();
}

/* Copyright message & intro
*/

Intro()
{
	putnl("WTD - What To Do: A 'to do list' manager");
	putnl("v1.01 for CP/M & Z80\n");
	putnl("(c) 2014 Floppy Software. Valencia (Spain).");
	putnl("www.floppysoftware.vacau.com");
	putnl("cpm-connections.blogspot.com.es");
	putnl("floppysoftware@gmail.com\n");
}

/* Main menu
*/

Menu()
{
	BOOL stop; char *buf;

	stop = FALSE; buf = kbf_subj;

	while(stop == FALSE)
	{
		put("Command (HELP for help)? ");

		ReadLine(buf, T_SUBJ_LN, RL_EMPTY);

		nl();

		if(!(*buf))
			continue;

		/* Simple commands */

		if(MatchCmd("ADD", buf))
			DoAddTask();
		else if(MatchCmd("FILTER", buf))
			DoFilter();
		else if(MatchCmd("PACK", buf))
			DoPack();
		else if(MatchCmd("PRINT", buf))
			DoPrint();
		else if(MatchCmd("SAVE", buf))
			DoSave();
		else if(MatchCmd("HELP", buf))
			DoHelp();
		else if(MatchCmd("QUIT", buf))
			stop = DoQuit();

		/* Commands + parameter */

		else if(MatchCmdEx("ADD", buf))
			DoAddTaskEx(buf + 4);
		else if(MatchCmdEx("EDIT", buf))
			DoChgTaskEx(buf + 5);
		else if(MatchCmdEx("KILL", buf))
			DoKillTaskEx(buf + 5);
		else if(MatchCmdEx("DONE", buf))
			DoDoneTaskEx(buf + 5);
		else if(MatchCmdEx("PENDING", buf))
			DoPendTaskEx(buf + 8);
		else if(MatchCmdEx("PRINT", buf))
			DoPrintEx(buf + 6);
		else if(MatchCmdEx("SORT", buf))
			DoSortEx(buf + 5);
		else if(MatchCmdEx("FILTER", buf))
			DoFilterEx(buf + 7);
		else
			Warning("Bad command or syntax error");
	}
}

/* Setup program
*/

Setup()
{
	int i; BYTE *pb_date, *pb_subj;

	tsk_done = malloc(MAX_TASKS);
	tsk_prio = malloc(MAX_TASKS);
	tsk_date = malloc(MAX_TASKS * 2);
	tsk_subj = malloc(MAX_TASKS * 2);

	tsk_date_bf = malloc(MAX_TASKS * T_DATE_SZ);
	tsk_subj_bf = malloc(MAX_TASKS * T_SUBJ_SZ);

	filbuf = malloc(F_SIZE);

	kbf_done = malloc(2);
	kbf_prio = malloc(2);
	kbf_date = malloc(T_DATE_SZ);
	kbf_subj = malloc(T_SUBJ_SZ);

	flt_done = malloc(2);
	flt_prio = malloc(2);
	flt_date = malloc(T_DATE_SZ);
	flt_subj = malloc(T_SUBJ_SZ);

	if(tsk_done == NULL || tsk_prio == NULL || tsk_date == NULL || tsk_subj == NULL ||
		tsk_date_bf == NULL || tsk_subj_bf == NULL || filbuf == NULL ||
		kbf_done == NULL || kbf_prio == NULL || kbf_date == NULL || kbf_subj == NULL ||
		flt_done == NULL || flt_prio == NULL || flt_date == NULL || flt_subj == NULL)
			Error("Not enough memory for buffers");

	pb_date = tsk_date_bf;
	pb_subj = tsk_subj_bf;

	for(i = 0; i < MAX_TASKS; ++i)
	{
		tsk_date[i] = pb_date; pb_date += T_DATE_SZ;
		tsk_subj[i] = pb_subj; pb_subj += T_SUBJ_SZ;
	}

	tasks = 0;

	*flt_done = *flt_prio = *flt_date = *flt_subj = 0;

	filter = FALSE;

	fn_todo = "todo.txt";
	fn_temp = "todo.$$$";

	scr_lines = 25;

	saved = TRUE;
}

/* ************************************
*/

/* Option QUIT: Return TRUE to quit program, else FALSE.
*/

DoQuit()
{
	if(saved == FALSE)
		return AreYouSure("There is unsaved data");

	return TRUE;
}

/* Option SAVE: Save "todo.txt" file on disc.
*/

DoSave()
{
	char *pb; int i;

	put("Saving tasks... ");

	if((fpo = fopen(fn_temp, "w")) == NULL)
		DoSaveErr();

	for(i = 0; i < tasks; ++i)
	{
		if(tsk_done[i] == 'x')
			fprintf(fpo, "x ");

		if(tsk_prio[i] != 0x7F)
			fprintf(fpo, "(%c) ", tsk_prio[i]);

		pb = tsk_date[i];

		if(*pb != 0x7F)
			fprintf(fpo, "%s ", tsk_date[i]);

		fprintf(fpo, "%s\n", tsk_subj[i]);

		if(ferror(fpo))
			DoSaveErr();
	}

	if(fclose(fpo))
		DoSaveErr();

	if(remove(fn_todo))
		DoSaveErr();

	if(rename(fn_temp, fn_todo))
		DoSaveErr();

	putnl("Ok\n");

	saved = TRUE;
}

DoSaveErr()
{
	putnl("Error"); Error("Saving tasks");
}

/* Option ADD: Add task.
*/

DoAddTaskEx(s)
char *s;
{
	if(tasks < MAX_TASKS)
	{
		BufToTsk(s, tasks++); saved = FALSE;
	}
	else
		Warning("Too many tasks");
}

DoAddTask()
{
	AskTask(FALSE, TRUE); *kbf_done = 0; KbfToBuf(); DoAddTaskEx(filbuf);
}

/* Option FILTER: Set / reset filters.
*/

DoFilterEx(s)
char *s;
{
	if(MatchCmd("OFF", s))
		filter = FALSE;
	else
		Warning("Bad option");
}

DoFilter()
{
	AskTask(TRUE, TRUE);

	strcpy(flt_done, kbf_done);
	strcpy(flt_prio, kbf_prio);
	strcpy(flt_date, kbf_date);
	strcpy(flt_subj, kbf_subj);

	filter = (*flt_done || *flt_prio || *flt_date || *flt_subj) ? TRUE : FALSE;
}

/* Option EDIT: Edit task fields.
*/

DoChgTaskEx(s)
char *s;
{
	int task; char *pb;

	if((task = CompTask(s)) != -1)
	{
		if(tsk_done[task] != ' ')
		{
			*kbf_done = tsk_done[task]; kbf_done[1] = 0;
		}
		else
			*kbf_done = 0;

		if(tsk_prio[task] != 0x7F)
		{
			*kbf_prio = tsk_prio[task]; kbf_prio[1] = 0;
		}
		else
			*kbf_prio = 0;

		pb = tsk_date[task];

		if(*pb != 0x7F)
			strcpy(kbf_date, tsk_date[task]);
		else
			*kbf_date = 0;

		strcpy(kbf_subj, tsk_subj[task]);

		AskTask(FALSE, FALSE); KbfToBuf(); BufToTsk(filbuf, task);

		saved = FALSE;
	}
}

/* Option DONE: Set task as done.
*/

DoDoneTaskEx(s)
char *s;
{
	int task;

	if((task = CompTask(s)) != -1)
	{
		tsk_done[task] = 'x'; saved = FALSE;
	}
}

/* Option PENDING: Set task as pending.
*/

DoPendTaskEx(s)
char *s;
{
	int task;

	if((task = CompTask(s)) != -1)
	{
		tsk_done[task] = ' '; saved = FALSE;
	}
}

/* Option KILL: Kill (delete) task.
*/

DoKillTaskEx(s)
char *s;
{
	int task;

	if((task = CompTask(s)) != -1)
	{
		KillTask(task); saved = FALSE;
	}
}

/* Option PACK: Kill all done tasks.
*/

DoPack()
{
	int i, k;

	if(AreYouSure("Done tasks will be killed") == FALSE)
		return;

	put("Packing... ");

	/* Caution: KillTask modifies variable 'tasks' */

	i = 0; k = tasks;

	while(i < tasks)
	{
		if(tsk_done[i] == 'x')
			KillTask(i);
		else
			++i;
	}

	saved = FALSE;

	printf("%d tasks killed\n\n", k - tasks);
}

/* Option PRINT: Print tasks with (optional) filtering.
*/

DoPrintEx(s)
char *s;
{
	int i, k, lines, sline, z; char *pb;

	if((lines = atoi(s)) < 1)
	{
		Warning("Bad number of tasks");	return;
	}

	sline = 3; /* Header size in lines */

	/* Header */

	put("+-----+---+---+------------+-"); putrpt('-', T_SUBJ_LN); putnl("-+");
	put("| Tsk | D | P | Date       | Subject"); putrpt(' ', T_SUBJ_LN - 7); putnl(" |");

	/* Print filters (if any) */

	if(filter)
	{
		printf("|     | %c | %c ", *flt_done ? *flt_done : ' ', *flt_prio ? *flt_prio : ' ');
		printf("| %s", flt_date); putrpt(' ', T_DATE_LN - strlen(flt_date) + 1);
		printf("| %s", flt_subj); putrpt(' ', T_SUBJ_LN - strlen(flt_subj) + 1);
		putchar('|'); nl();

		++sline;
	}

	put("+-----+---+---+------------+-"); putrpt('-', T_SUBJ_LN); putnl("-+");

	/* Print tasks that match with filters */

	for(i = k = 0; i < tasks; ++i)
	{
		if(filter)
		{
			if(ChkFilter(i) == FALSE)
				continue;
		}

		printf("| %03d | %c | %c ", i, tsk_done[i], tsk_prio[i] == 0x7F ? ' ' : tsk_prio[i]);

		pb = tsk_date[i];

		printf("| %s ", *pb != 0x7F ? tsk_date[i] : "          ");

		printf("| %s", tsk_subj[i]);

		putrpt(' ', T_SUBJ_LN - strlen(tsk_subj[i]) + 1); putchar('|'); nl();

		++k;

		/* Pause ? */

		if(++sline == scr_lines - 1)
		{
			put("Press ANY key to continue, ESC to cancel: "); z = getch(); putchar('\r');

			if(z == 0x1B)
				break;

			sline = 0;
		}

		/* Check if we have print the required # of tasks */

		if(!(--lines))
			break;
	}

	put("+-----+---+---+------------+-"); putrpt('-', T_SUBJ_LN); putnl("-+\n");

	/* Footer */

	printf("%d of %d tasks shown\n\n", k, tasks);
}

DoPrint()
{
	DoPrintEx("9999");
}

/* Option SORT: Sort tasks.
*/

DoSortEx(s)
char *s;
{
	if(MatchCmd("PRIORITY", s))
		Sort('P');
	else if(MatchCmd("DATE", s))
		Sort('D');
	else
		Warning("Bad sort method");
}

/* Option HELP: Show command summary.
*/

DoHelp()
{
	putnl("Help summary:\n");
	putnl("ADD     {task def}       : Add task");
	putnl("EDIT    task-number      : Edit task");
	putnl("KILL    task-number      : Kill (delete) task");
	putnl("DONE    task-number      : Set task as done");
	putnl("PENDING task-number      : Set task as pending");
	putnl("SORT    PRIORITY | DATE  : Sort tasks by Priority or by Date");
	putnl("FILTER  {OFF}            : Set filters for Print option");
	putnl("PACK                     : Kill all done tasks");
	putnl("PRINT   {how-many-tasks} : Print tasks on screen");
	putnl("SAVE                     : Save taks on disk");
	putnl("HELP                     : This option");
	putnl("QUIT                     : End program\n");
	putnl("task def = {priority} {date} subject");
	putnl("I.E.     = (B) 2014-08-16 Call Peter +Party @Phone");
	putnl("         = Paint kitchen +Home\n");
	putnl("Items between {} are optional.\n");
}

/* ************************************
*/

/* Ask task fields.
   Set TRUE to 'doneflag' if field DONE have to be entered.
   Set TRUE to 'newflag' if fields have to be erased before ask.
*/

AskTask(doneflag, newflag)
BOOL doneflag, newflag;
{
	int flags;

	flags = newflag ? RL_EMPTY : 0;

	if(doneflag)
	{
		put("Done (x/CR)          : "); ReadLine(kbf_done, T_DONE_LN, flags); nl();
	}

	put("Priority (A:Z/CR)    : "); ReadLine(kbf_prio, T_PRIO_LN, flags | RL_UPPER); nl();
	put("Date (yyyy-mm-dd/CR) : "); ReadLine(kbf_date, T_DATE_LN, flags); nl();
	put("Subject              : "); ReadLine(kbf_subj, T_SUBJ_LN, flags); nl(); nl();
}

/* Convert keyboard buffers to raw buffer data.
*/

KbfToBuf()
{
	char *pb;

	pb = filbuf;

	if(*kbf_done)
	{
		pb[0] = *kbf_done; pb[1] = ' ';
		pb += 2;
	}

	if(*kbf_prio)
	{
		pb[0] = '('; pb[1] = *kbf_prio; pb[2] = ')'; pb[3] = ' ';
		pb += 4;
	}

	if(*kbf_date)
	{
		strcpy(pb, kbf_date); pb[strlen(kbf_date)] = ' ';
		pb += strlen(kbf_date) + 1;
	}

	strcpy(pb, kbf_subj);
}

/* Convert raw buffer data to task data.
*/

BufToTsk(bf, task)
BYTE *bf; int task;
{
	char *pb;

	/* Done */

	if(*bf == 'x' && bf[1] == ' ')
	{
  		tsk_done[task] = 'x'; bf += 2;
 	}
 	else
  		tsk_done[task] = ' ';

 	/* Priority */

 	if(*bf == '(' && (bf[1] >= 'A' && bf[1] <= 'Z') && bf[2] == ')' && bf[3] == ' ')
 	{
  		tsk_prio[task] = bf[1]; bf += 4;
 	}
 	else
  		tsk_prio[task] = 0x7F;

 	/* Date */

 	if(isdigit(bf[0]) && isdigit(bf[1]) && isdigit(bf[2]) && isdigit(bf[3])
		&& bf[4] == '-' && isdigit(bf[5]) && isdigit(bf[6])
		&& bf[7] == '-' && isdigit(bf[8]) && isdigit(bf[9]) && bf[10] == ' ')
 	{
		memcpy(tsk_date[task], bf, 10); bf += 11;
 	}
	else
		memset(tsk_date[task], 0x7F, 10);

	pb = tsk_date[task]; pb[10] = 0;

	/* Subject */

	if(strlen(bf) < T_SUBJ_SZ)
		strcpy(tsk_subj[task], bf);
	else
		Error("Subject too long");
}

/* Test if a task matchs with filters. Return TRUE or FALSE.
*/

ChkFilter(task)
int task;
{
	int match;

	match = 0;

	if(*flt_done)
	{
		if(*flt_done == tsk_done[task])
			++match;
	}
	else
		++match;

	if(*flt_prio)
	{
		if(*flt_prio == tsk_prio[task])
			++match;
	}
	else
		++match;

	if(*flt_date)
	{
		if(FindSubStr(tsk_date[task], flt_date) != NULL)
			++match;
	}
	else
		++match;

	if(*flt_subj)
	{
		if(FindSubStr(tsk_subj[task], flt_subj) != NULL)
			++match;
	}
	else
		++match;

	return match == 4 ? TRUE : FALSE;
}

/* Compute task number. Return task number or -1 on error.
*/

CompTask(s)
char *s;
{
	int task;

	if(*s && isdigit(*s))
	{
		task = atoi(s);

		if(task >= 0 && task < tasks)
			return task;
	}

	Warning("Bad Task number");

	return -1;
}

/* Read line from keyboard
*/

ReadLine(buf, width, flags)
char *buf;
int width, flags;
{
	int len; char ch;

	if(flags & RL_EMPTY)
		*buf = len = 0;
	else
	{
		put(buf); len = strlen(buf);
	}

	while(1)
	{
		switch(ch = getch())
		{
			case 8   :
			case 127 :
				if(len)
				{
					putchar(8); putchar(' '); putchar(8); --len;
				}
				break;

			case '\r' :
			case '\n' :
				buf[len] = 0;
				return;

			default :
				if(ch >= ' ' && len < width)
					putchar(buf[len++] = flags & RL_UPPER ? toupper(ch) : ch);
		}
	}
}

/* Sort tasks (Bubble sort?).
*/

Sort(type)
char type;
{
	put("Sorting... ");

	int i, j;

	for(i = 0; i < tasks; ++i)
	{
  		for(j = i + 1; j < tasks; ++j)
		{
   			if(Comp(i, j, type) > 0) /* (a[i] > a[j]) */
			{
				Swap(i, j);
   			}
  		}
 	}

	putnl("Ok\n");
}

/* Compares two tasks
*/

Comp(a, b, type)
int a, b; char type;
{
	int dif;

	/* Type 'P' : Done, priority, date, subject */
	/* Type 'D' : Done, date, priority, subject */


	if((dif = tsk_done[a] - tsk_done[b]))
		return dif;

	if(type == 'P')
	{
		if((dif = tsk_prio[a] - tsk_prio[b]))
			return dif;

		if((dif = strcmp(tsk_date[a], tsk_date[b])))
			return dif;
	}
	else /* 'D' */
	{
		if((dif = strcmp(tsk_date[a], tsk_date[b])))
			return dif;

		if((dif = tsk_prio[a] - tsk_prio[b]))
			return dif;
	}

	return strcmp(tsk_subj[a], tsk_subj[b]);
}

/* Swap two tasks
*/

Swap(a, b)
int a, b;
{
	BYTE tb; WORD tw;

	tb = tsk_done[a]; tsk_done[a] = tsk_done[b]; tsk_done[b] = tb;

	tb = tsk_prio[a]; tsk_prio[a] = tsk_prio[b]; tsk_prio[b] = tb;

	tw = tsk_date[a]; tsk_date[a] = tsk_date[b]; tsk_date[b] = tw;

	tw = tsk_subj[a]; tsk_subj[a] = tsk_subj[b]; tsk_subj[b] = tw;
}

/* Read "todo.txt" file
*/

ReadFile()
{
	int k;

	put("Loading tasks... ");

	if((fpi = fopen(fn_todo, "r")) == NULL)
		Error("Reading file");

	for(tasks = 0; fgets(filbuf, F_SIZE, fpi) != NULL; ++tasks)
	{
		if(tasks == MAX_TASKS)
			Error("Too many lines");

		k = strlen(filbuf);

		if(filbuf[k - 1] != '\n')
			Error("Line too long");

		filbuf[k - 1] = 0;

		BufToTsk(filbuf, tasks);
	}

	fclose(fpi);

	putnl("Ok\n");

	return tasks;
}

/* Kill task
*/

KillTask(task)
int task;
{
	int i; WORD p_date, p_subj;

	if(task != tasks - 1)
	{
		p_date = tsk_date[task];
		p_subj = tsk_subj[task];

		for(i = task; i < tasks - 1; ++i)
		{
			tsk_done[i] = tsk_done[i + 1];
			tsk_prio[i] = tsk_prio[i + 1];
			tsk_date[i] = tsk_date[i + 1];
			tsk_subj[i] = tsk_subj[i + 1];
		}

		tsk_date[tasks - 1] = p_date;
		tsk_subj[tasks - 1] = p_subj;
	}

	--tasks;
}

/* Test command name. Returns TRUE or FALSE.
*/

MatchCmd(cmd, t)
char *cmd, *t;
{
	while(*cmd == toupper(*t))
	{
    		if(!(*cmd))
			return TRUE;
		++cmd;
		++t;
	}

	return FALSE;
}

/* Test command name with parameter. Returns TRUE or FALSE.
*/

MatchCmdEx(cmd, t)
char *cmd, *t;
{
	while(*cmd == toupper(*t))
	{
    		if(!(*cmd))
			return FALSE;
		++cmd;
		++t;
	}

	if(!(*cmd) && t[0] == ' ' && t[1])
		return TRUE;

	return FALSE;
}

/* Search a (sub)string inside of another string (like 'strstr' C function).
   Returns pointer to address of substring inside the string, or NULL.
*/

char *FindSubStr(str, sbs)
char *str, *sbs;
{
	char *p, *s1, *s2;

	if (!(*sbs))
	    return str;

	for(p = str; *p; ++p)
	{
		s1 = p;
		s2 = sbs;

		while(*s1 && *s2 && !(*s1-*s2))
		{
			++s1; ++s2;
		}

		if (!(*s2))
			return p;
	}

	return NULL;
}

/* Prints error message and aborts program
*/

Error(s)
char *s;
{
	printf("\n\nWTD ERROR: %s", s); exit(-1);
}

/* Prints warning message
*/

Warning(s)
char *s;
{
	put(s); putchar('!'); nl(); nl();
}

/* Gets confirmation
*/

AreYouSure(s)
char *s;
{
	char buf[2];

	put(s); put(". Are you sure (Y/N)? ");

	ReadLine(buf, 1, RL_UPPER | RL_EMPTY); nl(); nl();

	return *buf == 'Y' ? TRUE : FALSE;
}

/* Prints new line
*/

nl()
{
	putchar('\n');
}

/* Prints string
*/

put(s)
char *s;
{
	while(*s)
		putchar(*s++);
}

/* Prints string + new line
*/

putnl(s)
char *s;
{
	put(s); nl();
}

/* Prints a character a number of times
*/

putrpt(ch, rpt)
int ch, rpt;
{
	while(rpt--)
		putchar(ch);
}



