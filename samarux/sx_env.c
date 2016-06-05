/*	sx_env.c

	ENV for Samarux.

	Environment variables support.

	Copyright (c) 2007 - 2015 Miguel I. Garcia Lopez.

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

	env [name [value ...]]
	env [-s] name value ...

	Examples:

	env              -- print all variables
	env HOME A00:    -- set variable
	env MYDIR        -- delete variable

	Examples using -s option (skip spaces):

	env -s HELP $HOME somehelp.doc   -- HELP == A00:somehelp.doc
	env -s NAME John Smith           -- NAME == JohnSmith

	Changes:

	22 Dec 2014 : 1.00 : 1st version.
	24 Dec 2014 : 1.01 : Minor changes.
	25 Dec 2014 : 1.02 : Variable names must start with A-Z|a-z to avoid conflicts with batch variables.
	02 Jan 2015 : 1.03 : Added EnvGet function.
	08 Jan 2015 : 1.04 : Adapted to changes in shell.
	19 Jan 2015 : 1.05 : Now uses Samarux functions to control key tables.
	20 Jan 2015 : 1.06 : Added ErrorFindVar().
	14 Feb 2015 : 1.07 : Adapted to changes in shell.
	04 Mar 2015 : 1.08 : Added -s option.
*/

#define SX_ENV

EnvMain(argc, argv)
int argc, argv[];
{
	int i, x, flag;
	char *q;

	/* Without arguments ? */

	if(argc == 1)
		EnvPrint();
	else
	{
		/* Defaults */

		flag = i = 1;

		/* Option ? */

		if(!strcmp(argv[1], "-s"))
		{
			--flag; ++i;
		}

		/* Check name */

		if(argc == i)
			return ErrorPars();

		if(!isalpha(*(q = argv[i])))
			return Error("Bad name");

		/* Name exists ? */

		x = EnvFind(argv[i++]);

		if(argc == i)
		{
			/* No values */

			/* Check bad option */

			if(!flag)
				ErrorPars();

			/* Delete variable */

			if(x != -1)
				EnvDel(x);
			else
				return ErrorFindVar();
		}
		else
		{
			/* Assign values to variable */

			/* *** printf("UnParseEx(%d, %d, %d)\n", argc - i, argv + i, flag); *** */

			if((q = UnParseEx(argc - i, argv + i, flag)) != NULL)
			{
				/* *** printf("EnvAdd(%s, %s)\n", argv[i - 1], q); *** */

				if(EnvAdd(argv[i - 1], q) != -1)
				{
					if(x != -1)
						EnvDel(x);

					free(q);

					return 0;
				}

				free(q);
			}

			return 1;
		}
	}

	return 0;
}

EnvPrint()
{
	KeyPrint(sv_env_name, sv_env_value, SX_MAX_ENV);
}


EnvFind(name)
char *name;
{
	return KeyFind(sv_env_name, SX_MAX_ENV, name);
}

EnvGet(name)
char *name;
{
	return KeyGet(sv_env_name, sv_env_value, SX_MAX_ENV, name);
}

EnvAdd(name, value)
char *name, *value;
{
	return KeyAdd(sv_env_name, sv_env_value, SX_MAX_ENV, name, value);
}

EnvDel(pos)
int pos;
{
	KeyDel(sv_env_name, sv_env_value, SX_MAX_ENV, pos);
}


