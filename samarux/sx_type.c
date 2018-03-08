/*	sx_type.c

	TYPE for Samarux.

	Print information about command type.

	Copyright (c) 2007 - 2018 Miguel I. Garcia Lopez.

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

	Usage:

	type [-abtx] [name ...]

	Examples:

	type -t cat ls

	Changes:

	08 Mar 2018 : 1.00 : 1st version.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_TYPE

#else

#include "samarux.h"

extern int sv_cmd_max;
extern WORD sv_cmd_name[]; /* char *[] */

#define TypeMain main

#endif

#define TYPE_FLG_A 1
#define TYPE_FLG_B 2
#define TYPE_FLG_T 4
#define TYPE_FLG_X 8

unsigned type_flags;

TypeMain(argc, argv)
int argc, argv[];
{
	int i, result;
	char *p;

	/* Defaults */

	result = type_flags = 0;

	/* Check arguments */

	for(i = 1; i < argc; ++i)
	{
		p = argv[i];

		if(*p == '-')
		{
			while(*++p)
			{
				switch(*p)
				{
					case 'a' : type_flags |= TYPE_FLG_A; break; /* Print all matches for each name */
					case 'b' : type_flags |= TYPE_FLG_B; break; /* Exclude search for builtins */
					case 't' : type_flags |= TYPE_FLG_T; break; /* Print a single word instead of a full sentence: alias, builtin, file or none */
					case 'x' : type_flags |= TYPE_FLG_X; break; /* Exclude search for aliases */
					default  : return ErrorOpt();
				}
			}
		}
		else
			break;
	}

	/* Process names if any */

	for(i = i; i < argc; ++i)
	{
		if(TypeGo(argv[i]))
			result = 1;
	}

	/* Return success or failure */

	return result;
}

TypeGo(name)
char *name;
{
	int i, found;
	char *str;
	FILE *fp;

	/* Flag for -a option */

	found = 0;

	/* Aliases */

	if(!(type_flags & TYPE_FLG_X))
	{
		if((str = AliasGet(name)))
		{
			if(type_flags & TYPE_FLG_T)
				puts("alias");
			else
				printf("%s is aliased to '%s'\n", name, str);

			if(!(type_flags & TYPE_FLG_A))
				return 0;

			found = 1;
		}
	}

	/* Builtins */

	if(!(type_flags & TYPE_FLG_B))
	{
		for(i = 0; i < sv_cmd_max; ++i)
		{
			if(!strcmp(sv_cmd_name[i], name))
			{
				if(type_flags & TYPE_FLG_T)
					puts("builtin");
				else
					printf("%s is a shell builtin\n", name);

				if(type_flags & TYPE_FLG_A)
				{
					found = 1;
					break;
				}

				return 0;
			}
		}
	}

	/* Externals */

	if((fp = fopen((str = BinFile(name)), "r")))
	{
		fclose(fp);

		if(type_flags & TYPE_FLG_T)
			puts("file");
		else
			printf("%s is %s\n", name, str);

		return 0;
	}

	/* Not found */

	if(found)
		return 0;

	if(type_flags & TYPE_FLG_T)
		puts("none");
	else
		printf("%s not found\n", name);

	return 1;
}

#undef TYPE_FLG_A
#undef TYPE_FLG_B
#undef TYPE_FLG_T
#undef TYPE_FLG_X
