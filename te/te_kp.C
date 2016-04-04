/*	te_kp.c

	Text editor.

	CRT module for the Kaypro II, 4, and 10 (and any other terminal
	emulating the ADM-3A)

	Adapted by Stephen S. Mitchell from a version for the Amstrad PCW
        written by Miguel Garcia / Floppy Software.

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

	Original code: July 4, 2015.

*/

/* Definitions
   -----------
*/
#define CRT_NAME "Kaypro II, 4, and 10"

#define CRT_ROWS 24        /* CRT rows */
#define CRT_COLS 80        /* CRT columns */

#define RULER_TAB     '!'  /* Ruler: Tab stop character */
#define RULER_CHR     '.'  /* Ruler: Character */
#define SYS_LINE_SEP  '-'  /* System line separator character */

#define CRT_ENGLISH

/* Keys
   ----
*/
#define K_UP	 11 /* Ctl K */
#define K_DOWN	 10 /* Ctl J */
#define K_LEFT	 8  /* Ctl H */
#define K_RIGHT	 12 /* Ctl L */

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

#define CRT_ESC_KEY "ESC"

#define H_0 "Up     ^K [UP]     Left   ^H [LEFT]"
#define H_1 "Down   ^J [DOWN]   Right  ^L [RIGHT]"
#define H_2 "Begin  ^V          LtDel   [DEL]"
#define H_3 "End    ^\\          RtDel  ^G"
#define H_4 "Top    ^P          PgUp   ^Q"
#define H_5 "Bottom ^S          PgDown ^Z"
#define H_6 ""
#define H_7 "Cut    ^U"           
#define H_8 "Copy   ^R          Intro  ^M [RETURN]"
#define H_9 "Paste  ^W          Esc    [ESC]"



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
	CrtOut(26);
}

/* Locate the cursor (HOME is 0,0)
   -------------------------------
   void CrtLocate(int row, int col)
*/
CrtLocate(row, col)
int row, col;
{
	CrtOut(27); CrtOut('='); CrtOut(row + 32); CrtOut(col + 32);
}

/* Erase line and cursor to row,0
   ------------------------------
   void CrtClearLine(int row)
*/
CrtClearLine(row)
int row;
{
	CrtLocate(row, 0); CrtOut(24); 
}

/* Erase from the cursor to the end of the line
   --------------------------------------------
*/
CrtClearEol()
{
	CrtOut(24); 
}

