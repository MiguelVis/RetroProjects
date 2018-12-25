/**
 * @file
 * @project XPCW
 * @brief Bitmap library for XPCW.
 *
 * This library implements the bitmap functions for XPCW,
 * the graphic library for the Amstrad PCW & MESCC.
 *
 * An small bitmap must fit in the RSX buffer (see X_BUFSIZE in xpcw.h).
 *
 * The bitmaps are drawn in a row,col positions (not x,y), and their
 * size is specified in character rows and columns (not pixels).
 *
 * As a row is 8 pixels in height, and a column is 8 pixels in
 * width, the bitmap width and height in pixels, must to be an 8
 * multiply: 8, 16, 24, 32, etc.
 *
 * Needs the xpcw.h library.
 *
 * @author  Miguel Garcia / floppysoftware@gmail.com
 * @version 1.01
 * @date    17 Aug 2015
 *
 */

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
*/

/**
 * @fn     DrawSmallBitmap(int row, int col, int rows, int cols, BYTE *bitmap) : void
 * @brief  Draws a small bitmap on screen.
 *
 * A small bitmap can be only up to X_BUFSIZE bytes in size.
 *
 * @param  row    - vertical position on screen: 0..31
 * @param  col    - horizontal position on screen: 0..79
 * @param  rows   - height in character rows
 * @param  cols   - width in character positions
 * @param  bitmap - bitmap
 * @return -
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

/**
 * @fn     GetSmallBitmap(int row, int col, int rows, int cols, BYTE *bitmap) : void
 * @brief  Get a small bitmap from screen.
 *
 * A small bitmap can be only up to X_BUFSIZE bytes in size.
 *
 * @param  row    - vertical position on screen: 0..31
 * @param  col    - horizontal position on screen: 0..79
 * @param  rows   - height in character rows
 * @param  cols   - width in character positions
 * @param  bitmap - bitmap
 * @return -
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

/**
 * @fn     DrawBigBitmap(int row, int col, int rows, int cols, BYTE *bitmap) : void
 * @brief  Draws a big (or small) bitmap on screen.
 *
 * This funcion is slower than DrawSmallBitmap() for small
 * bitmaps.
 *
 * @param  row    - vertical position on screen: 0..31
 * @param  col    - horizontal position on screen: 0..79
 * @param  rows   - height in character rows
 * @param  cols   - width in character positions
 * @param  bitmap - bitmap
 * @return -
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

