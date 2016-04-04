/*	te_tak.c

	Text editor.

	CRT module for the Takeda Toshiya's CP/M emulator.

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

	Notes:

	For CPM.EXE / CP/M Player for Win32 console from Takeda Toshiya.

	It emulates a 25x80 VT-100.

	It needs to translate the IBM PC keyboard codes.
*/

/* Definitions
   -----------
*/
#define CRT_NAME "Takeda Toshiya's CP/M emulator"

#define CRT_ROWS 25       /* CRT rows */
#define CRT_COLS 80       /* CRT columns */

#define RULER_TAB    '!'  /* Ruler: Tab stop character - ie: ! */
#define RULER_CHR    '.'  /* Ruler: Character - ie: . */
#define SYS_LINE_SEP '-'  /* System line separator character - ie: - */

#define CRT_ESC_KEY "ESC" /* Escape key name */

/* Keys
   ----
*/
#define K_UP	 5  /* Ctl E */
#define K_DOWN	 24 /* Ctl X */
#define K_LEFT	 19 /* Ctl S */
#define K_RIGHT	 4  /* Ctl D */

#define K_PGUP	 18 /* Ctl R */
#define K_PGDOWN 3  /* Ctl C */

#define K_BEGIN	 2  /* Ctl B */
#define K_END	 1  /* Ctl A */

#define K_TOP    16 /* Ctl P */
#define K_BOTTOM 6  /* Ctl F */

#define K_TAB    9  /* Ctl I */

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
#define H_0 "Up     ^E [UP]        Left   ^S [LEFT]"
#define H_1 "Down   ^X [DOWN]      Right  ^D [RIGHT]"
#define H_2 "Begin  ^B [HOME]      LtDel  ^H [BACKSPACE]"
#define H_3 "End    ^A [END]       RtDel  7F [DELETE]"
#define H_4 "Top    ^P [CTRL+HOME] PgUp   ^R [PGUP]"
#define H_5 "Bottom ^F [CTRL+END]  PgDown ^C [PGDN]"
#define H_6 ""
#define H_7 "Cut    ^U [F1]"           
#define H_8 "Copy   ^O [F2]        Intro  ^M [ENTER]"
#define H_9 "Paste  ^W [F3]        Esc    ^[ [ESC]"

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

   Translates the IBM PC key codes into single characters.

   int CrtIn(void)
*/
CrtIn()
{
	int ch;

	ch = xCrtIn();

	/* Translate key codes begining with 0 or 224 */

	if(!ch || ch == 224)
	{
		ch = xCrtIn();

		switch(ch)
		{
			case 72  : /* UP */
				return 5;
			case 80  : /* DOWN */
				return 24;
			case 75  : /* LEFT */
				return 19;
			case 77  : /* RIGHT */
				return 4;
			case 71  : /* HOME - INICIO */
				return 2;
			case 79  : /* END */
				return 1;
			case 73  : /* PGUP */
				return 18;
			case 81  : /* PGDN */
				return 3;
			case 83  : /* DELETE - SUPR */
				return 127;
			case 119 : /* CTL HOME */
				return 16;
			case 117 : /* CTL END */
				return 6;
			case 59  : /* F1 */
				return 21;
			case 60  : /* F2 */
				return 15;
			case 61  : /* F3 */
				return 23;
			default  :
				return 0;
		}
	}

	return ch;
}

#asm
xCrtIn:
	ld hl,(1)
	ld de,6
	add hl,de
	ld de,xCrtIn2
	push de
	jp (hl)
xCrtIn2:
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

