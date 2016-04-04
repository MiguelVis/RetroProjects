/*	te_mur.c

	Text editor.

	CRT module for the K. Murakami's CP/M emulator.

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

	03 May 2015 : 1st version.

	Notes:

	For CPM.EXE / CP/M-80 program EXEcutor for Win32 v0.4 from K. Murakami.

	It emulates a 25x80 VT-100.

	It translates the PC scan codes to singlecharacters.
*/

/* Definitions
   -----------
*/
#define CRT_NAME "K. Murakami's CP/M emulator"

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

#define K_PGUP	 18 /* Ctl R */
#define K_PGDOWN 3  /* Ctl C */

#define K_BEGIN	 2  /* Ctl B */
#define K_END	 1  /* Ctl A */

#define K_TOP    16 /* Ctl P */
#define K_BOTTOM 6  /* Ctl F */

#define K_TAB    9  /* Ctl I -- WS */

#define K_INTRO	 13 /* Ctl M */
#define K_ESC	 27 /* Ctl [ */

#define K_RDEL	 127
#define K_LDEL   8  /* Ctl H */

#define K_CUT    21 /* Ctl U */
#define K_COPY   15 /* Crl O */
#define K_PASTE  23 /* Ctl W */

/* Help
   ----
*/
#define H_0 "Up     ^E [UP]    Left   ^S [LEFT]"
#define H_1 "Down   ^X [DOWN]  Right  ^D [RIGHT]"
#define H_2 "Begin  ^B         LtDel  ^H [<-DEL]"
#define H_3 "End    ^A         RtDel  7F [DEL->]"
#define H_4 "Top    ^P         PgUp   ^R [PG UP]"
#define H_5 "Bottom ^F         PgDown ^C [PG DOWN]"
#define H_6 ""
#define H_7 "Cut    ^U"           
#define H_8 "Copy   ^O         Intro  ^M [RETURN]"
#define H_9 "Paste  ^W         Esc    ^[ [EXIT]"

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


