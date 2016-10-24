/*	cf.c

	Some tests for CF.

	Copyright (c) 2016 Miguel Garcia / FloppySoftware

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 3, or (at your option) any
	later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

	Author's contact:

		www.floppysoftware.es
		cpm-connections.blogspot.com
		floppysoftware@gmail.com

	To compile with MESCC:

		cc cf
		ccopt cf
		zsm cf
		hextocom cf

	Usage:

		cf

	Revisions:

		06 Jul 2016 : Work begins.
		21 Oct 2016 : Refactorized CF function names.
		24 Oct 2016 : Added reading and writing of empty lines and comments.
*/

/* Defines for MESCC libraries
   ---------------------------
   Leave only what you need.
*/
//#define CC_STDIO        // Support for stdin, stdout, stderr.
//#define CC_REDIR        // Support for command line redirection - needs CC_STDIO.

//#define CC_CONIO_BIOS   // Console I/O through BIOS instead of BDOS.

//#define CC_FCX          // Support for user number in filenames.
//#define CC_FCX_DIR      // Support for named user numbers - needs CC_FCX and DirToDrvUsr().

//#define CC_FILEIO_SMALL // Exclude fread(), fwrite() and fgets().

//#define CC_NO_MUL       // Exclude support for MULTIPLICATION.
//#define CC_NO_DIV       // Exclude support for DIVISION and MODULUS.
//#define CC_NO_SWITCH    // Exclude support for SWITCH.

#define CC_NO_ARGS      // Exclude support for ARGC and ARGV.

/* Standard MESCC library
   ----------------------
   You must include this library;
   it contains some defs. and the runtime.
*/
#include <mescc.h>

/* Standard MESCC libraries
   ------------------------
   Leave only what you need.
*/
//#include <alloc.h>
//#include <atexit.h>
//#include <bsearch.h>
//#include <clock.h>
//#include <conio.h>
//#include <cpm.h>
//#include <ctype.h>
//#include <fileio.h>
//#include <fprintf.h>
//#include <mem.h>
#include <printf.h>
//#include <qsort.h>
//#include <rand.h>
//#include <redir.h>
//#include <setjmp.h>
//#include <sprintf.h>
//#include <string.h>
//#include <xprintf.h>
//#include <z80.h>

/* Project libraries
   -----------------
*/
#define CF_READ
#define CF_WRITE
#define CF_GET_ALL
#define CF_GET_BOOL
#define CF_SET_BOOL
#define CF_GET_INT
#define CF_GET_UINT
#define CF_GET_STR
#define CF_SET_STR

#include "cf.h"

/* Globals
   -------
*/
CF *cf;

/* Program entry
   -------------
*/
main()
{
	int k;

	printf("Creating CF\n\n");

	if(!(cf = CfCreate(9)))
		error("Can't create CF");

	set_key("#", "Book data:");
	set_key("title", "That's cool!");
	set_key("author", "Jim Brown");
	set_key("year", "1969");
	set_key("pages", "150");
	set_str("summary", "This book, blah, blah, blah...");
	set_key("", "");
	set_key("#", "Is it lent?");
	set_bool("lent", 1);

	// This should cause an error
	set_key("publisher", "This should cause an error: no more entries");

	printf("\n");
	pr_keys(cf);

	set_key("year", "1977");

	printf("\n");
	pr_keys(cf);

	printf("Writting test.cf\n\n");

	if(CfWrite(cf, "test.cf"))
		error("Can't write test.cf");

	printf("Destroying CF\n\n");

	CfDestroy(cf);

	// -------------------------------------------

	printf("Creating CF\n\n");

	if(!(cf = CfCreate(11)))
		error("Can't create CF");

	printf("Reading test.cf into CF\n\n");

	if(CfRead(cf, "test.cf", 1))
		error("Can't read test.cf");

	pr_keys(cf);

	printf("Reading test_2.cf into CF\n\n");

	if((k = CfRead(cf, "test_2.cf", 0)))
		error("Can't read test_2.cf");

	pr_keys(cf);

	CfGetAll(cf, pr_one_key);
	printf("\n");

	printf("Title     >> %s\n", CfGetKey(cf, "title"));
	printf("Author    >> %s\n", CfGetStr(cf, "author", "unknown"));
	printf("Publisher >> %s\n", CfGetStr(cf, "publisher", "n/a"));
	printf("Year      >> %d\n", CfGetUint(cf, "year", 9999));
	printf("Pages     >> %d\n", CfGetInt(cf, "pages", 9999));
	printf("Summary   >> %s\n", CfGetStr(cf, "summary", "n/a"));
	printf("Lent      >> %s\n", CfGetBool(cf, "lent", 0) ? "Yes" : "No");
	printf("To        >> %s\n", CfGetKey(cf, "lent_to"));
	printf("Expires   >> %s\n", CfGetKey(cf, "lend_expires"));

	printf("\n");

	printf("Destroying CF\n\n");

	CfDestroy(cf);

	// -------------------------------------------

	printf("Done\n");
}

set_key(key, value)
char *key, *value;
{
	int result;

	result = CfSetKey(cf, key, value);

	printf("Set %s = %s%s\n", key, value, result ? " --> ERROR" : "");
}

set_bool(key, value)
char *key; int value;
{
	int result;

	result = CfSetBool(cf, key, value);

	printf("Set %s = %s%s\n", key, value ? "true" : "false", result ? " --> ERROR" : "");
}

set_str(key, value)
char *key, *value;
{
	int result;

	result = CfSetStr(cf, key, value);

	printf("Set %s = \"%s\"%s\n", key, value, result ? " --> ERROR" : "");
}

pr_keys(cf)
CF *cf;
{
	CfPrKeys(cf);

	printf("\n");
}

pr_one_key(key, value)
char *key, *value;
{
	if(*key) {
		printf((*key == '#' || *key == ';') ? "%s %s\n" : "%s = %s\n", key, value);
	}
	else {
		printf("\n");
	}

	return 0;
}

error(msg)
char *msg;
{
	printf("ERROR: %s\n", msg);

	exit(-1);
}

