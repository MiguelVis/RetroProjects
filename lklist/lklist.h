/**
 * @file   lklist.h
 * @brief  Management library for doubly-linked lists in MESCC.
 * @author Miguel I. Garcia Lopez / FloppySoftware
 *
 * Doubly-linked lists management library, for MESCC (Mike's Enhanced
 * Small C Compiler for Z80 & CP/M).
 *
 * Public macros:
 *  - LKLIST       Linked list
 *  - LKLIST_ITEM  Linked list item
 *  - LKLIST_KEYS  Extension for key - value pairs.
 *
 * Revisions:
 *  - 18 Feb 2018 : Released first public version.
 *  - 30 Aug 2018 : Added LkUpdate(). Modified and renamed some functions. Removed code related to _LK_CURSOR.
 *  - 01 Sep 2018 : Added LKLIST_KEYS extension for key - value pairs: LkFindKey(), LkGetKey(), LkSetKey(), LkRemoveKey().
 *
 * Copyright (c) 2018 Miguel I. Garcia Lopez / FloppySoftware.
 *
 * Licensed under the GNU General Public License v3.
 *
 * http://www.floppysoftware.es
 * http://cpm-connections.blogspot.com
 * floppysoftware@gmail.com
 */
#ifndef LKLIST_H

#define LKLIST_H

/* Dependencies
   ------------
*/
/////#include <alloc.h>
/////#include <mem.h>

/* Public macros
   -------------
*/
#define LKLIST      unsigned int  // Linked list
#define LKLIST_ITEM unsigned int  // Linked list item

/* Private macros
   --------------
*/
//#define _LK_DEBUG

#define _LK_FIRST  0  // First item or NULL if empty
#define _LK_LAST   1  // Last item or NULL if empty
#define _LK_ITEMS  2  // # of items

#define _LK_HDRSZ  8  // Size in bytes of header structure

#define _LK_PREV   0  // Pointer to previous item or NULL on first item
#define _LK_NEXT   1  // Pointer to next item or NULL on last item
#define _LK_SIZE   2  // Size of data
#define _LK_DATA   3  // Data offset in WORDS

#define _LK_ITMSZ  6  // Size in bytes of item structure

/**
 * @fn     LKLIST *LkAlloc(void)
 * @brief  Allocate a linked list.
 * @return linked list pointer, or NULL on failure.
 */
LkAlloc()
{
	LKLIST *lk;

	// Alloc memory for header
	if((lk = malloc(_LK_HDRSZ))) {

		// Setup as empty
		lk[_LK_FIRST] = lk[_LK_LAST] = NULL;
		lk[_LK_ITEMS] = 0;
	}

#ifdef _LK_DEBUG
	printf("LkAlloc = %04xH\n", lk);
#endif

	// Success or failure
	return lk;
}

/**
 * @fn     void LkFree(LKLIST *lk)
 * @brief  Free a linked list.
 * @param  lk = linked list pointer
 */
LkFree(lk)
LKLIST *lk;
{
	LKLIST_ITEM *item, *next;

	// Free items
	for(item = lk[_LK_FIRST]; item != NULL; item = next) {

		next = item[_LK_NEXT];

		free(item);

#ifdef _LK_DEBUG
	printf("LkFree = %04xH > %04xH\n", lk, item);
#endif

	}

	// Free header
	free(lk);
}

/**
 * @fn     int LkGetSize(LKLIST *lk)
 * @brief  Get # of items in a linked list.
 * @param  lk = linked list pointer
 * @return # of items
 */
LkGetSize(lk)
LKLIST *lk;
{
	return lk[_LK_ITEMS];
}

/**
 * @fn     LKLIST_ITEM *LkGetFirst(LKLIST *lk)
 * @brief  Get first item in a linked list.
 * @param  lk = linked list pointer
 * @return item pointer, or NULL if linked list is empty
 */
LkGetFirst(lk)
LKLIST *lk;
{
	return lk[_LK_FIRST];
}

/**
 * @fn     LKLIST_ITEM *LkGetLast(LKLIST *lk)
 * @brief  Get last item in a linked list.
 * @param  lk = linked list pointer
 * @return item pointer, or NULL if linked list is empty
 */
LkGetLast(lk)
LKLIST *lk;
{
	return lk[_LK_LAST];
}

/**
 * @fn     LKLIST_ITEM *LkGetPrev(LKLIST_ITEM *item)
 * @brief  Get previous item of a linked list item.
 * @param  item = pointer to item
 * @return item pointer, or NULL if item is the first one
 */
LkGetPrev(item)
LKLIST_ITEM *item;
{
	return item[_LK_PREV];
}

/**
 * @fn     LKLIST_ITEM *LkGetNext(LKLIST_ITEM *item)
 * @brief  Get next item of a linked list item.
 * @param  item = pointer to item
 * @return item pointer, or NULL if item is the last one
 */
LkGetNext(item)
LKLIST_ITEM *item;
{
	return item[_LK_NEXT];
}

/**
 * @fn     void *LkGetData(LKLIST_ITEM *item)
 * @brief  Get pointer to item data.
 * @param  item = pointer to item
 * @return data pointer
 */
LkGetData(item)
LKLIST_ITEM *item;
{
	return item + _LK_DATA;
}

/**
 * @fn     void *LkGetDataSize(LKLIST_ITEM *item)
 * @brief  Get item data size in bytes.
 * @param  item = pointer to item
 * @return data size in bytes
 */
LkGetDataSize(item)
LKLIST_ITEM *item;
{
	return item[_LK_SIZE];
}

/**
 * @fn     LKLIST_ITEM *LkAdd(LKLIST *lk, void *data, int size)
 * @brief  Add an item to a linked list.
 * @param  lk = linked list pointer
 * @param  data = pointer to data
 * @param  size = data size in bytes
 * @return item pointer, or NULL if there is not enough memory
 */
LkAdd(lk, data, size)
LKLIST *lk;
BYTE *data;
int size;
{
	LKLIST_ITEM *item, *prev;

	if((item = malloc(_LK_ITMSZ + size))) {

		// Setup new item
		item[_LK_PREV] = lk[_LK_LAST];
		item[_LK_NEXT] = NULL;
		item[_LK_SIZE] = size;

		memcpy(item + _LK_DATA, data, size);

		// Modify previous item
		if((prev = lk[_LK_LAST])) {
			prev[_LK_NEXT] = item;
		}

		// Modify header
		if(!lk[_LK_FIRST]) {
			lk[_LK_FIRST] = item;
		}

		lk[_LK_LAST] = item;

		++lk[_LK_ITEMS];

#ifdef _LK_DEBUG
	printf("LkAdd = %04xH < %04xH [%d bytes]\n", lk, item, size);
#endif

	}

	// Success or failure
	return item;
}

/**
 * @fn     LKLIST_ITEM *LkUpdate(LKLIST *lk, LKLIST_ITEM *item, void *data, int size)
 * @brief  Update an item in a linked list.
 * @param  lk = linked list pointer
 * @param  item = pointer to item
 * @param  data = pointer to data
 * @param  size = data size in bytes
 * @return item pointer, or NULL if there is not enough memory
 */
LkUpdate(lk, item, data, size)
LKLIST *lk;
LKLIST_ITEM *item;
BYTE *data;
int size;
{
	LKLIST_ITEM *old_item, *prev, *next;

	if(size != item[_LK_SIZE]) {
		old_item = item;

		if((item = malloc(_LK_ITMSZ + size))) {

			// Setup new item
			item[_LK_PREV] = prev = old_item[_LK_PREV];
			item[_LK_NEXT] = next = old_item[_LK_NEXT];
			item[_LK_SIZE] = size;

			// Modify previous item if any
			if(prev) {
				prev[_LK_NEXT] = item;
			}
			else {
				// It's the first one
				lk[_LK_FIRST] = item;
			}

			// Modify next item if any
			if(next) {
				next[_LK_PREV] = item;
			}
			else {
				// It's the last one
				lk[_LK_LAST] = item;
			}

			free(old_item);
		}
	}

	if(item) {
		memcpy(item + _LK_DATA, data, size);

#ifdef _LK_DEBUG
		printf("LkUpdate = %04xH < %04xH [%d bytes]\n", lk, item, size);
#endif

	}

	// Success or failure
	return item;
}

/* Remove an item
   --------------
*/
/**
 * @fn     void LkRemove(LKLIST *lk, LKLIST_ITEM *item)
 * @brief  Remove an item in a linked list.
 * @param  lk = linked list pointer
 * @param  item = pointer to item
 */
LkRemove(lk, item)
LKLIST *lk;
LKLIST_ITEM *item;
{
	LKLIST_ITEM *prev, *next;

	// Modify previous item if any
	if((prev = item[_LK_PREV])) {
		prev[_LK_NEXT] = item[_LK_NEXT];
	}
	else {
		// It's the first one
		lk[_LK_FIRST] = item[_LK_NEXT];
	}

	// Modify next item if any
	if((next = item[_LK_NEXT])) {
		next[_LK_PREV] = item[_LK_PREV];
	}
	else {
		// It's the last one
		lk[_LK_LAST] = item[_LK_PREV];
	}

	--lk[_LK_SIZE];

	free(item);

#ifdef _LK_DEBUG
	printf("LkDel = %04xH > %04xH\n", lk, item);
#endif

}

/* Key - value pairs extension
   ---------------------------
*/
#ifdef LKLIST_KEYS

/**
 * @fn     LKLIST_ITEM *LkFindKey(LKLIST *lk, char *key)
 * @brief  Find a key in a linked list.
 * @param  lk = linked list pointer
 * @param  key = key to find
 * @return item pointer, or NULL if key does not exist
 */
LkFindKey(lk, key)
LKLIST *lk;
char *key;
{
	LKLIST_ITEM *item;

	for(item = LkGetFirst(lk); item != NULL; item = LkGetNext(item)) {
		if(strcmp(key, LkGetData(item)) == 0) {
			return item;
		}
	}

	return NULL;
}

/**
 * @fn     char *LkGetKey(LKLIST *lk, char *key)
 * @brief  Return a key value in a linked list.
 * @param  lk = linked list pointer
 * @param  key = key to find
 * @return key value, or NULL if key does not exist
 */
LkGetKey(lk, key)
LKLIST *lk;
char *key;
{
	LKLIST_ITEM *item;

	if((item = LkFindKey(lk, key)) != NULL) {
		return LkGetData(item) + strlen(key) + 1;
	}

	return NULL;
}

/**
 * @fn     LKLIST_ITEM *LkFindKey(LKLIST *lk, char *key, char *value)
 * @brief  Add or update a key in a linked list.
 * @param  lk = linked list pointer
 * @param  key = key to add or update
 * @param  value = key value to add or update
 * @return item pointer, or NULL if there is not enough memory
 */
LkSetKey(lk, key, value)
LKLIST *lk;
char *key;
char *value;
{
	LKLIST_ITEM *item;
	char *data;
	int key_length;
	int size;

	size = (key_length = strlen(key)) + strlen(value) + 2;

	if((data = malloc(size))) {
		strcpy(strcpy(data, key) + key_length + 1, value);

		if((item = LkFindKey(lk, key)) == NULL) {
			item = LkAdd(lk, data, size);
		}
		else {
			item = LkUpdate(lk, item, data, size);
		}

		free(data);

		return item;
	}

	return NULL;
}

/**
 * @fn     void LkGetKey(LKLIST *lk, char *key)
 * @brief  Remove a key in a linked list if exists
 * @param  lk = linked list pointer
 * @param  key = key to remove
 */
LkRemoveKey(lk, key)
LKLIST *lk;
char *key;
{
	LKLIST_ITEM *item;

	if((item = LkFindKey(lk, key)) != NULL) {
		LkRemove(lk, item);
	}
}

#endif

#ifdef _LK_DEBUG

/* Print linked list data
   ----------------------
*/
LkPrint(lk)
LKLIST *lk;
{
	LKLIST_ITEM *item;
	int n;

	printf("LkPrint : first item = %04xH\n", lk[_LK_FIRST]);
	printf("LkPrint : last item  = %04xH\n", lk[_LK_LAST]);
	printf("LkPrint : # of items = %d\n\n", lk[_LK_ITEMS]);

	n = 0;

	for(item = lk[_LK_FIRST]; item != NULL; item = item[_LK_NEXT]) {

		printf("LkPrint : #%5u = %04xH\n", n, item);
		printf("LkPrint : #%5u = prev %04xH\n", n, item[_LK_PREV]);
		printf("LkPrint : #%5u = next %04xH\n", n, item[_LK_NEXT]);
		printf("LkPrint : #%5u = size %d\n\n", n, item[_LK_SIZE]);

		++n;
	}
}

#endif

/* Cleaning
   --------
*/
#undef LKLIST_KEYS

#undef _LK_DEBUG

#undef _LK_FIRST
#undef _LK_LAST
#undef _LK_ITEMS

#undef _LK_HDRSZ

#undef _LK_PREV
#undef _LK_NEXT
#undef _LK_SIZE
#undef _LK_DATA

#undef _LK_ITMSZ

#endif

