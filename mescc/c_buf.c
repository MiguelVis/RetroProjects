/*	c_buf.c

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	I/O buffer module.

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
	27 May 2007 : issym() expanded to more speed.
	              Minor changes in inchar().
	16 Aug 2015 : Minor changes in BfWordEq() and BfWordNeq().
	26 Oct 2015 : Cleaned.
	29 Nov 2015 : Added ChEq() y ChNeq().
*/

issym1st(c)
char c;
{
	return isalpha(c) || c=='_';
}

issym(c)
char c;
{
	return isalpha(c) || c=='_' || isdigit(c);
}

/*
	int InSymEq(char *s)

	Devuelve TRUE (y avanza) si en la posicion actual
	esta el simbolo apuntado por *s.
*/

InSymEq(s)
char *s;
{
	int i;

	InBlanks();

	i=strlen(s);

	if(!memcmp(s,line+lptr,i))
	{
		if(!issym(line[lptr+i]))
		{
			lptr+=i;
			return 1;
		}
	}

	return 0;
}

/*
	int InChEq(char c)

	Devuelve TRUE (y avanza) si en la posicion actual esta
	el caracter c.
*/ 

InChEq(c)
char c;
{
	InBlanks();

	if(BfEq(c))
	{
		BfGet();
		return 1;
	}

	return 0;
}

/*
	int InWordEq(int w)

	Devuelve TRUE (y avanza) si en la posicion actual esta 'xx'.
*/

InWordEq(w)
int w;
{
	InBlanks();

	if(BfWordEq(w))
	{
		BfGet();
		BfGet();
		return 1;
	}

	return 0;
}

inbyte()
{
	while(!Bf())
	{
		if(eof)
			return 0;

		BfRead();
	}

	return BfGet();
}

inchar()
{
	if(!Bf())
		BfRead();

	return eof ? 0 : BfGet();
}

/*
	void InBlanks(void)

	Avanza hasta el primer caracter no blanco.
*/

InBlanks()
{
	while(1)
	{
		while(BfEq(' ') || BfEq('\t'))
			BfGet();

		if(Bf())
			break;

		BfRead();

		if(eof)
			break;
	}
}

/*
	int ChEq(char c)

	Devuelve TRUE si en la posicion actual esta
	el caracter c.
*/ 

ChEq(c)
char c;
{
	InBlanks();

	return BfEq(c);
}

/*
	int ChNeq(char c)

	Devuelve TRUE si en la posicion actual no esta
	el caracter c.
*/ 

ChNeq(c)
char c;
{
	InBlanks();

	return BfNeq(c);
}

/*
	char Bf(void)

	Devuelve el caracter en la posicion actual.
*/

Bf()
{
	return line[lptr];
}

/*
	char BfNext(void)

	Devuelve el caracter siguiente a la posicion actual.
*/

BfNext()
{
	if(Bf())
		return line[lptr+1];

	return 0;
}

/*
	char BfGet(void)

	Devuelve el caracter en la posicion actual
	e incrementa el puntero.
*/

BfGet()
{
	char c;

	if(c=Bf())
		++lptr;

	return c;
}

/*
	int BfEq(char c)

	Devuelve TRUE si Bf() == c.
*/

BfEq(c)
char c;
{
	return Bf()==c;
}

/*
	int BfNeq(char c)

	Devuelve TRUE si Bf() != c.
*/

BfNeq(c)
char c;
{
	return Bf()!=c;
}

/*
	int BfNextEq(char c)

	Devuelve TRUE si BfNext() == c.
*/

BfNextEq(c)
char c;
{
	return BfNext()==c;
}

/*
	int BfNextNeq(char c)

	Devuelve TRUE si BfNext() != c.
*/

BfNextNeq(c)
char c;
{
	return BfNext()!=c;
}

/*
	int BfWordEq(int w)

	Devuelve TRUE si 'xx' esta al comienzo del buffer.
*/

BfWordEq(w)
int w;
{
	return w == ((Bf() << 8) + BfNext());
}

/*
	int BfWordNeq(int w)

	Devuelve TRUE si 'xx' no esta al comienzo del buffer.
*/

BfWordNeq(w)
int w;
{
	return w != ((Bf() << 8) + BfNext());
}

/*
	void BfBlanks(void)

	Avanza en el buffer hasta el primer caracter no blanco.
*/

BfBlanks()
{
	while(BfEq(' ') || BfEq('\t'))
		BfGet();
}

/*
	void BfRead(void)

	Lectura de una linea en el buffer.
*/

BfRead()
{
	eof=cpp_read();
}

/*
	void BfKill(void)

	Borra el buffer.
*/

BfKill()
{
	line[0]=lptr=0;
}
