/*	xchrdef.h

	Graphic functions for the Amstrad PCW and MESCC.

	Get / set character definitions of internal font.

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

	Needs the xpcw.h library.
*/

/* Set a character definition
   --------------------------
*/
SetChrDef(ch, def)
int ch; BYTE *def;
{
	x_fun = X_WriteEnv;
	x_par[0] = 0xB800 + ch * 8;
	x_par[1] = def;
	x_par[2] = 8;

	x_call();
}

/* Get a character definition
   --------------------------
*/
GetChrDef(ch, def)
int ch; BYTE *def;
{
	x_fun = X_ReadEnv;
	x_par[0] = 0xB800 + ch * 8;
	x_par[1] = def;
	x_par[2] = 8;

	x_call();
}
