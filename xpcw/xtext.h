/**
 * @file
 * @project XPCW
 * @brief Text output library for XPCW.
 *
 * This library implements the text output functions for XPCW,
 * the graphic library for the Amstrad PCW & MESCC.
 *
 * Needs the xpcw.h library.
 *
 * @author  Miguel Garcia / floppysoftware@gmail.com
 * @version 1.02
 * @date    14 Apr 2016
 *
 */

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
	14 Apr 2016 : v1.02 : SetFont() - param is not BYTE *adr; it was char *adr.
	                      Added source documentation for gdoc.
*/

/**
 * @fn     SetFont(BYTE *adr) : void
 * @brief  Sets the font for text output.
 *
 * @param  adr - font address or NULL for the system font
 * @return -
 */
SetFont(adr)
BYTE *adr;
{
	x_fun = X_SetFont;
	x_par[0] = adr;

	x_call();
}

/**
 * @fn     SetTextAtt(BYTE att) : void
 * @brief  Sets the text attributes.
 *
 * @param  att - attributes
 * @return -
 */
SetTextAtt(att)
BYTE att;
{
	x_fun = X_SetTxAtt;
	x_par[0] = att;

	x_call();
}

/**
 * @fn     SetTextRC(int row, int col) : void
 * @brief  Sets the position on screen for text outputs.
 *
 * @param  row - screen row [0..31]
 * @param  col - screen col [0..89]
 * @return -
 */
SetTextRC(row, col)
int row, col;
{
	x_fun = X_SetTxRC;
	x_par[0] = row;
	x_par[1] = col;

	x_call();
}

/**
 * @fn     PrintCh(int ch) : void
 * @brief  Prints a character on screen.
 *
 * @param  ch - character code [0..255]
 * @return -
 */
PrintCh(ch)
int ch;
{
	x_fun = X_PrintCh;
	x_par[0] = ch;

	x_call();
}

/**
 * @fn     PrintChRpt(int ch, int n) : void
 * @brief  Prints n times a character on screen.
 *
 * It's faster than printing a character in a loop with PrintCh().
 *
 * @param  ch - character code [0..255]
 * @param  n  - times
 * @return -
 */
PrintChRpt(ch, n)
int ch, n;
{
	x_fun = X_PrintRpt;
	x_par[0] = ch;
	x_par[1] = n;

	x_call();
}

/**
 * @fn     PrintStr(BYTE *str) : void
 * @brief  Prints a string on screen.
 *
 * @param  str - string
 * @return -
 */
/* Print a string
   --------------
*/
PrintStr(str)
BYTE *str;
{
	while(*str)
		PrintCh(*str++);
}
