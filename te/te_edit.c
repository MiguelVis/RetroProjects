/*	te_macro.c

	Text editor.

	Edit line.

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

	Changes:

	30 Jan 2018 : Extracted from te.c.
	04 Feb 2018 : Support for macros. Go to line #.
	22 Feb 2018 : Check for buffer changes.
*/

/* Edit current line
   -----------------
   Returns last character entered.
*/
BfEdit()
{
	int i, ch, len, run, upd_lin, upd_col, upd_now, upd_cur, spc, old_len;
	char *buf;

	/* Get current line contents */

	strcpy(ln_dat, lp_arr[lp_cur]);

	/* Setup some things */

	len = old_len = strlen(ln_dat);

	run = upd_col = upd_now = upd_cur = 1; upd_lin = spc = 0;

	/* Adjust column position */

	if(box_shc > len)
		box_shc = len;

	/* Loop */

	while(run)
	{
		/* Print line? */

		if(upd_lin)
		{
			upd_lin = 0;

		/* ************************************
			for(i = box_shc; i < len; ++i)
				putchr(ln_dat[i]);
		   ************************************ */

			putstr(ln_dat + box_shc);

			/* Print a space? */

			if(spc)
			{
				putchr(' '); spc = 0;
			}
		}

		/* Print length? */

		if(upd_now)
		{
			upd_now = 0;

			CrtLocate(PS_ROW, PS_COL_NOW); putint("%02d", len);
		}

		/* Print column #? */

		if(upd_col)
		{
			upd_col = 0;

			CrtLocate(PS_ROW, PS_COL_CUR); putint("%02d", box_shc + 1);
		}

		/* Locate cursor? */

		if(upd_cur)
		{
			upd_cur = 0;

			CrtLocate(BOX_ROW + box_shr, box_shc);
		}

		/* Get character: forced entry or keyboard */



		if(!(ch = ForceGetCh()))

		{
			ch = getchr();
		}

		/* Check character and do action */

		/* Note: This function does preliminary checks in some
		   keys for Loop(), to avoid wasted time. */

		switch(ch)
		{
			case K_LEFT :    /* Move one character to the left ----------- */
				if(box_shc)
				{
					--box_shc; ++upd_col;
				}
				else if(lp_cur)
				{
					box_shc = 9999 /* strlen(lp_arr[lp_cur - 1]) */ ;

					ch = K_UP;

					run = 0;
				}
				++upd_cur;
				break;
			case K_RIGHT :   /* Move one character to the right ---------- */
				if(box_shc < len)
				{
					++box_shc; ++upd_col;
				}
				else if(lp_cur < lp_now - 1)
				{
					ch = K_DOWN;

					box_shc = run = 0;
				}
				++upd_cur;
				break;
			case K_LDEL :   /* Delete one character to the left --------- */
				if(box_shc)
				{
					strcpy(ln_dat + box_shc - 1, ln_dat + box_shc);

					--box_shc; --len; ++upd_now; ++upd_lin; ++spc; ++upd_col;

					putchr('\b');
				}
				else if(lp_cur)
					run = 0;
				++upd_cur;
				break;
			case K_RDEL :   /* Delete one character to the right ------- */
				if(box_shc < len)
				{
					strcpy(ln_dat + box_shc, ln_dat + box_shc + 1);

					--len; ++upd_now; ++upd_lin; ++spc;
				}
				else if(lp_cur < lp_now -1)
					run = 0;
				++upd_cur;
				break;
			case K_CUT :    /* Delete the line ------------------------ */
				CrtClearLine(BOX_ROW + box_shr);

				strcpy(ln_clp, ln_dat);

				ln_dat[0] = len = box_shc = 0;

				++upd_now; ++upd_col; ++upd_cur;
				break;
			case K_COPY :   /* Copy the line to the clipboard --------- */
				strcpy(ln_clp, ln_dat);
				++upd_cur;
				break;
			case K_PASTE :  /* Paste the line into current column ----- */
/*******************************************
				if((tmp = strlen(ln_clp)))
				{
					if(len + tmp < ln_max)
					{
						for(i = len; i > box_shc; --i)
							ln_dat[i + tmp - 1] = ln_dat[i - 1];

						for(i = 0; i < tmp; ++i)
							putchr((ln_dat[i + box_shc] = ln_clp[i]));

						len += tmp; box_shc += tmp;

						ln_dat[len] = 0;

						++upd_lin; ++upd_now; ++upd_col;
					}
				}
				++upd_cur;
**********************************************/
				ForceStr(ln_clp);
				++upd_cur;
				break;
			case K_PGUP :   /* Page up ------------------------------ */
			case K_TOP :    /* Document top ------------------------- */
				if(lp_cur || box_shc)
					run = 0;
				++upd_cur;
				break;
			case K_UP :     /* Up one line -------------------------- */
				if(lp_cur)
					run = 0;
				++upd_cur;
				break;
			case K_PGDOWN : /* Page down ---------------------------- */
			case K_BOTTOM : /* Document bottom ---------------------- */
				if(lp_cur < lp_now - 1 || box_shc != len)
					run = 0;
				++upd_cur;
				break;
			case K_DOWN :   /* One line down ------------------------ */
				if(lp_cur < lp_now - 1)
					run = 0;
				++upd_cur;
				break;
			case K_BEGIN :  /* Begin of line ------------------------ */
				if(box_shc)
				{
					box_shc = 0; ++upd_col;
				}
				++upd_cur;
				break;
			case K_END :    /* End of line -------------------------- */
				if(box_shc != len)
				{
					box_shc = len; ++upd_col;
				}
				++upd_cur;
				break;
			case K_ESC :    /* Escape: Show the menu ---------------- */
				run = 0;
				break;
			case K_INTRO :  /* Insert CR (split the line) ----------- */
				if(lp_now < MAX_LINES)
					run = 0;
				break;
			case K_TAB :    /* Insert TAB (spaces) ------------------ */
				i = 8 - box_shc % TAB_COLS;

				while(i--)
				{
					if(ForceCh(' '))
						break;
				}
				break;
#ifdef K_LWORD
			case K_LWORD :  /* Move one word to the left ------------ */

				if(box_shc)
				{
					/* Skip the current word if we are at its begining */

					if(ln_dat[box_shc] != ' ' && ln_dat[box_shc - 1] == ' ')
						--box_shc;

					/* Skip spaces */

					while(box_shc && ln_dat[box_shc] == ' ')
						--box_shc;

					/* Find the beginning of the word */

					while(box_shc && ln_dat[box_shc] != ' ')
					{
						/* Go to the beginning of the word */

						if(ln_dat[--box_shc] == ' ')
						{
							++box_shc; break;
						}
					}

					++upd_col;
				}

				++upd_cur;

				break;
#endif

#ifdef K_RWORD
			case K_RWORD :  /* Move one word to the right ----------- */

				/* Skip current word */

				while(ln_dat[box_shc] && ln_dat[box_shc] != ' ')
					++box_shc;

				/* Skip spaces */

				while(ln_dat[box_shc] == ' ')
					++box_shc;

				++upd_col; ++upd_cur;

				break;
#endif

#ifdef K_FIND
			case K_FIND :   /* Find string -------------------------- */
				run = 0;
				break;

			case K_NEXT :   /* Find next string --------------------- */
				if(find_str[0])
					run = 0;
				break;
#endif

#ifdef K_GOTO
			case K_GOTO :  /* Go to line # -------------------------- */
				run = 0;
				break;
#endif

#ifdef K_MACRO
			case K_MACRO :  /* Execute macro from file -------------- */
				run = 0;
				break;
#endif

			default :       /* Other: Insert character -------------- */
				if(len < ln_max && ch >= ' ')
				{
					putchr(ch);

					for(i = len; i > box_shc; --i)
					{
						ln_dat[i] = ln_dat[i - 1];
					}

			/* *** Following code can cause collateral effects on some compilers ***
					i = len;

					while(i > box_shc)
						ln_dat[i] = ln_dat[--i];
			************************************************************************/

					ln_dat[box_shc++] = ch; ln_dat[++len] = 0;

					++upd_lin; ++upd_now; ++upd_col;
				}
				++upd_cur;
				break;
		}
	}

	/* Update changes if any */

	if(len == old_len)
	{
		/* Check for changes */
		if(memcmp(lp_arr[lp_cur], ln_dat, len))
		{
			/* Update the changes */
			strcpy(lp_arr[lp_cur], ln_dat);

			/* Changes are not saved */
			lp_chg = 1;
		}
	}
	else if((buf = malloc(len + 1)))
	{
		/* Update the changes */
		strcpy(buf, ln_dat);

		/* Free old buffer */
		free(lp_arr[lp_cur]);

		/* Point to new buffer */
		lp_arr[lp_cur] = buf;

		/* Changes are not saved */
		lp_chg = 1;
	}
	else
	{
		ErrLineMem(); /* FIX-ME: Re-print the line with old contents? */
	}

	/* Return last character entered */

	return ch;
}


/* Add a character to forced entry buffer
   --------------------------------------
   Return Z on success, NZ on failure.
*/
ForceCh(ch)
int ch;
{
	if(fe_now < FORCED_MAX)
	{
		++fe_now;

		if(fe_set == FORCED_MAX)
			fe_set = 0;

		fe_dat[fe_set++] = ch;

		return 0;
	}

	return -1;
}

/* Add a string to forced entry buffer
   -----------------------------------
   Return Z on success, NZ on failure.
*/
ForceStr(s)
char *s;
{
	while(*s)
	{
		if(ForceCh(*s++))
			return -1;
	}

	return 0;
}

/* Return character from forced entry buffer
   -----------------------------------------
   Return Z if no characters left.
*/
ForceGetCh()
{
#ifdef K_MACRO
	int ch;
#endif
	if(fe_now)
	{
		--fe_now;

		if(fe_get == FORCED_MAX)
			fe_get = 0;

		return fe_dat[fe_get++];
	}

#ifdef K_MACRO

	if((ch = MacroGetCh()))
	{
		return ch;
	}
	else if(fe_now)
	{
		return ForceGetCh();
	}

#endif

	return '\0';
}

