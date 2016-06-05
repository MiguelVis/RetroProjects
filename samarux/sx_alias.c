/*	sx_alias.c

	ALIAS for Samarux.

	Manage command alias.

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

	alias [name [value ...]]

	Examples:

	alias            -- print all alias
	alias lf ls -f   -- set alias
	alias lf         -- remove alias

	Changes:

	19 Dec 2014 : 1.00 : 1st version.
	29 Dec 2014 : 1.01 : Rewritten.
	04 Jan 2015 : 1.02 : Added AliasGet fn.
	08 Jan 2015 : 1.03 : Adapted to changes in shell.
	19 Jan 2015 : 1.04 : Now uses Samarux functions to control key tables.
	14 Feb 2015 : 1.05 : Adapted to changes in shell.
	03 Sep 2015 : 1.06 : Delete unused variable.
*/

#define SX_ALIAS

AliasMain(argc, argv)
int argc, argv[];
{
	int x;
	char *q;

	/* Check arguments */

	if(argc == 1)
		AliasPrint();
	else
	{
		if(!isalpha(*(q = argv[1])))
			return Error("Bad alias name");

		x = AliasFind(argv[1]);

		if(argc == 2)
		{
			if(x != -1)
				AliasDel(x);
			else
				return Error("Can't find alias");
		}
		else
		{
			if((q = UnParse(argc - 2, argv + 2)) != NULL)
			{
				if(AliasAdd(argv[1], q) != -1)
				{
					if(x != -1)
						AliasDel(x);

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

AliasPrint()
{
	KeyPrint(sv_alias_name, sv_alias_value, SX_MAX_ALIAS);
}

AliasFind(name)
char *name;
{
	return KeyFind(sv_alias_name, SX_MAX_ALIAS, name);
}

AliasGet(name)
char *name;
{
	return KeyGet(sv_alias_name, sv_alias_value, SX_MAX_ALIAS, name);
}

AliasAdd(name, value)
char *name, *value;
{
	return KeyAdd(sv_alias_name, sv_alias_value, SX_MAX_ALIAS, name, value);
}

AliasDel(pos)
int pos;
{
	KeyDel(sv_alias_name, sv_alias_value, SX_MAX_ALIAS, pos);
}

