/*	cpm.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M.

	CP/M functions.

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

	19 Oct 2000 : Last revision.
	17 Apr 2004 : Added renfile function.
	16 Apr 2007 : GPL'd.
	12 Dec 2014 : Added FCX support.
	11 Feb 2015 : Added some BDOS #defines.
	03 Sep 2015 : Added FCX_DIR support (names for user areas, a sort of alias).
	08 Jan 2016 : Include ctype.h, mem.h libraries if CC_FCX is defined.

	Public:

	int bdos_hl(unsigned int bc, unsigned int de)
	unsigned char bdos_a(unsigned int bc, unsigned int de)
	unsigned char bdos_fcx_a(int fun, char *fcx)
	int setfcx(char *fname, char *fcx)
	int setfcb(char *fname, char *fcb)

	Private:

	TO BE WRITTEN SOME DAY

	Supports following #defs:

	#define CC_FCX     To support FCX (user number in file names).
	#define CC_FCX_DIR To support named directories in file names.

	Note:

	When CC_FCX_DIR is defined, you must supply a function that
	translates a directory name (string in upper case, up to 8
	characters in length + ZERO), into a drive & user number
	specification (string in upper case, up to 3 characters in
	length + ZERO).

	char *DirToDrvUsr(char *s)

	I.E.: DirToDrvUsr("ROOT") == "A0"

	It must to return NULL on unknown directory names.

	See UxGetDrvUsr() for an example.
*/

#ifndef CPM_H

#define CPM_H

#ifdef CC_FCX

#ifndef CTYPE_H
#include <ctype.h>
#endif

#ifndef MEM_H
#include <mem.h>
#endif

#endif

/*	BDOS FUNCTIONS
*/

#define BF_GETDRV 25
#define BF_SETDRV 14
#define BF_USER 32
#define BF_DMA 26
#define BF_FIND1ST 17
#define BF_FINDNEXT 18
#define BF_OSVER 12
#define BF_CONST 11
#define BF_FSIZE 35
#define BF_OPEN 15
#define BF_DELETE 19
#define BF_CREATE 22
#define BF_READSEQ 20
#define BF_WRITESEQ 21
#define BF_RENAME 23
#define BF_CLOSE 16
#define BF_ATTRIB 30

/*	unsigned int bdos_hl(unsigned int bc, unsigned int de)

	Calls to BDOS.
	Returns HL.
*/

#asm

bdos_hl
	POP	HL
	POP	DE
	POP	BC
	PUSH	BC
	PUSH	DE
	PUSH	HL
	JP	5
#endasm

/*	unsigned char bdos_a(unsigned int bc, unsigned int de)

	Calls to BDOS.
	Returns A.
*/

#asm

bdos_a
	POP	HL
	POP	DE
	POP	BC
	PUSH	BC
	PUSH	DE
	PUSH	HL
	CALL	5
	LD	H,0
	LD	L,A
	RET
#endasm

#ifdef CC_FCX

#define UX_FCX_SIZ 37
#define UX_FCX_USR 0
#define UX_FCX_DRV 1
#define UX_FCX_RRC 34	/* Random Record Number */

/*	unsigned char bdos_fcx_a(unsigned int bc, unsigned int de)

	Calls BDOS with FCX.
	Returns A.
*/

bdos_fcx_a(fun, fcx)
int fun; BYTE *fcx;
{
	int val; int old_user;

	if(*fcx)
	{
		if((old_user = bdos_a(BF_USER, 0xFFFF)) != *fcx - 1)
		{
			bdos_a(BF_USER, *fcx - 1);	/* Set user to FCX user */
			val = bdos_a(fun, fcx + 1);	/* Call BDOS function */
			bdos_a(BF_USER, old_user);	/* Set old user */

			return val;
		}
	}

	return bdos_a(fun, fcx + 1);
}

/*

gf long del field max 8.
   la long del field puede ser 0.
   un delim valido es byte 0.
   devuelve prt a delim encontrado, o siguiente byte
   a long. max (8).

	Returns:	Address of delimiter in str. Field filled (can be empty str.)
			NULL if len field > 8 or bad delimiter.

	Delimiters:	: . zero
*/


UxField(s, d)
char *s, *d;
{
	char c; int i;

	for(i=0; i!=8; ++i)
	{
		c=toupper(*s);

		if((c>='A'&& c<='Z') || (c>='0' && c<='9') || c=='$' || c=='_' || c=='*' || c=='?')
		{
			*d++=c; ++s;
		}
		else
			break;
	}

	*d=0; c=*s;

	if(c==':' || c=='.' || c==0)
		return s;

	return NULL;
}

/* -----------------------------------------------------
   This is an example, you must define your own funcion.
   -----------------------------------------------------

   char *DirToDrvUsr(char *s)

   Translate a directory name in upper case, to a drive
   and user name specification in upper case.
   Return NULL on unknown directory name.

DirToDrvUsr(s)
char *s;
{
	if(!strcmp(s, "ROOT"))
		return "A0";
	else if(!strcmp(s, "MESCC"))
		return "A1";
	else if(!strcmp(s, "TEMP"))
		return "M0";

	return NULL;
}
   -----------------------------------------------------
*/

UxGetDrvUsr(s)
char *s;
{
	int drv, usr;

#ifdef CC_FCX_DIR
	char *du;

	if((du = DirToDrvUsr(s)) != NULL)
		s = du;
#endif

	drv=usr=0xDD;

	if(*s>='A' && *s<='P')
		drv=*s++ -'A';

	if(isdigit(*s))
	{
			usr=0;

			do
			{
				if((usr=usr*10+*s-'0')>15)
					break;
			} while(isdigit(*++s));
	}

	if(*s)
		return -1;

	return (drv<<8) | usr;
}

UxPad(s, d, n)
char *s, *d; int n;
{
	int i; char f;

	f=' ';

	for(i=0; i!=n; ++i)
	{
		if(*s=='*')
		{
			f='?'; ++s; break;
		}
		else if(*s)
			*d++=*s++;
		else
			break;
	}

	if(*s)
		return NULL;

	while(i!=n)
	{
		*d++=f;	++i;
	}

	return 0xFFFF;
}

setfcx(s, fcx)
char *s; BYTE *fcx;
{
	char f[9]; WORD path; BYTE drv, usr;

	memset(fcx, 0, UX_FCX_SIZ);

	s=UxField(s, f);

	if(*s==':' && *f)
	{
		if((path=UxGetDrvUsr(f))==-1)
			return -1;

		drv=(path >> 8) & 0xFF;
		usr=path & 0xFF;

		fcx[UX_FCX_DRV]= drv==0xDD ? 0 : drv+1;
		fcx[UX_FCX_USR]= usr==0xDD ? 0 : usr+1;

		s=UxField(++s, f);
	}

	if((*s==0 || *s=='.') && *f)
	{
		if(UxPad(f, fcx+2, 8)==NULL)
			return -1;

		if(*s)
			s=UxField(++s, f);
		else
			*f=0;

		if(UxPad(f, fcx+10, 3)==NULL)
			return -1;
	}

	return *s || fcx[2]==0 ? -1 : 0;
}

/************************
UxFcxIsAmb(fcx)
BYTE *fcx;
{
	int i;

	for(i = 0; i < 11; ++i)
	{
		if(*++fcx == '?')
			return 1;
	}

	return 0;
}
*******************/

UxGetPath(path)
char *path;
{
	char s[9];

	path = UxField(path, s);

	if(*path ==':' && !path[1] && *s)
		return UxGetDrvUsr(s);

	return -1;
}

UxChdir(path)
char *path;
{
	int du, drv, usr;

	if((du = UxGetPath(path)) != -1)
	{
		drv = (du >> 8) & 0xFF;
		usr = du & 0xFF;

		if(drv!=0xDD && drv!=bdos_a(BF_GETDRV, 0))
			bdos_hl(BF_SETDRV, drv);

		if(usr!=0xDD && usr!=bdos_a(BF_USER, 0xFFFF))
			bdos_hl(BF_USER, usr);

		return du;
	}

	return -1;
}

#else

#asm

;	int setfcb(char *fname, char *fcb)
;
;	Devuelve 0 si ok, otro si error

setfcb:
	POP	BC
	POP	DE
	POP	HL
	PUSH	HL		;HL = Puntero a fname
	PUSH	DE		;DE = Puntero a fcb
	PUSH	BC

	INC	HL		;Se ha especificado unidad A: ... P: ?
	LD	A,(HL)
	DEC	HL
	CP	':'
	JR	NZ,sfcbdef
	LD	A,(HL)
	CALL	sfcbupp
	CP	'A'
	JR	C,sfcberr
	CP	'P' + 1
	JR	NC,sfcberr
	SUB	'A' - 1
	INC	HL
	INC	HL
	JR	sfcbdrv

sfcbdef
	XOR	A		;Unidad por defecto

sfcbdrv
	LD	(DE),A		;Fijar unidad en fcb
	INC	DE

sfcbnam
	LD	C,'.'		;Fijar nombre en fcb
	LD	B,8
	CALL	sfcbtok
	LD	A,B
	CP	8
	JR	Z,sfcberr
	LD	A,(HL)
	OR	A
	JR	Z,sfcbtyp
	CP	'.'
	JR	NZ,sfcberr
	INC	HL

sfcbtyp
	LD	C,0		;Fijar tipo en fcb
	LD	B,3
	CALL	sfcbtok
	LD	A,(HL)
	OR	A
	JR	NZ,sfcberr

	LD	A,0		;Fijar ceros en fcb sobrante
	LD	B,24
	CALL	sfcbset

	LD	HL,0		;Ok, fin
	RET

	;Entrada de error para sfcbtok

sfcbtke
	POP	HL		;Quitar direccion de la pila

sfcberr
	LD	HL,1		;Error, fin
	RET

	;Fijar token (nombre o tipo)
	;
	;Entrada:
	;	C = Caracter de terminacion ('.' para nombre, 0 para tipo)
	;	B = Maxima longitud del token (8 para nombre, 3 para tipo)
	;
	;Salida:
	;	B = Longitud restante

sfcbtok
	LD	A,(HL)		;Fin de cadena ?
	OR	A
	JR	Z,sfcbspc
	CP	C		;Caracter de terminacion ?
	JR	Z,sfcbspc
	CP	'*'		;Comodin ?
	JR	Z,sfcbamb

	;A partir de aqui, solo se aceptan caracteres validos

	CP	'#'		;# $ %
	JR	C,sfcbtke
	CP	'%' + 1
	JR	C,sfcbtks
	CP	'0'		;0 ... 9
	JR	C,sfcbtke
	CP	'9' + 1
	JR	C,sfcbtks
	CALL	sfcbupp		;? @ A ... Z
	CP	'?'
	JR	C,sfcbtke
	CP	'Z' + 1
	JR	C,sfcbtks
	CP	'_'		;_
	JR	NZ,sfcbtke

sfcbtks
	LD	(DE),A		;Fijar caracter del token en fcb
	INC	HL
	INC	DE
	DJNZ	sfcbtok		;Continuar hasta agotar la longitud
	RET

	;Fija resto del token con espacios,
	;y devuelve el contador de longitud tal y como estaba
	;para posterior comprobacion

sfcbspc
	LD	A,' '
	LD	C,B
	CALL	sfcbset
	LD	B,C
	RET

	;Fija '?' en el resto del token
	;y deja contador de longitud a cero

sfcbamb
	LD	A,'?'
	INC	HL

	;Rellena un area de memoria
	;
	;Entrada:
	;	DE = Direccion
	;	B  = Longitud
	;	A  = Byte

sfcbset
	LD	(DE),A
	INC	DE
	DJNZ	sfcbset
	RET

	;Convierte caracter a mayusculas
	;
	;Entrada:
	;	A = Caracter
	;
	;Salida:
	;	A = Caracter convertido a mayusculas si era a ... z

sfcbupp
	CP	'a'
	RET	C
	CP	'z' + 1
	RET	NC
	SUB	32
	RET

#endasm

#endif

#endif


