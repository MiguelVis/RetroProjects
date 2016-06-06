/*	sx_false.c

	FALSE for SamaruX.

	Returns the value FALSE: 1.
	
	Copyright (c) 2016 Miguel Garcia / FloppySoftware.
	
	Usage:
	
	false

	Examples:

	false
	
	Changes:
	
	06 Jun 2016 : v1.00 : First version.
*/

#define SX_FALSE

FalseMain(argc, argv)
int argc, argv[];
{
	/* Check arguments */

	if(argc != 1) {
		return ErrorPars();
	}

	/* Return FALSE */

	return 1;
}
