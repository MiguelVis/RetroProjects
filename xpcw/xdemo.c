/*	xdemo.c

	Demo for the Amstrad PCW graphical and keyboard functions.

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

	To compile with MESCC:

	cc xdemo
	zsm xdemo
	hexcom xdemo
	gencom xdemo.com xpcw.rsx

	Changes:

	10 Aug 2015 : 1st version.
	13 Aug 2015 : Added more text attributes. New PCW computer bitmap.
	17 Aug 2015 : Added use of HelloRsx().
	14 Oct 2018 : Added use of DrawVertLine() and DrawHorzLine();
*/

/* Some defines for MESCC
   ----------------------
*/
#define CC_NO_ARGS  /* No argc and argv[] in main */

/* MESCC libraries
   ---------------
*/
#include "mescc.h"
#include "conio.h"

/* XPCW libraries
   --------------
*/
#include "xpcw.h"
#include "xdraw.h"
#include "xtext.h"
#include "xchrdef.h"
#include "xbitmap.h"
#include "xkeyb.h"

/* Clear the screen, and cursor to 0,0
   -----------------------------------
*/
ClrScr()
{
	putch(27); putch('H');
	putch(27); putch('E');
}

/* Hide the cursor
   ---------------
*/
HideCursor()
{
	putch(27); putch('f');
}

/* Show the cursor
   ---------------
*/
ShowCursor()
{
	putch(27); putch('e');
}

/* Draw a screen bitmap
   --------------------
*/
DrawScrBitmap(row, col, scr_bitmap)
int row, col; BYTE *scr_bitmap;
{
	int rows, cols;

	/* Get the bitmap dimensions */

	cols  = scr_bitmap[0]; /* Columns: 1..90 */
	rows  = scr_bitmap[2]; /* Rows:    1..32 */

	/* Skip data fields */

	scr_bitmap += 4;

	/* Choose a draw bitmap function,
	   depending of the bitmap size in columns */

	if(rows * cols <= X_BUFCOLS)
		DrawSmallBitmap(row, col, rows, cols, scr_bitmap);
	else
		DrawBigBitmap(row, col, rows, cols, scr_bitmap);
}

/* Demo
   ----
*/
main()
{
	int x, y, rad;
	BYTE old_spc[8], new_spc[8];

	/* Check if the RSX is in memory */

	if(!HelloRsx()) {
		puts("The RSX is not in memory!"); return -1;
	}

	/* Reset some values */

	SetFont(NULL);
	SetTextAtt(XA_NORMAL);
	SetDrawMode(XM_SET);

	/* Clear the screen and hide the cursor */

	ClrScr();
	HideCursor();

	/* Draw a border around the screen, excluding the system line: 31 rows * 90 columns */

	DrawBox(0, 0, 720, 248);

	/* Draw two vertical lines to make the border nicer */

	DrawVertLine(  1, 1, 246);  /* Left */
	DrawVertLine(718, 1, 246);  /* Right */

	/* Print title */

	SetTextRC(1, 2); PrintStr("XPCW - Graphical functions for the Amstrad PCW.");

	/* Some text effects */

	SetTextAtt(XA_BOLD);   SetTextRC(3,  2); PrintStr("Bold");
	SetTextAtt(XA_ITALIC); SetTextRC(3,  8); PrintStr("Italic");
	SetTextAtt(XA_ULINE);  SetTextRC(3, 16); PrintStr("Underline");
	SetTextAtt(XA_REV);    SetTextRC(3, 27); PrintStr("Reverse");
	SetTextAtt(XA_LINK);   SetTextRC(3, 36); PrintStr("Link");
	SetTextAtt(XA_GRAY);   SetTextRC(3, 42); PrintStr("Gray");
	SetTextAtt(XA_NORMAL);

	/* Get / set a character definition (the space) */

	GetChrDef(' ', old_spc);

	new_spc[0] = new_spc[1] = new_spc[5] = new_spc[6] = new_spc[7] = 0;
	new_spc[2] = new_spc[3] = new_spc[4] = 0x7E;

	SetChrDef(' ', new_spc);

	SetTextRC(5, 2); PrintStr("Now the space is a small square!");

	SetChrDef(' ', old_spc);

	SetTextRC(7, 2); PrintStr("Now the space is... a space again!");

	/* Draw a screen bitmap */

	DrawScrBitmap(1, 73, pcw_bmp);

	/* Draw a circle */

	SetTextRC(9, 2); PrintStr("A circle:");

	rad = 5 * 8;
	x =  2 * 8 + rad;
	y = 11 * 8 + rad / 2;

	DrawCircle(x, y, rad);

	/* Draw a box */

	SetTextRC(9, 16); PrintStr("Cross & box:");

	x = 16 * 8;
	y = 11 * 8;

	DrawBox(x, y, 80, 40);

	DrawLine(x, y, x + 80 - 1, y + 40 - 1);
	DrawLine(x + 80 - 1, y, x, y + 40 - 1);

	/* Wait the RETURN key */

	SetTextRC(29, 2); PrintStr("Press RETURN to quit.");

	while(!KeyIsPressed(XK_RETURN)) {

		/* Consume any input character */

		if(kbhit())
			getch();
	}

	/* Consume the RETURN character */

	getch();

	/* Clear the screen and show the cursor */

	ClrScr();
	ShowCursor();
}

/* PCW computer bitmap
   -------------------
*/
#asm
pcw_bmp:
 DEFB 015, 000  ; Width in columns
 DEFB 009, 000  ; Height in rows
 DEFB 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 001, 006, 063, 063, 000, 000, 010, 106
 DEFB 093, 214, 255, 255, 000, 000, 170, 171
 DEFB 218, 173, 255, 255, 000, 000, 170, 085
 DEFB 182, 170, 255, 255, 000, 000, 170, 085
 DEFB 213, 173, 255, 255, 000, 000, 170, 085
 DEFB 170, 091, 255, 255, 000, 000, 171, 085
 DEFB 181, 085, 255, 255, 000, 000, 109, 085
 DEFB 085, 086, 255, 255, 000, 000, 221, 085
 DEFB 085, 170, 255, 255, 000, 000, 187, 074
 DEFB 106, 173, 255, 255, 000, 000, 253, 005
 DEFB 245, 086, 255, 255, 000, 016, 214, 041
 DEFB 085, 219, 255, 255, 000, 128, 088, 064
 DEFB 080, 160, 240, 208, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 063, 063, 062, 031
 DEFB 031, 031, 015, 015, 255, 181, 225, 138
 DEFB 096, 166, 201, 162, 255, 086, 041, 146
 DEFB 073, 170, 037, 148, 255, 221, 034, 085
 DEFB 074, 170, 085, 170, 255, 221, 034, 091
 DEFB 170, 170, 085, 170, 255, 221, 086, 085
 DEFB 170, 181, 086, 170, 255, 247, 169, 182
 DEFB 170, 085, 218, 170, 255, 118, 073, 170
 DEFB 170, 085, 170, 170, 255, 219, 036, 146
 DEFB 169, 085, 164, 085, 255, 111, 159, 071
 DEFB 047, 087, 135, 087, 255, 255, 240, 240
 DEFB 248, 240, 240, 184, 253, 223, 033, 001
 DEFB 001, 000, 001, 001, 240, 240, 176, 240
 DEFB 208, 240, 240, 176, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 015, 007, 007, 007
 DEFB 007, 003, 003, 003, 228, 194, 228, 242
 DEFB 209, 242, 233, 248, 082, 170, 149, 073
 DEFB 085, 073, 037, 148, 170, 149, 085, 085
 DEFB 042, 085, 037, 169, 182, 085, 085, 085
 DEFB 170, 085, 085, 042, 213, 086, 090, 085
 DEFB 170, 085, 085, 170, 085, 171, 218, 085
 DEFB 170, 085, 086, 170, 085, 106, 170, 085
 DEFB 170, 085, 170, 170, 082, 170, 146, 084
 DEFB 170, 073, 165, 084, 035, 151, 171, 147
 DEFB 075, 083, 043, 135, 240, 248, 240, 248
 DEFB 240, 248, 216, 255, 001, 001, 001, 001
 DEFB 001, 001, 001, 255, 240, 112, 208, 240
 DEFB 112, 224, 176, 240, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 001, 001, 001, 000
 DEFB 000, 000, 000, 000, 233, 248, 244, 220
 DEFB 250, 122, 125, 109, 082, 010, 169, 068
 DEFB 042, 001, 042, 001, 149, 084, 074, 165
 DEFB 018, 085, 073, 036, 085, 164, 149, 085
 DEFB 085, 074, 041, 165, 085, 170, 085, 085
 DEFB 085, 170, 085, 042, 085, 170, 085, 085
 DEFB 085, 170, 085, 170, 085, 170, 084, 082
 DEFB 085, 170, 085, 146, 082, 149, 169, 148
 DEFB 074, 081, 074, 036, 179, 011, 083, 167
 DEFB 019, 075, 163, 023, 240, 184, 248, 248
 DEFB 104, 248, 248, 176, 001, 001, 001, 001
 DEFB 001, 001, 001, 001, 096, 240, 160, 240
 DEFB 064, 240, 160, 224, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 061, 062, 055, 029
 DEFB 031, 000, 000, 000, 136, 245, 254, 183
 DEFB 127, 169, 005, 010, 146, 000, 255, 255
 DEFB 239, 125, 175, 080, 148, 074, 032, 255
 DEFB 255, 255, 183, 090, 165, 149, 032, 255
 DEFB 255, 255, 224, 191, 084, 085, 000, 255
 DEFB 255, 255, 002, 123, 169, 074, 032, 221
 DEFB 255, 247, 047, 174, 082, 072, 130, 125
 DEFB 223, 251, 239, 255, 163, 011, 135, 255
 DEFB 191, 246, 127, 219, 248, 176, 248, 105
 DEFB 255, 214, 255, 085, 001, 001, 001, 073
 DEFB 255, 181, 239, 091, 096, 192, 160, 224
 DEFB 064, 192, 064, 128, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 001, 002, 000, 000, 000, 020
 DEFB 090, 109, 082, 168, 000, 000, 000, 000
 DEFB 146, 233, 183, 008, 017, 045, 119, 000
 DEFB 148, 033, 222, 107, 043, 173, 246, 021
 DEFB 064, 010, 208, 127, 171, 085, 238, 191
 DEFB 000, 160, 005, 251, 082, 173, 218, 255
 DEFB 010, 000, 250, 183, 170, 173, 246, 173
 DEFB 255, 000, 128, 127, 219, 106, 182, 219
 DEFB 255, 000, 000, 234, 084, 171, 218, 109
 DEFB 255, 074, 000, 000, 164, 085, 170, 181
 DEFB 255, 173, 000, 000, 170, 033, 149, 106
 DEFB 182, 255, 000, 000, 072, 032, 080, 144
 DEFB 160, 240, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 001, 002, 010, 021, 011, 021, 042, 169
 DEFB 084, 165, 168, 085, 069, 040, 066, 050
 DEFB 137, 082, 033, 169, 169, 004, 181, 000
 DEFB 090, 017, 073, 036, 008, 212, 010, 170
 DEFB 132, 041, 074, 148, 133, 164, 086, 161
 DEFB 077, 066, 085, 164, 190, 066, 082, 041
 DEFB 082, 041, 082, 037, 255, 149, 144, 045
 DEFB 128, 092, 132, 040, 245, 095, 169, 068
 DEFB 150, 161, 149, 164, 095, 245, 095, 032
 DEFB 179, 009, 048, 037, 254, 107, 189, 083
 DEFB 008, 170, 072, 034, 160, 223, 118, 091
 DEFB 132, 101, 130, 169, 000, 237, 182, 109
 DEFB 182, 001, 056, 001, 000, 032, 223, 234
 DEFB 182, 011, 165, 045, 000, 000, 160, 160
 DEFB 224, 064, 160, 064, 042, 031, 000, 000
 DEFB 000, 000, 000, 000, 170, 222, 107, 001
 DEFB 000, 000, 000, 000, 173, 214, 123, 093
 DEFB 000, 000, 000, 000, 155, 237, 086, 255
 DEFB 085, 000, 000, 000, 073, 118, 219, 189
 DEFB 239, 043, 000, 000, 010, 233, 095, 234
 DEFB 123, 175, 042, 000, 080, 042, 213, 182
 DEFB 109, 219, 255, 000, 138, 148, 074, 245
 DEFB 086, 187, 237, 183, 067, 168, 018, 136
 DEFB 181, 093, 213, 126, 040, 146, 149, 066
 DEFB 081, 086, 181, 218, 144, 070, 065, 077
 DEFB 016, 148, 182, 218, 004, 168, 020, 033
 DEFB 073, 144, 231, 148, 106, 144, 069, 033
 DEFB 074, 004, 146, 156, 138, 087, 021, 042
 DEFB 053, 086, 090, 170, 128, 000, 128, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 171, 000, 000, 000
 DEFB 000, 000, 000, 000, 239, 021, 000, 000
 DEFB 000, 000, 000, 000, 182, 221, 002, 000
 DEFB 000, 000, 000, 000, 235, 189, 215, 000
 DEFB 000, 000, 000, 000, 106, 171, 253, 043
 DEFB 000, 000, 000, 000, 212, 084, 176, 120
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000
#endasm
