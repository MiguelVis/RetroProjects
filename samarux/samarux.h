/*	samarux.h

	Header for SamaruX external commands with MESCC (Mike's Enhanced Small C Compiler
	for Z80 & CP/M).

	SamaruX external commands are PRL files, with a header of 16 bytes.

	Copyright (c) 2015-2016 Miguel I. Garcia Lopez

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

	Notes:

	This header must to be included first in the source code.

	Don't include other standard MESCC headers (conio.h, string.h, etc.).
	
	Don't forget to update the version number and the linked symbols (see below),
	and to compile again the external commands, if the SamaruX shell changes.
	
	To update the linked symbols, after Samarux shell compilation:
	
	prntosym sx.prn > sx.sym
	
	and copy / paste the new symbol addresses.
	
	To compile a SamaruX external command (ie: robots.c):
	
	cc robots
	ccopt robots
	zmac robots.zsm --rel
	link robots [op]
	rename robots.prl robots.x

	Revisions:

	31 Mar 2015 : 1st version.
	01 Apr 2015 : Changes in code header.
	09 Apr 2015 : Idem.
	20 Aug 2015 : Added version number to header. Updated linked symbols for SamaruX v2.01.
	22 Aug 2015 : Updated to SamaruX v2.02.
	23 Aug 2015 : Added C declarations of stdin, stdout, stderr.
	24 Aug 2015 : Updated to SamaruX changes.
	04 Sep 2015 : Updated to SamaruX v2.03.
	29 Nov 2015 : Modified FILENAME_MAX value. Added external references to SamaruX symbols.
	24 Jan 2016 : Updated to SamaruX v2.04.
	03 Jun 2016 : Updated to SamaruX v2.05.
*/

#ifndef SAMARUX_H

#define SAMARUX_H

/* HOOKS
   -----
*/

#define exit HookExit

/* SAMARUX CODE HEADER
   -------------------
   16 bytes
*/

#asm
	CSEG

	JP 0

	DEFB 'SamaruX', 0
	
	; Version 2.05
	
	DEFB 2
	DEFB 05

	JP main
#endasm

/* MESCC.H
   -------
*/

#define BYTE	unsigned char
#define WORD	unsigned int
#define BOOL	char
#define NULL	0
#define TRUE	1
#define FALSE	0

#define SIZEOF_CHAR 1 /* [unsigned] char */
#define SIZEOF_INT  2 /* [unsigned] int */
#define SIZEOF_PTR  2 /* pointer */

/* CLOCK.H
   -------
*/

#define CLOCK_LEN 5 /* CP/M clock array lenght in BYTES - ie: unsigned char clk[5] */
#define DATE_LEN  7 /* Date array lenght in WORDS - ie: int dt[7] */

/* CPM.H
   -----
*/

#define BF_GETDRV 25
#define BF_SETDRV 14
#define BF_USER 32
#define BF_DMA 26
#define BF_FIND1ST 17
#define BF_FINDNEXT 18
#define BF_OSVER 12
#define BF_CONST 11
#define BF_FSIZE 35
#define BF_OPEN 15
#define BF_DELETE 19
#define BF_CREATE 22
#define BF_READSEQ 20
#define BF_WRITESEQ 21
#define BF_RENAME 23
#define BF_CLOSE 16
#define BF_ATTRIB 30

#define UX_FCX_SIZ 37
#define UX_FCX_USR 0
#define UX_FCX_DRV 1
#define UX_FCX_RRC 34 /* Random Record Number */

/* FILEIO.H
   --------
*/

#define FILE unsigned char
#define EOF -1

#define FILENAME_MAX 22  /* dusrname:filename.typ + ZERO */

extern FILE *stdin, *stdout, *stderr;

/* STDBOOL.H
   ---------
*/

#define bool  int
#define true  1
#define false 0

/* SAMARUX LINKED SYMBOLS
   ----------------------
*/

#include "sx.h"

extern int sv_cpmver;
extern int sv_drive;
extern int sv_user;

extern char sv_tmp_fn[FILENAME_MAX];

extern int *sv_hist;

extern int *sv_env_name;
extern int *sv_env_value;

extern int *sv_alias_name;
extern int *sv_alias_value;

extern int *sv_dir_path;
extern int *sv_dir_alias;

extern int sv_flg_hist;
extern int sv_flg_buf;
extern int sv_flg_quit;
extern int sv_flg_cpm;
extern int sv_batch;

extern BYTE sv_fcx[UX_FCX_SIZ];
extern BYTE sv_fcx2[UX_FCX_SIZ];

extern BYTE *sv_dma;

extern BYTE *sv_fcxbuf;

extern int sv_cmd_max;

//extern WORD sv_cmd_name[SX_MAX_CMD];
//extern WORD sv_cmd_fun[SX_MAX_CMD];

//extern char *sv_cmd_err;

extern int sv_cmd_exit;

//extern char sv_ed_buf[SX_LINESIZE];

//extern char sv_pipe_in[FILENAME_MAX];
//extern char sv_pipe_out[FILENAME_MAX];

#include "sx_sym.h"

#endif
