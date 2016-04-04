/*	xprintf.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Formatted output.

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

	Public:

	int xprintf(funout, funend, adrpars)

	Private:

	void pf_out(char c)	Salida de caracter
	void pf_s(char *s)	Salida de cadena
	void pf_cf(char c)	Salida de caracter con formato
	void pf_sf(char *s)	Salida de cadena con formato
	void pf_dec(int val)	Salida de decimal con formato
	void pf_dec2(int val)	Funcion auxiliar
	void pf_hex(int val)	Salida de hexadecimal con formato
	void pf_hex2(int val)	Funcion auxiliar

	Todas las funciones de salida con formato llaman a
	pf_sf, esta a pf_s, y esta a pf_out.
*/

#ifndef XPRINTF_H

#define XPRINTF_H

#ifndef STRING_H
#include <string.h>
#endif

/*
	VARIABLES GLOBALES
*/

unsigned char xpf_err;	/* TRUE si error */

extern int xpf_out;	/* Funcion de salida */
extern int xpf_end;	/* Funcion de finalizacion */

int xpf_fw;		/* Ancho */
unsigned char xpf_fa;	/* Alineacion: 0-Izquierda, 1-Derecha */
unsigned char xpf_fz;	/* Rellenado con ceros: 0-No, 1-Si */

int xpf_cnt;	/* Contador de caracteres enviados */

/*
	int xprintf(funout, funend, adrpars)

	Devuelve -1 si hay error en la salida.

	%<-><0><ancho><tipo>

	-	Salida alineada a la izquierda, en otro caso, a la derecha.
	0	Rellenado con ceros en alineacion a la derecha.
	ancho	Ancho de la salida para alineacion. Si el ancho especificado
		es menor que el del argumento, la salida se produce sin
		alineacion. Cuidado con sprintf!!!
	tipo	d	Entero decimal con signo.
		u	Entero decimal sin signo.
		x	Entero hexadecimal.
		s	Cadena.
		c	Caracter.

*/

xprintf(funout, funend, adrpars)
int funout, funend;
int *adrpars;
{
	int *parg;	/* Puntero a los parametros */
	char *pfor;	/* Puntero a la cadena de formato */
	int ivalue;
	char ch;

	/* Inicializacion */

	xpf_out=funout;
	xpf_end=funend;

	pfor=*adrpars;
	parg=--adrpars;

	xpf_err=xpf_cnt=0;

	/* A recorrer la cadena de formato! */

	while((ch=*pfor++))
	{
		/*ch=*pfor++;*/

		if(ch=='%')
		{
			if(*pfor=='-')
			{
				xpf_fa=0;	/* Izquierda */
				pfor++;
			}
			else
				xpf_fa=1;	/* Derecha */

			if(*pfor=='0')
			{
				xpf_fz=1;
				pfor++;
			}
			else
				xpf_fz=0;

			xpf_fw=0;

			while(*pfor>='0' && *pfor<='9')
				xpf_fw=xpf_fw*10+(*pfor++)-'0';

			ch=*pfor;

			if(ch=='d')
			{
				pfor++;
				ivalue=*parg--;
				pf_dec(ivalue);
			}
			else if(ch=='u')
			{
				pfor++;
				ivalue=*parg--;
				pf_udec(ivalue);
			}
			else if(ch=='x')
			{
				pfor++;
				ivalue=*parg--;
				pf_hex(ivalue);
			}
			else if(ch=='c')
			{
				pfor++;
				pf_cf(*parg--);
			}
			else if(ch=='s')
			{
				pfor++;
				pf_sf(*parg--);
			}
			else
				pf_out(ch);
		}
		else
			pf_out(ch);

		if(xpf_err)
			break;
	}

	pf_end();

	return xpf_err ? -1 : xpf_cnt;
}

/*
	void pf_sf(char *s)
	-------------------

	Salida de cadena con formato.
*/

pf_sf(s)
char *s;
{
	int len;
	char fill;

	if(xpf_fw)
	{
		if((len=strlen(s))<xpf_fw)
		{
			xpf_fw=xpf_fw-len;
			if(xpf_fa)	/* Derecha */
			{
				if(xpf_fz)
					fill='0';
				else
					fill=' ';
				while(xpf_fw--)
					pf_out(fill);
				pf_s(s);

			}
			else		/* Izquierda */
			{
				pf_s(s);
				while(xpf_fw--)
					pf_out(' ');
			}

			return;
		}
	}

	pf_s(s);
}

/*
	void pf_cf(char c)
	------------------

	Salida de caracter con formato.
*/

pf_cf(c)
char c;
{
	char tmp[2];

	tmp[0]=c;
	tmp[1]=0;

	pf_sf(tmp);
}

unsigned char xpf_dst[7];
unsigned char *xpf_dpt;

pf_dec(i)
int i;
{
	xpf_dpt=xpf_dst;

	if(i<0)
	{
		*xpf_dpt++='-';
		i=-i;
	}

	pf_dec2(i);

	*xpf_dpt=0;

	pf_sf(xpf_dst);
}

pf_dec2(i)
int i;
{
	int n;

	if(n=i/10)
		pf_dec2(n);

	*xpf_dpt++=i%10+'0';
}

pf_udec(i)
unsigned i;
{
	xpf_dpt=xpf_dst;

	pf_udec2(i);

	*xpf_dpt=0;

	pf_sf(xpf_dst);
}

pf_udec2(i)
unsigned i;
{
	unsigned n;

	if(n=i/10)
		pf_udec2(n);

	*xpf_dpt++=i%10+'0';
}

pf_hex(i)
unsigned i;
{
	xpf_dpt=xpf_dst;

	pf_hex2(i);

	*xpf_dpt=0;

	pf_sf(xpf_dst);
}

pf_hex2(i)
unsigned i;
{
	unsigned n;

	if(n=i/16)
		pf_hex2(n);

	i%=16;

	*xpf_dpt++ = i < 10 ? '0'+i : 'A'+i-10;
}

/*
	void pf_s(char *s)
	------------------

	Salida de cadena.
*/

pf_s(s)
char *s;
{
	while(*s)
		pf_out(*s++);
}

/*
	void pf_out(char c)
	-----------------

	Salida de caracter.
*/

#asm
pf_out:
	PUSH	HL
	DEFB	0CDH
xpf_out:
	DEFW	0
	POP	BC

	EX	DE,HL

	LD	HL,(xpf_cnt)
	INC	HL
	LD	(xpf_cnt),HL

	LD	A,D
	OR	E
	RET	Z
;;	LD	A,255
	LD	(xpf_err),A
	RET
#endasm

/*
	void pf_end(void)
*/

#asm
pf_end:
	DEFB	0C3H
xpf_end:
	DEFW	0
#endasm

#endif
