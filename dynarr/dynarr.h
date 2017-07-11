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

/* Private defines
   ---------------
*/
#define DY_DEBUG

/* Allocate and clear an array
   ---------------------------
   Return array address, or NULL on failure.
*/
DyAlloc(max)
int max;
{
	unsigned int *arr; // char *arr[]
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
DyFree(arr, size)
unsigned int *arr; // char *arr[]
int size;
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
DyFind(arr, size, key)
unsigned int *arr; // char *arr[]
int size; char *key;
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
DyAdd(arr, size, data)
unsigned int *arr; // char *arr[]
int size; char *data;
{
	int i;

	// Search for unallocated entry in the array
	for(i = 0; i < size; ++i) {
		if(!arr[i]) {

			// Set the value and return success or failure
			return DySet(arr, data, i) ? i : -1;
		}
	}

	// Failure
	return -1;
}

/* Set a value in an array
   -----------------------
   Return pointer to the entry data, or NULL on failure.
*/
DySet(arr, data, entry)
unsigned int *arr; // char *arr[]
char *data; int entry;
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
DyDel(arr, entry)
unsigned int *arr; // char *arr[]
int entry;
{
	// Free old data if needed
	if(arr[entry])
		free(arr[entry]);

	// Update pointer and return NULL
	return (arr[entry] = NULL);
}

#ifdef DY_DEBUG

/* Print array contents
   --------------------
*/
DyPrint(arr, size)
unsigned int *arr; // char *arr[]
int size;
{
	int i;

	for(i = 0; i < size; ++i) {
		if(arr[i]) {
			printf("%03d : '%s'\n", i, arr[i]);
		}
		else {
			printf("%03d : NULL\n", i);
		}
	}
}

#endif

#endif

