/*	fileio.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	File I/O.

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
	17 Apr 2004 : Added functions: fread, fwrite, remove, rename.
	16 Apr 2007 : GPL'd.
	20 Apr 2007 : Quit "rt" and "wt" modes. Now "r" and "w" are text modes.
	10 Nov 2013 : Solved bug related to DMA in fopen, rename, remove.
	19 Nov 2013 : Reworked for UX.
	11 Dec 2013 : Added function: fgets.
	08 Dec 2014 : Added stdin, stdout, stderr support if CC_STDIO is defined.
	09 Dec 2014 : Added support to FCX if CC_FCX defined.
	18 Dec 2014 : Modified #define EOF (-1) to -1.
	16 Jan 2015 : Solved related memory bug in rename.
	16 Jan 2015 : Added FILENAME_MAX ANSI C definition.
	17 Feb 2015 : Solved bug in rename.
	07 Mar 2015 : Modified FILENAME_MAX value (now includes ZERO according to ANSI).
	09 Apr 2015 : Test ambiguous file names in fopen().
	03 May 2015 : Solved bug in fgets - last character was lost in long lines.
	17 Nov 2015 : Added FILENAME_MAX value when CC_FCX_DIR is defined.
	24 Dec 2015 : Added CC_FILEIO_SMALL define to exclude fread(), fwrite() and fgets().
	04 Jan 2016 : Removed some code from fread() and fwrite().
	08 Jan 2016 : Include mem.h library.

	Public:

	FILE *fopen(char *fname, char *fmode)
	int fclose(FILE *fp)
	int fgetc(FILE *fp)
	int fputc(int ch, FILE *fp)
	int feof(FILE *fp)
	int ferror(FILE *fp)
	int fread(char *ptr, int size, int nobj, FILE *fp)
	int fwrite(char *ptr, int size, int nobj, FILE *fp)
	char *fgets(char *str, int size, FILE *fp)
	int remove(char *fname)
	int rename(char *oldname, char *newname)

	Private:

	int xfgetc(FILE *fp)
	int xfputc(int ch, FILE *fp)
	int xfnamb(char *fn)

	Supports following #defs:

	#define CC_STDIO        To support stdin, stdout & stderr.
	#define CC_FCX          To support FCX (user number in file names, see cpm.h).
	#define CC_FCX_DIR      To support named directories in file names (see cpm.h).
	#define CC_FILEIO_SMALL To exclude fread(), fwrite() and fgets().
*/

#ifndef FILEIO_H

#define FILEIO_H

#ifndef CPM_H
#include <cpm.h>
#endif

#ifndef ALLOC_H
#include <alloc.h>
#endif

#ifndef MEM_H
#include <mem.h>
#endif

#ifdef CC_STDIO
#ifndef CONIO_H
#include <conio.h>
#endif
#endif

/*	STANDARD DEFs
*/

#define FILE unsigned char
#define EOF -1

#ifdef CC_FCX

#ifdef CC_FCX_DIR

#define FILENAME_MAX 22  /* dusrname:filename.typ + ZERO */

#else

#define FILENAME_MAX 17  /* duu:filename.typ + ZERO */

#endif

#else

#define FILENAME_MAX 15  /* d:filename.typ + ZERO */

#endif

/*	PRIVATE DEFs
*/

#define XF_READ   1	/* Read mode */
#define XF_WRITE  2	/* Write mode */
#define XF_BIN	4	/* Binary mode */
#define XF_EOF	8	/* End of file */
#define XF_ERR	16	/* I/O error */

#define XF_IMOD	0	/* ModE (1 byte) */
#define XF_IPOS	1	/* Position in buffer (1 byte) */
#define XF_IBUF	2	/* Buffer (128 bytes) */
#define XF_IFCX	130	/* FCX (37 bytes: USER (1) + FCB (36)) */

#ifdef CC_FCX

#define XF_ISIZ	167	/* Data block size */

#define fileop bdos_fcx_a
#define makefb setfcx

#else

#define XF_ISIZ	166	/* Data block size */

#define fileop bdos_a
#define makefb setfcb

#endif

/*	FILE *fopen(char *fname, char *fmode)

	Apertura de archivo. Devuelve puntero a FILE, o NULL
	en caso de error.
	Valores validos para 'fmode':

		"rb"	Lectura binario.
		"r"	Lectura texto.
		"wb"	Escritura binario.
		"w"	Escritura texto.

	En lectura de texto, '\r' es ignorado, '\n' es fin de linea.

	En escritura de texto, '\n' se convierte a '\r' mas '\n'.
*/

fopen(fname,fmode)
char *fname, *fmode;
{
	int mode;
	FILE *fp;

	/* Establecer el modo de apertura */

	if(*fmode=='r')
		mode = XF_READ;
	else if(*fmode=='w')
		mode = XF_WRITE;
	else
		return NULL;

	if(*(fmode+1)=='b')
		mode |= XF_BIN;
	else if(*(fmode+1))
		return NULL;

	/* Nombre de fichero no ambiguo */

	if(xfnamb(fname))
		return NULL;

	/* Obtener memoria */

	if(!(fp=malloc(XF_ISIZ)))
		return NULL;

	/* Crear fcb */

	if(makefb(fname,fp+XF_IFCX))
	{
		free(fp);
		return NULL;
	}

	/* Abrir fichero */

	bdos_hl(BF_DMA, fp+XF_IBUF);

	if(mode & XF_READ)
	{
		if(fileop(BF_OPEN, fp+XF_IFCX)==255)
		{
			free(fp);
			return NULL;
		}

		fp[XF_IPOS]=128;	/* No hay datos en buffer */
	}
	else
	{
		if(fileop(BF_FIND1ST, fp+XF_IFCX)!=255)
		{
			if(fileop(BF_DELETE, fp+XF_IFCX)==255)
			{
				free(fp);
				return NULL;
			}
		}

		if(fileop(BF_CREATE, fp+XF_IFCX)==255)
		{
			free(fp);
			return NULL;
		}

		fp[XF_IPOS]=0;	/* No hay datos en buffer */
	}

	/* Fijar indicadores */

	fp[XF_IMOD]=mode;	/* Modo de apertura */

	/* Devolver puntero a canal */

	return fp;
}

/*	int fgetc(FILE *fp)

	Lee un caracter y devuelve su valor, o EOF en caso de error.
*/

fgetc(fp)
FILE *fp;
{
	int c;

#ifdef CC_STDIO
	/* Consola? */

	if(fp == NULL)
	{
		if((c = getch()) == '\r')
			c = '\n';
		else if (c == 0x1A)
			return EOF;

		if(c == '\n')
			putch('\r');

		return putch(c);
	}
#endif

	/* Fichero abierto para lectura y sin errores? */

	if(!(fp[XF_IMOD] & XF_READ) || fp[XF_IMOD] & (XF_EOF + XF_ERR))
		return EOF;

	/* Leer caracter */

	c=xfgetc(fp);

	if(!(fp[XF_IMOD] & XF_BIN))
	{
		while(c=='\r')
			c=xfgetc(fp);

		if(c==0x1A)
		{
			fp[XF_IMOD]|=XF_EOF;
			c=EOF;
		}
	}

	/* Devolver caracter */

	return c;
}

/*	int xfgetc(FILE *fp)

	Funcion auxiliar para fgetc. Devuelve siguiente caracter,
	o EOF en caso de error.
*/

xfgetc(fp)
FILE *fp;
{
	/* Leer registro si hace falta */

	if(fp[XF_IPOS]==128)
	{
		bdos_hl(BF_DMA, fp+XF_IBUF);

		if(fileop(BF_READSEQ, fp+XF_IFCX))
		{
			fp[XF_IMOD]|=XF_EOF;
			return EOF;
		}	

		fp[XF_IPOS]=0;
	}

	/* Obtener caracter e incrementar posicion */

	return *(fp + XF_IBUF + fp[XF_IPOS]++);
}

/*	int fputc(int c, FILE *fp)

	Escritura de caracter. Devuelve su valor, o EOF en
	caso de error.
*/

fputc(c,fp)
int c;
FILE *fp;
{

#ifdef CC_STDIO
	/* Consola? */

	if(fp == NULL)
	{
		if(c == '\n')
			putch('\r');

		return putch(c);
	}
#endif

	/* Fichero abierto para escritura y sin error ? */

	if(!(fp[XF_IMOD] & XF_WRITE) || fp[XF_IMOD] & (XF_EOF + XF_ERR))
		return EOF;

	if(!(fp[XF_IMOD] & XF_BIN))
	{
		if(c=='\n')
		{
			if(xfputc('\r',fp)==EOF)
				return EOF;
		}
	}

	return xfputc(c,fp);
}

/*	int xfputc(int c, FILE *fp)

	Funcion auxiliar para fputc. Devuelve valor del
	caracter escrito, o EOF en caso de error.
*/

xfputc(c,fp)
int c;
FILE *fp;
{

	/* Almacenar caracter e incrementar posicion */

	*(fp + XF_IBUF + fp[XF_IPOS]++)=c;

	/* Escribir registro si hace falta */

	if(fp[XF_IPOS]==128)
	{
		bdos_hl(BF_DMA, fp + XF_IBUF);

		if(fileop(BF_WRITESEQ, fp + XF_IFCX))
		{
			fp[XF_IMOD]|=XF_ERR;
			return EOF;
		}	

		fp[XF_IPOS]=0;
	}

	/* Devolver caracter */

	return c;
}

/*	int fclose(FILE *fp)

	Cerrado de fichero. Devuelve 0 en caso de exito,
	EOF en caso de error.
*/

fclose(fp)
FILE *fp;
{

#ifdef CC_STDIO
	/* Consola? */

	if(fp == NULL)
		return 0;
#endif

	/* Modo escritura? */

	if(fp[XF_IMOD] & XF_WRITE)
	{
		while(fp[XF_IPOS])
		{
			if(xfputc(0x1A, fp)==EOF)
				return EOF;
		}
	}

	/* Cerrar el fichero */

	bdos_hl(BF_DMA, fp + XF_IBUF);

	if(fileop(BF_CLOSE, fp + XF_IFCX)==255)
		return EOF;

	/* Liberar memoria */

	free(fp);

	/* Ok, fichero cerrado */

	return 0;
}

/*	int feof(FILE *fp)

	Devuelve un valor distinto de cero si se ha llegado
	al final del fichero.
*/

feof(fp)
FILE *fp;
{

#ifdef CC_STDIO
	/* Consola? */

	if(fp == NULL)
		return 0;
#endif

	return fp[XF_IMOD] & XF_EOF;
}

/*	int ferror(FILE *fp)

	Devuelve un valor distinto de cero si se origino
	un error en el fichero.
*/

ferror(fp)
FILE *fp;
{

#ifdef CC_STDIO
	/* Consola? */

	if(fp == NULL)
		return 0;
#endif

	return fp[XF_IMOD] & XF_ERR;
}

#ifndef CC_FILEIO_SMALL

/*	int fread(char *ptr, int size, int nobj, FILE *fp)

	Lee nobj objetos de tamanyo size y los pone en ptr.
	Devuelve el numero de objetos leidos.
*/

fread(ptr, size, nobj, fp)
unsigned char *ptr;
int size, nobj;
FILE *fp;
{
	int cobj, csize, c;

	//if(!(fp[XF_IMOD] & XF_READ) || fp[XF_IMOD] & (XF_EOF + XF_ERR))
	//	return EOF;

	for(cobj=0; cobj!=nobj; ++cobj)
	{
		for(csize=0; csize!=size; ++csize)
		{
			if((c = fgetc(fp)) == EOF)
				return cobj;

			*ptr++=c;
		}
	}

	return cobj;
}

/*	int fwrite(char *ptr, int size, int nobj, FILE *fp)

	Escribe nobj objetos de tamanyo size desde ptr.
	Devuelve el numero de objetos escritos.
*/

fwrite(ptr, size, nobj, fp)
unsigned char *ptr;
int size, nobj;
FILE *fp;
{
	int cobj, csize;

	//if(!(fp[XF_IMOD] & XF_WRITE) || fp[XF_IMOD] & (XF_EOF + XF_ERR))
	//	return EOF;

	for(cobj=0; cobj!=nobj; ++cobj)
	{
		for(csize=0; csize!=size; ++csize)
		{
			if(fputc(*ptr++, fp) == EOF)
				return cobj;
		}
	}

	return cobj;
}

/*	char *fgets(char *str, int size, FILE *fp)

	Read a string. Returns 'str' on success, else NULL.
*/

fgets(str, size, fp)
char *str; int size; FILE *fp;
{
	int c;
	char *cs;

	cs = str;

	while(--size)
	{
		if((c = fgetc(fp)) == EOF)
			break;

		if((*cs++ = c) == '\n')
			break;
	}

	if(c == EOF && cs == str)
		return NULL;

	*cs = 0;

	return str;
}

#endif

/*	int remove(char *fname)

	Borra un archivo. Devuelve un valor distinto de cero si hay error.
*/

remove(fname)
char *fname;
{
	BYTE *fc;
	int code;

	code = 0xFF; /* Error by default */

#ifdef CC_FCX
	fc = malloc(37);
#else
	fc = malloc(36);
#endif

	if(!makefb(fname, fc))
	{
		bdos_hl(BF_DMA, 0x80);

		code = fileop(BF_DELETE, fc);
	}

	free(fc);

	return code != 0xFF ? 0 : -1;
}

/*	int rename(char *oldname, char *newname)

	Renombra un archivo. Devuelve un valor distinto de cero si hay error.
*/

rename(oldname, newname)
char *oldname, *newname;
{
	int code;

	code = 0xFF; /* Error by default */

#ifdef CC_FCX

	BYTE *fcx;

	fcx = malloc(54); /* 37 + 17 *** FIXME *** */

	if(!makefb(oldname, fcx))
	{
		if(!makefb(newname, fcx + 17))
		{
			/* FIXME : drive + user unchecked */

			memcpy(fcx + 17, fcx + 18, 16);

			bdos_hl(BF_DMA, 0x0080);

			code = fileop(BF_RENAME, fcx);
		}
	}

	free(fcx);

#else
	BYTE *fcb;

	fcb = malloc(52); /* 36 + 16 *** FIXME *** */

	if(!makefb(oldname, fcb))
	{
		if(!makefb(newname, fcb + 16))
		{
			/* FIXME : drive + user unchecked */

			bdos_hl(BF_DMA, 0x0080);

			code = fileop(BF_RENAME, fcb);
		}
	}

	free (fcb);
#endif

	return code != 0xFF ? 0 : -1;
}

/*	int xfnamb(char *fn)

	Return 1 if is an ambiguous file name, else 0.
*/

xfnamb(fn)
char *fn;
{
	while(*fn)
	{
		if(*fn == '?' || *fn == '*')
			return 1;

		++fn;
	}

	return 0;
}

#undef fileop
#undef makefb

#endif
