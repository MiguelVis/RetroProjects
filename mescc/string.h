/*	string.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	String functions.

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

	19 Mar 2001 : Last revision.
	16 Apr 2007 : GPL'd.

	int strlen(char *str)
	char *strcpy(char *dst, char *src)
	char *strcat(char *dst, char *src)
	char *strupr(char *str)
	int strcmp(char *str1, char *str2)
	char *strchr(char *str, char ch)
	int atoi(char *s)
*/

#ifndef STRING_H

#define STRING_H

/*	int strlen(char *str)

	Return string length.
*/

#asm
strlen:
	LD	D,H
	LD	E,L
	LD	BC,0FFFFH
	XOR	A
	CPIR
	OR	A
	SBC	HL,DE
	DEC	HL
	RET
#endasm

/*	char *strcpy(char *dst, char *src)

	Copy src to dst and return dst.
*/

#asm
strcpy:
	POP	BC
	POP	HL
	POP	DE
	PUSH	DE
	PUSH	HL
	PUSH	BC

	PUSH	DE

strcpy2:
	LD	A,(HL)
	LD	(DE),A
	INC	HL
	INC	DE
	OR	A
	JR	NZ,strcpy2
	POP	HL
	RET
#endasm

/*	char *strcat(char *dst, char *src)

	Copy src at end of dst, and return dst.
*/

#asm
strcat:
	POP	BC
	POP	HL
	POP	DE
	PUSH	DE
	PUSH	HL
	PUSH	BC

	PUSH	DE

strcat2
	LD	A,(DE)
	OR	A
	JR	Z,strcpy2
	INC	DE
	JR	strcat2
#endasm

/*	int strcmp(char *str1, char *str2)

	Compare strings and return:

	<0	(-1)	str1 < str2
	=0	(0)	str1 == str2
	>0	(+1)	str1 > str2
*/

#asm
strcmp
	POP	BC
	POP	HL
	POP	DE
	PUSH	DE
	PUSH	HL
	PUSH	BC
strcmp1
	LD	A,(DE)
	CP	(HL)
	JR	NZ,strcmp2

	OR	A
	JR	Z,strcmp2

	INC	DE
	INC	HL
	JR	strcmp1

strcmp2
	LD	HL,0
	RET	Z
	JR	NC,strcmp3
	DEC	HL
	RET
strcmp3
	INC	L
	RET
#endasm

/*	char *strchr(char *str, char ch)

	Search ch in str. Return pointer to ch, else 0.
*/

#asm
strchr
	POP	BC
	POP	DE
	POP	HL
	PUSH	HL
	PUSH	DE
	PUSH	BC

strchr2
	LD	A,(HL)
	CP	E
	RET	Z

	INC	HL
	OR	A
	JR	NZ,strchr2

	LD	H,A
	LD	L,A
	RET
#endasm

/*	char *strupr(char *str)

	Convert str to upper case.
*/

#asm
strupr
	POP	BC
	POP	HL
	PUSH	HL
	PUSH	BC

	PUSH	HL

strupr1
	LD	A,(HL)
	OR	A
	JR	Z,strupr3

	CP	'a'
	JR	C,strupr2
	CP	'z'+1
	JR	NC,strupr2
	SUB	32
	LD	(HL),A

strupr2
	INC	HL
	JR	strupr1

strupr3
	POP	HL
	RET
#endasm

/*	int atoi(char *s)

	Devuelve el valor de tipo int de una cadena.
	El calculo termina al encontrar un caracter que no sea un
	digito decimal (0-9), o al finalizar la cadena.
	Se admite el signo + o - al pricipio de la cadena.
*/

atoi(s)
char *s;
{
	int sign,val;

	if(*s=='+')
	{
		sign=1;
		++s;
	}
	else if(*s=='-')
	{
		sign=-1;
		++s;
	}
	else
		sign=1;

	val=0;

	while(*s>='0' && *s<='9')
		val=val*10+(*s++-'0');

	return val*sign;
}

#endif
