/*	cf_gstr.h

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

	10 Jul 2016 : First version.
*/

/* Get the string value of a key
   -----------------------------
   Return a string, or the default value on failure.
*/
cf_get_str(cf, key, def)
CF *cf; char *key, *def;
{
	char *value;
	int k;

	// Get value
	if((value = cf_get_key(cf, key))) {

		// Check for quoted strings
		if(*value == '\"') {
			// Get the string length
			k = strlen(value);

			// Check for trailing quote
			if(k >= 2 && value[k - 1] == '\"') {
				// Remove trailing quote
				value[k - 1] = '\0';

				// Skip quote on the left
				++value;
			}
			else {
				// Failure
				return def;
			}
		}

		// Return the value
		return value;
	}

	// Failure
	return def;
}
