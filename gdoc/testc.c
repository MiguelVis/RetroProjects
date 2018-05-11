/**
 * @file
 *
 * @brief The purpose of this file is to test the gdoc program.
 *
 * The gdoc program is a documentation generator for MESCC source code files (and
 * presumably other sources for c-like compilers).
 *
 * It is written to be simple but effective, and it has some degree of
 * compatibility with other documentation generator tools as doxygen or javadoc.
 *
 * What about item lists?
 * - Item number 1.
 * - Item number 2.
 * - Item number 3.
 *
 * @author    Miguel I. Garcia Lopez / FloppySoftware
 * @version   1.00
 * @date      10 Apr 2016
 * @copyright (c) 2016 FloppySoftware. Licensed under the GNU General Public License v3.
 */
#include <mescc.h>
#include <printf.h>

/**
 * @fn     main(int argc, WORD argv[]) : int
 *
 * @brief  The main function.
 *
 * This function will be called on the first place. The second
 * argument would be 'char *argv[]' in standard C.
 *
 * @param argc - the number or arguments given in the command line
 * @param argv - the arguments (#0 is the program name)
 *
 * @return the success / error / whatever code
 */
int main(argc, argv)
int argc; WORD argv[]; // MESCC doesn't support char *argv[] -- pity
{
	/* Say hello */

	hello("Hello!");

	/* CP/M doesn't store the name of the program -- pity */

	argv[0] = "test.com";

	/* Print the name of the program */

	program(argv[0]);

	/* Print the arguments if any */

	params(--argc, &argv[1]);

	/* Say bye */

	bye();
}

/**
 * @fn    hello() : void
 *
 * @brief Say hello (or something)
 *
 * Say hello, or whatever the message is passed as an argument.
 *
 * @param msg - the message
 */
hello(msg)
char *msg;
{
	printf("%s\n\n", msg);
}

/**
 * @fn    program(char *name) : void
 *
 * @brief Print the program name.
 *
 * Print the name of the program, given as a parameter.
 *
 * @param name - the program name
 */
program(name)
char *name;
{
	printf("I am %s, a humble program.\n\n", name);
}

/**
 * @fn    params(int num, WORD what[]) : int
 *
 * @brief Print the program arguments.
 *
 * Print the arguments given in the command line, if any. The second
 * argument would be 'char *argv[]' in standard C.
 *
 * @param num - the number of arguments
 * @param what - the arguments
 *
 * @return the number of arguments
 */
params(num, what)
int num; WORD what[];
{
	int i;

	/* Print the arguments, if any */

	if(num) {

		/* Print the arguments */

		for(i = 0; i < num; ++i) {
			printf("Param #%d = %s\n", i, what[i]);
		}
	}
	else {
		/* There are no arguments */

		printf("There are no arguments on the command line.\n");
	}

	putchar('\n');

	/* Return the number of arguments */

	return num;
}

/**
 * @fn    bye() : void
 *
 * @brief Say bye.
 *
 * Say bye to the user. This function should be
 * the last called function in the program, but who knows.
 */
bye()
{
	printf("Bye, bye!\n");
}
