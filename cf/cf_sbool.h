/*	cf_sbool.h

	Management library for configuration files under MESCC.

	cf_set_bool() for CF.

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

/* Set the true / false value of a key
   -----------------------------------
   Return 0 on success, or -1 on failure.
*/
cf_set_bool(cf, key, value)
CF *cf; char *key; int value;
{
	// Set the value
	// and return the result.
	return cf_set_key(cf, key, value ? "true" : "false");
}
