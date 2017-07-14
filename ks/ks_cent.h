/**
 * @file
 * @brief Center text on screen.
 *
 * This file is part of the KS library.
 */

/*	ks_cent.h

	Keyboard & screen functions library for CP/M & MESCC - Mike's Enhanced Small C Compiler.

	Copyright (c) 2016, 2017 Miguel I. Garcia Lopez / FloppySoftware, Spain

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 3, or (at your option) any
	later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

	Author's contact:

		www.floppysoftware.es
		cpm-connections.blogspot.com
		floppysoftware@gmail.com

	Revisions:

		15 Jul 2017 : Start.
*/

/**
 * @fn     void KsCenterStr(int row, char *s)
 * @brief  Print a string centered on screen.
 * @param  row - screen row
 * @param  s - string
 */
KsCenterStr(row, s)
int row; char *s;
{
	KsPosCursor(row, (KsGetCols() - strlen(s)) / 2);
	KsPutStr(s);
}

