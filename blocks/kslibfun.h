/*	kslibfun.h

	More functions for kslib.h for CP/M and MESCC - Mike's Enhanced Small C Compiler.

	Copyright (c) 2012 Miguel I. Garcia Lopez, Valencia, Spain.

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

	02 Jul 2012 : Version 1.0.

	Functions:

	ScrOutRpt(ch, times)
	ScrOutStrRC(row, col, str)
	ScrBox(row, col, rows, cols, fill)
	ScrTitle(row, str)
*/

ScrOutRpt(ch, times)
int ch, times;
{
	while(times--)
		ScrOut(ch);
}

ScrOutStrRC(row, col, str)
int row, col; char *str;
{
	ScrSetRC(row, col); ScrOutStr(str);
}

ScrBox(row, col, rows, cols, fill)
int row, col, rows, cols, fill;
{
	int i;

	ScrSetRC(row++, col); ScrOut('+');
	ScrOutRpt('-', cols-2);
	ScrOut('+');

	for(i=rows-2; i; --i)
	{
		ScrSetRC(row, col); ScrOut('|');
		if(fill!=NULL)
			ScrOutRpt(fill, cols-2);
		ScrSetRC(row++, col+cols-1); ScrOut('|');
	}

	ScrSetRC(row, col); ScrOut('+');
	ScrOutRpt('-', cols-2);
	ScrOut('+');
}

ScrTitle(row, str)
int row; char *str;
{
	ScrOutStrRC(row, (SCR_COLS-strlen(str))/2, str);
}
