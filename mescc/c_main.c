/*	c_main.c

	Mike's Enhanced Small C Compiler for Z80 & CP/M

	Main module.

	Copyright (c) 1999-2015 Miguel I. Garcia Lopez

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

	16 Jan 2001 : Minor changes.
	20 Dec 2003 : Minor changes.
	20 Apr 2004 : Last revision.
	16 Apr 2007 : GPL'd.
	14 Dec 2013 : BUG solved in dofor(): 'continue' jumped to expresion test, instead of increment expr.
	10 Apr 2015 : Removed -A option (argc & argv support in mescc.h - now is managed by macro def. CC_NO_ARGS).
	16 Aug 2015 : Amended some comments and messages.
	26 Oct 2015 : Removed init_files() and cpp_end() calls.
	20 Nov 2015 : Updated, symbol has a new field SY_FLAGS.
	22 Nov 2015 : More on variable and array assignments. Cleaned a bit.
	              Solved bug when an empty function was declared (return was not written).
	26 Nov 2015 : More on array assignments.
	27 Nov 2015 : Modified some text messages.
	29 Nov 2015 : Modified dofor() to support forever loops: for(;;){...}.
	01 Nov 2015 : Emit function labels without ':' -- see newfunc().
	11 Oct 2016 : Changes in doswtch() output to help the optimizer. Extract parser functions to c_parser.c. Documented.
*/

// Globals
// -------

char	glbsymtab[GLB_TABSIZ],	// Global symbol table
	*glbptr;                // Pointer

char	locsymtab[LOC_TABSIZ],  // Local symbol table
	*locptr;                // Pointer

int	wq[WQ_TABSIZ],          // While queue
	*wqptr;                 // Pointer

char	litq[STRBUF_SIZ];       // String pool and pointer
int	litptr;

char	line[LN_SIZ];           // Parsing line buffer
int	lptr;                   // Index

int	letlab,		/* Label letter */
	errstop,	/* Pause on error ? */
	eof,		/* Non zero after final input */
	ctext,		/* Non-zero if C source to be included as comment */
	typout,		/* Output type: 0=program, 1=library, 2=assembler */
	lastst,		/* Type of last executed statement */
	nxtlab,		/* Next label num. */
	litlab,		/* String pool label */
	sptr,		/* Relative SP */
	argstk,		/* Function arg. SP */
	argtop,		/* Function arg. top SP */
	errcnt,		/* Error count */
	errmax,		/* Max. num. of errors */
	locsiz,		/* Size of local variables in a function */
	stksize;	/* Stack size for program */

char	ipfname[15],	/* Input file name */
	opfname[15];	/* Output file name */

/*	C PREPROCESSOR
*/

int	cppinasm,		/* TRUE = #asm activo */
	cppincmt,		/* TRUE = comentario activo */
	cppinign,		/* TRUE = #if FALSE activo */

	cppiflev,		/* Nivel de anidamiento de #if's */
	cppifact;		/* Nivel de #if activo */

int cppincs;			/* # of active includes */
unsigned int cppfps[CPPMAXINC];	/* FPs for active includes - FILE *cppfps[x] */
int cpplines[CPPMAXINC];	/* # of current line for active includes */
char cppfnames[45];		/* Buffer for filenames == CPPMAXINC * 15 */

char cpptmp[LN_SIZ];		/* Datos */
int cpptmpx;			/* Indice */

char cppmac[CPPMACSIZ];		/* Datos */
int cppmacx;			/* Indice */

int cpphash[CPPHASHSIZ];	/* Hash */

/* FILE I/O
*/

/* Values for current input file
*/

FILE *fi_fp;
char fi_name[15];
int fi_line;
int fi_eof;

/* Values for output file
*/

FILE *fo_fp;
char fo_name[15];

/*	MAIN FUNCTION
*/

main(argc,argv)
int argc,argv[];
{
	/* Init. vars. */

	glbptr=GLB_START;
	locptr=LOC_START;
	wqptr=wq;

	/* litptr=sptr=errcnt=eof=lastst=0; ***** already 0 */

	/* Show title */

	title();

	/* No parameters ? */

	if(argc==1)
	{
		help();	return;
	}

	/* Get options from command line */

	getargs(argc-1,&argv[1]);
			
	/* Init. more vars. */

	litlab=getlabl();

	/* Init. file and preprocessor system */

	//init_files();
	cpp_start();

	/* Open files */

	fi_open(ipfname);
	fo_open(opfname);

	/* Header for output file */

	a_start();

	/* Process all input */

	parse();

	/* Write strings and global variables */

	co_nl();

	dmplits();

	dmpglbs();

	/* Summary on screen */

	co_nl();
	co_line("Table    Used  Free");
	co_line("-------- ----- -----");
	co_str("Strings  ");co_dec05(litptr);co_ch(' ');co_dec05(STRBUF_SIZ-litptr);co_nl();
	co_str("Globals  ");co_dec05(glbptr-GLB_START);co_ch(' ');co_dec05(GLB_END-glbptr);co_nl();
	co_str("Macros   ");co_dec05(cppmacx);co_ch(' ');co_dec05(CPPMACSIZ-cppmacx);co_nl();
	co_nl();
	co_str("Errors:     ");co_dec(errcnt);co_nl();
	co_str("Next label: ");co_ch(letlab);co_dec(nxtlab);co_nl();

	/* Summary on file */

/*
	fo_nl();
	comment();fo_str("Errors:     ");fo_dec(errcnt);fo_nl();
	comment();fo_str("Next label: ");fo_ch(letlab);fo_dec(nxtlab);fo_nl();
*/

	/* Footer for output file */

	a_end();

	/* Close files and bye, bye! */

	fo_close();
	fi_close();

	//cpp_end();
}

/*	SET OPTIONS FROM COMMAND LINE / DEFAULT
*/

getargs(argcnt,argstr)
int argcnt,argstr[];
{
	int i;
	char *ptr;

	/* Default options */

	/* ctext=0; *** already 0 */
	errstop=1;
	errmax=50;
	letlab='a';
	/* nxtlab=0; *** already 0 */
	typout=OUT_PRG;
	*opfname=0;
	stksize=512;

	/* Input file name */

	strcpy(ipfname,argstr[0]);	

	/* Get options from command line */

	for(i=1;i < argcnt;++i)
	{
		ptr=strupr(argstr[i]);
	
		if(*ptr++!='-')
			errcmdl(ERCMDLN);

		/* Output file name */

		if((!memcmp("O:",ptr,2)) && *(ptr+2))
			strcpy(opfname,ptr+2);

		/* Output file type */

		else if(!strcmp("PRG",ptr))
			typout=OUT_PRG;
		else if(!strcmp("LIB",ptr))
			typout=OUT_LIB;
		else if(!strcmp("ASM",ptr))
			typout=OUT_ASM;

		/* Write C source as comment ? */

		else if(!strcmp("C+",ptr))
			ctext=1;
		else if(!strcmp("C-",ptr))
			ctext=0;

		/* Pause on errors ? */

		else if(!strcmp("E+",ptr))
			errstop=1;
		else if(!strcmp("E-",ptr))
			errstop=0;

		/* Max. num. of errors to abort */

		else if((!memcmp("E:",ptr,2)) && *(ptr+2))
		{
			if((errmax=atoi(ptr+2))<0)
				errcmdl(ERCMDER);
		}
		
		/* Letter and start number for labels */

		else if((!memcmp("L:",ptr,2)) && *(ptr+2) && *(ptr+3))
		{
			if(!isalpha((letlab=*ptr+2)) || (nxtlab=atoi(*ptr+3))<0)
				errcmdl(ERCMDLB);
		}

		/* Stack size */

		else if((!memcmp("S:",ptr,2)) && *(ptr+2))
		{
			if((stksize=atoi(ptr+2))<512)			
				errcmdl(ERCMDST);
		}

		/* Unknown option */

		else
			errcmdl(ERCMDLN);
	}

	/* Put type to input file name if need */

	if(!strchr(ipfname,'.'))
		strcat(ipfname,".c");

	/* Put name without type if not exist */

	if(!(*opfname))
	{
		strcpy(opfname,ipfname);
		ptr=strchr(opfname,'.');
		*ptr=0;
	}

	/* Put type to output file name if need */

	if(!strchr(opfname,'.'))
		strcat(opfname, typout==OUT_LIB ? ".lib" : ".zsm");
}

/*	TITLE
*/

title()
{
	co_line(VERSION);
	co_nl();
	co_line(COPYRGT);
	co_nl();
}

/*	HELP
*/

help()
{
	co_line("Usage: cc name[.typ] [options]\n");
	co_line("-PRG          Out normal file ---- Type=ZSM");
	co_line("-LIB          Out library file --- Type=LIB");
	co_line("-ASM          Out assembler file - Type=ZSM");
	co_line("-O:name[.typ] File name for output");
	co_line("-Cflag        C source as comments?");
	co_line("-Eflag        Pause on errors?");
	co_line("-E:number     Max. number of errors to abort");
	co_line("-L:letnum     Letter and start number for labels");
	co_line("-S:size       Mininum stack size\n");
	co_line("Default options are: (flag is + for YES, - for NO)\n");
	co_line("\t-PRG -C- -E+ -E:50 -L:A0 -S:512");
	co_line("\tInput file type is .c");
	co_line("\tOutput file name is the same as input");
}

