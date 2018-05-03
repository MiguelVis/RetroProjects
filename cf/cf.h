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
	16 Jul 2016 : Added CfGetAll().
	21 Oct 2016 : Solved a couple of bugs in CfRead() and xCfAdd(). Refactorized function names.
	24 Oct 2016 : Modified CfRead(), CfWrite(), CfSetKey() to support reading and writing empty lines and comments.
	01 Jan 2017 : Don't destroy CF on error in CfRead().
	03 May 2018 : Solved bug in quoted strings.

	Supported #defines:

	CF_READ     : CfRead().
	CF_WRITE    : CfWrite().
	CF_GET_ALL  : CfGetAll().
	CF_GET_BOOL : CfGetBool().
	CF_GET_INT  : CfGetInt().
	CF_GET_UINT : CfGetUint().
	CF_GET_STR  : CfGetStr().
	CF_SET_BOOL : CfSetBool().
	CF_SET_STR  : CfSetStr().

	Notes:

	Valid characters for key names: A..Z a..z 0..9 . _ -
	Valid characters for comments:  # ;
*/

#ifndef CF_H

#define CF_H

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
CfCreate(max)
int max;
{
	CF *cf;

	// Alloc memory
	if((cf = malloc(SIZEOF_PTR * XCF_FIELDS))) {
		if((cf[XCF_FKEYS] = xCfAlloc(max))) {
			if((cf[XCF_FVALUES] = xCfAlloc(max))) {

				// Store the rest of fields
				cf[XCF_FMAX] = max;

				// Success
				return cf;
			}

			// Failure
			xCfFree(cf[XCF_FKEYS], max);
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
CfDestroy(cf)
CF *cf;
{
	// Free the arrays and its entries
	xCfFree(cf[XCF_FKEYS], cf[XCF_FMAX]);
	xCfFree(cf[XCF_FVALUES], cf[XCF_FMAX]);

	// Free the buffer
	free(cf);

	// Done
	return NULL;
}

/* Set the value of a key
   ----------------------
   Return 0 on success, or -1 on failure.
*/
CfSetKey(cf, key, value)
CF *cf; char *key, *value;
{
	int entry;

	// Get entry number
	if((entry = (*key && *key != '#' && *key != ';') ? xCfFind(cf[XCF_FKEYS], cf[XCF_FMAX], key) : -1) != -1) {

		// The key exists: change its value
		if(xCfSet(cf[XCF_FVALUES], value, entry)) {

			// Success
			return 0;
		}
	}
	else {
		// The key doesn't exists: add it
		if((entry = xCfAdd(cf[XCF_FKEYS], cf[XCF_FMAX], key)) != -1) {

			// Add the value
			if(xCfSet(cf[XCF_FVALUES], value, entry)) {

				// Success
				return 0;
			}

			// Failure
			xCfDel(cf[XCF_FKEYS], entry);
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
CfGetKey(cf, key)
CF *cf; char *key;
{
	int entry;
	unsigned int *arr;

	// Get entry number
	if((entry = xCfFind(cf[XCF_FKEYS], cf[XCF_FMAX], key)) != -1) {

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
CfRead(cf, fname, cmt)
CF *cf; char *fname; int cmt;
{
	FILE *fp;
	int err, k;
	char *bf, *key, cmt_key[2];

	// Default: no errors
	err = cmt_key[1] = 0;

	// Open file
	if((fp = fopen(fname, "r"))) {
		while(fgets(xcf_buf, XCF_BF_SIZE, fp)) {

			// Get the length of the string
			k = strlen(xcf_buf);

			// Remove the trailing new line if any,
			// and check for too long lines.
			if(xcf_buf[k - 1] == '\n')
				xcf_buf[k - 1] = '\0';
			else if(k == XCF_BF_SIZE - 1) {
				// Line too long
				err = -1; break;
			}

			// Remove spaces on the left
			bf = xCfLfSpaces(xcf_buf);

			// Remove spaces on the right
			bf = xCfRtSpaces(bf);

			// Skip comments and empty lines
			if(!(*bf) || *bf == '#' || *bf == ';') {
				if(cmt)	{
					cmt_key[0] = *bf;

					if(*bf) {
						// Remove spaces on the left
						bf = xCfLfSpaces(bf + 1);
					}

					if(CfSetKey(cf, cmt_key, bf)) {
						//CfDestroy(cf);
						err = -1;
						break;
					}
				}

				continue;
			}

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
				bf = xCfLfSpaces(bf);

				// Check for =
				if(*bf != '=') {
					err = -1; break;
				}

				// Skip =
				++bf;
			}

			// Skip spaces
			bf = xCfLfSpaces(bf);

			// Remove quotes if any
			if(!(bf = xCfQuotes(bf))) {
				err = -1; break;
			}

			// Check if there is a value
			if(!(*bf)) {
				err = -1; break;
			}

			// Add the key / value pair to the configuration buffer
			if(CfSetKey(cf, key, bf)) {
				//CfDestroy(cf);
				err = -1;
				break;
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
CfWrite(cf, fname)
CF *cf; char *fname;
{
	FILE *fp;
	unsigned int *arrk, *arrv;
	int max, i, k;
	char *s;

	// Open file
	if((fp = fopen(fname, "w"))) {

		// Get fields
		arrk = cf[XCF_FKEYS];
		arrv = cf[XCF_FVALUES];
		max  = cf[XCF_FMAX];

		// Write key / value pairs
		for(i = 0; i < max; ++i) {
			if(arrk[i]) {

				// Get variable name or start of comment
				s = arrk[i];

				if(*s) {
					// Remember first character of variable name
					k = *s;

					// Output variable name or start of comment
					fputs(s, fp);

					fputc(' ', fp);

					// Get variable value or comment
					s = arrv[i];

					if(k != '#' && k != ';') {
						fputc('=', fp);
						fputc(' ', fp);

						// Check for spaces
						if(strchr(s, ' ')) {

							// Write as "string"
							fputc('\"', fp);
							fputs(s, fp);
							fputc('\"', fp);

						}
						else {
							// Write raw
							fputs(s, fp);
						}
					}
					else {
						// Write comment
						fputs(s, fp);
					}
				}

				// End of line
				fputc('\n', fp);
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
CfGetAll(cf, funct)
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
CfGetBool(cf, key, def)
CF *cf; char *key; int def;
{
	char *value;

	// Get value
	if((value = CfGetKey(cf, key))) {

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
CfGetInt(cf, key, def)
CF *cf; char *key; int def;
{
	char *value;

	// Get value
	if((value = CfGetKey(cf, key))) {

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
CfGetUint(cf, key, def)
CF *cf; char *key; unsigned int def;
{
	char *value;
	unsigned int val;

	// Setup value
	val = 0;

	// Get value
	if((value = CfGetKey(cf, key))) {

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
CfGetStr(cf, key, def)
CF *cf; char *key, *def;
{
	char *value;

	// Get value
	if((value = CfGetKey(cf, key))) {

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
CfSetBool(cf, key, value)
CF *cf; char *key; int value;
{
	// Set the value
	// and return the result.
	return CfSetKey(cf, key, value ? "true" : "false");
}

#endif

#ifdef CF_SET_STR

/* Set the string value of a key
   -----------------------------
   Return 0 on success, or -1 on failure.
*/
CfSetStr(cf, key, value)
CF *cf; char *key, *value;
{
	// Return success or failure
	return CfSetKey(cf, key, value);
}

#endif

// ---------------------
// -- DEBUG FUNCTIONS --
// ---------------------

#if XCF_DEBUG

/* Print keys and values
   ---------------------
*/
CfPrKeys(cf)
CF *cf;
{
	unsigned int *arrk, *arrv;
	int i;
	char *s;

	arrk = cf[XCF_FKEYS];
	arrv = cf[XCF_FVALUES];

	for(i = 0; i < cf[XCF_FMAX]; ++i) {
		if(arrk[i]) {
			s = arrk[i];

			if(*s) {
				printf((*s == '#' || *s == ';') ? "%02d : %s %s\n" : "%02d : %s = %s\n", i, arrk[i], arrv[i]);
			}
			else {
				printf("%02d :\n", i);
			}
		}
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
xCfAlloc(max)
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
xCfFree(arr, size)
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
xCfFind(arr, size, key)
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
xCfAdd(arr, size, data)
unsigned int *arr; int size; char *data;
{
	int i;

	// Search for unallocated entry in the array
	for(i = 0; i < size; ++i) {
		if(!arr[i]) {

			// Set the value and return success or failure
			return xCfSet(arr, data, i) ? i : -1;
		}
	}

	// Failure
	return -1;
}

/* Set a value in an array
   -----------------------
   Return pointer to the entry data, or NULL on failure.
*/
xCfSet(arr, data, entry)
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
xCfDel(arr, entry)
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

/* Skip spaces on the left
   -----------------------
   Return pointer to the first non space character.
*/
xCfLfSpaces(s)
char *s;
{
	// Skip spaces
	while(*s == ' ' || *s == '\t')
		++s;

	// Return pointer
	return s;
}

/* Skip spaces on the right
   ------------------------
   Return pointer to the string.
*/
xCfRtSpaces(s)
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

/* Remove double quotes if any
   ---------------------------
   Return pointer to the string or NULL on error.
*/
xCfQuotes(s)
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
		else
			s = NULL;
	}

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
