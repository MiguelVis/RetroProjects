/*	2048.c

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

	To compile with MESCC:

		cc 2048
		ccopt 2048
		zsm 2048
		hextocom 2048
		gencom 2048.com xpcw.rsx

	Usage:

		2048

	Dependencies:

		XPCW library.

	Notes:

		Change version number and release date in 2048.h.

	Revisions:

		14 Oct 2016 : Start of coding.
		17 Oct 2016 : Design of the screen layout.
		18 Oct 2016 : New boxes appear randomly.
		19 Oct 2016 : Load and save hi-scores. Draw goal box number in bold.
*/

// Defines for MESCC libraries
// ---------------------------

//#define CC_STDIO        // Support for stdin, stdout, stderr.
//#define CC_REDIR        // Support for command line redirection - needs CC_STDIO.

#define CC_CONIO_BIOS   // Console I/O through BIOS instead of BDOS.

//#define CC_FCX          // Support for user number in filenames.
//#define CC_FCX_DIR      // Support for named user numbers - needs CC_FCX and DirToDrvUsr().

//#define CC_FOPEN_A	// Enable modes "a" and "ab" for fopen.
#define CC_FREAD	// Include fread().
#define CC_FWRITE	// Include fwrite().
//#define CC_FGETS	// Include fgets().

//#define CC_NO_MUL       // Exclude support for MULTIPLICATION.
//#define CC_NO_DIV       // Exclude support for DIVISION and MODULUS.
//#define CC_NO_SWITCH    // Exclude support for SWITCH.

//#define CC_NO_ARGS      // Exclude support for ARGC and ARGV.

// Standard MESCC library
// ----------------------

#include <mescc.h>

// Standard MESCC libraries
// ------------------------

//#include <alloc.h>
//#include <atexit.h>
//#include <bsearch.h>
//#include <clock.h>
#include <conio.h>
//#include <cpm.h>
#include <ctype.h>
#include <fileio.h>
//#include <fprintf.h>
#include <mem.h>
//#include <printf.h>
//#include <qsort.h>
#include <rand.h>
//#include <redir.h>
//#include <setjmp.h>
#include <sprintf.h>
#include <string.h>
//#include <xprintf.h>
//#include <z80.h>

// XPCW libraries
// --------------

#include "xpcw.h"
#include "xtext.h"
#include "xbitmap.h"

// Project libraries
// -----------------

#include "2048.h"       // Defs.
#include "2048_scr.c"   // Screen commands
#include "2048_img.c"   // Images

// Globals
// -------

int board[BOARD_SIZE];     // Game board boxes
int goal;                  // Goal to reach
int level;                 // Goal level: 0..6
int score;                 // Score points
int hi_score;              // Hi-score
int hi_scores[7];          // Hi-scores for all levels
int hi_read;               // Set to non-zero if hi-scores have been read

char message[91];          // For use with sprintf and customized messages

extern BYTE font[];        // Font bitmap
extern BYTE img_2048[];    // Logo image
extern BYTE img_empty[];   // Empty box image
extern BYTE img_number[];  // Numbered box image

// Program entry
// -------------

main()
{
	FILE *fp;

	// Check if the XPCW RSX is in memory
	if(!HelloRsx())
	{
		error("The XPCW RSX is not in memory");
	}

	// Clear screen and hide cursor
	ClrScr();
	HideCursor();

	// Setup XPCW
	SetFont(font);
	SetTextAtt(XA_NORMAL);

	// Draw logo
	DrawSmallBitmap(SCORE_ROW, SCORE_COL + 11, 2, 10, img_2048);

	// Draw scores
	SetTextAtt(XA_REV);
	SetTextRC(SCORE_ROW, SCORE_COL);     PrintStr("  POINTS  ");
	SetTextRC(SCORE_ROW + 1, SCORE_COL); PrintStr("          ");
	SetTextRC(HISCO_ROW, HISCO_COL);     PrintStr("   BEST   ");
	SetTextRC(HISCO_ROW + 1, HISCO_COL); PrintStr("          ");
	SetTextAtt(XA_NORMAL);

	// Main loop
	do
	{
		// Draw fake scores for the menu
		score = hi_score = 2048;
		PrintScore();
		PrintHiScore();

		// Draw board background
		DrawBackground();

		// Show version, copyright, etc.
		CenterText(HOWTO_ROW, VERSION);
		CenterText(HOWTO_ROW + 1, COPYRIGHT);
		CenterText(HOWTO_ROW + 2, WEBSITE);

		// Load scores from file (if any)
		if(!hi_read)
		{
			CenterText(JOIN_ROW, "Reading scores...");

			if((fp = fopen(SCORES_FNAME, "rb")))
			{
				fread(hi_scores, 2, 7, fp);

				fclose(fp);
			}

			CenterText(JOIN_ROW, NULL);

			hi_read = 1;
		}

		// Select goal
		if((goal = SelectGoal()))
		{
			// Reset score
			score = 0;
			PrintScore();

			// Print best score
			hi_score = hi_scores[level];
			PrintHiScore();

			// Draw background board
			DrawBackground();

			// Play level and check best score
			if(PlayGoal() && score > hi_score)
			{
				CenterText(JOIN_ROW, "Saving best scores...");

				hi_scores[level] = score;

				if((fp = fopen(SCORES_FNAME, "wb")))
				{
					fwrite(hi_scores, 2, 7, fp);

					fclose(fp);
				}

				CenterText(JOIN_ROW, NULL);
			}
		}
	} while(goal);

	// Clear screen and show cursor
	ClrScr();
	ShowCursor();

	// Quit game
}

// Select goal (level)
// -------------------

SelectGoal()
{
	// Show menu
	SetTextAtt(XA_REV);

	CenterText(BOARD_ROW, "Select a goal:");
	
	CenterText(BOARD_ROW +  2, "A :    1 2 8");
	CenterText(BOARD_ROW +  3, "B :    2 5 6");
	CenterText(BOARD_ROW +  4, "C :    5 1 2");
	CenterText(BOARD_ROW +  5, "D :  1 0 2 4");
	CenterText(BOARD_ROW +  6, "E :  2 0 4 8");
	CenterText(BOARD_ROW +  7, "F :  4 0 9 6");
	CenterText(BOARD_ROW +  8, "G :  8 1 9 2");

	CenterText(BOARD_ROW + 11, "Press EXIT to quit the game");

	SetTextAtt(XA_NORMAL);
	
	// Select option
	while(1)
	{
		switch(tolower(getch()))
		{
			case 'a'   : level = 0; return  128;
			case 'b'   : level = 1; return  256;
			case 'c'   : level = 2; return  512;
			case 'd'   : level = 3; return 1024;
			case 'e'   : level = 4; return 2048;
			case 'f'   : level = 5; return 4096;
			case 'g'   : level = 6; return 8192;

			case K_ESC : return 0;
		}
	}
}

// Play level
// ----------
// return: 1 if goal reached, else 0

PlayGoal()
{
	int stop_play, goal_reached;
	char *stop_reason;
	WORD *clock;
	int i, r, c, k;
	
	// Reset board
	ResetBoard();
	
	// Draw empty board
	DrawDown();

	// Clear messages
	CenterText(JOIN_ROW, NULL);

	CenterText(HOWTO_ROW, NULL);
	CenterText(HOWTO_ROW + 1, NULL);
	CenterText(HOWTO_ROW + 2, NULL);

	// Show the goal
	sprintf(message, "Join the numbers and get to the %d tile!", goal);
	CenterText(JOIN_ROW, message);

	// Show how to play
	CenterText(HOWTO_ROW, "HOW TO PLAY: Use your arrow keys to move the tiles. When");
	CenterText(HOWTO_ROW + 1, "two tiles with the same number touch, they merge into one!");
	CenterText(HOWTO_ROW + 2, "Press EXIT to return to the menu.");

	// Put some boxes randomly
	srand(*(clock = 0xFBF7)); // CP/M Plus SCB -> MM:SS

	for(i = 0; i < 4; ++i)
	{
		k = GetRandBox();

		SetBoxValue(k / BOARD_ROWS, k % BOARD_COLS, GetRandValue());
	}
	
	// Redraw board with the initial boxes
	DrawDown();
	
	// Start playing
	stop_play = goal_reached = 0;

	do
	{
		// Execute commands from the player
		switch(getch())
		{
			case K_UP :
				JoinUp(); MergeUp(); JoinUp(); DrawUp();
				PrintScore();
				break;
			case K_DOWN :
				JoinDown(); MergeDown(); JoinDown(); DrawDown();
				PrintScore();
				break;
			case K_LEFT :
				JoinLeft(); MergeLeft(); JoinLeft(); DrawLeft();
				PrintScore();
				break;
			case K_RIGHT :
				JoinRight(); MergeRight(); JoinRight(); DrawRight();
				PrintScore();
				break;
			case K_ESC :
				stop_play = 1;
				stop_reason = "Game stopped by you!";
				break;
		}
		
		// Check if the play is finished
		if(!stop_play)
		{
			if((goal_reached = Is2048(goal)))
			{
				// Goal reached
				sprintf(message, "You got %d!", goal);

				stop_reason = message;
				stop_play   = 1;
			}
			else if(IsFull())
			{
				// The board is full
				stop_reason = "The board is full!";
				stop_play   = 1;
			}
			else
			{
				// Add a box randomly
				k = GetRandBox();

				r = k / BOARD_ROWS;
				c = k % BOARD_COLS;

				SetBoxValue(r, c, GetRandValue());

				// Wait a little
				for(i = 0; i < 1024; ++i)
					;

				DrawBox(r, c);
			}
		}
	} while(!stop_play);

	// Clear messages
	CenterText(JOIN_ROW, NULL);

	CenterText(HOWTO_ROW, NULL);
	CenterText(HOWTO_ROW + 1, NULL);
	CenterText(HOWTO_ROW + 2, NULL);

	// Print the reason
	CenterText(JOIN_ROW, stop_reason);

	// Wait a key press
	CenterText(HOWTO_ROW, "Press EXIT to continue");

	while(getch() != K_ESC)
		;

	// Clear messages
	CenterText(JOIN_ROW, NULL);

	CenterText(HOWTO_ROW, NULL);

	// Return
	return goal_reached;
}

// Reset board
// -----------

ResetBoard()
{
	int row, col;

	for(row = 0; row < BOARD_ROWS; ++row)
	{
		for(col = 0; col < BOARD_COLS; ++col)
		{
			SetBoxValue(row, col, 0);
		}
	}
}

// Check if board is full
// ----------------------
// return: 1 if full, else 0

IsFull()
{
	int row, col;

	for(row = 0; row < BOARD_ROWS; ++row)
	{
		for(col = 0; col < BOARD_COLS; ++col)
		{
			if(!GetBoxValue(row, col))
			{
				// Not full
				return 0;
			}
		}
	}
	
	// Is full
	return 1;
}

// Check if the goal has been reached
// ----------------------------------
// return: 1 if reached, else 0
Is2048(goal)
int goal;
{
	int row, col;

	for(row = 0; row < BOARD_ROWS; ++row)
	{
		for(col = 0; col < BOARD_COLS; ++col)
		{
			if(GetBoxValue(row, col) == goal)
			{
				// Goal reached
				return 1;
			}
		}
	}
	
	// Goal not reached yet
	return 0;
}

// Get box value
// -------------

GetBoxValue(board_row, board_col)
int board_row, board_col;
{
	return board[board_row * BOARD_COLS + board_col];
}

// Set box value
// -------------

SetBoxValue(board_row, board_col, value)
int board_row, board_col, value;
{
	board[board_row * BOARD_COLS + board_col] = value;
}

// Merge boxes with same value: from down to up
// --------------------------------------------

MergeUp()
{
	int row, col, k;

	for(col = 0; col < BOARD_COLS; ++col)
	{
		for(row = 0; row < BOARD_ROWS - 1; ++row)
		{
			if((k = GetBoxValue(row, col)))
			{
				if(k == GetBoxValue(row + 1, col))
				{
					SetBoxValue(row, col, (k += k));
					SetBoxValue(row + 1, col, 0);

					score += k;

					++row;
				}
			}
		}
	}
}

// Merge boxes with same value: from up to down
// --------------------------------------------

MergeDown()
{
	int row, col, k;

	for(col = 0; col < BOARD_COLS; ++col)
	{
		for(row = BOARD_ROWS - 1; row > 0; --row)
		{
			if((k = GetBoxValue(row, col)))
			{
				if(k == GetBoxValue(row - 1, col))
				{
					SetBoxValue(row, col, (k += k));
					SetBoxValue(row - 1, col, 0);

					score += k;

					--row;
				}
			}
		}
	}
}

// Merge boxes with same value: from right to left
// -----------------------------------------------

MergeLeft()
{
	int row, col, k;

	for(row = 0; row < BOARD_ROWS; ++row)
	{
		for(col = 0; col < BOARD_COLS - 1; ++col)
		{
			if((k = GetBoxValue(row, col)))
			{
				if(k == GetBoxValue(row, col + 1))
				{
					SetBoxValue(row, col, (k += k));
					SetBoxValue(row, col + 1, 0);

					score += k;

					++col;
				}
			}
		}
	}
}

// Merge boxes with same value: from left to right
// -----------------------------------------------

MergeRight()
{
	int row, col, k;

	for(row = 0; row < BOARD_ROWS; ++row)
	{
		for(col = BOARD_COLS - 1; col > 0; --col)
		{
			if((k = GetBoxValue(row, col)))
			{
				if(k == GetBoxValue(row, col - 1))
				{
					SetBoxValue(row, col, (k += k));
					SetBoxValue(row, col - 1, 0);

					score += k;

					--col;
				}
			}
		}
	}
}

// Join boxes: from down to up
// ---------------------------

JoinUp()
{
	int row, col, pos, k;
	
	for(col = 0; col < BOARD_COLS; ++col)
	{
		for(row = 0; row < BOARD_ROWS - 1; ++row)
		{
			if(!GetBoxValue(row, col))
			{
				for(pos = row + 1; pos < BOARD_ROWS; ++pos)
				{
					if((k = GetBoxValue(pos, col)))
					{
						SetBoxValue(row, col, k);
						SetBoxValue(pos, col, 0);

						break;
					}
				}
			}
		}
	}
}

// Join boxes: from up to down
// ---------------------------

JoinDown()
{
	int row, col, pos, k;
	
	for(col = 0; col < BOARD_COLS; ++col)
	{
		for(row = BOARD_ROWS - 1; row > 0; --row)
		{
			if(!GetBoxValue(row, col))
			{
				for(pos = row - 1; pos >= 0; --pos)
				{
					if((k = GetBoxValue(pos, col)))
					{
						SetBoxValue(row, col, k);
						SetBoxValue(pos, col, 0);

						break;
					}
				}
			}
		}
	}
}

// Join boxes: from right to left
// ------------------------------

JoinLeft()
{
	int row, col, pos, k;
	
	for(row = 0; row < BOARD_ROWS; ++row)
	{
		for(col = 0; col < BOARD_COLS - 1; ++col)
		{
			if(!GetBoxValue(row, col))
			{
				for(pos = col + 1; pos < BOARD_COLS; ++pos)
				{
					if((k = GetBoxValue(row, pos)))
					{
						SetBoxValue(row, col, k);
						SetBoxValue(row, pos, 0);

						break;
					}
				}
			}
		}
	}
}

// Join boxes: from left to right
// ------------------------------

JoinRight()
{
	int row, col, pos, k;
	
	for(row = 0; row < BOARD_ROWS; ++row)
	{
		for(col = BOARD_COLS - 1; col > 0; --col)
		{
			if(!GetBoxValue(row, col))
			{
				for(pos = col - 1; pos >= 0; --pos)
				{
					if((k = GetBoxValue(row, pos)))
					{
						SetBoxValue(row, col, k);
						SetBoxValue(row, pos, 0);

						break;
					}
				}
			}
		}
	}
}

// Draw board: from down to up
// ---------------------------

DrawUp()
{
	int row, col;
	
	for(col = 0; col < BOARD_COLS; ++col)
	{
		for(row = BOARD_ROWS - 1; row >= 0; --row)
		{
			DrawBox(row, col);
		}
	}
}

// Draw board: from up to down
// ---------------------------

DrawDown()
{
	int row, col;
	
	for(col = 0; col < BOARD_COLS; ++col)
	{
		for(row = 0; row < BOARD_ROWS; ++row)
		{
			DrawBox(row, col);
		}
	}
}

// Draw board: from right to left
// ------------------------------

DrawLeft()
{
	int row, col;
	
	for(row = 0; row < BOARD_ROWS; ++row)
	{
		for(col = BOARD_COLS - 1; col >= 0; --col)
		{
			DrawBox(row, col);
		}
	}
}

// Draw board: from left to right
// ------------------------------

DrawRight()
{
	int row, col;
	
	for(row = 0; row < BOARD_ROWS; ++row)
	{
		for(col = 0; col < BOARD_COLS; ++col)
		{
			DrawBox(row, col);
		}
	}
}

// Draw box
// --------

DrawBox(board_row, board_col)
int board_row, board_col;
{
	int row, col, val;

	// Get screen position
	row = BOARD_ROW + board_row * BOX_ROWS;
	col = BOARD_COL + board_col * BOX_COLS;
	
	// Draw image according to box value
	if(val = GetBoxValue(board_row, board_col))
	{
		// Draw numbered box
		DrawNumber(img_number + 56, val, val == goal);
		DrawSmallBitmap(row, col, BOX_ROWS, BOX_COLS, img_number);
	}
	else
	{
		// Draw empty box
		DrawSmallBitmap(row, col, BOX_ROWS, BOX_COLS, img_empty);
	}
}

// Draw centered number (for boxes and scores); 4 decimal digits max.
// ------------------------------------------------------------------

DrawNumber(buf, val, bold)
BYTE *buf; int val, bold;
{
	char str[5]; // 2048 + zero
	int i;
	
	// Reset buffer
	memset(buf, 0, 32);
		
	// Convert number to string
	sprintf(str, "%d", val);
		
	// Draw text and shift it if it's necessary to center it
	switch(strlen(str))
	{
		case 1 : // 1 digit
			DrNumber(buf, 2, str, bold);
			DrShift(buf, 2);
			break;
		case 2 : // 2 digits
			DrNumber(buf, 1, str, bold);
			break;
		case 3 : // 3 digits
			DrNumber(buf, 1, str, bold);
			DrShift(buf, 1);
			DrShift(buf, 2);
			DrShift(buf, 3);
			break;
		default: // 4 digits
			DrNumber(buf, 0, str, bold);
			break;
	}
		
	// Invert bits to make reverse text effect
	for(i = 0; i < 32; ++i)
	{
		buf[i] ^= 0xFF;
	}
}	

// Helper for DrawNumber()
// -----------------------

DrNumber(buf, col, str, bold)
BYTE *buf; int col; char *str; int bold;
{
	BYTE *p, *f, i;
	
	// Position in buffer
	p = buf + col * 8;
	
	// Draw each digit in the buffer
	while(*str)
	{
		// Character position in font bitmap
		f = font + ((*str++) & 0xFF) * 8;

		if(bold)
		{
			// Bold effect
			for(i = 0; i < 8; ++i)
			{
				*p++ = *f | (*f << 1);
				++f;
			}
		}
		else
		{
			// Normal effect
			memcpy(p, f, 8);
			p += 8;
		}
	}
}

// Helper for DrawNumber()
// -----------------------

DrShift(buf, col)
BYTE *buf; int col; 
{
	int i, ix_a, ix_b;

	ix_b = col * 8;
	ix_a = ix_b - 8;
	
	for(i = 0; i < 8; ++i)
	{
		buf[ix_a + i] = (buf[ix_a + i] & 0xF0) | (buf[ix_b + i] >> 4);
		buf[ix_b + i] = (buf[ix_b + i] & 0x0F) << 4;
	}
}

// Draw background board
// ---------------------

DrawBackground()
{
	int i;

	for(i = 0; i < GAME_ROWS; ++i)
	{
		SetTextRC(GAME_ROW + i, GAME_COL); PrintChRpt(4, GAME_COLS);
	}
}

// Print score
// -----------

PrintScore()
{
	BYTE buf[32]; // 4 digits * 8 bytes

	DrawNumber(buf, score, 0);
	DrawSmallBitmap(SCORE_ROW + 1, SCORE_COL + 3, 1, 4, buf);
}

// Print best score
// ----------------

PrintHiScore()
{
	BYTE buf[32]; // 4 digits * 8 bytes

	DrawNumber(buf, hi_score, 0);
	DrawSmallBitmap(HISCO_ROW + 1, HISCO_COL + 3, 1, 4, buf);
}

// Get random box
// --------------

GetRandBox()
{
	int r, c, k, boxes[BOARD_SIZE];

	// Get array with empty boxes
	for(r = k = 0; r < BOARD_ROWS; ++r)
	{
		for(c = 0; c < BOARD_COLS; ++c)
		{
			if(!GetBoxValue(r, c))
			{
				boxes[k++] = r * BOARD_COLS + c;
			}
		}
	}

	// Return an empty box
	return boxes[rand() % k];
}

// Get random value for box
// ------------------------
// return: 2 or 4

GetRandValue()
{
	//return ((rand() % 2) + 1) * 2;
	return (((rand() % 100) / 50) + 1) * 2;
}

// Print error and exit
// --------------------

error(txt)
char *txt;
{
	putstr("2048: "); puts(txt);

	exit(1);
}

