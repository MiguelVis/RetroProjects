/*	te_macro.c

	Text editor.

	Macros.

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
	20 Feb 2018 : Disable code for macros from strings, for now.
	26 Dec 2018 : Allow # of repeats in macros - ie {up:12}. Rename MacroGetCh() to MacroGet().
*/

/* Run a macro from file
   ---------------------
*/
MacroRunFile(fname)
char *fname;
{
	if(!(mac_fp = fopen(fname, "r")))
	{
		ErrLineOpen();
	}
}

/* Run a macro from string
   -----------------------
*/
/*
MacroRunStr(s)
char *s;
{
	mac_str = s;
}
*/

/* Stop a macro
   ------------
*/
MacroStop()
{
	if(mac_fp)
	{
		fclose(mac_fp);
	}

	mac_fp = /*mac_str =*/ NULL;
}

/* Read raw character from macro input
   -----------------------------------
*/
MacroGetRaw()
{
	int ch;

	if(mac_fp)
	{
		while((ch = fgetc(mac_fp)) == '\n')
			;

		if(ch != EOF)
		{
			return ch;
		}

		MacroStop();
	}
	/*
	else if(mac_str)
	{
		if(*mac_str)
		{
			return *mac_str++;
		}

		MacroStop();
	}
	*/

	/* No character available */
	return '\0';
}

/* Process a macro input unit
   --------------------------
*/
MacroGet()
{
	int i, n, ch;
	char sym[MAC_SYM_MAX];

	/* Continue if there is a character available */
	if((ch = MacroGetRaw()))
	{

		/* Check for escaped characters */
		if(ch == MAC_ESCAPE)
		{
			if((ch = MacroGetRaw()))
			{
				ForceCh(ch);
			}
			else
			{
				/* Error: missing escaped character */
				ForceCh(MAC_ERROR);
				
				MacroStop();
			}

			return;
		}

		/* Return character if it's not the start of a symbol */
		if(ch != MAC_START)
		{
			ForceCh(ch);
			return;
		}

		/* Get symbol name like {up} or {up:12} --> "up" */
		for(i = 0; isalpha(ch = MacroGetRaw()) && i < MAC_SYM_MAX - 1; ++i)
		{
			sym[i] = tolower(ch);
		}

		if(i)
		{
			/* End of symbol name */
			sym[i] = '\0';

			/* Get # of repeats if any - ie: {up:12} --> 12 */
			if(ch == MAC_SEP)
			{
				n = 0;

				while(isdigit(ch = MacroGetRaw()))
					n = n * 10 + ch - '0';

				if(n < 0 || n > FORCED_MAX)
				{
					n = 0;
				}
			}
			else
			{
				n = 1;
			}

			if(n && ch == MAC_END)
			{
				/* Do command action */
				ch = 0;

				if     (!strcmp(sym, "up"))     ch = K_UP;
				else if(!strcmp(sym, "down"))   ch = K_DOWN;
				else if(!strcmp(sym, "left"))   ch = K_LEFT;
				else if(!strcmp(sym, "right"))  ch = K_RIGHT;
				else if(!strcmp(sym, "begin"))  ch = K_BEGIN;
				else if(!strcmp(sym, "end"))    ch = K_END;
				else if(!strcmp(sym, "top"))    ch = K_TOP;
				else if(!strcmp(sym, "bottom")) ch = K_BOTTOM;
				else if(!strcmp(sym, "intro"))  ch = K_INTRO;
				else if(!strcmp(sym, "tab"))    ch = K_TAB;
				else if(!strcmp(sym, "cut"))    ch = K_CUT;
				else if(!strcmp(sym, "copy"))   ch = K_COPY;
				else if(!strcmp(sym, "paste"))  ch = K_PASTE;

				if(ch)
				{
					while(n--)
					{
						if(ForceCh(ch))
							break;
					}

					return;
				}

				/* Special commands */
				if(!strcmp(sym, "filename"))
				{
					while(n--)
						ForceStr(CurrentFile());

					return;
				}
			}
		}

		/* Error: symbol name not found, bad formed, too large, bad # of repeats */
		MacroStop();

		ForceCh(MAC_ERROR);
		return;
	}
}

