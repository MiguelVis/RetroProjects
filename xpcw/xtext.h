/*	xtext.h

	Graphic functions for the Amstrad PCW and MESCC.

	Text output library.

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

	21 Jul 2015 : v1.00 : First version.
	17 Aug 2015 : v1.01 : Added PrintChRpt().

	Notes:

	Needs the xpcw.h library.
*/

/* Set the current font
   --------------------
   NULL means the internal font.
*/
SetFont(adr)
char *adr;
{
	x_fun = X_SetFont;
	x_par[0] = adr;

	x_call();
}

/* Set the text attributes
   -----------------------
*/
SetTextAtt(att)
BYTE att;
{
	x_fun = X_SetTxAtt;
	x_par[0] = att;

	x_call();
}

/* Set the text position
   ---------------------
*/
SetTextRC(row, col)
int row, col;
{
	x_fun = X_SetTxRC;
	x_par[0] = row;
	x_par[1] = col;

	x_call();
}

/* Print a character
   -----------------
*/
PrintCh(ch)
int ch;
{
	x_fun = X_PrintCh;
	x_par[0] = ch;

	x_call();
}

/* Print a character n times
   -------------------------
   It's faster than using PrintCh() in a loop.
*/
PrintChRpt(ch, n)
int ch, n;
{
	x_fun = X_PrintRpt;
	x_par[0] = ch;
	x_par[1] = n;

	x_call();
}

/* Print a string
   --------------
*/
PrintStr(str)
BYTE *str;
{
	while(*str)
		PrintCh(*str++);
}

