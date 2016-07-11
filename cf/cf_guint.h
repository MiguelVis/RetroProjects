/*	cf_uint.h

	Management library for configuration files under MESCC.

	cf_get_guint() for CF.

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

/* Dependencies
   ------------
*/
#include <ctype.h>

/* Get the unsigned int value of a key
   -----------------------------------
   Return an unsigned int, or the default value on failure.
*/
cf_get_uint(cf, key, def)
CF *cf; char *key; unsigned int def;
{
	char *value;
	unsigned int val;

	// Setup value
	val = 0;

	// Get value
	if((value = cf_get_key(cf, key))) {

		// Compute the value
		while(isdigit(*value))
			val = val * 10 + (*value++ - '0');

		// Check end of value
		if(!(*value))
			return val;

		// Failure
	}

	// Failure
	return def;
}
