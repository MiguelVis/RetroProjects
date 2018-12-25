/*	xdraw.h

	Graphic functions for the Amstrad PCW and MESCC.

	Drawing library.

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
	14 Oct 2018 : Added DrawVertLine() and DrawHorzLine(). DrawBox() uses these functions now.

	Notes:

	Needs the xpcw.h library.
*/

/* Globals
   -------
*/
int x_mode;  /* Draw mode */

/* Set draw mode
   -------------
*/

SetDrawMode(mode)
int mode;
{
	x_mode = mode;
}

/* Draw a pixel in current mode
   ----------------------------
*/
DrawPixel(x, y)
int x, y;
{
	x_fun = X_SetPixel;
	x_par[0] = x;
	x_par[1] = y;
	x_par[2] = x_mode;

	x_call();
}

/* Return the state of a pixel
   ---------------------------
   Return NZ if painted, else Z.
*/
GetPixel(x, y)
int x, y;
{
	x_fun = X_GetPixel;
	x_par[0] = x;
	x_par[1] = y;

	return x_call();
}

/* Draw vertical line of pixels in current mode
   --------------------------------------------
*/
DrawVertLine(x, y, length)
int x, y, length;
{
	x_fun = X_VertLine;
	x_par[0] = x;
	x_par[1] = y;
	x_par[2] = x_mode;
	x_par[3] = length;

	x_call();
}

/* Draw horizontal line of pixels in current mode
   ----------------------------------------------
*/
DrawHorzLine(x, y, length)
int x, y, length;
{
	x_fun = X_HorzLine;
	x_par[0] = x;
	x_par[1] = y;
	x_par[2] = x_mode;
	x_par[3] = length;

	x_call();
}

/* Draw a line in the current draw mode
   ------------------------------------
*/
DrawLine(x0, y0, x1, y1)
int x0, y0, x1, y1;
{
	int dx, sx;
	int dy, sy;
	int err, e2;

	/*
	dx = abs(x1-x0); sx = x0<x1 ? 1 : -1;
	dy = abs(y1-y0); sy = y0<y1 ? 1 : -1;
	*/

	dx = x1-x0; if(dx<0) dx=-dx; sx = x0<x1 ? 1 : -1;
	dy = y1-y0; if(dy<0) dy=-dy; sy = y0<y1 ? 1 : -1;

	/*
	err = (dx>dy ? dx : -dy)/2;
	*/

	err = (dx>dy ? dx : -dy) >> 1;

	while(1) {
		DrawPixel(x0,y0);

		/*
		if(x0==x1 && y0==y1)
			break;
		*/

		if(x0==x1) {            /* This is faster in MESCC */
			if(y0==y1)
				break;
		}

		e2 = err;

		if(e2 >-dx) {
			err -= dy; x0 += sx;
		}

		if(e2 < dy) {
			err += dx; y0 += sy;
		}
	}
}

/* Draw a circle in the current draw mode
   --------------------------------------
*/
DrawCircle(x0, y0, radius)
int x0, y0, radius;
{
	int x, xh;
	int y, yh;
	int decision;

	x = radius;
	y = 0;
	decision = 1 - x;

	while(x >= y) {

		/* The PCW pixels are double in high */

		xh = x >> 1;  /* xh = x / 2 */
		yh = y >> 1;  /* yh = y / 2 */


		/*
		DrawPixel( x + x0,  y + y0);
		DrawPixel( y + x0,  x + y0);
		DrawPixel(-x + x0,  y + y0);
		DrawPixel(-y + x0,  x + y0);
		DrawPixel(-x + x0, -y + y0);
		DrawPixel(-y + x0, -x + y0);
		DrawPixel( x + x0, -y + y0);
		DrawPixel( y + x0, -x + y0);
		*/

		DrawPixel( x + x0,  yh + y0);
		DrawPixel( y + x0,  xh + y0);
		DrawPixel(-x + x0,  yh + y0);
		DrawPixel(-y + x0,  xh + y0);
		DrawPixel(-x + x0, -yh + y0);
		DrawPixel(-y + x0, -xh + y0);
		DrawPixel( x + x0, -yh + y0);
		DrawPixel( y + x0, -xh + y0);

		y++;

		if(decision <= 0) {
			decision += 2 * y + 1;
		}
		else {
			x--;
			decision += 2 * (y - x) + 1;
		}
	}
}

/* Draw a box in the current draw mode
   -----------------------------------
*/
DrawBox(x, y, width, height)
int x, y, width, height;
{
	DrawHorzLine(x, y, width);
	DrawVertLine(x, y + 1, height - 2);
	DrawVertLine(x + width - 1, y + 1, height - 2);
	DrawHorzline(x, y + height - 1, width);
}
