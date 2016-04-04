/*	FIND THAT MINE!

	A MineSweeper clon for CP/M.

	Derived from the Amstrad PCW & Samaruc GUI version.

	(c) Floppy Software. Valencia, Spain.

	-----------------------------------------------------------------------

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

	-----------------------------------------------------------------------

	LEVELS:		1	2	3	NOTES

	ROWS:		8	8	8
	COLS:		8	12	16
	SQUARES:	64	96	128	ROWS * COLS
	MINES:		8	12	16	SQUARES / 8 (1 MINES EACH 8 SQUARES)

	11 Sep 2012 : v1.0 : Initial version for Amstrad PCW & Samaruc GUI.
	26 Jul 2013 : v1.2 : Adapted to Samaruc v1.5.
	16 Feb 2014 : v1.3 : Adapted to last changes in Samaruc.
	09 Jul 2014 : v1.4 : CP/M version.

	To compile in MESCC:

	cc minescpm -a-
*/

/* Do you want debug tools?
*/

#define DEBUG_MINES 0

/* MESCC libraries
*/

#include <mescc.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <printf.h>

/* DEFs for BOARD
*/

#define MAX_ROWS 8   /* Max. number of rows */
#define MAX_COLS 16  /* Max. number of columns */
#define MAX_SQ   128 /* Max. number of squares (MAX_ROWS * MAX_COLS) */
#define MAX_BUF  512 /* MAX_SQ * 4 */

/* DEFs for SPRITES
*/

#define SPR_MINE  '*' /* Mine */
#define SPR_FLAG  'P' /* Flag */
#define SPR_UNK   '?' /* Unknown */
#define SPR_ERR   'X' /* Flag error */
#define SPR_BLANK ' ' /* Blank */

/* Global variables
*/

int brd_rows, /* Board size in rows */
    brd_cols; /* Board size in columns */

WORD mines_row[MAX_ROWS], /* Map for mines */
     visib_row[MAX_ROWS], /* Map for visible squares */
     flags_row[MAX_ROWS], /* Map for squares with flag */
     count_row[MAX_ROWS]; /* Map for counters */

BYTE sq_buf[MAX_BUF];     /* Buffer to hold the above maps */

int row_off[8],           /* Array to help in calculation */
    col_off[8];	          /* of adjacent squares */

int gameover, /* NZ if game is over */
    dead,     /* NZ if player is dead */
    level,    /* Current level */
    viscnt,   /* Number of visible squares in board */
    flgcnt,   /* Number of flags in board */
    squares,  /* Number of squares in board */
    mines,    /* Number of mines in board */
    random;   /* Random number */

/* Code starts here
*/

main()
{
	/* Presentation
	*/

	printf("FIND THAT MINE!\n");
	printf("v1.4 for CP/M & Z80\n\n");
	printf("A minesweeper type game, derived from\n");
	printf("the Amstrad PCW & Samaruc version.\n\n");
	printf("(c) 2014 Floppy Software. Valencia (Spain).\n");
	printf("www.floppysoftware.vacau.com\n");
	printf("cpm-connections.blogspot.com.es\n");
	printf("floppysoftware@gmail.com\n\n");

	/* Setup
	*/

	Setup();

	/* Play game
	*/

	Play();

	/* The end
	*/

	printf("Bye, bye!\n");
}

/* Setup program
*/

Setup()
{

	int i; BYTE *p;

	/* Build array to help in calculation of adjacent squares
	*/

	row_off[0]=-1; col_off[0]= 0; /* Up */
	row_off[1]=-1; col_off[1]=+1; /* Upper Right */
	row_off[2]= 0; col_off[2]=+1; /* Right */
	row_off[3]=+1; col_off[3]=+1; /* Lower Right */
	row_off[4]=+1; col_off[4]= 0; /* Down */
	row_off[5]=+1; col_off[5]=-1; /* Lower Left */
	row_off[6]= 0; col_off[6]=-1; /* Left */
	row_off[7]=-1; col_off[7]=-1; /* Upper Left */

	/* Setup maps
	*/

	p = sq_buf;

	for(i = 0; i < MAX_ROWS; ++i)
	{
		mines_row[i]=p; p+=MAX_COLS;
		visib_row[i]=p; p+=MAX_COLS;
		flags_row[i]=p; p+=MAX_COLS;
		count_row[i]=p; p+=MAX_COLS;
	}

	/* Other
	*/

	random = 0;
}

/* Wrong input error msg.
*/

WrongInput()
{
	printf("Wrong input!\n\n");
}

/* Select and play a level
*/

Play()
{
	int run; char buf[2];

	run = 1;

	while(run)
	{
		printf("1 > Level 1 : 08 x 08 squares, 08 mines\n");
		printf("2 > Level 2 : 08 x 12 squares, 12 mines\n");
		printf("3 > Level 3 : 08 x 16 squares, 16 mines\n");
		printf("Q > Quit game\n\n");

		printf("? ");

		ReadLine(buf, 1); putchar('\n'); putchar('\n');

		switch(*buf)
		{
			case '1' : SetLevel(1); RunLevel(); break;
			case '2' : SetLevel(2); RunLevel(); break;
			case '3' : SetLevel(3); RunLevel(); break;
			case 'Q' : run = 0; break;
			default  : WrongInput(); break;
		}
	}
}

/* Setup level
*/

SetLevel(lev)
int lev;
{
	/* Setup level values
	*/

	level = lev; brd_rows = 8;

	switch(level)
	{
		case 1 : brd_cols =  8; break;
		case 2 : brd_cols = 12; break;
		case 3 : brd_cols = 16; break;
	}

	squares = brd_rows * brd_cols;

	mines = squares / 8;

	gameover = dead = viscnt = flgcnt = 0;

	/* Setup board
	*/

	ClrBoard();

	/* Setup counters
	*/

	ClrCounts();
}

/* Play level
*/

RunLevel()
{
	int row, col; char buf[4];

	while(!gameover)
	{
		ShowBoard();

		printf("rc  > Select (e.g. 0B)\n");
		printf("rcF > Set/remove flag (e.g. 3DF)\n");
		printf("Q   > Quit level\n\n");
		printf("? ");

		ReadLine(buf, 3); putchar('\n'); putchar('\n');

		if(buf[0] >= '0' && buf[0] < '0' + brd_cols)
		{
			row = buf[0] - '0';

			if(buf[1] >= 'A' && buf[1] < 'A' + brd_cols)
			{
				col = buf[1] - 'A';

				if(!buf[2])
					Choice(row, col);
				else if(buf[2] == 'F' && !buf[3])
					ChoiceFlag(row, col);
				else
					WrongInput();

				if(gameover)
					ShowBoard();
			}
			else
				WrongInput();
		}
		else if(buf[0] == 'Q' && !buf[1])
			gameover = 1;
		else
			WrongInput();
	}
}

/* Test if player has win the game
*/

TestIfWin()
{

	if(flgcnt == mines && flgcnt + viscnt == squares)
		gameover = 1;
}

/* Set or remove a flag in a square
*/

ChoiceFlag(row, col)
int row, col;
{
	if(!TstVis(row, col))
	{
		if(TstFlag(row, col))
		{
			SetFlag(row, col, 0); --flgcnt;
		}
		else
		{
			SetFlag(row, col, 1); ++flgcnt;

			TestIfWin();
		}
	}
}

/* Select a square
*/

Choice(row, col)
int row, col;
{
	if(!TstVis(row, col) && !TstFlag(row, col))
	{
		if(TstMine(row, col))
		{
			gameover = dead = 1; return;
		}

		/* Make visible this square.
		*/

		SetVis(row, col, 1); ++viscnt;

		/* Make visible the adjacent squares, if there are not mines around.
		*/

		if(!GetCount(row, col))
			VisAdj(row, col);

		TestIfWin();
	}
}

/* Get square counter
*/

GetCount(row, col)
int row, col;
{
	BYTE *p;

	p = count_row[row];

	return p[col];
}

/* Set square counter
*/

SetCount(row, col, val)
int row, col, val;
{
	BYTE *p;

	p=count_row[row];

	p[col]=val;
}

/* Return 1 if there is a mine in the square, else 0
*/

TstMine(row, col)
int row, col;
{
	BYTE *p;

	p=mines_row[row];

	return p[col] ? 1 : 0;
}

/* Set/remove a mine in a square
*/

SetMine(row, col, val)
int row, col, val;
{
	BYTE *p;

	p=mines_row[row];

	p[col]=val;
}

/* Return 1 if there is a flag in the square, else 0
*/

TstFlag(row, col)
int row, col;
{
	BYTE *p;

	p=flags_row[row];

	return p[col] ? 1 : 0;
}

/* Set/remove a flag in a square
*/

SetFlag(row, col, val)
int row, col, val;
{
	BYTE *p;

	p=flags_row[row];

	p[col]=val;
}

/* Return 1 if it is a visible square, else 0
*/

TstVis(row, col)
int row, col;
{
	BYTE *p;

	p=visib_row[row];

	return p[col] ? 1 : 0;
}

/* Set square to visible or invisible
*/

SetVis(row, col, val)
int row, col, val;
{
	BYTE *p;

	p=visib_row[row];

	p[col]=val;
}

/* Return the number of mines in adjacent squares
*/

FindAdj(row, col)
int row, col;
{
	int mines, rn, cn, i;

	for((mines = i = 0); i < 8; ++i)
	{
		rn = row + row_off[i];
		cn = col + col_off[i];

		if(rn >= 0 && rn < brd_rows && cn >= 0 && cn < brd_cols)
			if(TstMine(rn, cn))
				++mines;
	}

	return mines;
}

/* Set adjacent squares visible - recursively
*/

VisAdj(row, col)
int row, col;
{
	int rn, cn, i;

	for(i=0; i < 8; ++i)
	{
		rn = row + row_off[i];
		cn = col + col_off[i];

		if(rn >= 0 && rn < brd_rows && cn >= 0 && cn < brd_cols)
		{
			if(!TstVis(rn, cn) && !TstFlag(rn, cn))
			{
				SetVis(rn, cn, 1); ++viscnt;

				if(!GetCount(rn, cn))
					VisAdj(rn, cn);
			}
		}
	}
}

/* Display board
*/

ShowBoard()
{
	int r, c;

	putchar(' '); putchar(' ');
	for(c = 0; c < brd_cols; ++c)
		putchar('A' + c);
	putchar('\n');

	putchar(' '); putchar('+');
	for(c = 0; c < brd_cols; ++c)
		putchar('-');
	putchar('+'); putchar('\n');

	for(r = 0; r < brd_rows; ++r)
	{
		putchar('0' + r); putchar('|');

		for(c = 0; c < brd_cols; ++c)
		{
			if(TstVis(r, c))
			{
				if(GetCount(r, c))
					putchar('0' + GetCount(r, c));
				else
					putchar(SPR_BLANK);
			}
			else if(gameover)
			{
				if(TstMine(r, c))
					putchar(TstFlag(r, c) ? SPR_FLAG : SPR_MINE);
				else if(TstFlag(r, c))
					putchar(SPR_ERR);
				else
					putchar(SPR_UNK);
			}
			else
			{
#if DEBUG_MINES
				if(TstFlag(r, c))
					putchar(SPR_FLAG);
				else if(TstMine(r, c))
					putchar(SPR_MINE);
				else
					putchar(SPR_UNK);
#else
				putchar(TstFlag(r, c) ? SPR_FLAG : SPR_UNK);
#endif
			}
		}

		putchar('|'); putchar('0' + r);

		switch(r)
		{
			case 0 :
				printf("  FIND THAT MINE!");
				break;
			case 2 :
				if(gameover)
					printf("  GAME OVER");
				else
					printf("  Level: %d", level);
				break;
			case 4 :
				if(gameover)
					printf(dead ? "  YOU ARE DEAD!!!" : "  YOU WIN!!!");
				else
					printf("  Mines: %d", mines);
				break;
			case 6 :
				if(!gameover)
					printf("  Flags: %d", flgcnt);
				break;
		}

		putchar('\n');
	}

	putchar(' '); putchar('+');
	for(c = 0; c < brd_cols; ++c)
		putchar('-');
	putchar('+'); putchar('\n');

	putchar(' '); putchar(' ');
	for(c = 0; c < brd_cols; ++c)
		putchar('A' + c);
	putchar('\n');

	putchar('\n');
}


/* Setup board.
   All squares without mines, invisibles and without flags.
   Set the mines randomly.
*/

extern char RandStr[];

ClrBoard()
{
	int r, c; char *p;

	/* 
	*/

	p = RandStr + 8 * (random & 0x0F);

	for(r = 0; r < brd_rows; ++r)
	{
		for(c = 0; c < brd_cols; ++c)
		{
			/* Set square
			*/

			SetVis(r, c, 0);
			SetFlag(r, c, 0);
			SetMine(r, c, *p == '@' ? 1 : 0);

			/* Update next value
			*/

			if((*++p)=='$')
				p = RandStr;
		}
	}
}

/* Setup counters of adjacent squares
*/

ClrCounts()
{
	int r, c; char *p;

	for(r = 0; r < brd_rows; ++r)
		for(c = 0; c < brd_cols; ++c)
			SetCount(r, c, FindAdj(r, c));
}

/* String to setup board randomly (total of 128 bytes). 1 mine each 8 bytes.
*/

#asm

RandStr:

	;     v-------v-------v-------v-------

	defb '----@---@-------------@----@----'
	defb '-@-----------@----@----------@--'
	defb '--@---------@----------@--@-----'
	defb '----@----@-----------@---@------'
	defb '$'

#endasm

/* Read line from keyboard.
   Setup random number.
*/

ReadLine(buf, width)
char *buf;
int width;
{
	int len; char ch;

	len = 0;

	while(!kbhit())
		++random;

	while(1)
	{
		switch(ch = getch())
		{
			case 8   :
			case 127 :
				if(len)
				{
					putchar(8); putchar(' '); putchar(8); --len;
				}
				break;

			case '\r' :
			case '\n' :
				buf[len] = 0;
				return;

			default :
				if(ch >= ' ' && len < width)
					putchar(buf[len++] = toupper(ch));
		}
	}
}


