CF READER
=========

CF Reader is a simple reader library for configuration files.

For a more capable library, have a look at my CF project.

Configuration files
-------------------

A configuration file, is a text file with a known syntax, based on key / value pairs, as in `.INI` or `.conf` files.

Comments are optional.

Syntax rules
------------

Legal key names are composed by the characters:
```
A..Z a..z 0..9 . _ -
```

Key names and values are separated by the equal symbol:
```
name = John Doe
```

Values can be quoted, but are returned to the application as if they were'nt:
```
name = "John Doe"
```

Blanks around key names, values and the equal symbol are ignored.

Legal characters that start a comment are:
```
# ;
```

Blanks before comments are ignored.

Comments are ignored.

Example
-------

```
# Book data:
title = "That's cool!"
author = Jim Brown
year = 1977
pages = 150
summary = "This book, blah, blah, blah..."
```

How it works
------------

The entire configuration file is read by the `CfReader()` function:

```
CfReader(fname, callback, pline)
char *fname; unsigned int callback; int *pline;
```

Parameters:
  - `fname` = filename
  - `callback` = function to call on each key / value pair found
  - `pline` = optional pointer (can be NULL) to store the line number on errors (first line number is 1, the value 0 is for global errors)

Returned value:
  - success or failure code
  
Standard return value codes are:
```
#define CFR_SUCCESS   0  /* Parsing success */
#define CFR_CANTOPEN  1  /* Can't open configuration file */
#define CFR_OVERFLOW  2  /* Line too long */
#define CFR_BADNAME   3  /* Illegal key name */
#define CFR_NOEQUAL   4  /* Equal symbol missing */
#define CFR_BADQUOTES 5  /* Missing quote in string */
#define CFR_ABORTED   6  /* Parsing aborted by callback function */
```

The application can define its own failure codes, provided they are 100 or higher.

Each time the `CfReader()` function finds a key / value pair, it calls the callback funcion:

```
callback(line, key, value)
int line; char *key, *value;
{
	if(!strcmp(key, "cache")) {
		if(!strcmp(value, "true")) {
			conf_cache = 1;
		}
		else if(!strcmp(value, "false")) {
			conf_cache = 0;
		}
		else {
			fprintf(stderr, "Illegal value for 'cache' in line %d of configuration file.\n", line);
			
			return ERR_ILLEGAL_VALUE;
		}
	}
	else {
		fprintf(stderr, "Unknown key name in line %d of configuration file.\n", line);
			
		return ERR_UNKNOWN_KEY;
	}
	
	/* Success */
	return 0;
}
```

Parameters:
  - `line` = line number (>= 1)
  - `key` = key name
  - `value`= value
  
Returned value:
  - 0 on success, other values on failure (custom error codes from this function, if used, must be 100 or higher)

If the callback function returns a failure code, the `CfReader()` function stops the parsing, and returns this code to the application.

Copyright
---------

Copyright (c) 2019 Miguel Garcia / FloppySoftware.
All rights reserved.

- Website: [www.floppysoftware.es](http://www.floppysoftware.es)
- Blog: [cpm-connections.blogspot.com](http://cpm-connections.blogspot.com)
- Email: [floppysoftware@gmail.com](mailto:floppysoftware@gmail.com)

License
-------

This software is licensed under the GNU GENERAL PUBLIC LICENSE Version 3.
