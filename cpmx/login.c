/*	login.c

	Unix-like login for CP/M.

	Not too much useful work here yet.

	Copyright (c) Miguel I. Garcia Lopez.

	29 May 2016 : 1.00 : 1st version.
*/

/* Defines for MESCC libraries
   ---------------------------
*/
#define CC_NO_ARGS	// Exclude support for ARGC and ARGV.
#define CC_NO_DIV       // Exclude support for DIVISION and MODULUS.

/* Standard MESCC library
   ----------------------
*/
#include <mescc.h>

/* Standard MESCC libraries
   ------------------------
*/
#include <conio.h>
#include <string.h>

/* Project defs.
   -------------
*/
#define APP_NAME    "login"
#define APP_VERSION "v1.00 / 29 May 2016"
#define APP_COPYRGT "(c) 2016 FloppySoftware"
#define APP_USAGE   "login"

#define USERNAME "elvis"
#define PASSWORD "presley"

#define NAME_MAX 8
#define NAME_SIZ 9

#define PASS_MAX 8
#define PASS_SIZ 9

/* The program
   -----------
*/
main()
{
	char usrname[NAME_SIZ],
	     usrpass[NAME_SIZ];
	int i;

	while(1)
	{
		// Ask for user name
		putstr("Login: ");
		read_line(usrname, NAME_MAX, 0);

		// Ask for password
		putstr("Password: ");
		read_line(usrpass, PASS_MAX, 1);

		// Check if username and password are valid
		if(!strcmp(usrname, USERNAME) && !strcmp(usrpass, PASSWORD)) {

			// Username and password are valid
			return 0;
		}

		// Error, try again
		putstr("\nBad user name or password.\n\n");
	}
}

/* Read line from console
   ----------------------
*/
read_line(buf, maxlen, pass)
char *buf;
int maxlen, pass;
{
	int len;
	char ch;

	// Buffer is empty
	len = 0;

	// Read some characters from console
	while(1)
	{
		switch(ch = getch())
		{
			case 8   :
			case 127 :
				// Delete last character
				if(len)
				{
					putchar(8); putchar(' '); putchar(8);
					--len;
				}
				break;
			case '\r' :
			case '\n' :
				// Done
				buf[len] = 0;
				putchar('\n');
				return;
			default :
				// Add character
				if(ch >= ' ' && len < maxlen) {
					buf[len++] = ch;

					putchar(pass ? '*' : ch);
				}
				break;
		}
	}
}
