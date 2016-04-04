/*	c_defs.c

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Definitions.

	Copyright (c) 1999-2015 Miguel I. Garcia Lopez, FloppySoftware.

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

	17 Jan 2001 : Last revision.
	16 Apr 2007 : GPL'd.
	31 Jul 2014 : String pool resized to 9K (was 6000 bytes before).
	31 Jul 2014 : v1.02
	17 Feb 2015 : v1.03
	10 Apr 2015 : v1.04
	11 May 2015 : v1.05
	14 Aug 2015 : v1.06
	16 Aug 2015 : Removed TURBO C and PCC support.
	16 Aug 2015 : v1.07
	04 Sep 2015 : v1.08
	28 Oct 2015 : v1.09
	20 Nov 2015 : Added SY_FLAGS. Decreased local symbol table.
	22 Nov 2015 : Added ERCNTAS error message.
	27 Nov 2015 : Removed some text defs.
	02 Dec 2015 : v1.10
	08 Jan 2016 : Supported #ifs from 5 to 8.
	04 Apr 2016 : v1.11
*/

/*	OPTIONAL DEFS

	You can define the following:

	C_USEPRINTF	If you want use printf/fprintf/sprintf in
			console/file/mem output functions.
*/

/* #define C_USEPRINTF */

/*	VERSION
*/

#define VERSION	"Mike's Enhanced Small C Compiler v1.11 - 04 Apr 2015"
#define COPYRGT "(c) 1999-2016 FloppySoftware"

/*	COMMON
*/

#define ERCONST	"Must be constant"

#define ERARRSZ "Illegal array size"
#define ERARRPT "Illegal *[]"
#define ERCNTAS "Can't assign"
#define ERARGNB "Illegal arguments number"
#define ERARGNM	"Need argument name"
#define ERFUNDE	"Illegal function declaration"
#define ERSYMNM	"Illegal symbol name"
#define ERALDEF	"Already defined"
#define EROFGLB	"Global symbol table full"
#define EROFLOC	"Local symbol table full"
#define ERTMWHI	"Too many active while's"
#define ERNOWHI	"No active while's"
#define EREXWHI	"Need a while"
#define ERLTLNG	"Line too long"
#define ERTMCAS	"Too many active case's"

#define OUT_PRG 0
#define OUT_LIB 1
#define OUT_ASM 2

/*	STRING POOL
*/

#define STRBUF_SIZ 9216

/*	INPUT LINE
*/

#define LN_SIZ	256
#define LN_MAX	255

/*	SYMBOL TABLE FORMAT
*/

#define SY_NAME		0
#define SY_IDENT	12
#define SY_TYPE		13
#define SY_STORAGE	14
#define SY_FLAGS	15
#define SY_OFFSET	16 /* Word */

#define SYMSIZE		18

/*	SYMBOL NAME SIZE
*/

#define NAME_SIZ	12
#define NAME_MAX	11

/*	IDENT
*/

#define ID_VAR	1
#define ID_ARR	2
#define ID_PTR	3
#define ID_FUN	4

/*	TYPE
*/

#define TY_CHAR		1
#define TY_INT		2
#define TY_UCHAR	5
#define TY_UINT		6

/*	STORAGE
*/

#define ST_EXTERN	1
#define ST_STATIK	2
#define ST_STKLOC	3

/*	FLAGS (bit values: 1, 2, 4, 8, 16, 32, 64, 128)
*/

#define FL_VALUE	1 /* It was initialized */
#define FL_STRBF	2 /* Value has an string buffer offset */

/*	USAGE
*/

#define HLP1	"Usage: cc name[.typ] [options]\n"
#define HLP2	"-PRG          Out normal file ---- Type=ZSM"
#define HLP3	"-LIB          Out library file --- Type=LIB"
#define HLP4	"-ASM          Out assembler file - Type=ZSM"
#define HLP5	"-O:name[.typ] File name for output"
#define HLP6	"-Cflag        C source as comments?"
#define HLP7	"-Eflag        Pause on errors?"
#define HLP9	"-E:number     Max. number of errors to abort"
#define HLP10	"-L:letnum     Letter and start number for labels"
#define HLP11	"-S:size       Mininum stack size\n"
#define HLP12	"Default options are: (flag is + for YES, - for NO)\n"
#define HLP13   "\t-PRG -C- -E+ -E:50 -L:A0 -S:512"
#define HLP14   "\tInput file type is .c"
#define HLP15	"\tOutput file name is the same as input"

/*	ERRORS IN COMMAND LINE
*/

#define ERCMDLN	"Illegal option"
#define ERCMDER	"Illegal max. errors number"
#define ERCMDLB	"Illegal label"
#define ERCMDST	"Illegal stack size"

/*	MESSAGES
*/

/* **********************************************
#define SMRYLAB	"Next label: "
#define SMRYERR	"Errors:     "

#define SMLIN1	"Table    Used  Free"
#define SMLIN2	"-------- ----- -----"
#define SMLITS	"Strings  "
#define SMGLBS	"Globals  "
#define SMMACS	"Macros   "
	
#define DUMPLIT	"Writing strings..."
#define DUMPGLO	"Writing globals..."
#define COMPFUN	"Function: "
************************************************/

/*	SYMBOL TABLE
*/

#define GLB_NUM		450
#define GLB_TABSIZ	8100		/* GLB_NUM * SYMSIZE */
#define GLB_START	glbsymtab
#define GLB_END		GLB_START+GLB_TABSIZ

#define LOC_NUM		32
#define LOC_TABSIZ	576		/* LOC_NUM * SYMSIZE */
#define LOC_START	locsymtab
#define LOC_END		LOC_START+LOC_TABSIZ

/*	STATEMENT TYPES
*/

#define S_IF		1
#define S_WHILE		2
#define S_FOR		3
#define S_SWITCH	4
#define S_RETURN	5
#define S_BREAK		6
#define S_CONT		7
#define S_EXP		8

/*	SWITCH
*/

#define SWITCH_MAX 254

/*	WHILE QUEUE PARAMETERS
*/

#define WQ_TABSIZ	120
#define WQ_SIZ		7
#define WQ_MAX		wq+WQ_TABSIZ-WQ_SIZ
#define WQ_SYM		0
#define WQ_SP		1
#define WQ_LOOP		2
#define WQ_LAB		3
#define WQ_LAB2		4
#define WQ_END		5
#define WQ_BODY		6

/*	PREPROCESSOR
*/

#define CPPMACSIZ 4000	/* Macro table size */
#define CPPMAXINC 3	/* Maximum # of included files */
#define CPPMAXIFS 8	/* Maximo numero de #if's anidados */
#define CPPHASHSIZ 53	/* Hash (A...Z a...z _) */

#define ERASMWE "#asm without #endasm"
#define EREASWA "#endasm without #asm"
#define ERMACAD "Macro already defined"
#define ERMACUD "Macro not exist"
#define ERTMIFS "Too many active #if's"
#define ERELSEW "#else without #if"
#define ERENDIF "#endif without #if"
#define ERBADCM "Illegal # command"
#define EROFMAC	"Macro table full"
#define ERTMINC	"Too many active includes"

/*	ERROR MESSAGES
*/

#define ERERROR	"Error! "
#define ERFILE	"File: "
#define ERLINE	"Line: "
/**********************************************************
#define ERTMERR	"Too many errors, compilation aborted."

#define ERNEED	"Need: ?"
***********************************************************/
/*	EXPRESSION PARSER
*/

#define ERSIZOF "Illegal use of sizeof"
#define EROFSTR	"String table full"
#define ERILADR	"Illegal adress"
#define ERCTSUB	"Can't index"
#define ERINVEX	"Illegal expresion"
#define ERMBLVL	"Must be lvalue"

/*	FILE I/O
*/

#define EROPEN	"Can't open"
#define ERWRITE	"Can't write"
#define ERCLOSE	"Can't close"
