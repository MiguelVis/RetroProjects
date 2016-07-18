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
	15 Jul 2016 : Added supported #defines.
	              Added '.' and '-' as valid characters for key names.
	              Added ';' as valid character for comments.
	16 Jul 2016 : Added cf_get_all().

	Supported #defines:

	CF_READ     : cf_read().
	CF_WRITE    : cf_write().
	CF_GET_ALL  : cf_get_all().
	CF_GET_BOOL : cf_get_bool().
	CF_GET_INT  : cf_get_int().
	CF_GET_UINT : cf_get_uint().
	CF_GET_STR  : cf_get_str().
	CF_SET_BOOL : cf_set_bool().
	CF_SET_STR  : cf_set_str().

	Notes:

	Valid characters for key names: A..Z a..z 0..9 . _ -
	Valid characters for comments:  # ;
*/

#ifndef CF_H

#define CF_H

/* Dependencies
   ------------
*/
#include <alloc.h>
#include <string.h>

#ifdef CF_READ
#include <fileio.h>
#include <ctype.h>
#endif

#ifdef CF_WRITE
#include <fileio.h>
#endif

#ifdef CF_GET_UINT
#include <ctype.h>
#endif

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

#ifdef CF_READ
#define XCF_BF_SIZE 130  // Size of buffer for file input: 128 + \n + 0
#endif

/* Private globals
   ---------------
*/
#ifdef CF_READ
char xcf_buf[XCF_BF_SIZE];
#endif

// --------------------
// -- CORE FUNCTIONS --
// --------------------

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

// ------------------------
// -- FILE I/O FUNCTIONS --
// ------------------------

#ifdef CF_READ

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
			if(*bf == '#' || *bf == ';')
				continue;

			// Remove spaces on the right
			bf = xcf_rt_spaces(bf);

			// Skip empty lines
			if(!(*bf))
				continue;

			// Set the pointer to the key name
			key = bf;

			// Go upto the end of the name
			while(isalnum(*bf) || *bf == '.' || *bf == '_' || *bf == '-')
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

#endif

#ifdef CF_WRITE

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

#endif

// -------------------
// -- GET FUNCTIONS --
// -------------------

#ifdef CF_GET_ALL

/* Get all keys
   ------------
*/
cf_get_all(cf, funct)
CF *cf; unsigned int funct;
{
	unsigned int *arrk, *arrv;
	int max, i;

	// Get fields
	arrk = cf[XCF_FKEYS];
	arrv = cf[XCF_FVALUES];
	max  = cf[XCF_FMAX];

	// Iterate the array
	for(i = 0; i < max; ++i) {
		if(arrk[i]) {
			if(funct(arrk[i], arrv[i]))
				break;
		}
	}
}

#endif

#ifdef CF_GET_BOOL

/* Get the true / false value of a key
   -----------------------------------
   Return 1 for true, 0 for false, or the default value on failure.
*/
cf_get_bool(cf, key, def)
CF *cf; char *key; int def;
{
	char *value;

	// Get value
	if((value = cf_get_key(cf, key))) {

		// Check for true or false
		if(!strcmp(value, "true"))
			return 1;
		else if(!strcmp(value, "false"))
			return 0;

		// Failure
	}

	// Failure
	return def;
}

#endif

#ifdef CF_GET_INT

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

#endif

#ifdef CF_GET_UINT

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

#endif

#ifdef CF_GET_STR

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

#endif

// -------------------
// -- SET FUNCTIONS --
// -------------------

#ifdef CF_SET_BOOL

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

#endif

#ifdef CF_SET_STR

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

#endif

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

// -------------------------------
// -- PRIVATE FUNCTIONS: ARRAYS --
// -------------------------------

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

// --------------------------------
// -- PRIVATE FUNCTIONS: STRINGS --
// --------------------------------

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

#undef CF_READ
#undef CF_WRITE
#undef CF_GET_BOOL
#undef CF_GET_INT
#undef CF_GET_UINT
#undef CF_GET_STR
#undef CF_SET_BOOL
#undef CF_SET_STR

#endif
