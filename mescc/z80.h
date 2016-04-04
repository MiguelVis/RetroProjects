/*	z80.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Z80 specific functions.

	Copyright (c) 1999-2014 Miguel I. Garcia Lopez

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

	13 Dec 2014 : 1st version.

	Public:

	unsigned int Z80in(int port)
	void Z80out(int port, int value)
	void Z80di(void)
	void Z80ei(void)	
*/

#ifndef Z80_H

#define Z80_H

/*	unsigned int Z80in(int port)

	Inputs a byte from a port.
*/

#asm

Z80in	LD	C,L
	IN	A,(C)
	LD	H,0
	LD	L,A
	RET
#endasm

/*	void Z80out(int port, int value)

	Outputs a byte to a port.
*/

#asm

Z80out	POP	DE
	POP	HL
	POP	BC
	PUSH	BC
	PUSH	HL
	PUSH	DE
	OUT	(C),L
	RET
#endasm

/*	void Z80di(void)

	Disables interrupts.
*/

#asm

Z80di	DI
	RET
#endasm

/*	void Z80ei(void)

	Enables interrupts.
*/

#asm

Z80ei	EI
	RET
#endasm

#endif

