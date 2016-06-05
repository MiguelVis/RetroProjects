/*	sx_if.c

	IF for Samarux.

	Conditional execution.

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

	if arg1 [not] eq arg2 command
	if arg1 [not] exist command
	if arg1 [not] in_env command

	Options:

	None.

	Examples:

	if $USER eq Mike echo Hello Mike!
	if profile.sx not exist echo No profile found!
	if USER not in_env echo Environment variable USER is not defined!

	Changes:

	15 Feb 2015 : 1.00 : 1st version.
	23 Feb 2015 : 1.01 : Now 'equal' is 'eq'.
	21 Mar 2015 : 1.02 : Added in_env test.
*/

#define SX_IF

#define IF_EQ 1
#define IF_EXIST 2
#define IF_IN_ENV 3

IfMain(argc, argv)
int argc, argv[];
{
	char *arg1, *arg2, *wrd, *q;
	int not, cmd, result, index;

	/* Set-up some variables */

	index = 1;
	not = cmd = result = 0;

	/* Get 1st argument */

	if((arg1 = IfGetArg(index++, argc, argv)) == NULL)
		return 1;

	/* Get 1st reserved word */

	if((wrd = IfGetArg(index++, argc, argv)) == NULL)
		return 1;

	/* NOT? */
	if(!strcmp("not", wrd))
	{
		++not;

		if((wrd = IfGetArg(index++, argc, argv)) == NULL)
			return 1;
	}

	/* EQ, EXIST, IN_ENV? */
	if(!strcmp("eq", wrd))
		cmd = IF_EQ;
	else if(!strcmp("exist", wrd))
		cmd = IF_EXIST;
	else if(!strcmp("in_env", wrd))
		cmd = IF_IN_ENV;
	else
		return Error("Bad syntax");

	/* Test specified condition */
	switch(cmd)
	{
		case IF_EQ :
			if((arg2 = IfGetArg(index++, argc, argv)) == NULL)
				return 1;

			result = strcmp(arg1, arg2) ? 0 : 1;
			break;

		case IF_EXIST :
			result = IfExist(arg1);
			break;

		case IF_IN_ENV :
			result = EnvGet(arg1) != NULL ? 1 : 0;
			break;
	}

	/* Check for command */
	if((q = IfGetArg(index, argc, argv)) == NULL)
		return 1;

	/* Check for NOT */
	if(not)
		result = 1 - result;

	/* Execute command if condition is TRUE */
	if(result)
	{
		if((q = UnParse(argc - index, argv + index)) == NULL)
			return 1;

		Execute(q);
		free(q);
	}

	/* Return success */
	return 0;
}

IfExist(fn)
char *fn;
{
	FILE *fp;

	if((fp = fopen(fn, "r")) == NULL)
		return 0;

	fclose(fp);

	return 1;
}

IfGetArg(index, argc, argv)
int index, argc, argv[];
{
	if(index < argc)
		return argv[index];

	ErrorPars();

	return NULL;
}

#undef IF_EQ
#undef IF_EXIST
#undef IF_IN_ENV
