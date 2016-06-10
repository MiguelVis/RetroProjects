/*	te.h

	Text editor.

	Defs. for main module.

	Copyright (c) 2015-2016 Miguel Garcia / FloppySoftware

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

	04 May 2015 : 1st version.
	12 May 2015 : v1.01
	14 May 2015 : v1.02
	15 May 2015 : v1.03
	31 Aug 2015 : v1.04
	02 Jun 2016 : v1.05 : CRT_FILE, CRT_ENGLISH, CRT_SPANISH, etc. are now out of this file.
	10 Jun 2016 : v1.06 : Removed BOX_COL. Added PS_FNAME, PS_INF, PS_LIN_CUR, PS_LIN_NOW,
	                      PS_LIN_MAX, PS_COL_CUR, PS_COL_NOW, PS_COL_MAX.

	Notes:

	Change TE_VERSION as required, before compilation.
*/

/* Version
   -------
*/

#define TE_VERSION "v1.06 / 10 Jun 2016 for CP/M"  /* Program version and date */

/* More defs.
   ----------
*/

#define MAX_LINES 512    /* Max. # of text lines: each empty line uses 2 bytes with the Z80 */

#define TAB_COLS 8       /* How many columns has a tab. (usually 8) */

#define FORCED_MAX 128   /* Keyboard forced entry buffer size (for paste, tabs, etc.) */

#define PS_ROW     0     /* Information position */
#define PS_FNAME   4     /* Filename - position in row */
#define PS_TXT     "Lin:0000/0000/0000 Col:00/00 Len:00"  /* Information layout */
#define PS_INF     (CRT_COLS - 35)  /* Information layout - position in row */
#define PS_LIN_CUR (CRT_COLS - 31)  /* Current line # - position in row */
#define PS_LIN_NOW (CRT_COLS - 26)  /* How many lines # - position in row */
#define PS_LIN_MAX (CRT_COLS - 21)  /* Max. # of lines - position in row */
#define PS_COL_CUR (CRT_COLS - 12)  /* Current column # - position in row */
#define PS_COL_NOW (CRT_COLS -  2)  /* Line length - position in row */
#define PS_COL_MAX (CRT_COLS - 9)   /* Max. line length - position in row */

#define BOX_ROW 2        /* Editor box position */

#define getchr CrtIn     /* Get a character from the keyboard */
#define putchr CrtOut    /* Print a character on screen */

