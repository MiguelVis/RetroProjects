/*	clear.c

	Unix-like clear for CP/M.

	Clears the console.

	Copyright (c) FloppySoftware / Miguel I. Garcia Lopez.

	29 May 2016 : 1.00 : 1st version.
	14 Apr 2018 : 1.01 : Added VT100. Added support for CP/M-86 & DR-C.
*/

/* TARGET: uncomment only one
   --------------------------
*/
#define CPM80
/* #define CPM86 */

/* TERMINAL TYPE: uncomment only one
   ---------------------------------
 */
/* #define TTY_VT52 */
#define TTY_VT100
/* #define TTY_UNKNOWN */

/* CP/M-80 & MESCC
   ---------------
*/
#ifdef CPM80

#define CC_NO_ARGS	 // Exclude support for ARGC and ARGV.
#define CC_NO_MUL	 // Exclude support for MULTIPLICATION.
#define CC_NO_DIV    // Exclude support for DIVISION and MODULUS.
#define CC_NO_SWITCH // Exclude support for SWITCH.

#include <mescc.h>
#include <conio.h>

#endif

/* CP/M-86 & DR-C
   --------------
*/
#ifdef CPM86

#include <stdio.h>

/* DR-C doesn't have this one */
putstr(s)
char *s;
{
	while(*s)
		putchar(*s++);
}

#endif

/* Project defs.
   -------------
*/
#define APP_NAME    "clear"
#define APP_VERSION "v1.01 / 14 Apr 2018"
#define APP_COPYRGT "(c) 2016-2018 FloppySoftware"
#define APP_USAGE   "clear"

/* The program
   -----------
*/
main()
{
	/* Clear the console and move cursor to home */

#ifdef TTY_VT52
	putchar(27); putchar('E'); putchar(27); putchar('H');
#endif

#ifdef TTY_VT100
	putchar(27); putstr("[1;1H"); putchar(27); putstr("[2J");
#endif

#ifdef TTY_UNKNOWN
	int i;

	for(i = 0; i < 24; ++i) {
		putchar('\n');
	}
#endif
}
