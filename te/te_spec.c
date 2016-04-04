/*	te_spec.c

	Text editor.

	CRT module for the Spectrum +3.

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

	06 May 2015 : 1st version.
	31 Aug 2015 : Minor changes in comments.

	Notes:

	The Spectrum +3 runs CP/M Plus (v3.1) with a VT52-like emulation,
	and a 24x51 CRT minus 1 line for system messages (it can be enabled
	or disabled).

	It supports 24x32 and 24x80 (in two pages) CRT modes also.
*/

/* Definitions
   -----------
*/
#define CRT_NAME "Spectrum +3 & CP/M 3"

#define CRT_ROWS 23        /* CRT rows: 24 - 1 (system line) */
#define CRT_COLS 51        /* CRT columns */

#define RULER_TAB     0x91 /* Ruler: Tab stop character - ie: ! */
#define RULER_CHR     0x90 /* Ruler: Character - ie: . */
#define SYS_LINE_SEP  0x9A /* System line separator character - ie: - */

/* Keys
   ----
*/

#define CRT_ESC_KEY "BREAK" /* ESCAPE key name */

#define K_UP	 31 /* Ctl _ */
#define K_DOWN	 30 /* Ctl ^ */
#define K_LEFT	 1  /* Ctl A */
#define K_RIGHT	 6  /* Ctl F */

#define K_PGUP	 17 /* Ctl Q */
#define K_PGDOWN 26 /* Ctl Z */

#define K_BEGIN	 22 /* Ctl V */
#define K_END	 28 /* Ctl \ */

#define K_TOP    16 /* Ctl P */
#define K_BOTTOM 19 /* Ctl S */

#define K_TAB    9  /* Ctl I */

#define K_INTRO	 13 /* Ctl M */
#define K_ESC	 27 /* Ctl [ */

#define K_RDEL	 7  /* Ctl G */
#define K_LDEL   127

#define K_CUT    21 /* Ctl U */
#define K_COPY   18 /* Crl R */
#define K_PASTE  23 /* Ctl W */

/* Help
   ----
*/

#define H_0 "Up     ^_ [UP]     Left   ^A [LEFT]"
#define H_1 "Down   ^^ [DOWN]   Right  ^F [RIGHT]"
#define H_2 "Begin  ^V          LtDel  7F [DEL]"
#define H_3 "End    ^\\          RtDel  ^G [GRAPH]"
#define H_4 "Top    ^P          PgUp   ^Q"
#define H_5 "Bottom ^S          PgDown ^Z"
#define H_6 ""
#define H_7 "Cut    ^U"           
#define H_8 "Copy   ^R          Intro  ^M [ENTER]"
#define H_9 "Paste  ^W          Esc    ^[ [BREAK]"

/* Setup CRT: Used when the editor starts
   --------------------------------------
   void CrtSetup(void)
*/
CrtSetup()
{
	CrtOut(27); CrtOut('3'); CrtOut(1); /* Set 24 x 51 mode */
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

   May be used to translate IBM PC key codes into single characters.

   int CrtIn(void)
*/
#asm
CrtIn:
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
	CrtOut(27); CrtOut('H'); /* Cursor to 0,0 */
	CrtOut(27); CrtOut('E'); /* Clear CRT */
}

/* Locate the cursor (HOME is 0,0)
   -------------------------------
   void CrtLocate(int row, int col)
*/
CrtLocate(row, col)
int row, col;
{
	CrtOut(27); CrtOut('Y'); CrtOut(row + 32); CrtOut(col + 32);
}

/* Erase line and cursor to row,0
   ------------------------------
   void CrtClearLine(int row)
*/
CrtClearLine(row)
int row;
{
	CrtLocate(row, 0); CrtOut(27); CrtOut('K');
}

/* Erase from the cursor to the end of the line
   --------------------------------------------
*/
CrtClearEol()
{
	CrtOut(27); CrtOut('K');
}

