/*	mem.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Memory functions.

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

	25 Oct 2000 : Last revision.
	16 Apr 2007 : GPL'd.

	Public:

	char *memset(char *dst, char data, int count)
	char *memcpy(char *dst, char *src, int count)
	int memcmp(char *mem1, char *mem2, int count)
*/

#ifndef MEM_H

#define MEM_H

/*	char *memset(char *dst, char data, int count)

	Fill 'count' bytes with 'data' into 'dst',
	and return pointer to 'dst'.
*/

#asm

memset:	POP	AF
	POP	BC
	POP	DE
	POP	HL
	PUSH	HL
	PUSH	DE
	PUSH	BC
	PUSH	AF
	PUSH	HL
memset2:
	LD	A,B
	OR	C
	JR	Z,memset3
	LD	(HL),E
	INC	HL
	DEC	BC
	JR	memset2
memset3:
	POP	HL
	RET
#endasm

/*	char *memcpy(char *dst, char *src, int count)

	Copy 'count' bytes from 'src' to 'dst',
	and return pointer to 'dst'.
*/

#asm

memcpy:
	POP	AF
	POP	BC
	POP	HL
	POP	DE
	PUSH	DE
	PUSH	HL
	PUSH	BC
	PUSH	AF
	PUSH	DE
	LD	A,B
	OR	C
	JR	Z,memcpy2
	LDIR
memcpy2
	POP	HL
	RET
#endasm

/*	int memcmp(char *mem1, char *mem2, int count)

	Compare 'count' bytes, and return:

	<1	(-1)	mem1 < mem2
	=0	(0)	mem1 == mem2
	>1	(+1)	mem1 > mem2
*/

#asm

memcmp
	POP	AF
	POP	BC
	POP	HL
	POP	DE
	PUSH	DE
	PUSH	HL
	PUSH	BC
	PUSH	AF

memcmp1
	LD	A,C
	OR	B
	JR	Z,memcmp2

	DEC	BC

	LD	A,(DE)
	CP	(HL)
	INC	DE
	INC	HL
	JR	Z,memcmp1	

memcmp2
	LD	HL,0
	RET	Z
	JR	NC,memcmp3
	DEC	HL
	RET

memcmp3
	INC	L
	RET
#endasm

#endif

