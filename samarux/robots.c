/*	robots.c

	A turn-based game for CP/M-80 & CP/M-86.

	Based on ROBOTS, a BSD game (but no source code was taken from it).

	The objetive of the robots is only one: to kill you.

	The only thing you can do, is to try to escape from them. But use the
	teletransportation with care: you have only a few teletransport tickets!

	The robots will die if they collide between them or crash against something.

	That's your only opportunity to win the robots.

	Good luck!

	Copyright (c) 2015 Miguel I. Garcia Lopez / FloppySoftware.

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

	Contact:

	www.floppysoftware.vacau.com
	cpm-connections.blogspot.com
	floppysoftware@gmail.com

	Usage:

	robots

	Compile with MESCC (CP/M-80):

	cc robots
	zsm robots
	hextocom robots

	Compile with DR-C (CP/M-86):

	drc robots
	link86 robots

	Compile with MESCC (SamaruX):

	cc robots
	zmac robots.zsm --rel
	link robots [op]
	rename robots.x=robots.prl

	Changes:

	18 Mar 2015 : 0.01 : Initial work from TinyWar source code.
	19 Mar 2015 : 1.00 : 1st version.
	09 Apr 2015 : 1.01 : Added support for SamaruX.
*/

/* TARGET: uncomment only one of following lines
   ---------------------------------------------
*/

/* #define CPM80 */
/* #define CPM86 */
#define SAMARUX

/* MESCC LIBRARIES
   ---------------
*/

#ifdef CPM80

#define CC_NO_ARGS

#include "mescc.h"
#include "conio.h"
#include "ctype.h"
#include "string.h"
#include "printf.h"

#endif

/* SAMARUX LIBRARIES
   -----------------
*/

#ifdef SAMARUX

#include "samarux.h"

#endif

/* DR-C LIBRARIES & SUPPORT STUFF
   ------------------------------
*/

#ifdef CPM86

#include "stdio.h"

#undef getchar

/* DR-C doesn't have this one */

kbhit()
{
	return __BDOS(11, 0);
}

/* DR-C doesn't work well mixing getchar() from STDIO.H
   and our kbhit() */

getchar()
{
	return __BDOS(1, 0);
}

/* DR-C doesn't have this one */

putstr(s)
char *s;
{
	while(*s)
		putchar(*s++);
}

/* DR-C has this one as a macro in CTYPE.H but cause
   some problems with this code */

toupper(ch)
int ch;
{
	if(ch >= 'a' && ch <= 'z')
		ch -= 32;

	return ch;
}

#endif

/* GAME DEFs & GLOBALS
   -------------------
*/

#define BOARD_ROWS 16       /* Board rows */
#define BOARD_COLS 16       /* Board columns */
#define BOARD_SIZE 256      /* Board cells = BOARD_ROWS * BOARD_COLS */

int brd_rows[BOARD_ROWS];   /* Board rows - char *brd_rows[] */
char brd_cell[BOARD_SIZE];  /* Board cells */

#define ROBOTS_UNITS 8      /* # of robots */

int robots_left;            /* Robots left in board */

int human_life;             /* NZ if human is alive */
int human_row;              /* Row position */
int human_col;              /* Column position */

#define TELETR_UNITS 3      /* Teletransportation units */

int teltr_left;             /* Teletransportation units left */

int moves;                  /* Number of moves done */

int random;                 /* Random number from 0 to 32767, managed by KbdIn() */

int running;                /* NZ if we are playing, Z to quit */

int act_robots;             /* NZ if robots can play */

char *brd_msg;              /* Pointer to board message */

/* BOARD OBJECTS
   -------------
*/

#define TYPE_GROUND  ' '
#define TYPE_WALL    'X'
#define TYPE_ROBOT   'R'
#define TYPE_HUMAN   'H'
#define TYPE_DEAD    'D'
#define TYPE_SCRAP   'C'
#define TYPE_TEMP    'T'

/* SPRITES
   -------
*/

#define SPR_GROUND  "   "
#define SPR_WALL    "@@@"
#define SPR_HUMAN   ".P."
#define SPR_DEAD    "XPX"
#define SPR_SCRAP   "X'X"

#define SPR_ROBOT0  "/'\\"
#define SPR_ROBOT1  "/'|"
#define SPR_ROBOT2  "|'\\"
#define SPR_ROBOT3  "|'|"

/* KEYS
   ----
*/

#define K_UP         '8'
#define K_DOWN       '2'
#define K_LEFT       '4'
#define K_RIGHT      '6'
#define K_UP_LEFT    '7'
#define K_UP_RIGHT   '9'
#define K_DOWN_LEFT  '1'
#define K_DOWN_RIGHT '3'
#define K_TELETRANSP 'T'
#define K_STOP       'Q'


/* MAIN CODE
   ---------
*/

main()
{
	/* Show the banner, copyright info, etc. */

	Banner();

	/* Set up the board */

	SetUpBoard();

	/* Clean the board */

	SetBoard();

	/* We are playing! */

	running = 1;

	/* Main loop */

	while(human_life && robots_left && running)
	{
		/* Show the board */

		PrtLn();
		PrtBoard();
		PrtLn();

		/* Robots should do their part */

		act_robots = 1;

		/* Ask human for an action */

		AskAction();

		/* Robots should play too? */

		if(act_robots)
			ActRobots();
	}

	/* Game is over, check the reason and print a message */

	if(!human_life)
		Msg("You are dead, bad luck!");
	else if(!robots_left)
		Msg("You win, congratulations!");
	else
		Msg("Ok, I'm leaving! See you!");

	/* Show the board for last time */

	PrtLn();
	PrtBoard();
}

/* BANNER, COPYRIGHT, ETC.
   -----------------------
*/

Banner()
{
	/* Good looking banner */

	puts("\n      R           ()   : :   ()");
	puts("    R O B O T S    \\  [0_0]  /");
	puts("      B             x-+-:-+-x");
	puts("R O B O T S           |___|");
	puts("      T               /   \\");
	puts("      S               \\   /       v1.00\n");

	/* Copyright and contact details */

	puts("(c) 2015 Miguel Garcia / FloppySoftware");
	puts("www.floppysoftware.vacau.com");
	puts("cpm-connections.blogspot.com");
	puts("floppysoftware@gmail.com\n");

	/* Waiting for the user */

	putstr("Press any key to start... ");

	/* Wait a keyboard entry (this will be useful for random number generation) */

	KbdIn();

	/* New line */

	PrtLn();
}

/* HUMAN ACTION
   ------------
*/

AskAction()
{
	int action;

	/* No board message */

	Msg(NULL);

	/* Ask for action */

	putstr("Action? ");

	/* Read keyboard */

	action = KbdIn();

	if(action != '\n')
		PrtLn();

	/* Do the action */

	switch(action)
	{
		case K_UP :
			ActMove(-1, 0);
			break;
		case K_DOWN :
			ActMove(1, 0);
			break;
		case K_LEFT :
			ActMove(0, -1);
			break;
		case K_RIGHT :
			ActMove(0, 1);
			break;
		case K_UP_LEFT :
			ActMove(-1, -1);
			break;
		case K_UP_RIGHT :
			ActMove(-1, 1);
			break;
		case K_DOWN_LEFT :
			ActMove(1, -1);
			break;
		case K_DOWN_RIGHT :
			ActMove(1, 1);
			break;

		case K_TELETRANSP :
			ActTelTr();
			break;

		case K_STOP :
			ActExit();
			break;

		default :
			act_robots = 0;
			break;
	}
}

/* ASK HUMAN FOR CONFIRMATION
   --------------------------
*/

AskConfirm()
{
	int reply;

	/* Prompt */

	putstr("Confirm (Yes, No)? ");

	/* Read keyboard */

	reply = KbdIn();

	if(reply != '\n')
		PrtLn();

	/* Return TRUE or FALSE */

	return reply == 'Y' ? 1 : 0;
}

/* MOVE THE HUMAN
   --------------
*/

ActMove(srow, scol)
int srow, scol;
{
	int to_row, to_col, type;

	/* To where? */

	to_row = human_row + srow;
	to_col = human_col + scol;

	/* Act if next position is inside of board */

	if(to_row >= 0 && to_row < BOARD_ROWS && to_col >= 0 && to_col < BOARD_COLS)
	{
		/* Check contents of new position */

		type = GetCell(to_row, to_col);

		if(type == TYPE_GROUND)
		{
			/* GROUND: move human there */

			SetCell(human_row, human_col, TYPE_GROUND);

			human_row = to_row;
			human_col = to_col;

			SetCell(human_row, human_col, TYPE_HUMAN);

			/* Increment moves */

			++moves;

			/* Done */

			return;
		}
		else if(type == TYPE_ROBOT)
		{
			/* ROBOT: human is dead */

			--human_life;

			SetCell(human_row, human_col, TYPE_DEAD);
		}
	}

	/* Robots will do nothing */

	act_robots = 0;
}

/* TELETRANSPORTATION
   ------------------
*/

ActTelTr()
{
	int to_row, to_col;

	/* Do it if there are some teletransportations units */

	if(teltr_left)
	{
		/* Find a random safe location */

		do {

			to_row = random % BOARD_ROWS;
			to_col = random % BOARD_COLS;

			random += 3;

		} while(GetCell(to_row, to_col) != TYPE_GROUND);

		/* Move the human there */

		SetCell(human_row, human_col, TYPE_GROUND);

		human_row = to_row;
		human_col = to_col;

		SetCell(human_row, human_col, TYPE_HUMAN);

		/* We lost a teletransportation unit */

		--teltr_left;

		/* Increment moves */

		++moves;
	}

	/* Robots will do nothing */

	act_robots = 0;
}

/* QUIT GAME
   ---------
*/

ActExit()
{
	/* Ask for confirmation */

	PrtLn();

	running = !AskConfirm();

	/* Robots will do nothing */

	act_robots = 0;
}


/* MOVE ROBOTS
   -----------
*/

ActRobots()
{
	int row, col, to_row, to_col, type;

	/* Search robots in the board */

	for(row = 0; row < BOARD_ROWS; ++row)
	{
		for(col = 0; col < BOARD_COLS; ++col)
		{
			/* Cell has a robot? */

			if(GetCell(row, col) == TYPE_ROBOT)
			{
				/* Move near to the human */

				to_row = row;
				to_col = col;


				/* Look for new row position */

				if(row < human_row)
					++to_row;
				else if(row > human_row)
					--to_row;

				/* Look for new column position */

				if(col < human_col)
					++to_col;
				else if(col > human_col)
					--to_col;

				/* Is there something there? */

				type = GetCell(to_row, to_col);

				if(type == TYPE_GROUND)
				{
					/* GROUND: Move the robot there */

					SetCell(row, col, TYPE_GROUND);
					SetCell(to_row, to_col, TYPE_TEMP);
				}
				else
				{
					/* NOT GROUND: Robot is dead */

					SetCell(row, col, TYPE_SCRAP);

					/* This robot is dead */

					--robots_left;

					/* If there is the human or another robot
					   there, he is dead too */

					if(type == TYPE_ROBOT || type == TYPE_TEMP)
					{
						/* Robot: dead */

						SetCell(to_row, to_col, TYPE_SCRAP);

						--robots_left;
					}
					else if(type == TYPE_HUMAN)
					{
						/* Human: dead */

						SetCell(to_row, to_col, TYPE_DEAD);

						--human_life;
					}
				}
			}
		}
	}

	/* Persist the new locations of the robots */

	for(row = 0; row < BOARD_ROWS; ++row)
	{
		for(col = 0; col < BOARD_COLS; ++col)
		{
			if(GetCell(row, col) == TYPE_TEMP)
				SetCell(row, col, TYPE_ROBOT);
		}
	}
}

/* SHOW THE BOARD
   --------------
*/

PrtBoard()
{
	int i, row, col, type;

	PrtBoardLn();

	for(row = 0; row < BOARD_ROWS; ++row)
	{
		/* Left panel */

		putchar('|');

		for(col = 0; col < BOARD_COLS; ++col)
		{
			/* Get cell contents */

			type = GetCell(row, col);

			/* Act according to the cell contents */

			switch(type)
			{
				case TYPE_GROUND :
					putstr(SPR_GROUND);
					break;

				case TYPE_ROBOT :
					switch(row & 0x03)
					{
						case 0 : putstr(SPR_ROBOT0); break;
						case 1 : putstr(SPR_ROBOT1); break;
						case 2 : putstr(SPR_ROBOT2); break;
						case 3 : putstr(SPR_ROBOT3); break;
					}
					break;

				case TYPE_SCRAP :
					putstr(SPR_SCRAP);
					break;

				case TYPE_HUMAN :
					putstr(SPR_HUMAN);
					break;

				case TYPE_DEAD :
					putstr(SPR_DEAD);
					break;

				case TYPE_WALL :
					putstr(SPR_WALL);
					break;
			}
		}

		/* Right panel */

		putchar('|');

		switch(row)
		{
			case 1 :
				putstr("    7 8 9    ");
				break;
			case 2 :
				putstr("     \\|/     ");
				break;
			case 3 :
				putstr("  4 --+-- 6  ");
				break;
			case 4 :
				putstr("     /|\\     ");
				break;
			case 5 :
				putstr("    1 2 3    ");
				break;

			case 7 :
				putstr(" T:Teletran. ");
				break;
			case 8 :
				putstr(" Q:Quit      ");
				break;

			case 12:
				printf(" Teletr.: %2d ", teltr_left);
				break;
			case 13:
				printf(" Moves: %4d ", moves);
				break;

			case 10:
				putstr("-------------");
				break;

			default :
				putstr("             "); break;
		}

		putchar('|');

 		PrtLn();
	}

	PrtBoardLn();

	/* Show message */

	putstr("| ");

	putstr(brd_msg);

	for(i = 3 * BOARD_COLS - strlen(brd_msg) - 1; i; --i)
		putchar(' ');

	puts("| R O B O T S |");

	PrtBoardLn();
}

/* PRINT BOARD LINE
   ----------------
*/

PrtBoardLn()
{
	int col;

	putchar('+');

	for(col = 0; col < BOARD_COLS; ++col)
		putstr("---");

	puts("+-------------+");
}

/* PRINT NEW LINE
   --------------
*/

PrtLn()
{
	putchar('\n');
}

/* SET UP BOARD
   ------------
*/

SetUpBoard()
{
	int i;

	/* Set up the rows pointers */

	for(i = 0; i < BOARD_ROWS; ++i)
		brd_rows[i] = brd_cell + i * BOARD_COLS;
}

/* CLEAR THE BOARD
   ---------------
*/

ClearBoard()
{
	int row, col;

	/* Clear all cells */

	for(row = 0; row < BOARD_ROWS; ++row)
		for(col = 0; col < BOARD_COLS; ++col)
			SetCell(row, col, TYPE_GROUND);
}

/* SET CELL CONTENTS
   -----------------
*/

SetCell(row, col, type)
int row, col, type;
{
	char *p;

	/* Get the row pointer */

	p = brd_rows[row];

	/* Set the cell value */

	p[col] = type;
}

/* GET CELL CONTENTS
   -----------------
*/

GetCell(row, col)
int row, col;
{
	char *p;

	/* Get the row pointer */

	p = brd_rows[row];

	/* Set the cell value */

	return p[col];
}

/* SET BOARD MESSAGE
   -----------------
*/

Msg(txt)
char *txt;
{
	brd_msg = txt == NULL ? "(c) 2015 FloppySoftware" : txt;
}

/* SET BOARD CONTENTS
   ------------------
*/

SetBoard()
{
	int row, col, i, x;
	char *pos;

	/* Clear message */

	Msg(NULL);

	/* Clear board */

	ClearBoard();

	/* Reset some values */

	moves = 0;
	teltr_left = TELETR_UNITS;

	/* Set some walls */

	SetCell(2, 2, TYPE_WALL);
	SetCell(3, 2, TYPE_WALL);
	SetCell(BOARD_ROWS - 4, BOARD_COLS - 3, TYPE_WALL);
	SetCell(BOARD_ROWS - 3, BOARD_COLS - 3, TYPE_WALL);

	/* Set human values */

	human_life = 1;

	human_row = BOARD_ROWS / 2;
	human_col = BOARD_COLS / 2;

	SetCell(human_row, human_col, TYPE_HUMAN);

	/* Set robots values */

	robots_left = ROBOTS_UNITS;

	/* Locate robots in random positions */

	    /* 0 1 2 3 4 5 6 7 8 9 A B C D E F */
	pos = "044E8AF2C5638EB229E936FAE5471EBB";

 	/* Get a number from 0 to 31 */

	x = random % 32;

	i = 0;

	while(i < robots_left)
	{
		/* Get a position */

		row = GetVal(pos[x++]) % BOARD_ROWS; if(x > 31) x = 0;
		col = GetVal(pos[x++]) % BOARD_COLS; if(x > 31) x = 0;

		/* Place the robot there, if the location is empty */

		if(GetCell(row, col) == TYPE_GROUND)
		{
			SetCell(row, col, TYPE_ROBOT);

			++i;
		}
	}
}

/* HELPER FOR SetBoard()
   ---------------------
*/

GetVal(c)
int c;
{
	/* Return HEX value of digit */

	if(c >= '0' && c <= '9')
		return c - '0';

	return c - 'A' + 10;
}

/* GET CHARACTER FROM KEYBOARD AND SET RANDOM VALUE
   ------------------------------------------------
*/

KbdIn()
{
	/* Init random value */

	random = 0;

	/* Wait for a key and increment the random value */

	while(!kbhit())
		++random;

	/* Set the random value to a legal one (0 ... 32767) */

	random &= 0x7FFF;

	/* Return character */

	return toupper(getchar());
}




