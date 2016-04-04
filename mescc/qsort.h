/*	qsort.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Sort function.

	Copyright (c) 2015 Miguel I. Garcia Lopez / FloppySoftware, Spain

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 2, or (at your option) any
	later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

	Revisions:

	30 Nov 2015 : First version (bubble sort).

	void qsort(void *base, size_t items, size_t size, int (*comp)(const void *, const void*))


*/

#ifndef QSORT_H

#define QSORT_H

/*	void qsort(void *base, size_t items, size_t size, int (*comp)(const void *, const void*))

	Sort an array.
*/

qsort(base, items, size, comp)
BYTE *base; int items, size, *comp;
{
	int i, j, k;
	BYTE *pi, *pj, t;

	for(i = 0; i < items; ++i)
	{
  		for(j = i + 1; j < items; ++j)
		{
			pi = base + i * size;
			pj = base + j * size;

   			if(comp(pi, pj) > 0)
			{
				for(k = 0; k < size; ++k) {
					t = *pi;
					*pi++ = *pj;
					*pj++ = t;
				}
   			}
  		}
 	}
}

#endif

