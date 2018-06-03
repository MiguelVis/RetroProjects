# gdoc

Documentation generator from source files.

**Note: this document is a work in progress and far to be completed (that's not so good for a document generator program!) **

## Introduction

**gdoc** is a documentation generator from source files that runs under the good old CP/M operating system.

Currently it accepts C-like and assembler file sources, and it is able to output the documentation in
HTML, [Markdown](https://daringfireball.net/projects/markdown/syntax) and plain text files.

It syntax has some compatibility degree or similarity with [Doxygen](http://www.stack.nl/~dimitri/doxygen/)
and [Javadoc](http://www.oracle.com/technetwork/java/javase/documentation/index-137868.html).

## Usage

The command line syntax for **gdoc** is as follows:

```
gdoc [-options...] filename [> destination]
```

The options are:

* `-c` for C-like sources (default)
* `-a` for assembler sources
* `-t` for text output (default)
* `-h` for HTML output
* `-m` for Markdown output

For example, to read C-like sources from the file `myapp.c` and write the output documentation as HTML to the file `myapp.htm`, you should use the following command line:

```
gdoc -ch myapp.c > myapp.htm
```

## How to write the documentation

You have to write the documentation in your source code inside of a `docblock`.

A `docblock` is a comment with a special format, like this one in C language:

```
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
```

or this one in assembler:

```
;(
; @fn     say
; @brief  Prints a string on screen
;
; This function simply prints a string on screen,
; using the BDOS function #9. The string must to
; be terminated with a '$' character.
;
; @param  de - string
; @return all registers destroyed
;)
say:
    ld   c,9
    jp   5
```

As you can see, you can use `@tags` like `@return something` for special purposes.

## Implemented tags

The implemented tags are the following:

```
@file [MAIN | filename]
@fn prototype
@section title
@project name
@doclink address|title
@brief small explanation
@author name  [add more @author tags if needed]
@copyright value
@version value
@date value
@param details [add more @param tags if needed]
@return details
- list item
detailed explanation...
```

## Examples

With **gdoc** are included two example files with their respective outputs in HTML, Markdown and plain text:

* `test.c` as an example of C-like source
* `test.a` as an example of assembler source

## Copyright

Copyright (c) 2016-18 Miguel Garcia / FloppySoftware.

All rights reserved.

You can contact me at [http://www.floppysoftware.es](http://www.floppysoftware.es).

## License

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