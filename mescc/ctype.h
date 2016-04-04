/*	ctype.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Characters test and conversion functions.

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

	19 Dec 2000 : Last revision.
	16 Apr 2007 : GPL'd.

	int isalpha(char ch)
	int isdigit(char ch)
	int isxdigit(char ch)
	int isalnum(char ch)
	int isupper(char ch)
	int islower(char ch)
	int toupper(char ch)
	int tolower(char ch)
*/

#ifndef CTYPE_H

#define CTYPE_H

/*	int isalpha(char ch)

	Return TRUE if ch is a letter, else FALSE.
*/

#asm

isalpha:
	ld a,l
	ld hl,0
	cp 'A'
	ret c
	cp 'Z'+1
	jr  c,isalpha1
	cp 'a'
	ret c
	cp 'z'+1
	ret nc
isalpha1:
	inc l
	ret

#endasm

/*	int isdigit(char ch)

	Return TRUE if ch is a decimal digit, else FALSE.
*/

#asm

isdigit:
	ld a,l
        ld hl,0
	cp '0'
	ret c
	cp '9'+1
	ret nc
	inc l
	ret

#endasm

/*	int isxdigit(char ch)

	Return TRUE if ch is a hex digit, else FALSE.
*/

#asm

isxdigit
	LD	C,L
	CALL	isdigit
	RET	C
	LD	HL,0
	LD	A,C
	CP	'A'
	RET	C
	CP	'G'
	JR	C,isxdigit1
	CP	'a'
	RET	C
	CP	'g'
	RET	NC
isxdigit1
	INC	L
	RET

#endasm

/*	int isalnum(char ch)

	Return TRUE if ch is a letter or a decimal digit, else FALSE.
*/

#asm

isalnum
	LD	C,L
	CALL	isdigit
	RET	C
	LD	L,C
	JP	isalpha

#endasm

/*	int isupper(char ch)

	Return TRUE if ch is uppercase, else FALSE.
*/

#asm

isupper:
	ld a,l
        ld hl,0
	cp 'A'
	ret c
	cp 'Z'+1
	ret nc
	inc l
	ret

#endasm

/*	int islower(char ch)

	Return TRUE if ch is lowercase, else FALSE.
*/

#asm

islower:
	ld a,l
        ld hl,0
	cp 'a'
	ret c
	cp 'z'+1
	ret nc
	inc l
	ret
#endasm

/*	int toupper(char ch)

	Return uppercase value of ch.
*/

#asm

toupper:
	ld a,l
	cp 'a'
	ret c
	cp 'z'+1
	ret nc
	sub 20h
	ld l,a
	ret
#endasm

/*	int tolower(char ch)

	Return lowercase value of ch.
*/

#asm

tolower:
	ld a,l
	cp 'A'
	ret c
	cp 'Z'+1
	ret nc
	add 20h
	ld l,a
	ret
#endasm

#endif

