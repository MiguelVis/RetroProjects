/*	cf_read.h

	Management library for configuration files under MESCC.

	cf_read() for CF.

	Copyright (c) 2016 Miguel I. Garcia Lopez / FloppySoftware, Spain

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

	09 Jul 2016 : First version.
*/

/* Dependencies
   ------------
*/
#include <fileio.h>

/* Private defines
   ---------------
*/
#define XCF_COMMENT '#'  // Start of comment
#define XCF_BF_SIZE 130  // Size of buffer for file input: 128 + \n + 0

/* Private globals
   ---------------
*/
char xcf_buf[XCF_BF_SIZE];

/* Read a configuration buffer from a file
   ---------------------------------------
   Return 0 on success, or -1 on failure.
*/
cf_read(cf, fname)
CF *cf; char *fname;
{
	FILE *fp;
	int err, k;
	char *bf, *key;

	// Default: no errors
	err = 0;

	// Open file
	if((fp = fopen(fname, "r"))) {
		while(fgets(xcf_buf, XCF_BF_SIZE, fp)) {

			// Get the length of the string
			// and skip empty lines
			if(!(k = strlen(xcf_buf)))
				continue;

			// Remove the trailing new line if any,
			// and check for too long lines.
			if(xcf_buf[k - 1] == '\n')
				xcf_buf[k - 1] = '\0';
			else if(k == XCF_BF_SIZE - 1) {
				// Line too long
				err = -1; break;
			}

			// Remove spaces on the left
			bf = xcf_lf_spaces(xcf_buf);

			// Skip comments
			if(*bf == XCF_COMMENT)
				continue;

			// Remove spaces on the right
			bf = xcf_rt_spaces(bf);

			// Skip empty lines
			if(!(*bf))
				continue;

			// Set the pointer to the key name
			key = bf;

			// Go upto the end of the name
			while(isalnum(*bf) || *bf == '_')
				++bf;

			// Check if it's a legal name
			// and the next character is valid.
			if(key == bf || (*bf != ' ' && *bf != '\t' && *bf != '=')) {
				err = -1; break;
			}

			// Get the next character
			k = *bf;

			// Set the end of the name, and skip the next character
			*bf++ = '\0';

			// Check for =
			if(k != '=') {
				// Skip spaces
				bf = xcf_lf_spaces(bf);

				// Check for =
				if(*bf != '=') {
					err = -1; break;
				}

				// Skip =
				++bf;
			}

			// Skip spaces
			bf = xcf_lf_spaces(bf);

			// Check if there is a value
			if(!(*bf)) {
				err = -1; break;
			}

			// Add the key / value pair to the configuration buffer
			if(cf_set_key(cf, key, bf)) {
				err = -1; break;
			}
		}

		// Close file
		fclose(fp);

		// Success or failure
		return err;
	}

	// Failure
	return -1;
}
