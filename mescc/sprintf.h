/*	sprintf.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	sprintf function.

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
	14 Apr 2015 : Ammended a bad closed comment.

	Public:

	int sprintf(char *dst, char *fmt, arg1, arg2, ...)

	Private:

	int xspfout(char ch)
	void xspfend(void)

	Notes:

	See XPRINT.H
*/

#ifndef SPRINTF_H

#define SPRINTF_H

#ifndef XPRINTF_H
#include <xprintf.h>
#endif

/*	int sprintf(char *dst, char *fmt, arg1, arg2, ...)

	Formatted output to memory.

	Return -1 on error, else return number of chars written.

	Currently always return number of chars written.
*/

#asm

sprintf:
	ADD	HL,HL
	ADD	HL,SP	;HL=Adr. fmt

	LD	DE,xspfout
	PUSH	DE
	LD	DE,xspfend
	PUSH	DE
	PUSH	HL

	INC	HL
	INC	HL	;HL=Adr. dst
	LD	A,(HL)
	INC	HL
	LD	H,(HL)
	LD	L,A
	LD	(xspfout+2),HL

	CALL	xprintf

	POP	BC
	POP	BC
	POP	BC

	RET
#endasm

/*	int xspfout(char ch)

	Auxiliary function for xprintf.

	Output ch to memory, and return TRUE on error, else FALSE.

	Currently always return FALSE.
*/

#asm

xspfout:
	LD	A,L
	LD	HL,0	;Adr.
	LD	(HL),A

	INC	HL
	LD	(xspfout+2),HL

	LD	HL,0

	RET
#endasm

/*	void xspfend(void)

	Auxiliary function for xprintf.

	Ends formatted output, writing a zero byte.
*/

#asm

xspfend:
	LD	HL,(xspfout+2)
	LD	(HL),0
	RET

#endasm

#endif
