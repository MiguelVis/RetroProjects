/**
 * @file   alloc.h
 * @brief  Dynamic memory allocation.
 * @author Miguel I. Garcia Lopez / FloppySoftware
 *
 * Dynamic memory allocation functions, for MESCC (Mike's Enhanced
 * Small C Compiler for Z80 & CP/M).
 *
 * Format of each memory block:
 *  - WORD size;        size of data
 *  - BYTE used;        0 = no, 1 = yes
 *  - BYTE data[size];  data
 *
 * Revisions:
 *  - 13 Dec 2000 : Last revision.
 *  - 16 Apr 2007 : GPL'd.
 *  - 26 Aug 2012 : Changed some things for more speed.
 *  - 19 Feb 2015 : Now free() checks if pointer is NULL.
 *  - 15 Aug 2016 : Optimized and documented. GPL v3.
 *
 * Copyright (c) 1999-2016 Miguel I. Garcia Lopez / FloppySoftware.
 *
 * Licensed under the GNU General Public License v3.
 *
 * http://www.floppysoftware.es
 * floppysoftware@gmail.com
 */
#ifndef ALLOC_H

#define ALLOC_H

//#define DEBUG_ALLOC

extern BYTE *ccfreefirst;
extern WORD ccfreebytes;

BYTE xm_ini,  // 0x69 if initialized
     *xm_top, // First block
     *xm_end; // Last block

/**
 * @fn     void *malloc(unsigned int size)
 * @brief  Allocate memory.
 *
 * This function tries to allocated a memory block of requested
 * size in bytes.
 *
 * The contents of the allocated memory block is undefined.
 *
 * @param  size - needed size in bytes
 * @return pointer to allocated memory, or null pointer on failure
 */
malloc(size)
WORD size;
{
	BYTE *memptr;
	WORD memsize;

	// Setup library if needed

	if(xm_ini != 0x69)
	{
		if(ccfreebytes < 7)
			return 0;

		xm_top = ccfreefirst;
		xm_end = xm_top + ccfreebytes - 3;

		xm_pw(xm_top, ccfreebytes - 6);
		xm_top[2] = 0;

		xm_pw(xm_end, 0);
		xm_end[2] = 1;

		xm_ini = 0x69;
	}

	// Search a free block

	for(memptr = xm_top; memptr != xm_end; memptr += memsize + 3)
	{
		memsize = xm_gw(memptr);

		if(!memptr[2])
		{
			if(memsize >= size)
			{
				memptr[2] = 1;

				if(memsize >= size + 3)
				{
					xm_pw(memptr, size);
					xm_pw(memptr + size + 3, memsize - size - 3);
					*(memptr + size + 5) = 0;
				}

#ifdef DEBUG_ALLOC
	alloc_dbg("malloc", size);
#endif
				return memptr + 3;
			}
		}
	}

#ifdef DEBUG_ALLOC
	alloc_dbg("malloc", size);
#endif

	return NULL;
}

/**
 * @fn     void free(void *ptr)
 * @brief  Deallocate memory.
 *
 * This function deallocates memory, previously allocated with malloc.
 *
 * If ptr is a null pointer, the function does nothing.
 *
 * @param  ptr - memory block to deallocate
 */
free(ptr)
BYTE *ptr;
{
	char *memptr;
	unsigned memsize;

	// Do nothing on null pointer

	if(!ptr)
		return;

	// Make free

	*(ptr - 1) = 0;

	// Join to another free memory blocks if possible

	for(memptr = xm_top; memptr != xm_end; memptr += memsize + 3)
	{
		memsize = xm_gw(memptr);

		if(!memptr[2])
		{
			if(!(*(memptr + memsize + 5)))
			{
				memsize += xm_gw(memptr + memsize + 3) + 3;

				if(!(*(memptr + memsize + 5)))
					memsize += xm_gw(memptr + memsize + 3) + 3;

				xm_pw(memptr, memsize);

				break;
			}
		}
	}

#ifdef DEBUG_ALLOC
	alloc_dbg("free", ptr);
#endif
}

// int xm_gw(int *ptr) : get an int from ptr.

xm_gw(ptr)
WORD *ptr;
{
	return *ptr;
}

// void xm_pw(int *ptr, int val) : put an int into ptr.

xm_pw(ptr, val)
WORD *ptr, val;
{
	*ptr=val;
}

#ifdef DEBUG_ALLOC

// void alloc_dbg(char *fn, WORD wrd) : Quick and dirty debug.

alloc_dbg(fn, wrd)
char *fn; WORD wrd;
{
	BYTE *pb, data[4];
	WORD size, *pw;
	int use;

	printf("SP = %04x (%s %04x)\n", alloc_sp(), fn, wrd);

	printf("Addr Size       Next Use Data\n");

	for(pb = xm_top; pb != xm_end; pb += size + 3)
	{
		pw = pb;
		size = *pw;
		use = pb[2];
		memcpy(data, pb + 3, 4);

		printf("%04x %04x %5u %04x %s \n", pb, size, size, pb + 3 + size, use ? "Used" : "Free");
	}

	getch();
}

#asm
alloc_sp
	LD  HL,0
	ADD HL,SP
	RET
#endasm

#endif

// Cleaning

#undef DEBUG_ALLOC

#endif

