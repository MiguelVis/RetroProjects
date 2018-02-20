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
		else
		{
			MacroStop();
		}
	}
	/*
	else if(mac_str)
	{
		if(*mac_str)
		{
			return *mac_str++;
		}
		else
		{
			MacroStop();
		}
	}
	*/

	/* No character available */
	return '\0';
}

/* Read coocked character for macro input
   --------------------------------------
*/
MacroGetCh()
{
	int i, ch;
	char sym[MAC_SYM_MAX];

	/* Continue if there is a character available */
	if((ch = MacroGetRaw()))
	{

		/* Check for escaped characters */
		if(ch == MAC_ESCAPE)
		{
			if((ch = MacroGetRaw()))
			{
				return ch;
			}

			/* Error: bad escaped character */
			MacroStop();

			return MAC_ERROR;
		}

		/* Return character if it's not the start of a symbol */
		if(ch != MAC_START)
		{
			return ch;
		}

		/* Get symbol name like {name} --> "name" */

		/*
		for(i = 0; i < MAC_SYM_MAX - 1; ++i)
		{
			ch = MacroGetRaw();

			if(ch && ch != MAC_END)
			{
				sym[i] = tolower(ch);
			}
			else
			{
				break;
			}
		}
		*/

		for(i = 0; isalpha(ch = MacroGetRaw()) && i < MAC_SYM_MAX - 1; ++i)
		{
			sym[i] = tolower(ch);
		}

		if(i && ch == MAC_END)
		{
			/* End of string */
			sym[i] = '\0';

			/* Find symbol name and return its associated character */
			if     (!strcmp(sym, "up"))     return K_UP;
			else if(!strcmp(sym, "down"))   return K_DOWN;
			else if(!strcmp(sym, "left"))   return K_LEFT;
			else if(!strcmp(sym, "right"))  return K_RIGHT;
			else if(!strcmp(sym, "begin"))  return K_BEGIN;
			else if(!strcmp(sym, "end"))    return K_END;
			else if(!strcmp(sym, "top"))    return K_TOP;
			else if(!strcmp(sym, "bottom")) return K_BOTTOM;
			else if(!strcmp(sym, "intro"))  return K_INTRO;
			else if(!strcmp(sym, "tab"))    return K_TAB;
			else if(!strcmp(sym, "cut"))    return K_CUT;
			else if(!strcmp(sym, "copy"))   return K_COPY;
			else if(!strcmp(sym, "paste"))  return K_PASTE;
			else if(!strcmp(sym, "filename"))
			{
				ForceStr(CurrentFile());

				return '\0';
			}
		}

		/* Error: symbol name not found, too large, or EOF */
		MacroStop();

		return MAC_ERROR;
	}

	/* No character available */
	return '\0';
}

