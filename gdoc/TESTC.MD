# testc.c
The purpose of this file is to test the gdoc program.

Author:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Miguel I. Garcia Lopez / FloppySoftware

Version:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1.00

Date:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;10 Apr 2016

Copyright:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;(c) 2016 FloppySoftware. Licensed under the GNU General Public License v3.

The gdoc program is a documentation generator for MESCC source code files (and
presumably other sources for c-like compilers).

It is written to be simple but effective, and it has some degree of
compatibility with other documentation generator tools as doxygen or javadoc.

What about item lists?

- Item number 1.
- Item number 2.
- Item number 3.

Functions:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[main(int argc, WORD argv[]) : int](#fn0)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- The main function.  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[hello() : void](#fn1)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Say hello (or something)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[program(char *name) : void](#fn2)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Print the program name.  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[params(int num, WORD what[]) : int](#fn3)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Print the program arguments.  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;[bye() : void](#fn4)  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- Say bye.

---
## <a id="fn0"></a>main(int argc, WORD argv[]) : int
The main function.

This function will be called on the first place. The second
argument would be 'char *argv[]' in standard C.

Parameters:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;argc - the number or arguments given in the command line  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;argv - the arguments (#0 is the program name)

Returns:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;the success / error / whatever code

---
## <a id="fn1"></a>hello() : void
Say hello (or something)

Say hello, or whatever the message is passed as an argument.

Parameters:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;msg - the message

---
## <a id="fn2"></a>program(char *name) : void
Print the program name.

Print the name of the program, given as a parameter.

Parameters:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;name - the program name

---
## <a id="fn3"></a>params(int num, WORD what[]) : int
Print the program arguments.

Print the arguments given in the command line, if any. The second
argument would be 'char *argv[]' in standard C.

Parameters:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;num - the number of arguments  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;what - the arguments

Returns:  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;the number of arguments

---
## <a id="fn4"></a>bye() : void
Say bye.

Say bye to the user. This function should be
the last called function in the program, but who knows.

