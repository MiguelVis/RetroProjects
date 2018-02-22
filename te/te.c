/*	te.c

	Text editor.

	Main module.

	Copyright (c) 2015-2018 Miguel Garcia / FloppySoftware

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

	Notes:

	Change TE_VERSION in 'te.h' as required, before compilation.

	Changes:

	29 Apr 2015 : Start working.
	02 May 2015 : Added Amstrad PCW version (te_pcw.c).
	03 May 2015 : Added K. Murakami's CP/M emulator for Win32 version (te_em1.c).
	04 May 2015 : Added te.h file. Added forced entry. Nice ruler with tab stops.
	07 May 2015 : While porting it to Pelles C & Windows 32 bit, it was discovered
	              collateral efects in 3 pieces of code. Recoded and marked below.
	              Solved bug in ReadFile() - characters > 0x7F caused 'bad
	              character' errors.
	              Added checking for filenames too long in command line.
	08 May 2015 : v1.00 : Added BackupFile().
	12 May 2015 : v1.01 : Now K_CUT copies the line to the clipboard before deleting it.
	14 May 2015 : v1.02 : Added support for word left & word right cursor movement.
	                      You must define K_LWORD and K_RWORD in your CRT_FILE
	                      to activate them.
	15 May 2015 : v1.03 : Modified getch & putch to getchr & putchr.
	31 Aug 2015 : v1.04 : Minor changes in comments and ReadLine().
	02 Jun 2016 : v1.05 : Minor changes.
	10 Jun 2016 : v1.06 : Screen optimizations in Menu(). Removed BOX_COL.
	                      Removed lp_max, box_cols, ps_fname, ps_lin_cur, ps_lin_now, ps_lin_max,
	                      ps_col_cur, ps_col_now, ps_col_max.
	14 Jun 2016 : v1.07 : Hack for SamaruX.
	05 Jul 2017 : v1.08 : Optimizations in NULL comparisons. Include CC_FGETS.
	24 Jan 2018 : v1.09 : Added find string and find next string commands.
	20 Feb 2018 : v1.10 : Added command to execute macro from file. Split te.c in modules. Added go to line #.
	                      Disable code for macros from strings, for now.
	22 Feb 2018 : v1.11 : Ask for confirmation only if changes were not saved.

	Notes:

	See FIX-ME notes.
*/

/* Operating System
   ----------------
*/
#define OS_CPM
//#define OS_SAMARUX

/* Libraries
   ---------
*/
#ifdef OS_SAMARUX
#include <samarux.h>
#define ReadFile InputFile
#define ReadLine InputLine
#define putstr putstring
#else

#define CC_FGETS

#include <mescc.h>
#include <string.h>
#include <ctype.h>
#include <fileio.h>
#include <sprintf.h>
#endif

/* TE definitions
   --------------
*/
#include <te.h>

/* Globals
   -------
*/

/* Array of text lines
   -------------------
*/
WORD *lp_arr; /* Text lines pointers array */
int   lp_now; /* How man lines are in the array */
int   lp_cur; /* Current line */
int   lp_chg; /* 0 if no changes are made */

/* Current line
   ------------
*/
char *ln_dat; /* Data buffer */
int   ln_max; /* Max. # of characters */
char *ln_clp; /* Clipboard data (just one line) */

/* Filename
   --------
*/
char file_name[FILENAME_MAX];

/* Editor box
   ----------
*/
int box_rows; /* Height in lines */
int box_shr;  /* Vertical   position of cursor in the box (0..box_rows - 1) */
int box_shc;  /* Horizontal position of cursor in the box (0..CRT_COLS - 1) */

/* Keyboard forced entry
   ---------------------
*/
char *fe_dat; /* Data buffer */
int fe_now;   /* How many characters are now in the buffer */
int fe_set;   /* Set position */
int fe_get;   /* Get position */

/* System line
   -----------
*/
int sysln;    /* NZ when written - for Loop() */

#ifdef K_FIND

/* Find string
   -----------
*/
char find_str[FIND_MAX];

#endif

#ifdef K_MACRO

/* Macros
   ------
*/
FILE *mac_fp;  /* FP for a file macro, or NULL */
/*char *mac_str;*/ /* Address for a string macro, or NULL */

#endif

/* TE modules
   ----------
*/
#include "te_ui.c"
#include "te_file.c"
#include "te_edit.c"
#include "te_error.c"

#ifdef K_MACRO
#include "te_macro.c"
#endif

/* Program entry
   -------------
*/
main(argc, argv)
int argc, argv[];
{
	int i;

	/* Setup some globals */

	box_rows = CRT_ROWS - 4;

	ln_max = CRT_COLS - 1;

	/* Setup CRT */

	CrtSetup();

	/* Print layout */

	Layout();

	/* Allocate buffers */

	ln_dat = malloc(ln_max + 2);

	ln_clp = malloc(ln_max + 1);

	fe_dat = malloc(FORCED_MAX);

	lp_arr = malloc(MAX_LINES * SIZEOF_PTR);

	//if(ln_dat == NULL || ln_clp == NULL || fe_dat == NULL || lp_arr == NULL)
	if(!ln_dat || !ln_clp || !fe_dat || !lp_arr)
	{
		ErrLineMem(); CrtReset(); return 1;
	}

	/* Setup more things */

	*ln_clp = 0;

	/* Setup lines array */

	for(i = 0; i < MAX_LINES; ++i)
		lp_arr[i] = NULL;

	/* Check command line */

	if(argc == 1)
	{
		NewFile();
	}
	else if(argc == 2)
	{
		if(strlen(argv[1]) > FILENAME_MAX - 1)
		{
			ErrLine("Filename too long.");

			NewFile();
		}
		else if(ReadFile(argv[1]))
			NewFile();
		else
			strcpy(file_name, argv[1]);
	}
	else
	{
		ErrLine("Bad command line. Use: te [filename]");

		NewFile();
	}

	/* Main loop */

	Loop();

	/* Free allocated buffers with malloc? Not needed in CP/M. */

#ifdef OS_SAMARUX
	free(lp_arr);
	free(fe_dat);
	free(ln_clp);
	free(ln_dat);
#endif

	/* Clear CRT */

	CrtClear();

	/* Reset CRT */

	CrtReset();

	/* Exit */

	return 0;
}

/* Main loop
   ---------
*/
Loop()
{
	int run, ch;

	/* Setup forced entry */

	fe_now = fe_get = fe_set = 0;

	/* Setup more things */

	run = sysln = 1;

	/* Print filename */

	LoopFileName();

	/* Refresh editor box */

	RefreshAll();

	/* Loop */

	while(run)
	{
		/* Refresh system line message if changed */

		if(sysln)
		{
			SysLine("Press [");
			putstr(CRT_ESC_KEY);
			putstr("] to show the menu.");

			sysln = 0; /* Reset flag */
		}

		/* Print current line number and number of lines */

		CrtLocate(PS_ROW, PS_LIN_CUR); putint("%04d", lp_cur + 1);
		CrtLocate(PS_ROW, PS_LIN_NOW); putint("%04d", lp_now);

		/* Edit the line */

		ch = BfEdit();

		/* Note: BfEdit() does previous checks for following
		   actions, to not waste time when an action is not
		   possible */

		/* Check returned control character */

		switch(ch)
		{
			case K_UP :    /* Up one line --------------------- */
				--lp_cur;

				if(box_shr)
					--box_shr;
				else
					Refresh(0, lp_cur);
				break;
			case K_DOWN :  /* Down one line ------------------- */
				++lp_cur;

				if(box_shr < box_rows - 1)
					++box_shr;
				else
					Refresh(0, lp_cur - box_rows + 1);
				break;
			case K_INTRO : /* Insert CR ----------------------- */
				LoopInsert();
				break;
			case K_LDEL :  /* Delete CR on the left ----------- */
				LoopLeftDel();
				break;
			case K_RDEL :  /* Delete CR on the right ---------- */
				LoopRightDel();
				break;
			case K_PGUP :  /* Page up ------------------------- */
				LoopPgUp();
				break;
			case K_PGDOWN :/* Page down ----------------------- */
				LoopPgDown();
				break;
			case K_TOP :   /* Top of document ----------------- */
				LoopTop();
				break;
			case K_BOTTOM :/* Bottom of document -------------- */
				LoopBottom();
				break;
#ifdef K_FIND
			case K_FIND :  /* Find string --------------------- */
				LoopFindFirst();
				break;
			case K_NEXT :  /* Find next string ---------------- */
				LoopFindNext();
				break;
#endif

#ifdef K_GOTO
			case K_GOTO : /* Go to line # --------------------- */
				LoopGoLine();
				break;
#endif

#ifdef K_MACRO
			case K_MACRO : /* Execute macro from file --------- */
				LoopMacro();
				break;
#endif

			case K_ESC :   /* Escape: show the menu ----------- */
				if(Menu())
					run = 0;
				else
				{
					LoopFileName(); /* Refresh filename */
					RefreshAll();   /* Refresh editor box */
				}
				break;
		}
	}
}

/* Return line # of first line printed on the editor box
   -----------------------------------------------------
*/
LoopFirst()
{
	return lp_cur - box_shr;
}

/* Return line # of last line printed on the editor box
   ----------------------------------------------------
*/
LoopLast()
{
	int last;

	last = LoopFirst() + box_rows - 1;

	return last >= lp_now - 1 ? lp_now - 1 : last; /* min(lp_now - 1, last) */
}

/* Go to document top
   ------------------
*/
LoopTop()
{
	int first;

	first = LoopFirst();

	lp_cur = box_shr = box_shc = 0;

	if(first > 0)
		RefreshAll();
}

/* Go to document bottom
   ---------------------
*/
LoopBottom()
{
	int first, last;

	first = LoopFirst();
	last = LoopLast();

	lp_cur = lp_now - 1; box_shc = 32000;

	if(last < lp_now - 1)
	{
		box_shr = box_rows - 1;
		RefreshAll();
	}
	else
		box_shr = last - first;
}

/* Page up
   -------
*/
LoopPgUp()
{
	int first, to;

	first = LoopFirst();

	if(first)
	{
		if((to = first - box_rows) < 0) /* max(to, 0) */
			to = 0;

		lp_cur = to; box_shr = box_shc = 0;

		RefreshAll();
	}
	else
		LoopTop();
}

/* Page down
   ---------
*/
LoopPgDown()
{
	int first, last, to;

	first = LoopFirst();
	last = LoopLast();

	if(last < lp_now - 1)
	{
		to = first + box_rows;

		if(to >= lp_now)
			to = lp_now - 1;

		lp_cur = to; box_shr = box_shc = 0;

		RefreshAll();
	}
	else
		LoopBottom();
}

/* Print filename
   --------------
*/
LoopFileName()
{
	int i;

	CrtLocate(PS_ROW, PS_FNAME);

	for(i = FILENAME_MAX - 1; i; --i)
		putchr(' ');

	CrtLocate(PS_ROW, PS_FNAME);

	putstr(CurrentFile());
}

/* Insert CR (intro)
   -----------------
*/
LoopInsert()
{
	int left_len, right_len, i;
	char *p1, *p2;

	left_len = box_shc;
	right_len = strlen(ln_dat) - box_shc;

	p1 = malloc(left_len + 1);
	p2 = malloc(right_len + 1);

	//if(p1 == NULL || p2 == NULL)
	if(!p1 || !p2)
	{
		ErrLineMem();

		//if(p1 != NULL)
		if(p1)
			free(p1);
		//if(p2 != NULL)
		if(p2)
			free(p2);

		return;
	}

	strcpy(p2, ln_dat + box_shc);

	ln_dat[box_shc] = 0;

	strcpy(p1, ln_dat);

	for(i = lp_now; i > lp_cur; --i)
		lp_arr[i] = lp_arr[i - 1];

	++lp_now;

	free(lp_arr[lp_cur]);

	lp_arr[lp_cur++] = p1;
	lp_arr[lp_cur] = p2;

	CrtClearEol();

	if(box_shr < box_rows - 1)
		Refresh(++box_shr, lp_cur);
	else
		Refresh(0, lp_cur - box_rows + 1);

	box_shc = 0;
}

/* Delete CR on the left
   ---------------------
*/
LoopLeftDel()
{
	int len_up, len_cur, i;
	char *p;

	len_up = strlen(lp_arr[lp_cur - 1]);
	len_cur = strlen(ln_dat);

	if(len_up + len_cur > ln_max)
	{
		/**ErrLineLong();**/ return;
	}

	//if((p = malloc(len_up + len_cur + 1)) == NULL)
	if(!(p = malloc(len_up + len_cur + 1)))
	{
		ErrLineMem(); return;
	}

	strcpy(p, lp_arr[lp_cur - 1]); strcat(p, ln_dat);

	--lp_now;

	free(lp_arr[lp_cur]);
	free(lp_arr[lp_cur - 1]);

	for(i = lp_cur; i < lp_now; ++i)
		lp_arr[i] = lp_arr[i + 1];

	/* *** Following code can cause collateral effects on some compilers ***
	i = lp_cur;

	while(i < lp_now)
		lp_arr[i] = lp_arr[++i];
	************************************************************************/

	lp_arr[lp_now] = NULL;

	lp_arr[--lp_cur] = p;

	box_shc = len_up;

	if(box_shr)
	{
		CrtLocate(BOX_ROW + --box_shr, box_shc); putstr(lp_arr[lp_cur] + box_shc);

		Refresh(box_shr + 1, lp_cur + 1);
	}
	else
		Refresh(0, lp_cur);
}

/* Delete CR on the right
   ----------------------
*/
LoopRightDel()
{
	int len_dn, len_cur, i;
	char *p;

	len_dn = strlen(lp_arr[lp_cur + 1]);
	len_cur = strlen(ln_dat);

	if(len_dn + len_cur > ln_max)
	{
		/**ErrLineLong();**/ return;
	}

	//if((p = malloc(len_dn + len_cur + 1)) == NULL)
	if(!(p = malloc(len_dn + len_cur + 1)))
	{
		ErrLineMem(); return;
	}

	strcpy(p, ln_dat); strcat(p, lp_arr[lp_cur + 1]);

	--lp_now;

	free(lp_arr[lp_cur]);
	free(lp_arr[lp_cur + 1]);

	for(i = lp_cur + 1; i < lp_now; ++i)
		lp_arr[i] = lp_arr[i + 1];

	/* *** Following code can cause collateral effects on some compilers ***
	i = lp_cur + 1;

	while(i < lp_now)
		lp_arr[i] = lp_arr[++i];
	************************************************************************/

	lp_arr[lp_now] = NULL;

	lp_arr[lp_cur] = p;

	putstr(p + box_shc);

	if(box_shr < box_rows - 1)
		Refresh(box_shr + 1, lp_cur + 1);
}

#ifdef K_FIND

/* Find string
   -----------
*/
LoopFind()
{
	int slen, flen, line, row, col, i;
	char *p;

	row  = box_shr;
	col  = box_shc;
	flen = strlen(find_str);

	for(line = lp_cur; line < lp_now; ++line)
	{
		for(slen = strlen((p = lp_arr[line])); flen <= (slen - col) && col < slen; ++col)
		{
			for(i = 0; i < flen && find_str[i] == p[col + i]; ++i)
				;

			if(i == flen)
			{
				/* Found, set new cursor position and refresh the screen if needed */

				lp_cur = line;
				box_shc = col;

				if(row < box_rows)
					box_shr = row;
				else
					Refresh((box_shr = 0), lp_cur);

				return 1;
			}
		}

		++row; col = 0;
	}

	/* Not found */

	return 0;
}

/* Find first string
   -----------------
*/
LoopFindFirst()
{
	find_str[0] = '\0';

	if(SysLineStr("Find string", find_str, FIND_MAX - 1))
	{
		LoopFind();
	}
}

/* Find next string
   ----------------
*/
LoopFindNext()
{
	int old_box_shc;

	if(find_str[0])
	{
		old_box_shc = box_shc;

		/* Skip current character */

		if(box_shc < strlen(lp_arr[lp_cur]))
			++box_shc;

		/* Set old cursor position on find failure */

		if(!LoopFind())
			box_shc = old_box_shc;
	}
}

#endif

#ifdef K_GOTO

/* Go to line #
   ------------
*/
LoopGoLine()
{
	char buf[6];
	int line, first, last;

	buf[0] = '\0';

	if(SysLineStr("Go to line #", buf, 5))
	{
		line = atoi(buf);

		if(line > 0 && line <= lp_now)
		{
			first = LoopFirst();
			last = LoopLast();

			lp_cur = line - 1;
			box_shc = 0;

			if(lp_cur >= first && lp_cur <= last)
				box_shr = lp_cur - first;
			else
				Refresh((box_shr = 0), lp_cur);
		}
	}
}

#endif

#ifdef K_MACRO

/* Execute macro from file
   -----------------------
*/
LoopMacro()
{
	char fn[FILENAME_MAX];

	fn[0] = '\0';

	if(SysLineStr("Macro filename", fn, FILENAME_MAX - 1))
	{
		MacroRunFile(fn);
	}
}

#endif

