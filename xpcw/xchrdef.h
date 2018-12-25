/**
 * @file
 * @project XPCW
 * @brief Character definitions library for XPCW.
 *
 * This library implements the character definitions functions
 * for XPCW, the graphic library for the Amstrad PCW & MESCC.
 *
 * Its functions deal with the PCW internal font under CP/M Plus.
 *
 * Needs the xpcw.h library.
 *
 * @author  Miguel Garcia / floppysoftware@gmail.com
 * @version 1.00
 * @date    12 Aug 2015
 *
 */

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
*/

/**
 * @fn     SetChrDef(int ch, BYTE *def) : void
 * @brief  Sets the definition of a character.
 *
 * @param  ch  - character code: 0..255
 * @param  def - definition (8 bytes)
 * @return -
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

/**
 * @fn     GetChrDef(int ch, BYTE *def) : void
 * @brief  Gets the definition of a character.
 *
 * @param  ch  - character code: 0..255
 * @param  def - definition (8 bytes)
 * @return -
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
