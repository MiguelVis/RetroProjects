/*	c_iocon.c

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	I/O console module.

	Copyright (c) 1999-2015 Miguel I. Garcia Lopez, FloppySoftware.

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
	26 Oct 2015 : Cleaned.
*/

/*	int co_ch(char c)

	Character output.
        '\n' must output '\r' & '\n'.
	Return character value.
*/

co_ch(c)
char c;
{
	return putchar(c);
}


/*	char *co_str(char *s)

	String output. Return pointer to string.
*/

co_str(s)
char *s;
{

#if C_USEPRINTF
	printf("%s",s);
#else
	char *p;

        p=s;

	while(*p)
		co_ch(*p++);
#endif
	return s;
}


/*	char *co_line(char *s)

	String & '\n' output. Return string pointer.
*/

co_line(s)
char *s;
{
	co_str(s);co_ch('\n');

        return s;
}

/*	int co_nl(void)

	Outputs '\n'. Return '\n' value.
*/

co_nl()
{
	return co_ch('\n');
}

/*	int co_dec(int n)

	Signed integer output. Return value.
*/

co_dec(n)
int n;
{
#ifdef C_USEPRINTF
	printf("%d",n);

	return n;
#else
	int i;

	i=n;

	if(i<0)
	{
		co_ch('-');
		i=-i;
	}

	xco_dec2(i);

	return n;
}

xco_dec2(n)
int n;
{
	int i;

	if(i=n/10)
		xco_dec2(i);

	co_ch(n % 10 + '0');
#endif

}

/*	int co_dec05(int n)

	Signed integer output.
	Five digits left filled with zeros.
	Return value.
*/

co_dec05(n)
int n;
{
#ifdef C_USEPRINTF
	printf("%05d",n);
#else
	int i;

	i=n;

	if(i < 10000)
	{
		co_ch('0');
		if(i < 1000)
		{
			co_ch('0');
			if(i < 100)
			{
				co_ch('0');
				if(i < 10)
					co_ch('0');
			}
		}
	}

	co_dec(i);
#endif
        return n;
}
