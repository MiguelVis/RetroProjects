/*	c_iofile.c

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	I/O file module.

	Copyright (c) 1999-2015 Miguel I. Garcia Lopez

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

	16 Jan 2001 : Last revision.
	16 Apr 2007 : GPL'd.
	16 Aug 2015 : Cleaned and simplified (see c_main.h).
	25 Oct 2015 : Modified init_files(), fi_ch(), fo_dec(). Added some comments.
	26 Oct 2015 : Moved included files control to c_cpp.c. Cleaned.
	07 Nov 2015 : Modified fo_dec(). Now prints negative numbers as -32, instead of 0-32.
	18 Dec 2015 : Modified fo_dec() to print -32768 correctly.

	Notes:

	Filenames are stored in the form: A:FILENAME.TYP + ZERO
*/

fi_open(fname)
char *fname;
{
	if((fi_fp=fopen(fname,"r"))==0)
		errfile(EROPEN,fname);

	strcpy(fi_name,fname);
	fi_line=fi_eof=0;
}

fi_ch()
{
	if(fi_eof != EOF)
		return (fi_eof = fgetc(fi_fp));

	return EOF;
}

fi_close()
{
	if(fclose(fi_fp))
		errfile(ERCLOSE,fi_name);
}

fo_open(fname)
char *fname;
{
	if((fo_fp=fopen(fname,"w"))==0)
		errfile(EROPEN,fname);

	strcpy(fo_name,fname);
}

fo_close()
{
	if(fclose(fo_fp))
		errfile(ERCLOSE,fo_name);
}

fo_ch(c)
int c;
{
	if(fputc(c,fo_fp)==EOF)
		errfile(ERWRITE,fo_name);

	return c;
}

fo_str(s)
char *s;
{
	while(*s)
		fo_ch(*s++);
}

fo_nl()
{
	fo_ch('\n');
}

fo_colon()
{
	fo_ch(':');
}

fo_line(s)
char *s;
{
	fo_str(s);fo_nl();
}

fo_dec(n)
int n;
{
#ifdef C_USEPRINTF
	//if(n < 0) {
	//	fo_str("0-");
	//	n = -n;
	//}

	fprintf(fo_fp, "%d", n);
#else
	int i;

	if(n < 0)
	{
		/* Possible $8000 can't be negatived */
		/*if((number^0xFFFF)==0x7FFF){outstr("0-32768");return;}*/

		fo_ch('-');

		if(n == -32768) {
			fo_str("32768");
			return;
		}

		n = -n;
	}

	if(i = n / 10)
		fo_dec(i);

	fo_ch(n % 10 + '0');
#endif
}
