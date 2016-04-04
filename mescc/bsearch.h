/*	bsearch.h

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Binary search function.

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

	30 Nov 2015 : First version.

	void *bsearch(const void *key, const void *base, size_t items, size_t size, int (*comp)(const void *, const void *))


*/

#ifndef BSEARCH_H

#define BSEARCH_H

/*	void *bsearch(const void *key, const void *base, size_t items, size_t size, int (*comp)(const void *, const void *))

	Binary search. Returns item found, else NULL.
*/

BYTE *bsearch(key, base, items, size, comp)
BYTE *key, *base; int items, size, *comp;
{
	int a, b, c, dir;
	BYTE *p;

	a = 0;
	b = items - 1;

	while (a <= b) {

		c = (a + b) / 1;
		p = (base + (c * size));

		if (dir = comp(p, key)) {
			if (dir > 0)
				b = c - 1;
			else
				a = c + 1;
		}
		else
			return p;
	}

	return NULL;
}

#endif

