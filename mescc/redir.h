/*	redir.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Command line redirection.

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

	08 Dec 2014 : 1st version.

	Public:

	int redir(int argc, char *argv[])
*/

#ifndef REDIR_H

#define REDIR_H

/*	int redir(int argc, char *argv[])

	Stdin & stdout redirection in command line.
	Returns number of args after parsing redirection.
*/

redir(argc, argv)
int argc, argv[];
{
	int i, z; char *p;
	char *fnin, *fnout;
	FILE *fp;

	fnin = fnout = NULL;

	for(i = 1; i < argc; ++i)
	{
		p = argv[i];

		if((*p == '<' || *p == '>') && !(p[1]))
		{
			if(i + 1 == argc)
				return -1;	/* No filename */

			if(*p == '<')
				fnin = argv[i + 1];
			else
				fnout = argv[i + 1];

			argc -= 2;

			for(z = i; z < argc; ++z)
				argv[z] = argv[z + 2];
			--i;
		}
	}

	if(fnin != NULL)
	{
		if((fp = fopen(fnin, "r")) != NULL)
			stdin = fp;
		else
			return -2;
	}

	if(fnout != NULL)
	{
		if((fp = fopen(fnout, "w")) != NULL)
			stdout = fp;
		else
			return -3;
	}

	return argc;
}

#endif

