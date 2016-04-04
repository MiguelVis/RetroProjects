/*	BLOCKS!

	Game from Floppy Software for MESCC - Mike's Enhanced Small C Compiler.

	Copyright (c) 2012 Miguel I. Garcia Lopez, Valencia, Spain.

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

	Revisions:

	02 Jul 2012 : Generic CP/M version 1.0 for 24x80 VT52 screen.
*/

#include <mescc.h>	/* MESCC header & runtime */
#include <string.h>	/* We need strlen() */
#include <ctype.h>	/* We need toupper() */
#include <sprintf.h>	/* We need sprintf() */

#ifndef NULL
#define NULL 0
#endif

#ifndef WORD
#define WORD unsigned int
#endif

#include <kslib.h>	/* Screen output for CP/M */
#include <kslibfun.h>	/* More functions for kslib */

/*	GAME DEFS
*/

#define BOARD_ROW	6	/* Screen position for board */
#define BOARD_COL	25	/* id */

#define BORDER_ROW	5	/* Screen position for border */
#define BORDER_COL	24	/* id */

#define BOARD_ROWS	10	/* Board rows */
#define BOARD_COLS	10	/* Board columns */

#define BLOCK_COLS	3	/* Block size */
#define BLOCK_ROWS	1	/* id */

#define BLOCK_EMPTY	' '	/* Value if block is empty */

#define LEVEL_ROW	2	/* Screen position for level info - used to show blocks killed */
#define LEVEL_COL	2	/* id */

#define SCORE_ROW	2	/* Screen position for score info */
#define SCORE_COL	73	/* id */

#define K_UP		'Q'	/* Key up */
#define K_DOWN		'A'	/* Key down */
#define K_LEFT		'O'	/* Key left */
#define K_RIGHT		'P'	/* Key right */
#define K_EXIT		'X'	/* Key exit */
#define K_SELECT	'S'	/* Key select */
#define K_KILL   	'K'	/* Key kill */
#define K_GRAV_UP 	'1'	/* Key gravity up */
#define K_GRAV_LEFT 	'2'	/* Key gravity left */
#define K_GRAV_RIGHT 	'3'	/* Key gravity right */
#define K_GRAV_DOWN 	'4'	/* Key gravity down */

/*	VARIABLES
*/

WORD board[BOARD_ROWS];		/* Array for blocks values <<char *board[] -- MESCC things>> */
WORD board_sel[BOARD_ROWS];	/* Array for blocks selections <<id>> */

int	score,			/* Score */
	blocks,			/* Blocks remaining */
	randindex,		/* Random number used for generate blocks values */
	selected,		/* Number of blocks selected */
	add_to_score,		/* How much can I add to score if I kill selected blocks */
	automode,		/* Non zero if automode is selected */
	ChkPlsCtr,		/* CheckPlease variable */
	ChkPlsMul,		/* id */
	ChkPlsVal;		/* id */

/*	MAIN FUNCTION
*/

main(argc, argv)
int argc, argv[];
{
	int opt;

	/* SETUP KSLIB ENVIRONMENT
	*/

	KbdScrStart();

	/* SETUP SOME THINGS
	*/

	board[0]="1234567890"; board_sel[0]="1234567890";
	board[1]="1234567890"; board_sel[1]="1234567890";
	board[2]="1234567890"; board_sel[2]="1234567890";
	board[3]="1234567890"; board_sel[3]="1234567890";
	board[4]="1234567890"; board_sel[4]="1234567890";
	board[5]="1234567890"; board_sel[5]="1234567890";
	board[6]="1234567890"; board_sel[6]="1234567890";
	board[7]="1234567890"; board_sel[7]="1234567890";
	board[8]="1234567890"; board_sel[8]="1234567890";
	board[9]="1234567890"; board_sel[9]="1234567890";

	/* PLAY THE GAME, PLEASE
	*/

	while((opt=Menu()))
	{
		if(opt==1)
			Play();
	}

	/* GAME IS OVER - BEST TO CLEAN THE SCREEN
	*/

	ScrClr(); ScrCurOn();

	/* WE SAY GOOD BYE TO KSLIB ENVIRONMENT & WE SAY HELLO TO CP/M
	*/

	KbdScrEnd();
}

/*	MENU - RETURN 0 IF WE WANT TO END GAME, 1 IF WE WANT TO PLAY GAME, OR 2 IF WE WANT TO RETURN TO MENU
*/

Menu()
{
	ScrClr(); ScrCurOff();

	ScrTitle(0, "BLOCKS");
	ScrTitle(1, "v1.0");
	ScrTitle(3, "(c) 2012 FLOPPY SOFTWARE");

	ScrOutStrRC( 6, 25, "1 : PLAY GAME IN NORMAL MODE");
	ScrOutStrRC( 8, 25, "2 : PLAY GAME IN AUTOMATIC MODE");
	ScrOutStrRC(10, 25, "3 : SHOW HELP");

	ScrOutStrRC(12, 25, "X : EXIT GAME");

	ScrTitle(SCR_ROWS-2, "SELECT YOUR CHOICE");

	randindex=0; /* This is used to generate a random number */

	while(1)
	{
		while(!KbdChk())
		{
			if(++randindex==BOARD_ROWS*BOARD_COLS)
				randindex=0;
		}

		switch(toupper(KbdIn()))
		{
			case '1' : automode=0; return 1;
			case '2' : automode=1; return 1;
			case '3' : Help(); return 2;
			case 'X' : return 0;
		}
	}
}

/*	HELP -- SHOW HELP
*/

Help()
{
	ScrClr();

	ScrTitle( 0, "BLOCKS");

	ScrTitle( 2, "The object of the game is to remove all the");
	ScrTitle( 3, "blocks of the board.");
	ScrTitle( 5, "You can select just one block, or a group of");
	ScrTitle( 6, "blocks of the same type.");
	ScrTitle( 8, "Then, if you are in the normal mode, you can place all");
	ScrTitle( 9, "the blocks against a side (top, bottom, left or right).");
	ScrTitle(11, "If you are in the automatic mode, the blocks rows will be");
	ScrTitle(12, "placed on the bottom side of the board, and the columns");
	ScrTitle(13, "will be placed on the middle of the board.");
	ScrTitle(15, "The partial score is the square of the number of");
	ScrTitle(16, "blocks selected.");
	ScrTitle(18, "The special block 'X', multiplies that result by 10.");
	ScrTitle(20, "Good luck!");
	ScrTitle(22, "PRESS ANY KEY");

	KbdIn();
}

/*	PLAY THE GAME, PLEASE
*/

Play()
{
	int row, col, run, val, key;

	/* SETUP VARIABLES & BOARD
	*/

	row=col=selected=score=0; run=1;

	blocks=BOARD_ROWS*BOARD_COLS;

	SetupBoard();

	/* DRAW SCREEN
	*/

	ScrClr();

	ScrBox(0, 0, SCR_ROWS-1, SCR_COLS, NULL); ScrTitle(0, "| BLOCKS |");

	ScrOutStrRC(LEVEL_ROW, LEVEL_COL, "BLOCKS"); PrintBlocks();

	ScrOutStrRC(SCORE_ROW, SCORE_COL, "SCORE"); PrintScore();


	ScrBox(BORDER_ROW, BORDER_COL, BOARD_ROWS*BLOCK_ROWS+2, BOARD_COLS*BLOCK_COLS+2, NULL);


	PaintBoard();


	if(!automode)
	{
		scrTitle(BORDER_ROW-1, "1");

		ScrTitle(BORDER_ROW+BOARD_ROWS+2, "4");

		ScrOutStrRC(BORDER_ROW+5, BORDER_COL-2, "2");

		ScrOutStrRC(BORDER_ROW+5, BORDER_COL+BOARD_COLS*BLOCK_COLS+3, "3");
	}

	ScrOutStrRC(BORDER_ROW+3, 7, "    Q");
	ScrOutStrRC(BORDER_ROW+4, 7, "    |");
	ScrOutStrRC(BORDER_ROW+5, 7, "O --x-- P");
	ScrOutStrRC(BORDER_ROW+6, 7, "    |");
	ScrOutStrRC(BORDER_ROW+7, 7, "    A");

	ScrOutStrRC(BORDER_ROW+4, BORDER_COL+BOARD_COLS*BLOCK_COLS+8, "S> SELECT BLOCKS");
	ScrOutStrRC(BORDER_ROW+5, BORDER_COL+BOARD_COLS*BLOCK_COLS+8, "K> KILL BLOCKS");
	ScrOutStrRC(BORDER_ROW+6, BORDER_COL+BOARD_COLS*BLOCK_COLS+8, "X> EXIT");

	/* PLAY GAME
	*/

	while(run)
	{
		/* DRAW CURSOR
		*/

		ScrSetRC(BOARD_ROW+row*BLOCK_ROWS, BOARD_COL+col*BLOCK_COLS+1); ScrCurOn();

		/* USER ACTION
		*/

		key=toupper(KbdIn());

		ScrCurOff();

		switch(key)
		{
			case K_UP:	if(row) --row;
					break;

			case K_DOWN:	if(++row==BOARD_ROWS) --row;
					break;

			case K_LEFT:	if(col) --col;
					break;

			case K_RIGHT:	if(++col==BOARD_COLS) -- col;
					break;

			case K_SELECT:	/* CHECK FOR VALID BLOCKS
					*/

					val=GetBlock(row, col);

					if(val==BLOCK_EMPTY || val=='X')
						break;

					/* CHECK IF ALREADY SELECTED
					*/

					if(TstSelBlock(row, col))
						break;

					/* UNSELECT PREVIOUS SELECTION
					*/

					if(selected)
					{
						/* HEY! TOO MUCH WORK IF ONLY 1 SELECTED */

						UnSelAllBlocks();
						PaintBoard();
					}

					/* SELECT
					*/

					selected=CheckPlease(row, col);

					add_to_score=selected*selected*ChkPlsMul;

					PrintSelec();

					break;

			case K_KILL:	if(selected)
					{
						/* HEY! TOO MUCH WORK IF ONLY 1 SELECTED */

						KillSelBlocks();

						score+=add_to_score;

						PrintScore();

						blocks-=selected; PrintBlocks();

						selected=add_to_score=0;

						PrintSelec();

						if(!blocks)
						{
							GameOver(); run=0;
						}
						else if(automode)
						{
							DoAutoMode(); PaintBoard();
						}
					}

					break;

			case K_GRAV_UP: if(automode)
						break;

					GravityUp();

					if(selected)
					{
						UnSelAllBlocks(); selected=0;
					}

					PaintBoard();

					break;

			case K_GRAV_DOWN:
					if(automode)
						break;

					GravityDown();

					if(selected)
					{
						UnSelAllBlocks(); selected=0;
					}

					PaintBoard();

					break;

			case K_GRAV_LEFT:
					if(automode)
						break;

					GravityLeft();

					if(selected)
					{
						UnSelAllBlocks(); selected=0;
					}

					PaintBoard();

					break;

			case K_GRAV_RIGHT:
					if(automode)
						break;

					GravityRight();

					if(selected)
					{
						UnSelAllBlocks(); selected=0;
					}

					PaintBoard();

					break;

			case K_EXIT :
					PrintMsg("ARE YOU SURE Y/N?");

					if(toupper(KbdIn())=='Y')
						run=0;
					else
						PrintMsg("");

					break;
		}
	}
}

/*	GAME IS OVER
*/

GameOver()
{
	ScrTitle(BORDER_ROW+4, "*** GAME OVER ***");

	PrintMsg("PRESS ANY KEY");

	KbdIn();
}

/*	SETUP BOARD
*/

SetupBoard()
{
	int r, c; unsigned char *values;

	/*      0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789 (width 100) */
	values="4321242333123432411123443444432111234332222343331214422444441243444132222311114242423111133222441111";

	for(r=0; r!=BOARD_ROWS; ++r)
	{
		for(c=0; c!=BOARD_COLS; ++c)
		{
			SetBlock(r, c, values[randindex]);

			UnSelBlock(r, c);

			if(++randindex==BOARD_ROWS*BOARD_COLS)
				randindex=0;
		}
	}

	if(randindex<25)
		SetBlock(2, 5, 'X');
	else if(randindex<50)
		SetBlock(9, 4, 'X');
	else if(randindex<75)
		SetBlock(4, 8, 'X');
	else
		SetBlock(7, 3, 'X');
}

/*	SET BLOCK WITH VALUE
*/

SetBlock(row, col, value)
int row, col, value;
{
	char *ptr;

	ptr=board[row];

	ptr[col]=value;
}

/*	GET BLOCK VALUE
*/

GetBlock(row, col)
int row, col;
{
	char *ptr;

	ptr=board[row];

	return ptr[col];
}

/*	SELECT BLOCK
*/

SelBlock(row, col)
int row, col;
{
	char *ptr;

	ptr=board_sel[row];

	ptr[col]=1;
}

/*	UNSELECT BLOCK
*/

UnSelBlock(row, col)
int row, col;
{
	char *ptr;

	ptr=board_sel[row];

	ptr[col]=0;
}

/*	UNSELECT ALL BLOCKS
*/

UnSelAllBlocks()
{
	int r, c;

	for(r=0; r!=BOARD_ROWS; ++r)
		for(c=0; c!=BOARD_COLS; ++c)
			UnSelBlock(r, c);
}

/*	TEST IF A BLOCK IS SELECTED
*/

TstSelBlock(row, col)
int row, col;
{
	char *ptr;

	ptr=board_sel[row];

	return ptr[col];
}

/*	KILL SELECTED BLOCKS
*/

KillSelBlocks()
{
	int r, c;

	for(r=0; r!=BOARD_ROWS; ++r)
	{
		for(c=0; c!=BOARD_COLS; ++c)
		{
			if(TstSelBlock(r, c))
			{
				SetBlock(r, c, BLOCK_EMPTY);
				UnSelBlock(r, c);
				PaintBlock(r, c);
			}
		}
	}
}

/*	PAINT BLOCK	
*/

PaintBlock(row, col)
int row, col;
{
	int selected;

	ScrSetRC(BOARD_ROW+row*BLOCK_ROWS, BOARD_COL+col*BLOCK_COLS);

	ScrOut((selected=TstSelBlock(row, col)) ? ':' : ' ');

	switch(GetBlock(row, col))
	{
		case '1' : ScrOut('$'); break;
		case '2' : ScrOut('O'); break;
		case '3' : ScrOut('+'); break;
		case '4' : ScrOut('#'); break;
		case 'X' : ScrOut('X'); break;
		case BLOCK_EMPTY : ScrOut(' '); break;
		default  : ScrOut('?'); break;
	}

	ScrOut(selected ? ':' : ' ');
}

/*	PAINT ALL BLOCKS
*/

PaintBoard()
{
	int r, c;

	for(r=0; r!=BOARD_ROWS; ++r)
		for(c=0; c!=BOARD_COLS; ++c)
			PaintBlock(r, c);
}

/*	CHECK WHAT BLOCKS CAN BE SELECTED - SETUP THREE VARIABLES
*/

CheckPlease(row, col)
int row, col;
{
	ChkPlsCtr=0; ChkPlsMul=1; ChkPlsVal=GetBlock(row, col);

	ChkPls2(row, col);

	return ChkPlsCtr;
}

ChkPls2(row, col)
int row, col;
{
	int val;

	SelBlock(row, col); PaintBlock(row, col);

	if(GetBlock(row, col)=='X')
		ChkPlsMul=10;

	if(row)
	{
		if(((val=GetBlock(row-1, col))==ChkPlsVal || val=='X') && (!TstSelBlock(row-1, col)))
			ChkPls2(row-1, col);
	}

	if(col<(BOARD_COLS-1))
	{
		if(((val=GetBlock(row, col+1))==ChkPlsVal || val=='X') && (!TstSelBlock(row, col+1)))
			ChkPls2(row, col+1);
	}

	if(row<(BOARD_ROWS-1))
	{
		if(((val=GetBlock(row+1, col))==ChkPlsVal || val=='X') && (!TstSelBlock(row+1, col)))
			ChkPls2(row+1, col);
	}

	if(col)
	{
		if(((val=GetBlock(row, col-1))==ChkPlsVal || val=='X') && (!TstSelBlock(row, col-1)))
			ChkPls2(row, col-1);
	}

	++ChkPlsCtr;
}

/*	GRAVITY DOWN (BOARD BOTTOM)
*/

GravityDown()
{
	int r, c, i, blk, row[BOARD_ROWS];

	for(c=0; c!=BOARD_COLS; ++c)
	{
		for((r=i=BOARD_ROWS-1); r!=-1; --r)
		{
			if((blk=GetBlock(r, c))!=BLOCK_EMPTY)
				row[i--]=blk;
		}

		while(i!=-1)
			row[i--]=BLOCK_EMPTY;

		for(r=0; r!=BOARD_ROWS; ++r)
			SetBlock(r, c, row[r]);
	}
}

/*	GRAVITY UP (WE PUT BLOCKS ON BOARD TOP)
*/

GravityUp()
{
	int r, c, i, blk, row[BOARD_ROWS];

	for(c=0; c!=BOARD_COLS; ++c)
	{
		for((r=i=0); r!=BOARD_ROWS; ++r)
		{
			if((blk=GetBlock(r, c))!=BLOCK_EMPTY)
				row[i++]=blk;
		}

		while(i!=BOARD_ROWS)
			row[i++]=BLOCK_EMPTY;

		for(r=0; r!=BOARD_ROWS; ++r)
			SetBlock(r, c, row[r]);
	}
}

/*	GRAVITY LEFT (WE PUT BLOCKS ON BOARD LEFT)
*/

GravityLeft()
{
	int r, c, i, blk, col[BOARD_ROWS];

	for(r=0; r!=BOARD_ROWS; ++r)
	{
		for((c=i=0); c!=BOARD_COLS; ++c)
		{
			if((blk=GetBlock(r, c))!=BLOCK_EMPTY)
				col[i++]=blk;
		}

		while(i!=BOARD_COLS)
			col[i++]=BLOCK_EMPTY;

		for(c=0; c!=BOARD_COLS; ++c)
			SetBlock(r, c, col[c]);
	}
}

/*	GRAVITY RIGHT (WE PUT BLOCKS ON BOARD RIGHT)
*/

GravityRight()
{
	int r, c, i, blk, col[BOARD_ROWS];

	for(r=0; r!=BOARD_ROWS; ++r)
	{
		for((c=i=BOARD_COLS-1); c!=-1; --c)
		{
			if((blk=GetBlock(r, c))!=BLOCK_EMPTY)
				col[i--]=blk;
		}

		while(i!=-1)
			col[i--]=BLOCK_EMPTY;

		for(c=0; c!=BOARD_COLS; ++c)
			SetBlock(r, c, col[c]);
	}
}

/*	AUTOMATIC MODE (WE PUT BLOKS FIRST ON BOARD BOTTOM, THEN WE CENTER THE COLUMNS)
*/

DoAutoMode()
{
	int r, c, i, pos, cols;

	GravityDown();

	/* COPY NON EMPTY COLUMNS TO THE LEFT
	*/

	for(i=c=0; c!=BOARD_COLS; ++c)
	{
		if(GetBlock(BOARD_ROWS-1, c) != BLOCK_EMPTY)
		{
			if(i!=c)
			{
				for(r=0; r!=BOARD_ROWS; ++r)
					SetBlock(r, i, GetBlock(r, c));
			}

			++i;
		}
	}

	/* DO NOTHING IF ALL COLUMNS ARE NON EMPTY
	*/

	if(i==BOARD_COLS)
		return;

	/* CALCULATE WHERE TO PUT THE COLUMNS
	*/

	pos=(BOARD_COLS-i)/2;

	/* COPY COLUMNS WITHOUT OVERLAP
	*/

	cols=i;

	for(c=pos+i-1; i!=-1; --c)
	{
		--i;

		for(r=0; r!=BOARD_ROWS; ++r)
			SetBlock(r, c, GetBlock(r, i));
	}

	/* EMPTY COLUMNS TO THE LEFT
	*/

	for(c=0; c!=pos; ++c)
		for(r=0; r!=BOARD_ROWS; ++r)
			SetBlock(r, c, BLOCK_EMPTY);

	/* EMPTY COLUMNS TO THE RIGHT
	*/

	for(c=pos+cols; c!=BOARD_COLS; ++c)
		for(r=0; r!=BOARD_ROWS; ++r)
			SetBlock(r, c, BLOCK_EMPTY);
}

/*	PRINT HOW MANY BLOCKS ARE REMAINING
*/

PrintBlocks()
{
	char str[6];

	sprintf(str, "%d  ", blocks);

	ScrOutStrRC(LEVEL_ROW+1, LEVEL_COL, str);
}

/*	PRINT SCORE
*/

PrintScore()
{
	char str[6];

	sprintf(str, "%5d", score);

	ScrOutStrRC(SCORE_ROW+1, SCORE_COL, str);
}

/*	PRINT SELECTION INFO.
*/

PrintSelec()
{
	char str[7];

	sprintf(str, "x%d  ", selected);

	ScrOutStrRC(LEVEL_ROW+3, LEVEL_COL, str);

	sprintf(str, "%5d+", add_to_score);

	ScrOutStrRC(SCORE_ROW+3, SCORE_COL-1, str);
}

/*	PRINT TEXT STRING (MESSAGE)
*/

PrintMsg(str)
char *str;
{
	ScrSetRC(SCR_ROWS-4, 2);
	ScrOutRpt(' ', SCR_COLS-4);

	ScrTitle(SCR_ROWS-4, str);
}
