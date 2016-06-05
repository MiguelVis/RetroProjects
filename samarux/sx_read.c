/*	sx_read.c

	READ for Samarux.

	Read a line from standard input and assign its value
	to an environment variable.

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

	read name

	Options:

	None

	Examples:

	read USER

	Changes:

	20 Jan 2015 : 1.00 : 1st version.
*/

#define SX_READ

#define READ_MAX_LEN 32

ReadMain(argc, argv)
int argc, argv[];
{
	char *input; int code, len, env;

	/* Check arguments */

	if(argc != 2)
		return ErrorPars();

	/* Create input buffer */

	if((input = malloc(READ_MAX_LEN + 1)) == NULL)
		return ErrorMem();

	/* Read a line from stdin */

	/* STDIN is a tty? *** FIX-ME : non standard */

	code = 0;

	if(stdin == NULL)
	{
		*input = 0;

		ReadLine(input, READ_MAX_LEN); putchar('\n');
	}
	else
	{
		if(fgets(input, READ_MAX_LEN, stdin) == NULL)
			code = ErrorRead();
		else
		{
			len = strlen(input);

			if(input[len - 1] != '\n')
				code = ErrorTooLong();
			else
				input[len - 1] = 0;
		}
	}

	/* Assign input to environment variable */

	if(!code)
	{
		if((env = EnvFind(argv[1])) != -1)
			EnvDel(env);

		if(EnvAdd(argv[1], input) == -1)
			++code;
	}

	/* Free input buffer */

	free(input);

	/* Return success or failure */

	return code;
}

#undef READ_MAX_LEN

