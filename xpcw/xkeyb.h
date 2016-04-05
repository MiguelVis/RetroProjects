/*	xkeyb.h

	Graphic functions for the Amstrad PCW and MESCC.

	Keyboard.

	Copyright (c) 2015 Miguel I. Garcia Lopez / FloppySoftware, Spain

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

	12 Aug 2015 : v1.00 : First version.

	Notes:

	Needs the xgraph.h library.
*/

/* Some keys
   ---------
*/
#define XK_RETURN 0x0204
#define XK_EXIT   0x0101
#define XK_STOP   0x0804
#define XK_SPACE  0x0580
#define XK_UP     0x0140
#define XK_DOWN   0x0A40
#define XK_LEFT   0x0140
#define XK_RIGHT  0x0040

/* Get the keyboard state matrix
   -----------------------------
   Returns the keyboard state matrix
*/
GetKeyMatrix()
{
	x_fun = X_KeyMatrix;

	return x_call();
}

/* Test a key against a keyboard state matrix
   ------------------------------------------
   Returns NZ if pressed, else Z.
*/
KeyTest(matrix, key)
BYTE *matrix; int key;
{
	return matrix[(key >> 8) & 0x0F] & (key & 0xFF);
}

/* Test if a key is pressed
   ------------------------
*/
KeyIsPressed(key)
int key;
{
	return KeyTest(GetKeyMatrix(), key);
}
