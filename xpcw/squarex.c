/*	squarex.c

	A game for the Amstrad PCW.

	Copyright (c) 2015 Miguel Garcia / FloppySoftware

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

	To compile with MESCC:

	cc squarex
	zsm squarex
	hexcom squarex
	gencom squarex.com xpcw.rsx

	Note:

	Change version and date in DrawScreen() function.

	Changes:

	31 Jul 2015 : v1.00 : Initial version.
	12 Aug 2015 : v1.01 : Some minor improvements.
	13 Aug 2015 : v1.02 : Added PCW computer bitmap.
	17 Aug 2015 : v1.03 : Added use of PrintChRpt() and HelloRsx().
*/

/* Some defines for MESCC
   ----------------------
*/
#define CC_NO_ARGS  /* No argc and argv[] in main */

/* MESCC libraries
   ---------------
*/
#include "mescc.h"
#include "conio.h"
#include "string.h"
#include "ctype.h"
#include "sprintf.h"
#include "z80.h"

/* XPCW libraries
   --------------
*/
#include "xpcw.h"
#include "xtext.h"
#include "xbitmap.h"

/* Board
   -----
*/
#define BOARD_ROW    6   /* Board position on screen */
#define BOARD_COL    32  /* idem */

#define BOARD_WIDTH  13  /* Board width in squares */
#define BOARD_HEIGHT 18  /* Board height in squares */
#define BOARD_SIZE   234 /* BOARD_WIDTH * BOARD_HEIGHT */

BYTE board[BOARD_SIZE];  /* Board array */

/* Pieces
   ------
*/
#define PIECE_WIDTH  3   /* Piece width in squares */
#define PIECE_HEIGHT 3   /* Piece height in squares */
#define PIECE_SIZE   9   /* PIECE_WIDTH * PIECE_HEIGHT */

extern BYTE piece_o[PIECE_SIZE];
extern BYTE piece_i[PIECE_SIZE], piece_s[PIECE_SIZE];
extern BYTE piece_z[PIECE_SIZE], piece_l[PIECE_SIZE];
extern BYTE piece_j[PIECE_SIZE], piece_t[PIECE_SIZE];

/* Random selection
   ----------------
*/
char *rand_piece;  /* Pieces */
char *rand_rot;    /* Rotation */
int rand_next;     /* Index to next piece */

/* Playing values
   --------------
*/
BYTE play_piece[PIECE_SIZE];  /* Current piece */
BYTE play_next[PIECE_SIZE];   /* Next piece */
BYTE play_temp[PIECE_SIZE];   /* Temporary piece */
int play_row, play_col;       /* Piece position on board */
int play_pieces;              /* # of played pieces */
int play_lines;               /* # of completed board lines */

/* Other globals
   -------------
*/
int sound;                    /* NON ZERO if sounds is ON */
extern BYTE font[];           /* Text font */
extern BYTE pcw_bmp[];        /* PCW computer bitmap */

/* Emit a small beep if sound is ON
   --------------------------------
*/
Beep()
{
	if(sound) {
		Z80out(248, 11);
		Z80out(248, 12);
	}
}

/* Set random values
   -----------------
*/
SetupRand(seed)
int seed;
{
	int max;

	rand_piece = "COZLTJCOTLSCTOTCLOTXJLOCTLOCTJLSOTZSCOXTLCSTOLCTLCJTCSOTCLSXZTOLTCTSJOTZJLSOJTCJCLJOX";
	rand_rot   = " L R R R L   R  L R   L  R    R  R L LR  L L   R R L R L  L L L LL R R  R R LRL  R L ";

	max = strlen(rand_piece) - 1;

	while(seed > max) {
		seed /= 2;
	}

	rand_next = seed;
}

/* Peek next piece
   ---------------
*/
PeekNext(piece)
BYTE *piece;
{
	BYTE *p;

	/* Check random index */

	if(!rand_piece[rand_next]) {
		rand_next = 0;
	}

	/* Peek next piece */

	switch(rand_piece[rand_next]) {
		case 'C' : p = piece_c; break;
		case 'L' : p = piece_l; break;
		case 'J' : p = piece_j; break;
		case 'X' : p = piece_x; break;
		case 'S' : p = piece_s; break;
		case 'Z' : p = piece_z; break;
		case 'O' : p = piece_o; break;
		default  : p = piece_t; break;
	}

	/* Peek the piece */

	CopyPiece(piece, p);

	/* Rotate if needed */

	switch(rand_rot[rand_next]) {
		case 'L' : RotateLeft(piece); break;
		case 'R' : RotateRight(piece); break;
		default  : break;
	}
}

/* Get next piece
   --------------
*/
GetNext(piece)
BYTE *piece;
{
	PeekNext(piece); ++rand_next;
}

/* Set value in a board square
   ---------------------------
*/
SetBoard(row, col, value)
int row, col, value;
{
	board[row * BOARD_WIDTH + col] = value;
}

/* Get value from a board square
   -----------------------------
*/
GetBoard(row, col)
int row, col;
{
	return board[row * BOARD_WIDTH + col];
}

/* Reset the board
   ---------------
*/
ResetBoard()
{
	int r, c;

	for(r = 0; r < BOARD_HEIGHT; ++r) {
		for(c = 0; c < BOARD_WIDTH; ++c) {
			SetBoard(r, c, 0);
		}
	}
}

/* Quit completed board lines
   --------------------------
*/
QuitLines()
{
	int r, c, j;

	/* Check all rows, starting on the bottom */

	for(r = play_row + PIECE_HEIGHT - 1; r >= play_row; --r) {

		/* Check if all the squares in the line are filled */

		for(c = 0; c < BOARD_WIDTH; ++c) {
			if(!GetBoard(r, c))
				break;
		}

		/* Check if the line is completed */

		if(c == BOARD_WIDTH) {

			/* Line is completed */

			++play_lines;

			/* Draw all squares as filled */

			for(c = 0; c < BOARD_WIDTH; ++c) {
				DrawSquare(r, c, fill_bmp); Beep();
			}

			/* A small pause */

			for(j = 0; j < 1000; ++j);

			/* Redraw the board */

			for(j = r; j > 0; --j) {
				for(c = 0; c < BOARD_WIDTH; ++c) {
					SetBoard(j, c, GetBoard(j - 1, c));
					DrawSquare(j, c, GetBoard(j, c) ? piece_bmp : empty_bmp);
				}
			}

			/* Clean the 1st row (now is empty for sure) */

			for(c = 0; c < BOARD_WIDTH; ++c) {
				SetBoard(0, c, 0);
				DrawSquare(0, c, empty_bmp);
			}

			/* Important */

			++r;
			++play_row;
		}
	}
}

/* Draw the screen
   ---------------
*/
DrawScreen()
{
	int r, c, ch;
	char *s;

	/* Clear the screen, and hide the cursor */

	putch(27); putch('H');
	putch(27); putch('E');
	putch(27); putch('f');

	/* Next piece */

	SetTextRC(0, BOARD_COL + 11);
	PrintStr("NEXT");

	/* Board border */

	SetTextAtt(XA_REV);

	for(r = 0; r < BOARD_ROW + BOARD_HEIGHT; ++r) {

		ch = (r == BOARD_ROW - 1) ? '_' : ' ';

		SetTextRC(r, BOARD_COL - 2);

		/* **********************
		PrintCh(ch); PrintCh(ch);
		********************** */

		PrintChRpt(ch, 2);

		SetTextRC(r, BOARD_COL + BOARD_WIDTH + BOARD_WIDTH);

		/* **********************
		PrintCh(ch); PrintCh(ch);
		********************** */

		PrintChRpt(ch, 2);
	}

	SetTextRC(r, BOARD_COL - 2);

	/* ***********************************************
	for(c = BOARD_WIDTH + BOARD_WIDTH + 4; c > 0; --c)
		PrintCh(' ');
	*********************************************** */

	PrintChRpt(' ', BOARD_WIDTH + BOARD_WIDTH + 4);

	/* Title and credits */

	SetTextAtt(XA_NORMAL);

	r = 3;
	c = 0;

	SetTextAtt(XA_BOLD);
	SetTextRC(r++, c); PrintStr("          SquareX");
	SetTextAtt(XA_NORMAL);
	++r;
	SetTextRC(r++, c); PrintStr("    v1.03 / 17 Aug 2015");
	++r;                    s = "   c 2015 FloppySoftware"; s[3] = 164; /* (c) */
	SetTextRC(r++, c); PrintStr(s);
	++r;;
	SetTextRC(r++, c); PrintStr("www.floppysoftware.vacau.com");
	SetTextRC(r++, c); PrintStr("cpm-connections.blogspot.com");
	SetTextRC(r++, c); PrintStr("  floppysoftware@gmail.com");

	++r;
	++r;

	SetTextRC(r++, c); PrintStr("          Sep 2015");
	SetTextRC(r++, c); PrintStr("      30th Anniversary");
	SetTextRC(r++, c); PrintStr("           of the");
	SetFont(NULL);
	SetTextRC(r++, c); PrintStr("        Amstrad ");
	SetTextAtt(XA_ITALIC);
	PrintStr("PCW");
	SetTextAtt(XA_NORMAL);
	SetFont(font);

	/* PCW computer bitmap */

	DrawBigBitmap(++r, c + 5, pcw_bmp[2], pcw_bmp[0], pcw_bmp + 4);

	/* Pieces and lines */

	r = 3;
	c = 63;

	SetTextRC(r++, c); PrintStr("Pieces : 0");
	SetTextRC(r++, c); PrintStr(" Lines : 0");

	++r;
	++r;
	++r;

	/* Keyboard summary */

	SetTextRC(r++, c); PrintStr("     O : Left");
	SetTextRC(r++, c); PrintStr("     P : Right");
	SetTextRC(r++, c); PrintStr("     Q : Rotate left");
	SetTextRC(r++, c); PrintStr("     W : Rotate right");
	SetTextRC(r++, c); PrintStr(" SPACE : Down fast");
	SetTextRC(r++, c); PrintStr("RETURN : Pause / continue");
	SetTextRC(r++, c); PrintStr("   TAB : Sound (now ???)"); SoundSt();
	SetTextRC(r++, c); PrintStr("  EXIT : Quit game");
}

/* Print the sound status
   ----------------------
*/
SoundSt()
{
	SetTextRC(14, 83); PrintStr(sound ? "ON " : "OFF");
}

/* Get the value of a piece square
   -------------------------------
*/
GetPiece(row, col, piece)
int row, col; BYTE *piece;
{
	return piece[row * PIECE_WIDTH + col];
}

/* Set the value of a piece square
   -------------------------------
*/
SetPiece(row, col, piece, value)
int row, col; BYTE *piece; int value;
{
	piece[row * PIECE_WIDTH + col] = value;
}

/* Copy a piece into another
   -------------------------
*/
CopyPiece(to, from)
BYTE *to, *from;
{
	int r, c;

	for(r = 0; r < PIECE_HEIGHT; ++r) {
		for(c = 0; c < PIECE_WIDTH; ++c) {
			SetPiece(r, c, to, GetPiece(r, c, from));
		}
	}
}

/* Rotate a piece to the left
   --------------------------
*/
RotateLeft(piece)
BYTE *piece;
{
	int r, c;
	BYTE tmp[PIECE_SIZE];

	for(r = 0; r < PIECE_HEIGHT; ++r) {
		for(c = 0; c < PIECE_WIDTH; ++c) {
			SetPiece(PIECE_HEIGHT - 1 - c, r, tmp, GetPiece(r, c, piece));
		}
	}

	CopyPiece(piece, tmp);
}

/* Rotate a piece to the right
   ---------------------------
*/
RotateRight(piece)
BYTE *piece;
{
	int r, c;
	BYTE tmp[PIECE_SIZE];

	for(r = 0; r < PIECE_HEIGHT; ++r) {
		for(c = 0; c < PIECE_WIDTH; ++c) {
			SetPiece(c, PIECE_WIDTH - 1 - r, tmp, GetPiece(r, c, piece));
		}
	}

	CopyPiece(piece, tmp);
}

/* Check if a piece holds in the board
   -----------------------------------
   Return 0 if not, else 1.
*/
CanHold(row, col, piece)
int row, col; BYTE *piece;
{
	int r, c;

	if(row + PIECE_HEIGHT > BOARD_HEIGHT)
		return 0;

	for(r = PIECE_HEIGHT - 1; r >= 0; --r) {
		if(row + r < 0)
			continue;

		for(c = 0; c < PIECE_WIDTH; ++c) {
			if(GetPiece(r, c, piece)) {
				if(GetBoard(row + r, col + c))
					return 0;
			}
		}
	}

	return 1;
}

/* Hold (put) a piece in the board
   -------------------------------
*/
Hold(row, col, piece)
int row, col; BYTE *piece;
{
	int r, c;

	for(r = 0; r < PIECE_WIDTH; ++r) {
		for(c = 0; c < PIECE_WIDTH; ++c) {
			if(GetPiece(r, c, piece)) {
				SetBoard(row + r, col + c, 1);
			}
		}
	}
}

/* Draw a piece on the board
   -------------------------
*/
DrawPiece(row, col, piece)
int row, col; BYTE *piece;
{
	int r, c;

	for(r = 0; r < PIECE_WIDTH; ++r) {
		for(c = 0; c < PIECE_WIDTH; ++c) {
			if(GetPiece(r, c, piece)) {
				DrawSquare(row + r, col + c, piece_bmp);
			}
		}
	}
}

/* Erase a piece on the board
   --------------------------
*/
ErasePiece(row, col, piece)
int row, col; BYTE *piece;
{
	int r, c;

	for(r = 0; r < PIECE_WIDTH; ++r) {
		for(c = 0; c < PIECE_WIDTH; ++c) {
			if(GetPiece(r, c, piece)) {
				DrawSquare(row + r, col + c, empty_bmp);
			}
		}
	}
}

/* Draw a square in the board
   --------------------------
*/
DrawSquare(row, col, bitmap)
int row, col; BYTE *bitmap;
{
	/****
	DrawScr(BOARD_ROW + row, BOARD_COL + col * 2, bitmap, 16);
	****/

	DrawScr(BOARD_ROW + row, BOARD_COL + (col << 1), bitmap, 16);
}

/* Print centered text
   -------------------
   If text if NULL, the line is erased instead.
*/
CenterText(row, txt)
int row; char *txt;
{
	/* ***
	int i;
	*** */

	if(txt != NULL) {

		/* Center the text */

		SetTextRC(row, 44 - strlen(txt) / 2); PrintStr(txt);
	}
	else {
		/* Erase the line */

		SetTextRC(row, 0);

		/* ********************
		for(i = 0; i < 90; ++i)
			PrintCh(' ');
		******************** */

		PrintChRpt(' ', 90);
	}
}

/* Draw on the screen
   ------------------
*/
DrawScr(row, col, data, size)
int row, col; BYTE *data; int size;
{
	x_fun = X_PutRC;

	x_par[0] = row;
	x_par[1] = col;
	x_par[2] = data;
	x_par[3] = size;

	x_call();
}

/* Play a piece
   ------------
*/
PlayPiece()
{
	int i, loop_end, ch;

	/* Get next piece */

	GetNext(play_piece);

	/* Initial location */

	play_row = -4;
	play_col = 5;

	/* Keyboard loop iterations */

	loop_end = 900;

	/* Main loop */

	while(1) {

		/* Draw the piece */

		DrawPiece(play_row, play_col, play_piece);

		/* A beep */

		Beep();

		/* Draw next piece if the current one is on the row 3 */

		if(play_row == 3) {
			PeekNext(play_next);
			DrawPiece(-4, 5, play_next);
		}

		/* Keyboard loop */

		for(i = 0; i < loop_end; ++i) {

			/* If no keyboard hit, skip this iteration */

			if(!kbhit())
				continue;

			/* Check the key */

			switch((ch = tolower(getch()))) {

				/* Move left */

				case 'o' :
					if(play_col > 0 && CanHold(play_row, play_col - 1, play_piece)) {
						ErasePiece(play_row, play_col, play_piece);
						DrawPiece(play_row, --play_col, play_piece);
					}
					break;

				/* Move right */

				case 'p' :
					if(play_col < BOARD_WIDTH - PIECE_WIDTH && CanHold(play_row, play_col + 1, play_piece)) {
						ErasePiece(play_row, play_col, play_piece);
						DrawPiece(play_row, ++play_col, play_piece);
					}
					break;

				/* Rotate left */

				case 'q' :

				/* Rotate right */

				case 'w' :
					CopyPiece(play_temp, play_piece);

					if(ch == 'q')
						RotateLeft(play_temp);
					else
						RotateRight(play_temp);

					if(CanHold(play_row, play_col, play_temp)) {
						ErasePiece(play_row, play_col, play_piece);
						DrawPiece(play_row, play_col, play_temp);
						CopyPiece(play_piece, play_temp);
					}
					break;

				/* Hold fast */

				case ' ' :
					loop_end = 0;
					break;

				/* Pause / continue */

				case 13 : /* RETURN */
					CenterText(29, "P A U S E D");
					while(getch() != 13);
					CenterText(29, NULL);
					break;

				/* Sound on / off */

				case 9 : /* TAB */
					sound = 1 - sound;
					SoundSt();
					break;

				/* Stop the game */

				case 27 : /* EXIT */
					return 1;
			}
		}

		/* Check if the piece can move down */

		if(!CanHold(play_row + 1, play_col, play_piece)) {

			/* No: check if the piece is in the board */

			if(play_row >= 0) {

				/* Yes: Hold */

				Hold(play_row, play_col, play_piece);
				break;
			}
			else {
 				/* No: Board is full */

				return 1;
			}
		}

		/* Erase the piece */

		ErasePiece(play_row++, play_col, play_piece);
	}

	/* Erase completed lines */

	if(play_row >= 0)
		QuitLines();

	/* Erase the next piece */

	ErasePiece(-4, 5, play_next);

	/* Board is not full */

	return 0;
}

/* Ask user to play or exit
   ------------------------
   Return the random seed.
*/
WhatNow(quit)
int quit;
{
	int seed, ch;

	/* Print input prompt */

	CenterText(29, quit ? "Press SPACE to play again, or EXIT to quit" : "Press SPACE to play");

	/* Reset random seed */

	seed = 0;

	/* Wait a reply */

	while(1) {
		if(kbhit()) {
			ch = getch();

			if(ch == ' ')             /* Play */
				break;

			if(ch == 27 && quit) {    /* Exit */
				seed = -1;
				break;
			}
		}

		/* Increment seed */

		++seed;
	}

	/* Erase prompt */

	CenterText(29, NULL);

	/* Return seed */

	return seed;
}

/* Entry point
   -----------
*/
main()
{
	int result, start, seed;
	char number[6];

	/* Check if the RSX is in memory */

	if(!HelloRsx()) {
		puts("The RSX is not in memory!"); return -1;
	}

	start = 0; /* ZERO for the first time */
	sound = 0; /* Sound OFF */

	/* Main loop */

	while(1) {

		/* Set font */

		SetFont(font);
		SetTextAtt(XA_NORMAL);

		/* Draw the screen */

		DrawScreen();

		/* Reset values */

		ResetBoard();

		play_pieces = play_lines = 0;

		/* Prompt for the 1st time */

		if(!start++) {
			if((seed = WhatNow(0)) == -1)
				break;
		}

		/* Setup random values */

		SetupRand(seed);

		/* Play the pieces */

		do {
			/* Play a piece */

			result = PlayPiece();

			/* Update the # of played pieces */

			sprintf(number, "%d", ++play_pieces);

			SetTextRC(3, 72); PrintStr(number);

			/* Update the # of completed lines */

			sprintf(number, "%d", play_lines);

			SetTextRC(5, 72); PrintStr(number);

		} while(!result);

		/* Game over */

		CenterText(27, "G A M E   O V E R");

		/* Prompt to continue or exit */

		if((seed = WhatNow(1)) == -1)
			break;
	}

	/* Clear the screen, and show the cursor */

	putch(27); putch('H');
	putch(27); putch('E');
	putch(27); putch('e');
}

/* Pieces
   ------
*/
#asm
piece_c:
	defb 1,1,1
	defb 1,0,0
	defb 1,1,1

piece_l:
	defb 1,0,0
	defb 1,0,0
	defb 1,1,1

piece_j:
	defb 0,1,0
	defb 0,1,0
	defb 1,0,1

piece_x:
	defb 1,0,1
	defb 0,1,0
	defb 1,0,1

piece_s:
	defb 0,1,1
	defb 0,1,0
	defb 1,1,0

piece_z:
	defb 1,1,0
	defb 0,1,0
	defb 0,1,1

piece_t:
	defb 1,1,1
	defb 0,1,0
	defb 0,1,0

piece_o:
	defb 1,1,1
	defb 1,1,1
	defb 1,1,1
#endasm

/* Bitmaps
   -------
*/
#asm
piece_bmp:
	DEFB 255, 213, 234, 213
	DEFB 234, 213, 234, 128, 254, 084, 168, 084
	DEFB 168, 084, 168, 000

empty_bmp:
	defb 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

fill_bmp:
	DEFB 255, 213, 255, 223
	DEFB 255, 223, 255, 128, 254, 084, 252, 252
	DEFB 252, 252, 252, 000
#endasm

/* Font (samaruc_thin)
   -------------------
*/
#asm
font:
 DEFB 000, 063, 063, 063
 DEFB 063, 063, 063, 063, 000, 255, 255, 255
 DEFB 255, 255, 255, 255, 000, 252, 252, 252
 DEFB 252, 252, 252, 252, 063, 063, 063, 063
 DEFB 063, 063, 063, 063, 255, 255, 255, 255
 DEFB 255, 255, 255, 255, 252, 252, 252, 252
 DEFB 252, 252, 252, 252, 063, 063, 063, 063
 DEFB 063, 063, 063, 000, 255, 255, 255, 255
 DEFB 255, 255, 255, 000, 252, 252, 252, 252
 DEFB 252, 252, 252, 000, 000, 021, 042, 021
 DEFB 042, 021, 042, 021, 000, 085, 170, 085
 DEFB 170, 085, 170, 085, 168, 084, 168, 084
 DEFB 168, 084, 168, 000, 042, 021, 042, 021
 DEFB 042, 021, 042, 021, 170, 085, 170, 085
 DEFB 170, 085, 170, 085, 168, 084, 168, 084
 DEFB 168, 084, 168, 084, 042, 021, 042, 021
 DEFB 042, 021, 042, 000, 170, 085, 170, 085
 DEFB 170, 085, 170, 000, 168, 084, 168, 084
 DEFB 168, 084, 168, 000, 000, 000, 001, 003
 DEFB 007, 015, 000, 000, 000, 000, 128, 192
 DEFB 224, 240, 000, 000, 000, 000, 015, 007
 DEFB 003, 001, 000, 000, 000, 000, 240, 224
 DEFB 192, 128, 000, 000, 252, 240, 192, 192
 DEFB 192, 192, 240, 252, 063, 015, 003, 003
 DEFB 003, 003, 015, 063, 255, 255, 255, 000
 DEFB 255, 255, 255, 255, 124, 016, 016, 016
 DEFB 016, 016, 016, 124, 001, 059, 109, 253
 DEFB 093, 027, 001, 000, 129, 227, 247, 255
 DEFB 247, 227, 129, 000, 000, 000, 001, 007
 DEFB 001, 000, 000, 000, 000, 096, 224, 224
 DEFB 224, 096, 000, 000, 000, 006, 007, 007
 DEFB 007, 006, 000, 000, 000, 000, 128, 224
 DEFB 128, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 024, 060, 060, 024
 DEFB 024, 000, 024, 000, 068, 102, 000, 000
 DEFB 000, 000, 000, 000, 108, 254, 108, 108
 DEFB 108, 254, 108, 000, 024, 060, 102, 048
 DEFB 012, 102, 060, 024, 000, 198, 204, 024
 DEFB 048, 102, 198, 000, 056, 108, 056, 112
 DEFB 218, 204, 118, 000, 024, 008, 000, 000
 DEFB 000, 000, 000, 000, 014, 056, 096, 096
 DEFB 096, 056, 014, 000, 112, 028, 006, 006
 DEFB 006, 028, 112, 000, 000, 198, 056, 254
 DEFB 056, 198, 000, 000, 000, 024, 024, 126
 DEFB 024, 024, 000, 000, 000, 000, 000, 000
 DEFB 000, 024, 008, 016, 000, 000, 000, 126
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 024, 000, 000, 006, 012, 024
 DEFB 048, 096, 192, 000, 060, 066, 078, 090
 DEFB 114, 066, 060, 000, 008, 024, 056, 008
 DEFB 008, 008, 008, 000, 060, 066, 002, 060
 DEFB 064, 066, 126, 000, 060, 066, 002, 028
 DEFB 002, 066, 060, 000, 028, 036, 068, 132
 DEFB 254, 012, 012, 000, 126, 064, 124, 002
 DEFB 066, 066, 060, 000, 060, 066, 064, 124
 DEFB 066, 066, 060, 000, 126, 002, 004, 008
 DEFB 008, 008, 008, 000, 060, 066, 066, 060
 DEFB 066, 066, 060, 000, 060, 066, 066, 062
 DEFB 002, 002, 002, 000, 000, 000, 000, 000
 DEFB 024, 000, 024, 000, 000, 000, 000, 024
 DEFB 000, 024, 008, 016, 012, 024, 048, 096
 DEFB 048, 024, 012, 000, 000, 000, 126, 000
 DEFB 126, 000, 000, 000, 048, 024, 012, 006
 DEFB 012, 024, 048, 000, 060, 102, 102, 012
 DEFB 024, 000, 024, 000, 060, 066, 090, 090
 DEFB 076, 064, 062, 000, 060, 066, 066, 126
 DEFB 066, 066, 066, 000, 124, 066, 066, 124
 DEFB 066, 066, 124, 000, 060, 070, 064, 064
 DEFB 064, 070, 060, 000, 124, 066, 066, 066
 DEFB 066, 066, 124, 000, 126, 064, 064, 120
 DEFB 064, 064, 126, 000, 126, 064, 064, 120
 DEFB 064, 064, 064, 000, 060, 066, 064, 078
 DEFB 066, 066, 060, 000, 066, 066, 066, 126
 DEFB 066, 066, 066, 000, 124, 016, 016, 016
 DEFB 016, 016, 124, 000, 002, 002, 002, 002
 DEFB 066, 066, 060, 000, 066, 068, 072, 112
 DEFB 072, 068, 066, 000, 064, 064, 064, 064
 DEFB 064, 064, 126, 000, 066, 102, 090, 066
 DEFB 066, 066, 066, 000, 066, 098, 082, 074
 DEFB 070, 066, 066, 000, 060, 066, 066, 066
 DEFB 066, 066, 060, 000, 124, 066, 066, 066
 DEFB 124, 064, 064, 000, 060, 066, 066, 066
 DEFB 066, 108, 054, 000, 124, 066, 066, 124
 DEFB 066, 066, 066, 000, 060, 066, 064, 060
 DEFB 002, 066, 060, 000, 124, 016, 016, 016
 DEFB 016, 016, 016, 000, 066, 066, 066, 066
 DEFB 066, 066, 060, 000, 066, 066, 066, 066
 DEFB 066, 036, 024, 000, 130, 130, 130, 130
 DEFB 146, 170, 198, 000, 066, 066, 036, 024
 DEFB 036, 066, 066, 000, 068, 068, 068, 040
 DEFB 016, 016, 016, 000, 124, 004, 008, 016
 DEFB 032, 064, 124, 000, 060, 048, 048, 048
 DEFB 048, 048, 060, 000, 000, 192, 096, 048
 DEFB 024, 012, 006, 000, 060, 012, 012, 012
 DEFB 012, 012, 060, 000, 000, 024, 060, 126
 DEFB 024, 024, 024, 024, 000, 000, 000, 000
 DEFB 000, 000, 000, 255, 048, 024, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 062, 066
 DEFB 066, 078, 050, 000, 064, 064, 124, 066
 DEFB 066, 066, 124, 000, 000, 000, 060, 066
 DEFB 064, 066, 060, 000, 006, 006, 062, 066
 DEFB 066, 066, 062, 000, 000, 000, 060, 066
 DEFB 124, 064, 062, 000, 028, 034, 032, 120
 DEFB 032, 032, 032, 032, 000, 000, 060, 066
 DEFB 066, 062, 002, 124, 064, 064, 124, 066
 DEFB 066, 066, 066, 000, 000, 016, 000, 112
 DEFB 016, 016, 124, 000, 000, 002, 000, 002
 DEFB 002, 002, 066, 060, 000, 000, 066, 068
 DEFB 120, 068, 066, 000, 048, 016, 016, 016
 DEFB 016, 016, 056, 000, 000, 000, 068, 170
 DEFB 146, 130, 130, 000, 000, 000, 076, 082
 DEFB 098, 066, 066, 000, 000, 000, 060, 066
 DEFB 066, 066, 060, 000, 000, 000, 124, 066
 DEFB 066, 124, 064, 064, 000, 000, 062, 066
 DEFB 066, 062, 002, 002, 000, 000, 124, 066
 DEFB 064, 064, 064, 000, 000, 000, 062, 064
 DEFB 060, 002, 124, 000, 032, 032, 112, 032
 DEFB 032, 034, 028, 000, 000, 000, 066, 066
 DEFB 070, 074, 050, 000, 000, 000, 066, 066
 DEFB 066, 036, 024, 000, 000, 000, 130, 130
 DEFB 146, 170, 068, 000, 000, 000, 066, 036
 DEFB 024, 036, 066, 000, 000, 000, 066, 066
 DEFB 062, 002, 002, 124, 000, 000, 124, 008
 DEFB 016, 032, 124, 000, 014, 024, 024, 112
 DEFB 024, 024, 014, 000, 000, 024, 024, 024
 DEFB 024, 024, 024, 000, 112, 024, 024, 014
 DEFB 024, 024, 112, 000, 118, 220, 000, 000
 DEFB 000, 000, 000, 000, 060, 102, 102, 102
 DEFB 102, 102, 060, 000, 060, 098, 248, 096
 DEFB 248, 098, 060, 000, 000, 000, 000, 255
 DEFB 000, 000, 000, 000, 000, 124, 254, 254
 DEFB 124, 000, 000, 000, 000, 124, 198, 198
 DEFB 124, 000, 000, 000, 000, 000, 124, 124
 DEFB 124, 000, 000, 000, 000, 096, 120, 126
 DEFB 120, 096, 000, 000, 000, 063, 063, 060
 DEFB 063, 063, 000, 255, 003, 243, 243, 243
 DEFB 243, 243, 003, 255, 000, 063, 051, 060
 DEFB 051, 063, 000, 255, 003, 243, 051, 243
 DEFB 051, 243, 003, 255, 192, 031, 063, 060
 DEFB 063, 031, 192, 255, 007, 227, 243, 243
 DEFB 243, 227, 015, 255, 192, 031, 056, 048
 DEFB 056, 031, 192, 255, 007, 227, 115, 051
 DEFB 115, 227, 015, 255, 000, 000, 255, 000
 DEFB 231, 102, 102, 102, 102, 102, 231, 000
 DEFB 231, 102, 102, 102, 000, 000, 000, 024
 DEFB 000, 000, 000, 000, 024, 024, 024, 024
 DEFB 000, 000, 000, 000, 000, 000, 000, 031
 DEFB 000, 000, 000, 000, 024, 024, 024, 015
 DEFB 000, 000, 000, 000, 000, 000, 000, 024
 DEFB 024, 024, 024, 024, 024, 024, 024, 024
 DEFB 024, 024, 024, 024, 000, 000, 000, 015
 DEFB 024, 024, 024, 024, 024, 024, 024, 031
 DEFB 024, 024, 024, 024, 000, 000, 000, 248
 DEFB 000, 000, 000, 000, 024, 024, 024, 240
 DEFB 000, 000, 000, 000, 000, 000, 000, 255
 DEFB 000, 000, 000, 000, 024, 024, 024, 255
 DEFB 000, 000, 000, 000, 000, 000, 024, 060
 DEFB 126, 255, 000, 000, 000, 000, 255, 126
 DEFB 060, 024, 000, 000, 000, 006, 030, 126
 DEFB 030, 006, 000, 000, 000, 096, 120, 126
 DEFB 120, 096, 000, 000, 062, 102, 110, 054
 DEFB 000, 126, 000, 000, 060, 102, 102, 060
 DEFB 000, 126, 000, 000, 060, 102, 102, 060
 DEFB 000, 000, 000, 000, 060, 102, 096, 240
 DEFB 096, 102, 252, 000, 124, 130, 186, 162
 DEFB 186, 130, 124, 000, 126, 244, 244, 116
 DEFB 020, 020, 062, 000, 028, 048, 060, 102
 DEFB 060, 012, 056, 000, 195, 060, 102, 102
 DEFB 060, 195, 000, 000, 064, 192, 068, 076
 DEFB 020, 030, 004, 000, 064, 192, 076, 082
 DEFB 004, 008, 030, 000, 192, 064, 196, 076
 DEFB 212, 030, 004, 000, 000, 000, 102, 204
 DEFB 102, 000, 000, 000, 000, 000, 204, 102
 DEFB 204, 000, 000, 000, 000, 000, 000, 126
 DEFB 006, 000, 000, 000, 024, 000, 024, 048
 DEFB 102, 102, 060, 000, 024, 000, 024, 024
 DEFB 060, 060, 024, 000, 024, 024, 024, 000
 DEFB 024, 024, 024, 000, 024, 060, 102, 096
 DEFB 102, 060, 024, 000, 102, 000, 000, 000
 DEFB 000, 000, 000, 000, 012, 024, 000, 000
 DEFB 000, 000, 000, 000, 000, 126, 000, 000
 DEFB 000, 000, 000, 000, 000, 024, 126, 024
 DEFB 000, 126, 000, 000, 060, 102, 024, 096
 DEFB 126, 000, 000, 000, 060, 102, 012, 102
 DEFB 060, 000, 000, 000, 000, 000, 102, 102
 DEFB 102, 124, 096, 096, 000, 000, 000, 024
 DEFB 000, 000, 000, 000, 060, 102, 108, 102
 DEFB 102, 124, 096, 000, 000, 000, 000, 000
 DEFB 000, 000, 024, 048, 012, 028, 060, 012
 DEFB 012, 000, 000, 000, 102, 102, 060, 024
 DEFB 060, 024, 024, 000, 124, 130, 178, 178
 DEFB 170, 130, 124, 000, 234, 078, 074, 074
 DEFB 074, 000, 000, 000, 012, 024, 060, 102
 DEFB 126, 102, 102, 000, 012, 024, 126, 096
 DEFB 120, 096, 126, 000, 012, 024, 126, 024
 DEFB 024, 024, 126, 000, 012, 024, 060, 102
 DEFB 102, 102, 060, 000, 012, 024, 102, 102
 DEFB 102, 102, 060, 000, 024, 036, 060, 102
 DEFB 126, 102, 102, 000, 024, 036, 126, 096
 DEFB 120, 096, 126, 000, 024, 036, 126, 024
 DEFB 024, 024, 126, 000, 024, 036, 060, 102
 DEFB 102, 102, 060, 000, 024, 036, 102, 102
 DEFB 102, 102, 060, 000, 048, 024, 060, 102
 DEFB 126, 102, 102, 000, 048, 024, 126, 096
 DEFB 120, 096, 126, 000, 048, 024, 126, 024
 DEFB 024, 024, 126, 000, 048, 024, 060, 102
 DEFB 102, 102, 060, 000, 048, 024, 102, 102
 DEFB 102, 102, 060, 000, 124, 102, 102, 246
 DEFB 102, 102, 124, 000, 102, 000, 060, 102
 DEFB 126, 102, 102, 000, 102, 000, 126, 096
 DEFB 120, 096, 126, 000, 102, 000, 126, 024
 DEFB 024, 024, 126, 000, 102, 000, 060, 102
 DEFB 102, 102, 060, 000, 102, 000, 102, 102
 DEFB 102, 102, 060, 000, 060, 102, 096, 096
 DEFB 096, 102, 060, 024, 238, 168, 168, 238
 DEFB 168, 168, 174, 000, 024, 000, 060, 102
 DEFB 126, 102, 102, 000, 002, 052, 074, 082
 DEFB 034, 092, 128, 000, 126, 000, 102, 118
 DEFB 110, 102, 102, 000, 054, 108, 060, 102
 DEFB 126, 102, 102, 000, 054, 108, 060, 102
 DEFB 102, 102, 060, 000, 000, 000, 102, 024
 DEFB 102, 000, 000, 000, 006, 024, 102, 102
 DEFB 060, 024, 024, 000, 120, 096, 124, 102
 DEFB 124, 096, 120, 000, 054, 108, 006, 060
 DEFB 102, 102, 060, 000, 012, 024, 060, 102
 DEFB 102, 110, 054, 000, 012, 024, 060, 102
 DEFB 126, 096, 062, 000, 012, 024, 120, 024
 DEFB 024, 024, 126, 000, 012, 024, 060, 102
 DEFB 102, 102, 060, 000, 012, 024, 102, 102
 DEFB 102, 110, 054, 000, 024, 036, 060, 102
 DEFB 102, 110, 054, 000, 024, 036, 060, 102
 DEFB 126, 096, 062, 000, 024, 036, 120, 024
 DEFB 024, 024, 126, 000, 024, 036, 060, 102
 DEFB 102, 102, 060, 000, 024, 036, 102, 102
 DEFB 102, 110, 054, 000, 048, 024, 060, 102
 DEFB 102, 110, 054, 000, 048, 024, 060, 102
 DEFB 126, 096, 062, 000, 048, 024, 120, 024
 DEFB 024, 024, 126, 000, 048, 024, 060, 102
 DEFB 102, 102, 060, 000, 048, 024, 102, 102
 DEFB 102, 110, 054, 000, 102, 000, 102, 102
 DEFB 062, 006, 006, 124, 102, 000, 060, 102
 DEFB 102, 110, 054, 000, 102, 000, 060, 102
 DEFB 126, 096, 062, 000, 102, 000, 120, 024
 DEFB 024, 024, 126, 000, 102, 000, 060, 102
 DEFB 102, 102, 060, 000, 102, 000, 102, 102
 DEFB 102, 110, 054, 000, 000, 000, 060, 102
 DEFB 096, 102, 060, 024, 000, 000, 238, 170
 DEFB 174, 232, 046, 000, 024, 000, 060, 102
 DEFB 102, 110, 054, 000, 002, 004, 052, 074
 DEFB 082, 044, 064, 000, 126, 000, 108, 118
 DEFB 102, 102, 102, 000, 054, 108, 060, 102
 DEFB 102, 110, 054, 000, 054, 108, 060, 102
 DEFB 102, 102, 060, 000, 000, 024, 000, 126
 DEFB 000, 024, 000, 000, 012, 024, 102, 102
 DEFB 062, 006, 006, 124, 096, 096, 124, 102
 DEFB 124, 096, 120, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000
#endasm

/* PCW computer bitmap
   -------------------
*/
#asm
pcw_bmp:
 DEFB 015, 000  ; Width in columns
 DEFB 009, 000  ; Height in rows
 DEFB 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 001, 006, 063, 063, 000, 000, 010, 106
 DEFB 093, 214, 255, 255, 000, 000, 170, 171
 DEFB 218, 173, 255, 255, 000, 000, 170, 085
 DEFB 182, 170, 255, 255, 000, 000, 170, 085
 DEFB 213, 173, 255, 255, 000, 000, 170, 085
 DEFB 170, 091, 255, 255, 000, 000, 171, 085
 DEFB 181, 085, 255, 255, 000, 000, 109, 085
 DEFB 085, 086, 255, 255, 000, 000, 221, 085
 DEFB 085, 170, 255, 255, 000, 000, 187, 074
 DEFB 106, 173, 255, 255, 000, 000, 253, 005
 DEFB 245, 086, 255, 255, 000, 016, 214, 041
 DEFB 085, 219, 255, 255, 000, 128, 088, 064
 DEFB 080, 160, 240, 208, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 063, 063, 062, 031
 DEFB 031, 031, 015, 015, 255, 181, 225, 138
 DEFB 096, 166, 201, 162, 255, 086, 041, 146
 DEFB 073, 170, 037, 148, 255, 221, 034, 085
 DEFB 074, 170, 085, 170, 255, 221, 034, 091
 DEFB 170, 170, 085, 170, 255, 221, 086, 085
 DEFB 170, 181, 086, 170, 255, 247, 169, 182
 DEFB 170, 085, 218, 170, 255, 118, 073, 170
 DEFB 170, 085, 170, 170, 255, 219, 036, 146
 DEFB 169, 085, 164, 085, 255, 111, 159, 071
 DEFB 047, 087, 135, 087, 255, 255, 240, 240
 DEFB 248, 240, 240, 184, 253, 223, 033, 001
 DEFB 001, 000, 001, 001, 240, 240, 176, 240
 DEFB 208, 240, 240, 176, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 015, 007, 007, 007
 DEFB 007, 003, 003, 003, 228, 194, 228, 242
 DEFB 209, 242, 233, 248, 082, 170, 149, 073
 DEFB 085, 073, 037, 148, 170, 149, 085, 085
 DEFB 042, 085, 037, 169, 182, 085, 085, 085
 DEFB 170, 085, 085, 042, 213, 086, 090, 085
 DEFB 170, 085, 085, 170, 085, 171, 218, 085
 DEFB 170, 085, 086, 170, 085, 106, 170, 085
 DEFB 170, 085, 170, 170, 082, 170, 146, 084
 DEFB 170, 073, 165, 084, 035, 151, 171, 147
 DEFB 075, 083, 043, 135, 240, 248, 240, 248
 DEFB 240, 248, 216, 255, 001, 001, 001, 001
 DEFB 001, 001, 001, 255, 240, 112, 208, 240
 DEFB 112, 224, 176, 240, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 001, 001, 001, 000
 DEFB 000, 000, 000, 000, 233, 248, 244, 220
 DEFB 250, 122, 125, 109, 082, 010, 169, 068
 DEFB 042, 001, 042, 001, 149, 084, 074, 165
 DEFB 018, 085, 073, 036, 085, 164, 149, 085
 DEFB 085, 074, 041, 165, 085, 170, 085, 085
 DEFB 085, 170, 085, 042, 085, 170, 085, 085
 DEFB 085, 170, 085, 170, 085, 170, 084, 082
 DEFB 085, 170, 085, 146, 082, 149, 169, 148
 DEFB 074, 081, 074, 036, 179, 011, 083, 167
 DEFB 019, 075, 163, 023, 240, 184, 248, 248
 DEFB 104, 248, 248, 176, 001, 001, 001, 001
 DEFB 001, 001, 001, 001, 096, 240, 160, 240
 DEFB 064, 240, 160, 224, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 061, 062, 055, 029
 DEFB 031, 000, 000, 000, 136, 245, 254, 183
 DEFB 127, 169, 005, 010, 146, 000, 255, 255
 DEFB 239, 125, 175, 080, 148, 074, 032, 255
 DEFB 255, 255, 183, 090, 165, 149, 032, 255
 DEFB 255, 255, 224, 191, 084, 085, 000, 255
 DEFB 255, 255, 002, 123, 169, 074, 032, 221
 DEFB 255, 247, 047, 174, 082, 072, 130, 125
 DEFB 223, 251, 239, 255, 163, 011, 135, 255
 DEFB 191, 246, 127, 219, 248, 176, 248, 105
 DEFB 255, 214, 255, 085, 001, 001, 001, 073
 DEFB 255, 181, 239, 091, 096, 192, 160, 224
 DEFB 064, 192, 064, 128, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 001, 002, 000, 000, 000, 020
 DEFB 090, 109, 082, 168, 000, 000, 000, 000
 DEFB 146, 233, 183, 008, 017, 045, 119, 000
 DEFB 148, 033, 222, 107, 043, 173, 246, 021
 DEFB 064, 010, 208, 127, 171, 085, 238, 191
 DEFB 000, 160, 005, 251, 082, 173, 218, 255
 DEFB 010, 000, 250, 183, 170, 173, 246, 173
 DEFB 255, 000, 128, 127, 219, 106, 182, 219
 DEFB 255, 000, 000, 234, 084, 171, 218, 109
 DEFB 255, 074, 000, 000, 164, 085, 170, 181
 DEFB 255, 173, 000, 000, 170, 033, 149, 106
 DEFB 182, 255, 000, 000, 072, 032, 080, 144
 DEFB 160, 240, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 001, 002, 010, 021, 011, 021, 042, 169
 DEFB 084, 165, 168, 085, 069, 040, 066, 050
 DEFB 137, 082, 033, 169, 169, 004, 181, 000
 DEFB 090, 017, 073, 036, 008, 212, 010, 170
 DEFB 132, 041, 074, 148, 133, 164, 086, 161
 DEFB 077, 066, 085, 164, 190, 066, 082, 041
 DEFB 082, 041, 082, 037, 255, 149, 144, 045
 DEFB 128, 092, 132, 040, 245, 095, 169, 068
 DEFB 150, 161, 149, 164, 095, 245, 095, 032
 DEFB 179, 009, 048, 037, 254, 107, 189, 083
 DEFB 008, 170, 072, 034, 160, 223, 118, 091
 DEFB 132, 101, 130, 169, 000, 237, 182, 109
 DEFB 182, 001, 056, 001, 000, 032, 223, 234
 DEFB 182, 011, 165, 045, 000, 000, 160, 160
 DEFB 224, 064, 160, 064, 042, 031, 000, 000
 DEFB 000, 000, 000, 000, 170, 222, 107, 001
 DEFB 000, 000, 000, 000, 173, 214, 123, 093
 DEFB 000, 000, 000, 000, 155, 237, 086, 255
 DEFB 085, 000, 000, 000, 073, 118, 219, 189
 DEFB 239, 043, 000, 000, 010, 233, 095, 234
 DEFB 123, 175, 042, 000, 080, 042, 213, 182
 DEFB 109, 219, 255, 000, 138, 148, 074, 245
 DEFB 086, 187, 237, 183, 067, 168, 018, 136
 DEFB 181, 093, 213, 126, 040, 146, 149, 066
 DEFB 081, 086, 181, 218, 144, 070, 065, 077
 DEFB 016, 148, 182, 218, 004, 168, 020, 033
 DEFB 073, 144, 231, 148, 106, 144, 069, 033
 DEFB 074, 004, 146, 156, 138, 087, 021, 042
 DEFB 053, 086, 090, 170, 128, 000, 128, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000, 171, 000, 000, 000
 DEFB 000, 000, 000, 000, 239, 021, 000, 000
 DEFB 000, 000, 000, 000, 182, 221, 002, 000
 DEFB 000, 000, 000, 000, 235, 189, 215, 000
 DEFB 000, 000, 000, 000, 106, 171, 253, 043
 DEFB 000, 000, 000, 000, 212, 084, 176, 120
 DEFB 000, 000, 000, 000, 000, 000, 000, 000
 DEFB 000, 000, 000, 000
#endasm

