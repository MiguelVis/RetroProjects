/*	clear.c

	Unix-like clear for CP/M.

	Clears the console.

	Copyright (c) Miguel I. Garcia Lopez.

	29 May 2014 : 1.00 : 1st version.
*/

/* Defines for MESCC libraries
   ---------------------------
*/
#define CC_NO_ARGS	// Exclude support for ARGC and ARGV.
#define CC_NO_MUL	// Exclude support for MULTIPLICATION.
#define CC_NO_DIV       // Exclude support for DIVISION and MODULUS.
#define CC_NO_SWITCH    // Exclude support for SWITCH.

/* Standard MESCC library
   ----------------------
*/
#include <mescc.h>

/* Standard MESCC libraries
   ------------------------
*/
#include <conio.h>

/* Project defs.
   -------------
*/
#define APP_NAME    "clear"
#define APP_VERSION "v1.00 / 29 May 2016"
#define APP_COPYRGT "(c) 2016 FloppySoftware"
#define APP_USAGE   "clear"

//#define TTY_VT52
#define TTY_UNKNOWN

/* The program
   -----------
*/
main()
{
	// Clear the console

#ifdef TTY_UNKNOWN
	int i;

	for(i = 0; i < 24; ++i) {
		putchar('\n');
	}
#endif

#ifdef TTY_VT52
	putch(27); putch('E'); putch(27); putch('H');
#endif
}
