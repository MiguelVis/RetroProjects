/*	conio.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M.

	Console I/O.

	Copyright (c) 1999-2015 Miguel I. Garcia Lopez / FloppySoftware, Spain

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

	22 Jan 2001 : Last revision.
	16 Apr 2007 : GPL'd.
	21 Apr 2007 : Changed puts for ANSI compatibility.
	15 May 2007 : Bug solved - added LF output to puts.
	13 Jul 2014 : Added kbhit().
	08 Dec 2014 : Added support for stdin, stdout & stderr.
	31 Dec 2014 : Solved bug in putstr when characters are > 0x7F.
	20 Dec 2015 : Added macro CC_CONIO_BIOS to support direct console I/O using BIOS, instead of BDOS.
	08 Jan 2015 : Modified getch() when access BDOS (fn. 6 instead of 1).

	Public:

	int putch(int ch)
	int getch(void)
	int kbhit(void)
	int getchar(void)
	int putchar(int ch)
	int putstr(char *s)
	int puts(char *s)

	Supports following #defs:

	#define CC_STDIO      Support to stdin, stdout & stderr.
	#define CC_CONIO_BIOS Support to direct console I/O.
*/

#ifndef CONIO_H

#define CONIO_H

/*	int putch(int ch)

	Sends character ch to the console.
	Returns ch.
*/

#ifdef CC_CONIO_BIOS
#asm

putch
	PUSH HL
	LD   C,L
	LD   E,9
	CALL xbios
	POP  HL
	RET

xbios
	LD   HL,(1)
	LD   D,0
	ADD  HL,DE
	JP   (HL)

#endasm
#else
#asm

putch
	PUSH	HL
	LD	C,2
	LD	E,L
	CALL	5
	POP	HL
	RET
#endasm
#endif

/*	int getch(void)

	Gets a character from the console without echo. 
	Waits until a character is available.
	Returns character.
*/

#ifdef CC_CONIO_BIOS
#asm

getch
	LD   E,6
	CALL xbios
	LD   H,0
	LD   L,A
	RET

#endasm
#else
#asm

getch
	LD	C,6
	LD	E,255
	CALL	5
	OR	A
	JR	Z,getch
	LD	H,0
	LD	L,A
	RET
#endasm
#endif

/*	int kbhit(void)

	Tests console input status.
	Returns !=0 if a character is available, 0 otherwise.
*/

#ifdef CC_CONIO_BIOS
#asm

kbhit
	LD   E, 3
	CALL xbios
	LD   H,A
	LD   L,A
	RET

#endasm
#else
#asm

kbhit
	LD	C,11
	CALL	5
	LD	H,A
	LD	L,A
	RET
#endasm
#endif

/*	int getchar(void)

	Gets a character from the console or stdin.
	Waits until a character is available.

	#ifdef  CC_STDIO: Returns a character from stdin, or EOF on end of file or error.
	#ifndef CC_STDIO: Returns a character from the console. Echoes the character.
*/

getchar()
{

#ifdef CC_STDIO

	return fgetc(stdin);

#else

	return putchar(getch());

#endif

}

/*	int putchar(int ch)

	Sends a character to the console or stdout.

	#ifdef  CC_STDIO: Returns ch, or EOF on error.
	#ifndef CC_STDIO: Returns ch.
*/

putchar(ch)
int ch;
{

#ifdef CC_STDIO

	return fputc(ch, stdout);

#else

	if(ch == '\n')
		putch('\r');

	return putch(ch);

#endif

}

/*	int putstr(char *s)

	Sends a string to the console or stdout.

	#ifdef  CC_STDIO: Returns the number of characters sent, or EOF on error.
	#ifndef CC_STDIO: Returns a non-negative value to indicate success.
*/

putstr(s)
char *s;
{

#ifdef CC_STDIO

	/* FIXME : Better if call to fputs (if available) */

	int i, c;

	i = 0;

	while(*s)
	{
		/* FIXME : -1 hardcoded -- < 0 causes strange
 		   behaviour if ch > 0x7F */

		if((c = putchar(*s++)) == -1)
			return c;
		++i;
	}

	return i;
#else
	while(*s)
		putchar(*s++);

	return 0;
#endif

}

/*	int puts(char *s)

	Sends a string, plus a new line, to the console or stdout.

	#ifdef  CC_STDIO: Returns the number of characters sent, or EOF on error. -- FIXME
	#ifndef CC_STDIO: Returns a non-negative value to indicate success.
*/

puts(s)
char *s;
{
	putstr(s);

	return putchar('\n');
}

#endif
