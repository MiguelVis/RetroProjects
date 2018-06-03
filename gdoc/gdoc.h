/*	gdoc.h

	Documentation generator for MESCC source code files.

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

		14 Apr 2016 : v0.05
		16 Apr 2016 : v0.06
		02 Jun 2016 : v1.00
		15 Aug 2016 : v1.01
		11 Dec 2016 : v1.02
		19 Jan 2018 : v1.03
		11 May 2018 : v1.04
		16 May 2018 : v1.05
		22 May 2018 : v1.06
*/

/* Program data
   ------------
*/
#define APP_NAME    "gdoc"
#define APP_VERSION "v1.06 / 22 May 2018"
#define APP_COPYRGT "(c) 2016-18 FloppySoftware"
#define APP_USAGE   "gdoc [-options...] filename [> destination]"
#define APP_OPTIONS "\t-c for C source (default)\n\t-a for assembler source\n\t-t for text output (default)\n\t-h for html output\n\t-m for markdown output"

/* Documentation markers for C language
   ------------------------------------
*/
#define C_BEGIN "/**"  // Doc block begins with: /**
#define C_END   "*/"   // Doc block ends with: */
#define C_SKIP  '*'    // Lines inside doc block can start with: *

/* Documentation markers for assembler language
   --------------------------------------------
*/
#define A_BEGIN ";("   // Doc block begins with: ;(
#define A_END   ";)"   // Doc block ends with: ;)
#define A_SKIP  ';'    // Lines inside doc block can start with: ;


/* Buffer for input line from file
   -------------------------------
*/
#define LIN_LEN 127  // Max. lenght in chars.
#define LIN_SIZ 128  // Size in bytes

/* Prefix for tags
   ---------------
*/
#define TAG_PREFIX '@'

/* Tags
   ----
*/
#define TAG_FILE      "file"
#define TAG_FN        "fn"
#define TAG_BRIEF     "brief"
#define TAG_AUTHOR    "author"
#define TAG_DATE      "date"
#define TAG_VERSION   "version"
#define TAG_COPYRIGHT "copyright"
#define TAG_PARAM     "param"
#define TAG_RETURN    "return"
#define TAG_SECTION   "section"
#define TAG_DOCLINK   "doclink"
#define TAG_PROJECT   "project"

/* Special tag values
   ------------------
*/
#define TAG_FILE_MAIN "MAIN"

/* Max. # of entries in arrays
   ---------------------------
*/
#define TAG_MAX_DETAILS  64  // Detail lines
#define TAG_MAX_AUTHORS  8   // Authors
#define TAG_MAX_PARAMS   8   // Parameters
#define TAG_MAX_DOCLINKS 16  // Document links

#define REF_MAX_FUNCTS   32  // References to functions on index
#define REF_MAX_SECTIONS 16  // References to sections on index

/* Prefix for links
   ----------------
*/
#define LINK_FN_PREFIX   'f' // Function
#define LINK_SC_PREFIX   's' // Section

/* Prefix for lists
   ----------------
*/
#define LIST_PREFIX '-'

/* Error codes
   -----------
*/
#define ERR_BAD_OPT    10 // Bad option
#define ERR_BAD_CMD    11 // Bad command line

#define ERR_OPEN        20 // Can't open
#define ERR_CLOSE       21 // Can't close
#define ERR_READ        22 // Can't read
#define ERR_WRITE       23 // Can't write
#define ERR_FNAME       24 // Missing filename

#define ERR_LN_LONG     25 // Line too long

#define ERR_NO_MEM      28 // Not enough memory

#define ERR_IN_DOC      30 // Nexted doc block at line #
#define ERR_EOF_DOC     31 // Unclosed doc block

#define ERR_UNK_TAG     40 // Unknown tag at line #

#define ERR_TM_DETAILS  41 // Too many detail lines at line #
#define ERR_TM_AUTHORS  42 // Too many authors at line #
#define ERR_TM_PARAMS   43 // Too many parameters at line #
#define ERR_TM_DOCLINKS 44 // Too many document links at line #

#define ERR_TM_FUNCTS   50 // Too many functions at line #
#define ERR_TM_SECTIONS 51 // Too many sections at line #

#define ERR_NO_VAL      60 // Missing value at line #
