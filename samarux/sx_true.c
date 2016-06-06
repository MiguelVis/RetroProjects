/*	sx_true.c

	TRUE for SamaruX.

	Returns the value TRUE: 0.
	
	Copyright (c) 2016 Miguel Garcia / FloppySoftware.
	
	Usage:
	
	true

	Examples:

	true
	
	Changes:
	
	06 Jun 2016 : v1.00 : First version.
*/

#define SX_TRUE

TrueMain(argc, argv)
int argc, argv[];
{
	/* Check arguments */

	if(argc != 1) {
		return ErrorPars();
	}

	/* Return TRUE */

	return 0;
}
