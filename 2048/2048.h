/*	2048.h

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

		19 Oct 2016 : Definitions isolated in this file.
		20 Oct 2016 : v1.00
*/

// Project defs.
// -------------

#define VERSION    "v1.00 / 20 Oct 2016"
#define COPYRIGHT  "(c) 2016 FloppySoftware"
#define WEBSITE    "www.floppysoftware.es"

#define SCORE_ROW  0
#define SCORE_COL  29  // GAME_COL

#define HISCO_ROW  0
#define HISCO_COL  51

#define JOIN_ROW   3   // "Join the numbers and get..."

#define HOWTO_ROW  22  // "How to play..." : GAME_ROW + GAME_ROWS + 1

#define GAME_ROW   5
#define GAME_COL   29
#define GAME_ROWS  16  // BOARD_ROWS * BOX_ROWS + 2 * 2
#define GAME_COLS  32  // BOARD_COLS * BOX_COLS + 4 * 2

#define BOARD_ROW  7  // GAME_ROW + 2
#define BOARD_COL  33 // GAME_COL + 4

#define BOARD_ROWS 4  // In BOX units
#define BOARD_COLS 4  // In BOX units
#define BOARD_SIZE 16 // BOARD_ROWS * BOARD_COLS

#define BOX_ROWS   3  // Height in screen rows
#define BOX_COLS   6  // Width in screen columns

#define K_UP	   31 // Key UP
#define K_DOWN	   30 // Key DOWN
#define K_LEFT	   1  // Key LEFT
#define K_RIGHT	   6  // Key RIGHT
#define K_ESC	   27 // Key ESC (EXIT)

#define SCORES_FNAME "2048.sco"  // File name for scores

