/*	fprintf.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	fprint function.

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

	23 Jan 2001 : Last revision.
	16 Apr 2007 : GPL'd.

	Public:

	int fprintf(FILE *fp, char *fmt, arg1, arg2, ...)

	Private:

	void xfpfout(int ch)
	void xfpfend(void)

	Notes:

	See XPRINTF.H
*/

#ifndef FPRINTF_H

#define FPRINTF_H

#ifndef XPRINTF_H
#include <xprintf.h>
#endif

#ifndef FILEIO_H
#include <fileio.h>
#endif

/*	int fprintf(FILE *fp, char *fmt, arg1, arg2, ...)

	Formatted output to file.

	Return -1 on error, else return number of chars written.
*/

#asm

fprintf:
	ADD	HL,HL
	ADD	HL,SP	;HL=Adr. fmt

	LD	DE,xfpfout
	PUSH	DE
	LD	DE,xfpfend
	PUSH	DE
	PUSH	HL

	INC	HL
	INC	HL	;HL=Adr. *fp
	LD	A,(HL)
	INC	HL
	LD	H,(HL)
	LD	L,A
	LD	(xfpfout+2),HL

	CALL	xprintf

	POP	BC
	POP	BC
	POP	BC

	RET
#endasm

/*	int xfpfout(char ch)

	Auxiliary function for xprintf.

	Output ch to file, and return TRUE on error, else FALSE.
*/

#asm

xfpfout:
	PUSH	HL	;Char.
	LD	HL,0	;*FP
	PUSH	HL

	CALL	fputc

	POP	BC
	POP	BC

	EX	DE,HL
	LD	HL,0

	LD	A,255
	CP	D
	RET	NZ
	CP	E
	RET	NZ

	INC	L
	RET
#endasm

/*	void xfpfend(void)

	Auxiliary function for xprintf.

	Ends formatted output.

	Currently do nothing.
*/

#asm

xfpfend:
	RET

#endasm

#endif
