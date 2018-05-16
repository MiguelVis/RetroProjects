/*	gdoc.c

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

	To compile with MESCC:

		cc gdoc
		ccopt gdoc
		zsm gdoc
		hextocom gdoc

	Usage:

		gdoc [-options...] filename [> destination]

		-c for C sources (default)
		-a for assembler sources
		-t for text output (default)
		-h for html output
		-m for markdown output

	Revisions:

		08 Apr 2016 : 0.00 : Work begins.
		09 Apr 2016 : 0.01 : Added some @tags.
		10 Apr 2016 : 0.02 : Now there are two steps to print functions index in @file.
		11 Apr 2016 : 0.03 : Added @brief for functions to the index.
		12 Apr 2016 : 0.04 : Added HTML output.
		14 Apr 2016 : 0.05 : Modified command line. Added support for assembler sources.
		16 Apr 2016 : 0.06 : Improved error_line().
		02 Jun 2016 : 1.00 : First public release.
		15 Aug 2016 : 1.01 : Solved bug: a couple of <\p> instead of </p>. Added support for simple lists.
		                     Non proportional font for HTML output.
		11 Dec 2016 : 1.02 : Incremented max. # of detail lines to 64. Adapted to changes in fileio.h.
		19 Jan 2018 : 1.03 : Solved bug in line break after list. Join lines in list items. Added @copyright.
		11 May 2018 : 1.04 : Markdown output.
		16 May 2018 : 1.05 : Added @section, @doclink.

	Implemented tags:

		@file [filename]
		@fn prototype
		@section title
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

	To do:
		Implement @see, @bug, @code, @endcode, @warning,
		@deprecated, @since, @todo, @note, @retval, @var.
*/

/* Defines for MESCC libraries
   ---------------------------
*/
#define CC_STDIO        // Support for stdin, stdout, stderr.
#define CC_REDIR        // Support for command line redirection - needs CC_STDIO.

/* Standard MESCC library
   ----------------------
*/
#include <mescc.h>

/* Standard MESCC libraries
   ------------------------
*/
#include <alloc.h>
#include <fileio.h>
#include <printf.h>
#include <fprintf.h>
#include <sprintf.h>
#include <redir.h>
#include <string.h>

/* Project libraries
   -----------------
*/
#include "gdoc.h"

/* Globals
   -------
*/
char *g_doc_begin = C_BEGIN;  // Begin of doc block
char *g_doc_end   = C_END;    // End of doc block
char g_doc_skip   = C_SKIP;   // Character to skip in doc block

char g_buf[LIN_SIZ];  // Buffer for input line from file
char *g_fname;        // File name
int g_html;           // True if output is in HTML format
int g_md;             // True if output is in MARKDOWN format
FILE *g_fp;           // File handler
int g_line;           // Line # in process
int g_step;           // Step
int g_fns;            // Processed functions (for step #1)
int g_scs;            // Processed sections (for step #1)

/* Values in the whole file - step #0
   ----------------------------------
*/
char *ref_file;                      // @file
WORD ref_fns[REF_MAX_FUNCTS];        // @fn
WORD ref_fnlinks[REF_MAX_FUNCTS];    // Internal links for functions
WORD ref_fnbriefs[REF_MAX_FUNCTS];   // @brief for functions
int ref_num_fns;                     // # of functions
WORD ref_scs[REF_MAX_SECTIONS];      // @section
WORD ref_scslinks[REF_MAX_SECTIONS]; // Internal links for sections
int ref_num_scs;                     // # of sections

/* Tag values in a comment block - step #1
   ---------------------------------------
*/
char *tag_file;      // @file
char *tag_fn;        // @fn
char *tag_brief;     // @brief
char *tag_date;      // @date
char *tag_version;   // @version
char *tag_copyright; // @copyright
char *tag_return;    // @return
char *tag_section;   // @section

WORD tag_details[TAG_MAX_DETAILS];   // Text lines for details
int tag_num_details;                 // # of...

WORD tag_authors[TAG_MAX_AUTHORS];   // @author
int tag_num_authors;                 // # of authors

WORD tag_params[TAG_MAX_PARAMS];     // @param
int tag_num_params;                  // # of parameters

WORD tag_docs[TAG_MAX_DOCLINKS];     // @doclink
WORD tag_doclinks[TAG_MAX_DOCLINKS]; // External links for doclinks
int tag_num_doclinks;                // # of doclinks

/* Program entry
   -------------
*/
main(argc, argv)
int argc;
WORD argv[]; // char *argv[] - unsupported by MESCC (yet?)
{
	int argx;
	char *pch;

	// Usage

	if(argc == 1) {
		fprintf(stderr, "%s %s\n\n", APP_NAME, APP_VERSION);
		fprintf(stderr, "%s\n\n", APP_COPYRGT);
		fprintf(stderr, "Usage: %s\n\n", APP_USAGE);
		fprintf(stderr, "%s\n", APP_OPTIONS);
		exit(0);
	}

	// Parse options

	for(argx = 1; *(pch = argv[argx]) == '-'; ++argx) {
		while(*(++pch)) {
			switch(*pch)
			{
				case 'A' : g_doc_begin = A_BEGIN;       // ASSEMBLER sources
				           g_doc_end   = A_END;
				           g_doc_skip  = A_SKIP;
				           break;
				case 'C' : g_doc_begin = C_BEGIN;       // C sources
				           g_doc_end   = C_END;
				           g_doc_skip  = C_SKIP;
				           break;
				case 'T' : g_html = g_md = 0; break;    // TEXT output
				case 'H' : g_html = 1; g_md = 0; break; // HTML output
				case 'M' : g_html = 0; g_md = 1; break; // MARKDOWN output
				default  : error(ERR_BAD_OPT); break;
			}
		}
	}

	// Parse filename

	if(argx == argc) {
		error(ERR_FNAME);
	}

	g_fname = argv[argx++];

	// Check end of command line

	if(argx != argc) {
		error(ERR_BAD_CMD);
	}

	// Process the file (step #0)

	g_step = 0;

	proc_file();

	// Process the file (step #1)

	++g_step;

	proc_file();

	// The end

	exit(0);
}

/* Process the file
   ----------------
*/
proc_file()
{
	char *buf;
	int doc;

	// Open the file

	if((g_fp = fopen(g_fname, "r")) == NULL) {
		error_fname(ERR_OPEN);
	}

	// Setup some variables

	g_line = 1;  // Line #
	doc = 0;     // Not in a doc block

	// Header in step #1

	if(g_step && g_html) {
		printf("<!DOCTYPE html>\n<html>\n <head>\n  <title>%s</title>\n <head>\n <body style='font-family:monospace'>\n",
			ref_file ? ref_file : g_fname);
	}

	// Read the whole file

	while(read_line()) {

		// Process the line

		buf = blanks(g_buf);

		// Check for doc block markers

		if(!strcmp(buf, g_doc_begin)) {

			// Found: /**

			if(doc) {
				error_line(ERR_IN_DOC);  // Already in a doc block
			}

			doc = 1;       // We are now in a doc block

			// Reset doc block values

			reset_tags();
		}
		else if(doc) {

			// We are in a doc block

			if(!strcmp(buf, g_doc_end)) {

				// Found: */

				doc = 0;   // We are not out a doc block

				// Process the doc block according to the step #

				if(g_step) {
					dump_st1();  // Step #1
				}
				else {
					dump_st0();  // Step #0
				}
			}
			else {

				// If the next character is *, skip it

				if(*buf == g_doc_skip) {
					buf = blanks(++buf);
				}

				// Process the line, according to the step #

				if(g_step) {
					proc_st1(buf);  // Step #1
				}
				else {
					proc_st0(buf);  // Step #0
				}
			}
		}

		// Increment line #

		++g_line;
	}

	// Check for unclosed doc block

	if(doc) {
		error(ERR_EOF_DOC);
	}

	// Footer in step #1

	if(g_step) {

		if(g_html) {
			printf("  <hr>\n  <p style='text-align:right;'>\n   ");
		}
		else if(g_md) {
			printf("---\n");
		}

		printf("Documentation generated by %s %s - %s\n", APP_NAME, APP_VERSION, APP_COPYRGT);

		if(g_html) {
			printf("  </p>\n </body>\n</html>");
		}
	}

	// Close the file

	fclose(g_fp);
}

/* Process a line (step #0)
   ------------------------
*/
proc_st0(buf)
char *buf;
{
	char *val;

	// Check for @

	if(*buf == TAG_PREFIX) {

		++buf;

		// Check for tags

		if((val = eat_tag(TAG_FN, buf))) {            // @fn
			tag_fn = need_value(val);
		}
		else if((val = eat_tag(TAG_BRIEF, buf))) {    // @brief
			tag_brief = need_value(val);
		}
		else if((val = eat_tag(TAG_FILE, buf))) {     // @file
			tag_file = get_value(val);
		}
		else if((val = eat_tag(TAG_SECTION, buf))) {  // @section
			tag_section = need_value(val);
		}
	}
}

/* Process a line (step #1)
   ------------------------
*/
proc_st1(buf)
char *buf;
{
	char *val;

	// Check for @

	if(*buf == TAG_PREFIX) {

		++buf;

		// Check for tags

		if((val = eat_tag(TAG_FILE, buf))) {            // @file
			tag_file = get_value(val);
		}
		else if((val = eat_tag(TAG_FN, buf))) {         // @fn
			tag_fn = need_value(val);
		}
		else if((val = eat_tag(TAG_BRIEF, buf))) {      // @brief
			tag_brief = need_value(val);
		}
		else if((val = eat_tag(TAG_AUTHOR, buf))) {     // @author
			add_author(need_value(val));
		}
		else if((val = eat_tag(TAG_DATE, buf))) {       // @date
			tag_date = need_value(val);
		}
		else if((val = eat_tag(TAG_VERSION, buf))) {    // @version
			tag_version = need_value(val);
		}
		else if((val = eat_tag(TAG_COPYRIGHT, buf))) {  // @copyright
			tag_copyright = need_value(val);
		}
		else if((val = eat_tag(TAG_RETURN, buf))) {     // @return
			tag_return = need_value(val);
		}
		else if((val = eat_tag(TAG_PARAM, buf))) {      // @param
			add_param(need_value(val));
		}
		else if((val = eat_tag(TAG_DOCLINK, buf))) {    // @doclink
			add_doclink(need_value(val));
		}
		else if((val = eat_tag(TAG_SECTION, buf))) {    // @section
			tag_section = need_value(val);
		}
		else {
			// Unknown tag

			error_line(ERR_UNK_TAG);
		}

		// Done

		return;
	}

	// It's not a tag; it should be a detail line

	add_detail(get_value(buf));
}

/* Add a detail line
   -----------------
*/
add_detail(val)
char *val;
{
	char *pch;

	// If it's an empty line, check some things

	if(!*val) {
		// First line can't be empty

		if(!tag_num_details) {
			return;
		}

		// Avoid two empty lines

		pch = tag_details[tag_num_details - 1];

		if(!*pch) {
			return;
		}
	}

	if(tag_num_details < TAG_MAX_DETAILS) {

		// Add it

		tag_details[tag_num_details++] = val;
	}
	else {
		// Not enough room

		error_line(ERR_TM_DETAILS);
	}
}

/* Add an author
   -------------
*/
add_author(val)
char *val;
{
	if(tag_num_authors < TAG_MAX_AUTHORS) {

		// Add it

		tag_authors[tag_num_authors++] = val;
	}
	else {
		// Not enough room

		error_line(ERR_TM_AUTHORS);
	}
}

/* Add a parameter
   ---------------
*/
add_param(val)
char *val;
{
	if(tag_num_params < TAG_MAX_PARAMS) {

		// Add it

		tag_params[tag_num_params++] = val;
	}
	else {
		// Not enough room

		error_line(ERR_TM_PARAMS);
	}
}

/* Add a doclink
   -------------
*/
add_doclink(val)
char *val;
{
	if(tag_num_doclinks < TAG_MAX_DOCLINKS) {

		// Add it

		tag_docs[tag_num_doclinks] = get_link_title(val);
		tag_doclinks[tag_num_doclinks++] = get_link_address(val);

	}
	else {
		// Not enough room

		error_line(ERR_TM_DOCLINKS);
	}
}

/* Reset doc block tags
   --------------------
*/
reset_tags()
{
	int i;

	// Free buffers if there were allocated

	free(tag_fn);
	free(tag_brief);
	free(tag_file);
	free(tag_section);

	// Invalidate pointers

	tag_fn = tag_brief = tag_file = tag_section = NULL;

	// Only for step #1

	if(g_step) {

		// Free buffers if there were allocated

		free(tag_date);
		free(tag_version);
		free(tag_copyright);
		free(tag_return);

		// Invalidate pointers

		tag_date = tag_version = tag_copyright = tag_return = NULL;

		// Reset arrays

		tag_num_details  = reset_array(tag_details, tag_num_details);
		tag_num_authors  = reset_array(tag_authors, tag_num_authors);
		tag_num_params   = reset_array(tag_params, tag_num_params);
		                   reset_array(tag_doclinks, tag_num_doclinks);
		tag_num_doclinks = reset_array(tag_docs, tag_num_doclinks);
	}
}

/* Reset an array
   --------------
   Returns 0.
*/
reset_array(array, items)
WORD array[]; int items;
{
	int i;

	// Reset each entry in the array

	for(i = 0; i < items; ++i) {
		free(array[i]);       // Free buffer
		array[i] = NULL;      // Invalidate the pointer
	}

	// Return zero

	return 0;
}

/* Dump doc block, step #0
   -----------------------
*/
dump_st0()
{
	if(tag_file) {

		// Collect @file for the title

		ref_file = *tag_file ? tag_file : g_fname;

		// Prevent free

		tag_file = NULL;
	}
	else if(tag_fn) {

		// Collect function references for the index

		if(ref_num_fns < REF_MAX_FUNCTS) {

			// Add @fn

			ref_fns[ref_num_fns] = tag_fn;

			// Add link

			ref_fnlinks[ref_num_fns] = make_link(LINK_FN_PREFIX, ref_num_fns);

			// Add @brief

			ref_fnbriefs[ref_num_fns++] = (tag_brief ? tag_brief : "");

			// Prevent free

			tag_fn = tag_brief = NULL;
		}
		else {
			// Not enough room

			error_line(ERR_TM_FUNCTS);
		}
	}
	else if(tag_section) {

		// Collect section references for the index

		if(ref_num_scs < REF_MAX_SECTIONS) {

			// Add @section

			ref_scs[ref_num_scs] = tag_section;

			// Add link

			ref_scslinks[ref_num_scs] = make_link(LINK_SC_PREFIX, ref_num_scs);

			++ref_num_scs;

			// Prevent free

			tag_section = NULL;
		}
	}
}

/* Dump doc block, step #1
   -----------------------
*/
dump_st1()
{
	int i;
	char *pch;
	int ul;

	// Skip last detail line if it's empty

	if(tag_num_details) {
		pch = tag_details[tag_num_details - 1];

		if(!*pch) {
			--tag_num_details;
		}
	}

	// Header

	if(tag_file) {
		header(*tag_file ? tag_file : g_fname, 0, NULL);  // File
	}
	else if(tag_fn) {
		pch = ref_fnlinks[g_fns++];

		header(tag_fn, 1, pch + 1);                       // Function
	}
	else if(tag_section) {
		pch = ref_scslinks[g_scs++];

		header(tag_section, 1, pch + 1);                  // Section
	}
	else {
		header("Unknown", 1, NULL);                       // Unknown -- FIXME
	}

	// Brief

	if(tag_brief) {
		if(g_html) {
			printf("  <p>%s</p>\n", tag_brief);
		}
		else {
			printf("%s\n\n", tag_brief);
		}
	}

	// Authors

	if(tag_num_authors) {
		list("Author", tag_authors, tag_num_authors);
	}

	// Version

	if(tag_version) {
		listOne("Version", tag_version);
	}

	// Date

	if(tag_date) {
		listOne("Date", tag_date);
	}

	// Copyright

	if(tag_copyright) {
		listOne("Copyright", tag_copyright);
	}

	// Details

	if(tag_num_details) {

		// No list

		ul = 0;

		// Open paragraph

		if(g_html) {
			printf("  <p>\n");
		}

		// Write lines

		for(i = 0; i < tag_num_details; ++i) {

			// Pointer to line

			pch = tag_details[i];

			// Write line

			if(*pch == LIST_PREFIX) {

				// List item

				if(ul) {

					// List already opened, close previous list item

					if(g_html) {
						printf("    </li>\n");
					}
				}
				else {

					// Open list

					ul = 1;

					if(g_html) {
						printf("   <ul>\n");
					}
					else if(g_md) {
						putchar('\n');
					}
				}

				// Open list item

				pch = blanks(++pch);

				if(g_html) {
					printf("    <li>\n     %s\n", pch);
				}
				else if(g_md) {
					printf("- %s\n", pch);
				}
				else {
					printf("   - %s\n", pch);
				}
			}
			else if(ul) {

				// List opened, close it if blank line, or join adjacent lines

				//pch = blanks(pch);

				if(*pch) {

					// Join adjacent lines

					if(g_html) {
						printf("     %s\n", pch);
					}
					else {
						printf("     %s\n", pch);
					}
				}
				else {

					// Close list

					ul = 0;

					if(g_html) {
						printf("    </li>\n   </ul>\n");
					}
					else {
						printf("\n");
					}
				}
			}
			else {

				// Text line

				if(g_html) {
					if(*pch) {
						printf("   %s\n", pch);
					}
					else {
						printf("   <br>\n   <br>\n");
					}
				}
				else {
					printf("%s\n", pch);
				}
			}
		}

		// Close list if opened

		if(ul) {
			if(g_html) {
				printf("    </li>\n   </ul>\n");
			}
		}

		// Close paragraph

		if(g_html) {
			printf("  </p>\n");
		} else {
			putchar('\n');
		}
	}

	// Only for @file

	if(tag_file) {

		// Section links

		if(ref_num_scs) {
			listDouble("Sections", ref_scs, NULL, ref_scslinks, ref_num_scs);
		}
	}

	// Document links

	if(tag_num_doclinks) {
		listDouble("Document links", tag_docs, NULL, tag_doclinks, tag_num_doclinks);
	}

	// Only for @file

	if(tag_file) {

		// Function links

		if(ref_num_fns) {
			listDouble("Functions", ref_fns, ref_fnbriefs, ref_fnlinks, ref_num_fns);
		}

		// Done

		return;
	}

	// Parameters

	if(tag_num_params) {
		list("Parameters", tag_params, tag_num_params);
	}

	// Return

	if(tag_return) {
		listOne("Returns", tag_return);
	}
}

/* Print a header
   --------------
*/
header(title, level, anchor)
int title, level; char *anchor;
{
	int len;

	// Header for HTML output

	if(g_html) {
		if(level) {
			printf("   <hr>\n");
		}

		if(anchor) {
			printf("  <h%c id=\"%s\">%s</h%c>\n", '1' + level, anchor, title, '1' + level);
		}
		else {
			printf("  <h%c>%s</h%c>\n", '1' + level, title, '1' + level);
		}

		return;
	}

	// Header for MARKDOWN output
	if(g_md) {
		if(level) {
			printf("---\n#");
		}

		printf("# ");

		if(anchor) {
			printf("<a id=\"%s\"></a>", anchor);
		}

		printf("%s\n", title);

		return;
	}

	// Compute the title length

	len = strlen(title);

	// Draw a ruler

	ruler(len, level ? '-' : '=');

	// Print the title

	printf("%s\n", title);

	// Draw a ruler

	ruler(len, level ? '-' : '=');

	// Separator

	putchar('\n');
}

/* Draw a ruler
   ------------
*/
ruler(width, ch)
int width, ch;
{
	while(width--) {
		putchar(ch);
	}

	putchar('\n');
}

/* Print a list of one member
   --------------------------
*/
listOne(title, value)
char *title, *value;
{
	WORD array[1];

	array[0] = value;

	list(title, array, 1);
}

/* Print a list
   ------------
*/
list(title, array, size)
char *title; WORD array[]; int size;
{
	listDouble(title, array, NULL, NULL, size);
}

/* Print a double list
   -------------------
*/
listDouble(title, array, arrayTwo, links, size)
char *title; WORD array[], arrayTwo[], links[]; int size;
{
	int i;
	char *ps;

	if(g_html) {
		printf("  <dl>\n   <dt>%s:</dt>\n", title);
	}
	else if(g_md) {
		printf("%s:  \n", title);
	}
	else {
		printf("%s:\n", title);
	}

	for(i = 0; i < size; ++i) {
		if(g_html) {
			if(links) {
				printf("   <dd><a href=\"%s\">%s</a></dd>\n", links[i], array[i]);
			}
			else {
				printf("   <dd>%s</dd>\n", array[i]);
			}

			if(arrayTwo) {
				printf("   <dd>&nbsp;- %s</dd>\n", arrayTwo[i]);
			}
		}
		else if(g_md) {
			printf("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");

			if(links) {
				printf("[%s](%s)", array[i], links[i]);
			}
			else {
				printf("%s", array[i]);
			}

			if(arrayTwo || i < size - 1) {
				printf("  ");
			}

			putchar('\n');

			if(arrayTwo) {
				printf("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;- %s", arrayTwo[i]);

				if(i < size - 1) {
					printf("  ");
				}

				putchar('\n');
			}
		}
		else {
			if(links) {
				if(*(ps = links[i]) == '#') {
					printf("   %s\n", array[i]);
				}
				else {
					printf("   %s | %s\n", links[i], array[i]);
				}
			}
			else {
				printf("   %s\n", array[i]);
			}

			if(arrayTwo) {
				printf("     -%s\n", arrayTwo[i]);
			}
		}
	}

	if(g_html) {
		printf("  </dl>\n");
	}
	else {
		putchar('\n');
	}
}

/* Skip blanks
   -----------
   Returns a pointer to the first non blank character.
*/
blanks(s)
char *s;
{
	// Skip spaces or tabs

	while(*s == ' ' || *s == '\t') {
		++s;
	}

	// Return pointer

	return s;
}

/* Skip blanks on the right
   ------------------------
   Returns a pointer to the first non blank character.
*/
rblanks(s)
char *s;
{
	char *p;

	// Go to the end of the string

	p = s + strlen(s);

	// Skip spaces or tabs

	while(p != s) {

		--p;

		if(*p != ' ' && *p != '\t') {
			break;
		}

		*p = '\0';
	}

	// Return pointer

	return s;
}

/* Compare and eat a tag
   ---------------------
   Returns a pointer to the value following the tag (or byte 0),
   or NULL if the tag does not match.
*/
eat_tag(tag, buf)
char *tag, *buf;
{
	// Compare and skip the tag

	while(*tag && *tag == *buf) {
		++tag;
		++buf;
	}

	// Check if matches

	if(!*tag) {

		// Needs blank, tab or ZERO

		if(*buf == ' ' || *buf == '\t' || !*buf) {

			// Skip blanks and return

			return blanks(buf);
		}
	}

	// The tag does not match

	return NULL;
}

/* Get value
   ---------
   Returns the value.
*/
get_value(buf)
char *buf;
{
	char *val;

	// Skip spaces on the right, and
	// allocate buffer.

	val = alloc_mem(strlen(rblanks(buf)) + 1);

	// Return value

	return strcpy(val, buf);
}

/* Need value
   ----------
   Same as get_value(), but throws error if it's missing.
*/
need_value(buf)
char *buf;
{
	// Get value

	buf = get_value(buf);

	// Needs a value

	if(!*buf) {

		// Missing value

		error_line(ERR_NO_VAL);
	}

	// Return value

	return buf;
}

/* Make internal link
   ------------------
   Return address.
*/
make_link(prefix, index)
char prefix; unsigned int index;
{
    char s[8];

	// #f12345 + '\0'

    sprintf(s, "#%c%u", prefix, index);

    return strcpy(alloc_mem(strlen(s) + 1), s);
}

/* Get link address from "address|title"
   -------------------------------------
   Return address with file extension if needed.
*/
get_link_address(s)
char *s;
{
    char *ps, *px; int len;

    ps = strchr(s, '|');

    if((px = strchr(s, '.')) && (!ps || ps - px > 0)) {
        px = "";
    }
    else if(g_html) {
        px = ".htm";
    }
    else if(g_md) {
        px = ".md";
    }
    else {
        px = ".txt";
    }

    len = (ps ? ps - s : strlen(s));

    ps = alloc_mem(len + strlen(px) + 1);

    memcpy(ps, s, len);

    ps[len] = '\0';

    return strcat(ps, px);
}

/* Get link title from "address|title"
   -----------------------------------
   Return title or address on failure.
*/
get_link_title(s)
char *s;
{
    char *ps;

    ps = strchr(s, '|');

    return ((!ps || !ps[1]) ? s : ps + 1);
}

/* Read a line from the file
   -------------------------
   Returns pointer to the buffer, or NULL on EOF.
*/
read_line()
{
	int i, ch;

	// Read chars until a new line or EOF is reached

	for(i = 0; (ch = fgetc(g_fp)) != EOF; ++i) {

		// Check for new line

		if(ch == '\n') {
			break;
		}

		// Check for overflow

		if(i == LIN_LEN) {
			error(ERR_LN_LONG);
		}

		// Get it

		g_buf[i] = ch;
	}

	// Check for EOF and empty line

	if(ch == EOF && !i) {
		return NULL;
	}

	// Set EOL

	g_buf[i] = '\0';

	// Return pointer to buffer

	return g_buf;
}

/* Alloc memory
   ------------
   Return address or throws error if not enough memory.
*/
alloc_mem(size)
unsigned int size;
{
    char *p;

    if((p = malloc(size))) {
        return p;
    }

	error(ERR_NO_MEM);
}

/* Print extended error message and exit
   -------------------------------------
*/
error_ext(err, ext)
int err; char *ext;
{
	char *s;

	// Set the error message

	switch(err) {
		case ERR_BAD_OPT    : s = "Bad option"; break;
		case ERR_BAD_CMD    : s = "Bad command line"; break;

		case ERR_OPEN       : s = "Can't open"; break;
		case ERR_CLOSE      : s = "Can't close"; break;
		case ERR_READ       : s = "Can't read"; break;
		case ERR_WRITE      : s = "Can't write"; break;
		case ERR_FNAME      : s = "Missing filename"; break;

		case ERR_LN_LONG    : s = "Line too long"; break;

		case ERR_NO_MEM     : s = "Not enough memory"; break;

		case ERR_IN_DOC     : s = "Nexted doc block"; break;
		case ERR_EOF_DOC    : s = "Unclosed doc block"; break;

		case ERR_UNK_TAG    : s = "Unknown tag"; break;

		case ERR_TM_DETAILS : s = "Too many detail lines"; break;
		case ERR_TM_AUTHORS : s = "Too many authors"; break;
		case ERR_TM_PARAMS  : s = "Too many parameters"; break;
		case ERR_TM_DOCLINKS: s = "Too many document links"; break;

		case ERR_TM_FUNCTS  : s = "Too many functions"; break;
		case ERR_TM_SECTIONS: s = "Too many sections"; break;

		case ERR_NO_VAL     : s = "Missing value"; break;

		default             : s = "Unknown error"; break;
	}

	// Print error message

	fprintf(stderr, "%s: %s", APP_NAME, s);

	if(ext) {

		// Extended

		fprintf(stderr, " %s", ext);
	}

	fprintf(stderr, ".\n");

	// Exit

	exit(-1);
}

/* Print error message with line # and exit
   ----------------------------------------
*/
error_line(err)
int err;
{
	char *str;

	// Setup string

	str = "at line 12345";

	// Line # to string

	sprintf(str + 8, "%d", g_line);

	// Print error

	error_ext(err, str);
}

/* Print error message with filename and exit
   ------------------------------------------
*/
error_fname(err)
int err;
{
	// Print error

	error_ext(err, g_fname);
}

/* Print simple error message and exit
   -----------------------------------
*/
error(err)
int err;
{
	// Print error

	error_ext(err, NULL);
}

