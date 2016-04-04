/*	te_ws100.c

	Text editor.

	CRT module for VT100 & WordStar keys, under CP/M.

	Copyright (c) 2015 Miguel Garcia / FloppySoftware

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

	12 May 2015 : 1st version.
	14 May 2015 : Completed adaptation for WS keys.
*/

/* Definitions
   -----------
*/
#define CRT_NAME "VT100 & WordStar keys"

#define CRT_ROWS 25       /* CRT rows */
#define CRT_COLS 80       /* CRT columns */

#define RULER_TAB    '!'  /* Ruler: Tab stop character - ie: ! */
#define RULER_CHR    '.'  /* Ruler: Character - ie: . */
#define SYS_LINE_SEP '-'  /* System line separator character - ie: - */

#define CRT_ESC_KEY "ESC" /* Escape key name */

/* Keys
   ----
*/
#define K_UP	 5  /* Ctl E -- WS */
#define K_DOWN	 24 /* Ctl X -- WS */
#define K_LEFT	 19 /* Ctl S -- WS */
#define K_RIGHT	 4  /* Ctl D -- WS */

#define K_PGUP	 3  /* Ctl C -- WS */
#define K_PGDOWN 18 /* Ctl R -- WS */

#define K_BEGIN	 2  /* Ctl B -- translated from WS */
#define K_END	 11 /* Ctl K -- translated from WS */

#define K_TOP    12 /* Ctl L -- translated from WS */
#define K_BOTTOM 16 /* Ctl P -- translated from WS */

#define K_TAB    9  /* Ctl I -- WS */

#define K_INTRO	 13 /* Ctl M -- WS */
#define K_ESC	 27 /* Ctl [ */

#define K_RDEL	 7   /* Ctl G -- WS */
#define K_LDEL   127 /* 0x7F  -- WS */

#define K_CUT    25 /* Ctl Y -- WS */
#define K_COPY   15 /* Crl O */
#define K_PASTE  23 /* Ctl W */

#define K_LWORD  1  /* Ctl A -- WS */
#define K_RWORD  6  /* Ctl F -- WS */

/* Help
   ----
*/
#define H_0 "Up     ^E   Left   ^S"
#define H_1 "Down   ^X   Right  ^D"
#define H_2 "Begin  ^QS  LtDel  7F / ^H"
#define H_3 "End    ^QD  RtDel  ^G"
#define H_4 "Top    ^QR  PgUp   ^C  Left  Word ^A"
#define H_5 "Bottom ^QC  PgDown ^R  Right Word ^F"
#define H_6 ""
#define H_7 "Cut    ^Y   Tab   ^I"
#define H_8 "Copy   ^O   Intro ^M"
#define H_9 "Paste  ^W   Esc   ^["

/* Setup CRT: Used when the editor starts
   --------------------------------------
   void CrtSetup(void)
*/
CrtSetup()
{
}

/* Reset CRT: Used when the editor exits
   -------------------------------------
   void CrtReset(void)
*/
CrtReset()
{
}

/* Output character to the CRT
   ---------------------------
   All program output is done with this function.

   On '\n' outputs '\n' + '\r'.

   void CrtOut(int ch)
*/
#asm
CrtOut:
	ld a,l
	cp 10
	jr nz,CrtOut2
	call CrtOut2
	ld l,13
CrtOut2:
	ld c,l
	ld hl,(1)
	ld de,9
	add hl,de
	jp (hl)
#endasm

/* Input character from the keyboard
   ---------------------------------
   All program input is done with this function.

   May be used to translate key codes.

   int CrtIn(void)
*/

CrtIn()
{
	int ch;

	/* Get character from keyboard */

	ch = CrtInEx();

	/* Make Ctrl H (backspace) equivalent to DEL */

	if(ch == 8) /* Ctrl H */
		return K_LDEL;

	/* Translate some Ctrl Q + ? commands */

	if(ch == 17) /* Ctrl Q */
	{
		ch = CrtInEx();

		switch(ch)
		{
			case 'r' :
			case 'R' :
				return K_TOP;
			case 'c' :
			case 'C' :
				return K_BOTTOM;
			case 's' :
			case 'S' :
				return K_BEGIN;
			case 'd' :
			case 'D' :
				return K_END;
		}
	}

	/* Return character */

	return ch;
}

#asm
CrtInEx:
	ld hl,(1)
	ld de,6
	add hl,de
	ld de,CrtIn2
	push de
	jp (hl)
CrtIn2:
	ld h,0
	ld l,a
	ret
#endasm

/* Clear screen and send cursor to 0,0
   -----------------------------------
   void CrtClear(void)
*/
CrtClear()
{
	CrtOut(27); putstr("[1;1H"); /* Cursor to 0,0 */
	CrtOut(27); putstr("[2J");   /* Clear CRT */
}

/* Locate the cursor (HOME is 0,0)
   -------------------------------
   void CrtLocate(int row, int col)
*/
CrtLocate(row, col)
int row, col;
{
	CrtOut(27); CrtOut('[');
	putint("%d", row + 1); CrtOut(';');
	putint("%d", col + 1); CrtOut('H');
}

/* Erase line and cursor to row,0
   ------------------------------
   void CrtClearLine(int row)
*/
CrtClearLine(row)
int row;
{
	CrtLocate(row, 0); CrtOut(27); putstr("[K");
}

/* Erase from the cursor to the end of the line
   --------------------------------------------
*/
CrtClearEol()
{
	CrtOut(27); putstr("[K");
}


