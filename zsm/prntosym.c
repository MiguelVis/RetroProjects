/*	prntosym.c

	Extract symbols from a PRN file (from ZSM v2.8).

	(C) 2015 FloppySoftware (Miguel I. Garcia Lopez, Spain).

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 2 of the License, or (at your
	option) any later version.

	This program is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

	CONTACT:

	www.floppysoftware.vacau.com
	cpm-connections.blogspot.com
	floppysoftware@gmail.com

	USAGE:

	prntosym source [> destination]

	TO COMPILE WITH MESCC:

	cc prntosym
	ccopt prntosym.zsm
	zsm prntosym
	hextocom prntosym

	REVISIONS:

	20 Aug 2015 : v1.00 : Initial version.
	21 Aug 2015 : v1.01 : Added SKIP_INTERNAL and SKIP_SAMARUX defines.
	                      Added EXIT to skipped symbol names in SamaruX.
	24 Aug 2015 : v1.02 : Minor changes in file output.
	04 Sep 2015 : v1.03 : Modified some comments and messages.
	
	NOTES:
	
	It was developed to support SamaruX external commands, but it can be useful on other matters too.
	
	It skips some symbol names, if you define:
	
	SKIP_INTERNAL : To skip symbols finishing with a decimal digit, like 'a32' (MESCC internal ones).
	
	SKIP_SAMARUX  : To skip following symbols:
	
			CCFREEMEM
			EXIT
			MAIN
*/

/* MESCC DEFs
*/

#define CC_STDIO     /* Support for stdin, stdout, stderr */
#define CC_REDIR     /* Support for command line redirection */

#define CC_NO_SWITCH /* No switch */

/* MESCC libraries
*/

#include <mescc.h>
#include <conio.h>
#include <ctype.h>
#include <fileio.h>
#include <fprintf.h>
#include <string.h>
#include <mem.h>
#include <redir.h>

/* DEFs and globals
*/

#define SKIP_INTERNAL
#define SKIP_SAMARUX

#define VERSION "1.03 / 04 Sep 2015\n\n(c) 2015 FloppySoftware"

#define BUF_LEN 127  /* Max. lenght for input buffer */
#define BUF_SIZ 128  /* Size for input buffer */

FILE *fp;            /* Input file FP */
char buf[BUF_SIZ];   /* Input buffer */

/* Main
 */

main(argc, argv)
int argc, argv[];
{
	/* Show banner */

	fprintf(stderr, "PrnToSym v%s\n\n", VERSION);

	/* Check arguments */

	if(argc != 2)
	{
		fprintf(stderr, "Usage: prntosym inputfile [> outputfile]\n");
		exit(1);
	}

	/* Process input file */

	procfile(argv[1]);
	
	/* End */
	
	fprintf(stderr, "Done.\n");
}

/* Process input file
*/

procfile(fn)
char *fn;
{
	int k, flag;

	/* Open file */

	if((fp = fopen(fn, "r")) == NULL)
		error("Opening input file");

	/* Process file */
	
	flag = 0;

	while(1)
	{
		/* Read line */

		if(fgets(buf, BUF_SIZ, fp) == NULL)
			break;

		/* Check EOL */

		k = strlen(buf);

		if(buf[--k] != '\n')
			error("Line too long or missing newline");

		/* Clear LF and put a ZERO instead */

		buf[k] = 0;

		/* Process line */
		
		if(flag)
			procline();
		else if(!memcmp(buf, "Next", 4))  /* ie: Next address: CE05H */
			++flag;
	}

	/* Close file */

	fclose(fp);
}

/* Process input line
*/

procline()
{
	char *p, name[12], adr[5];
	int k, x;

	/* Link to line */

	p = buf;
	
	/* Can be an empty or title line */
	
	if(*p == 12 || !(*p))
		return;

	/* Each line can hold up to 4 symbols: */
	
	/* NUMBER	    52C5    NXTLAB	BC5D	OPFNAME     BC7C    ORFN	6144 */
	
	while(isalpha(*p) || *p == '_')
	{
		/* Symbol name */
		
		k = 0;
		
		do {
			name[k++] = *p++;
			
			if(k == 12)
				error("Symbol name too long");
			
		} while(isalpha(*p) || *p == '_' || isdigit(*p));
		
		name[k] = 0;

		/* Skip blanks */
		
		while(*p == ' ' || *p == '\t')
			++p;
		
		/* Address */
		
		x = 0;
		
		while(isxdigit(*p)) {
			adr[x++] = *p++;
			
			if(x == 5)
				error("Bad symbol address");
		}
		
		adr[x] = 0;
		
		/* Skip blanks */
		
		while(*p == ' ' || *p == '\t')
			++p;

#ifdef SKIP_INTERNAL		
		/* Skip symbols finishing with a decimal digit (MESCC internal ones) */
		
		if(isdigit(name[k - 1]))
			continue;
#endif

#ifdef SKIP_SAMARUX
		/* Skip some symbols for SamaruX external commands */
		
		if(!strcmp(name, "CCFREEMEM"))
			continue;
		
		if(!strcmp(name, "EXIT"))
			continue;
		
		if(!strcmp(name, "MAIN"))
			continue;
#endif
		
		/* Print symbol and address */
		
		fprintf(stdout, "%-12s equ 0%sH\n", name, adr);
	}

	/* Check EOL */

	if(*p)
		error("Bad line format");
}

/* Print error and exit
*/

error(txt)
char *txt;
{
	fprintf(stderr, "ERROR: %s.\n", txt); exit(1);
}
