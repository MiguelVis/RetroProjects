/*	strings.c

	Print strings from (binary) files.

	Copyright (c) 2014, 2015 Miguel Garcia / FloppySoftware.

	To build the CP/M version:

	cc strings
	zsm strings
	hexcom strings

	To build the SamaruX version:
	
	cc strings
	zmac strings.zsm --rel
	link strings [op]
	ren strings.x=strings.prl

	Usage:

	strings [-nNUMBER] filename ...

	Example:

	strings -n6 cc.com dump.x

	Revisions:

	16 Dec 2014 : v1.00 : Version for CP/M.
	26 Aug 2015 : v1.01 : Version for SamaruX as external command.
	07 Sep 2015 : v1.02 : Modified some messages and comments.
	22 Mar 2019 : v1.03 : Delete unused variable.

	To-do & ideas:

	- Print a banner with the filename, ONLY when files > 1 -- NOW PRINTS THE BANNER ALWAYS.
	- Add an option to disable banners (see prev. comment).

	Define only one of following defs.:
*/

/*#define FOR_CPM*/
#define FOR_SAMARUX

#ifdef FOR_CPM

/* CP/M version
   ------------
*/

#define CC_STDIO  /* Support for stdin, stdout, stderr */
#define CC_REDIR  /* Support for CP/M command line redirection */

#include <mescc.h>
#include <conio.h>
#include <fileio.h>
#include <string.h>
#include <printf.h>
#include <fprintf.h>
#include <ctype.h>
#include <redir.h>

#define OPTION_N 'N'

error(msg)
char *msg;
{
	fprintf(stderr, "ERROR [strings]: %s\n", msg); return 1;
}

#define ERR_BADOPT error("Bad option")
#define ERR_BADLEN error("Bad length")
#define ERR_FOPEN  error("Can't open file")

#endif

#ifdef FOR_SAMARUX

/* SamaruX version
   ---------------
*/

#include <samarux.h>

#define OPTION_N 'n'

#define ERR_BADOPT ErrorOpt()
#define ERR_BADLEN Error("Bad length")
#define ERR_FOPEN  ErrorOpen()

#endif

#define VERSION "Strings v1.03 / 22 Mar 2019\n\n(c) 2015-19 FloppySoftware"

#define MAX_LENGHT 16

main(argc, argv)
int argc, argv[];
{
	char *pch;
	int minlen, i;

	/* No command line arguments: show usage and exit */

	if(argc == 1)
	{
		fprintf(stderr, "%s\n\nUsage: strings [-n4] filename ...\n", VERSION);

		return 0;
	}

	/* Default values */

	minlen = 4;  /* Minimum lenght of strings */

	/* Parse command line arguments */

	for(i = 1; i < argc; ++i)
	{
		pch = argv[i];

		if(*pch == '-')  /* Look for -options */
		{
			if(*(++pch) == OPTION_N)
			{
				minlen = atoi(++pch);

				if(minlen < 1 || minlen > MAX_LENGHT)
					return ERR_BADLEN;
			}
			else
				return ERR_BADOPT;
		}
		else  /* Look for files */
		{
			find(pch, minlen);
		}
	}

	/* Success */

	return 0;
}

find(fn, minlen)
char *fn; int minlen;
{
	FILE *fp;
	int b, i, k, f;
	unsigned int off;
	char s[MAX_LENGHT];

	if((fp = fopen(fn, "rb")) == NULL)
		return ERR_FOPEN;

	printf("\n--> %s <--\n\n", fn);

	for(off = i = f = 0; (b = fgetc(fp)) != EOF; ++off)
	{
		if(isprint(b))
		{
			s[i++] = b;

			if(isalnum(b))
				++f;

			if(i == minlen)
			{
				if(f)
				{
					printf("0x%04x : ", off - i + 1);

					for(k = 0; k < i; ++k)
						putchar(s[k]);

					while((b = fgetc(fp)) != EOF)
					{
						++off;

						if(isprint(b))
							putchar(b);
						else
							break;
					}

					putchar('\n');
				}

				i = f = 0;
			}
		}
	}

	fclose(fp);
}

isprint(ch)
int ch;
{
	return (ch >= ' ' && ch < 0x7F) ? 1 : 0;
}
