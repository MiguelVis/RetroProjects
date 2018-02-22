/*	te_macro.c

	Text editor.

	File I/O.

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
	22 Feb 2018 : Check for buffer changes.
*/

/* Reset lines array
   -----------------
*/
ResetLines()
{
	int i;

	for(i = 0; i < MAX_LINES; ++i)
	{
		//if(lp_arr[i] != NULL)
		if(lp_arr[i])
		{
			free(lp_arr[i]); lp_arr[i] = NULL;
		}
	}

	lp_cur = lp_now = lp_chg = box_shr = box_shc = 0;
}

/* New file
   --------
*/
NewFile()
{
	char *p;

	/* Free current contents */

	ResetLines();

	file_name[0] = 0;

	/* Build first line: Just one byte, please! */

	p = malloc(1); *p = 0; lp_arr[lp_now++] = p;
}

/* Read text file
   --------------
   Returns NZ on error.
*/
ReadFile(fn)
char *fn;
{
	FILE *fp;
	int ch, code, len, i, tabs;
	char *p;

	/* Free current contents */

	ResetLines();

	/* Setup some things */

	code = 0;

	/* Open the file */

	SysLine("Reading file.");

	//if((fp = fopen(fn, "r")) == NULL)
	if(!(fp = fopen(fn, "r")))
	{
		ErrLineOpen(); return -1;
	}

	/* Read the file */

	for(i = 0; i < 32000; ++i)
	{
		//if(fgets(ln_dat, ln_max + 2, fp) == NULL) /* ln_max + CR + ZERO */
		if(!fgets(ln_dat, ln_max + 2, fp)) /* ln_max + CR + ZERO */
			break;

		if(i == MAX_LINES)
		{
			ErrLineTooMany(); ++code; break;
		}

		len = strlen(ln_dat);

		if(ln_dat[len - 1] == '\n')
			ln_dat[--len] = 0;
		else if(len > ln_max)
		{
			ErrLineLong(); ++code; break;
		}

		//if((lp_arr[i] = malloc(len + 1)) == NULL)
		if(!(lp_arr[i] = malloc(len + 1)))
		{
			ErrLineMem(); ++code; break;
		}

		strcpy(lp_arr[i], ln_dat);
	}

	/* Close the file */

	fclose(fp);

	/* Check errors */

	if(code)
		return -1;

	/* Set readed lines */

	lp_now = i;

	/* Check if empty file */

	if(!lp_now)
	{
		/* Build first line: Just one byte, please! */

		p = malloc(1); *p = 0; lp_arr[lp_now++] = p;
	}

	/* Change TABs to SPACEs, and check characters */

	for(i = tabs = 0; i < lp_now; ++i)
	{
		p = lp_arr[i];

		while((ch = (*p & 0xFF)))
		{
			if(ch < ' ')
			{
				if(ch == '\t')
				{
					*p = ' '; ++tabs;
				}
				else
				{
					ErrLine("Bad character found.");

					return -1;
				}
			}

			++p;
		}
	}

	/* Check TABs */

	if(tabs)
		ErrLine("Tabs changed to spaces.");

	/* Success */

	return 0;
}

/* Backup the previous file with the same name
   -------------------------------------------
   Return NZ on error.
*/
BackupFile(fn)
char *fn;
{
	FILE *fp;
	char *bkp;

	/* Check if file exists */

	//if((fp = fopen(fn, "r")) != NULL)
	if((fp = fopen(fn, "r")))
	{
		fclose(fp);

		bkp = "te.bkp";

		/* Remove the previous backup file */

		remove(bkp);

		/* Rename the old file as backup */

		rename(fn, bkp);
	}
}

/* Write text file
   ---------------
   Returns NZ on error.
*/
WriteFile(fn)
char *fn;
{
	FILE *fp;
	int i, err;
	char *p;

	/* Do backup of old file */

	SysLine("Writing file.");

	BackupFile(fn);

	/* Open the file */

	//if((fp = fopen(fn, "w")) == NULL)
	if(!(fp = fopen(fn, "w")))
	{
		ErrLineOpen(); return -1;
	}

	/* Write the file */

	for(i = err = 0; i < lp_now; ++i)
	{
		p = lp_arr[i];

		/* FIX-ME: We don't have fputs() yet! */

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

		if(err)
		{
			fclose(fp); remove(fn);

			ErrLine("Can't write.");

			return -1;
		}
	}

	/* Close the file */

	if(fclose(fp) == EOF)
	{
		remove(fn);

		ErrLine("Can't close.");

		return -1;
	}

	/* Success */

	return (lp_chg = 0);
}

