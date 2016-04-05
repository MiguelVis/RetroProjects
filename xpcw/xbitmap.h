/*	xbitmap.h

	Graphic functions for the Amstrad PCW and MESCC.

	Bitmaps.

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
	17 Aug 2015 : v1.01 : Added some comments.

	Notes:

	An small bitmap must fit in the RSX buffer (see X_BUFSIZE).

	The bitmaps are drawn in a row,col position (not x,y), and their
	size is specified in rows and columns (not pixels).

	As a row is 8 pixels in height, and a column is 8 pixels in
	width, the bitmap width and height in pixels, must to be an 8
	multiply: 8, 16, 24, 32, etc.

	Needs the xpcw.h library.
*/

/* Draw a small bitmap
   -------------------
*/
DrawSmallBitmap(row, col, rows, cols, bitmap)
int row, col, rows, cols; BYTE *bitmap;
{
	x_fun = X_PutBmpRC;

	x_par[0] = row;
	x_par[1] = col;
	x_par[2] = rows;
	x_par[3] = cols;
	x_par[4] = bitmap;

	x_call();
}

/* Get a small bitmap
   ------------------
*/
GetSmallBitmap(row, col, rows, cols, bitmap)
int row, col, rows, cols; BYTE *bitmap;
{
	x_fun = X_GetBmpRC;

	x_par[0] = row;
	x_par[1] = col;
	x_par[2] = rows;
	x_par[3] = cols;
	x_par[4] = bitmap;

	x_call();
}

/* Draw a big bitmap
   -----------------
*/
DrawBigBitmap(row, col, rows, cols, bitmap)
int row, col, rows, cols; BYTE *bitmap;
{
	int width;

	width  = cols * 8;

	x_fun = X_PutRC;

	while(rows--) {
		x_par[0] = row++;
		x_par[1] = col;
		x_par[2] = bitmap;
		x_par[3] = width;

		x_call();

		bitmap += width;
	}
}

