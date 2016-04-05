/* 10 Sep 2015 ported to MESCC and the XPCW library
   by Miguel Garcia / FloppySoftware.

   To compile with MESCC:

   cc xppong -s:2048
   ccopt xppong.zsm
   zsm xppong
   hextocom xppong
   gencom xppong.com xpcw.rsx

   -------------------------------------------------

   Polish Pong game for H19/H89, RHH (Robert H. Halstead) August 1980: 

   Object is to guide the little ball around the screen by setting up
   and removing blockade sections. All control is via the keypad; "4"
   and "6" cause blockades to be formed at the current position of the
   roving ball, while pressing "5" at the exact moment the ball hits a
   blockade should make that blockade disappear. Oh yes, and the POINT
   of all this is to make the ball hit the little square target--once
   this is done, the square will disappear and reappear somewhere else,
   to be hit again. Go for hitting the target the specified number of
   times AS QUICKLY AS POSSIBLE. Your score is how many seconds you
   took; the lower the better.
   Keys "8" and "2" speed up and slow down the ball; as you get better,
   try it at a faster speed!
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
#include "sprintf.h"
#include "rand.h"

/* XPCW libraries
   --------------
*/
#include "xpcw.h"
#include "xchrdef.h"

#define	MAXX	88		/* horizontal size of board */
#define	MAXY	23		/* vertical size */
#define SIZE	2024		/* MAXX * MAXY */
#define	MAXTARG	20		/* # of targets per game */
#define	ISPEED	400		/* initial ball speed */
#define	SPEEDINC 100		/* increments/decrements in ball speed */
 
#define	TIMX	50		/* locations of status strings */
#define	TIMY	24
#define	TARGX	30
#define	TARGY	24
#define	SPEEDX	10
#define	SPEEDY	24
#define	BESTX	70
#define	BESTY	24

#define	CONINF	1		/* console input FDOS function */

#define	MSPS	960		/* "millisecs" per second */

#define	QUITCH	3		/* ^C quits the program */
#define	DELETE	127		/* DELETE restarts the game */

#define	XOFF	('S'&31)	/* flow control chars */
#define	XON	('Q'&31)

#define	ESC	27		

#define VBAR	0x95	
#define	HBAR	0x9A
#define	SLASH	'/'
#define	BSLASH	'\\'
#define	BALL	0x82
#define	TARGET	0x83

BYTE board[SIZE];
/*char board[MAXX][MAXY];*/		/* board with current layout */

int ballx,bally,ballxv,ballyv;	/* state of ball */
int Speed,Dist;			/* speed of ball */
int TargLeft;			/* number of targets left */
int MSecs,Secs;			/* bookkeeping for time elapsed */
int NewTime;			/* nonzero if time has changed */
int Best;			/* best score so far */
int InChar;			/* character read from input */

putchx(c)
 int c;
  {	if (++MSecs >= MSPS)
	  { MSecs = 0; Secs++; 
	    NewTime = 1;
	  }
	Dist += Speed;
	if (kbhit())
	  { InChar = getch() & 127;
	    if (InChar == QUITCH)
	      {
		bye();
	      }
	    if (InChar == XOFF)
	      {	while (InChar != XON)
		  { while (!kbhit());
		    InChar = getch() & 127;
		  }
		InChar = -1;
	      }
	  }
	putch(c);
	/* *************************
	if (c == '\n') putchx('\r');
	************************* */
  }

prints(s)			/* put out a string */
 char *s;
  {	int c;
	while (c = *s++) putchx(c);
  }

ouch(ch,x,y)			/* put character at position */
 int ch,x,y;
  {
	putchx(ESC); putchx('Y'); putchx(y+36); putchx(x+32); putchx(ch);
  }

outs(x,y,s)			/* put string at position (x,y) */
 int x,y;
 char *s;
  {
	putchx(ESC); putchx('Y'); putchx(y+36); putchx(x+32);
	prints(s);
  }

puttarg()
  {	char buff[100];
	sprintf(buff,"%cp%2d%cq",27, TargLeft, 27);
	outs(TARGX,TARGY,buff);
  }

puttime()
  {	char buff[100];
	sprintf(buff,"%cp%3d%cq",27, Secs, 27);
	outs(TIMX,TIMY,buff);
  }

putspeed()
  {	char buff[100];
	sprintf(buff,"%cp%3d%cq",27, Speed/10, 27);
	outs(SPEEDX,SPEEDY,buff);
  }

int getboard(x, y)
int x, y;
{
	return board[y * MAXX + x];
}

setboard(x, y, c)
int x, y, c;
{
	board[y * MAXX + x] = c;
}
	
int moveball()
  {	int i,nx,ny;
	Dist = 0;
	i = InChar;
	if (i > 0)
	  { InChar = -1;
	    switch (i)
	      {	case '4':			/* lay down backslash */
		    if (getboard(ballx,bally) == ' ')
		      setboard(ballx,bally,BSLASH);
		     else putchx(7);
		    break;

		case '6':			/* lay down slash */
		    if (getboard(ballx,bally) == ' ')
		      setboard(ballx,bally,SLASH);
		     else putchx(7);
		    break;

		case '5':			/* delete current char */
		    i = getboard(ballx,bally);
		    if (i == SLASH || i == BSLASH) setboard(ballx,bally,' ');
		     else putchx(7);
		    break;

		case '8':			/* go faster */
		    if (Speed < 1000) { Speed += SPEEDINC; putspeed(); }
		    break;

		case '2':			/* go slower */
		    if (Speed > SPEEDINC+50) { Speed -= SPEEDINC; putspeed(); }
		    break;

		case DELETE:			/* start a new game */
		    return(0);

		default:
		    putchx(7); break;
	      }
	  }
	switch (getboard(ballx,bally))
	  { case ' ':	break;
	    case VBAR:	ballxv = -ballxv; break;
	    case HBAR:	ballyv = -ballyv; break;
	    case BSLASH: i = ballxv; ballxv = ballyv; ballyv = i; break;
	    case SLASH:	i = ballxv; ballxv = -ballyv; ballyv = -i; break;
	    case TARGET:
		if (--TargLeft <= 0) return(0);
		puttarg();
		setboard(ballx,bally,' ');
		do {	nx = rand()%(MAXX-2) + 1;
			ny = rand()%(MAXY-2) + 1;
		   } while (getboard(nx,ny) != ' ');
		setboard(nx,ny,TARGET);
		ouch(TARGET,nx,ny);
		break;
	  }
	nx = ballx + ballxv;
	ny = bally + ballyv;
	ouch(BALL,nx,ny);
	ouch(getboard(ballx,bally),ballx,bally);
	if (NewTime) { puttime(); NewTime = 0; }
	ballx = nx; bally = ny;
	while (Dist < (ballyv?22000:10000)) putchx(1);
					/* further delay to slow ball down */
	return(1);
  }

main()
  {
	/* Check if the XPCW RSX is in memory */

	if(!HelloRsx()) {
		puts("The XPCW RSX is not in memory!"); return -1;
	}

	swapfont();

	putch(27);putch('E');
	putch(27);putch('H');
	prints("Welcome to Polish Pong! by Robert H. Halstead, Aug 1980\n");
	prints("                        XPCW port by Miguel Garcia / FloppySoftware, 10 Sep 2015\n");
	Best = 32767;
	Speed = ISPEED;		/* governs how fast ball moves */
	while (playgame());

	bye();
  }

int playgame()
  {	int i,j;
	char buff[100];		/* temp */

	InChar = -1;			/* initially, no char typed in */
	putch(27);putch('e');
	outs(0,-1,"Type any key to start game: ");
	InChar = getch();
	if (InChar == QUITCH) 		/* clear the input character */
		bye();			/* and quit on control-C	*/
	prints("\rGood luck!                   \n");
	InChar = -1;		/* clear space out of input buffer */
	ballx = rand()%(MAXX-2) + 1;
	bally = rand()%(MAXY-2) + 1;
	ballxv = ballyv = 0;
	i = (rand()&2) - 1;
	if (rand()&1) ballxv = i; else ballyv = i;
	for (i = 0; i < MAXX; i++) for (j = 0; j < MAXY; j++) setboard(i,j,' ');
	for (i = 0; i < MAXX; i++) { setboard(i,0,HBAR); setboard(i,MAXY-1,HBAR); }
	for (i = 0; i < MAXY; i++) { setboard(0,i,VBAR); setboard(MAXX-1,i,VBAR); }
	setboard(0,0,0x96);		/* special corner pieces */
	setboard(0,MAXY-1,0x93);
	setboard(MAXX-1,0,0x9C);
	setboard(MAXX-1,MAXY-1,0x99);
	setboard(rand()%(MAXX-2)+1,rand()%(MAXY-2)+1,TARGET);
					/* place initial target */
	TargLeft = MAXTARG;	/* start with full complement of targets */
	putchx(ESC); putchx('f'); /* Cursor off */
	for (j = 0; j < MAXY; j++)
	  { for (i = 0; i < MAXX; i++) putchx(getboard(i,j));
	    putchx('\n');
	  }
	putch(27);putch('p');
	outs(SPEEDX-7,TIMY-1,"8:IncSpeed   2:DecSpeed   4:Put \\   6:Put /   5:ClrBlock   <-DEL:Reset   CtlC:Stop");
	outs(TIMX-11,TIMY,"Time Used: ");
	outs(TARGX-14,TARGY,"Targets Left: ");
	if (Best < 32767)
	  { sprintf(buff,"Best Time: %3d",Best);
	    outs(BESTX-11,BESTY,buff);
	  }
	outs(SPEEDX-7,SPEEDY,"Speed: ");
	putch(27);putch('q');
	putspeed();
	puttarg();
	MSecs = Secs = 0; puttime();
	ouch(BALL,ballx,bally);
	while (moveball());
	if (TargLeft == 0 && Secs < Best) Best = Secs;
	return(1);
  }

bye()
{
	putch(27);putch('H');
	putch(27);putch('e');

	swapfont();

	exit(0);
}

extern BYTE font[];

/* Swap the PCW internal font and the PPONG one
*/
swapfont()
{
	BYTE *fnt;
	BYTE chr[8];
	int i, k;

	fnt = font;

	for(i = 0; i < 256; ++i)
	{
		GetChrDef(i, chr);
		SetChrDef(i, fnt);

		for(k = 0; k < 8; ++k)
			*fnt++ = chr[k];
	}
}

/* PPONG font by FloppySoftware
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
 DEFB 000, 000, 000, 000, 000, 000, 000, 240
 DEFB 024, 024, 024, 024, 000, 000, 255, 126
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
