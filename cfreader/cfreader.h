/*	cfreader.h

	Reader for configuration files under MESCC.

	Copyright (c) 2019 Miguel I. Garcia Lopez / FloppySoftware, Spain

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

	Revisions:

	02 Nov 2019 : v1.00.

	Notes:

	Legal characters for key names: A..Z a..z 0..9 . _ -
	Legal characters for comments:  # ;
	Blanks at the start of the line are ignored.
	Blanks around the equal symbol are ignored.
	Blanks around values are trimmed.
	Values are optional: key =
	Values can be quoted, but are returned without the quotes: "value" --> value.
	Recommended custom return codes for callback functions are 100 or higher.
*/

#ifndef CFREADER_H

#define CFREADER_H

/* Public defines
   --------------
*/
#define CFR_SUCCESS   0  /* Parsing success */
#define CFR_CANTOPEN  1  /* Can't open configuration file */
#define CFR_OVERFLOW  2  /* Line too long */
#define CFR_BADNAME   3  /* Illegal key name */
#define CFR_NOEQUAL   4  /* Equal symbol missing */
#define CFR_BADQUOTES 5  /* Missing quote in string */
#define CFR_ABORTED   6  /* Parsing aborted by callback function */
//#define CFR_NOVALUE   7

/* Private defines
   ---------------
*/
#define XCFR_BF_SIZE 130  // Size of buffer for file input: 128 + \n + 0

/* Private globals
   ---------------
*/
char xcfr_buf[XCFR_BF_SIZE];

// ----------------------
// -- PUBLIC FUNCTIONS --
// ----------------------

/* Read a configuration file
   -------------------------
   Return success or failure code, from this function or the callback function.
*/
CfReader(fname, callback, pline)
char *fname; unsigned int callback; int *pline;
{
	FILE *fp;
	int ret, k, line;
	char *bf, *key;

	// Default: success
	ret = CFR_SUCCESS;

	// Set line number (first == 1)
	line = 0;

	// Open file
	if((fp = fopen(fname, "r"))) {
		while(fgets(xcfr_buf, XCFR_BF_SIZE, fp)) {

			// Update line number
			++line;

			// Get the length of the string
			k = strlen(xcfr_buf);

			// Check for too long lines and
			// remove the trailing new line if any.
			if(xcfr_buf[k - 1] == '\n') {
				xcfr_buf[k - 1] = '\0';
			}
			else if(k == XCFR_BF_SIZE - 1) {
				// Line too long
				ret = CFR_OVERFLOW; break;
			}

			// Remove spaces on both sides
			bf = xCfrTrimRight(xCfrTrimLeft(xcfr_buf));

			// Skip empty lines and comments
			if(!(*bf) || *bf == '#' || *bf == ';') {
				continue;
			}

			// Set key pointer
			key = bf;

			// Skip the key name
			bf = xCfrTrimName(bf);

			// Check if it's a legal name
			if(key == bf) {
				ret = CFR_BADNAME; break;
			}

			// Get the character after the key name for later use
			k = *bf;

			// Set the end of the key name
			*bf++ = '\0';

			// Check for equal symbol
			if(k != '=') {
				// Skip spaces
				bf = xCfrTrimLeft(bf);

				// Check for equal symbol again
				if(*bf != '=') {
					ret = CFR_NOEQUAL; break;
				}

				// Skip equal symbol
				++bf;
			}

			// Skip spaces
			bf = xCfrTrimLeft(bf);

			// Remove quotes if any
			if(!(bf = xCfrTrimQuotes(bf))) {
				ret = CFR_BADQUOTES; break;
			}
/*
			// Check if there is a value
			if(!(*bf)) {
				ret = CFR_NOVALUE; break;
			}
*/
			// Call callback function with the key / value pair
			if((k = callback(line, key, bf))) {
				// Abort
				ret = k; break;
			}
		}

		// Close file
		fclose(fp);
	}
	else {
		ret = CFR_CANTOPEN;
	}

	// Get last line number
	if(pline) {
		*pline = line;
	}

	// Success or failure
	return ret;
}

// -----------------------
// -- PRIVATE FUNCTIONS --
// -----------------------

/* Skip key name
   -------------
   Return pointer to the first non legal character.
*/
xCfrTrimName(s)
char *s;
{
	while(isalnum(*s) || *s == '.' || *s == '_' || *s == '-') {
		++s;
	}

	// Return pointer
	return s;
}



/* Skip spaces on the left
   -----------------------
   Return pointer to the first non space character.
*/
xCfrTrimLeft(s)
char *s;
{
	// Skip spaces
	while(*s == ' ' || *s == '\t') {
		++s;
	}

	// Return pointer
	return s;
}

/* Skip spaces on the right
   ------------------------
   Return pointer to the string.
*/
xCfrTrimRight(s)
char *s;
{
	char *rtp;

	// Set pointer to the last character on the right
	rtp = s + strlen(s) - 1;

	// Search for the last non space character
	while(rtp >= s && (*rtp == ' ' || *rtp == '\t')) {
		rtp--;
	}

	// Set trailing zero
	*(++rtp) = '\0';

	// Return pointer to string
	return s;
}

/* Remove double quotes if any
   ---------------------------
   Return pointer to the string or NULL on error.
*/
xCfrTrimQuotes(s)
char *s;
{
	int len;

	// Check for "strings"
	if(*s == '\"') {
		len = strlen(s);

		if(len > 1 && s[len - 1] == '\"') {

			// Set trailing zero
			s[len - 1] = '\0';

			// Point to first non quote character
			++s;
		}
		else {
			s = NULL;
		}
	}

	// Return pointer to string
	return s;
}

#endif
