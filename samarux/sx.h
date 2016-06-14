/*	sx.h

	+------------------------------------+
	| SamaruX - Unix like shell for CP/M |
	+------------------------------------+

	Copyright (c) 2007-2016 Miguel I. Garcia Lopez.

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 2, or (at your option) any
	later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

	Contact:

	www.floppysoftware.es
	cpm-connections.blogspot.com
	floppysoftware@gmail.com

	Revisions:

	05 Jun 2016 : Extracted from sx.c.
*/

/* SAMARUX DEFINES
   ---------------
*/

#define SX_VERSION 2
#define SX_RELEASE 05
#define SX_APPDATE "06 Jun 2016"

#define SX_COPYRGT "(c) 2015-2016 Miguel Garcia / FloppySoftware"

#define SX_PROFILE "profile.sx"  /* Profile for interactive mode */
#define SX_PROFCPM "profcpm.sx"  /* Profile for CP/M mode */

#define SX_PIPEIN  "pipein.sx"   /* Pipe input  file name */
#define SX_PIPEOUT "pipeout.sx"  /* Pipe output file name */

#define SX_TTY_ROWS 25  /* Default console rows    */
#define SX_TTY_COLS 80  /* Default console columns */

#define SX_MAX_HIST 10	/* Max. # of lines in commands line history */

#define SX_MAX_ENV 16   /* Max. # of environment variables */

#define SX_MAX_ALIAS 16 /* Max. # of aliases */

#define SX_LINESIZE 128 /* Input line size in characters + zero terminal */

#define SX_MAX_FCX 128  /* Max. # of FCXs for file search */

#define SX_MAX_ARGS 24  /* Max. # of arguments in the command line */

#define SX_MAX_CMD 40   /* Max. # of built-in commands */

#define SX_MAX_BATCH 10 /* Max. # of batch variables: $0 .. $9 */

#define SX_MAX_DIR 16   /* Max. # of directory names */
