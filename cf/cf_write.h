/*	cf_write.h

	Management library for configuration files under MESCC.

	cf_write() for CF.

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

/* Write a configuration buffer to a file
   --------------------------------------
   Return 0 on success, or -1 on failure.
*/
cf_write(cf, fname)
CF *cf; char *fname;
{
	FILE *fp;
	unsigned int *arrk, *arrv;
	int max, i;

#ifndef FPRINTF_H
	char *s;
#endif

	// Open file
	if((fp = fopen(fname, "w"))) {

		// Get fields
		arrk = cf[XCF_FKEYS];
		arrv = cf[XCF_FVALUES];
		max  = cf[XCF_FMAX];

		// Write key / value pairs
		for(i = 0; i < max; ++i) {
			if(arrk[i]) {
#ifndef FPRINTF_H
				s = arrk[i]; while(*s) fputc(*s++, fp);

				fputc(' ', fp); fputc('=', fp); fputc(' ', fp);

				s = arrv[i]; while(*s) fputc(*s++, fp);

				fputc('\n', fp);
#else
				fprintf(fp, "%s = %s\n", arrk[i], arrv[i]);
#endif
			}
		}

		// Success or failure
		return (fclose(fp) ? -1 : 0);
	}

	// Failure
	return -1;
}
