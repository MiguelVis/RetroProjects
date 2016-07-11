/*	cf.h

	Management library for configuration files under MESCC.

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

	06 Jul 2016 : Work begins.
	07 Jul 2016 : Minor changes and optimizations.
	08 Jul 2016 : Set max. # of key / value pairs on creation time.
*/

#ifndef CF_H

#define CF_H

/* Dependencies
   ------------
*/
#include <alloc.h>
#include <string.h>

/* Public defines
   --------------
*/
#define CF unsigned int

/* Private defines
   ---------------
*/
#define XCF_FIELDS    3   // Number of fields

#define XCF_FKEYS     0   // Address of the array of keys
#define XCF_FVALUES   1   // Address of the array of values
#define XCF_FMAX      2   // Max. # of key / value pairs

#define XCF_DEBUG     1   // Enable or disable debug

// ----------------------
// -- PUBLIC FUNCTIONS --
// ----------------------

/* Create a configuration buffer
   -----------------------------
   Return address, or NULL on failure.
*/
cf_create(max)
int max;
{
	CF *cf;

	// Alloc memory
	if((cf = malloc(SIZEOF_PTR * XCF_FIELDS))) {
		if((cf[XCF_FKEYS] = xcf_alloc(max))) {
			if((cf[XCF_FVALUES] = xcf_alloc(max))) {

				// Store the rest of fields
				cf[XCF_FMAX] = max;
			
				// Success
				return cf;
			}

			// Failure
			xcf_free(cf[XCF_FKEYS], max);
		}

		// Failure
		free(cf);
	}

	// Failure
	return NULL;
}

/* Destroy a configuration buffer
   ------------------------------
   Return NULL.
*/
cf_destroy(cf)
CF *cf;
{
	// Free the arrays and its entries
	xcf_free(cf[XCF_FKEYS], cf[XCF_FMAX]);
	xcf_free(cf[XCF_FVALUES], cf[XCF_FMAX]);

	// Free the buffer
	free(cf);

	// Done
	return NULL;
}

/* Add a unique key to a configuration buffer
   ------------------------------------------
   Return 0 on success, or -1 on failure.
*/
/*********************************************************************************
cf_add_key(cf, key, value)
CF *cf; char *key, *value;
{
	int entry;

	// Add the key and value, only if key not exists.
	if((entry = xcf_find(cf[XCF_FKEYS], cf[XCF_FMAX], key)) == -1) {

		// Add the key
		if((entry = xcf_add(cf[XCF_FKEYS], cf[XCF_FMAX], key)) != -1) {

			// Add the value
			if(xcf_set(cf[XCF_FVALUES], value, entry)) {

				// Success
				return 0;
			}

			// Failure
			xcf_del(cf[XCF_FKEYS], entry);
		}
	}

	// Failure
	return -1;
}
*********************************************************************************/

/* Change the value of an existent key
   -----------------------------------
   Return 0 on success, or -1 on failure.
*/
/********************************************************************************
cf_chg_key(cf, key, value)
CF *cf; char *key, *value;
{
	int entry;

	// Get entry number
	if((entry = xcf_find(cf[XCF_FKEYS], cf[XCF_FMAX], key)) != -1) {

		// Change the value
		if(xcf_set(cf[XCF_FVALUES], value, entry)) {

			// Success
			return 0;
		}
	}

	// Failure
	return -1;
}
*********************************************************************************/

/* Set the value of a key
   ----------------------
   Return 0 on success, or -1 on failure.
*/
cf_set_key(cf, key, value)
CF *cf; char *key, *value;
{
	int entry;

	// Get entry number
	if((entry = xcf_find(cf[XCF_FKEYS], cf[XCF_FMAX], key)) != -1) {

		// The key exists: change its value
		if(xcf_set(cf[XCF_FVALUES], value, entry)) {

			// Success
			return 0;
		}
	}
	else {
		// The key doesn't exists: add it
		if((entry = xcf_add(cf[XCF_FKEYS], cf[XCF_FMAX], key)) != -1) {

			// Add the value
			if(xcf_set(cf[XCF_FVALUES], value, entry)) {

				// Success
				return 0;
			}

			// Failure
			xcf_del(cf[XCF_FKEYS], entry);
		}

		// Failure
	}

	// Failure
	return -1;
}

/* Get the raw value of a key
   --------------------------
   Return pointer to the value, or NULL on failure.
*/
cf_get_key(cf, key)
CF *cf; char *key;
{
	int entry;
	unsigned int *arr;

	// Get entry number
	if((entry = xcf_find(cf[XCF_FKEYS], cf[XCF_FMAX], key)) != -1) {

		// Get array address
		arr = cf[XCF_FVALUES];

		// Success
		return arr[entry];
	}

	// Failure
	return NULL;
}

// ---------------------
// -- DEBUG FUNCTIONS --
// ---------------------

#if XCF_DEBUG

/* Print keys and values
   ---------------------
*/
cf_pr_keys(cf)
CF *cf;
{
	unsigned int *arrk, *arrv;
	int i;

	arrk = cf[XCF_FKEYS];
	arrv = cf[XCF_FVALUES];

	for(i = 0; i < cf[XCF_FMAX]; ++i) {
		if(arrk[i])
			printf("%d : %s = %s\n", i, arrk[i], arrv[i]);
	}
}

#endif

// -----------------------
// -- PRIVATE FUNCTIONS --
// -----------------------

/* Allocate and clear an array
   ---------------------------
   Return array address, or NULL on failure.
*/
xcf_alloc(max)
int max;
{
	unsigned int *arr;
	int i;

	// Alloc memory for the array
	if((arr = malloc(SIZEOF_PTR * max))) {

		// Clear the array
		for(i = 0; i < max; ++i)
			arr[i] = NULL;

		// Success
		return arr;
	}

	// Failure
	return NULL;
}

/* Free an array and its entries
   -----------------------------
*/
xcf_free(arr, size)
unsigned int *arr; int size;
{
	int i;

	// Free allocated entries
	for(i = 0; i < size; ++i) {
		if(arr[i])
			free(arr[i]);
	}

	// Free the array
	free(arr);
}

/* Find a key
   ----------
   Return entry number, or -1 on failure.
*/
xcf_find(arr, size, key)
unsigned int *arr; int size; char *key;
{
	int i;

	// Search for data in the array
	for(i = 0; i < size; ++i) {
		if(arr[i]) {
			if(!strcmp(arr[i], key)) {

				// Found
				return i;
			}
		}
	}

	// Not found
	return -1;
}

/* Add a value to an array
   -----------------------
   Return entry number, or -1 on failure.
*/
xcf_add(arr, size, data)
unsigned int *arr; int size; char *data;
{
	int i;

	// Search for unallocated entry in the array
	for(i = 0; i < size; ++i) {
		if(!arr[i]) {

			// Set the value
			if(xcf_set(arr, data, i)) {

				// Success
				return i;
			}
		}
	}

	// Failure
	return -1;
}

/* Set a value in an array
   -----------------------
   Return pointer to the entry data, or NULL on failure.
*/
xcf_set(arr, data, entry)
unsigned int *arr; char *data; int entry;
{
	char *str;

	// Allocate memory for data
	if((str = malloc(strlen(data) + 1))) {

		// Free old data if needed
		if(arr[entry])
			free(arr[entry]);

		// Copy the data, update the pointer,
		// and return pointer to the entry data.
		return (arr[entry] = strcpy(str, data));
	}

	// Failure
	return NULL;
}

/* Delete a value in an array
   --------------------------
   Return NULL.
*/
xcf_del(arr, entry)
unsigned int *arr; int entry;
{
	// Free old data if needed
	if(arr[entry])
		free(arr[entry]);

	// Update pointer and return NULL
	return (arr[entry] = NULL);
}

/* Skip spaces on the left of a string
   -----------------------------------
   Return pointer to the first non space character.
*/
xcf_lf_spaces(s)
char *s;
{
	// Skip spaces
	while(*s == ' ' || *s == '\t')
		++s;

	// Return pointer
	return s;
}

xcf_rt_spaces(s)
char *s;
{
	char *rtp;

	// Set pointer to the last character on the right
	rtp = s + strlen(s) - 1;

	// Search for the last non space character
	while(rtp >= s && (*rtp == ' ' || *rtp == '\t'))
		rtp--;

	// Set trailing zero
	*(++rtp) = '\0';

	// Return pointer to string
	return s;
}

#endif
