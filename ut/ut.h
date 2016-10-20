/*	ut.h

	Unitary tests library for MESCC.

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

	Revisions:

		18 Oct 2016 : Implemented.
		20 Oct 2016 : Documented and optimized.

	Dependencies:

		Needs MESCC v1.17 or higher.
*/

// Private globals
// ---------------

char *xut_file;  // __FILE__ value
int xut_line;    // __LINE__ value

int xut_passed;  // # of passed tests
int xut_failed;  // # of failed tests

// Public definitions
// ------------------

#define UtTrue        xUtSetEnv(__FILE__, __LINE__);  xUtTrue
#define UtFalse       xUtSetEnv(__FILE__, __LINE__);  xUtFalse

#define UtIntEq       xUtSetEnv(__FILE__, __LINE__);  xUtIntEq
#define UtIntNotEq    xUtSetEnv(__FILE__, __LINE__);  xUtIntNotEq

#define UtUintEq      xUtSetEnv(__FILE__, __LINE__);  xUtUintEq
#define UtUintNotEq   xUtSetEnv(__FILE__, __LINE__);  xUtUintNotEq

#define UtStrNull     xUtSetEnv(__FILE__, __LINE__);  xUtStrNull
#define UtStrNotNull  xUtSetEnv(__FILE__, __LINE__);  xUtStrNotNull
#define UtStrEq       xUtSetEnv(__FILE__, __LINE__);  xUtStrEq
#define UtStrNotEq    xUtSetEnv(__FILE__, __LINE__);  xUtStrNotEq

// Public functions
// ----------------

// Start tests

UtStart()
{
	xut_passed = xut_failed = 0;

	printf("Executing tests...\n\n");
}

// End tests

UtEnd()
{
	if(xut_failed)
	{
		putchar('\n');
	}

	printf("Tests Summary Report\n");
	printf("--------------------\n");
	printf("Tests:  %5d\n", xut_passed + xut_failed);
	printf("Passed: %5d\n", xut_passed);
	printf("Failed: %5d\n", xut_failed);
}

// Private functions
// -----------------

// Set environment for a test

xUtSetEnv(file, line)
char *file; int line;
{
	xut_file = file;
	xut_line = line;
}

// Test if condition is true -- use this function as base test

xUtTrue(condition, message)
int condition; char *message;
{
	// Check condition
	if(!condition)
	{
		// Test failed
		printf("Test failed! File: \'%s\' - Line: %d - %s.\n", xut_file, xut_line, message);

		++xut_failed;
	}
	else
	{
		// Test passed
		++xut_passed;
	}
}

// Test if condition is false

xUtFalse(condition, message)
int condition; char *message;
{
	xUtTrue(!condition, message);
}

// Test if int number is equal to another

xUtIntEq(expected, actual, message)
int expected, actual; char *message;
{
	xUtTrue(expected == actual, message);
}

// Test if int number is not equal to another

xUtIntNotEq(expected, actual, message)
int expected, actual; char *message;
{
	xUtTrue(expected != actual, message);
}

// Test if unsigned int number is equal to another

xUtUintEq(expected, actual, message)
unsigned int expected, actual; char *message;
{
	xUtTrue(expected == actual, message);
}

// Test if unsigned int number is not equal to another

xUtUintNotEq(expected, actual, message)
unsigned int expected, actual; char *message;
{
	xUtTrue(expected != actual, message);
}

// Test if pointer string is null

xUtStrNull(str, message)
char *str, *message;
{
	xUtTrue(str == NULL, message);
}

// Test if pointer string is not null

xUtStrNotNull(str, message)
char *str, *message;
{
	xUtTrue(str != NULL, message);
}

// Test if string is equal to another

xUtStrEq(expected, actual, message)
char *expected, *actual, *message;
{
	xUtTrue(!strcmp(expected, actual), message);
}

// Test if string is not equal to another

xUtStrNotEq(expected, actual, message)
char *expected, *actual, *message;
{
	xUtTrue(strcmp(expected, actual), message);
}

