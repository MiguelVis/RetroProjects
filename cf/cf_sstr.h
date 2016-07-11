/*	cf_sstr.h

	Management library for configuration files under MESCC.

	cf_get_str() for CF.

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

	12 Jul 2016 : First version.
*/

/* Set the string value of a key
   -----------------------------
   Return 0 on success, or -1 on failure.
*/
cf_set_str(cf, key, value)
CF *cf; char *key, *value;
{
	char *tmp;
	int size, result;

	// Compute size
	size = strlen(value) + 2;

	// Alloc memory
	if((tmp = malloc(size))) {
		// Setup string
		*tmp = '\"';
		strcpy(tmp + 1, value);
		tmp[size - 2] = '\"';

		// Set the key
		result = cf_set_key(cf, key, tmp);

		// Free allocated memory
		free(tmp);

		// Return success or failure
		return result;
	}

	// Failure
	return -1;
}
