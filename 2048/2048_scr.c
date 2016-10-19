/*	2048_scr.c

	A clon of the game 2048 for the Amstrad PCW.

	Copyright (c) 2016 Miguel Garcia / FloppySoftware

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

		19 Oct 2016 : Screen functions isolated in this file.
*/

// Clear the screen, and cursor to 0,0
// -----------------------------------

ClrScr()
{
	putch(27); putch('H');
	putch(27); putch('E');
}

// Hide the cursor
// ---------------

HideCursor()
{
	putch(27); putch('f');
}

// Show the cursor
// ---------------

ShowCursor()
{
	putch(27); putch('e');
}

// Print centered text
// -------------------
// If text if NULL, the line is erased instead.

CenterText(row, txt)
int row; char *txt;
{
	if(txt)
	{
		// Center the text 
		SetTextRC(row, 44 - strlen(txt) / 2); PrintStr(txt);
	}
	else
	{
		// Erase the line
		SetTextRC(row, 0); PrintChRpt(' ', 90);
	}
}

