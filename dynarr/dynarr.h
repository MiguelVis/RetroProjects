/*	dynarr.h

	Management library for dynamic arrays of strings under MESCC.

	Copyright (c) 2017 Miguel I. Garcia Lopez / FloppySoftware, Spain

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

	Author's contact:

		www.floppysoftware.es
		cpm-connections.blogspot.com
		floppysoftware@gmail.com

	Revisions:

		11 Jul 2017 : Start.
*/

#ifndef DYNARR_H

#define DYNARR_H

/* Dependencies
   ------------
*/
/////#include <alloc.h>
/////#include <string.h>
/////#include <mem.h>

/* Public defines
   --------------
*/
#define DYNARR unsigned int  // void *

/* Private defines
   ---------------
*/
#define DY_DEBUG

#define DY_SIZE   0  // # of entries
#define DY_DATA   1  // Pointer to vector or null if # of entries == 0

#define DY_FIELDS 2  // # of fields in above structure

/* Allocate a dynamic array
   ------------------------
   Return address, or NULL on failure.
*/
DyAlloc()
{
	DYNARR *dyn;

	// Alloc memory for empty structure
	if((dyn = malloc(DY_FIELDS * SIZEOF_PTR))) {

		// Setup the array
		dyn[DY_SIZE] = 0;
		dyn[DY_DATA] = NULL;
	}

#ifdef DY_DEBUG
	printf("DyAlloc = %u\n", dyn);
#endif

	// Success or failure
	return dyn;
}

/* Free a dynamic array
   --------------------
*/
DyFree(dyn)
DYNARR *dyn;
{
	int dyn_len;
	WORD *dyn_arr;  // char *[]
	int i;

	if(dyn) {
		dyn_len = dyn[DY_SIZE];
		dyn_arr = dyn[DY_DATA];

		// Free allocated entries if any
		if(dyn_len) {
			for(i = 0; i < dyn_len; ++i) {
#ifdef DY_DEBUG
	printf("DyFree = %u > %u\n", dyn, dyn_arr[i]);
#endif
				free(dyn_arr[i]);
			}
		}

#ifdef DY_DEBUG
	printf("DyFree = %u\n", dyn);
#endif

		// Free the structure
		free(dyn);
	}
}

/* Get length
   ----------
   Return length or -1 on failure.
*/
DyLength(dyn)
DYNARR *dyn;
{
	if(dyn) {
		return dyn[DY_SIZE];
	}

	//
	return -1;
}

/* Get entry
   ----------
   Return entry or NULL on failure.
*/
DyGet(dyn, entry)
DYNARR *dyn;
int entry;
{
	WORD *dyn_arr;

	if(dyn && entry >= 0 && entry < dyn[DY_SIZE]) {
		dyn_arr = dyn[DY_DATA];

		return dyn_arr[entry];
	}

	//
	return NULL;
}

/* Find a value
   -------------
   Return entry number, or -1 on failure.
*/
DyFind(dyn, data, size)
DYNARR *dyn;
char *data;
int size;
{
	int dyn_len;
	WORD *dyn_arr;  // char *[]
	int i;

	if(dyn) {
		dyn_len = dyn[DY_SIZE];
		dyn_arr = dyn[DY_DATA];

		if(dyn_len) {
			for(i = 0; i < dyn_len; ++i) {
				if(!memcmp(dyn_arr[i], data, size)) {
					// Found
					return i;
				}
			}
		}
	}

	// Not found
	return -1;
}

/* Find a string
   -------------
   Return entry number, or -1 on failure.
*/
DyFindStr(dyn, str)
DYNARR *dyn;
char *str;
{
	return DyFind(dyn, str, strlen(str) + 1);
}

/* Add an entry
   ------------
   Return entry number, or -1 on failure.
*/
DyAdd(dyn, data, size)
DYNARR *dyn;
char *data; // void *
int size;
{
	int dyn_len;
	WORD *dyn_arr;  // char *[]
	WORD *arr; // char *[]
	char *val;

	if(dyn && size) {
		dyn_len = dyn[DY_SIZE];
		dyn_arr = dyn[DY_DATA];

		// Alloc space for another entry in vector  -- FIXME: MESCC doesn't have realloc() yet
		if((arr = malloc((dyn_len + 1) * SIZEOF_PTR))) {
			if((val = malloc(size))) {
				// Setup vector
				if(dyn_len) {
					memcpy(arr, dyn_arr, dyn_len * SIZEOF_PTR);

					free(dyn_arr);
				}

				arr[dyn_len] = val;

				memcpy(val, data, size);

				dyn[DY_DATA] = arr;
				dyn[DY_SIZE] = dyn_len + 1;

#ifdef DY_DEBUG
	printf("DyAdd = %u < %u [%d bytes]\n",dyn, arr, size);
#endif

				return dyn_len;
			}

			// Free
			free(arr);
		}
	}

	// Failure
	return -1;
}

/* Add a string
   ------------
   Return entry number, or -1 on failure.
*/
DyAddStr(dyn, str)
DYNARR *dyn;
char *str;
{
#ifdef DY_DEBUG
	printf("DyAddStr = %u < '%s'\n", dyn, str);
#endif

	return DyAdd(dyn, str, strlen(str) + 1);
}


/*
DySet(dyn, str)
DYNARR *dyn;
char *str;
{
	return DyAdd(dyn, str, strlen(str) + 1);
}
*/

/* Delete an entry
   ---------------
   Return entry number, or -1 on failure.
*/
DyDel(dyn, entry)
DYNARR *dyn;
int entry;
{
	int dyn_len;
	WORD *dyn_arr;  // char *[]
	WORD *arr; // char *[]

	if(dyn && entry >= 0 && entry < dyn[DY_SIZE]) {
		dyn_len = dyn[DY_SIZE];
		dyn_arr = dyn[DY_DATA];

		if(dyn_len > 1) {
			// Alloc space for new vector  -- FIXME: MESCC doesn't have realloc() yet
			if(!(arr = malloc((dyn_len - 1) * SIZEOF_PTR))) {
				// Failure
				return -1;
			}

			// ...memcpy(arr, dyn_arr, dyn_len * SIZEOF_PTR);

			if(entry > 0) {
				memcpy(arr, dyn_arr, entry * SIZEOF_PTR);
			}

			if(entry < dyn_len - 1) {
				memcpy(arr + entry * SIZEOF_PTR, dyn_arr + (entry + 1) * SIZEOF_PTR, (dyn_len - 1 - entry) * SIZEOF_PTR);
			}

			dyn[DY_DATA] = arr;
		}
		else {
			dyn[DY_DATA] = NULL;
		}

		free(dyn_arr);

		--dyn[DY_SIZE];

#ifdef DY_DEBUG
	printf("DyDel = %u > entry %d\n", dyn, entry);
#endif

		return entry;
	}

	// Failure
	return -1;
}

#ifdef DY_DEBUG

/* Print string array contents
   ---------------------------
*/
DyPrint(dyn)
DYNARR *dyn;
{
	int i;

	for(i = 0; i < DyLength(dyn); ++i) {
		printf("%03d : '%s'\n", i, DyGet(dyn, i));
	}
}

#endif

#endif

