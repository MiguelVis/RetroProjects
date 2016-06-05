/*	sx_cp.c

	CP for Samarux.

	Copies files.

	Copyright (c) 2007 - 2015 Miguel I. Garcia Lopez.

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

	Usage:

	cp [-fi] file_src file_dst
	cp [-fi] file_src ... directory

	Options:

	-f Force:       Delete destination if already exists
	-i Interactive: Asks to user to delete destination if already exists. It has prevalence over -f option.
	-v Verbose

	Examples:

	cp myfile.txt myfile.bak
	cp -i cat.c ls.c head.c B01:

	Changes:

	19 Dec 2014 : 1.00 : 1st version.
	24 Dec 2014 : 1.01 : Minor changes.
	05 Jan 2015 : 1.02 : Added verbose option.
	06 Jan 2015 : 1.03 : Added source and destination files existence checking. Copy procedure.
	08 Jan 2015 : 1.04 : Adapted to changes in shell.
	05 Jun 2016 : 1.05 : Added support for builtin / external.
*/

/* Built-in or external
   --------------------
*/
#ifdef SX_SAMARUX

#define SX_CP

#else

#include "samarux.h"

#define CpMain main

#endif

#define CP_F_F 1
#define CP_F_I 2
#define CP_F_V 4

CpMain(argc, argv)
int argc, argv[];
{
	int i, src_ix, src_hm;
	char *p, *src, *dst, *fcx_src, *fcx_dst, *buf;
	int flags, files, max;

	/* Default options */

	flags = 0;

	fcx_src = sv_fcx;
	fcx_dst = sv_fcx_two;

	/* Check options */

	for(i = 1; i < argc; ++i)
	{
		p = argv[i];

		if(*p == '-')
		{
			while(*++p)
			{
				switch(*p)
				{
					case 'f' : flags |= CP_F_F; break;
					case 'i' : flags |= CP_F_I; break;
					case 'v' : flags |= CP_F_V; break;
					default  : return ErrorOpt();
				}
			}
		}
		else 
			break;
	}

	/* i == next arg */

	/* We need 2 args or more */

	if(i + 2 > argc)
		return ErrorPars();

	/* Set source */

	src_ix = i;            /* Argv index */
	src_hm = argc - i - 1; /* How many sources */

	/* Set destination */

	dst = argv[argc - 1];

	if(CpMakeFcx(dst, fcx_dst))
		return 1;

	/* Destination is a file */

	if(!FnIsDir(dst))
	{
		/* Check cp src_file dst_file */

		/* Only two unambiguous file names are allowed */

		src = argv[src_ix];

		if(src_hm > 1 || (FnIsDir(src)))
			return ErrorIll();

		if(FnIsAmb(src) || FnIsAmb(dst))
			return ErrorAmb();

		/* Make source FCX */

		if(CpMakeFcx(src, fcx_src))
			return 1;

		/* Check if source exists */

		bdos_hl(BF_DMA, sv_dma);

		if(bdos_fcx_a(BF_FIND1ST, fcx_src) == 0xFF)
			return ErrorNoFile();

		/* Copy file */

		return CpCopy(fcx_src, fcx_dst, flags);
	}

	/* Destination is a directory */

	files = 0;
	max = SX_MAX_FCX;
	buf = sv_fcxbuf;

	for(i = 0; i < src_hm; ++i)
	{
		src = argv[src_ix + i];

		if(CpMakeFcx(src, fcx_src))
			return 1;

		if((files = RtExpandFcx(fcx_src, buf, max)) == -1)
			return ErrorTooMany();

		if(!files)
			return ErrorNoFile();

		max -= files;
		buf += files * 13;
	}

	files = SX_MAX_FCX - max;

	buf = sv_fcxbuf;

	for(i = 0; i < files; ++i)
	{
		memcpy(fcx_src, buf, 13); memset(fcx_src + 13, 0, UX_FCX_SIZ - 13);
		memcpy(fcx_dst + 2, fcx_src + 2, 11);

		if(CpCopy(fcx_src, fcx_dst, flags))
			return 1;

		buf += 13;
	}

	return 0;
}

/* Unambiguous FCXs. Source exists. May be source already exists. */

CpCopy(fcx_src, fcx_dst, flags)
char *fcx_src, *fcx_dst; int flags;
{
	int force, reply, code; char fcx_tmp[UX_FCX_SIZ], *tmp;

	/* Check is they are the same file */

	if(!memcmp(fcx_src, fcx_dst, 13))
		return Error("Source and destination files are the same");

	/* Check if destination file already exists */

	bdos_hl(BF_DMA, sv_dma);

	force = 0;

	if(bdos_fcx_a(BF_FIND1ST, fcx_dst) != 0xFF)
	{
		if(flags & CP_F_V || flags & CP_F_I || !(flags & CP_F_F))
		{
			RtPrintFcx(fcx_dst); putstr(" already exists. ");
		}

		if(flags & CP_F_F)
			++force;

		if(flags & CP_F_I)
		{
			putstr("Remove it (y/n)? "); reply = getch(); putchar('\n');

			if(reply != 'y')
			{
				puts("File not copied."); return 0;
			}

			++force;
		}

		if(!force)
		{
			puts("Aborted."); return 1;
		}

		if(flags & CP_F_V)
			putchar('\n');
	}

	/* Copy file */

	if(flags & CP_F_V)
	{
		printf("Copying "); RtPrintFcx(fcx_src);
		printf(" to "); RtPrintFcx(fcx_dst);
		putchar('\n');
	}

	tmp = "CP.$$$";

	setfcx(tmp, fcx_tmp);

	fcx_tmp[UX_FCX_DRV] = fcx_dst[UX_FCX_DRV];
	fcx_tmp[UX_FCX_USR] = fcx_dst[UX_FCX_USR];

	bdos_fcx_a(BF_DELETE, fcx_tmp);

	if(bdos_fcx_a(BF_CREATE, fcx_tmp) == 0xFF)
		return ErrorCreate();

	if(bdos_fcx_a(BF_OPEN, fcx_src) == 0xFF)
	{
		bdos_fcx_a(BF_CLOSE, fcx_tmp); bdos_fcx_a(BF_DELETE, fcx_tmp);

		return ErrorOpen();
	}

	while(1)
	{
		if(!(code = bdos_fcx_a(BF_READSEQ, fcx_src)))
		{
			if((code = bdos_fcx_a(BF_WRITESEQ, fcx_tmp)))
			{
				ErrorWrite(); break;
			}

			continue;
		}

		if(code == 1)
			code = 0;
		else
			ErrorRead();

		break;
	}

	if(code)
	{
		bdos_fcx_a(BF_CLOSE, fcx_src); bdos_fcx_a(BF_CLOSE, fcx_tmp); bdos_fcx_a(BF_DELETE, fcx_tmp);

		return 1;
	}

	code = bdos_fcx_a(BF_CLOSE, fcx_src);
	code |= bdos_fcx_a(BF_CLOSE, fcx_tmp);

	if(code)
	{
		bdos_fcx_a(BF_DELETE, fcx_tmp);
		return ErrorClose();
	}

	if(force)
	{
		if(bdos_fcx_a(BF_DELETE, fcx_dst) == 0xFF) /* FIXME: File can be R/O */
		{
			bdos_fcx_a(BF_DELETE, fcx_tmp);
			return ErrorDelete();
		}
	}

	memcpy(fcx_tmp + 17, fcx_dst + 1, 16);

	if(bdos_fcx_a(BF_RENAME, fcx_tmp) == 0xFF)
	{
		bdos_fcx_a(BF_DELETE, fcx_tmp);
		return ErrorRename();
	}

	return 0;
}

CpMakeFcx(fn, fcx)
char *fn, *fcx;
{
	if(MakeFcx(fn, fcx))
		return ErrorFName();

	if(!fcx[UX_FCX_DRV])
		fcx[UX_FCX_DRV] = sv_drive + 1;

	if(!fcx[UX_FCX_USR])
		fcx[UX_FCX_USR] = sv_user + 1;

	return 0;
}

#undef CP_F_F
#undef CP_F_I
#undef CP_F_V



