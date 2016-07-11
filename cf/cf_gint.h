/*	cf_gint.h

	Management library for configuration files under MESCC.

	cf_get_int() for CF.

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

/* Get the int value of a key
   --------------------------
   Return an int, or the default value on failure.
*/
cf_get_int(cf, key, def)
CF *cf; char *key; int def;
{
	char *value;

	// Get value
	if((value = cf_get_key(cf, key))) {

		// Return the int value
		return atoi(value);
	}

	// Failure
	return def;
}
