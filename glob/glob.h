/**
 * @file   glob.h
 * @brief  Library for filename expansion using wildcards.
 * @author Miguel I. Garcia Lopez / FloppySoftware
 *
 * Library for filename expansion using wildcards for MESCC (Mike's Enhanced
 * Small C Compiler for Z80 & CP/M).
 *
 * Public macros:
 *  - GLOB_INC_DRIVE to include drive in returned filenames
 *  - GLOB_INC_USER to include user # in returned filenames
 *  - GLOB_INC_PATH to include both drive and user # in returned filenames
 *
 * Supports following MESCC macros:
 *  - CC_FCX to support FCX (user # in file names, see cpm.h)
 *
 * Dependencies:
 *  - lklib.h (linked lists for MESCC)
 *
 * Revisions:
 *  - 17 Feb 2018 : First version.
 *
 * Copyright (c) 2018 Miguel I. Garcia Lopez / FloppySoftware.
 *
 * Licensed under the GNU General Public License v3.
 *
 * http://www.floppysoftware.es
 * http://cpm-connections.blogspot.com
 * floppysoftware@gmail.com
 */
#ifndef GLOB_H

#define GLOB_H

/* Public macros
   -------------
*/
#define GLOB_INC_DRIVE 1
#define GLOB_INC_USER  2
#define GLOB_INC_PATH  (GLOB_INC_DRIVE | GLOB_INC_USER)

/* Private macros
   --------------
*/
#ifdef CC_FCX
#define _GLOB_FCB_SIZE UX_FCX_SIZ
#define _GLOB_SET_FCB  setfcx
#define _GLOB_FCB_DRV  UX_FCX_DRV
#else
#define _GLOB_FCB_SIZE 36
#define _GLOB_SET_FCB  setfcb
#define _GLOB_FCB_DRV  0
#endif

/**
 * @fn     int glob(char *fname, LKLIST *lk, WORD flags)
 * @brief  Do filename expansion using wildcards.
 * @param  fname - filename with optional wildcards
 * @param  lk - linked list for storage of found filenames
 * @param  flags - optional flags
 * @return # of found filenames on success, else -1
 */
glob(fname, lk, flags)
char *fname; LKLIST *lk; WORD flags;
{
	char *pc, fn[17]; /* duu:filename.typ + ZERO */
	BYTE *pb, fcb[_GLOB_FCB_SIZE];
	int i, count, ret, drive, user, user2;
#ifdef CC_FCX
	int old_user;
#endif

	/* Set FCB */
	if(_GLOB_SET_FCB(fname, fcb)) {
		return -1;
	}

	/* Get FCB drive */
	if(flags & GLOB_INC_DRIVE) {
		drive = 'A' + (fcb[_GLOB_FCB_DRV] ? fcb[_GLOB_FCB_DRV] - 1 : bdos_a(BF_GETDRV, 0xCACA));
	}

#ifdef CC_FCX
	/* Get current user if needed */
	old_user = -1;

	if(fcb[UX_FCX_USR]) {
		if((ret = bdos_a(BF_USER, 0xFFFF)) != fcb[UX_FCX_USR] - 1)	{

			old_user = ret;

			bdos_a(BF_USER, fcb[UX_FCX_USR] - 1);
		}
	}
#endif

	/* Get FCB user */
	if(flags & GLOB_INC_USER) {
#ifdef CC_FCX
		if(fcb[UX_FCX_USR]) {
			user = fcb[UX_FCX_USR] - 1;
		}
		else {
			user = (old_user != -1 ? old_user : bdos_a(BF_USER, 0xFFFF));
		}
#else
		user = bdos_a(BF_USER, 0xFFFF);
#endif
		if(user < 10) {
			user2 = '0' + user;
			user  = '0';
		}
		else {
			user2 = '0' + (user - 10);
			user  = '1';
		}
	}

	/* Set DMA */
	bdos_hl(BF_DMA, 0x0080);

	/* Find files */
	count = 0;

#ifdef CC_FCX
	ret = bdos_a(BF_FIND1ST, fcb + UX_FCX_DRV);
#else
	ret = bdos_a(BF_FIND1ST, fcb);
#endif

	while(ret != 0xFF) {
		/* Point to filename in directory entry */
		pb = 0x0080 + (ret << 5) + 1; /* ret * 32 + 1 */
		pc = fn;

		/* Drive */
		if(flags & GLOB_INC_DRIVE) {
			*pc++ = drive;
		}

		/* User */
		if(flags & GLOB_INC_USER) {
			*pc++ = user;
			*pc++ = user2;
		}

		/* Path separator */
		if(flags & GLOB_INC_PATH) {
			*pc++ = ':';
		}

		/* Filename */
		for(i = 0; i < 8; ++i) {
			if((ret = (pb[i] & 0x7F)) == ' ') {
				break;
			}

			*pc++ = ret;
		}

		/* File type separator */
		if((ret = (pb[8] & 0x7F)) != ' ') {
			*pc++ = '.';
			*pc++ = ret;

			for(i = 9; i < 11; ++i) {
				if((ret = (pb[i] & 0x7F)) == ' ') {
					break;
				}

				*pc++ = ret;
			}
		}

		/* EOS */
		*pc = '\0';

		/* Add item to linked list */
		//if(!LkAdd(lk, fn, strlen(fn) + 1)) {
		if(!LkAdd(lk, fn, pc - fn + 1)) {
			return -1;
		}

		++count;

#ifdef CC_FCX
		ret = bdos_a(BF_FINDNEXT, fcb + UX_FCX_DRV);
#else
		ret = bdos_a(BF_FINDNEXT, fcb);
#endif
	}

#ifdef CC_FCX
	/* Set old user */
	if(old_user != -1) {
		bdos_a(BF_USER, old_user);
	}
#endif

	/* Return # of matched files */
	return count;
}

/* Cleaning
   --------
*/
#undef _GLOB_FCB_SIZE
#undef _GLOB_SET_FCB
#undef _GLOB_FCB_DRV

#endif

