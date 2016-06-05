/*	sx_whoami.c

	WHOAMI for SamaruX.

	Prints the user name.
	
	Copyright (c) 2015-2016 Miguel Garcia / FloppySoftware.
	
	Usage:
	
	whoami

	Examples:

	whoami
	
	Changes:
	
	23 Aug 2015 : v1.00 : Initial version.
	03 Jun 2016 : v1.01 : Built-in & external.
*/
	
/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_WHOAMI

#else

#include "samarux.h"

#define WhoamiMain main

#endif

WhoamiMain(argc, argv)
int argc, argv[];
{
	char *username;

	/* Check arguments */

	if(argc != 1) {
		return ErrorPars();
	}

	/* Search environment variable */

	if((username = EnvGet("USER")) == NULL) {
		return Error("Can't find $USER");
	}

	/* Show user name */

	puts(username);

	/* Success */

	return 0;
}
