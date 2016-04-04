/*	printf.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	printf function.

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

	20 Oct 2000 : Last revision.
	16 Apr 2007 : GPL'd.

	Public:

	int printf(char *fmt, arg1, arg2, ...)

	Private:

	void xpfout(char ch)
	void xpfend(void)

	Notes:

	See XPRINTF.H
*/

#ifndef PRINTF_H

#define PRINTF_H

#ifndef XPRINTF_H
#include <xprintf.h>
#endif

#ifndef CONIO_H
#include <conio.h>
#endif

/*	int printf(char *fmt, arg1, arg2, ...)

	Formatted output to console.

	Return -1 on error, else return number of chars written.

	Currently always return number of chars written.
*/

#asm

printf:
	ADD	HL,HL
	ADD	HL,SP
	INC	HL
	INC	HL	;HL=Adr. fmt

	LD	DE,xpfout
	PUSH	DE
	LD	DE,xpfend
	PUSH	DE
	PUSH	HL

	CALL	xprintf

	POP	BC
	POP	BC
	POP	BC

	RET
#endasm

/*	int xpfout(char ch)

	Auxiliary function for xprintf.

	Output ch to console, and return TRUE on error, else FALSE.

	Currently always return FALSE.
*/

#asm

xpfout:
	PUSH	HL
	CALL	putchar
	POP	BC
	LD	HL,0
	RET

#endasm

/*	void xpfend(void)

	Auxiliary function for xprintf.

	Ends formatted output.

	Currently do nothing.
*/

#asm

xpfend:
	RET

#endasm

#endif
